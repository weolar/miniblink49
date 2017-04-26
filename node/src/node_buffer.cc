#include "node.h"
#include "node_buffer.h"

#include "env.h"
#include "env-inl.h"
#include "string_bytes.h"
#include "string_search.h"
#include "util.h"
#include "util-inl.h"
#include "v8-profiler.h"
#include "v8.h"

#include <string.h>
#include <limits.h>

#define BUFFER_ID 0xB0E4

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define THROW_AND_RETURN_IF_OOB(r)                                          \
  do {                                                                      \
    if (!(r)) return env->ThrowRangeError("out of range index");            \
  } while (0)

#define THROW_AND_RETURN_UNLESS_BUFFER(env, obj)                            \
  do {                                                                      \
    if (!HasInstance(obj))                                                  \
      return env->ThrowTypeError("argument should be a Buffer");            \
  } while (0)

#define SPREAD_ARG(val, name)                                                 \
  CHECK((val)->IsUint8Array());                                               \
  Local<Uint8Array> name = (val).As<Uint8Array>();                            \
  ArrayBuffer::Contents name##_c = name->Buffer()->GetContents();             \
  const size_t name##_offset = name->ByteOffset();                            \
  const size_t name##_length = name->ByteLength();                            \
  char* const name##_data =                                                   \
      static_cast<char*>(name##_c.Data()) + name##_offset;                    \
  if (name##_length > 0)                                                      \
    CHECK_NE(name##_data, nullptr);

#define SLICE_START_END(start_arg, end_arg, end_max)                        \
  size_t start;                                                             \
  size_t end;                                                               \
  THROW_AND_RETURN_IF_OOB(ParseArrayIndex(start_arg, 0, &start));           \
  THROW_AND_RETURN_IF_OOB(ParseArrayIndex(end_arg, end_max, &end));         \
  if (end < start) end = start;                                             \
  THROW_AND_RETURN_IF_OOB(end <= end_max);                                  \
  size_t length = end - start;

#define BUFFER_MALLOC(length)                                               \
  zero_fill_all_buffers ? node::Calloc(length, 1) : node::Malloc(length)

namespace node {

// if true, all Buffer and SlowBuffer instances will automatically zero-fill
bool zero_fill_all_buffers = false;

namespace Buffer {

using v8::ArrayBuffer;
using v8::ArrayBufferCreationMode;
using v8::Context;
using v8::EscapableHandleScope;
using v8::FunctionCallbackInfo;
using v8::Integer;
using v8::Isolate;
using v8::Local;
using v8::Maybe;
using v8::MaybeLocal;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Uint32Array;
using v8::Uint8Array;
using v8::Value;
using v8::WeakCallbackInfo;

class CallbackInfo {
 public:
  static inline void Free(char* data, void* hint);
  static inline CallbackInfo* New(Isolate* isolate,
                                  Local<ArrayBuffer> object,
                                  FreeCallback callback,
                                  char* data,
                                  void* hint = 0);
 private:
  static void WeakCallback(const WeakCallbackInfo<CallbackInfo>&);
  inline void WeakCallback(Isolate* isolate);
  inline CallbackInfo(Isolate* isolate,
                      Local<ArrayBuffer> object,
                      FreeCallback callback,
                      char* data,
                      void* hint);
  ~CallbackInfo();
  Persistent<ArrayBuffer> persistent_;
  FreeCallback const callback_;
  char* const data_;
  void* const hint_;
  DISALLOW_COPY_AND_ASSIGN(CallbackInfo);
};


void CallbackInfo::Free(char* data, void*) {
  ::free(data);
}


CallbackInfo* CallbackInfo::New(Isolate* isolate,
                                Local<ArrayBuffer> object,
                                FreeCallback callback,
                                char* data,
                                void* hint) {
  return new CallbackInfo(isolate, object, callback, data, hint);
}


CallbackInfo::CallbackInfo(Isolate* isolate,
                           Local<ArrayBuffer> object,
                           FreeCallback callback,
                           char* data,
                           void* hint)
    : persistent_(isolate, object),
      callback_(callback),
      data_(data),
      hint_(hint) {
  ArrayBuffer::Contents obj_c = object->GetContents();
  CHECK_EQ(data_, static_cast<char*>(obj_c.Data()));
  if (object->ByteLength() != 0)
    CHECK_NE(data_, nullptr);

  persistent_.SetWeak(this, WeakCallback, v8::WeakCallbackType::kParameter);
  persistent_.SetWrapperClassId(BUFFER_ID);
  persistent_.MarkIndependent();
  isolate->AdjustAmountOfExternalAllocatedMemory(sizeof(*this));
}


CallbackInfo::~CallbackInfo() {
  persistent_.Reset();
}


void CallbackInfo::WeakCallback(
    const WeakCallbackInfo<CallbackInfo>& data) {
  CallbackInfo* self = data.GetParameter();
  self->WeakCallback(data.GetIsolate());
  delete self;
}


void CallbackInfo::WeakCallback(Isolate* isolate) {
  callback_(data_, hint_);
  int64_t change_in_bytes = -static_cast<int64_t>(sizeof(*this));
  isolate->AdjustAmountOfExternalAllocatedMemory(change_in_bytes);
}


// Parse index for external array data.
inline MUST_USE_RESULT bool ParseArrayIndex(Local<Value> arg,
                                            size_t def,
                                            size_t* ret) {
  if (arg->IsUndefined()) {
    *ret = def;
    return true;
  }

  int64_t tmp_i = arg->IntegerValue();

  if (tmp_i < 0)
    return false;

  // Check that the result fits in a size_t.
  const uint64_t kSizeMax = static_cast<uint64_t>(static_cast<size_t>(-1));
  // coverity[pointless_expression]
  if (static_cast<uint64_t>(tmp_i) > kSizeMax)
    return false;

  *ret = static_cast<size_t>(tmp_i);
  return true;
}


// Buffer methods

bool HasInstance(Local<Value> val) {
  return val->IsUint8Array();
}


bool HasInstance(Local<Object> obj) {
  return obj->IsUint8Array();
}


char* Data(Local<Value> val) {
  CHECK(val->IsUint8Array());
  Local<Uint8Array> ui = val.As<Uint8Array>();
  ArrayBuffer::Contents ab_c = ui->Buffer()->GetContents();
  return static_cast<char*>(ab_c.Data()) + ui->ByteOffset();
}


char* Data(Local<Object> obj) {
  CHECK(obj->IsUint8Array());
  Local<Uint8Array> ui = obj.As<Uint8Array>();
  ArrayBuffer::Contents ab_c = ui->Buffer()->GetContents();
  return static_cast<char*>(ab_c.Data()) + ui->ByteOffset();
}


size_t Length(Local<Value> val) {
  CHECK(val->IsUint8Array());
  Local<Uint8Array> ui = val.As<Uint8Array>();
  return ui->ByteLength();
}


size_t Length(Local<Object> obj) {
  CHECK(obj->IsUint8Array());
  Local<Uint8Array> ui = obj.As<Uint8Array>();
  return ui->ByteLength();
}


MaybeLocal<Object> New(Isolate* isolate,
                       Local<String> string,
                       enum encoding enc) {
  EscapableHandleScope scope(isolate);

  const size_t length = StringBytes::Size(isolate, string, enc);
  size_t actual = 0;
  char* data = nullptr;

  if (length > 0) {
    data = static_cast<char*>(BUFFER_MALLOC(length));

    if (data == nullptr)
      return Local<Object>();

    actual = StringBytes::Write(isolate, data, length, string, enc);
    CHECK(actual <= length);

    if (actual == 0) {
      free(data);
      data = nullptr;
    } else if (actual < length) {
      data = static_cast<char*>(node::Realloc(data, actual));
      CHECK_NE(data, nullptr);
    }
  }

  Local<Object> buf;
  if (New(isolate, data, actual).ToLocal(&buf))
    return scope.Escape(buf);

  // Object failed to be created. Clean up resources.
  free(data);
  return Local<Object>();
}


MaybeLocal<Object> New(Isolate* isolate, size_t length) {
  EscapableHandleScope handle_scope(isolate);
  Local<Object> obj;
  if (Buffer::New(Environment::GetCurrent(isolate), length).ToLocal(&obj))
    return handle_scope.Escape(obj);
  return Local<Object>();
}


MaybeLocal<Object> New(Environment* env, size_t length) {
  EscapableHandleScope scope(env->isolate());

  // V8 currently only allows a maximum Typed Array index of max Smi.
  if (length > kMaxLength) {
    return Local<Object>();
  }

  void* data;
  if (length > 0) {
    data = BUFFER_MALLOC(length);
    if (data == nullptr)
      return Local<Object>();
  } else {
    data = nullptr;
  }

  Local<ArrayBuffer> ab =
    ArrayBuffer::New(env->isolate(),
        data,
        length,
        ArrayBufferCreationMode::kInternalized);
  Local<Uint8Array> ui = Uint8Array::New(ab, 0, length);
  Maybe<bool> mb =
      ui->SetPrototype(env->context(), env->buffer_prototype_object());
  if (mb.FromMaybe(false))
    return scope.Escape(ui);

  // Object failed to be created. Clean up resources.
  free(data);
  return Local<Object>();
}


MaybeLocal<Object> Copy(Isolate* isolate, const char* data, size_t length) {
  EscapableHandleScope handle_scope(isolate);
  Environment* env = Environment::GetCurrent(isolate);
  Local<Object> obj;
  if (Buffer::Copy(env, data, length).ToLocal(&obj))
    return handle_scope.Escape(obj);
  return Local<Object>();
}


MaybeLocal<Object> Copy(Environment* env, const char* data, size_t length) {
  EscapableHandleScope scope(env->isolate());

  // V8 currently only allows a maximum Typed Array index of max Smi.
  if (length > kMaxLength) {
    return Local<Object>();
  }

  void* new_data;
  if (length > 0) {
    CHECK_NE(data, nullptr);
    new_data = node::Malloc(length);
    if (new_data == nullptr)
      return Local<Object>();
    memcpy(new_data, data, length);
  } else {
    new_data = nullptr;
  }

  Local<ArrayBuffer> ab =
    ArrayBuffer::New(env->isolate(),
        new_data,
        length,
        ArrayBufferCreationMode::kInternalized);
  Local<Uint8Array> ui = Uint8Array::New(ab, 0, length);
  Maybe<bool> mb =
      ui->SetPrototype(env->context(), env->buffer_prototype_object());
  if (mb.FromMaybe(false))
    return scope.Escape(ui);

  // Object failed to be created. Clean up resources.
  free(new_data);
  return Local<Object>();
}


MaybeLocal<Object> New(Isolate* isolate,
                       char* data,
                       size_t length,
                       FreeCallback callback,
                       void* hint) {
  EscapableHandleScope handle_scope(isolate);
  Environment* env = Environment::GetCurrent(isolate);
  Local<Object> obj;
  if (Buffer::New(env, data, length, callback, hint).ToLocal(&obj))
    return handle_scope.Escape(obj);
  return Local<Object>();
}


MaybeLocal<Object> New(Environment* env,
                       char* data,
                       size_t length,
                       FreeCallback callback,
                       void* hint) {
  EscapableHandleScope scope(env->isolate());

  if (length > kMaxLength) {
    return Local<Object>();
  }

  Local<ArrayBuffer> ab = ArrayBuffer::New(env->isolate(), data, length);
  // `Neuter()`ing is required here to prevent materialization of the backing
  // store in v8. `nullptr` buffers are not writable, so this is semantically
  // correct.
  if (data == nullptr)
    ab->Neuter();
  Local<Uint8Array> ui = Uint8Array::New(ab, 0, length);
  Maybe<bool> mb =
      ui->SetPrototype(env->context(), env->buffer_prototype_object());

  if (!mb.FromMaybe(false))
    return Local<Object>();

  CallbackInfo::New(env->isolate(), ab, callback, data, hint);
  return scope.Escape(ui);
}


MaybeLocal<Object> New(Isolate* isolate, char* data, size_t length) {
  EscapableHandleScope handle_scope(isolate);
  Environment* env = Environment::GetCurrent(isolate);
  Local<Object> obj;
  if (Buffer::New(env, data, length).ToLocal(&obj))
    return handle_scope.Escape(obj);
  return Local<Object>();
}


MaybeLocal<Object> New(Environment* env, char* data, size_t length) {
  EscapableHandleScope scope(env->isolate());

  if (length > 0) {
    CHECK_NE(data, nullptr);
    CHECK(length <= kMaxLength);
  }

  Local<ArrayBuffer> ab =
      ArrayBuffer::New(env->isolate(),
                       data,
                       length,
                       ArrayBufferCreationMode::kInternalized);
  Local<Uint8Array> ui = Uint8Array::New(ab, 0, length);
  Maybe<bool> mb =
      ui->SetPrototype(env->context(), env->buffer_prototype_object());
  if (mb.FromMaybe(false))
    return scope.Escape(ui);
  return Local<Object>();
}


void CreateFromString(const FunctionCallbackInfo<Value>& args) {
  CHECK(args[0]->IsString());
  CHECK(args[1]->IsString());

  enum encoding enc = ParseEncoding(args.GetIsolate(),
                                    args[1].As<String>(),
                                    UTF8);
  Local<Object> buf;
  if (New(args.GetIsolate(), args[0].As<String>(), enc).ToLocal(&buf))
    args.GetReturnValue().Set(buf);
}


template <encoding encoding>
void StringSlice(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  THROW_AND_RETURN_UNLESS_BUFFER(env, args.This());
  SPREAD_ARG(args.This(), ts_obj);

  if (ts_obj_length == 0)
    return args.GetReturnValue().SetEmptyString();

  SLICE_START_END(args[0], args[1], ts_obj_length)

  args.GetReturnValue().Set(
      StringBytes::Encode(isolate, ts_obj_data + start, length, encoding));
}


template <>
void StringSlice<UCS2>(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  THROW_AND_RETURN_UNLESS_BUFFER(env, args.This());
  SPREAD_ARG(args.This(), ts_obj);

  if (ts_obj_length == 0)
    return args.GetReturnValue().SetEmptyString();

  SLICE_START_END(args[0], args[1], ts_obj_length)
  length /= 2;

  const char* data = ts_obj_data + start;
  const uint16_t* buf;
  bool release = false;

  // Node's "ucs2" encoding expects LE character data inside a Buffer, so we
  // need to reorder on BE platforms.  See http://nodejs.org/api/buffer.html
  // regarding Node's "ucs2" encoding specification.
  const bool aligned = (reinterpret_cast<uintptr_t>(data) % sizeof(*buf) == 0);
  if (IsLittleEndian() && !aligned) {
    // Make a copy to avoid unaligned accesses in v8::String::NewFromTwoByte().
    // This applies ONLY to little endian platforms, as misalignment will be
    // handled by a byte-swapping operation in StringBytes::Encode on
    // big endian platforms.
    uint16_t* copy = new uint16_t[length];
    for (size_t i = 0, k = 0; i < length; i += 1, k += 2) {
      // Assumes that the input is little endian.
      const uint8_t lo = static_cast<uint8_t>(data[k + 0]);
      const uint8_t hi = static_cast<uint8_t>(data[k + 1]);
      copy[i] = lo | hi << 8;
    }
    buf = copy;
    release = true;
  } else {
    buf = reinterpret_cast<const uint16_t*>(data);
  }

  args.GetReturnValue().Set(StringBytes::Encode(env->isolate(), buf, length));

  if (release)
    delete[] buf;
}


void Latin1Slice(const FunctionCallbackInfo<Value>& args) {
  StringSlice<LATIN1>(args);
}


void AsciiSlice(const FunctionCallbackInfo<Value>& args) {
  StringSlice<ASCII>(args);
}


void Utf8Slice(const FunctionCallbackInfo<Value>& args) {
  StringSlice<UTF8>(args);
}


void Ucs2Slice(const FunctionCallbackInfo<Value>& args) {
  StringSlice<UCS2>(args);
}


void HexSlice(const FunctionCallbackInfo<Value>& args) {
  StringSlice<HEX>(args);
}


void Base64Slice(const FunctionCallbackInfo<Value>& args) {
  StringSlice<BASE64>(args);
}


// bytesCopied = buffer.copy(target[, targetStart][, sourceStart][, sourceEnd]);
void Copy(const FunctionCallbackInfo<Value> &args) {
  Environment* env = Environment::GetCurrent(args);

  THROW_AND_RETURN_UNLESS_BUFFER(env, args.This());
  THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
  Local<Object> target_obj = args[0].As<Object>();
  SPREAD_ARG(args.This(), ts_obj);
  SPREAD_ARG(target_obj, target);

  size_t target_start;
  size_t source_start;
  size_t source_end;

  THROW_AND_RETURN_IF_OOB(ParseArrayIndex(args[1], 0, &target_start));
  THROW_AND_RETURN_IF_OOB(ParseArrayIndex(args[2], 0, &source_start));
  THROW_AND_RETURN_IF_OOB(ParseArrayIndex(args[3], ts_obj_length, &source_end));

  // Copy 0 bytes; we're done
  if (target_start >= target_length || source_start >= source_end)
    return args.GetReturnValue().Set(0);

  if (source_start > ts_obj_length)
    return env->ThrowRangeError("out of range index");

  if (source_end - source_start > target_length - target_start)
    source_end = source_start + target_length - target_start;

  uint32_t to_copy = MIN(MIN(source_end - source_start,
                             target_length - target_start),
                             ts_obj_length - source_start);

  memmove(target_data + target_start, ts_obj_data + source_start, to_copy);
  args.GetReturnValue().Set(to_copy);
}


void Fill(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
  SPREAD_ARG(args[0], ts_obj);

  size_t start = args[2]->Uint32Value();
  size_t end = args[3]->Uint32Value();
  size_t fill_length = end - start;
  Local<String> str_obj;
  size_t str_length;
  enum encoding enc;
  CHECK(fill_length + start <= ts_obj_length);

  // First check if Buffer has been passed.
  if (Buffer::HasInstance(args[1])) {
    SPREAD_ARG(args[1], fill_obj);
    str_length = fill_obj_length;
    memcpy(ts_obj_data + start, fill_obj_data, MIN(str_length, fill_length));
    goto start_fill;
  }

  // Then coerce everything that's not a string.
  if (!args[1]->IsString()) {
    int value = args[1]->Uint32Value() & 255;
    memset(ts_obj_data + start, value, fill_length);
    return;
  }

  str_obj = args[1]->ToString(env->isolate());
  enc = ParseEncoding(env->isolate(), args[4], UTF8);
  str_length =
      enc == UTF8 ? str_obj->Utf8Length() :
      enc == UCS2 ? str_obj->Length() * sizeof(uint16_t) : str_obj->Length();

  if (enc == HEX && str_length  % 2 != 0)
    return env->ThrowTypeError("Invalid hex string");

  if (str_length == 0)
    return;

  // Can't use StringBytes::Write() in all cases. For example if attempting
  // to write a two byte character into a one byte Buffer.
  if (enc == UTF8) {
    node::Utf8Value str(env->isolate(), args[1]);
    memcpy(ts_obj_data + start, *str, MIN(str_length, fill_length));

  } else if (enc == UCS2) {
    node::TwoByteValue str(env->isolate(), args[1]);
    memcpy(ts_obj_data + start, *str, MIN(str_length, fill_length));

  } else {
    // Write initial String to Buffer, then use that memory to copy remainder
    // of string. Correct the string length for cases like HEX where less than
    // the total string length is written.
    str_length = StringBytes::Write(env->isolate(),
                                    ts_obj_data + start,
                                    fill_length,
                                    str_obj,
                                    enc,
                                    nullptr);
    // This check is also needed in case Write() returns that no bytes could
    // be written.
    // TODO(trevnorris): Should this throw? Because of the string length was
    // greater than 0 but couldn't be written then the string was invalid.
    if (str_length == 0)
      return;
  }

 start_fill:

  if (str_length >= fill_length)
    return;


  size_t in_there = str_length;
  char* ptr = ts_obj_data + start + str_length;

  while (in_there < fill_length - in_there) {
    memcpy(ptr, ts_obj_data + start, in_there);
    ptr += in_there;
    in_there *= 2;
  }

  if (in_there < fill_length) {
    memcpy(ptr, ts_obj_data + start, fill_length - in_there);
  }
}


template <encoding encoding>
void StringWrite(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  THROW_AND_RETURN_UNLESS_BUFFER(env, args.This());
  SPREAD_ARG(args.This(), ts_obj);

  if (!args[0]->IsString())
    return env->ThrowTypeError("Argument must be a string");

  Local<String> str = args[0]->ToString(env->isolate());

  if (encoding == HEX && str->Length() % 2 != 0)
    return env->ThrowTypeError("Invalid hex string");

  size_t offset;
  size_t max_length;

  THROW_AND_RETURN_IF_OOB(ParseArrayIndex(args[1], 0, &offset));
  if (offset > ts_obj_length)
    return env->ThrowRangeError("Offset is out of bounds");

  THROW_AND_RETURN_IF_OOB(ParseArrayIndex(args[2], ts_obj_length - offset,
                                          &max_length));

  max_length = MIN(ts_obj_length - offset, max_length);

  if (max_length == 0)
    return args.GetReturnValue().Set(0);

  uint32_t written = StringBytes::Write(env->isolate(),
                                        ts_obj_data + offset,
                                        max_length,
                                        str,
                                        encoding,
                                        nullptr);
  args.GetReturnValue().Set(written);
}


void Base64Write(const FunctionCallbackInfo<Value>& args) {
  StringWrite<BASE64>(args);
}


void Latin1Write(const FunctionCallbackInfo<Value>& args) {
  StringWrite<LATIN1>(args);
}


void Utf8Write(const FunctionCallbackInfo<Value>& args) {
  StringWrite<UTF8>(args);
}


void Ucs2Write(const FunctionCallbackInfo<Value>& args) {
  StringWrite<UCS2>(args);
}


void HexWrite(const FunctionCallbackInfo<Value>& args) {
  StringWrite<HEX>(args);
}


void AsciiWrite(const FunctionCallbackInfo<Value>& args) {
  StringWrite<ASCII>(args);
}


static inline void Swizzle(char* start, unsigned int len) {
  char* end = start + len - 1;
  while (start < end) {
    char tmp = *start;
    *start++ = *end;
    *end-- = tmp;
  }
}


template <typename T, enum Endianness endianness>
void ReadFloatGeneric(const FunctionCallbackInfo<Value>& args) {
  THROW_AND_RETURN_UNLESS_BUFFER(Environment::GetCurrent(args), args[0]);
  SPREAD_ARG(args[0], ts_obj);

  uint32_t offset = args[1]->Uint32Value();
  CHECK_LE(offset + sizeof(T), ts_obj_length);

  union NoAlias {
    T val;
    char bytes[sizeof(T)];
  };

  union NoAlias na;
  const char* ptr = static_cast<const char*>(ts_obj_data) + offset;
  memcpy(na.bytes, ptr, sizeof(na.bytes));
  if (endianness != GetEndianness())
    Swizzle(na.bytes, sizeof(na.bytes));

  args.GetReturnValue().Set(na.val);
}


void ReadFloatLE(const FunctionCallbackInfo<Value>& args) {
  ReadFloatGeneric<float, kLittleEndian>(args);
}


void ReadFloatBE(const FunctionCallbackInfo<Value>& args) {
  ReadFloatGeneric<float, kBigEndian>(args);
}


void ReadDoubleLE(const FunctionCallbackInfo<Value>& args) {
  ReadFloatGeneric<double, kLittleEndian>(args);
}


void ReadDoubleBE(const FunctionCallbackInfo<Value>& args) {
  ReadFloatGeneric<double, kBigEndian>(args);
}


template <typename T, enum Endianness endianness>
void WriteFloatGeneric(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  bool should_assert = args.Length() < 4;

  if (should_assert) {
    THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
  }

  Local<Uint8Array> ts_obj = args[0].As<Uint8Array>();
  ArrayBuffer::Contents ts_obj_c = ts_obj->Buffer()->GetContents();
  const size_t ts_obj_offset = ts_obj->ByteOffset();
  const size_t ts_obj_length = ts_obj->ByteLength();
  char* const ts_obj_data =
      static_cast<char*>(ts_obj_c.Data()) + ts_obj_offset;
  if (ts_obj_length > 0)
    CHECK_NE(ts_obj_data, nullptr);

  T val = args[1]->NumberValue(env->context()).FromMaybe(0);
  size_t offset = args[2]->IntegerValue(env->context()).FromMaybe(0);

  size_t memcpy_num = sizeof(T);

  if (should_assert) {
    THROW_AND_RETURN_IF_OOB(offset + memcpy_num >= memcpy_num);
    THROW_AND_RETURN_IF_OOB(offset + memcpy_num <= ts_obj_length);
  }

  if (offset + memcpy_num > ts_obj_length)
    memcpy_num = ts_obj_length - offset;

  union NoAlias {
    T val;
    char bytes[sizeof(T)];
  };

  union NoAlias na = { val };
  char* ptr = static_cast<char*>(ts_obj_data) + offset;
  if (endianness != GetEndianness())
    Swizzle(na.bytes, sizeof(na.bytes));
  memcpy(ptr, na.bytes, memcpy_num);
}


void WriteFloatLE(const FunctionCallbackInfo<Value>& args) {
  WriteFloatGeneric<float, kLittleEndian>(args);
}


void WriteFloatBE(const FunctionCallbackInfo<Value>& args) {
  WriteFloatGeneric<float, kBigEndian>(args);
}


void WriteDoubleLE(const FunctionCallbackInfo<Value>& args) {
  WriteFloatGeneric<double, kLittleEndian>(args);
}


void WriteDoubleBE(const FunctionCallbackInfo<Value>& args) {
  WriteFloatGeneric<double, kBigEndian>(args);
}


void ByteLengthUtf8(const FunctionCallbackInfo<Value> &args) {
  CHECK(args[0]->IsString());

  // Fast case: avoid StringBytes on UTF8 string. Jump to v8.
  args.GetReturnValue().Set(args[0].As<String>()->Utf8Length());
}

// Normalize val to be an integer in the range of [1, -1] since
// implementations of memcmp() can vary by platform.
static int normalizeCompareVal(int val, size_t a_length, size_t b_length) {
  if (val == 0) {
    if (a_length > b_length)
      return 1;
    else if (a_length < b_length)
      return -1;
  } else {
    if (val > 0)
      return 1;
    else
      return -1;
  }
  return val;
}

void CompareOffset(const FunctionCallbackInfo<Value> &args) {
  Environment* env = Environment::GetCurrent(args);

  THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
  THROW_AND_RETURN_UNLESS_BUFFER(env, args[1]);
  SPREAD_ARG(args[0], ts_obj);
  SPREAD_ARG(args[1], target);

  size_t target_start;
  size_t source_start;
  size_t source_end;
  size_t target_end;

  THROW_AND_RETURN_IF_OOB(ParseArrayIndex(args[2], 0, &target_start));
  THROW_AND_RETURN_IF_OOB(ParseArrayIndex(args[3], 0, &source_start));
  THROW_AND_RETURN_IF_OOB(ParseArrayIndex(args[4], target_length, &target_end));
  THROW_AND_RETURN_IF_OOB(ParseArrayIndex(args[5], ts_obj_length, &source_end));

  if (source_start > ts_obj_length)
    return env->ThrowRangeError("out of range index");
  if (target_start > target_length)
    return env->ThrowRangeError("out of range index");

  CHECK_LE(source_start, source_end);
  CHECK_LE(target_start, target_end);

  size_t to_cmp = MIN(MIN(source_end - source_start,
                      target_end - target_start),
                      ts_obj_length - source_start);

  int val = normalizeCompareVal(to_cmp > 0 ?
                                  memcmp(ts_obj_data + source_start,
                                         target_data + target_start,
                                         to_cmp) : 0,
                                source_end - source_start,
                                target_end - target_start);

  args.GetReturnValue().Set(val);
}

void Compare(const FunctionCallbackInfo<Value> &args) {
  Environment* env = Environment::GetCurrent(args);

  THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
  THROW_AND_RETURN_UNLESS_BUFFER(env, args[1]);
  SPREAD_ARG(args[0], obj_a);
  SPREAD_ARG(args[1], obj_b);

  size_t cmp_length = MIN(obj_a_length, obj_b_length);

  int val = normalizeCompareVal(cmp_length > 0 ?
                                memcmp(obj_a_data, obj_b_data, cmp_length) : 0,
                                obj_a_length, obj_b_length);
  args.GetReturnValue().Set(val);
}


// Computes the offset for starting an indexOf or lastIndexOf search.
// Returns either a valid offset in [0...<length - 1>], ie inside the Buffer,
// or -1 to signal that there is no possible match.
int64_t IndexOfOffset(size_t length, int64_t offset_i64, bool is_forward) {
  int64_t length_i64 = static_cast<int64_t>(length);
  if (length_i64 == 0) {
    // Empty buffer, no match.
    return -1;
  }
  if (offset_i64 < 0) {
    if (offset_i64 + length_i64 >= 0) {
      // Negative offsets count backwards from the end of the buffer.
      return length_i64 + offset_i64;
    } else if (is_forward) {
      // indexOf from before the start of the buffer: search the whole buffer.
      return 0;
    } else {
      // lastIndexOf from before the start of the buffer: no match.
      return -1;
    }
  } else {
    if (offset_i64 < length_i64) {
      // Valid positive offset.
      return offset_i64;
    } else if (is_forward) {
      // indexOf from past the end of the buffer: no match.
      return -1;
    } else {
      // lastIndexOf from past the end of the buffer: search the whole buffer.
      return length_i64 - 1;
    }
  }
}

void IndexOfString(const FunctionCallbackInfo<Value>& args) {
  ASSERT(args[1]->IsString());
  ASSERT(args[2]->IsNumber());
  ASSERT(args[4]->IsBoolean());

  enum encoding enc = ParseEncoding(args.GetIsolate(),
                                    args[3],
                                    UTF8);

  THROW_AND_RETURN_UNLESS_BUFFER(Environment::GetCurrent(args), args[0]);
  SPREAD_ARG(args[0], ts_obj);

  Local<String> needle = args[1].As<String>();
  int64_t offset_i64 = args[2]->IntegerValue();
  bool is_forward = args[4]->IsTrue();

  const char* haystack = ts_obj_data;
  // Round down to the nearest multiple of 2 in case of UCS2.
  const size_t haystack_length = (enc == UCS2) ?
      ts_obj_length &~ 1 : ts_obj_length;  // NOLINT(whitespace/operators)

  const size_t needle_length =
      StringBytes::Size(args.GetIsolate(), needle, enc);

  if (needle_length == 0 || haystack_length == 0) {
    return args.GetReturnValue().Set(-1);
  }

  int64_t opt_offset = IndexOfOffset(haystack_length, offset_i64, is_forward);
  if (opt_offset <= -1) {
    return args.GetReturnValue().Set(-1);
  }
  size_t offset = static_cast<size_t>(opt_offset);
  CHECK_LT(offset, haystack_length);
  if ((is_forward && needle_length + offset > haystack_length) ||
      needle_length > haystack_length) {
    return args.GetReturnValue().Set(-1);
  }

  size_t result = haystack_length;

  if (enc == UCS2) {
    String::Value needle_value(needle);
    if (*needle_value == nullptr)
      return args.GetReturnValue().Set(-1);

    if (haystack_length < 2 || needle_value.length() < 1) {
      return args.GetReturnValue().Set(-1);
    }

    if (IsBigEndian()) {
      StringBytes::InlineDecoder decoder;
      decoder.Decode(Environment::GetCurrent(args), needle, args[3], UCS2);
      const uint16_t* decoded_string =
          reinterpret_cast<const uint16_t*>(decoder.out());

      if (decoded_string == nullptr)
        return args.GetReturnValue().Set(-1);

      result = SearchString(reinterpret_cast<const uint16_t*>(haystack),
                            haystack_length / 2,
                            decoded_string,
                            decoder.size() / 2,
                            offset / 2,
                            is_forward);
    } else {
      result = SearchString(reinterpret_cast<const uint16_t*>(haystack),
                            haystack_length / 2,
                            reinterpret_cast<const uint16_t*>(*needle_value),
                            needle_value.length(),
                            offset / 2,
                            is_forward);
    }
    result *= 2;
  } else if (enc == UTF8) {
    String::Utf8Value needle_value(needle);
    if (*needle_value == nullptr)
      return args.GetReturnValue().Set(-1);

    result = SearchString(reinterpret_cast<const uint8_t*>(haystack),
                          haystack_length,
                          reinterpret_cast<const uint8_t*>(*needle_value),
                          needle_length,
                          offset,
                          is_forward);
  } else if (enc == LATIN1) {
    uint8_t* needle_data = static_cast<uint8_t*>(node::Malloc(needle_length));
    if (needle_data == nullptr) {
      return args.GetReturnValue().Set(-1);
    }
    needle->WriteOneByte(
        needle_data, 0, needle_length, String::NO_NULL_TERMINATION);

    result = SearchString(reinterpret_cast<const uint8_t*>(haystack),
                          haystack_length,
                          needle_data,
                          needle_length,
                          offset,
                          is_forward);
    free(needle_data);
  }

  args.GetReturnValue().Set(
      result == haystack_length ? -1 : static_cast<int>(result));
}

void IndexOfBuffer(const FunctionCallbackInfo<Value>& args) {
  ASSERT(args[1]->IsObject());
  ASSERT(args[2]->IsNumber());
  ASSERT(args[4]->IsBoolean());

  enum encoding enc = ParseEncoding(args.GetIsolate(),
                                    args[3],
                                    UTF8);

  THROW_AND_RETURN_UNLESS_BUFFER(Environment::GetCurrent(args), args[0]);
  THROW_AND_RETURN_UNLESS_BUFFER(Environment::GetCurrent(args), args[1]);
  SPREAD_ARG(args[0], ts_obj);
  SPREAD_ARG(args[1], buf);
  int64_t offset_i64 = args[2]->IntegerValue();
  bool is_forward = args[4]->IsTrue();

  const char* haystack = ts_obj_data;
  const size_t haystack_length = ts_obj_length;
  const char* needle = buf_data;
  const size_t needle_length = buf_length;

  if (needle_length == 0 || haystack_length == 0) {
    return args.GetReturnValue().Set(-1);
  }

  int64_t opt_offset = IndexOfOffset(haystack_length, offset_i64, is_forward);
  if (opt_offset <= -1) {
    return args.GetReturnValue().Set(-1);
  }
  size_t offset = static_cast<size_t>(opt_offset);
  CHECK_LT(offset, haystack_length);
  if ((is_forward && needle_length + offset > haystack_length) ||
      needle_length > haystack_length) {
    return args.GetReturnValue().Set(-1);
  }

  size_t result = haystack_length;

  if (enc == UCS2) {
    if (haystack_length < 2 || needle_length < 2) {
      return args.GetReturnValue().Set(-1);
    }
    result = SearchString(
        reinterpret_cast<const uint16_t*>(haystack),
        haystack_length / 2,
        reinterpret_cast<const uint16_t*>(needle),
        needle_length / 2,
        offset / 2,
        is_forward);
    result *= 2;
  } else {
    result = SearchString(
        reinterpret_cast<const uint8_t*>(haystack),
        haystack_length,
        reinterpret_cast<const uint8_t*>(needle),
        needle_length,
        offset,
        is_forward);
  }

  args.GetReturnValue().Set(
      result == haystack_length ? -1 : static_cast<int>(result));
}

void IndexOfNumber(const FunctionCallbackInfo<Value>& args) {
  ASSERT(args[1]->IsNumber());
  ASSERT(args[2]->IsNumber());
  ASSERT(args[3]->IsBoolean());

  THROW_AND_RETURN_UNLESS_BUFFER(Environment::GetCurrent(args), args[0]);
  SPREAD_ARG(args[0], ts_obj);

  uint32_t needle = args[1]->Uint32Value();
  int64_t offset_i64 = args[2]->IntegerValue();
  bool is_forward = args[3]->IsTrue();

  int64_t opt_offset = IndexOfOffset(ts_obj_length, offset_i64, is_forward);
  if (opt_offset <= -1) {
    return args.GetReturnValue().Set(-1);
  }
  size_t offset = static_cast<size_t>(opt_offset);
  CHECK_LT(offset, ts_obj_length);

  const void* ptr;
  if (is_forward) {
    ptr = memchr(ts_obj_data + offset, needle, ts_obj_length - offset);
  } else {
    ptr = node::stringsearch::MemrchrFill(ts_obj_data, needle, offset + 1);
  }
  const char* ptr_char = static_cast<const char*>(ptr);
  args.GetReturnValue().Set(ptr ? static_cast<int>(ptr_char - ts_obj_data)
                                : -1);
}


void Swap16(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
  SPREAD_ARG(args[0], ts_obj);
  SwapBytes16(ts_obj_data, ts_obj_length);
  args.GetReturnValue().Set(args[0]);
}


void Swap32(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
  SPREAD_ARG(args[0], ts_obj);
  SwapBytes32(ts_obj_data, ts_obj_length);
  args.GetReturnValue().Set(args[0]);
}


void Swap64(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
  SPREAD_ARG(args[0], ts_obj);
  SwapBytes64(ts_obj_data, ts_obj_length);
  args.GetReturnValue().Set(args[0]);
}


// pass Buffer object to load prototype methods
void SetupBufferJS(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  CHECK(args[0]->IsObject());
  Local<Object> proto = args[0].As<Object>();
  env->set_buffer_prototype_object(proto);

  env->SetMethod(proto, "asciiSlice", AsciiSlice);
  env->SetMethod(proto, "base64Slice", Base64Slice);
  env->SetMethod(proto, "latin1Slice", Latin1Slice);
  env->SetMethod(proto, "hexSlice", HexSlice);
  env->SetMethod(proto, "ucs2Slice", Ucs2Slice);
  env->SetMethod(proto, "utf8Slice", Utf8Slice);

  env->SetMethod(proto, "asciiWrite", AsciiWrite);
  env->SetMethod(proto, "base64Write", Base64Write);
  env->SetMethod(proto, "latin1Write", Latin1Write);
  env->SetMethod(proto, "hexWrite", HexWrite);
  env->SetMethod(proto, "ucs2Write", Ucs2Write);
  env->SetMethod(proto, "utf8Write", Utf8Write);

  env->SetMethod(proto, "copy", Copy);

  CHECK(args[1]->IsObject());
  Local<Object> bObj = args[1].As<Object>();

  uint32_t* const fields = env->array_buffer_allocator_info()->fields();
  uint32_t const fields_count =
      env->array_buffer_allocator_info()->fields_count();

  Local<ArrayBuffer> array_buffer =
      ArrayBuffer::New(env->isolate(), fields, sizeof(*fields) * fields_count);

  bObj->Set(String::NewFromUtf8(env->isolate(), "flags"),
            Uint32Array::New(array_buffer, 0, fields_count));
}


void Initialize(Local<Object> target,
                Local<Value> unused,
                Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);

  env->SetMethod(target, "setupBufferJS", SetupBufferJS);
  env->SetMethod(target, "createFromString", CreateFromString);

  env->SetMethod(target, "byteLengthUtf8", ByteLengthUtf8);
  env->SetMethod(target, "compare", Compare);
  env->SetMethod(target, "compareOffset", CompareOffset);
  env->SetMethod(target, "fill", Fill);
  env->SetMethod(target, "indexOfBuffer", IndexOfBuffer);
  env->SetMethod(target, "indexOfNumber", IndexOfNumber);
  env->SetMethod(target, "indexOfString", IndexOfString);

  env->SetMethod(target, "readDoubleBE", ReadDoubleBE);
  env->SetMethod(target, "readDoubleLE", ReadDoubleLE);
  env->SetMethod(target, "readFloatBE", ReadFloatBE);
  env->SetMethod(target, "readFloatLE", ReadFloatLE);

  env->SetMethod(target, "writeDoubleBE", WriteDoubleBE);
  env->SetMethod(target, "writeDoubleLE", WriteDoubleLE);
  env->SetMethod(target, "writeFloatBE", WriteFloatBE);
  env->SetMethod(target, "writeFloatLE", WriteFloatLE);

  env->SetMethod(target, "swap16", Swap16);
  env->SetMethod(target, "swap32", Swap32);
  env->SetMethod(target, "swap64", Swap64);

  target->Set(env->context(),
              FIXED_ONE_BYTE_STRING(env->isolate(), "kMaxLength"),
              Integer::NewFromUnsigned(env->isolate(), kMaxLength)).FromJust();

  target->Set(env->context(),
              FIXED_ONE_BYTE_STRING(env->isolate(), "kStringMaxLength"),
              Integer::New(env->isolate(), String::kMaxLength)).FromJust();
}


}  // namespace Buffer
}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(buffer, node::Buffer::Initialize)
