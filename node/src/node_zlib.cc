#include "node.h"
#include "node_buffer.h"

#include "async-wrap.h"
#include "async-wrap-inl.h"
#include "env.h"
#include "env-inl.h"
#include "util.h"
#include "util-inl.h"

#include "v8.h"
#include "zlib.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

namespace node {

using v8::Array;
using v8::Context;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Integer;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

enum node_zlib_mode {
  NONE,
  DEFLATE,
  INFLATE,
  GZIP,
  GUNZIP,
  DEFLATERAW,
  INFLATERAW,
  UNZIP
};


void InitZlib(v8::Local<v8::Object> target);


/**
 * Deflate/Inflate
 */
class ZCtx : public AsyncWrap {
 public:

  ZCtx(Environment* env, Local<Object> wrap, node_zlib_mode mode)
      : AsyncWrap(env, wrap, AsyncWrap::PROVIDER_ZLIB),
        chunk_size_(0),
        dictionary_(nullptr),
        dictionary_len_(0),
        err_(0),
        flush_(0),
        init_done_(false),
        level_(0),
        memLevel_(0),
        mode_(mode),
        strategy_(0),
        windowBits_(0),
        write_in_progress_(false),
        pending_close_(false),
        refs_(0) {
    MakeWeak<ZCtx>(this);
  }


  ~ZCtx() override {
    CHECK_EQ(false, write_in_progress_ && "write in progress");
    Close();
  }

  void Close() {
    if (write_in_progress_) {
      pending_close_ = true;
      return;
    }

    pending_close_ = false;
    CHECK(init_done_ && "close before init");
    CHECK_LE(mode_, UNZIP);

    if (mode_ == DEFLATE || mode_ == GZIP || mode_ == DEFLATERAW) {
      (void)deflateEnd(&strm_);
      int64_t change_in_bytes = -static_cast<int64_t>(kDeflateContextSize);
      env()->isolate()->AdjustAmountOfExternalAllocatedMemory(change_in_bytes);
    } else if (mode_ == INFLATE || mode_ == GUNZIP || mode_ == INFLATERAW ||
               mode_ == UNZIP) {
      (void)inflateEnd(&strm_);
      int64_t change_in_bytes = -static_cast<int64_t>(kInflateContextSize);
      env()->isolate()->AdjustAmountOfExternalAllocatedMemory(change_in_bytes);
    }
    mode_ = NONE;

    if (dictionary_ != nullptr) {
      delete[] dictionary_;
      dictionary_ = nullptr;
    }
  }


  static void Close(const FunctionCallbackInfo<Value>& args) {
    ZCtx* ctx = Unwrap<ZCtx>(args.Holder());
    ctx->Close();
  }


  // write(flush, in, in_off, in_len, out, out_off, out_len)
  template <bool async>
  static void Write(const FunctionCallbackInfo<Value>& args) {
    CHECK_EQ(args.Length(), 7);

    ZCtx* ctx = Unwrap<ZCtx>(args.Holder());
    CHECK(ctx->init_done_ && "write before init");
    CHECK(ctx->mode_ != NONE && "already finalized");

    CHECK_EQ(false, ctx->write_in_progress_ && "write already in progress");
    CHECK_EQ(false, ctx->pending_close_ && "close is pending");
    ctx->write_in_progress_ = true;
    ctx->Ref();

    CHECK_EQ(false, args[0]->IsUndefined() && "must provide flush value");

    unsigned int flush = args[0]->Uint32Value();

    if (flush != Z_NO_FLUSH &&
        flush != Z_PARTIAL_FLUSH &&
        flush != Z_SYNC_FLUSH &&
        flush != Z_FULL_FLUSH &&
        flush != Z_FINISH &&
        flush != Z_BLOCK) {
      CHECK(0 && "Invalid flush value");
    }

    Bytef *in;
    Bytef *out;
    size_t in_off, in_len, out_off, out_len;
    Environment* env = ctx->env();

    if (args[1]->IsNull()) {
      // just a flush
      Bytef nada[1] = { 0 };
      in = nada;
      in_len = 0;
      in_off = 0;
    } else {
      CHECK(Buffer::HasInstance(args[1]));
      Local<Object> in_buf;
      in_buf = args[1]->ToObject(env->isolate());
      in_off = args[2]->Uint32Value();
      in_len = args[3]->Uint32Value();

      CHECK(Buffer::IsWithinBounds(in_off, in_len, Buffer::Length(in_buf)));
      in = reinterpret_cast<Bytef *>(Buffer::Data(in_buf) + in_off);
    }

    CHECK(Buffer::HasInstance(args[4]));
    Local<Object> out_buf = args[4]->ToObject(env->isolate());
    out_off = args[5]->Uint32Value();
    out_len = args[6]->Uint32Value();
    CHECK(Buffer::IsWithinBounds(out_off, out_len, Buffer::Length(out_buf)));
    out = reinterpret_cast<Bytef *>(Buffer::Data(out_buf) + out_off);

    // build up the work request
    uv_work_t* work_req = &(ctx->work_req_);

    ctx->strm_.avail_in = in_len;
    ctx->strm_.next_in = in;
    ctx->strm_.avail_out = out_len;
    ctx->strm_.next_out = out;
    ctx->flush_ = flush;

    // set this so that later on, I can easily tell how much was written.
    ctx->chunk_size_ = out_len;

    if (!async) {
      // sync version
      ctx->env()->PrintSyncTrace();
      Process(work_req);
      if (CheckError(ctx))
        AfterSync(ctx, args);
      return;
    }

    // async version
    uv_queue_work(ctx->env()->event_loop(),
                  work_req,
                  ZCtx::Process,
                  ZCtx::After);

    args.GetReturnValue().Set(ctx->object());
  }


