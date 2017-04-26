#ifndef SRC_STRING_BYTES_H_
#define SRC_STRING_BYTES_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

// Decodes a v8::Local<v8::String> or Buffer to a raw char*

#include "v8.h"
#include "node.h"
#include "env.h"
#include "env-inl.h"
#include "util.h"

namespace node {

class StringBytes {
 public:
  class InlineDecoder : public MaybeStackBuffer<char> {
   public:
    inline bool Decode(Environment* env,
                       v8::Local<v8::String> string,
                       v8::Local<v8::Value> encoding,
                       enum encoding _default) {
      enum encoding enc = ParseEncoding(env->isolate(), encoding, _default);
      if (!StringBytes::IsValidString(env->isolate(), string, enc)) {
        env->ThrowTypeError("Bad input string");
        return false;
      }

      const size_t storage = StringBytes::StorageSize(env->isolate(),
                                                      string,
                                                      enc);
      AllocateSufficientStorage(storage);
      const size_t length = StringBytes::Write(env->isolate(),
                                               out(),
                                               storage,
                                               string,
                                               enc);

      // No zero terminator is included when using this method.
      SetLength(length);
      return true;
    }

    inline size_t size() const { return length(); }
  };

  // Does the string match the encoding? Quick but non-exhaustive.
  // Example: a HEX string must have a length that's a multiple of two.
  // FIXME(bnoordhuis) IsMaybeValidString()? Naming things is hard...
  static bool IsValidString(v8::Isolate* isolate,
                            v8::Local<v8::String> string,
                            enum encoding enc);

  // Fast, but can be 2 bytes oversized for Base64, and
  // as much as triple UTF-8 strings <= 65536 chars in length
  static size_t StorageSize(v8::Isolate* isolate,
                            v8::Local<v8::Value> val,
                            enum encoding enc);

  // Precise byte count, but slightly slower for Base64 and
  // very much slower for UTF-8
  static size_t Size(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     enum encoding enc);

  // If the string is external then assign external properties to data and len,
  // then return true. If not return false.
  static bool GetExternalParts(v8::Isolate* isolate,
                               v8::Local<v8::Value> val,
                               const char** data,
                               size_t* len);

  // Write the bytes from the string or buffer into the char*
  // returns the number of bytes written, which will always be
  // <= buflen.  Use StorageSize/Size first to know how much
  // memory to allocate.
  static size_t Write(v8::Isolate* isolate,
                      char* buf,
                      size_t buflen,
                      v8::Local<v8::Value> val,
                      enum encoding enc,
                      int* chars_written = nullptr);

  // Take the bytes in the src, and turn it into a Buffer or String.
  // Don't call with encoding=UCS2.
  static v8::Local<v8::Value> Encode(v8::Isolate* isolate,
                                     const char* buf,
                                     size_t buflen,
                                     enum encoding encoding);

  // The input buffer should be in host endianness.
  static v8::Local<v8::Value> Encode(v8::Isolate* isolate,
                                     const uint16_t* buf,
                                     size_t buflen);

  static v8::Local<v8::Value> Encode(v8::Isolate* isolate,
                                     const char* buf,
                                     enum encoding encoding);

 private:
  static size_t WriteUCS2(char* buf,
                          size_t buflen,
                          size_t nbytes,
                          const char* data,
                          v8::Local<v8::String> str,
                          int flags,
                          size_t* chars_written);
};

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_STRING_BYTES_H_
