#undef HAVE_OPENSSL // weolar

#if HAVE_OPENSSL

#include "node.h"
#include "node_buffer.h"
#include "node_crypto.h"
#include "node_crypto_bio.h"
#include "node_crypto_groups.h"
#include "tls_wrap.h"  // TLSWrap

#include "async-wrap.h"
#include "async-wrap-inl.h"
#include "env.h"
#include "env-inl.h"
#include "string_bytes.h"
#include "util.h"
#include "util-inl.h"
#include "v8.h"
// CNNIC Hash WhiteList is taken from
// https://hg.mozilla.org/mozilla-central/raw-file/98820360ab66/security/
// certverifier/CNNICHashWhitelist.inc
#include "CNNICHashWhitelist.inc"

#include <errno.h>
#include <limits.h>  // INT_MAX
#include <math.h>
#include <stdlib.h>
#include <string.h>

#if USING_VC6RT == 1
extern "C" int snprintf(char* buffer, size_t count, const char* format, ...);
#include <algorithmvc6.h>
#endif

#define THROW_AND_RETURN_IF_NOT_STRING_OR_BUFFER(val, prefix)                  \
  do {                                                                         \
    if (!Buffer::HasInstance(val) && !val->IsString()) {                       \
      return env->ThrowTypeError(prefix " must be a string or a buffer");      \
    }                                                                          \
  } while (0)

#define THROW_AND_RETURN_IF_NOT_BUFFER(val, prefix)           \
  do {                                                        \
    if (!Buffer::HasInstance(val)) {                          \
      return env->ThrowTypeError(prefix " must be a buffer"); \
    }                                                         \
  } while (0)

#define THROW_AND_RETURN_IF_NOT_STRING(val, prefix)           \
  do {                                                        \
    if (!val->IsString()) {                                   \
      return env->ThrowTypeError(prefix " must be a string"); \
    }                                                         \
  } while (0)

static const char PUBLIC_KEY_PFX[] =  "-----BEGIN PUBLIC KEY-----";
static const int PUBLIC_KEY_PFX_LEN = sizeof(PUBLIC_KEY_PFX) - 1;
static const char PUBRSA_KEY_PFX[] =  "-----BEGIN RSA PUBLIC KEY-----";
static const int PUBRSA_KEY_PFX_LEN = sizeof(PUBRSA_KEY_PFX) - 1;
static const char CERTIFICATE_PFX[] =  "-----BEGIN CERTIFICATE-----";
static const int CERTIFICATE_PFX_LEN = sizeof(CERTIFICATE_PFX) - 1;

static const int X509_NAME_FLAGS = ASN1_STRFLGS_ESC_CTRL
                                 | ASN1_STRFLGS_UTF8_CONVERT
                                 | XN_FLAG_SEP_MULTILINE
                                 | XN_FLAG_FN_SN;

namespace node {
namespace crypto {

using v8::AccessorSignature;
using v8::Array;
using v8::Boolean;
using v8::Context;
using v8::DEFAULT;
using v8::DontDelete;
using v8::EscapableHandleScope;
using v8::Exception;
using v8::External;
using v8::False;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Integer;
using v8::Isolate;
using v8::Local;
using v8::Null;
using v8::Object;
using v8::Persistent;
using v8::PropertyAttribute;
using v8::PropertyCallbackInfo;
using v8::ReadOnly;
using v8::String;
using v8::Value;


// Subject DER of CNNIC ROOT CA and CNNIC EV ROOT CA are taken from
// https://hg.mozilla.org/mozilla-central/file/98820360ab66/security/
// certverifier/NSSCertDBTrustDomain.cpp#l672
// C = CN, O = CNNIC, CN = CNNIC ROOT
static const uint8_t CNNIC_ROOT_CA_SUBJECT_DATA[] =
    "\x30\x32\x31\x0B\x30\x09\x06\x03\x55\x04\x06\x13\x02\x43\x4E\x31\x0E\x30"
    "\x0C\x06\x03\x55\x04\x0A\x13\x05\x43\x4E\x4E\x49\x43\x31\x13\x30\x11\x06"
    "\x03\x55\x04\x03\x13\x0A\x43\x4E\x4E\x49\x43\x20\x52\x4F\x4F\x54";
static const uint8_t* cnnic_p = CNNIC_ROOT_CA_SUBJECT_DATA;
static X509_NAME* cnnic_name =
    d2i_X509_NAME(nullptr, &cnnic_p, sizeof(CNNIC_ROOT_CA_SUBJECT_DATA)-1);

// C = CN, O = China Internet Network Information Center, CN = China
// Internet Network Information Center EV Certificates Root
static const uint8_t CNNIC_EV_ROOT_CA_SUBJECT_DATA[] =
    "\x30\x81\x8A\x31\x0B\x30\x09\x06\x03\x55\x04\x06\x13\x02\x43\x4E\x31\x32"
    "\x30\x30\x06\x03\x55\x04\x0A\x0C\x29\x43\x68\x69\x6E\x61\x20\x49\x6E\x74"
    "\x65\x72\x6E\x65\x74\x20\x4E\x65\x74\x77\x6F\x72\x6B\x20\x49\x6E\x66\x6F"
    "\x72\x6D\x61\x74\x69\x6F\x6E\x20\x43\x65\x6E\x74\x65\x72\x31\x47\x30\x45"
    "\x06\x03\x55\x04\x03\x0C\x3E\x43\x68\x69\x6E\x61\x20\x49\x6E\x74\x65\x72"
    "\x6E\x65\x74\x20\x4E\x65\x74\x77\x6F\x72\x6B\x20\x49\x6E\x66\x6F\x72\x6D"
    "\x61\x74\x69\x6F\x6E\x20\x43\x65\x6E\x74\x65\x72\x20\x45\x56\x20\x43\x65"
    "\x72\x74\x69\x66\x69\x63\x61\x74\x65\x73\x20\x52\x6F\x6F\x74";
static const uint8_t* cnnic_ev_p = CNNIC_EV_ROOT_CA_SUBJECT_DATA;
static X509_NAME *cnnic_ev_name =
    d2i_X509_NAME(nullptr, &cnnic_ev_p,
                  sizeof(CNNIC_EV_ROOT_CA_SUBJECT_DATA)-1);

static Mutex* mutexes;

const char* const root_certs[] = {
#include "node_root_certs.h"  // NOLINT(build/include_order)
};

X509_STORE* root_cert_store;

// Just to generate static methods
template class SSLWrap<TLSWrap>;
template void SSLWrap<TLSWrap>::AddMethods(Environment* env,
                                           Local<FunctionTemplate> t);
template void SSLWrap<TLSWrap>::InitNPN(SecureContext* sc);
template void SSLWrap<TLSWrap>::SetSNIContext(SecureContext* sc);
template int SSLWrap<TLSWrap>::SetCACerts(SecureContext* sc);
template SSL_SESSION* SSLWrap<TLSWrap>::GetSessionCallback(
    SSL* s,
    unsigned char* key,
    int len,
    int* copy);
template int SSLWrap<TLSWrap>::NewSessionCallback(SSL* s,
                                                  SSL_SESSION* sess);
template void SSLWrap<TLSWrap>::OnClientHello(
    void* arg,
    const ClientHelloParser::ClientHello& hello);

#ifdef OPENSSL_NPN_NEGOTIATED
template int SSLWrap<TLSWrap>::AdvertiseNextProtoCallback(
    SSL* s,
    const unsigned char** data,
    unsigned int* len,
    void* arg);
template int SSLWrap<TLSWrap>::SelectNextProtoCallback(
    SSL* s,
    unsigned char** out,
    unsigned char* outlen,
    const unsigned char* in,
    unsigned int inlen,
    void* arg);
#endif

#ifdef NODE__HAVE_TLSEXT_STATUS_CB
template int SSLWrap<TLSWrap>::TLSExtStatusCallback(SSL* s, void* arg);
#endif

template void SSLWrap<TLSWrap>::DestroySSL();
template int SSLWrap<TLSWrap>::SSLCertCallback(SSL* s, void* arg);
template void SSLWrap<TLSWrap>::WaitForCertCb(CertCb cb, void* arg);

#ifdef TLSEXT_TYPE_application_layer_protocol_negotiation
template int SSLWrap<TLSWrap>::SelectALPNCallback(
    SSL* s,
    const unsigned char** out,
    unsigned char* outlen,
    const unsigned char* in,
    unsigned int inlen,
    void* arg);
#endif  // TLSEXT_TYPE_application_layer_protocol_negotiation

static void crypto_threadid_cb(CRYPTO_THREADID* tid) {
  static_assert(sizeof(uv_thread_t) <= sizeof(void*),
                "uv_thread_t does not fit in a pointer");
  CRYPTO_THREADID_set_pointer(tid, reinterpret_cast<void*>(uv_thread_self()));
}


static void crypto_lock_init(void) {
  mutexes = new Mutex[CRYPTO_num_locks()];
}


static void crypto_lock_cb(int mode, int n, const char* file, int line) {
  CHECK(!(mode & CRYPTO_LOCK) ^ !(mode & CRYPTO_UNLOCK));
  CHECK(!(mode & CRYPTO_READ) ^ !(mode & CRYPTO_WRITE));

  auto mutex = &mutexes[n];
  if (mode & CRYPTO_LOCK)
    mutex->Lock();
  else
    mutex->Unlock();
}


static int CryptoPemCallback(char *buf, int size, int rwflag, void *u) {
  if (u) {
    size_t buflen = static_cast<size_t>(size);
    size_t len = strlen(static_cast<const char*>(u));
    len = len > buflen ? buflen : len;
    memcpy(buf, u, len);
    return len;
  }

  return 0;
}


void ThrowCryptoError(Environment* env,
                      unsigned long err,  // NOLINT(runtime/int)
                      const char* default_message = nullptr) {
  HandleScope scope(env->isolate());
  if (err != 0 || default_message == nullptr) {
    char errmsg[128] = { 0 };
    ERR_error_string_n(err, errmsg, sizeof(errmsg));
    env->ThrowError(errmsg);
  } else {
    env->ThrowError(default_message);
  }
}


// Ensure that OpenSSL has enough entropy (at least 256 bits) for its PRNG.
// The entropy pool starts out empty and needs to fill up before the PRNG
// can be used securely.  Once the pool is filled, it never dries up again;
// its contents is stirred and reused when necessary.
//
// OpenSSL normally fills the pool automatically but not when someone starts
// generating random numbers before the pool is full: in that case OpenSSL
// keeps lowering the entropy estimate to thwart attackers trying to guess
// the initial state of the PRNG.
//
// When that happens, we will have to wait until enough entropy is available.
// That should normally never take longer than a few milliseconds.
//
// OpenSSL draws from /dev/random and /dev/urandom.  While /dev/random may
// block pending "true" randomness, /dev/urandom is a CSPRNG that doesn't
// block under normal circumstances.
//
// The only time when /dev/urandom may conceivably block is right after boot,
// when the whole system is still low on entropy.  That's not something we can
// do anything about.
inline void CheckEntropy() {
  for (;;) {
    int status = RAND_status();
    CHECK_GE(status, 0);  // Cannot fail.
    if (status != 0)
      break;

    // Give up, RAND_poll() not supported.
    if (RAND_poll() == 0)
      break;
  }
}


bool EntropySource(unsigned char* buffer, size_t length) {
  // Ensure that OpenSSL's PRNG is properly seeded.
  CheckEntropy();
  // RAND_bytes() can return 0 to indicate that the entropy data is not truly
  // random. That's okay, it's still better than V8's stock source of entropy,
  // which is /dev/urandom on UNIX platforms and the current time on Windows.
  return RAND_bytes(buffer, length) != -1;
}


void SecureContext::Initialize(Environment* env, Local<Object> target) {
  Local<FunctionTemplate> t = env->NewFunctionTemplate(SecureContext::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(), "SecureContext"));

  env->SetProtoMethod(t, "init", SecureContext::Init);
  env->SetProtoMethod(t, "setKey", SecureContext::SetKey);
  env->SetProtoMethod(t, "setCert", SecureContext::SetCert);
  env->SetProtoMethod(t, "addCACert", SecureContext::AddCACert);
  env->SetProtoMethod(t, "addCRL", SecureContext::AddCRL);
  env->SetProtoMethod(t, "addRootCerts", SecureContext::AddRootCerts);
  env->SetProtoMethod(t, "setCiphers", SecureContext::SetCiphers);
  env->SetProtoMethod(t, "setECDHCurve", SecureContext::SetECDHCurve);
  env->SetProtoMethod(t, "setDHParam", SecureContext::SetDHParam);
  env->SetProtoMethod(t, "setOptions", SecureContext::SetOptions);
  env->SetProtoMethod(t, "setSessionIdContext",
                      SecureContext::SetSessionIdContext);
  env->SetProtoMethod(t, "setSessionTimeout",
                      SecureContext::SetSessionTimeout);
  env->SetProtoMethod(t, "close", SecureContext::Close);
  env->SetProtoMethod(t, "loadPKCS12", SecureContext::LoadPKCS12);
  env->SetProtoMethod(t, "getTicketKeys", SecureContext::GetTicketKeys);
  env->SetProtoMethod(t, "setTicketKeys", SecureContext::SetTicketKeys);
  env->SetProtoMethod(t, "setFreeListLength", SecureContext::SetFreeListLength);
  env->SetProtoMethod(t,
                      "enableTicketKeyCallback",
                      SecureContext::EnableTicketKeyCallback);
  env->SetProtoMethod(t, "getCertificate", SecureContext::GetCertificate<true>);
  env->SetProtoMethod(t, "getIssuer", SecureContext::GetCertificate<false>);

  t->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "kTicketKeyReturnIndex"),
         Integer::NewFromUnsigned(env->isolate(), kTicketKeyReturnIndex));
  t->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "kTicketKeyHMACIndex"),
         Integer::NewFromUnsigned(env->isolate(), kTicketKeyHMACIndex));
  t->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "kTicketKeyAESIndex"),
         Integer::NewFromUnsigned(env->isolate(), kTicketKeyAESIndex));
  t->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "kTicketKeyNameIndex"),
         Integer::NewFromUnsigned(env->isolate(), kTicketKeyNameIndex));
  t->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "kTicketKeyIVIndex"),
         Integer::NewFromUnsigned(env->isolate(), kTicketKeyIVIndex));

  t->PrototypeTemplate()->SetAccessor(
      FIXED_ONE_BYTE_STRING(env->isolate(), "_external"),
      CtxGetter,
      nullptr,
      env->as_external(),
      DEFAULT,
      static_cast<PropertyAttribute>(ReadOnly | DontDelete),
      AccessorSignature::New(env->isolate(), t));

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "SecureContext"),
              t->GetFunction());
  env->set_secure_context_constructor_template(t);
}


void SecureContext::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  new SecureContext(env, args.This());
}


void SecureContext::Init(const FunctionCallbackInfo<Value>& args) {
  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());
  Environment* env = sc->env();

  const SSL_METHOD* method = SSLv23_method();

  if (args.Length() == 1 && args[0]->IsString()) {
    const node::Utf8Value sslmethod(env->isolate(), args[0]);

    // Note that SSLv2 and SSLv3 are disallowed but SSLv2_method and friends
    // are still accepted.  They are OpenSSL's way of saying that all known
    // protocols are supported unless explicitly disabled (which we do below
    // for SSLv2 and SSLv3.)
    if (strcmp(*sslmethod, "SSLv2_method") == 0) {
      return env->ThrowError("SSLv2 methods disabled");
    } else if (strcmp(*sslmethod, "SSLv2_server_method") == 0) {
      return env->ThrowError("SSLv2 methods disabled");
    } else if (strcmp(*sslmethod, "SSLv2_client_method") == 0) {
      return env->ThrowError("SSLv2 methods disabled");
    } else if (strcmp(*sslmethod, "SSLv3_method") == 0) {
      return env->ThrowError("SSLv3 methods disabled");
    } else if (strcmp(*sslmethod, "SSLv3_server_method") == 0) {
      return env->ThrowError("SSLv3 methods disabled");
    } else if (strcmp(*sslmethod, "SSLv3_client_method") == 0) {
      return env->ThrowError("SSLv3 methods disabled");
    } else if (strcmp(*sslmethod, "SSLv23_method") == 0) {
      method = SSLv23_method();
    } else if (strcmp(*sslmethod, "SSLv23_server_method") == 0) {
      method = SSLv23_server_method();
    } else if (strcmp(*sslmethod, "SSLv23_client_method") == 0) {
      method = SSLv23_client_method();
    } else if (strcmp(*sslmethod, "TLSv1_method") == 0) {
      method = TLSv1_method();
    } else if (strcmp(*sslmethod, "TLSv1_server_method") == 0) {
      method = TLSv1_server_method();
    } else if (strcmp(*sslmethod, "TLSv1_client_method") == 0) {
      method = TLSv1_client_method();
    } else if (strcmp(*sslmethod, "TLSv1_1_method") == 0) {
      method = TLSv1_1_method();
    } else if (strcmp(*sslmethod, "TLSv1_1_server_method") == 0) {
      method = TLSv1_1_server_method();
    } else if (strcmp(*sslmethod, "TLSv1_1_client_method") == 0) {
      method = TLSv1_1_client_method();
    } else if (strcmp(*sslmethod, "TLSv1_2_method") == 0) {
      method = TLSv1_2_method();
    } else if (strcmp(*sslmethod, "TLSv1_2_server_method") == 0) {
      method = TLSv1_2_server_method();
    } else if (strcmp(*sslmethod, "TLSv1_2_client_method") == 0) {
      method = TLSv1_2_client_method();
    } else {
      return env->ThrowError("Unknown method");
    }
  }

  sc->ctx_ = SSL_CTX_new(method);
  SSL_CTX_set_app_data(sc->ctx_, sc);

  // Disable SSLv2 in the case when method == SSLv23_method() and the
  // cipher list contains SSLv2 ciphers (not the default, should be rare.)
  // The bundled OpenSSL doesn't have SSLv2 support but the system OpenSSL may.
  // SSLv3 is disabled because it's susceptible to downgrade attacks (POODLE.)
  SSL_CTX_set_options(sc->ctx_, SSL_OP_NO_SSLv2);
  SSL_CTX_set_options(sc->ctx_, SSL_OP_NO_SSLv3);

  // SSL session cache configuration
  SSL_CTX_set_session_cache_mode(sc->ctx_,
                                 SSL_SESS_CACHE_SERVER |
                                 SSL_SESS_CACHE_NO_INTERNAL |
                                 SSL_SESS_CACHE_NO_AUTO_CLEAR);
  SSL_CTX_sess_set_get_cb(sc->ctx_, SSLWrap<Connection>::GetSessionCallback);
  SSL_CTX_sess_set_new_cb(sc->ctx_, SSLWrap<Connection>::NewSessionCallback);

  sc->ca_store_ = nullptr;
}


// Takes a string or buffer and loads it into a BIO.
// Caller responsible for BIO_free_all-ing the returned object.
static BIO* LoadBIO(Environment* env, Local<Value> v) {
  HandleScope scope(env->isolate());

  if (v->IsString()) {
    const node::Utf8Value s(env->isolate(), v);
    return NodeBIO::NewFixed(*s, s.length());
  }

  if (Buffer::HasInstance(v)) {
    return NodeBIO::NewFixed(Buffer::Data(v), Buffer::Length(v));
  }

  return nullptr;
}


void SecureContext::SetKey(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());

  unsigned int len = args.Length();
  if (len < 1) {
    return env->ThrowError("Private key argument is mandatory");
  }

  if (len > 2) {
    return env->ThrowError("Only private key and pass phrase are expected");
  }

  if (len == 2) {
    THROW_AND_RETURN_IF_NOT_STRING(args[1], "Pass phrase");
  }

  BIO *bio = LoadBIO(env, args[0]);
  if (!bio)
    return;

  node::Utf8Value passphrase(env->isolate(), args[1]);

  EVP_PKEY* key = PEM_read_bio_PrivateKey(bio,
                                          nullptr,
                                          CryptoPemCallback,
                                          len == 1 ? nullptr : *passphrase);

  if (!key) {
    BIO_free_all(bio);
    unsigned long err = ERR_get_error();  // NOLINT(runtime/int)
    if (!err) {
      return env->ThrowError("PEM_read_bio_PrivateKey");
    }
    return ThrowCryptoError(env, err);
  }

  int rv = SSL_CTX_use_PrivateKey(sc->ctx_, key);
  EVP_PKEY_free(key);
  BIO_free_all(bio);

  if (!rv) {
    unsigned long err = ERR_get_error();  // NOLINT(runtime/int)
    if (!err)
      return env->ThrowError("SSL_CTX_use_PrivateKey");
    return ThrowCryptoError(env, err);
  }
}


int SSL_CTX_get_issuer(SSL_CTX* ctx, X509* cert, X509** issuer) {
  int ret;

  X509_STORE* store = SSL_CTX_get_cert_store(ctx);
  X509_STORE_CTX store_ctx;

  ret = X509_STORE_CTX_init(&store_ctx, store, nullptr, nullptr);
  if (!ret)
    goto end;

  ret = X509_STORE_CTX_get1_issuer(issuer, &store_ctx, cert);
  X509_STORE_CTX_cleanup(&store_ctx);

 end:
  return ret;
}


int SSL_CTX_use_certificate_chain(SSL_CTX* ctx,
                                  X509* x,
                                  STACK_OF(X509)* extra_certs,
                                  X509** cert,
                                  X509** issuer) {
  CHECK_EQ(*issuer, nullptr);
  CHECK_EQ(*cert, nullptr);

  int ret = SSL_CTX_use_certificate(ctx, x);

  if (ret) {
    // If we could set up our certificate, now proceed to
    // the CA certificates.
    int r;

    SSL_CTX_clear_extra_chain_certs(ctx);

    for (int i = 0; i < sk_X509_num(extra_certs); i++) {
      X509* ca = sk_X509_value(extra_certs, i);

      // NOTE: Increments reference count on `ca`
      r = SSL_CTX_add1_chain_cert(ctx, ca);

      if (!r) {
        ret = 0;
        *issuer = nullptr;
        goto end;
      }
      // Note that we must not free r if it was successfully
      // added to the chain (while we must free the main
      // certificate, since its reference count is increased
      // by SSL_CTX_use_certificate).

      // Find issuer
      if (*issuer != nullptr || X509_check_issued(ca, x) != X509_V_OK)
        continue;

      *issuer = ca;
    }
  }

  // Try getting issuer from a cert store
  if (ret) {
    if (*issuer == nullptr) {
      ret = SSL_CTX_get_issuer(ctx, x, issuer);
      ret = ret < 0 ? 0 : 1;
      // NOTE: get_cert_store doesn't increment reference count,
      // no need to free `store`
    } else {
      // Increment issuer reference count
      *issuer = X509_dup(*issuer);
      if (*issuer == nullptr) {
        ret = 0;
        goto end;
      }
    }
  }

 end:
  if (ret && x != nullptr) {
    *cert = X509_dup(x);
    if (*cert == nullptr)
      ret = 0;
  }
  return ret;
}