  static void AfterSync(ZCtx* ctx, const FunctionCallbackInfo<Value>& args) {
    Environment* env = ctx->env();
    Local<Integer> avail_out = Integer::New(env->isolate(),
                                            ctx->strm_.avail_out);
    Local<Integer> avail_in = Integer::New(env->isolate(),
                                           ctx->strm_.avail_in);

    ctx->write_in_progress_ = false;

    Local<Array> result = Array::New(env->isolate(), 2);
    result->Set(0, avail_in);
    result->Set(1, avail_out);
    args.GetReturnValue().Set(result);

    ctx->Unref();
  }


  // thread pool!
  // This function may be called multiple times on the uv_work pool
  // for a single write() call, until all of the input bytes have
  // been consumed.
  static void Process(uv_work_t* work_req) {
    ZCtx *ctx = ContainerOf(&ZCtx::work_req_, work_req);

    // If the avail_out is left at 0, then it means that it ran out
    // of room.  If there was avail_out left over, then it means
    // that all of the input was consumed.
    switch (ctx->mode_) {
      case DEFLATE:
      case GZIP:
      case DEFLATERAW:
        ctx->err_ = deflate(&ctx->strm_, ctx->flush_);
        break;
      case UNZIP:
      case INFLATE:
      case GUNZIP:
      case INFLATERAW:
        ctx->err_ = inflate(&ctx->strm_, ctx->flush_);

        // If data was encoded with dictionary
        if (ctx->err_ == Z_NEED_DICT && ctx->dictionary_ != nullptr) {
          // Load it
          ctx->err_ = inflateSetDictionary(&ctx->strm_,
                                           ctx->dictionary_,
                                           ctx->dictionary_len_);
          if (ctx->err_ == Z_OK) {
            // And try to decode again
            ctx->err_ = inflate(&ctx->strm_, ctx->flush_);
          } else if (ctx->err_ == Z_DATA_ERROR) {
            // Both inflateSetDictionary() and inflate() return Z_DATA_ERROR.
            // Make it possible for After() to tell a bad dictionary from bad
            // input.
            ctx->err_ = Z_NEED_DICT;
          }
        }
        break;
      default:
        CHECK(0 && "wtf?");
    }

    // pass any errors back to the main thread to deal with.

    // now After will emit the output, and
    // either schedule another call to Process,
    // or shift the queue and call Process.
  }


  static bool CheckError(ZCtx* ctx) {
    // Acceptable error states depend on the type of zlib stream.
    switch (ctx->err_) {
    case Z_OK:
    case Z_STREAM_END:
    case Z_BUF_ERROR:
      // normal statuses, not fatal
      break;
    case Z_NEED_DICT:
      if (ctx->dictionary_ == nullptr)
        ZCtx::Error(ctx, "Missing dictionary");
      else
        ZCtx::Error(ctx, "Bad dictionary");
      return false;
    default:
      // something else.
      ZCtx::Error(ctx, "Zlib error");
      return false;
    }

    return true;
  }


