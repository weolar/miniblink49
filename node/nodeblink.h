#include "node.h"
#include "env.h"
#include "env-inl.h"
#include "uv.h"
namespace node {
	typedef struct {
		char** argv;
		int argc;
		uv_loop_t *child_loop_;
		Environment* child_env_;
	} nodeargc;

	NODE_EXTERN nodeargc* RunNodeThread(int argc, wchar_t *wargv[]);
}