// Read a file that contains our certificate in "PEM" format,
// possibly followed by a sequence of CA certificates that should be
// sent to the peer in the Certificate message.
//
// Taken from OpenSSL - edited for style.
int SSL_CTX_use_certificate_chain(SSL_CTX* ctx,
                                  BIO* in,
                                  X509** cert,
                                  X509** issuer) {
  X509* x = nullptr;

  // Just to ensure that `ERR_peek_last_error` below will return only errors
  // that we are interested in
  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(in, nullptr, CryptoPemCallback, nullptr);

  if (x == nullptr) {
    SSLerr(SSL_F_SSL_CTX_USE_CERTIFICATE_CHAIN_FILE, ERR_R_PEM_LIB);
    return 0;
  }

  X509* extra = nullptr;
  int ret = 0;
  unsigned long err = 0;  // NOLINT(runtime/int)

  // Read extra certs
  STACK_OF(X509)* extra_certs = sk_X509_new_null();
  if (extra_certs == nullptr) {
    SSLerr(SSL_F_SSL_CTX_USE_CERTIFICATE_CHAIN_FILE, ERR_R_MALLOC_FAILURE);
    goto done;
  }

  while ((extra = PEM_read_bio_X509(in, nullptr, CryptoPemCallback, nullptr))) {
    if (sk_X509_push(extra_certs, extra))
      continue;

    // Failure, free all certs
    goto done;
  }
  extra = nullptr;

  // When the while loop ends, it's usually just EOF.
  err = ERR_peek_last_error();
  if (ERR_GET_LIB(err) == ERR_LIB_PEM &&
      ERR_GET_REASON(err) == PEM_R_NO_START_LINE) {
    ERR_clear_error();
  } else  {
    // some real error
    goto done;
  }

  ret = SSL_CTX_use_certificate_chain(ctx, x, extra_certs, cert, issuer);
  if (!ret)
    goto done;

 done:
  if (extra_certs != nullptr)
    sk_X509_pop_free(extra_certs, X509_free);
  if (extra != nullptr)
    X509_free(extra);
  if (x != nullptr)
    X509_free(x);

  return ret;
}


void SecureContext::SetCert(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());

  if (args.Length() != 1) {
    return env->ThrowTypeError("Certificate argument is mandatory");
  }

  BIO* bio = LoadBIO(env, args[0]);
  if (!bio)
    return;

  // Free previous certs
  if (sc->issuer_ != nullptr) {
    X509_free(sc->issuer_);
    sc->issuer_ = nullptr;
  }
  if (sc->cert_ != nullptr) {
    X509_free(sc->cert_);
    sc->cert_ = nullptr;
  }

  int rv = SSL_CTX_use_certificate_chain(sc->ctx_,
                                         bio,
                                         &sc->cert_,
                                         &sc->issuer_);

  BIO_free_all(bio);

  if (!rv) {
    unsigned long err = ERR_get_error();  // NOLINT(runtime/int)
    if (!err) {
      return env->ThrowError("SSL_CTX_use_certificate_chain");
    }
    return ThrowCryptoError(env, err);
  }
}


void SecureContext::AddCACert(const FunctionCallbackInfo<Value>& args) {
  bool newCAStore = false;
  Environment* env = Environment::GetCurrent(args);

  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());
  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence compiler warning.

  if (args.Length() != 1) {
    return env->ThrowTypeError("CA certificate argument is mandatory");
  }

  if (!sc->ca_store_) {
    sc->ca_store_ = X509_STORE_new();
    newCAStore = true;
  }

  unsigned cert_count = 0;
  if (BIO* bio = LoadBIO(env, args[0])) {
    while (X509* x509 =
        PEM_read_bio_X509(bio, nullptr, CryptoPemCallback, nullptr)) {
      X509_STORE_add_cert(sc->ca_store_, x509);
      SSL_CTX_add_client_CA(sc->ctx_, x509);
      X509_free(x509);
      cert_count += 1;
    }
    BIO_free_all(bio);
  }

  if (cert_count > 0 && newCAStore) {
    SSL_CTX_set_cert_store(sc->ctx_, sc->ca_store_);
  }
}


void SecureContext::AddCRL(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());

  if (args.Length() != 1) {
    return env->ThrowTypeError("CRL argument is mandatory");
  }

  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence compiler warning.

  BIO *bio = LoadBIO(env, args[0]);
  if (!bio)
    return;

  X509_CRL *x509 =
      PEM_read_bio_X509_CRL(bio, nullptr, CryptoPemCallback, nullptr);

  if (x509 == nullptr) {
    BIO_free_all(bio);
    return;
  }

  X509_STORE_add_crl(sc->ca_store_, x509);
  X509_STORE_set_flags(sc->ca_store_, X509_V_FLAG_CRL_CHECK |
                                      X509_V_FLAG_CRL_CHECK_ALL);
  BIO_free_all(bio);
  X509_CRL_free(x509);
}



void SecureContext::AddRootCerts(const FunctionCallbackInfo<Value>& args) {
  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());
  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence compiler warning.

  CHECK_EQ(sc->ca_store_, nullptr);

  if (!root_cert_store) {
    root_cert_store = X509_STORE_new();

    for (size_t i = 0; i < arraysize(root_certs); i++) {
      BIO* bp = NodeBIO::NewFixed(root_certs[i], strlen(root_certs[i]));
      if (bp == nullptr) {
        return;
      }

      X509 *x509 = PEM_read_bio_X509(bp, nullptr, CryptoPemCallback, nullptr);
      if (x509 == nullptr) {
        BIO_free_all(bp);
        return;
      }

      X509_STORE_add_cert(root_cert_store, x509);

      BIO_free_all(bp);
      X509_free(x509);
    }
  }

  sc->ca_store_ = root_cert_store;
  SSL_CTX_set_cert_store(sc->ctx_, sc->ca_store_);
}


void SecureContext::SetCiphers(const FunctionCallbackInfo<Value>& args) {
  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());
  Environment* env = sc->env();
  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence compiler warning.

  if (args.Length() != 1) {
    return env->ThrowTypeError("Ciphers argument is mandatory");
  }

  THROW_AND_RETURN_IF_NOT_STRING(args[0], "Ciphers");

  const node::Utf8Value ciphers(args.GetIsolate(), args[0]);
  SSL_CTX_set_cipher_list(sc->ctx_, *ciphers);
}


void SecureContext::SetECDHCurve(const FunctionCallbackInfo<Value>& args) {
  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());
  Environment* env = sc->env();

  if (args.Length() != 1)
    return env->ThrowTypeError("ECDH curve name argument is mandatory");

  THROW_AND_RETURN_IF_NOT_STRING(args[0], "ECDH curve name");

  node::Utf8Value curve(env->isolate(), args[0]);

  int nid = OBJ_sn2nid(*curve);

  if (nid == NID_undef)
    return env->ThrowTypeError("First argument should be a valid curve name");

  EC_KEY* ecdh = EC_KEY_new_by_curve_name(nid);

  if (ecdh == nullptr)
    return env->ThrowTypeError("First argument should be a valid curve name");

  SSL_CTX_set_options(sc->ctx_, SSL_OP_SINGLE_ECDH_USE);
  SSL_CTX_set_tmp_ecdh(sc->ctx_, ecdh);

  EC_KEY_free(ecdh);
}


void SecureContext::SetDHParam(const FunctionCallbackInfo<Value>& args) {
  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.This());
  Environment* env = sc->env();
  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence compiler warning.

  // Auto DH is not supported in openssl 1.0.1, so dhparam needs
  // to be specifed explicitly
  if (args.Length() != 1)
    return env->ThrowTypeError("DH argument is mandatory");

  // Invalid dhparam is silently discarded and DHE is no longer used.
  BIO* bio = LoadBIO(env, args[0]);
  if (!bio)
    return;

  DH* dh = PEM_read_bio_DHparams(bio, nullptr, nullptr, nullptr);
  BIO_free_all(bio);

  if (dh == nullptr)
    return;

  const int size = BN_num_bits(dh->p);
  if (size < 1024) {
    return env->ThrowError("DH parameter is less than 1024 bits");
  } else if (size < 2048) {
    args.GetReturnValue().Set(FIXED_ONE_BYTE_STRING(
        env->isolate(), "WARNING: DH parameter is less than 2048 bits"));
  }

  SSL_CTX_set_options(sc->ctx_, SSL_OP_SINGLE_DH_USE);
  int r = SSL_CTX_set_tmp_dh(sc->ctx_, dh);
  DH_free(dh);

  if (!r)
    return env->ThrowTypeError("Error setting temp DH parameter");
}


void SecureContext::SetOptions(const FunctionCallbackInfo<Value>& args) {
  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());

  if (args.Length() != 1 || !args[0]->IntegerValue()) {
    return sc->env()->ThrowTypeError("Options must be an integer value");
  }

  SSL_CTX_set_options(
      sc->ctx_,
      static_cast<long>(args[0]->IntegerValue()));  // NOLINT(runtime/int)
}


void SecureContext::SetSessionIdContext(
    const FunctionCallbackInfo<Value>& args) {
  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());
  Environment* env = sc->env();

  if (args.Length() != 1) {
    return env->ThrowTypeError("Session ID context argument is mandatory");
  }

  THROW_AND_RETURN_IF_NOT_STRING(args[0], "Session ID context");

  const node::Utf8Value sessionIdContext(args.GetIsolate(), args[0]);
  const unsigned char* sid_ctx =
      reinterpret_cast<const unsigned char*>(*sessionIdContext);
  unsigned int sid_ctx_len = sessionIdContext.length();

  int r = SSL_CTX_set_session_id_context(sc->ctx_, sid_ctx, sid_ctx_len);
  if (r == 1)
    return;

  BIO* bio;
  BUF_MEM* mem;
  Local<String> message;

  bio = BIO_new(BIO_s_mem());
  if (bio == nullptr) {
    message = FIXED_ONE_BYTE_STRING(args.GetIsolate(),
                                    "SSL_CTX_set_session_id_context error");
  } else {
    ERR_print_errors(bio);
    BIO_get_mem_ptr(bio, &mem);
    message = OneByteString(args.GetIsolate(), mem->data, mem->length);
    BIO_free_all(bio);
  }

  args.GetIsolate()->ThrowException(Exception::TypeError(message));
}


void SecureContext::SetSessionTimeout(const FunctionCallbackInfo<Value>& args) {
  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());

  if (args.Length() != 1 || !args[0]->IsInt32()) {
    return sc->env()->ThrowTypeError(
        "Session timeout must be a 32-bit integer");
  }

  int32_t sessionTimeout = args[0]->Int32Value();
  SSL_CTX_set_timeout(sc->ctx_, sessionTimeout);
}


void SecureContext::Close(const FunctionCallbackInfo<Value>& args) {
  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());
  sc->FreeCTXMem();
}


// Takes .pfx or .p12 and password in string or buffer format
void SecureContext::LoadPKCS12(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  BIO* in = nullptr;
  PKCS12* p12 = nullptr;
  EVP_PKEY* pkey = nullptr;
  X509* cert = nullptr;
  STACK_OF(X509)* extra_certs = nullptr;
  char* pass = nullptr;
  bool ret = false;

  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args.Holder());
  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence compiler warning.

  if (args.Length() < 1) {
    return env->ThrowTypeError("PFX certificate argument is mandatory");
  }

  in = LoadBIO(env, args[0]);
  if (in == nullptr) {
    return env->ThrowError("Unable to load BIO");
  }

  if (args.Length() >= 2) {
    THROW_AND_RETURN_IF_NOT_BUFFER(args[1], "Pass phrase");
    size_t passlen = Buffer::Length(args[1]);
    pass = new char[passlen + 1];
    memcpy(pass, Buffer::Data(args[1]), passlen);
    pass[passlen] = '\0';
  }

  // Free previous certs
  if (sc->issuer_ != nullptr) {
    X509_free(sc->issuer_);
    sc->issuer_ = nullptr;
  }
  if (sc->cert_ != nullptr) {
    X509_free(sc->cert_);
    sc->cert_ = nullptr;
  }

  if (d2i_PKCS12_bio(in, &p12) &&
      PKCS12_parse(p12, pass, &pkey, &cert, &extra_certs) &&
      SSL_CTX_use_certificate_chain(sc->ctx_,
                                    cert,
                                    extra_certs,
                                    &sc->cert_,
                                    &sc->issuer_) &&
      SSL_CTX_use_PrivateKey(sc->ctx_, pkey)) {
    // Add CA certs too
    for (int i = 0; i < sk_X509_num(extra_certs); i++) {
      X509* ca = sk_X509_value(extra_certs, i);

      if (!sc->ca_store_) {
        sc->ca_store_ = X509_STORE_new();
        SSL_CTX_set_cert_store(sc->ctx_, sc->ca_store_);
      }
      X509_STORE_add_cert(sc->ca_store_, ca);
      SSL_CTX_add_client_CA(sc->ctx_, ca);
    }
    ret = true;
  }

  if (pkey != nullptr)
    EVP_PKEY_free(pkey);
  if (cert != nullptr)
    X509_free(cert);
  if (extra_certs != nullptr)
    sk_X509_pop_free(extra_certs, X509_free);

  PKCS12_free(p12);
  BIO_free_all(in);
  delete[] pass;

  if (!ret) {
    unsigned long err = ERR_get_error();  // NOLINT(runtime/int)
    const char* str = ERR_reason_error_string(err);
    return env->ThrowError(str);
  }
}


void SecureContext::GetTicketKeys(const FunctionCallbackInfo<Value>& args) {
#if !defined(OPENSSL_NO_TLSEXT) && defined(SSL_CTX_get_tlsext_ticket_keys)

  SecureContext* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  Local<Object> buff = Buffer::New(wrap->env(), 48).ToLocalChecked();
  if (SSL_CTX_get_tlsext_ticket_keys(wrap->ctx_,
                                     Buffer::Data(buff),
                                     Buffer::Length(buff)) != 1) {
    return wrap->env()->ThrowError("Failed to fetch tls ticket keys");
  }

  args.GetReturnValue().Set(buff);
#endif  // !def(OPENSSL_NO_TLSEXT) && def(SSL_CTX_get_tlsext_ticket_keys)
}


void SecureContext::SetTicketKeys(const FunctionCallbackInfo<Value>& args) {
#if !defined(OPENSSL_NO_TLSEXT) && defined(SSL_CTX_get_tlsext_ticket_keys)
  SecureContext* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());
  Environment* env = wrap->env();

  if (args.Length() < 1) {
    return env->ThrowTypeError("Ticket keys argument is mandatory");
  }

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Ticket keys");

  if (Buffer::Length(args[0]) != 48) {
    return env->ThrowTypeError("Ticket keys length must be 48 bytes");
  }

  if (SSL_CTX_set_tlsext_ticket_keys(wrap->ctx_,
                                     Buffer::Data(args[0]),
                                     Buffer::Length(args[0])) != 1) {
    return env->ThrowError("Failed to fetch tls ticket keys");
  }

  args.GetReturnValue().Set(true);
#endif  // !def(OPENSSL_NO_TLSEXT) && def(SSL_CTX_get_tlsext_ticket_keys)
}


void SecureContext::SetFreeListLength(const FunctionCallbackInfo<Value>& args) {
  SecureContext* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  wrap->ctx_->freelist_max_len = args[0]->Int32Value();
}


void SecureContext::EnableTicketKeyCallback(
    const FunctionCallbackInfo<Value>& args) {
  SecureContext* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  SSL_CTX_set_tlsext_ticket_key_cb(wrap->ctx_, TicketKeyCallback);
}


int SecureContext::TicketKeyCallback(SSL* ssl,
                                     unsigned char* name,
                                     unsigned char* iv,
                                     EVP_CIPHER_CTX* ectx,
                                     HMAC_CTX* hctx,
                                     int enc) {
  static const int kTicketPartSize = 16;

  SecureContext* sc = static_cast<SecureContext*>(
      SSL_CTX_get_app_data(SSL_get_SSL_CTX(ssl)));

  Environment* env = sc->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  Local<Value> argv[] = {
    Buffer::Copy(env,
                 reinterpret_cast<char*>(name),
                 kTicketPartSize).ToLocalChecked(),
    Buffer::Copy(env,
                 reinterpret_cast<char*>(iv),
                 kTicketPartSize).ToLocalChecked(),
    Boolean::New(env->isolate(), enc != 0)
  };
  Local<Value> ret = node::MakeCallback(env,
                                        sc->object(),
                                        env->ticketkeycallback_string(),
                                        arraysize(argv),
                                        argv);
  Local<Array> arr = ret.As<Array>();

  int r = arr->Get(kTicketKeyReturnIndex)->Int32Value();
  if (r < 0)
    return r;

  Local<Value> hmac = arr->Get(kTicketKeyHMACIndex);
  Local<Value> aes = arr->Get(kTicketKeyAESIndex);
  if (Buffer::Length(aes) != kTicketPartSize)
    return -1;

  if (enc) {
    Local<Value> name_val = arr->Get(kTicketKeyNameIndex);
    Local<Value> iv_val = arr->Get(kTicketKeyIVIndex);

    if (Buffer::Length(name_val) != kTicketPartSize ||
        Buffer::Length(iv_val) != kTicketPartSize) {
      return -1;
    }

    memcpy(name, Buffer::Data(name_val), kTicketPartSize);
    memcpy(iv, Buffer::Data(iv_val), kTicketPartSize);
  }

  HMAC_Init_ex(hctx,
               Buffer::Data(hmac),
               Buffer::Length(hmac),
               EVP_sha256(),
               nullptr);

  const unsigned char* aes_key =
      reinterpret_cast<unsigned char*>(Buffer::Data(aes));
  if (enc) {
    EVP_EncryptInit_ex(ectx,
                       EVP_aes_128_cbc(),
                       nullptr,
                       aes_key,
                       iv);
  } else {
    EVP_DecryptInit_ex(ectx,
                       EVP_aes_128_cbc(),
                       nullptr,
                       aes_key,
                       iv);
  }

  return r;
}




void SecureContext::CtxGetter(Local<String> property,
                              const PropertyCallbackInfo<Value>& info) {
  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, info.This());
  Local<External> ext = External::New(info.GetIsolate(), sc->ctx_);
  info.GetReturnValue().Set(ext);
}


template <bool primary>
void SecureContext::GetCertificate(const FunctionCallbackInfo<Value>& args) {
  SecureContext* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());
  Environment* env = wrap->env();
  X509* cert;

  if (primary)
    cert = wrap->cert_;
  else
    cert = wrap->issuer_;
  if (cert == nullptr)
    return args.GetReturnValue().Set(Null(env->isolate()));

  int size = openssl_i2d_X509(cert, nullptr);
  Local<Object> buff = Buffer::New(env, size).ToLocalChecked();
  unsigned char* serialized = reinterpret_cast<unsigned char*>(
      Buffer::Data(buff));
  openssl_i2d_X509(cert, &serialized);

  args.GetReturnValue().Set(buff);
}


template <class Base>
void SSLWrap<Base>::AddMethods(Environment* env, Local<FunctionTemplate> t) {
  HandleScope scope(env->isolate());

  env->SetProtoMethod(t, "getPeerCertificate", GetPeerCertificate);
  env->SetProtoMethod(t, "getSession", GetSession);
  env->SetProtoMethod(t, "setSession", SetSession);
  env->SetProtoMethod(t, "loadSession", LoadSession);
  env->SetProtoMethod(t, "isSessionReused", IsSessionReused);
  env->SetProtoMethod(t, "isInitFinished", IsInitFinished);
  env->SetProtoMethod(t, "verifyError", VerifyError);
  env->SetProtoMethod(t, "getCurrentCipher", GetCurrentCipher);
  env->SetProtoMethod(t, "endParser", EndParser);
  env->SetProtoMethod(t, "certCbDone", CertCbDone);
  env->SetProtoMethod(t, "renegotiate", Renegotiate);
  env->SetProtoMethod(t, "shutdownSSL", Shutdown);
  env->SetProtoMethod(t, "getTLSTicket", GetTLSTicket);
  env->SetProtoMethod(t, "newSessionDone", NewSessionDone);
  env->SetProtoMethod(t, "setOCSPResponse", SetOCSPResponse);
  env->SetProtoMethod(t, "requestOCSP", RequestOCSP);
  env->SetProtoMethod(t, "getEphemeralKeyInfo", GetEphemeralKeyInfo);
  env->SetProtoMethod(t, "getProtocol", GetProtocol);

#ifdef SSL_set_max_send_fragment
  env->SetProtoMethod(t, "setMaxSendFragment", SetMaxSendFragment);
#endif  // SSL_set_max_send_fragment

#ifdef OPENSSL_NPN_NEGOTIATED
  env->SetProtoMethod(t, "getNegotiatedProtocol", GetNegotiatedProto);
#endif  // OPENSSL_NPN_NEGOTIATED

#ifdef OPENSSL_NPN_NEGOTIATED
  env->SetProtoMethod(t, "setNPNProtocols", SetNPNProtocols);
#endif

  env->SetProtoMethod(t, "getALPNNegotiatedProtocol", GetALPNNegotiatedProto);
  env->SetProtoMethod(t, "setALPNProtocols", SetALPNProtocols);

  t->PrototypeTemplate()->SetAccessor(
      FIXED_ONE_BYTE_STRING(env->isolate(), "_external"),
      SSLGetter,
      nullptr,
      env->as_external(),
      DEFAULT,
      static_cast<PropertyAttribute>(ReadOnly | DontDelete),
      AccessorSignature::New(env->isolate(), t));
}


template <class Base>
void SSLWrap<Base>::InitNPN(SecureContext* sc) {
#ifdef OPENSSL_NPN_NEGOTIATED
  // Server should advertise NPN protocols
  SSL_CTX_set_next_protos_advertised_cb(sc->ctx_,
                                        AdvertiseNextProtoCallback,
                                        nullptr);
  // Client should select protocol from list of advertised
  // If server supports NPN
  SSL_CTX_set_next_proto_select_cb(sc->ctx_, SelectNextProtoCallback, nullptr);
#endif  // OPENSSL_NPN_NEGOTIATED

#ifdef NODE__HAVE_TLSEXT_STATUS_CB
  // OCSP stapling
  SSL_CTX_set_tlsext_status_cb(sc->ctx_, TLSExtStatusCallback);
  SSL_CTX_set_tlsext_status_arg(sc->ctx_, nullptr);
#endif  // NODE__HAVE_TLSEXT_STATUS_CB
}


template <class Base>
SSL_SESSION* SSLWrap<Base>::GetSessionCallback(SSL* s,
                                               unsigned char* key,
                                               int len,
                                               int* copy) {
  Base* w = static_cast<Base*>(SSL_get_app_data(s));

  *copy = 0;
  SSL_SESSION* sess = w->next_sess_;
  w->next_sess_ = nullptr;

  return sess;
}


template <class Base>
int SSLWrap<Base>::NewSessionCallback(SSL* s, SSL_SESSION* sess) {
  Base* w = static_cast<Base*>(SSL_get_app_data(s));
  Environment* env = w->ssl_env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  if (!w->session_callbacks_)
    return 0;

  // Check if session is small enough to be stored
  int size = i2d_SSL_SESSION(sess, nullptr);
  if (size > SecureContext::kMaxSessionSize)
    return 0;

  // Serialize session
  Local<Object> buff = Buffer::New(env, size).ToLocalChecked();
  unsigned char* serialized = reinterpret_cast<unsigned char*>(
      Buffer::Data(buff));
  memset(serialized, 0, size);
  i2d_SSL_SESSION(sess, &serialized);

  Local<Object> session = Buffer::Copy(
      env,
      reinterpret_cast<char*>(sess->session_id),
      sess->session_id_length).ToLocalChecked();
  Local<Value> argv[] = { session, buff };
  w->new_session_wait_ = true;
  w->MakeCallback(env->onnewsession_string(), arraysize(argv), argv);

  return 0;
}


template <class Base>
void SSLWrap<Base>::OnClientHello(void* arg,
                                  const ClientHelloParser::ClientHello& hello) {
  Base* w = static_cast<Base*>(arg);
  Environment* env = w->ssl_env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  Local<Object> hello_obj = Object::New(env->isolate());
  Local<Object> buff = Buffer::Copy(
      env,
      reinterpret_cast<const char*>(hello.session_id()),
      hello.session_size()).ToLocalChecked();
  hello_obj->Set(env->session_id_string(), buff);
  if (hello.servername() == nullptr) {
    hello_obj->Set(env->servername_string(), String::Empty(env->isolate()));
  } else {
    Local<String> servername = OneByteString(env->isolate(),
                                             hello.servername(),
                                             hello.servername_size());
    hello_obj->Set(env->servername_string(), servername);
  }
  hello_obj->Set(env->tls_ticket_string(),
                 Boolean::New(env->isolate(), hello.has_ticket()));
  hello_obj->Set(env->ocsp_request_string(),
                 Boolean::New(env->isolate(), hello.ocsp_request()));

  Local<Value> argv[] = { hello_obj };
  w->MakeCallback(env->onclienthello_string(), arraysize(argv), argv);
}