  // v8 land!
  static void After(uv_work_t* work_req, int status) {
    CHECK_EQ(status, 0);

    ZCtx* ctx = ContainerOf(&ZCtx::work_req_, work_req);
    Environment* env = ctx->env();

    HandleScope handle_scope(env->isolate());
    Context::Scope context_scope(env->context());

    if (!CheckError(ctx))
      return;

    Local<Integer> avail_out = Integer::New(env->isolate(),
                                            ctx->strm_.avail_out);
    Local<Integer> avail_in = Integer::New(env->isolate(),
                                           ctx->strm_.avail_in);

    ctx->write_in_progress_ = false;

    // call the write() cb
    Local<Value> args[2] = { avail_in, avail_out };
    ctx->MakeCallback(env->callback_string(), ARRAY_SIZE(args), args);

    ctx->Unref();
    if (ctx->pending_close_)
      ctx->Close();
  }

  static void Error(ZCtx* ctx, const char* message) {
    Environment* env = ctx->env();

    // If you hit this assertion, you forgot to enter the v8::Context first.
    CHECK_EQ(env->context(), env->isolate()->GetCurrentContext());

    if (ctx->strm_.msg != nullptr) {
      message = ctx->strm_.msg;
    }

    HandleScope scope(env->isolate());
    Local<Value> args[2] = {
      OneByteString(env->isolate(), message),
      Number::New(env->isolate(), ctx->err_)
    };
    ctx->MakeCallback(env->onerror_string(), ARRAY_SIZE(args), args);

    // no hope of rescue.
    if (ctx->write_in_progress_)
      ctx->Unref();
    ctx->write_in_progress_ = false;
    if (ctx->pending_close_)
      ctx->Close();
  }

  static void New(const FunctionCallbackInfo<Value>& args) {
    Environment* env = Environment::GetCurrent(args);

    if (args.Length() < 1 || !args[0]->IsInt32()) {
      return env->ThrowTypeError("Bad argument");
    }
    node_zlib_mode mode = static_cast<node_zlib_mode>(args[0]->Int32Value());

    if (mode < DEFLATE || mode > UNZIP) {
      return env->ThrowTypeError("Bad argument");
    }

    new ZCtx(env, args.This(), mode);
  }

  // just pull the ints out of the args and call the other Init
  static void Init(const FunctionCallbackInfo<Value>& args) {
    CHECK((args.Length() == 4 || args.Length() == 5) &&
           "init(windowBits, level, memLevel, strategy, [dictionary])");

    ZCtx* ctx = Unwrap<ZCtx>(args.Holder());

    int windowBits = args[0]->Uint32Value();
    CHECK((windowBits >= 8 && windowBits <= 15) && "invalid windowBits");

    int level = args[1]->Int32Value();
    CHECK((level >= -1 && level <= 9) && "invalid compression level");

    int memLevel = args[2]->Uint32Value();
    CHECK((memLevel >= 1 && memLevel <= 9) && "invalid memlevel");

    int strategy = args[3]->Uint32Value();
    CHECK((strategy == Z_FILTERED ||
            strategy == Z_HUFFMAN_ONLY ||
            strategy == Z_RLE ||
            strategy == Z_FIXED ||
            strategy == Z_DEFAULT_STRATEGY) && "invalid strategy");

    char* dictionary = nullptr;
    size_t dictionary_len = 0;
    if (args.Length() >= 5 && Buffer::HasInstance(args[4])) {
      Local<Object> dictionary_ = args[4]->ToObject(args.GetIsolate());

      dictionary_len = Buffer::Length(dictionary_);
      dictionary = new char[dictionary_len];

      memcpy(dictionary, Buffer::Data(dictionary_), dictionary_len);
    }

    Init(ctx, level, windowBits, memLevel, strategy,
         dictionary, dictionary_len);
    SetDictionary(ctx);
  }

