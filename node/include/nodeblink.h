#define NODE_ARCH "ia32"
#define NODE_PLATFORM "win32"
#define NODE_WANT_INTERNALS 1
#define HAVE_OPENSSL 1
#define HAVE_ETW 1
#define HAVE_PERFCTR 1
#define V8_INSPECTOR_USE_STL 1
#define NODE_USE_V8_PLATFORM 0
#define USING_V8_SHARED 1
#define USING_UV_SHARED 1
#define CARES_BUILDING_LIBRARY 0
#include "node.h"
#include "env.h"
#include "env-inl.h"
#include "uv.h"
struct node_native {
	const char* name;
	const char* source;
	size_t source_len;
};
namespace node {
	struct nodeargc;
	typedef void(*nodeInitCallBack)(nodeargc*);
	typedef struct nodeargc {
		char** argv;
		int argc;
		uv_loop_t *child_loop_;
		Environment* child_env_;
		nodeInitCallBack initcall;
	};

	NODE_EXTERN nodeargc* RunNodeThread(int argc, wchar_t *wargv[], nodeInitCallBack initcall);
	NODE_EXTERN Environment* NodeGetEnvironment(nodeargc*);
}