static bool SafeX509ExtPrint(BIO* out, X509_EXTENSION* ext) {
  const X509V3_EXT_METHOD* method = X509V3_EXT_get(ext);

  if (method != X509V3_EXT_get_nid(NID_subject_alt_name))
    return false;

  const unsigned char* p = ext->value->data;
  GENERAL_NAMES* names = reinterpret_cast<GENERAL_NAMES*>(ASN1_item_d2i(
      NULL,
      &p,
      ext->value->length,
      ASN1_ITEM_ptr(method->it)));
  if (names == NULL)
    return false;

  for (int i = 0; i < sk_GENERAL_NAME_num(names); i++) {
    GENERAL_NAME* gen = sk_GENERAL_NAME_value(names, i);

    if (i != 0)
      BIO_write(out, ", ", 2);

    if (gen->type == GEN_DNS) {
      ASN1_IA5STRING* name = gen->d.dNSName;

      BIO_write(out, "DNS:", 4);
      BIO_write(out, name->data, name->length);
    } else {
      STACK_OF(CONF_VALUE)* nval = i2v_GENERAL_NAME(
          const_cast<X509V3_EXT_METHOD*>(method), gen, NULL);
      if (nval == NULL)
        return false;
      X509V3_EXT_val_prn(out, nval, 0, 0);
      sk_CONF_VALUE_pop_free(nval, X509V3_conf_free);
    }
  }
  sk_GENERAL_NAME_pop_free(names, GENERAL_NAME_free);

  return true;
}


static Local<Object> X509ToObject(Environment* env, X509* cert) {
  EscapableHandleScope scope(env->isolate());

  Local<Object> info = Object::New(env->isolate());

  BIO* bio = BIO_new(BIO_s_mem());
  BUF_MEM* mem;
  if (X509_NAME_print_ex(bio,
                         X509_get_subject_name(cert),
                         0,
                         X509_NAME_FLAGS) > 0) {
    BIO_get_mem_ptr(bio, &mem);
    info->Set(env->subject_string(),
              String::NewFromUtf8(env->isolate(), mem->data,
                                  String::kNormalString, mem->length));
  }
  (void) BIO_reset(bio);

  X509_NAME* issuer_name = X509_get_issuer_name(cert);
  if (X509_NAME_print_ex(bio, issuer_name, 0, X509_NAME_FLAGS) > 0) {
    BIO_get_mem_ptr(bio, &mem);
    info->Set(env->issuer_string(),
              String::NewFromUtf8(env->isolate(), mem->data,
                                  String::kNormalString, mem->length));
  }
  (void) BIO_reset(bio);

  int nids[] = { NID_subject_alt_name, NID_info_access };
  Local<String> keys[] = { env->subjectaltname_string(),
                           env->infoaccess_string() };
  CHECK_EQ(arraysize(nids), arraysize(keys));
  for (size_t i = 0; i < arraysize(nids); i++) {
    int index = X509_get_ext_by_NID(cert, nids[i], -1);
    if (index < 0)
      continue;

    X509_EXTENSION* ext;
    int rv;

    ext = X509_get_ext(cert, index);
    CHECK_NE(ext, nullptr);

    if (!SafeX509ExtPrint(bio, ext)) {
      rv = X509V3_EXT_print(bio, ext, 0, 0);
      CHECK_EQ(rv, 1);
    }

    BIO_get_mem_ptr(bio, &mem);
    info->Set(keys[i],
              String::NewFromUtf8(env->isolate(), mem->data,
                                  String::kNormalString, mem->length));

    (void) BIO_reset(bio);
  }

  EVP_PKEY* pkey = X509_get_pubkey(cert);
  RSA* rsa = nullptr;
  if (pkey != nullptr)
    rsa = EVP_PKEY_get1_RSA(pkey);

  if (rsa != nullptr) {
      BN_print(bio, rsa->n);
      BIO_get_mem_ptr(bio, &mem);
      info->Set(env->modulus_string(),
                String::NewFromUtf8(env->isolate(), mem->data,
                                    String::kNormalString, mem->length));
      (void) BIO_reset(bio);

      BN_ULONG exponent_word = BN_get_word(rsa->e);
      BIO_printf(bio, "0x%lx", exponent_word);

      BIO_get_mem_ptr(bio, &mem);
      info->Set(env->exponent_string(),
                String::NewFromUtf8(env->isolate(), mem->data,
                                    String::kNormalString, mem->length));
      (void) BIO_reset(bio);
  }

  if (pkey != nullptr) {
    EVP_PKEY_free(pkey);
    pkey = nullptr;
  }
  if (rsa != nullptr) {
    RSA_free(rsa);
    rsa = nullptr;
  }

  ASN1_TIME_print(bio, X509_get_notBefore(cert));
  BIO_get_mem_ptr(bio, &mem);
  info->Set(env->valid_from_string(),
            String::NewFromUtf8(env->isolate(), mem->data,
                                String::kNormalString, mem->length));
  (void) BIO_reset(bio);

  ASN1_TIME_print(bio, X509_get_notAfter(cert));
  BIO_get_mem_ptr(bio, &mem);
  info->Set(env->valid_to_string(),
            String::NewFromUtf8(env->isolate(), mem->data,
                                String::kNormalString, mem->length));
  BIO_free_all(bio);

  unsigned int md_size, i;
  unsigned char md[EVP_MAX_MD_SIZE];
  if (X509_digest(cert, EVP_sha1(), md, &md_size)) {
    const char hex[] = "0123456789ABCDEF";
    char fingerprint[EVP_MAX_MD_SIZE * 3];

    // TODO(indutny): Unify it with buffer's code
    for (i = 0; i < md_size; i++) {
      fingerprint[3*i] = hex[(md[i] & 0xf0) >> 4];
      fingerprint[(3*i)+1] = hex[(md[i] & 0x0f)];
      fingerprint[(3*i)+2] = ':';
    }

    if (md_size > 0) {
      fingerprint[(3*(md_size-1))+2] = '\0';
    } else {
      fingerprint[0] = '\0';
    }

    info->Set(env->fingerprint_string(),
              OneByteString(env->isolate(), fingerprint));
  }

  STACK_OF(ASN1_OBJECT)* eku = static_cast<STACK_OF(ASN1_OBJECT)*>(
      X509_get_ext_d2i(cert, NID_ext_key_usage, nullptr, nullptr));
  if (eku != nullptr) {
    Local<Array> ext_key_usage = Array::New(env->isolate());
    char buf[256];

    int j = 0;
    for (int i = 0; i < sk_ASN1_OBJECT_num(eku); i++) {
      if (OBJ_obj2txt(buf, sizeof(buf), sk_ASN1_OBJECT_value(eku, i), 1) >= 0)
        ext_key_usage->Set(j++, OneByteString(env->isolate(), buf));
    }

    sk_ASN1_OBJECT_pop_free(eku, ASN1_OBJECT_free);
    info->Set(env->ext_key_usage_string(), ext_key_usage);
  }

  if (ASN1_INTEGER* serial_number = X509_get_serialNumber(cert)) {
    if (BIGNUM* bn = ASN1_INTEGER_to_BN(serial_number, nullptr)) {
      if (char* buf = BN_bn2hex(bn)) {
        info->Set(env->serial_number_string(),
                  OneByteString(env->isolate(), buf));
        OPENSSL_free(buf);
      }
      BN_free(bn);
    }
  }
  
  // Raw DER certificate
  int size = openssl_i2d_X509(cert, nullptr);
  Local<Object> buff = Buffer::New(env, size).ToLocalChecked();
  unsigned char* serialized = reinterpret_cast<unsigned char*>(
      Buffer::Data(buff));
  openssl_i2d_X509(cert, &serialized);
  info->Set(env->raw_string(), buff);

  return scope.Escape(info);
}


// TODO(indutny): Split it into multiple smaller functions
template <class Base>
void SSLWrap<Base>::GetPeerCertificate(
    const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  Environment* env = w->ssl_env();

  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence unused variable warning.

  Local<Object> result;
  Local<Object> info;

  // NOTE: This is because of the odd OpenSSL behavior. On client `cert_chain`
  // contains the `peer_certificate`, but on server it doesn't
  X509* cert = w->is_server() ? SSL_get_peer_certificate(w->ssl_) : nullptr;
  STACK_OF(X509)* ssl_certs = SSL_get_peer_cert_chain(w->ssl_);
  STACK_OF(X509)* peer_certs = nullptr;
  if (cert == nullptr && ssl_certs == nullptr)
    goto done;

  if (cert == nullptr && sk_X509_num(ssl_certs) == 0)
    goto done;

  // Short result requested
  if (args.Length() < 1 || !args[0]->IsTrue()) {
    result = X509ToObject(env,
                          cert == nullptr ? sk_X509_value(ssl_certs, 0) : cert);
    goto done;
  }

  // Clone `ssl_certs`, because we are going to destruct it
  peer_certs = sk_X509_new(nullptr);
  if (cert != nullptr)
    sk_X509_push(peer_certs, cert);
  for (int i = 0; i < sk_X509_num(ssl_certs); i++) {
    cert = X509_dup(sk_X509_value(ssl_certs, i));
    if (cert == nullptr)
      goto done;
    if (!sk_X509_push(peer_certs, cert))
      goto done;
  }

  // First and main certificate
  cert = sk_X509_value(peer_certs, 0);
  result = X509ToObject(env, cert);
  info = result;

  // Put issuer inside the object
  cert = sk_X509_delete(peer_certs, 0);
  while (sk_X509_num(peer_certs) > 0) {
    int i;
    for (i = 0; i < sk_X509_num(peer_certs); i++) {
      X509* ca = sk_X509_value(peer_certs, i);
      if (X509_check_issued(ca, cert) != X509_V_OK)
        continue;

      Local<Object> ca_info = X509ToObject(env, ca);
      info->Set(env->issuercert_string(), ca_info);
      info = ca_info;

      // NOTE: Intentionally freeing cert that is not used anymore
      X509_free(cert);

      // Delete cert and continue aggregating issuers
      cert = sk_X509_delete(peer_certs, i);
      break;
    }

    // Issuer not found, break out of the loop
    if (i == sk_X509_num(peer_certs))
      break;
  }

  // Last certificate should be self-signed
  while (X509_check_issued(cert, cert) != X509_V_OK) {
    X509* ca;
    if (SSL_CTX_get_issuer(SSL_get_SSL_CTX(w->ssl_), cert, &ca) <= 0)
      break;

    Local<Object> ca_info = X509ToObject(env, ca);
    info->Set(env->issuercert_string(), ca_info);
    info = ca_info;

    // NOTE: Intentionally freeing cert that is not used anymore
    X509_free(cert);

    // Delete cert and continue aggregating issuers
    cert = ca;
  }

  // Self-issued certificate
  if (X509_check_issued(cert, cert) == X509_V_OK)
    info->Set(env->issuercert_string(), info);

  CHECK_NE(cert, nullptr);

 done:
  if (cert != nullptr)
    X509_free(cert);
  if (peer_certs != nullptr)
    sk_X509_pop_free(peer_certs, X509_free);
  if (result.IsEmpty())
    result = Object::New(env->isolate());
  args.GetReturnValue().Set(result);
}


template <class Base>
void SSLWrap<Base>::GetSession(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());

  SSL_SESSION* sess = SSL_get_session(w->ssl_);
  if (sess == nullptr)
    return;

  int slen = i2d_SSL_SESSION(sess, nullptr);
  CHECK_GT(slen, 0);

  char* sbuf = new char[slen];
  unsigned char* p = reinterpret_cast<unsigned char*>(sbuf);
  i2d_SSL_SESSION(sess, &p);
  args.GetReturnValue().Set(Encode(env->isolate(), sbuf, slen, BUFFER));
  delete[] sbuf;
}


template <class Base>
void SSLWrap<Base>::SetSession(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());

  if (args.Length() < 1) {
    return env->ThrowError("Session argument is mandatory");
  }

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Session");
  size_t slen = Buffer::Length(args[0]);
  char* sbuf = new char[slen];
  memcpy(sbuf, Buffer::Data(args[0]), slen);

  const unsigned char* p = reinterpret_cast<const unsigned char*>(sbuf);
  SSL_SESSION* sess = d2i_SSL_SESSION(nullptr, &p, slen);

  delete[] sbuf;

  if (sess == nullptr)
    return;

  int r = SSL_set_session(w->ssl_, sess);
  SSL_SESSION_free(sess);

  if (!r)
    return env->ThrowError("SSL_set_session error");
}


template <class Base>
void SSLWrap<Base>::LoadSession(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  Environment* env = w->ssl_env();

  if (args.Length() >= 1 && Buffer::HasInstance(args[0])) {
    ssize_t slen = Buffer::Length(args[0]);
    char* sbuf = Buffer::Data(args[0]);

    const unsigned char* p = reinterpret_cast<unsigned char*>(sbuf);
    SSL_SESSION* sess = d2i_SSL_SESSION(nullptr, &p, slen);

    // Setup next session and move hello to the BIO buffer
    if (w->next_sess_ != nullptr)
      SSL_SESSION_free(w->next_sess_);
    w->next_sess_ = sess;

    Local<Object> info = Object::New(env->isolate());
#ifndef OPENSSL_NO_TLSEXT
    if (sess->tlsext_hostname == nullptr) {
      info->Set(env->servername_string(), False(args.GetIsolate()));
    } else {
      info->Set(env->servername_string(),
                OneByteString(args.GetIsolate(), sess->tlsext_hostname));
    }
#endif
    args.GetReturnValue().Set(info);
  }
}


template <class Base>
void SSLWrap<Base>::IsSessionReused(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  bool yes = SSL_session_reused(w->ssl_);
  args.GetReturnValue().Set(yes);
}


template <class Base>
void SSLWrap<Base>::EndParser(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  w->hello_parser_.End();
}


template <class Base>
void SSLWrap<Base>::Renegotiate(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());

  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence unused variable warning.

  bool yes = SSL_renegotiate(w->ssl_) == 1;
  args.GetReturnValue().Set(yes);
}


template <class Base>
void SSLWrap<Base>::Shutdown(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());

  int rv = SSL_shutdown(w->ssl_);
  args.GetReturnValue().Set(rv);
}


template <class Base>
void SSLWrap<Base>::GetTLSTicket(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  Environment* env = w->ssl_env();

  SSL_SESSION* sess = SSL_get_session(w->ssl_);
  if (sess == nullptr || sess->tlsext_tick == nullptr)
    return;

  Local<Object> buff = Buffer::Copy(
      env,
      reinterpret_cast<char*>(sess->tlsext_tick),
      sess->tlsext_ticklen).ToLocalChecked();

  args.GetReturnValue().Set(buff);
}


template <class Base>
void SSLWrap<Base>::NewSessionDone(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  w->new_session_wait_ = false;
  w->NewSessionDoneCb();
}


template <class Base>
void SSLWrap<Base>::SetOCSPResponse(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
#ifdef NODE__HAVE_TLSEXT_STATUS_CB
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  Environment* env = w->env();

  if (args.Length() < 1)
    return env->ThrowTypeError("OCSP response argument is mandatory");

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "OCSP response");

  w->ocsp_response_.Reset(args.GetIsolate(), args[0].As<Object>());
#endif  // NODE__HAVE_TLSEXT_STATUS_CB
}


template <class Base>
void SSLWrap<Base>::RequestOCSP(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
#ifdef NODE__HAVE_TLSEXT_STATUS_CB
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());

  SSL_set_tlsext_status_type(w->ssl_, TLSEXT_STATUSTYPE_ocsp);
#endif  // NODE__HAVE_TLSEXT_STATUS_CB
}


template <class Base>
void SSLWrap<Base>::GetEphemeralKeyInfo(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  Environment* env = Environment::GetCurrent(args);

  CHECK_NE(w->ssl_, nullptr);

  // tmp key is available on only client
  if (w->is_server())
    return args.GetReturnValue().SetNull();

  Local<Object> info = Object::New(env->isolate());

  EVP_PKEY* key;

  if (SSL_get_server_tmp_key(w->ssl_, &key)) {
    switch (EVP_PKEY_id(key)) {
      case EVP_PKEY_DH:
        info->Set(env->type_string(),
                  FIXED_ONE_BYTE_STRING(env->isolate(), "DH"));
        info->Set(env->size_string(),
            Integer::New(env->isolate(), openssl_EVP_PKEY_bits(key)));
        break;
      case EVP_PKEY_EC:
        {
          EC_KEY* ec = EVP_PKEY_get1_EC_KEY(key);
          int nid = EC_GROUP_get_curve_name(EC_KEY_get0_group(ec));
          EC_KEY_free(ec);
          info->Set(env->type_string(),
                    FIXED_ONE_BYTE_STRING(env->isolate(), "ECDH"));
          info->Set(env->name_string(),
                    OneByteString(args.GetIsolate(), OBJ_nid2sn(nid)));
          info->Set(env->size_string(),
                    Integer::New(env->isolate(), openssl_EVP_PKEY_bits(key)));
        }
    }
    EVP_PKEY_free(key);
  }
  
  return args.GetReturnValue().Set(info);
}


#ifdef SSL_set_max_send_fragment
template <class Base>
void SSLWrap<Base>::SetMaxSendFragment(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  CHECK(args.Length() >= 1 && args[0]->IsNumber());

  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());

  int rv = SSL_set_max_send_fragment(w->ssl_, args[0]->Int32Value());
  args.GetReturnValue().Set(rv);
}
#endif  // SSL_set_max_send_fragment


template <class Base>
void SSLWrap<Base>::IsInitFinished(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  bool yes = SSL_is_init_finished(w->ssl_);
  args.GetReturnValue().Set(yes);
}


template <class Base>
void SSLWrap<Base>::VerifyError(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());

  // XXX(bnoordhuis) The UNABLE_TO_GET_ISSUER_CERT error when there is no
  // peer certificate is questionable but it's compatible with what was
  // here before.
  long x509_verify_error =  // NOLINT(runtime/int)
      X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT;
  if (X509* peer_cert = SSL_get_peer_certificate(w->ssl_)) {
    X509_free(peer_cert);
    x509_verify_error = SSL_get_verify_result(w->ssl_);
  }

  if (x509_verify_error == X509_V_OK)
    return args.GetReturnValue().SetNull();

  // XXX(bnoordhuis) X509_verify_cert_error_string() is not actually thread-safe
  // in the presence of invalid error codes.  Probably academical but something
  // to keep in mind if/when node ever grows multi-isolate capabilities.
  const char* reason = X509_verify_cert_error_string(x509_verify_error);
  const char* code = reason;
#define CASE_X509_ERR(CODE) case X509_V_ERR_##CODE: code = #CODE; break;
  switch (x509_verify_error) {
    CASE_X509_ERR(UNABLE_TO_GET_ISSUER_CERT)
    CASE_X509_ERR(UNABLE_TO_GET_CRL)
    CASE_X509_ERR(UNABLE_TO_DECRYPT_CERT_SIGNATURE)
    CASE_X509_ERR(UNABLE_TO_DECRYPT_CRL_SIGNATURE)
    CASE_X509_ERR(UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY)
    CASE_X509_ERR(CERT_SIGNATURE_FAILURE)
    CASE_X509_ERR(CRL_SIGNATURE_FAILURE)
    CASE_X509_ERR(CERT_NOT_YET_VALID)
    CASE_X509_ERR(CERT_HAS_EXPIRED)
    CASE_X509_ERR(CRL_NOT_YET_VALID)
    CASE_X509_ERR(CRL_HAS_EXPIRED)
    CASE_X509_ERR(ERROR_IN_CERT_NOT_BEFORE_FIELD)
    CASE_X509_ERR(ERROR_IN_CERT_NOT_AFTER_FIELD)
    CASE_X509_ERR(ERROR_IN_CRL_LAST_UPDATE_FIELD)
    CASE_X509_ERR(ERROR_IN_CRL_NEXT_UPDATE_FIELD)
    CASE_X509_ERR(OUT_OF_MEM)
    CASE_X509_ERR(DEPTH_ZERO_SELF_SIGNED_CERT)
    CASE_X509_ERR(SELF_SIGNED_CERT_IN_CHAIN)
    CASE_X509_ERR(UNABLE_TO_GET_ISSUER_CERT_LOCALLY)
    CASE_X509_ERR(UNABLE_TO_VERIFY_LEAF_SIGNATURE)
    CASE_X509_ERR(CERT_CHAIN_TOO_LONG)
    CASE_X509_ERR(CERT_REVOKED)
    CASE_X509_ERR(INVALID_CA)
    CASE_X509_ERR(PATH_LENGTH_EXCEEDED)
    CASE_X509_ERR(INVALID_PURPOSE)
    CASE_X509_ERR(CERT_UNTRUSTED)
    CASE_X509_ERR(CERT_REJECTED)
  }
#undef CASE_X509_ERR

  Isolate* isolate = args.GetIsolate();
  Local<String> reason_string = OneByteString(isolate, reason);
  Local<Value> exception_value = Exception::Error(reason_string);
  Local<Object> exception_object = exception_value->ToObject(isolate);
  exception_object->Set(FIXED_ONE_BYTE_STRING(isolate, "code"),
                        OneByteString(isolate, code));
  args.GetReturnValue().Set(exception_object);
}


template <class Base>
void SSLWrap<Base>::GetCurrentCipher(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  Environment* env = w->ssl_env();

  const SSL_CIPHER* c = SSL_get_current_cipher(w->ssl_);
  if (c == nullptr)
    return;

  Local<Object> info = Object::New(env->isolate());
  const char* cipher_name = SSL_CIPHER_get_name(c);
  info->Set(env->name_string(), OneByteString(args.GetIsolate(), cipher_name));
  const char* cipher_version = SSL_CIPHER_get_version(c);
  info->Set(env->version_string(),
            OneByteString(args.GetIsolate(), cipher_version));
  args.GetReturnValue().Set(info);
}


template <class Base>
void SSLWrap<Base>::GetProtocol(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());

  const char* tls_version = SSL_get_version(w->ssl_);
  args.GetReturnValue().Set(OneByteString(args.GetIsolate(), tls_version));
}


#ifdef OPENSSL_NPN_NEGOTIATED
template <class Base>
int SSLWrap<Base>::AdvertiseNextProtoCallback(SSL* s,
                                              const unsigned char** data,
                                              unsigned int* len,
                                              void* arg) {
  Base* w = static_cast<Base*>(SSL_get_app_data(s));
  Environment* env = w->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  auto npn_buffer =
      w->object()->GetPrivate(
          env->context(),
          env->npn_buffer_private_symbol()).ToLocalChecked();

  if (npn_buffer->IsUndefined()) {
    // No initialization - no NPN protocols
    *data = reinterpret_cast<const unsigned char*>("");
    *len = 0;
  } else {
    CHECK(Buffer::HasInstance(npn_buffer));
    *data = reinterpret_cast<const unsigned char*>(Buffer::Data(npn_buffer));
    *len = Buffer::Length(npn_buffer);
  }

  return SSL_TLSEXT_ERR_OK;
}