  static void Params(const FunctionCallbackInfo<Value>& args) {
    CHECK(args.Length() == 2 && "params(level, strategy)");
    ZCtx* ctx = Unwrap<ZCtx>(args.Holder());
    Params(ctx, args[0]->Int32Value(), args[1]->Int32Value());
  }

  static void Reset(const FunctionCallbackInfo<Value> &args) {
    ZCtx* ctx = Unwrap<ZCtx>(args.Holder());
    Reset(ctx);
    SetDictionary(ctx);
  }

  static void Init(ZCtx *ctx, int level, int windowBits, int memLevel,
                   int strategy, char* dictionary, size_t dictionary_len) {
    ctx->level_ = level;
    ctx->windowBits_ = windowBits;
    ctx->memLevel_ = memLevel;
    ctx->strategy_ = strategy;

    ctx->strm_.zalloc = Z_NULL;
    ctx->strm_.zfree = Z_NULL;
    ctx->strm_.opaque = Z_NULL;

    ctx->flush_ = Z_NO_FLUSH;

    ctx->err_ = Z_OK;

    if (ctx->mode_ == GZIP || ctx->mode_ == GUNZIP) {
      ctx->windowBits_ += 16;
    }

    if (ctx->mode_ == UNZIP) {
      ctx->windowBits_ += 32;
    }

    if (ctx->mode_ == DEFLATERAW || ctx->mode_ == INFLATERAW) {
      ctx->windowBits_ *= -1;
    }

    switch (ctx->mode_) {
      case DEFLATE:
      case GZIP:
      case DEFLATERAW:
        ctx->err_ = deflateInit2(&ctx->strm_,
                                 ctx->level_,
                                 Z_DEFLATED,
                                 ctx->windowBits_,
                                 ctx->memLevel_,
                                 ctx->strategy_);
        ctx->env()->isolate()
            ->AdjustAmountOfExternalAllocatedMemory(kDeflateContextSize);
        break;
      case INFLATE:
      case GUNZIP:
      case INFLATERAW:
      case UNZIP:
        ctx->err_ = inflateInit2(&ctx->strm_, ctx->windowBits_);
        ctx->env()->isolate()
            ->AdjustAmountOfExternalAllocatedMemory(kInflateContextSize);
        break;
      default:
        CHECK(0 && "wtf?");
    }

    if (ctx->err_ != Z_OK) {
      ZCtx::Error(ctx, "Init error");
    }


    ctx->dictionary_ = reinterpret_cast<Bytef *>(dictionary);
    ctx->dictionary_len_ = dictionary_len;

    ctx->write_in_progress_ = false;
    ctx->init_done_ = true;
  }

  static void SetDictionary(ZCtx* ctx) {
    if (ctx->dictionary_ == nullptr)
      return;

    ctx->err_ = Z_OK;

    switch (ctx->mode_) {
      case DEFLATE:
      case DEFLATERAW:
        ctx->err_ = deflateSetDictionary(&ctx->strm_,
                                         ctx->dictionary_,
                                         ctx->dictionary_len_);
        break;
      default:
        break;
    }

    if (ctx->err_ != Z_OK) {
      ZCtx::Error(ctx, "Failed to set dictionary");
    }
  }

  static void Params(ZCtx* ctx, int level, int strategy) {
    ctx->err_ = Z_OK;

    switch (ctx->mode_) {
      case DEFLATE:
      case DEFLATERAW:
        ctx->err_ = deflateParams(&ctx->strm_, level, strategy);
        break;
      default:
        break;
    }

    if (ctx->err_ != Z_OK && ctx->err_ != Z_BUF_ERROR) {
      ZCtx::Error(ctx, "Failed to set parameters");
    }
  }

  static void Reset(ZCtx* ctx) {
    ctx->err_ = Z_OK;

    switch (ctx->mode_) {
      case DEFLATE:
      case DEFLATERAW:
        ctx->err_ = deflateReset(&ctx->strm_);
        break;
      case INFLATE:
      case INFLATERAW:
        ctx->err_ = inflateReset(&ctx->strm_);
        break;
      default:
        break;
    }

    if (ctx->err_ != Z_OK) {
      ZCtx::Error(ctx, "Failed to reset stream");
    }
  }

  size_t self_size() const override { return sizeof(*this); }

 private:
  void Ref() {
    if (++refs_ == 1) {
      ClearWeak();
    }
  }

