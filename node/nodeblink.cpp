#include "include/nodeblink.h"

#include <string.h>
#include <Windows.h>
#include <process.h>

#pragma comment(lib,"openssl.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Psapi.lib")
namespace node {

	void ChildSignalCb(uv_async_t* signal) {

	}
	void WorkerRun(nodeargc *p) {
		v8::Isolate::CreateParams params;
		node::ArrayBufferAllocator array_buffer_allocator;
		params.array_buffer_allocator = &array_buffer_allocator;
		v8::Isolate* isolate = v8::Isolate::New(params);
		{
			v8::Isolate::Scope isolate_scope(isolate);

			v8::HandleScope handle_scope(isolate);
			v8::Local<v8::Context> context = v8::Context::New(isolate);

			v8::Context::Scope context_scope(context);
			node::Environment* env = node::CreateEnvironment(
				isolate,
				p->child_loop_,
				context,
				p->argc,
				p->argv,
				p->argc,
				p->argv);

			p->child_env_ = env;

			// Expose API
			LoadEnvironment(env);

			if (p->initcall)
				p->initcall(p);
			CHECK_EQ(p->child_loop_, env->event_loop());
			uv_run(p->child_loop_, UV_RUN_DEFAULT);

			// Clean-up all running handles
			env->CleanupHandles();

			env->Dispose();
			env = nullptr;
		}
		isolate->Dispose();
	}


	extern "C" NODE_EXTERN nodeargc* RunNodeThread(int argc, wchar_t *wargv[], nodeInitCallBack initcall) {
		nodeargc *p = (nodeargc *)malloc(sizeof(nodeargc));
		memset(p, 0, sizeof(nodeargc));
		p->initcall = initcall;
		p->child_loop_ = (uv_loop_t *)malloc(sizeof(uv_loop_t));
		p->argv = new char*[argc + 1];
		for (int i = 0; i < argc; i++) {
			// Compute the size of the required buffer
			DWORD size = WideCharToMultiByte(CP_UTF8,
				0,
				wargv[i],
				-1,
				nullptr,
				0,
				nullptr,
				nullptr);
			if (size == 0) {
				// This should never happen.
				fprintf(stderr, "Could not convert arguments to utf8.");
				exit(1);
			}
			// Do the actual conversion
			p->argv[i] = new char[size];
			DWORD result = WideCharToMultiByte(CP_UTF8,
				0,
				wargv[i],
				-1,
				p->argv[i],
				size,
				nullptr,
				nullptr);
			if (result == 0) {
				// This should never happen.
				fprintf(stderr, "Could not convert arguments to utf8.");
				exit(1);
			}
		}
		p->argv[argc] = nullptr;
		p->argc = argc;


		int err;

		uv_async_t child_signal_;
		uv_thread_t thread_;
		err = uv_loop_init(p->child_loop_);
		if (err != 0)
			goto loop_init_failed;

		// Interruption signal handler
		err = uv_async_init(p->child_loop_, &child_signal_, ChildSignalCb);
		if (err != 0)
			goto async_init_failed;
		uv_unref(reinterpret_cast<uv_handle_t*>(&child_signal_));

		err = uv_thread_create(&thread_, reinterpret_cast<uv_thread_cb>(WorkerRun), p);
		if (err != 0)
			goto thread_create_failed;

		return p;

	thread_create_failed:
		uv_close(reinterpret_cast<uv_handle_t*>(&child_signal_), nullptr);

	async_init_failed:
		err = uv_loop_close(p->child_loop_);
		CHECK_EQ(err, 0);

	loop_init_failed:
		free(p);
		return NULL;
	}
	extern "C" NODE_EXTERN Environment* NodeGetEnvironment(nodeargc* p) {
		if (p)
		{
			return p->child_env_;
		}
		return nullptr;
	}
}