template <class Base>
int SSLWrap<Base>::SelectNextProtoCallback(SSL* s,
                                           unsigned char** out,
                                           unsigned char* outlen,
                                           const unsigned char* in,
                                           unsigned int inlen,
                                           void* arg) {
  Base* w = static_cast<Base*>(SSL_get_app_data(s));
  Environment* env = w->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  auto npn_buffer =
      w->object()->GetPrivate(
          env->context(),
          env->npn_buffer_private_symbol()).ToLocalChecked();

  if (npn_buffer->IsUndefined()) {
    // We should at least select one protocol
    // If server is using NPN
    *out = reinterpret_cast<unsigned char*>(const_cast<char*>("http/1.1"));
    *outlen = 8;

    // set status: unsupported
    CHECK(
        w->object()->SetPrivate(
            env->context(),
            env->selected_npn_buffer_private_symbol(),
            False(env->isolate())).FromJust());

    return SSL_TLSEXT_ERR_OK;
  }

  CHECK(Buffer::HasInstance(npn_buffer));
  const unsigned char* npn_protos =
      reinterpret_cast<const unsigned char*>(Buffer::Data(npn_buffer));
  size_t len = Buffer::Length(npn_buffer);

  int status = SSL_select_next_proto(out, outlen, in, inlen, npn_protos, len);
  Local<Value> result;
  switch (status) {
    case OPENSSL_NPN_UNSUPPORTED:
      result = Null(env->isolate());
      break;
    case OPENSSL_NPN_NEGOTIATED:
      result = OneByteString(env->isolate(), *out, *outlen);
      break;
    case OPENSSL_NPN_NO_OVERLAP:
      result = False(env->isolate());
      break;
    default:
      break;
  }

  CHECK(
      w->object()->SetPrivate(
          env->context(),
          env->selected_npn_buffer_private_symbol(),
          result).FromJust());

  return SSL_TLSEXT_ERR_OK;
}


template <class Base>
void SSLWrap<Base>::GetNegotiatedProto(
    const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  Environment* env = w->env();

  if (w->is_client()) {
    auto selected_npn_buffer =
        w->object()->GetPrivate(
            env->context(),
            env->selected_npn_buffer_private_symbol()).ToLocalChecked();
    args.GetReturnValue().Set(selected_npn_buffer);
    return;
  }

  const unsigned char* npn_proto;
  unsigned int npn_proto_len;

  SSL_get0_next_proto_negotiated(w->ssl_, &npn_proto, &npn_proto_len);

  if (!npn_proto)
    return args.GetReturnValue().Set(false);

  args.GetReturnValue().Set(
      OneByteString(args.GetIsolate(), npn_proto, npn_proto_len));
}


template <class Base>
void SSLWrap<Base>::SetNPNProtocols(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  Environment* env = w->env();

  if (args.Length() < 1)
    return env->ThrowTypeError("NPN protocols argument is mandatory");

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "NPN protocols");

  CHECK(
      w->object()->SetPrivate(
          env->context(),
          env->npn_buffer_private_symbol(),
          args[0]).FromJust());
}
#endif  // OPENSSL_NPN_NEGOTIATED

#ifdef TLSEXT_TYPE_application_layer_protocol_negotiation
template <class Base>
int SSLWrap<Base>::SelectALPNCallback(SSL* s,
                                      const unsigned char** out,
                                      unsigned char* outlen,
                                      const unsigned char* in,
                                      unsigned int inlen,
                                      void* arg) {
  Base* w = static_cast<Base*>(SSL_get_app_data(s));
  Environment* env = w->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  Local<Value> alpn_buffer =
      w->object()->GetPrivate(
          env->context(),
          env->alpn_buffer_private_symbol()).ToLocalChecked();
  CHECK(Buffer::HasInstance(alpn_buffer));
  const unsigned char* alpn_protos =
      reinterpret_cast<const unsigned char*>(Buffer::Data(alpn_buffer));
  unsigned alpn_protos_len = Buffer::Length(alpn_buffer);
  int status = SSL_select_next_proto(const_cast<unsigned char**>(out), outlen,
                                     alpn_protos, alpn_protos_len, in, inlen);

  switch (status) {
    case OPENSSL_NPN_NO_OVERLAP:
      // According to 3.2. Protocol Selection of RFC7301,
      // fatal no_application_protocol alert shall be sent
      // but current openssl does not support it yet. See
      // https://rt.openssl.org/Ticket/Display.html?id=3463&user=guest&pass=guest
      // Instead, we send a warning alert for now.
      return SSL_TLSEXT_ERR_ALERT_WARNING;
    case OPENSSL_NPN_NEGOTIATED:
      return SSL_TLSEXT_ERR_OK;
    default:
      return SSL_TLSEXT_ERR_ALERT_FATAL;
  }
}
#endif  // TLSEXT_TYPE_application_layer_protocol_negotiation


template <class Base>
void SSLWrap<Base>::GetALPNNegotiatedProto(
    const FunctionCallbackInfo<v8::Value>& args) {
#ifdef TLSEXT_TYPE_application_layer_protocol_negotiation
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());

  const unsigned char* alpn_proto;
  unsigned int alpn_proto_len;

  SSL_get0_alpn_selected(w->ssl_, &alpn_proto, &alpn_proto_len);

  if (!alpn_proto)
    return args.GetReturnValue().Set(false);

  args.GetReturnValue().Set(
      OneByteString(args.GetIsolate(), alpn_proto, alpn_proto_len));
#endif  // TLSEXT_TYPE_application_layer_protocol_negotiation
}


template <class Base>
void SSLWrap<Base>::SetALPNProtocols(
    const FunctionCallbackInfo<v8::Value>& args) {
#ifdef TLSEXT_TYPE_application_layer_protocol_negotiation
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  Environment* env = w->env();
  if (args.Length() < 1 || !Buffer::HasInstance(args[0]))
    return env->ThrowTypeError("Must give a Buffer as first argument");

  if (w->is_client()) {
    const unsigned char* alpn_protos =
        reinterpret_cast<const unsigned char*>(Buffer::Data(args[0]));
    unsigned alpn_protos_len = Buffer::Length(args[0]);
    int r = SSL_set_alpn_protos(w->ssl_, alpn_protos, alpn_protos_len);
    CHECK_EQ(r, 0);
  } else {
    CHECK(
        w->object()->SetPrivate(
          env->context(),
          env->alpn_buffer_private_symbol(),
          args[0]).FromJust());
    // Server should select ALPN protocol from list of advertised by client
    SSL_CTX_set_alpn_select_cb(SSL_get_SSL_CTX(w->ssl_), SelectALPNCallback,
                               nullptr);
  }
#endif  // TLSEXT_TYPE_application_layer_protocol_negotiation
}


#ifdef NODE__HAVE_TLSEXT_STATUS_CB
template <class Base>
int SSLWrap<Base>::TLSExtStatusCallback(SSL* s, void* arg) {
  Base* w = static_cast<Base*>(SSL_get_app_data(s));
  Environment* env = w->env();
  HandleScope handle_scope(env->isolate());

  if (w->is_client()) {
    // Incoming response
    const unsigned char* resp;
    int len = SSL_get_tlsext_status_ocsp_resp(s, &resp);
    Local<Value> arg;
    if (resp == nullptr) {
      arg = Null(env->isolate());
    } else {
      arg = Buffer::Copy(
          env,
          reinterpret_cast<char*>(const_cast<unsigned char*>(resp)),
          len).ToLocalChecked();
    }

    w->MakeCallback(env->onocspresponse_string(), 1, &arg);

    // Somehow, client is expecting different return value here
    return 1;
  } else {
    // Outgoing response
    if (w->ocsp_response_.IsEmpty())
      return SSL_TLSEXT_ERR_NOACK;

    Local<Object> obj = PersistentToLocal(env->isolate(), w->ocsp_response_);
    char* resp = Buffer::Data(obj);
    size_t len = Buffer::Length(obj);

    // OpenSSL takes control of the pointer after accepting it
    char* data = reinterpret_cast<char*>(node::Malloc(len));
    CHECK_NE(data, nullptr);
    memcpy(data, resp, len);

    if (!SSL_set_tlsext_status_ocsp_resp(s, data, len))
      free(data);
    w->ocsp_response_.Reset();

    return SSL_TLSEXT_ERR_OK;
  }
}
#endif  // NODE__HAVE_TLSEXT_STATUS_CB


template <class Base>
void SSLWrap<Base>::WaitForCertCb(CertCb cb, void* arg) {
  cert_cb_ = cb;
  cert_cb_arg_ = arg;
}


template <class Base>
int SSLWrap<Base>::SSLCertCallback(SSL* s, void* arg) {
  Base* w = static_cast<Base*>(SSL_get_app_data(s));

  if (!w->is_server())
    return 1;

  if (!w->is_waiting_cert_cb())
    return 1;

  if (w->cert_cb_running_)
    return -1;

  Environment* env = w->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());
  w->cert_cb_running_ = true;

  Local<Object> info = Object::New(env->isolate());

  SSL_SESSION* sess = SSL_get_session(s);
  if (sess != nullptr) {
    if (sess->tlsext_hostname == nullptr) {
      info->Set(env->servername_string(), String::Empty(env->isolate()));
    } else {
      Local<String> servername = OneByteString(env->isolate(),
                                               sess->tlsext_hostname,
                                               strlen(sess->tlsext_hostname));
      info->Set(env->servername_string(), servername);
    }
    info->Set(env->tls_ticket_string(),
              Boolean::New(env->isolate(), sess->tlsext_ticklen != 0));
  }

  bool ocsp = false;
#ifdef NODE__HAVE_TLSEXT_STATUS_CB
  ocsp = s->tlsext_status_type == TLSEXT_STATUSTYPE_ocsp;
#endif

  info->Set(env->ocsp_request_string(), Boolean::New(env->isolate(), ocsp));

  Local<Value> argv[] = { info };
  w->MakeCallback(env->oncertcb_string(), arraysize(argv), argv);

  if (!w->cert_cb_running_)
    return 1;

  // Performing async action, wait...
  return -1;
}


template <class Base>
void SSLWrap<Base>::CertCbDone(const FunctionCallbackInfo<Value>& args) {
  Base* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.Holder());
  Environment* env = w->env();

  CHECK(w->is_waiting_cert_cb() && w->cert_cb_running_);

  Local<Object> object = w->object();
  Local<Value> ctx = object->Get(env->sni_context_string());
  Local<FunctionTemplate> cons = env->secure_context_constructor_template();

  // Not an object, probably undefined or null
  if (!ctx->IsObject())
    goto fire_cb;

  if (cons->HasInstance(ctx)) {
    SecureContext* sc;
    ASSIGN_OR_RETURN_UNWRAP(&sc, ctx.As<Object>());
    w->sni_context_.Reset();
    w->sni_context_.Reset(env->isolate(), ctx);

    int rv;

    // NOTE: reference count is not increased by this API methods
    X509* x509 = SSL_CTX_get0_certificate(sc->ctx_);
    EVP_PKEY* pkey = SSL_CTX_get0_privatekey(sc->ctx_);
    STACK_OF(X509)* chain;

    rv = SSL_CTX_get0_chain_certs(sc->ctx_, &chain);
    if (rv)
      rv = SSL_use_certificate(w->ssl_, x509);
    if (rv)
      rv = SSL_use_PrivateKey(w->ssl_, pkey);
    if (rv && chain != nullptr)
      rv = SSL_set1_chain(w->ssl_, chain);
    if (rv)
      rv = w->SetCACerts(sc);
    if (!rv) {
      unsigned long err = ERR_get_error();  // NOLINT(runtime/int)
      if (!err)
        return env->ThrowError("CertCbDone");
      return ThrowCryptoError(env, err);
    }
  } else {
    // Failure: incorrect SNI context object
    Local<Value> err = Exception::TypeError(env->sni_context_err_string());
    w->MakeCallback(env->onerror_string(), 1, &err);
    return;
  }

 fire_cb:
  CertCb cb;
  void* arg;

  cb = w->cert_cb_;
  arg = w->cert_cb_arg_;

  w->cert_cb_running_ = false;
  w->cert_cb_ = nullptr;
  w->cert_cb_arg_ = nullptr;

  cb(arg);
}


template <class Base>
void SSLWrap<Base>::SSLGetter(Local<String> property,
                              const PropertyCallbackInfo<Value>& info) {
  Base* base;
  ASSIGN_OR_RETURN_UNWRAP(&base, info.This());
  SSL* ssl = base->ssl_;
  Local<External> ext = External::New(info.GetIsolate(), ssl);
  info.GetReturnValue().Set(ext);
}


template <class Base>
void SSLWrap<Base>::DestroySSL() {
  if (ssl_ == nullptr)
    return;

  SSL_free(ssl_);
  env_->isolate()->AdjustAmountOfExternalAllocatedMemory(-kExternalSize);
  ssl_ = nullptr;
}


template <class Base>
void SSLWrap<Base>::SetSNIContext(SecureContext* sc) {
  InitNPN(sc);
  CHECK_EQ(SSL_set_SSL_CTX(ssl_, sc->ctx_), sc->ctx_);

  SetCACerts(sc);
}


template <class Base>
int SSLWrap<Base>::SetCACerts(SecureContext* sc) {
  int err = SSL_set1_verify_cert_store(ssl_, SSL_CTX_get_cert_store(sc->ctx_));
  if (err != 1)
    return err;

  STACK_OF(X509_NAME)* list = SSL_dup_CA_list(
      SSL_CTX_get_client_CA_list(sc->ctx_));

  // NOTE: `SSL_set_client_CA_list` takes the ownership of `list`
  SSL_set_client_CA_list(ssl_, list);
  return 1;
}


void Connection::OnClientHelloParseEnd(void* arg) {
  Connection* conn = static_cast<Connection*>(arg);

  // Write all accumulated data
  int r = BIO_write(conn->bio_read_,
                    reinterpret_cast<char*>(conn->hello_data_),
                    conn->hello_offset_);
  conn->HandleBIOError(conn->bio_read_, "BIO_write", r);
  conn->SetShutdownFlags();
}


#ifdef SSL_PRINT_DEBUG
# define DEBUG_PRINT(...) fprintf (stderr, __VA_ARGS__)
#else
# define DEBUG_PRINT(...)
#endif


int Connection::HandleBIOError(BIO *bio, const char* func, int rv) {
  if (rv >= 0)
    return rv;

  int retry = BIO_should_retry(bio);
  (void) retry;  // unused if !defined(SSL_PRINT_DEBUG)

  if (BIO_should_write(bio)) {
    DEBUG_PRINT("[%p] BIO: %s want write. should retry %d\n",
                ssl_,
                func,
                retry);
    return 0;

  } else if (BIO_should_read(bio)) {
    DEBUG_PRINT("[%p] BIO: %s want read. should retry %d\n", ssl_, func, retry);
    return 0;

  } else {
    char ssl_error_buf[512];
    ERR_error_string_n(rv, ssl_error_buf, sizeof(ssl_error_buf));

    HandleScope scope(ssl_env()->isolate());
    Local<Value> exception =
        Exception::Error(OneByteString(ssl_env()->isolate(), ssl_error_buf));
    object()->Set(ssl_env()->error_string(), exception);

    DEBUG_PRINT("[%p] BIO: %s failed: (%d) %s\n",
                ssl_,
                func,
                rv,
                ssl_error_buf);

    return rv;
  }

  return 0;
}


int Connection::HandleSSLError(const char* func,
                               int rv,
                               ZeroStatus zs,
                               SyscallStatus ss) {
  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence unused variable warning.

  if (rv > 0)
    return rv;
  if (rv == 0 && zs == kZeroIsNotAnError)
    return rv;

  int err = SSL_get_error(ssl_, rv);

  if (err == SSL_ERROR_NONE) {
    return 0;

  } else if (err == SSL_ERROR_WANT_WRITE) {
    DEBUG_PRINT("[%p] SSL: %s want write\n", ssl_, func);
    return 0;

  } else if (err == SSL_ERROR_WANT_READ) {
    DEBUG_PRINT("[%p] SSL: %s want read\n", ssl_, func);
    return 0;

  } else if (err == SSL_ERROR_WANT_X509_LOOKUP) {
    DEBUG_PRINT("[%p] SSL: %s want x509 lookup\n", ssl_, func);
    return 0;

  } else if (err == SSL_ERROR_ZERO_RETURN) {
    HandleScope scope(ssl_env()->isolate());

    Local<Value> exception =
        Exception::Error(ssl_env()->zero_return_string());
    object()->Set(ssl_env()->error_string(), exception);
    return rv;

  } else if (err == SSL_ERROR_SYSCALL && ss == kIgnoreSyscall) {
    return 0;

  } else {
    HandleScope scope(ssl_env()->isolate());
    BUF_MEM* mem;
    BIO *bio;

    CHECK(err == SSL_ERROR_SSL || err == SSL_ERROR_SYSCALL);

    // XXX We need to drain the error queue for this thread or else OpenSSL
    // has the possibility of blocking connections? This problem is not well
    // understood. And we should be somehow propagating these errors up
    // into JavaScript. There is no test which demonstrates this problem.
    // https://github.com/joyent/node/issues/1719
    bio = BIO_new(BIO_s_mem());
    if (bio != nullptr) {
      ERR_print_errors(bio);
      BIO_get_mem_ptr(bio, &mem);
      Local<Value> exception = Exception::Error(
          OneByteString(ssl_env()->isolate(),
            mem->data,
            mem->length));
      object()->Set(ssl_env()->error_string(), exception);
      BIO_free_all(bio);
    }

    return rv;
  }

  return 0;
}


void Connection::ClearError() {
#ifndef NDEBUG
  HandleScope scope(ssl_env()->isolate());

  // We should clear the error in JS-land
  Local<String> error_key = ssl_env()->error_string();
  Local<Value> error = object()->Get(error_key);
  CHECK_EQ(error->BooleanValue(), false);
#endif  // NDEBUG
}


void Connection::SetShutdownFlags() {
  HandleScope scope(ssl_env()->isolate());

  int flags = SSL_get_shutdown(ssl_);

  if (flags & SSL_SENT_SHUTDOWN) {
    Local<String> sent_shutdown_key = ssl_env()->sent_shutdown_string();
    object()->Set(sent_shutdown_key, True(ssl_env()->isolate()));
  }

  if (flags & SSL_RECEIVED_SHUTDOWN) {
    Local<String> received_shutdown_key = ssl_env()->received_shutdown_string();
    object()->Set(received_shutdown_key, True(ssl_env()->isolate()));
  }
}


void Connection::NewSessionDoneCb() {
  HandleScope scope(env()->isolate());

  MakeCallback(env()->onnewsessiondone_string(), 0, nullptr);
}


void Connection::Initialize(Environment* env, Local<Object> target) {
  Local<FunctionTemplate> t = env->NewFunctionTemplate(Connection::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(), "Connection"));

  env->SetProtoMethod(t, "encIn", Connection::EncIn);
  env->SetProtoMethod(t, "clearOut", Connection::ClearOut);
  env->SetProtoMethod(t, "clearIn", Connection::ClearIn);
  env->SetProtoMethod(t, "encOut", Connection::EncOut);
  env->SetProtoMethod(t, "clearPending", Connection::ClearPending);
  env->SetProtoMethod(t, "encPending", Connection::EncPending);
  env->SetProtoMethod(t, "start", Connection::Start);
  env->SetProtoMethod(t, "close", Connection::Close);

  SSLWrap<Connection>::AddMethods(env, t);


#ifdef SSL_CTRL_SET_TLSEXT_SERVERNAME_CB
  env->SetProtoMethod(t, "getServername", Connection::GetServername);
  env->SetProtoMethod(t, "setSNICallback",  Connection::SetSNICallback);
#endif

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "Connection"),
              t->GetFunction());
}


inline int compar(const void* a, const void* b) {
  return memcmp(a, b, CNNIC_WHITELIST_HASH_LEN);
}


inline int IsSelfSigned(X509* cert) {
  return X509_NAME_cmp(X509_get_subject_name(cert),
                       X509_get_issuer_name(cert)) == 0;
}


inline X509* FindRoot(STACK_OF(X509)* sk) {
  for (int i = 0; i < sk_X509_num(sk); i++) {
    X509* cert = sk_X509_value(sk, i);
    if (IsSelfSigned(cert))
      return cert;
  }
  return nullptr;
}


// Whitelist check for certs issued by CNNIC. See
// https://blog.mozilla.org/security/2015/04/02
// /distrusting-new-cnnic-certificates/
inline CheckResult CheckWhitelistedServerCert(X509_STORE_CTX* ctx) {
  unsigned char hash[CNNIC_WHITELIST_HASH_LEN];
  unsigned int hashlen = CNNIC_WHITELIST_HASH_LEN;

  STACK_OF(X509)* chain = X509_STORE_CTX_get1_chain(ctx);
  CHECK_NE(chain, nullptr);
  CHECK_GT(sk_X509_num(chain), 0);

  // Take the last cert as root at the first time.
  X509* root_cert = sk_X509_value(chain, sk_X509_num(chain)-1);
  X509_NAME* root_name = X509_get_subject_name(root_cert);

  if (!IsSelfSigned(root_cert)) {
    root_cert = FindRoot(chain);
    CHECK_NE(root_cert, nullptr);
    root_name = X509_get_subject_name(root_cert);
  }

  // When the cert is issued from either CNNNIC ROOT CA or CNNNIC EV
  // ROOT CA, check a hash of its leaf cert if it is in the whitelist.
  if (X509_NAME_cmp(root_name, cnnic_name) == 0 ||
      X509_NAME_cmp(root_name, cnnic_ev_name) == 0) {
    X509* leaf_cert = sk_X509_value(chain, 0);
    int ret = X509_digest(leaf_cert, EVP_sha256(), hash,
                          &hashlen);
    CHECK(ret);

    void* result = bsearch(hash, WhitelistedCNNICHashes,
                           arraysize(WhitelistedCNNICHashes),
                           CNNIC_WHITELIST_HASH_LEN, compar);
    if (result == nullptr) {
      sk_X509_pop_free(chain, X509_free);
      return CHECK_CERT_REVOKED;
    }
  }

  sk_X509_pop_free(chain, X509_free);
  return CHECK_OK;
}


inline int VerifyCallback(int preverify_ok, X509_STORE_CTX* ctx) {
  // Failure on verification of the cert is handled in
  // Connection::VerifyError.
  if (preverify_ok == 0 || X509_STORE_CTX_get_error(ctx) != X509_V_OK)
    return 1;

  // Server does not need to check the whitelist.
  SSL* ssl = static_cast<SSL*>(
      X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx()));

  if (SSL_is_server(ssl))
    return 1;

  // Client needs to check if the server cert is listed in the
  // whitelist when it is issued by the specific rootCAs.
  CheckResult ret = CheckWhitelistedServerCert(ctx);
  if (ret == CHECK_CERT_REVOKED)
    X509_STORE_CTX_set_error(ctx, X509_V_ERR_CERT_REVOKED);

  return ret;
}


#ifdef SSL_CTRL_SET_TLSEXT_SERVERNAME_CB
int Connection::SelectSNIContextCallback_(SSL *s, int *ad, void* arg) {
  Connection* conn = static_cast<Connection*>(SSL_get_app_data(s));
  Environment* env = conn->env();
  HandleScope scope(env->isolate());

  const char* servername = SSL_get_servername(s, TLSEXT_NAMETYPE_host_name);

  if (servername) {
    conn->servername_.Reset(env->isolate(),
                            OneByteString(env->isolate(), servername));

    // Call the SNI callback and use its return value as context
    if (!conn->sniObject_.IsEmpty()) {
      conn->sni_context_.Reset();

      Local<Object> sni_obj = PersistentToLocal(env->isolate(),
                                                conn->sniObject_);

      Local<Value> arg = PersistentToLocal(env->isolate(), conn->servername_);
      Local<Value> ret = node::MakeCallback(env->isolate(),
                                            sni_obj,
                                            env->onselect_string(),
                                            1,
                                            &arg);

      // If ret is SecureContext
      Local<FunctionTemplate> secure_context_constructor_template =
          env->secure_context_constructor_template();
      if (secure_context_constructor_template->HasInstance(ret)) {
        conn->sni_context_.Reset(env->isolate(), ret);
        SecureContext* sc;
        ASSIGN_OR_RETURN_UNWRAP(&sc, ret.As<Object>(), SSL_TLSEXT_ERR_NOACK);
        conn->SetSNIContext(sc);
      } else {
        return SSL_TLSEXT_ERR_NOACK;
      }
    }
  }

  return SSL_TLSEXT_ERR_OK;
}
#endif