  void Unref() {
    CHECK_GT(refs_, 0);
    if (--refs_ == 0) {
      MakeWeak<ZCtx>(this);
    }
  }

  static const int kDeflateContextSize = 16384;  // approximate
  static const int kInflateContextSize = 10240;  // approximate

  int chunk_size_;
  Bytef* dictionary_;
  size_t dictionary_len_;
  int err_;
  int flush_;
  bool init_done_;
  int level_;
  int memLevel_;
  node_zlib_mode mode_;
  int strategy_;
  z_stream strm_;
  int windowBits_;
  uv_work_t work_req_;
  bool write_in_progress_;
  bool pending_close_;
  unsigned int refs_;
};


void InitZlib(Local<Object> target,
              Local<Value> unused,
              Local<Context> context,
              void* priv) {
  Environment* env = Environment::GetCurrent(context);
  Local<FunctionTemplate> z = env->NewFunctionTemplate(ZCtx::New);

  z->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(z, "write", ZCtx::Write<true>);
  env->SetProtoMethod(z, "writeSync", ZCtx::Write<false>);
  env->SetProtoMethod(z, "init", ZCtx::Init);
  env->SetProtoMethod(z, "close", ZCtx::Close);
  env->SetProtoMethod(z, "params", ZCtx::Params);
  env->SetProtoMethod(z, "reset", ZCtx::Reset);

  z->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(), "Zlib"));
  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "Zlib"), z->GetFunction());

  // valid flush values.
  NODE_DEFINE_CONSTANT(target, Z_NO_FLUSH);
  NODE_DEFINE_CONSTANT(target, Z_PARTIAL_FLUSH);
  NODE_DEFINE_CONSTANT(target, Z_SYNC_FLUSH);
  NODE_DEFINE_CONSTANT(target, Z_FULL_FLUSH);
  NODE_DEFINE_CONSTANT(target, Z_FINISH);
  NODE_DEFINE_CONSTANT(target, Z_BLOCK);

  // return/error codes
  NODE_DEFINE_CONSTANT(target, Z_OK);
  NODE_DEFINE_CONSTANT(target, Z_STREAM_END);
  NODE_DEFINE_CONSTANT(target, Z_NEED_DICT);
  NODE_DEFINE_CONSTANT(target, Z_ERRNO);
  NODE_DEFINE_CONSTANT(target, Z_STREAM_ERROR);
  NODE_DEFINE_CONSTANT(target, Z_DATA_ERROR);
  NODE_DEFINE_CONSTANT(target, Z_MEM_ERROR);
  NODE_DEFINE_CONSTANT(target, Z_BUF_ERROR);
  NODE_DEFINE_CONSTANT(target, Z_VERSION_ERROR);

  NODE_DEFINE_CONSTANT(target, Z_NO_COMPRESSION);
  NODE_DEFINE_CONSTANT(target, Z_BEST_SPEED);
  NODE_DEFINE_CONSTANT(target, Z_BEST_COMPRESSION);
  NODE_DEFINE_CONSTANT(target, Z_DEFAULT_COMPRESSION);
  NODE_DEFINE_CONSTANT(target, Z_FILTERED);
  NODE_DEFINE_CONSTANT(target, Z_HUFFMAN_ONLY);
  NODE_DEFINE_CONSTANT(target, Z_RLE);
  NODE_DEFINE_CONSTANT(target, Z_FIXED);
  NODE_DEFINE_CONSTANT(target, Z_DEFAULT_STRATEGY);
  NODE_DEFINE_CONSTANT(target, ZLIB_VERNUM);

  NODE_DEFINE_CONSTANT(target, DEFLATE);
  NODE_DEFINE_CONSTANT(target, INFLATE);
  NODE_DEFINE_CONSTANT(target, GZIP);
  NODE_DEFINE_CONSTANT(target, GUNZIP);
  NODE_DEFINE_CONSTANT(target, DEFLATERAW);
  NODE_DEFINE_CONSTANT(target, INFLATERAW);
  NODE_DEFINE_CONSTANT(target, UNZIP);

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "ZLIB_VERSION"),
              FIXED_ONE_BYTE_STRING(env->isolate(), ZLIB_VERSION));
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(zlib, node::InitZlib)
