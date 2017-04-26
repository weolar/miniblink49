// Copyright 2006-2008 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/base/logging.h"

#if V8_LIBC_GLIBC || V8_OS_BSD
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#elif V8_OS_QNX
#include <backtrace.h>
#endif  // V8_LIBC_GLIBC || V8_OS_BSD

#include <cstdio>
#include <cstdlib>

#include "src/base/platform/platform.h"

namespace v8 {
namespace base {

// Explicit instantiations for commonly used comparisons.
#define DEFINE_MAKE_CHECK_OP_STRING(type)              \
  template std::string* MakeCheckOpString<type, type>( \
      type const&, type const&, char const*);
DEFINE_MAKE_CHECK_OP_STRING(int)
DEFINE_MAKE_CHECK_OP_STRING(long)       // NOLINT(runtime/int)
//DEFINE_MAKE_CHECK_OP_STRING(long long)  // NOLINT(runtime/int)
DEFINE_MAKE_CHECK_OP_STRING(unsigned int)
DEFINE_MAKE_CHECK_OP_STRING(unsigned long)       // NOLINT(runtime/int)
//DEFINE_MAKE_CHECK_OP_STRING(unsigned long long)  // NOLINT(runtime/int)
DEFINE_MAKE_CHECK_OP_STRING(char const*)
DEFINE_MAKE_CHECK_OP_STRING(void const*)
#undef DEFINE_MAKE_CHECK_OP_STRING


// Explicit instantiations for floating point checks.
#define DEFINE_CHECK_OP_IMPL(NAME)                          \
  template std::string* Check##NAME##Impl<float, float>(    \
      float const& lhs, float const& rhs, char const* msg); \
  template std::string* Check##NAME##Impl<double, double>(  \
      double const& lhs, double const& rhs, char const* msg);
DEFINE_CHECK_OP_IMPL(EQ)
DEFINE_CHECK_OP_IMPL(NE)
DEFINE_CHECK_OP_IMPL(LE)
DEFINE_CHECK_OP_IMPL(LT)
DEFINE_CHECK_OP_IMPL(GE)
DEFINE_CHECK_OP_IMPL(GT)
#undef DEFINE_CHECK_OP_IMPL


// Attempts to dump a backtrace (if supported).
void DumpBacktrace() {
#if V8_LIBC_GLIBC || V8_OS_BSD
  void* trace[100];
  int size = backtrace(trace, arraysize(trace));
  OS::PrintError("\n==== C stack trace ===============================\n\n");
  if (size == 0) {
    OS::PrintError("(empty)\n");
  } else {
    for (int i = 1; i < size; ++i) {
      OS::PrintError("%2d: ", i);
      Dl_info info;
      char* demangled = NULL;
      if (!dladdr(trace[i], &info) || !info.dli_sname) {
        OS::PrintError("%p\n", trace[i]);
      } else if ((demangled = abi::__cxa_demangle(info.dli_sname, 0, 0, 0))) {
        OS::PrintError("%s\n", demangled);
        free(demangled);
      } else {
        OS::PrintError("%s\n", info.dli_sname);
      }
    }
  }
#elif V8_OS_QNX
  char out[1024];
  bt_accessor_t acc;
  bt_memmap_t memmap;
  bt_init_accessor(&acc, BT_SELF);
  bt_load_memmap(&acc, &memmap);
  bt_sprn_memmap(&memmap, out, sizeof(out));
  OS::PrintError(out);
  bt_addr_t trace[100];
  int size = bt_get_backtrace(&acc, trace, arraysize(trace));
  OS::PrintError("\n==== C stack trace ===============================\n\n");
  if (size == 0) {
    OS::PrintError("(empty)\n");
  } else {
    bt_sprnf_addrs(&memmap, trace, size, const_cast<char*>("%a\n"),
                   out, sizeof(out), NULL);
    OS::PrintError(out);
  }
  bt_unload_memmap(&memmap);
  bt_release_accessor(&acc);
#endif  // V8_LIBC_GLIBC || V8_OS_BSD
}

#if USING_VC6RT == 1
std::basic_string<char, std::char_traits<char>, class std::allocator<char> >* MakeCheckOpString<__int64, __int64>(__int64 const & lhs, __int64 const & rhs, char const * msg) {
    char* buf = new char[2000];
    sprintf(buf, " (%I64d vs. %I64d)", lhs, rhs);
    std::ostringstream ss;
    ss << msg << buf;
    delete[] buf;
    return new std::string(ss.str());
}
#endif

}  // namespace base
}  // namespace v8


// Contains protection against recursive calls (faults while handling faults).
extern "C" void V8_Fatal(const char* file, int line, const char* format, ...) {
  fflush(stdout);
  fflush(stderr);
  v8::base::OS::PrintError("\n\n#\n# Fatal error in %s, line %d\n# ", file,
                           line);
  va_list arguments;
  va_start(arguments, format);
  v8::base::OS::VPrintError(format, arguments);
  va_end(arguments);
  v8::base::OS::PrintError("\n#\n");
  v8::base::DumpBacktrace();
  fflush(stderr);
  v8::base::OS::Abort();
}