void Connection::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  if (args.Length() < 1 || !args[0]->IsObject()) {
    env->ThrowError("First argument must be a tls module SecureContext");
    return;
  }

  SecureContext* sc;
  ASSIGN_OR_RETURN_UNWRAP(&sc, args[0].As<Object>());

  bool is_server = args[1]->BooleanValue();

  SSLWrap<Connection>::Kind kind =
      is_server ? SSLWrap<Connection>::kServer : SSLWrap<Connection>::kClient;
  Connection* conn = new Connection(env, args.This(), sc, kind);
  conn->bio_read_ = NodeBIO::New();
  conn->bio_write_ = NodeBIO::New();

  SSL_set_app_data(conn->ssl_, conn);

  if (is_server)
    SSL_set_info_callback(conn->ssl_, SSLInfoCallback);

  InitNPN(sc);

  SSL_set_cert_cb(conn->ssl_, SSLWrap<Connection>::SSLCertCallback, conn);

#ifdef SSL_CTRL_SET_TLSEXT_SERVERNAME_CB
  if (is_server) {
    SSL_CTX_set_tlsext_servername_callback(sc->ctx_, SelectSNIContextCallback_);
  } else if (args[2]->IsString()) {
    const node::Utf8Value servername(env->isolate(), args[2]);
    SSL_set_tlsext_host_name(conn->ssl_, *servername);
  }
#endif

  SSL_set_bio(conn->ssl_, conn->bio_read_, conn->bio_write_);

#ifdef SSL_MODE_RELEASE_BUFFERS
  long mode = SSL_get_mode(conn->ssl_);  // NOLINT(runtime/int)
  SSL_set_mode(conn->ssl_, mode | SSL_MODE_RELEASE_BUFFERS);
#endif


  int verify_mode;
  if (is_server) {
    bool request_cert = args[2]->BooleanValue();
    if (!request_cert) {
      // Note reject_unauthorized ignored.
      verify_mode = SSL_VERIFY_NONE;
    } else {
      bool reject_unauthorized = args[3]->BooleanValue();
      verify_mode = SSL_VERIFY_PEER;
      if (reject_unauthorized)
        verify_mode |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
    }
  } else {
    // Note request_cert and reject_unauthorized are ignored for clients.
    verify_mode = SSL_VERIFY_NONE;
  }


  // Always allow a connection. We'll reject in javascript.
  SSL_set_verify(conn->ssl_, verify_mode, VerifyCallback);

  if (is_server) {
    SSL_set_accept_state(conn->ssl_);
  } else {
    SSL_set_connect_state(conn->ssl_);
  }
}


void Connection::SSLInfoCallback(const SSL *ssl_, int where, int ret) {
  if (!(where & (SSL_CB_HANDSHAKE_START | SSL_CB_HANDSHAKE_DONE)))
    return;

  // Be compatible with older versions of OpenSSL. SSL_get_app_data() wants
  // a non-const SSL* in OpenSSL <= 0.9.7e.
  SSL* ssl = const_cast<SSL*>(ssl_);
  Connection* conn = static_cast<Connection*>(SSL_get_app_data(ssl));
  Environment* env = conn->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  if (where & SSL_CB_HANDSHAKE_START) {
    conn->MakeCallback(env->onhandshakestart_string(), 0, nullptr);
  }

  if (where & SSL_CB_HANDSHAKE_DONE) {
    conn->MakeCallback(env->onhandshakedone_string(), 0, nullptr);
  }
}


void Connection::EncIn(const FunctionCallbackInfo<Value>& args) {
  Connection* conn;
  ASSIGN_OR_RETURN_UNWRAP(&conn, args.Holder());
  Environment* env = conn->env();

  if (args.Length() < 3) {
    return env->ThrowTypeError(
        "Data, offset, and length arguments are mandatory");
  }

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Data");

  char* buffer_data = Buffer::Data(args[0]);
  size_t buffer_length = Buffer::Length(args[0]);

  size_t off = args[1]->Int32Value();
  size_t len = args[2]->Int32Value();

  if (!Buffer::IsWithinBounds(off, len, buffer_length))
    return env->ThrowRangeError("offset + length > buffer.length");

  int bytes_written;
  char* data = buffer_data + off;

  if (conn->is_server() && !conn->hello_parser_.IsEnded()) {
    // Just accumulate data, everything will be pushed to BIO later
    if (conn->hello_parser_.IsPaused()) {
      bytes_written = 0;
    } else {
      // Copy incoming data to the internal buffer
      // (which has a size of the biggest possible TLS frame)
      size_t available = sizeof(conn->hello_data_) - conn->hello_offset_;
      size_t copied = len < available ? len : available;
      memcpy(conn->hello_data_ + conn->hello_offset_, data, copied);
      conn->hello_offset_ += copied;

      conn->hello_parser_.Parse(conn->hello_data_, conn->hello_offset_);
      bytes_written = copied;
    }
  } else {
    bytes_written = BIO_write(conn->bio_read_, data, len);
    conn->HandleBIOError(conn->bio_read_, "BIO_write", bytes_written);
    conn->SetShutdownFlags();
  }

  args.GetReturnValue().Set(bytes_written);
}


void Connection::ClearOut(const FunctionCallbackInfo<Value>& args) {
  Connection* conn;
  ASSIGN_OR_RETURN_UNWRAP(&conn, args.Holder());
  Environment* env = conn->env();

  if (args.Length() < 3) {
    return env->ThrowTypeError(
        "Data, offset, and length arguments are mandatory");
  }

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Data");

  char* buffer_data = Buffer::Data(args[0]);
  size_t buffer_length = Buffer::Length(args[0]);

  size_t off = args[1]->Int32Value();
  size_t len = args[2]->Int32Value();

  if (!Buffer::IsWithinBounds(off, len, buffer_length))
    return env->ThrowRangeError("offset + length > buffer.length");

  if (!SSL_is_init_finished(conn->ssl_)) {
    int rv;

    if (conn->is_server()) {
      rv = SSL_accept(conn->ssl_);
      conn->HandleSSLError("SSL_accept:ClearOut",
                           rv,
                           kZeroIsAnError,
                           kSyscallError);
    } else {
      rv = SSL_connect(conn->ssl_);
      conn->HandleSSLError("SSL_connect:ClearOut",
                           rv,
                           kZeroIsAnError,
                           kSyscallError);
    }

    if (rv < 0) {
      return args.GetReturnValue().Set(rv);
    }
  }

  int bytes_read = SSL_read(conn->ssl_, buffer_data + off, len);
  conn->HandleSSLError("SSL_read:ClearOut",
                       bytes_read,
                       kZeroIsNotAnError,
                       kSyscallError);
  conn->SetShutdownFlags();

  args.GetReturnValue().Set(bytes_read);
}


void Connection::ClearPending(const FunctionCallbackInfo<Value>& args) {
  Connection* conn;
  ASSIGN_OR_RETURN_UNWRAP(&conn, args.Holder());
  int bytes_pending = BIO_pending(conn->bio_read_);
  args.GetReturnValue().Set(bytes_pending);
}


void Connection::EncPending(const FunctionCallbackInfo<Value>& args) {
  Connection* conn;
  ASSIGN_OR_RETURN_UNWRAP(&conn, args.Holder());
  int bytes_pending = BIO_pending(conn->bio_write_);
  args.GetReturnValue().Set(bytes_pending);
}


void Connection::EncOut(const FunctionCallbackInfo<Value>& args) {
  Connection* conn;
  ASSIGN_OR_RETURN_UNWRAP(&conn, args.Holder());
  Environment* env = conn->env();

  if (args.Length() < 3) {
    return env->ThrowTypeError(
        "Data, offset, and length arguments are mandatory");
  }

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Data");

  char* buffer_data = Buffer::Data(args[0]);
  size_t buffer_length = Buffer::Length(args[0]);

  size_t off = args[1]->Int32Value();
  size_t len = args[2]->Int32Value();

  if (!Buffer::IsWithinBounds(off, len, buffer_length))
    return env->ThrowRangeError("offset + length > buffer.length");

  int bytes_read = BIO_read(conn->bio_write_, buffer_data + off, len);

  conn->HandleBIOError(conn->bio_write_, "BIO_read:EncOut", bytes_read);
  conn->SetShutdownFlags();

  args.GetReturnValue().Set(bytes_read);
}


void Connection::ClearIn(const FunctionCallbackInfo<Value>& args) {
  Connection* conn;
  ASSIGN_OR_RETURN_UNWRAP(&conn, args.Holder());
  Environment* env = conn->env();

  if (args.Length() < 3) {
    return env->ThrowTypeError(
        "Data, offset, and length arguments are mandatory");
  }

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Data");

  char* buffer_data = Buffer::Data(args[0]);
  size_t buffer_length = Buffer::Length(args[0]);

  size_t off = args[1]->Int32Value();
  size_t len = args[2]->Int32Value();

  if (!Buffer::IsWithinBounds(off, len, buffer_length))
    return env->ThrowRangeError("offset + length > buffer.length");

  if (!SSL_is_init_finished(conn->ssl_)) {
    int rv;
    if (conn->is_server()) {
      rv = SSL_accept(conn->ssl_);
      conn->HandleSSLError("SSL_accept:ClearIn",
                           rv,
                           kZeroIsAnError,
                           kSyscallError);
    } else {
      rv = SSL_connect(conn->ssl_);
      conn->HandleSSLError("SSL_connect:ClearIn",
                           rv,
                           kZeroIsAnError,
                           kSyscallError);
    }

    if (rv < 0) {
      return args.GetReturnValue().Set(rv);
    }
  }

  int bytes_written = SSL_write(conn->ssl_, buffer_data + off, len);

  conn->HandleSSLError("SSL_write:ClearIn",
                       bytes_written,
                       len == 0 ? kZeroIsNotAnError : kZeroIsAnError,
                       kSyscallError);
  conn->SetShutdownFlags();

  args.GetReturnValue().Set(bytes_written);
}


void Connection::Start(const FunctionCallbackInfo<Value>& args) {
  Connection* conn;
  ASSIGN_OR_RETURN_UNWRAP(&conn, args.Holder());

  int rv = 0;
  if (!SSL_is_init_finished(conn->ssl_)) {
    if (conn->is_server()) {
      rv = SSL_accept(conn->ssl_);
      conn->HandleSSLError("SSL_accept:Start",
                           rv,
                           kZeroIsAnError,
                           kSyscallError);
    } else {
      rv = SSL_connect(conn->ssl_);
      conn->HandleSSLError("SSL_connect:Start",
                           rv,
                           kZeroIsAnError,
                           kSyscallError);
    }
  }
  args.GetReturnValue().Set(rv);
}


void Connection::Close(const FunctionCallbackInfo<Value>& args) {
  Connection* conn;
  ASSIGN_OR_RETURN_UNWRAP(&conn, args.Holder());

  if (conn->ssl_ != nullptr) {
    SSL_free(conn->ssl_);
    conn->ssl_ = nullptr;
  }
}


#ifdef SSL_CTRL_SET_TLSEXT_SERVERNAME_CB
void Connection::GetServername(const FunctionCallbackInfo<Value>& args) {
  Connection* conn;
  ASSIGN_OR_RETURN_UNWRAP(&conn, args.Holder());

  if (conn->is_server() && !conn->servername_.IsEmpty()) {
    args.GetReturnValue().Set(conn->servername_);
  } else {
    args.GetReturnValue().Set(false);
  }
}


void Connection::SetSNICallback(const FunctionCallbackInfo<Value>& args) {
  Connection* conn;
  ASSIGN_OR_RETURN_UNWRAP(&conn, args.Holder());
  Environment* env = conn->env();

  if (args.Length() < 1 || !args[0]->IsFunction()) {
    return env->ThrowError("Must give a Function as first argument");
  }

  Local<Object> obj = Object::New(env->isolate());
  obj->Set(FIXED_ONE_BYTE_STRING(args.GetIsolate(), "onselect"), args[0]);
  conn->sniObject_.Reset(args.GetIsolate(), obj);
}
#endif


void CipherBase::Initialize(Environment* env, Local<Object> target) {
  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);

  t->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(t, "init", Init);
  env->SetProtoMethod(t, "initiv", InitIv);
  env->SetProtoMethod(t, "update", Update);
  env->SetProtoMethod(t, "final", Final);
  env->SetProtoMethod(t, "setAutoPadding", SetAutoPadding);
  env->SetProtoMethod(t, "getAuthTag", GetAuthTag);
  env->SetProtoMethod(t, "setAuthTag", SetAuthTag);
  env->SetProtoMethod(t, "setAAD", SetAAD);

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "CipherBase"),
              t->GetFunction());
}


void CipherBase::New(const FunctionCallbackInfo<Value>& args) {
  CHECK_EQ(args.IsConstructCall(), true);
  CipherKind kind = args[0]->IsTrue() ? kCipher : kDecipher;
  Environment* env = Environment::GetCurrent(args);
  new CipherBase(env, args.This(), kind);
}


void CipherBase::Init(const char* cipher_type,
                      const char* key_buf,
                      int key_buf_len) {
  HandleScope scope(env()->isolate());

#ifdef NODE_FIPS_MODE
  if (FIPS_mode()) {
    return env()->ThrowError(
        "crypto.createCipher() is not supported in FIPS mode.");
  }
#endif  // NODE_FIPS_MODE

  CHECK_EQ(cipher_, nullptr);
  cipher_ = EVP_get_cipherbyname(cipher_type);
  if (cipher_ == nullptr) {
    return env()->ThrowError("Unknown cipher");
  }

  unsigned char key[EVP_MAX_KEY_LENGTH];
  unsigned char iv[EVP_MAX_IV_LENGTH];

  int key_len = EVP_BytesToKey(cipher_,
                               EVP_md5(),
                               nullptr,
                               reinterpret_cast<const unsigned char*>(key_buf),
                               key_buf_len,
                               1,
                               key,
                               iv);

  EVP_CIPHER_CTX_init(&ctx_);
  const bool encrypt = (kind_ == kCipher);
  EVP_CipherInit_ex(&ctx_, cipher_, nullptr, nullptr, nullptr, encrypt);
  if (!EVP_CIPHER_CTX_set_key_length(&ctx_, key_len)) {
    EVP_CIPHER_CTX_cleanup(&ctx_);
    return env()->ThrowError("Invalid key length");
  }

  EVP_CipherInit_ex(&ctx_,
                    nullptr,
                    nullptr,
                    reinterpret_cast<unsigned char*>(key),
                    reinterpret_cast<unsigned char*>(iv),
                    kind_ == kCipher);
  initialised_ = true;
}


void CipherBase::Init(const FunctionCallbackInfo<Value>& args) {
  CipherBase* cipher;
  ASSIGN_OR_RETURN_UNWRAP(&cipher, args.Holder());
  Environment* env = cipher->env();

  if (args.Length() < 2) {
    return env->ThrowError("Cipher type and key arguments are mandatory");
  }

  THROW_AND_RETURN_IF_NOT_STRING(args[0], "Cipher type");
  THROW_AND_RETURN_IF_NOT_BUFFER(args[1], "Key");

  const node::Utf8Value cipher_type(args.GetIsolate(), args[0]);
  const char* key_buf = Buffer::Data(args[1]);
  ssize_t key_buf_len = Buffer::Length(args[1]);
  cipher->Init(*cipher_type, key_buf, key_buf_len);
}


void CipherBase::InitIv(const char* cipher_type,
                        const char* key,
                        int key_len,
                        const char* iv,
                        int iv_len) {
  HandleScope scope(env()->isolate());

  cipher_ = EVP_get_cipherbyname(cipher_type);
  if (cipher_ == nullptr) {
    return env()->ThrowError("Unknown cipher");
  }

  const int expected_iv_len = EVP_CIPHER_iv_length(cipher_);
  const bool is_gcm_mode = (EVP_CIPH_GCM_MODE == EVP_CIPHER_mode(cipher_));

  if (is_gcm_mode == false && iv_len != expected_iv_len) {
    return env()->ThrowError("Invalid IV length");
  }

  EVP_CIPHER_CTX_init(&ctx_);
  const bool encrypt = (kind_ == kCipher);
  EVP_CipherInit_ex(&ctx_, cipher_, nullptr, nullptr, nullptr, encrypt);

  if (is_gcm_mode &&
      !EVP_CIPHER_CTX_ctrl(&ctx_, EVP_CTRL_GCM_SET_IVLEN, iv_len, nullptr)) {
    EVP_CIPHER_CTX_cleanup(&ctx_);
    return env()->ThrowError("Invalid IV length");
  }

  if (!EVP_CIPHER_CTX_set_key_length(&ctx_, key_len)) {
    EVP_CIPHER_CTX_cleanup(&ctx_);
    return env()->ThrowError("Invalid key length");
  }

  EVP_CipherInit_ex(&ctx_,
                    nullptr,
                    nullptr,
                    reinterpret_cast<const unsigned char*>(key),
                    reinterpret_cast<const unsigned char*>(iv),
                    kind_ == kCipher);
  initialised_ = true;
}


void CipherBase::InitIv(const FunctionCallbackInfo<Value>& args) {
  CipherBase* cipher;
  ASSIGN_OR_RETURN_UNWRAP(&cipher, args.Holder());
  Environment* env = cipher->env();

  if (args.Length() < 3) {
    return env->ThrowError("Cipher type, key, and IV arguments are mandatory");
  }

  THROW_AND_RETURN_IF_NOT_STRING(args[0], "Cipher type");
  THROW_AND_RETURN_IF_NOT_BUFFER(args[1], "Key");
  THROW_AND_RETURN_IF_NOT_BUFFER(args[2], "IV");

  const node::Utf8Value cipher_type(env->isolate(), args[0]);
  ssize_t key_len = Buffer::Length(args[1]);
  const char* key_buf = Buffer::Data(args[1]);
  ssize_t iv_len = Buffer::Length(args[2]);
  const char* iv_buf = Buffer::Data(args[2]);
  cipher->InitIv(*cipher_type, key_buf, key_len, iv_buf, iv_len);
}


bool CipherBase::IsAuthenticatedMode() const {
  // check if this cipher operates in an AEAD mode that we support.
  if (!cipher_)
    return false;
  int mode = EVP_CIPHER_mode(cipher_);
  return mode == EVP_CIPH_GCM_MODE;
}


bool CipherBase::GetAuthTag(char** out, unsigned int* out_len) const {
  // only callable after Final and if encrypting.
  if (initialised_ || kind_ != kCipher || !auth_tag_)
    return false;
  *out_len = auth_tag_len_;
  *out = static_cast<char*>(node::Malloc(auth_tag_len_));
  CHECK_NE(*out, nullptr);
  memcpy(*out, auth_tag_, auth_tag_len_);
  return true;
}


void CipherBase::GetAuthTag(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CipherBase* cipher;
  ASSIGN_OR_RETURN_UNWRAP(&cipher, args.Holder());

  char* out = nullptr;
  unsigned int out_len = 0;

  if (cipher->GetAuthTag(&out, &out_len)) {
    Local<Object> buf = Buffer::New(env, out, out_len).ToLocalChecked();
    args.GetReturnValue().Set(buf);
  } else {
    env->ThrowError("Attempting to get auth tag in unsupported state");
  }
}


bool CipherBase::SetAuthTag(const char* data, unsigned int len) {
  if (!initialised_ || !IsAuthenticatedMode() || kind_ != kDecipher)
    return false;
  delete[] auth_tag_;
  auth_tag_len_ = len;
  auth_tag_ = new char[len];
  memcpy(auth_tag_, data, len);
  return true;
}


void CipherBase::SetAuthTag(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Local<Object> buf = args[0].As<Object>();

  if (!buf->IsObject() || !Buffer::HasInstance(buf))
    return env->ThrowTypeError("Auth tag must be a Buffer");

  CipherBase* cipher;
  ASSIGN_OR_RETURN_UNWRAP(&cipher, args.Holder());

  if (!cipher->SetAuthTag(Buffer::Data(buf), Buffer::Length(buf)))
    env->ThrowError("Attempting to set auth tag in unsupported state");
}


bool CipherBase::SetAAD(const char* data, unsigned int len) {
  if (!initialised_ || !IsAuthenticatedMode())
    return false;
  int outlen;
  if (!EVP_CipherUpdate(&ctx_,
                        nullptr,
                        &outlen,
                        reinterpret_cast<const unsigned char*>(data),
                        len)) {
    return false;
  }
  return true;
}


void CipherBase::SetAAD(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "AAD");

  CipherBase* cipher;
  ASSIGN_OR_RETURN_UNWRAP(&cipher, args.Holder());

  if (!cipher->SetAAD(Buffer::Data(args[0]), Buffer::Length(args[0])))
    env->ThrowError("Attempting to set AAD in unsupported state");
}


bool CipherBase::Update(const char* data,
                        int len,
                        unsigned char** out,
                        int* out_len) {
  if (!initialised_)
    return 0;

  // on first update:
  if (kind_ == kDecipher && IsAuthenticatedMode() && auth_tag_ != nullptr) {
    EVP_CIPHER_CTX_ctrl(&ctx_,
                        EVP_CTRL_GCM_SET_TAG,
                        auth_tag_len_,
                        reinterpret_cast<unsigned char*>(auth_tag_));
    delete[] auth_tag_;
    auth_tag_ = nullptr;
  }

  *out_len = len + EVP_CIPHER_CTX_block_size(&ctx_);
  *out = new unsigned char[*out_len];
  return EVP_CipherUpdate(&ctx_,
                          *out,
                          out_len,
                          reinterpret_cast<const unsigned char*>(data),
                          len);
}


void CipherBase::Update(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  CipherBase* cipher;
  ASSIGN_OR_RETURN_UNWRAP(&cipher, args.Holder());

  THROW_AND_RETURN_IF_NOT_STRING_OR_BUFFER(args[0], "Cipher data");

  unsigned char* out = nullptr;
  bool r;
  int out_len = 0;

  // Only copy the data if we have to, because it's a string
  if (args[0]->IsString()) {
    StringBytes::InlineDecoder decoder;
    if (!decoder.Decode(env, args[0].As<String>(), args[1], UTF8))
      return;
    r = cipher->Update(decoder.out(), decoder.size(), &out, &out_len);
  } else {
    char* buf = Buffer::Data(args[0]);
    size_t buflen = Buffer::Length(args[0]);
    r = cipher->Update(buf, buflen, &out, &out_len);
  }

  if (!r) {
    delete[] out;
    return ThrowCryptoError(env,
                            ERR_get_error(),
                            "Trying to add data in unsupported state");
  }

  CHECK(out != nullptr || out_len == 0);
  Local<Object> buf =
      Buffer::Copy(env, reinterpret_cast<char*>(out), out_len).ToLocalChecked();
  if (out)
    delete[] out;

  args.GetReturnValue().Set(buf);
}


bool CipherBase::SetAutoPadding(bool auto_padding) {
  if (!initialised_)
    return false;
  return EVP_CIPHER_CTX_set_padding(&ctx_, auto_padding);
}


void CipherBase::SetAutoPadding(const FunctionCallbackInfo<Value>& args) {
  CipherBase* cipher;
  ASSIGN_OR_RETURN_UNWRAP(&cipher, args.Holder());
  cipher->SetAutoPadding(args.Length() < 1 || args[0]->BooleanValue());
}


bool CipherBase::Final(unsigned char** out, int *out_len) {
  if (!initialised_)
    return false;

  *out = new unsigned char[EVP_CIPHER_CTX_block_size(&ctx_)];
  int r = EVP_CipherFinal_ex(&ctx_, *out, out_len);

  if (r && kind_ == kCipher) {
    delete[] auth_tag_;
    auth_tag_ = nullptr;
    if (IsAuthenticatedMode()) {
      auth_tag_len_ = EVP_GCM_TLS_TAG_LEN;  // use default tag length
      auth_tag_ = new char[auth_tag_len_];
      memset(auth_tag_, 0, auth_tag_len_);
      EVP_CIPHER_CTX_ctrl(&ctx_,
                          EVP_CTRL_GCM_GET_TAG,
                          auth_tag_len_,
                          reinterpret_cast<unsigned char*>(auth_tag_));
    }
  }

  EVP_CIPHER_CTX_cleanup(&ctx_);
  initialised_ = false;

  return r == 1;
}


void CipherBase::Final(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  CipherBase* cipher;
  ASSIGN_OR_RETURN_UNWRAP(&cipher, args.Holder());

  unsigned char* out_value = nullptr;
  int out_len = -1;
  Local<Value> outString;

  bool r = cipher->Final(&out_value, &out_len);

  if (out_len <= 0 || !r) {
    delete[] out_value;
    out_value = nullptr;
    out_len = 0;
    if (!r) {
      const char* msg = cipher->IsAuthenticatedMode() ?
          "Unsupported state or unable to authenticate data" :
          "Unsupported state";

      return ThrowCryptoError(env,
                              ERR_get_error(),
                              msg);
    }
  }

  Local<Object> buf = Buffer::Copy(
      env,
      reinterpret_cast<char*>(out_value),
      out_len).ToLocalChecked();
  args.GetReturnValue().Set(buf);
  delete[] out_value;
}


void Hmac::Initialize(Environment* env, v8::Local<v8::Object> target) {
  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);

  t->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(t, "init", HmacInit);
  env->SetProtoMethod(t, "update", HmacUpdate);
  env->SetProtoMethod(t, "digest", HmacDigest);

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "Hmac"), t->GetFunction());
}


void Hmac::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  new Hmac(env, args.This());
}


void Hmac::HmacInit(const char* hash_type, const char* key, int key_len) {
  HandleScope scope(env()->isolate());

  CHECK_EQ(initialised_, false);
  const EVP_MD* md = EVP_get_digestbyname(hash_type);
  if (md == nullptr) {
    return env()->ThrowError("Unknown message digest");
  }
  HMAC_CTX_init(&ctx_);
  if (key_len == 0) {
    key = "";
  }
  if (!HMAC_Init_ex(&ctx_, key, key_len, md, nullptr)) {
    return ThrowCryptoError(env(), ERR_get_error());
  }
  initialised_ = true;
}


void Hmac::HmacInit(const FunctionCallbackInfo<Value>& args) {
  Hmac* hmac;
  ASSIGN_OR_RETURN_UNWRAP(&hmac, args.Holder());
  Environment* env = hmac->env();

  if (args.Length() < 2) {
    return env->ThrowError("Hash type and key arguments are mandatory");
  }

  THROW_AND_RETURN_IF_NOT_STRING(args[0], "Hash type");
  THROW_AND_RETURN_IF_NOT_BUFFER(args[1], "Key");

  const node::Utf8Value hash_type(env->isolate(), args[0]);
  const char* buffer_data = Buffer::Data(args[1]);
  size_t buffer_length = Buffer::Length(args[1]);
  hmac->HmacInit(*hash_type, buffer_data, buffer_length);
}


bool Hmac::HmacUpdate(const char* data, int len) {
  if (!initialised_)
    return false;
  HMAC_Update(&ctx_, reinterpret_cast<const unsigned char*>(data), len);
  return true;
}


void Hmac::HmacUpdate(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Hmac* hmac;
  ASSIGN_OR_RETURN_UNWRAP(&hmac, args.Holder());

  THROW_AND_RETURN_IF_NOT_STRING_OR_BUFFER(args[0], "Data");

  // Only copy the data if we have to, because it's a string
  bool r;
  if (args[0]->IsString()) {
    StringBytes::InlineDecoder decoder;
    if (!decoder.Decode(env, args[0].As<String>(), args[1], UTF8))
      return;
    r = hmac->HmacUpdate(decoder.out(), decoder.size());
  } else {
    char* buf = Buffer::Data(args[0]);
    size_t buflen = Buffer::Length(args[0]);
    r = hmac->HmacUpdate(buf, buflen);
  }

  if (!r) {
    return env->ThrowTypeError("HmacUpdate fail");
  }
}


bool Hmac::HmacDigest(unsigned char** md_value, unsigned int* md_len) {
  if (!initialised_)
    return false;
  *md_value = new unsigned char[EVP_MAX_MD_SIZE];
  HMAC_Final(&ctx_, *md_value, md_len);
  HMAC_CTX_cleanup(&ctx_);
  initialised_ = false;
  return true;
}


void Hmac::HmacDigest(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Hmac* hmac;
  ASSIGN_OR_RETURN_UNWRAP(&hmac, args.Holder());

  enum encoding encoding = BUFFER;
  if (args.Length() >= 1) {
    encoding = ParseEncoding(env->isolate(),
                             args[0]->ToString(env->isolate()),
                             BUFFER);
  }

  unsigned char* md_value = nullptr;
  unsigned int md_len = 0;

  bool r = hmac->HmacDigest(&md_value, &md_len);
  if (!r) {
    md_value = nullptr;
    md_len = 0;
  }

  Local<Value> rc = StringBytes::Encode(env->isolate(),
                                        reinterpret_cast<const char*>(md_value),
                                        md_len,
                                        encoding);
  delete[] md_value;
  args.GetReturnValue().Set(rc);
}


void Hash::Initialize(Environment* env, v8::Local<v8::Object> target) {
  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);

  t->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(t, "update", HashUpdate);
  env->SetProtoMethod(t, "digest", HashDigest);

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "Hash"), t->GetFunction());
}


void Hash::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  if (args.Length() == 0 || !args[0]->IsString()) {
    return env->ThrowError("Must give hashtype string as argument");
  }

  const node::Utf8Value hash_type(env->isolate(), args[0]);

  Hash* hash = new Hash(env, args.This());
  if (!hash->HashInit(*hash_type)) {
    return ThrowCryptoError(env, ERR_get_error(),
                            "Digest method not supported");
  }
}


bool Hash::HashInit(const char* hash_type) {
  CHECK_EQ(initialised_, false);
  const EVP_MD* md = EVP_get_digestbyname(hash_type);
  if (md == nullptr)
    return false;
  EVP_MD_CTX_init(&mdctx_);
  if (EVP_DigestInit_ex(&mdctx_, md, nullptr) <= 0) {
    return false;
  }
  initialised_ = true;
  finalized_ = false;
  return true;
}


bool Hash::HashUpdate(const char* data, int len) {
  if (!initialised_)
    return false;
  EVP_DigestUpdate(&mdctx_, data, len);
  return true;
}


void Hash::HashUpdate(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Hash* hash;
  ASSIGN_OR_RETURN_UNWRAP(&hash, args.Holder());

  THROW_AND_RETURN_IF_NOT_STRING_OR_BUFFER(args[0], "Data");

  if (!hash->initialised_) {
    return env->ThrowError("Not initialized");
  }
  if (hash->finalized_) {
    return env->ThrowError("Digest already called");
  }

  // Only copy the data if we have to, because it's a string
  bool r;
  if (args[0]->IsString()) {
    StringBytes::InlineDecoder decoder;
    if (!decoder.Decode(env, args[0].As<String>(), args[1], UTF8))
      return;
    r = hash->HashUpdate(decoder.out(), decoder.size());
  } else {
    char* buf = Buffer::Data(args[0]);
    size_t buflen = Buffer::Length(args[0]);
    r = hash->HashUpdate(buf, buflen);
  }

  if (!r) {
    return env->ThrowTypeError("HashUpdate fail");
  }
}


void Hash::HashDigest(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Hash* hash;
  ASSIGN_OR_RETURN_UNWRAP(&hash, args.Holder());

  if (!hash->initialised_) {
    return env->ThrowError("Not initialized");
  }
  if (hash->finalized_) {
    return env->ThrowError("Digest already called");
  }

  enum encoding encoding = BUFFER;
  if (args.Length() >= 1) {
    encoding = ParseEncoding(env->isolate(),
                             args[0]->ToString(env->isolate()),
                             BUFFER);
  }

  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len;

  EVP_DigestFinal_ex(&hash->mdctx_, md_value, &md_len);
  EVP_MD_CTX_cleanup(&hash->mdctx_);
  hash->finalized_ = true;

  Local<Value> rc = StringBytes::Encode(env->isolate(),
                                        reinterpret_cast<const char*>(md_value),
                                        md_len,
                                        encoding);
  args.GetReturnValue().Set(rc);
}


void SignBase::CheckThrow(SignBase::Error error) {
  HandleScope scope(env()->isolate());

  switch (error) {
    case kSignUnknownDigest:
      return env()->ThrowError("Unknown message digest");

    case kSignNotInitialised:
      return env()->ThrowError("Not initialised");

    case kSignInit:
    case kSignUpdate:
    case kSignPrivateKey:
    case kSignPublicKey:
      {
        unsigned long err = ERR_get_error();  // NOLINT(runtime/int)
        if (err)
          return ThrowCryptoError(env(), err);
        switch (error) {
          case kSignInit:
            return env()->ThrowError("EVP_SignInit_ex failed");
          case kSignUpdate:
            return env()->ThrowError("EVP_SignUpdate failed");
          case kSignPrivateKey:
            return env()->ThrowError("PEM_read_bio_PrivateKey failed");
          case kSignPublicKey:
            return env()->ThrowError("PEM_read_bio_PUBKEY failed");
          default:
            ABORT();
        }
      }

    case kSignOk:
      return;
  }
}




void Sign::Initialize(Environment* env, v8::Local<v8::Object> target) {
  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);

  t->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(t, "init", SignInit);
  env->SetProtoMethod(t, "update", SignUpdate);
  env->SetProtoMethod(t, "sign", SignFinal);

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "Sign"), t->GetFunction());
}


void Sign::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  new Sign(env, args.This());
}


SignBase::Error Sign::SignInit(const char* sign_type) {
  CHECK_EQ(initialised_, false);
  const EVP_MD* md = EVP_get_digestbyname(sign_type);
  if (md == nullptr)
    return kSignUnknownDigest;

  EVP_MD_CTX_init(&mdctx_);
  if (!EVP_SignInit_ex(&mdctx_, md, nullptr))
    return kSignInit;
  initialised_ = true;

  return kSignOk;
}


void Sign::SignInit(const FunctionCallbackInfo<Value>& args) {
  Sign* sign;
  ASSIGN_OR_RETURN_UNWRAP(&sign, args.Holder());
  Environment* env = sign->env();

  if (args.Length() == 0) {
    return env->ThrowError("Sign type argument is mandatory");
  }

  THROW_AND_RETURN_IF_NOT_STRING(args[0], "Sign type");

  const node::Utf8Value sign_type(args.GetIsolate(), args[0]);
  sign->CheckThrow(sign->SignInit(*sign_type));
}


SignBase::Error Sign::SignUpdate(const char* data, int len) {
  if (!initialised_)
    return kSignNotInitialised;
  if (!EVP_SignUpdate(&mdctx_, data, len))
    return kSignUpdate;
  return kSignOk;
}


void Sign::SignUpdate(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Sign* sign;
  ASSIGN_OR_RETURN_UNWRAP(&sign, args.Holder());

  THROW_AND_RETURN_IF_NOT_STRING_OR_BUFFER(args[0], "Data");

  // Only copy the data if we have to, because it's a string
  Error err;
  if (args[0]->IsString()) {
    StringBytes::InlineDecoder decoder;
    if (!decoder.Decode(env, args[0].As<String>(), args[1], UTF8))
      return;
    err = sign->SignUpdate(decoder.out(), decoder.size());
  } else {
    char* buf = Buffer::Data(args[0]);
    size_t buflen = Buffer::Length(args[0]);
    err = sign->SignUpdate(buf, buflen);
  }

  sign->CheckThrow(err);
}


SignBase::Error Sign::SignFinal(const char* key_pem,
                                int key_pem_len,
                                const char* passphrase,
                                unsigned char** sig,
                                unsigned int *sig_len) {
  if (!initialised_)
    return kSignNotInitialised;

  BIO* bp = nullptr;
  EVP_PKEY* pkey = nullptr;
  bool fatal = true;

  bp = BIO_new_mem_buf(const_cast<char*>(key_pem), key_pem_len);
  if (bp == nullptr)
    goto exit;

  pkey = PEM_read_bio_PrivateKey(bp,
                                 nullptr,
                                 CryptoPemCallback,
                                 const_cast<char*>(passphrase));

  // Errors might be injected into OpenSSL's error stack
  // without `pkey` being set to nullptr;
  // cf. the test of `test_bad_rsa_privkey.pem` for an example.
  if (pkey == nullptr || 0 != ERR_peek_error())
    goto exit;

#ifdef NODE_FIPS_MODE
  /* Validate DSA2 parameters from FIPS 186-4 */
  if (FIPS_mode() && EVP_PKEY_DSA == pkey->type) {
    size_t L = BN_num_bits(pkey->pkey.dsa->p);
    size_t N = BN_num_bits(pkey->pkey.dsa->q);
    bool result = false;

    if (L == 1024 && N == 160)
      result = true;
    else if (L == 2048 && N == 224)
      result = true;
    else if (L == 2048 && N == 256)
      result = true;
    else if (L == 3072 && N == 256)
      result = true;

    if (!result) {
      fatal = true;
      goto exit;
    }
  }
#endif  // NODE_FIPS_MODE

  if (EVP_SignFinal(&mdctx_, *sig, sig_len, pkey))
    fatal = false;

  initialised_ = false;

 exit:
  if (pkey != nullptr)
    EVP_PKEY_free(pkey);
  if (bp != nullptr)
    BIO_free_all(bp);

  EVP_MD_CTX_cleanup(&mdctx_);

  if (fatal)
    return kSignPrivateKey;

  return kSignOk;
}


void Sign::SignFinal(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Sign* sign;
  ASSIGN_OR_RETURN_UNWRAP(&sign, args.Holder());

  unsigned char* md_value;
  unsigned int md_len;

  unsigned int len = args.Length();
  enum encoding encoding = BUFFER;
  if (len >= 2 && args[1]->IsString()) {
    encoding = ParseEncoding(env->isolate(),
                             args[1]->ToString(env->isolate()),
                             BUFFER);
  }

  node::Utf8Value passphrase(env->isolate(), args[2]);

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Data");
  size_t buf_len = Buffer::Length(args[0]);
  char* buf = Buffer::Data(args[0]);

  md_len = 8192;  // Maximum key size is 8192 bits
  md_value = new unsigned char[md_len];

  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence compiler warning.

  Error err = sign->SignFinal(
      buf,
      buf_len,
      len >= 3 && !args[2]->IsNull() ? *passphrase : nullptr,
      &md_value,
      &md_len);
  if (err != kSignOk) {
    delete[] md_value;
    md_value = nullptr;
    md_len = 0;
    return sign->CheckThrow(err);
  }

  Local<Value> rc = StringBytes::Encode(env->isolate(),
                                        reinterpret_cast<const char*>(md_value),
                                        md_len,
                                        encoding);
  delete[] md_value;
  args.GetReturnValue().Set(rc);
}


void Verify::Initialize(Environment* env, v8::Local<v8::Object> target) {
  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);

  t->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(t, "init", VerifyInit);
  env->SetProtoMethod(t, "update", VerifyUpdate);
  env->SetProtoMethod(t, "verify", VerifyFinal);

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "Verify"),
              t->GetFunction());
}


void Verify::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  new Verify(env, args.This());
}


SignBase::Error Verify::VerifyInit(const char* verify_type) {
  CHECK_EQ(initialised_, false);
  const EVP_MD* md = EVP_get_digestbyname(verify_type);
  if (md == nullptr)
    return kSignUnknownDigest;

  EVP_MD_CTX_init(&mdctx_);
  if (!EVP_VerifyInit_ex(&mdctx_, md, nullptr))
    return kSignInit;
  initialised_ = true;

  return kSignOk;
}


void Verify::VerifyInit(const FunctionCallbackInfo<Value>& args) {
  Verify* verify;
  ASSIGN_OR_RETURN_UNWRAP(&verify, args.Holder());
  Environment* env = verify->env();

  if (args.Length() == 0) {
    return env->ThrowError("Verify type argument is mandatory");
  }

  THROW_AND_RETURN_IF_NOT_STRING(args[0], "Verify type");

  const node::Utf8Value verify_type(args.GetIsolate(), args[0]);
  verify->CheckThrow(verify->VerifyInit(*verify_type));
}


SignBase::Error Verify::VerifyUpdate(const char* data, int len) {
  if (!initialised_)
    return kSignNotInitialised;

  if (!EVP_VerifyUpdate(&mdctx_, data, len))
    return kSignUpdate;

  return kSignOk;
}


void Verify::VerifyUpdate(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Verify* verify;
  ASSIGN_OR_RETURN_UNWRAP(&verify, args.Holder());

  THROW_AND_RETURN_IF_NOT_STRING_OR_BUFFER(args[0], "Data");

  // Only copy the data if we have to, because it's a string
  Error err;
  if (args[0]->IsString()) {
    StringBytes::InlineDecoder decoder;
    if (!decoder.Decode(env, args[0].As<String>(), args[1], UTF8))
      return;
    err = verify->VerifyUpdate(decoder.out(), decoder.size());
  } else {
    char* buf = Buffer::Data(args[0]);
    size_t buflen = Buffer::Length(args[0]);
    err = verify->VerifyUpdate(buf, buflen);
  }

  verify->CheckThrow(err);
}


SignBase::Error Verify::VerifyFinal(const char* key_pem,
                                    int key_pem_len,
                                    const char* sig,
                                    int siglen,
                                    bool* verify_result) {
  if (!initialised_)
    return kSignNotInitialised;

  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence compiler warning.

  EVP_PKEY* pkey = nullptr;
  BIO* bp = nullptr;
  X509* x509 = nullptr;
  bool fatal = true;
  int r = 0;

  bp = BIO_new_mem_buf(const_cast<char*>(key_pem), key_pem_len);
  if (bp == nullptr)
    goto exit;

  // Check if this is a PKCS#8 or RSA public key before trying as X.509.
  // Split this out into a separate function once we have more than one
  // consumer of public keys.
  if (strncmp(key_pem, PUBLIC_KEY_PFX, PUBLIC_KEY_PFX_LEN) == 0) {
    pkey = PEM_read_bio_PUBKEY(bp, nullptr, CryptoPemCallback, nullptr);
    if (pkey == nullptr)
      goto exit;
  } else if (strncmp(key_pem, PUBRSA_KEY_PFX, PUBRSA_KEY_PFX_LEN) == 0) {
    RSA* rsa =
        PEM_read_bio_RSAPublicKey(bp, nullptr, CryptoPemCallback, nullptr);
    if (rsa) {
      pkey = EVP_PKEY_new();
      if (pkey)
        EVP_PKEY_set1_RSA(pkey, rsa);
      RSA_free(rsa);
    }
    if (pkey == nullptr)
      goto exit;
  } else {
    // X.509 fallback
    x509 = PEM_read_bio_X509(bp, nullptr, CryptoPemCallback, nullptr);
    if (x509 == nullptr)
      goto exit;

    pkey = X509_get_pubkey(x509);
    if (pkey == nullptr)
      goto exit;
  }

  fatal = false;
  r = EVP_VerifyFinal(&mdctx_,
                      reinterpret_cast<const unsigned char*>(sig),
                      siglen,
                      pkey);

 exit:
  if (pkey != nullptr)
    EVP_PKEY_free(pkey);
  if (bp != nullptr)
    BIO_free_all(bp);
  if (x509 != nullptr)
    X509_free(x509);

  EVP_MD_CTX_cleanup(&mdctx_);
  initialised_ = false;

  if (fatal)
    return kSignPublicKey;

  *verify_result = r == 1;
  return kSignOk;
}


void Verify::VerifyFinal(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Verify* verify;
  ASSIGN_OR_RETURN_UNWRAP(&verify, args.Holder());

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Key");
  char* kbuf = Buffer::Data(args[0]);
  ssize_t klen = Buffer::Length(args[0]);

  THROW_AND_RETURN_IF_NOT_STRING_OR_BUFFER(args[1], "Hash");

  enum encoding encoding = UTF8;
  if (args.Length() >= 3) {
    encoding = ParseEncoding(env->isolate(),
                             args[2]->ToString(env->isolate()),
                             UTF8);
  }

  ssize_t hlen = StringBytes::Size(env->isolate(), args[1], encoding);

  // only copy if we need to, because it's a string.
  char* hbuf;
  if (args[1]->IsString()) {
    hbuf = new char[hlen];
    ssize_t hwritten = StringBytes::Write(env->isolate(),
                                          hbuf,
                                          hlen,
                                          args[1],
                                          encoding);
    CHECK_EQ(hwritten, hlen);
  } else {
    hbuf = Buffer::Data(args[1]);
  }

  bool verify_result;
  Error err = verify->VerifyFinal(kbuf, klen, hbuf, hlen, &verify_result);
  if (args[1]->IsString())
    delete[] hbuf;
  if (err != kSignOk)
    return verify->CheckThrow(err);
  args.GetReturnValue().Set(verify_result);
}


template <PublicKeyCipher::Operation operation,
          PublicKeyCipher::EVP_PKEY_cipher_init_t EVP_PKEY_cipher_init,
          PublicKeyCipher::EVP_PKEY_cipher_t EVP_PKEY_cipher>
bool PublicKeyCipher::Cipher(const char* key_pem,
                             int key_pem_len,
                             const char* passphrase,
                             int padding,
                             const unsigned char* data,
                             int len,
                             unsigned char** out,
                             size_t* out_len) {
  EVP_PKEY* pkey = nullptr;
  EVP_PKEY_CTX* ctx = nullptr;
  BIO* bp = nullptr;
  X509* x509 = nullptr;
  bool fatal = true;

  bp = BIO_new_mem_buf(const_cast<char*>(key_pem), key_pem_len);
  if (bp == nullptr)
    goto exit;

  // Check if this is a PKCS#8 or RSA public key before trying as X.509 and
  // private key.
  if (operation == kPublic &&
      strncmp(key_pem, PUBLIC_KEY_PFX, PUBLIC_KEY_PFX_LEN) == 0) {
    pkey = PEM_read_bio_PUBKEY(bp, nullptr, nullptr, nullptr);
    if (pkey == nullptr)
      goto exit;
  } else if (operation == kPublic &&
             strncmp(key_pem, PUBRSA_KEY_PFX, PUBRSA_KEY_PFX_LEN) == 0) {
    RSA* rsa = PEM_read_bio_RSAPublicKey(bp, nullptr, nullptr, nullptr);
    if (rsa) {
      pkey = EVP_PKEY_new();
      if (pkey)
        EVP_PKEY_set1_RSA(pkey, rsa);
      RSA_free(rsa);
    }
    if (pkey == nullptr)
      goto exit;
  } else if (operation == kPublic &&
             strncmp(key_pem, CERTIFICATE_PFX, CERTIFICATE_PFX_LEN) == 0) {
    x509 = PEM_read_bio_X509(bp, nullptr, CryptoPemCallback, nullptr);
    if (x509 == nullptr)
      goto exit;

    pkey = X509_get_pubkey(x509);
    if (pkey == nullptr)
      goto exit;
  } else {
    pkey = PEM_read_bio_PrivateKey(bp,
                                   nullptr,
                                   CryptoPemCallback,
                                   const_cast<char*>(passphrase));
    if (pkey == nullptr)
      goto exit;
  }

  ctx = EVP_PKEY_CTX_new(pkey, nullptr);
  if (!ctx)
    goto exit;
  if (EVP_PKEY_cipher_init(ctx) <= 0)
    goto exit;
  if (EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0)
    goto exit;

  if (EVP_PKEY_cipher(ctx, nullptr, out_len, data, len) <= 0)
    goto exit;

  *out = new unsigned char[*out_len];

  if (EVP_PKEY_cipher(ctx, *out, out_len, data, len) <= 0)
    goto exit;

  fatal = false;

 exit:
  if (x509 != nullptr)
    X509_free(x509);
  if (pkey != nullptr)
    EVP_PKEY_free(pkey);
  if (bp != nullptr)
    BIO_free_all(bp);
  if (ctx != nullptr)
    EVP_PKEY_CTX_free(ctx);

  return !fatal;
}


template <PublicKeyCipher::Operation operation,
          PublicKeyCipher::EVP_PKEY_cipher_init_t EVP_PKEY_cipher_init,
          PublicKeyCipher::EVP_PKEY_cipher_t EVP_PKEY_cipher>
void PublicKeyCipher::Cipher(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Key");
  char* kbuf = Buffer::Data(args[0]);
  ssize_t klen = Buffer::Length(args[0]);

  THROW_AND_RETURN_IF_NOT_BUFFER(args[1], "Data");
  char* buf = Buffer::Data(args[1]);
  ssize_t len = Buffer::Length(args[1]);

  int padding = args[2]->Uint32Value();

  String::Utf8Value passphrase(args[3]);

  unsigned char* out_value = nullptr;
  size_t out_len = 0;

  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence compiler warning.

  bool r = Cipher<operation, EVP_PKEY_cipher_init, EVP_PKEY_cipher>(
      kbuf,
      klen,
      args.Length() >= 3 && !args[2]->IsNull() ? *passphrase : nullptr,
      padding,
      reinterpret_cast<const unsigned char*>(buf),
      len,
      &out_value,
      &out_len);

  if (out_len == 0 || !r) {
    delete[] out_value;
    out_value = nullptr;
    out_len = 0;
    if (!r) {
      return ThrowCryptoError(env,
        ERR_get_error());
    }
  }

  Local<Object> vbuf = Buffer::Copy(
      env,
      reinterpret_cast<char*>(out_value),
      out_len).ToLocalChecked();
  args.GetReturnValue().Set(vbuf);
  delete[] out_value;
}


void DiffieHellman::Initialize(Environment* env, Local<Object> target) {
  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);

  const PropertyAttribute attributes =
      static_cast<PropertyAttribute>(v8::ReadOnly | v8::DontDelete);

  t->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(t, "generateKeys", GenerateKeys);
  env->SetProtoMethod(t, "computeSecret", ComputeSecret);
  env->SetProtoMethod(t, "getPrime", GetPrime);
  env->SetProtoMethod(t, "getGenerator", GetGenerator);
  env->SetProtoMethod(t, "getPublicKey", GetPublicKey);
  env->SetProtoMethod(t, "getPrivateKey", GetPrivateKey);
  env->SetProtoMethod(t, "setPublicKey", SetPublicKey);
  env->SetProtoMethod(t, "setPrivateKey", SetPrivateKey);

  t->InstanceTemplate()->SetAccessor(
      env->verify_error_string(),
      DiffieHellman::VerifyErrorGetter,
      nullptr,
      env->as_external(),
      DEFAULT,
      attributes,
      AccessorSignature::New(env->isolate(), t));

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "DiffieHellman"),
              t->GetFunction());

  Local<FunctionTemplate> t2 = env->NewFunctionTemplate(DiffieHellmanGroup);
  t2->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(t2, "generateKeys", GenerateKeys);
  env->SetProtoMethod(t2, "computeSecret", ComputeSecret);
  env->SetProtoMethod(t2, "getPrime", GetPrime);
  env->SetProtoMethod(t2, "getGenerator", GetGenerator);
  env->SetProtoMethod(t2, "getPublicKey", GetPublicKey);
  env->SetProtoMethod(t2, "getPrivateKey", GetPrivateKey);

  t2->InstanceTemplate()->SetAccessor(
      env->verify_error_string(),
      DiffieHellman::VerifyErrorGetter,
      nullptr,
      env->as_external(),
      DEFAULT,
      attributes,
      AccessorSignature::New(env->isolate(), t2));

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "DiffieHellmanGroup"),
              t2->GetFunction());
}


bool DiffieHellman::Init(int primeLength, int g) {
  dh = DH_new();
  if (!DH_generate_parameters_ex(dh, primeLength, g, 0))
    return false;
  bool result = VerifyContext();
  if (!result)
    return false;
  initialised_ = true;
  return true;
}


bool DiffieHellman::Init(const char* p, int p_len, int g) {
  dh = DH_new();
  dh->p = BN_bin2bn(reinterpret_cast<const unsigned char*>(p), p_len, 0);
  dh->g = BN_new();
  if (!BN_set_word(dh->g, g))
    return false;
  bool result = VerifyContext();
  if (!result)
    return false;
  initialised_ = true;
  return true;
}


bool DiffieHellman::Init(const char* p, int p_len, const char* g, int g_len) {
  dh = DH_new();
  dh->p = BN_bin2bn(reinterpret_cast<const unsigned char*>(p), p_len, 0);
  dh->g = BN_bin2bn(reinterpret_cast<const unsigned char*>(g), g_len, 0);
  bool result = VerifyContext();
  if (!result)
    return false;
  initialised_ = true;
  return true;
}


void DiffieHellman::DiffieHellmanGroup(
    const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  DiffieHellman* diffieHellman = new DiffieHellman(env, args.This());

  if (args.Length() != 1) {
    return env->ThrowError("Group name argument is mandatory");
  }

  THROW_AND_RETURN_IF_NOT_STRING(args[0], "Group name");

  bool initialized = false;

  const node::Utf8Value group_name(env->isolate(), args[0]);
  for (size_t i = 0; i < arraysize(modp_groups); ++i) {
    const modp_group* it = modp_groups + i;

    if (!StringEqualNoCase(*group_name, it->name))
      continue;

    initialized = diffieHellman->Init(it->prime,
                                      it->prime_size,
                                      it->gen,
                                      it->gen_size);
    if (!initialized)
      env->ThrowError("Initialization failed");
    return;
  }

  env->ThrowError("Unknown group");
}


void DiffieHellman::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  DiffieHellman* diffieHellman =
      new DiffieHellman(env, args.This());
  bool initialized = false;

  if (args.Length() == 2) {
    if (args[0]->IsInt32()) {
      if (args[1]->IsInt32()) {
        initialized = diffieHellman->Init(args[0]->Int32Value(),
                                          args[1]->Int32Value());
      }
    } else {
      if (args[1]->IsInt32()) {
        initialized = diffieHellman->Init(Buffer::Data(args[0]),
                                          Buffer::Length(args[0]),
                                          args[1]->Int32Value());
      } else {
        initialized = diffieHellman->Init(Buffer::Data(args[0]),
                                          Buffer::Length(args[0]),
                                          Buffer::Data(args[1]),
                                          Buffer::Length(args[1]));
      }
    }
  }

  if (!initialized) {
    return ThrowCryptoError(env, ERR_get_error(), "Initialization failed");
  }
}


void DiffieHellman::GenerateKeys(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  DiffieHellman* diffieHellman;
  ASSIGN_OR_RETURN_UNWRAP(&diffieHellman, args.Holder());

  if (!diffieHellman->initialised_) {
    return ThrowCryptoError(env, ERR_get_error(), "Not initialized");
  }

  if (!DH_generate_key(diffieHellman->dh)) {
    return ThrowCryptoError(env, ERR_get_error(), "Key generation failed");
  }

  int dataSize = BN_num_bytes(diffieHellman->dh->pub_key);
  char* data = new char[dataSize];
  BN_bn2bin(diffieHellman->dh->pub_key,
            reinterpret_cast<unsigned char*>(data));

  args.GetReturnValue().Set(Encode(env->isolate(), data, dataSize, BUFFER));
  delete[] data;
}


void DiffieHellman::GetPrime(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  DiffieHellman* diffieHellman;
  ASSIGN_OR_RETURN_UNWRAP(&diffieHellman, args.Holder());

  if (!diffieHellman->initialised_) {
    return ThrowCryptoError(env, ERR_get_error(), "Not initialized");
  }

  int dataSize = BN_num_bytes(diffieHellman->dh->p);
  char* data = new char[dataSize];
  BN_bn2bin(diffieHellman->dh->p, reinterpret_cast<unsigned char*>(data));

  args.GetReturnValue().Set(Encode(env->isolate(), data, dataSize, BUFFER));
  delete[] data;
}


void DiffieHellman::GetGenerator(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  DiffieHellman* diffieHellman;
  ASSIGN_OR_RETURN_UNWRAP(&diffieHellman, args.Holder());

  if (!diffieHellman->initialised_) {
    return ThrowCryptoError(env, ERR_get_error(), "Not initialized");
  }

  int dataSize = BN_num_bytes(diffieHellman->dh->g);
  char* data = new char[dataSize];
  BN_bn2bin(diffieHellman->dh->g, reinterpret_cast<unsigned char*>(data));

  args.GetReturnValue().Set(Encode(env->isolate(), data, dataSize, BUFFER));
  delete[] data;
}


void DiffieHellman::GetPublicKey(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  DiffieHellman* diffieHellman;
  ASSIGN_OR_RETURN_UNWRAP(&diffieHellman, args.Holder());

  if (!diffieHellman->initialised_) {
    return ThrowCryptoError(env, ERR_get_error(), "Not initialized");
  }

  if (diffieHellman->dh->pub_key == nullptr) {
    return env->ThrowError("No public key - did you forget to generate one?");
  }

  int dataSize = BN_num_bytes(diffieHellman->dh->pub_key);
  char* data = new char[dataSize];
  BN_bn2bin(diffieHellman->dh->pub_key,
            reinterpret_cast<unsigned char*>(data));

  args.GetReturnValue().Set(Encode(env->isolate(), data, dataSize, BUFFER));
  delete[] data;
}


void DiffieHellman::GetPrivateKey(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  DiffieHellman* diffieHellman;
  ASSIGN_OR_RETURN_UNWRAP(&diffieHellman, args.Holder());

  if (!diffieHellman->initialised_) {
    return ThrowCryptoError(env, ERR_get_error(), "Not initialized");
  }

  if (diffieHellman->dh->priv_key == nullptr) {
    return env->ThrowError("No private key - did you forget to generate one?");
  }

  int dataSize = BN_num_bytes(diffieHellman->dh->priv_key);
  char* data = new char[dataSize];
  BN_bn2bin(diffieHellman->dh->priv_key,
            reinterpret_cast<unsigned char*>(data));

  args.GetReturnValue().Set(Encode(env->isolate(), data, dataSize, BUFFER));
  delete[] data;
}


void DiffieHellman::ComputeSecret(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  DiffieHellman* diffieHellman;
  ASSIGN_OR_RETURN_UNWRAP(&diffieHellman, args.Holder());

  if (!diffieHellman->initialised_) {
    return ThrowCryptoError(env, ERR_get_error(), "Not initialized");
  }

  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence compiler warning.
  BIGNUM* key = nullptr;

  if (args.Length() == 0) {
    return env->ThrowError("Other party's public key argument is mandatory");
  } else {
    THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Other party's public key");
    key = BN_bin2bn(
        reinterpret_cast<unsigned char*>(Buffer::Data(args[0])),
        Buffer::Length(args[0]),
        0);
  }

  int dataSize = DH_size(diffieHellman->dh);
  char* data = new char[dataSize];

  int size = DH_compute_key(reinterpret_cast<unsigned char*>(data),
                            key,
                            diffieHellman->dh);

  if (size == -1) {
    int checkResult;
    int checked;

    checked = DH_check_pub_key(diffieHellman->dh, key, &checkResult);
    BN_free(key);
    delete[] data;

    if (!checked) {
      return ThrowCryptoError(env, ERR_get_error(), "Invalid Key");
    } else if (checkResult) {
      if (checkResult & DH_CHECK_PUBKEY_TOO_SMALL) {
        return env->ThrowError("Supplied key is too small");
      } else if (checkResult & DH_CHECK_PUBKEY_TOO_LARGE) {
        return env->ThrowError("Supplied key is too large");
      } else {
        return env->ThrowError("Invalid key");
      }
    } else {
      return env->ThrowError("Invalid key");
    }
  }

  BN_free(key);
  CHECK_GE(size, 0);

  // DH_size returns number of bytes in a prime number
  // DH_compute_key returns number of bytes in a remainder of exponent, which
  // may have less bytes than a prime number. Therefore add 0-padding to the
  // allocated buffer.
  if (size != dataSize) {
    CHECK(dataSize > size);
    memmove(data + dataSize - size, data, size);
    memset(data, 0, dataSize - size);
  }

  args.GetReturnValue().Set(Encode(env->isolate(), data, dataSize, BUFFER));
  delete[] data;
}


void DiffieHellman::SetPublicKey(const FunctionCallbackInfo<Value>& args) {
  DiffieHellman* diffieHellman;
  ASSIGN_OR_RETURN_UNWRAP(&diffieHellman, args.Holder());
  Environment* env = diffieHellman->env();

  if (!diffieHellman->initialised_) {
    return ThrowCryptoError(env, ERR_get_error(), "Not initialized");
  }

  if (args.Length() == 0) {
    return env->ThrowError("Public key argument is mandatory");
  } else {
    THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Public key");
    diffieHellman->dh->pub_key = BN_bin2bn(
        reinterpret_cast<unsigned char*>(Buffer::Data(args[0])),
        Buffer::Length(args[0]), 0);
  }
}


void DiffieHellman::SetPrivateKey(const FunctionCallbackInfo<Value>& args) {
  DiffieHellman* diffieHellman;
  ASSIGN_OR_RETURN_UNWRAP(&diffieHellman, args.Holder());
  Environment* env = diffieHellman->env();

  if (!diffieHellman->initialised_) {
    return ThrowCryptoError(env, ERR_get_error(), "Not initialized");
  }

  if (args.Length() == 0) {
    return env->ThrowError("Private key argument is mandatory");
  } else {
    THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Private key");
    diffieHellman->dh->priv_key = BN_bin2bn(
        reinterpret_cast<unsigned char*>(Buffer::Data(args[0])),
        Buffer::Length(args[0]),
        0);
  }
}


void DiffieHellman::VerifyErrorGetter(Local<String> property,
                                      const PropertyCallbackInfo<Value>& args) {
  HandleScope scope(args.GetIsolate());

  DiffieHellman* diffieHellman;
  ASSIGN_OR_RETURN_UNWRAP(&diffieHellman, args.Holder());

  if (!diffieHellman->initialised_)
    return ThrowCryptoError(diffieHellman->env(), ERR_get_error(),
                            "Not initialized");

  args.GetReturnValue().Set(diffieHellman->verifyError_);
}


bool DiffieHellman::VerifyContext() {
  int codes;
  if (!DH_check(dh, &codes))
    return false;
  verifyError_ = codes;
  return true;
}


void ECDH::Initialize(Environment* env, Local<Object> target) {
  HandleScope scope(env->isolate());

  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);

  t->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(t, "generateKeys", GenerateKeys);
  env->SetProtoMethod(t, "computeSecret", ComputeSecret);
  env->SetProtoMethod(t, "getPublicKey", GetPublicKey);
  env->SetProtoMethod(t, "getPrivateKey", GetPrivateKey);
  env->SetProtoMethod(t, "setPublicKey", SetPublicKey);
  env->SetProtoMethod(t, "setPrivateKey", SetPrivateKey);

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "ECDH"),
              t->GetFunction());
}


void ECDH::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  MarkPopErrorOnReturn mark_pop_error_on_return;

  // TODO(indutny): Support raw curves?
  THROW_AND_RETURN_IF_NOT_STRING(args[0], "ECDH curve name");
  node::Utf8Value curve(env->isolate(), args[0]);

  int nid = OBJ_sn2nid(*curve);
  if (nid == NID_undef)
    return env->ThrowTypeError("First argument should be a valid curve name");

  EC_KEY* key = EC_KEY_new_by_curve_name(nid);
  if (key == nullptr)
    return env->ThrowError("Failed to create EC_KEY using curve name");

  new ECDH(env, args.This(), key);
}


void ECDH::GenerateKeys(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  ECDH* ecdh;
  ASSIGN_OR_RETURN_UNWRAP(&ecdh, args.Holder());

  if (!EC_KEY_generate_key(ecdh->key_))
    return env->ThrowError("Failed to generate EC_KEY");
}


EC_POINT* ECDH::BufferToPoint(char* data, size_t len) {
  EC_POINT* pub;
  int r;

  pub = EC_POINT_new(group_);
  if (pub == nullptr) {
    env()->ThrowError("Failed to allocate EC_POINT for a public key");
    return nullptr;
  }

  r = EC_POINT_oct2point(
      group_,
      pub,
      reinterpret_cast<unsigned char*>(data),
      len,
      nullptr);
  if (!r) {
    env()->ThrowError("Failed to translate Buffer to a EC_POINT");
    goto fatal;
  }

  return pub;

 fatal:
  EC_POINT_free(pub);
  return nullptr;
}


void ECDH::ComputeSecret(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Data");

  ECDH* ecdh;
  ASSIGN_OR_RETURN_UNWRAP(&ecdh, args.Holder());

  if (!ecdh->IsKeyPairValid())
    return env->ThrowError("Invalid key pair");

  EC_POINT* pub = ecdh->BufferToPoint(Buffer::Data(args[0]),
                                      Buffer::Length(args[0]));
  if (pub == nullptr)
    return;

  // NOTE: field_size is in bits
  int field_size = EC_GROUP_get_degree(ecdh->group_);
  size_t out_len = (field_size + 7) / 8;
  char* out = static_cast<char*>(node::Malloc(out_len));
  CHECK_NE(out, nullptr);

  int r = ECDH_compute_key(out, out_len, pub, ecdh->key_, nullptr);
  EC_POINT_free(pub);
  if (!r) {
    free(out);
    return env->ThrowError("Failed to compute ECDH key");
  }

  Local<Object> buf = Buffer::New(env, out, out_len).ToLocalChecked();
  args.GetReturnValue().Set(buf);
}


void ECDH::GetPublicKey(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  // Conversion form
  CHECK_EQ(args.Length(), 1);

  ECDH* ecdh;
  ASSIGN_OR_RETURN_UNWRAP(&ecdh, args.Holder());

  const EC_POINT* pub = EC_KEY_get0_public_key(ecdh->key_);
  if (pub == nullptr)
    return env->ThrowError("Failed to get ECDH public key");

  int size;
  point_conversion_form_t form =
      static_cast<point_conversion_form_t>(args[0]->Uint32Value());

  size = EC_POINT_point2oct(ecdh->group_, pub, form, nullptr, 0, nullptr);
  if (size == 0)
    return env->ThrowError("Failed to get public key length");

  unsigned char* out = static_cast<unsigned char*>(node::Malloc(size));
  CHECK_NE(out, nullptr);

  int r = EC_POINT_point2oct(ecdh->group_, pub, form, out, size, nullptr);
  if (r != size) {
    free(out);
    return env->ThrowError("Failed to get public key");
  }

  Local<Object> buf =
      Buffer::New(env, reinterpret_cast<char*>(out), size).ToLocalChecked();
  args.GetReturnValue().Set(buf);
}


void ECDH::GetPrivateKey(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  ECDH* ecdh;
  ASSIGN_OR_RETURN_UNWRAP(&ecdh, args.Holder());

  const BIGNUM* b = EC_KEY_get0_private_key(ecdh->key_);
  if (b == nullptr)
    return env->ThrowError("Failed to get ECDH private key");

  int size = BN_num_bytes(b);
  unsigned char* out = static_cast<unsigned char*>(node::Malloc(size));
  CHECK_NE(out, nullptr);

  if (size != BN_bn2bin(b, out)) {
    free(out);
    return env->ThrowError("Failed to convert ECDH private key to Buffer");
  }

  Local<Object> buf =
      Buffer::New(env, reinterpret_cast<char*>(out), size).ToLocalChecked();
  args.GetReturnValue().Set(buf);
}


void ECDH::SetPrivateKey(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  ECDH* ecdh;
  ASSIGN_OR_RETURN_UNWRAP(&ecdh, args.Holder());

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Private key");

  BIGNUM* priv = BN_bin2bn(
      reinterpret_cast<unsigned char*>(Buffer::Data(args[0].As<Object>())),
      Buffer::Length(args[0].As<Object>()),
      nullptr);
  if (priv == nullptr)
    return env->ThrowError("Failed to convert Buffer to BN");

  if (!ecdh->IsKeyValidForCurve(priv)) {
    BN_free(priv);
    return env->ThrowError("Private key is not valid for specified curve.");
  }

  int result = EC_KEY_set_private_key(ecdh->key_, priv);
  BN_free(priv);

  if (!result) {
    return env->ThrowError("Failed to convert BN to a private key");
  }

  // To avoid inconsistency, clear the current public key in-case computing
  // the new one fails for some reason.
  EC_KEY_set_public_key(ecdh->key_, nullptr);

  MarkPopErrorOnReturn mark_pop_error_on_return;
  (void) &mark_pop_error_on_return;  // Silence compiler warning.

  const BIGNUM* priv_key = EC_KEY_get0_private_key(ecdh->key_);
  CHECK_NE(priv_key, nullptr);

  EC_POINT* pub = EC_POINT_new(ecdh->group_);
  CHECK_NE(pub, nullptr);

  if (!EC_POINT_mul(ecdh->group_, pub, priv_key, nullptr, nullptr, nullptr)) {
    EC_POINT_free(pub);
    return env->ThrowError("Failed to generate ECDH public key");
  }

  if (!EC_KEY_set_public_key(ecdh->key_, pub)) {
    EC_POINT_free(pub);
    return env->ThrowError("Failed to set generated public key");
  }

  EC_POINT_free(pub);
}


void ECDH::SetPublicKey(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  ECDH* ecdh;
  ASSIGN_OR_RETURN_UNWRAP(&ecdh, args.Holder());

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Public key");

  EC_POINT* pub = ecdh->BufferToPoint(Buffer::Data(args[0].As<Object>()),
                                      Buffer::Length(args[0].As<Object>()));
  if (pub == nullptr)
    return env->ThrowError("Failed to convert Buffer to EC_POINT");

  int r = EC_KEY_set_public_key(ecdh->key_, pub);
  EC_POINT_free(pub);
  if (!r)
    return env->ThrowError("Failed to set EC_POINT as the public key");
}


bool ECDH::IsKeyValidForCurve(const BIGNUM* private_key) {
  ASSERT_NE(group_, nullptr);
  CHECK_NE(private_key, nullptr);
  // Private keys must be in the range [1, n-1].
  // Ref: Section 3.2.1 - http://www.secg.org/sec1-v2.pdf
  if (BN_cmp(private_key, BN_value_one()) < 0) {
    return false;
  }
  BIGNUM* order = BN_new();
  CHECK_NE(order, nullptr);
  bool result = EC_GROUP_get_order(group_, order, nullptr) &&
                BN_cmp(private_key, order) < 0;
  BN_free(order);
  return result;
}


bool ECDH::IsKeyPairValid() {
  MarkPopErrorOnReturn mark_pop_error_on_return;
  (void) &mark_pop_error_on_return;  // Silence compiler warning.
  return 1 == EC_KEY_check_key(key_);
}


class PBKDF2Request : public AsyncWrap {
 public:
  PBKDF2Request(Environment* env,
                Local<Object> object,
                const EVP_MD* digest,
                int passlen,
                char* pass,
                int saltlen,
                char* salt,
                int iter,
                int keylen)
      : AsyncWrap(env, object, AsyncWrap::PROVIDER_CRYPTO),
        digest_(digest),
        error_(0),
        passlen_(passlen),
        pass_(pass),
        saltlen_(saltlen),
        salt_(salt),
        keylen_(keylen),
        key_(static_cast<char*>(node::Malloc(keylen))),
        iter_(iter) {
    if (key() == nullptr)
      FatalError("node::PBKDF2Request()", "Out of Memory");
    Wrap(object, this);
  }

  ~PBKDF2Request() override {
    release();
    ClearWrap(object());
    persistent().Reset();
  }

  uv_work_t* work_req() {
    return &work_req_;
  }

  inline const EVP_MD* digest() const {
    return digest_;
  }

  inline int passlen() const {
    return passlen_;
  }

  inline char* pass() const {
    return pass_;
  }

  inline int saltlen() const {
    return saltlen_;
  }

  inline char* salt() const {
    return salt_;
  }

  inline int keylen() const {
    return keylen_;
  }

  inline char* key() const {
    return key_;
  }

  inline int iter() const {
    return iter_;
  }

  inline void release() {
    free(pass_);
    pass_ = nullptr;
    passlen_ = 0;

    free(salt_);
    salt_ = nullptr;
    saltlen_ = 0;

    free(key_);
    key_ = nullptr;
    keylen_ = 0;
  }

  inline int error() const {
    return error_;
  }

  inline void set_error(int err) {
    error_ = err;
  }

  size_t self_size() const override { return sizeof(*this); }

  uv_work_t work_req_;

 private:
  const EVP_MD* digest_;
  int error_;
  int passlen_;
  char* pass_;
  int saltlen_;
  char* salt_;
  int keylen_;
  char* key_;
  int iter_;
};


void EIO_PBKDF2(PBKDF2Request* req) {
  req->set_error(PKCS5_PBKDF2_HMAC(
    req->pass(),
    req->passlen(),
    reinterpret_cast<unsigned char*>(req->salt()),
    req->saltlen(),
    req->iter(),
    req->digest(),
    req->keylen(),
    reinterpret_cast<unsigned char*>(req->key())));
  OPENSSL_cleanse(req->pass(), req->passlen());
  OPENSSL_cleanse(req->salt(), req->saltlen());
}


void EIO_PBKDF2(uv_work_t* work_req) {
  PBKDF2Request* req = ContainerOf(&PBKDF2Request::work_req_, work_req);
  EIO_PBKDF2(req);
}


void EIO_PBKDF2After(PBKDF2Request* req, Local<Value> argv[2]) {
  if (req->error()) {
    argv[0] = Undefined(req->env()->isolate());
    argv[1] = Encode(req->env()->isolate(), req->key(), req->keylen(), BUFFER);
    OPENSSL_cleanse(req->key(), req->keylen());
  } else {
    argv[0] = Exception::Error(req->env()->pbkdf2_error_string());
    argv[1] = Undefined(req->env()->isolate());
  }
}


void EIO_PBKDF2After(uv_work_t* work_req, int status) {
  CHECK_EQ(status, 0);
  PBKDF2Request* req = ContainerOf(&PBKDF2Request::work_req_, work_req);
  Environment* env = req->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());
  Local<Value> argv[2];
  EIO_PBKDF2After(req, argv);
  req->MakeCallback(env->ondone_string(), arraysize(argv), argv);
  delete req;
}


void PBKDF2(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  const EVP_MD* digest = nullptr;
  const char* type_error = nullptr;
  char* pass = nullptr;
  char* salt = nullptr;
  int passlen = -1;
  int saltlen = -1;
  double raw_keylen = -1;
  int keylen = -1;
  int iter = -1;
  PBKDF2Request* req = nullptr;
  Local<Object> obj;

  if (args.Length() != 5 && args.Length() != 6) {
    type_error = "Bad parameter";
    goto err;
  }

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Pass phrase");
  passlen = Buffer::Length(args[0]);
  if (passlen < 0) {
    type_error = "Bad password";
    goto err;
  }

  THROW_AND_RETURN_IF_NOT_BUFFER(args[1], "Salt");

  pass = static_cast<char*>(node::Malloc(passlen));
  if (pass == nullptr) {
    FatalError("node::PBKDF2()", "Out of Memory");
  }
  memcpy(pass, Buffer::Data(args[0]), passlen);

  saltlen = Buffer::Length(args[1]);
  if (saltlen < 0) {
    type_error = "Bad salt";
    goto err;
  }

  salt = static_cast<char*>(node::Malloc(saltlen));
  if (salt == nullptr) {
    FatalError("node::PBKDF2()", "Out of Memory");
  }
  memcpy(salt, Buffer::Data(args[1]), saltlen);

  if (!args[2]->IsNumber()) {
    type_error = "Iterations not a number";
    goto err;
  }

  iter = args[2]->Int32Value();
  if (iter < 0) {
    type_error = "Bad iterations";
    goto err;
  }

  if (!args[3]->IsNumber()) {
    type_error = "Key length not a number";
    goto err;
  }

  raw_keylen = args[3]->NumberValue();
  if (raw_keylen < 0.0 || std::isnan(raw_keylen) || std::isinf(raw_keylen) ||
      raw_keylen > INT_MAX) {
    type_error = "Bad key length";
    goto err;
  }

  keylen = static_cast<int>(raw_keylen);

  if (args[4]->IsString()) {
    node::Utf8Value digest_name(env->isolate(), args[4]);
    digest = EVP_get_digestbyname(*digest_name);
    if (digest == nullptr) {
      type_error = "Bad digest name";
      goto err;
    }
  }

  if (digest == nullptr) {
    digest = EVP_sha1();
  }

  obj = env->NewInternalFieldObject();
  req = new PBKDF2Request(env,
                          obj,
                          digest,
                          passlen,
                          pass,
                          saltlen,
                          salt,
                          iter,
                          keylen);

  if (args[5]->IsFunction()) {
    obj->Set(env->ondone_string(), args[5]);

    if (env->in_domain())
      obj->Set(env->domain_string(), env->domain_array()->Get(0));
    uv_queue_work(env->event_loop(),
                  req->work_req(),
                  EIO_PBKDF2,
                  EIO_PBKDF2After);
  } else {
    env->PrintSyncTrace();
    Local<Value> argv[2];
    EIO_PBKDF2(req);
    EIO_PBKDF2After(req, argv);

    delete req;

    if (argv[0]->IsObject())
      env->isolate()->ThrowException(argv[0]);
    else
      args.GetReturnValue().Set(argv[1]);
  }
  return;

 err:
  free(salt);
  free(pass);
  return env->ThrowTypeError(type_error);
}


// Only instantiate within a valid HandleScope.
class RandomBytesRequest : public AsyncWrap {
 public:
  RandomBytesRequest(Environment* env, Local<Object> object, size_t size)
      : AsyncWrap(env, object, AsyncWrap::PROVIDER_CRYPTO),
        error_(0),
        size_(size),
        data_(static_cast<char*>(node::Malloc(size))) {
    if (data() == nullptr)
      FatalError("node::RandomBytesRequest()", "Out of Memory");
    Wrap(object, this);
  }

  ~RandomBytesRequest() override {
    ClearWrap(object());
    persistent().Reset();
  }

  uv_work_t* work_req() {
    return &work_req_;
  }

  inline size_t size() const {
    return size_;
  }

  inline char* data() const {
    return data_;
  }

  inline void release() {
    free(data_);
    size_ = 0;
  }

  inline void return_memory(char** d, size_t* len) {
    *d = data_;
    data_ = nullptr;
    *len = size_;
    size_ = 0;
  }

  inline unsigned long error() const {  // NOLINT(runtime/int)
    return error_;
  }

  inline void set_error(unsigned long err) {  // NOLINT(runtime/int)
    error_ = err;
  }

  size_t self_size() const override { return sizeof(*this); }

  uv_work_t work_req_;

 private:
  unsigned long error_;  // NOLINT(runtime/int)
  size_t size_;
  char* data_;
};


void RandomBytesWork(uv_work_t* work_req) {
  RandomBytesRequest* req =
      ContainerOf(&RandomBytesRequest::work_req_, work_req);

  // Ensure that OpenSSL's PRNG is properly seeded.
  CheckEntropy();

  const int r = RAND_bytes(reinterpret_cast<unsigned char*>(req->data()),
                           req->size());

  // RAND_bytes() returns 0 on error.
  if (r == 0) {
    req->set_error(ERR_get_error());  // NOLINT(runtime/int)
  } else if (r == -1) {
    req->set_error(static_cast<unsigned long>(-1));  // NOLINT(runtime/int)
  }
}


// don't call this function without a valid HandleScope
void RandomBytesCheck(RandomBytesRequest* req, Local<Value> argv[2]) {
  if (req->error()) {
    char errmsg[256] = "Operation not supported";

    if (req->error() != static_cast<unsigned long>(-1))  // NOLINT(runtime/int)
      ERR_error_string_n(req->error(), errmsg, sizeof errmsg);

    argv[0] = Exception::Error(OneByteString(req->env()->isolate(), errmsg));
    argv[1] = Null(req->env()->isolate());
    req->release();
  } else {
    char* data = nullptr;
    size_t size;
    req->return_memory(&data, &size);
    argv[0] = Null(req->env()->isolate());
    argv[1] = Buffer::New(req->env(), data, size).ToLocalChecked();
  }
}


void RandomBytesAfter(uv_work_t* work_req, int status) {
  CHECK_EQ(status, 0);
  RandomBytesRequest* req =
      ContainerOf(&RandomBytesRequest::work_req_, work_req);
  Environment* env = req->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());
  Local<Value> argv[2];
  RandomBytesCheck(req, argv);
  req->MakeCallback(env->ondone_string(), arraysize(argv), argv);
  delete req;
}


void RandomBytes(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  // maybe allow a buffer to write to? cuts down on object creation
  // when generating random data in a loop
  if (!args[0]->IsUint32()) {
    return env->ThrowTypeError("size must be a number >= 0");
  }

  const int64_t size = args[0]->IntegerValue();
  if (size < 0 || size > Buffer::kMaxLength)
    return env->ThrowRangeError("size is not a valid Smi");

  Local<Object> obj = env->NewInternalFieldObject();
  RandomBytesRequest* req = new RandomBytesRequest(env, obj, size);

  if (args[1]->IsFunction()) {
    obj->Set(FIXED_ONE_BYTE_STRING(args.GetIsolate(), "ondone"), args[1]);

    if (env->in_domain())
      obj->Set(env->domain_string(), env->domain_array()->Get(0));
    uv_queue_work(env->event_loop(),
                  req->work_req(),
                  RandomBytesWork,
                  RandomBytesAfter);
    args.GetReturnValue().Set(obj);
  } else {
    env->PrintSyncTrace();
    Local<Value> argv[2];
    RandomBytesWork(req->work_req());
    RandomBytesCheck(req, argv);
    delete req;

    if (!argv[0]->IsNull())
      env->isolate()->ThrowException(argv[0]);
    else
      args.GetReturnValue().Set(argv[1]);
  }
}


void GetSSLCiphers(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  SSL_CTX* ctx = SSL_CTX_new(TLSv1_server_method());
  if (ctx == nullptr) {
    return env->ThrowError("SSL_CTX_new() failed.");
  }

  SSL* ssl = SSL_new(ctx);
  if (ssl == nullptr) {
    SSL_CTX_free(ctx);
    return env->ThrowError("SSL_new() failed.");
  }

  Local<Array> arr = Array::New(env->isolate());
  STACK_OF(SSL_CIPHER)* ciphers = SSL_get_ciphers(ssl);

  for (int i = 0; i < sk_SSL_CIPHER_num(ciphers); ++i) {
    const SSL_CIPHER* cipher = sk_SSL_CIPHER_value(ciphers, i);
    arr->Set(i, OneByteString(args.GetIsolate(), SSL_CIPHER_get_name(cipher)));
  }

  SSL_free(ssl);
  SSL_CTX_free(ctx);

  args.GetReturnValue().Set(arr);
}


class CipherPushContext {
 public:
  explicit CipherPushContext(Environment* env)
      : arr(Array::New(env->isolate())),
        env_(env) {
  }

  inline Environment* env() const { return env_; }

  Local<Array> arr;

 private:
  Environment* env_;
};


template <class TypeName>
static void array_push_back(const TypeName* md,
                            const char* from,
                            const char* to,
                            void* arg) {
  CipherPushContext* ctx = static_cast<CipherPushContext*>(arg);
  ctx->arr->Set(ctx->arr->Length(), OneByteString(ctx->env()->isolate(), from));
}


void GetCiphers(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CipherPushContext ctx(env);
  EVP_CIPHER_do_all_sorted(array_push_back<EVP_CIPHER>, &ctx);
  args.GetReturnValue().Set(ctx.arr);
}


void GetHashes(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CipherPushContext ctx(env);
  EVP_MD_do_all_sorted(array_push_back<EVP_MD>, &ctx);
  args.GetReturnValue().Set(ctx.arr);
}


void GetCurves(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  const size_t num_curves = EC_get_builtin_curves(nullptr, 0);
  Local<Array> arr = Array::New(env->isolate(), num_curves);
  EC_builtin_curve* curves;
  size_t alloc_size;

  if (num_curves) {
    alloc_size = sizeof(*curves) * num_curves;
    curves = static_cast<EC_builtin_curve*>(node::Malloc(alloc_size));

    CHECK_NE(curves, nullptr);

    if (EC_get_builtin_curves(curves, num_curves)) {
      for (size_t i = 0; i < num_curves; i++) {
        arr->Set(i, OneByteString(env->isolate(), OBJ_nid2sn(curves[i].nid)));
      }
    }

    free(curves);
  }

  args.GetReturnValue().Set(arr);
}


bool VerifySpkac(const char* data, unsigned int len) {
  bool i = 0;
  EVP_PKEY* pkey = nullptr;
  NETSCAPE_SPKI* spki = nullptr;

  spki = NETSCAPE_SPKI_b64_decode(data, len);
  if (spki == nullptr)
    goto exit;

  pkey = X509_PUBKEY_get(spki->spkac->pubkey);
  if (pkey == nullptr)
    goto exit;

  i = NETSCAPE_SPKI_verify(spki, pkey) > 0;

 exit:
  if (pkey != nullptr)
    EVP_PKEY_free(pkey);

  if (spki != nullptr)
    NETSCAPE_SPKI_free(spki);

  return i;
}


void VerifySpkac(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  bool i = false;

  if (args.Length() < 1)
    return env->ThrowTypeError("Data argument is mandatory");

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Data");

  size_t length = Buffer::Length(args[0]);
  if (length == 0)
    return args.GetReturnValue().Set(i);

  char* data = Buffer::Data(args[0]);
  CHECK_NE(data, nullptr);

  i = VerifySpkac(data, length);

  args.GetReturnValue().Set(i);
}


const char* ExportPublicKey(const char* data, int len) {
  char* buf = nullptr;
  EVP_PKEY* pkey = nullptr;
  NETSCAPE_SPKI* spki = nullptr;

  BIO* bio = BIO_new(BIO_s_mem());
  if (bio == nullptr)
    goto exit;

  spki = NETSCAPE_SPKI_b64_decode(data, len);
  if (spki == nullptr)
    goto exit;

  pkey = NETSCAPE_SPKI_get_pubkey(spki);
  if (pkey == nullptr)
    goto exit;

  if (PEM_write_bio_PUBKEY(bio, pkey) <= 0)
    goto exit;

  BIO_write(bio, "\0", 1);
  BUF_MEM* ptr;
  BIO_get_mem_ptr(bio, &ptr);

  buf = new char[ptr->length];
  memcpy(buf, ptr->data, ptr->length);

 exit:
  if (pkey != nullptr)
    EVP_PKEY_free(pkey);

  if (spki != nullptr)
    NETSCAPE_SPKI_free(spki);

  if (bio != nullptr)
    BIO_free_all(bio);

  return buf;
}


void ExportPublicKey(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  if (args.Length() < 1)
    return env->ThrowTypeError("Public key argument is mandatory");

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Public key");

  size_t length = Buffer::Length(args[0]);
  if (length == 0)
    return args.GetReturnValue().SetEmptyString();

  char* data = Buffer::Data(args[0]);
  CHECK_NE(data, nullptr);

  const char* pkey = ExportPublicKey(data, length);
  if (pkey == nullptr)
    return args.GetReturnValue().SetEmptyString();

  Local<Value> out = Encode(env->isolate(), pkey, strlen(pkey), BUFFER);

  delete[] pkey;

  args.GetReturnValue().Set(out);
}


const char* ExportChallenge(const char* data, int len) {
  NETSCAPE_SPKI* sp = nullptr;

  sp = NETSCAPE_SPKI_b64_decode(data, len);
  if (sp == nullptr)
    return nullptr;

  unsigned char* buf = nullptr;
  ASN1_STRING_to_UTF8(&buf, sp->spkac->challenge);

  NETSCAPE_SPKI_free(sp);

  return reinterpret_cast<const char*>(buf);
}


void ExportChallenge(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  if (args.Length() < 1)
    return env->ThrowTypeError("Challenge argument is mandatory");

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "Challenge");

  size_t len = Buffer::Length(args[0]);
  if (len == 0)
    return args.GetReturnValue().SetEmptyString();

  char* data = Buffer::Data(args[0]);
  CHECK_NE(data, nullptr);

  const char* cert = ExportChallenge(data, len);
  if (cert == nullptr)
    return args.GetReturnValue().SetEmptyString();

  Local<Value> outString = Encode(env->isolate(), cert, strlen(cert), BUFFER);

  OPENSSL_free(const_cast<char*>(cert));

  args.GetReturnValue().Set(outString);
}

void TimingSafeEqual(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  THROW_AND_RETURN_IF_NOT_BUFFER(args[0], "First argument");
  THROW_AND_RETURN_IF_NOT_BUFFER(args[1], "Second argument");

  size_t buf_length = Buffer::Length(args[0]);
  if (buf_length != Buffer::Length(args[1])) {
    return env->ThrowTypeError("Input buffers must have the same length");
  }

  const char* buf1 = Buffer::Data(args[0]);
  const char* buf2 = Buffer::Data(args[1]);

  return args.GetReturnValue().Set(CRYPTO_memcmp(buf1, buf2, buf_length) == 0);
}

void InitCryptoOnce() {
  SSL_load_error_strings();
  OPENSSL_no_config();

  // --openssl-config=...
  if (openssl_config != nullptr) {
    OPENSSL_load_builtin_modules();
#ifndef OPENSSL_NO_ENGINE
    ENGINE_load_builtin_engines();
#endif
    ERR_clear_error();
    CONF_modules_load_file(
        openssl_config,
        nullptr,
        CONF_MFLAGS_DEFAULT_SECTION);
    int err = ERR_get_error();
    if (0 != err) {
      fprintf(stderr,
              "openssl config failed: %s\n",
              ERR_error_string(err, NULL));
      CHECK_NE(err, 0);
    }
  }

  SSL_library_init();
  OpenSSL_add_all_algorithms();

  crypto_lock_init();
  CRYPTO_set_locking_callback(crypto_lock_cb);
  CRYPTO_THREADID_set_callback(crypto_threadid_cb);

#ifdef NODE_FIPS_MODE
  /* Override FIPS settings in cnf file, if needed. */
  unsigned long err = 0;  // NOLINT(runtime/int)
  if (enable_fips_crypto || force_fips_crypto) {
    if (0 == FIPS_mode() && !FIPS_mode_set(1)) {
      err = ERR_get_error();
    }
  }
  if (0 != err) {
    fprintf(stderr, "openssl fips failed: %s\n", ERR_error_string(err, NULL));
    UNREACHABLE();
  }
#endif  // NODE_FIPS_MODE


  // Turn off compression. Saves memory and protects against CRIME attacks.
  // No-op with OPENSSL_NO_COMP builds of OpenSSL.
  sk_SSL_COMP_zero(openssl_SSL_COMP_get_compression_methods());

#ifndef OPENSSL_NO_ENGINE
  ERR_load_ENGINE_strings();
  ENGINE_load_builtin_engines();
#endif  // !OPENSSL_NO_ENGINE
}


#ifndef OPENSSL_NO_ENGINE
void SetEngine(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(args.Length() >= 2 && args[0]->IsString());
  unsigned int flags = args[1]->Uint32Value();

  ClearErrorOnReturn clear_error_on_return;
  (void) &clear_error_on_return;  // Silence compiler warning.

  const node::Utf8Value engine_id(env->isolate(), args[0]);
  ENGINE* engine = ENGINE_by_id(*engine_id);

  // Engine not found, try loading dynamically
  if (engine == nullptr) {
    engine = ENGINE_by_id("dynamic");
    if (engine != nullptr) {
      if (!ENGINE_ctrl_cmd_string(engine, "SO_PATH", *engine_id, 0) ||
          !ENGINE_ctrl_cmd_string(engine, "LOAD", nullptr, 0)) {
        ENGINE_free(engine);
        engine = nullptr;
      }
    }
  }

  if (engine == nullptr) {
    int err = ERR_get_error();
    if (err == 0) {
      char tmp[1024];
      snprintf(tmp, sizeof(tmp), "Engine \"%s\" was not found", *engine_id);
      return env->ThrowError(tmp);
    } else {
      return ThrowCryptoError(env, err);
    }
  }

  int r = ENGINE_set_default(engine, flags);
  ENGINE_free(engine);
  if (r == 0)
    return ThrowCryptoError(env, ERR_get_error());
}
#endif  // !OPENSSL_NO_ENGINE

void GetFipsCrypto(const FunctionCallbackInfo<Value>& args) {
  if (FIPS_mode()) {
    args.GetReturnValue().Set(1);
  } else {
    args.GetReturnValue().Set(0);
  }
}

void SetFipsCrypto(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
#ifdef NODE_FIPS_MODE
  bool mode = args[0]->BooleanValue();
  if (force_fips_crypto) {
    return env->ThrowError(
        "Cannot set FIPS mode, it was forced with --force-fips at startup.");
  } else if (!FIPS_mode_set(mode)) {
    unsigned long err = ERR_get_error();  // NOLINT(runtime/int)
    return ThrowCryptoError(env, err);
  }
#else
  return env->ThrowError("Cannot set FIPS mode in a non-FIPS build.");
#endif /* NODE_FIPS_MODE */
}

// FIXME(bnoordhuis) Handle global init correctly.
void InitCrypto(Local<Object> target,
                Local<Value> unused,
                Local<Context> context,
                void* priv) {
  static uv_once_t init_once = UV_ONCE_INIT;
  uv_once(&init_once, InitCryptoOnce);

  Environment* env = Environment::GetCurrent(context);
  SecureContext::Initialize(env, target);
  Connection::Initialize(env, target);
  CipherBase::Initialize(env, target);
  DiffieHellman::Initialize(env, target);
  ECDH::Initialize(env, target);
  Hmac::Initialize(env, target);
  Hash::Initialize(env, target);
  Sign::Initialize(env, target);
  Verify::Initialize(env, target);

  env->SetMethod(target, "certVerifySpkac", VerifySpkac);
  env->SetMethod(target, "certExportPublicKey", ExportPublicKey);
  env->SetMethod(target, "certExportChallenge", ExportChallenge);
#ifndef OPENSSL_NO_ENGINE
  env->SetMethod(target, "setEngine", SetEngine);
#endif  // !OPENSSL_NO_ENGINE
  env->SetMethod(target, "getFipsCrypto", GetFipsCrypto);
  env->SetMethod(target, "setFipsCrypto", SetFipsCrypto);
  env->SetMethod(target, "PBKDF2", PBKDF2);
  env->SetMethod(target, "randomBytes", RandomBytes);
  env->SetMethod(target, "timingSafeEqual", TimingSafeEqual);
  env->SetMethod(target, "getSSLCiphers", GetSSLCiphers);
  env->SetMethod(target, "getCiphers", GetCiphers);
  env->SetMethod(target, "getHashes", GetHashes);
  env->SetMethod(target, "getCurves", GetCurves);
  env->SetMethod(target, "publicEncrypt",
                 PublicKeyCipher::Cipher<PublicKeyCipher::kPublic,
                                         EVP_PKEY_encrypt_init,
                                         EVP_PKEY_encrypt>);
  env->SetMethod(target, "privateDecrypt",
                 PublicKeyCipher::Cipher<PublicKeyCipher::kPrivate,
                                         EVP_PKEY_decrypt_init,
                                         EVP_PKEY_decrypt>);
  env->SetMethod(target, "privateEncrypt",
                 PublicKeyCipher::Cipher<PublicKeyCipher::kPrivate,
                                         EVP_PKEY_sign_init,
                                         EVP_PKEY_sign>);
  env->SetMethod(target, "publicDecrypt",
                 PublicKeyCipher::Cipher<PublicKeyCipher::kPublic,
                                         EVP_PKEY_verify_recover_init,
                                         EVP_PKEY_verify_recover>);
}

}  // namespace crypto
}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(crypto, node::crypto::InitCrypto)

#endif // NODE_UES_OPENSSL
