#include "env.h"
#include "env-inl.h"
#include "v8.h"
#include "net/ActivatingObjCheck.h"

#if defined(_MSC_VER)
#define getpid GetCurrentProcessId
#else
#include <unistd.h>
#endif

#include <stdio.h>

#ifndef MINIBLINK_NOT_IMPLEMENTED
#include "node/nodeblink.h"
#include <list>
#include <algorithm>
#endif

namespace node {

using v8::HandleScope;
using v8::Local;
using v8::Message;
using v8::StackFrame;
using v8::StackTrace;

void Environment::PrintSyncTrace() const {
  if (!trace_sync_io_)
    return;

  HandleScope handle_scope(isolate());
  Local<v8::StackTrace> stack =
      StackTrace::CurrentStackTrace(isolate(), 10, StackTrace::kDetailed);

  fprintf(stderr, "(node:%d) WARNING: Detected use of sync API\n", getpid());

  for (int i = 0; i < stack->GetFrameCount() - 1; i++) {
    Local<StackFrame> stack_frame = stack->GetFrame(i);
    node::Utf8Value fn_name_s(isolate(), stack_frame->GetFunctionName());
    node::Utf8Value script_name(isolate(), stack_frame->GetScriptName());
    const int line_number = stack_frame->GetLineNumber();
    const int column = stack_frame->GetColumn();

    if (stack_frame->IsEval()) {
      if (stack_frame->GetScriptId() == Message::kNoScriptIdInfo) {
        fprintf(stderr, "    at [eval]:%i:%i\n", line_number, column);
      } else {
        fprintf(stderr,
                "    at [eval] (%s:%i:%i)\n",
                *script_name,
                line_number,
                column);
      }
      break;
    }

    if (fn_name_s.length() == 0) {
      fprintf(stderr, "    at %s:%i:%i\n", *script_name, line_number, column);
    } else {
      fprintf(stderr,
              "    at %s (%s:%i:%i)\n",
              *fn_name_s,
              *script_name,
              line_number,
              column);
    }
  }
  fflush(stderr);
}

void Environment::AddCleanupHook(void(*fn)(void*), void* arg) {
    CleanupHookCallback* cb = new CleanupHookCallback(fn, arg, cleanup_hook_counter_++);
    if (!cleanup_hooks_)
        cleanup_hooks_ = new std::vector<CleanupHookCallback*>();
    cleanup_hooks_->push_back(cb);
}

void Environment::RemoveCleanupHook(void(*fn)(void*), void* arg) {   
    for (size_t i = 0; i < cleanup_hooks_->size(); ++i) {
        CleanupHookCallback* hook = cleanup_hooks_->at(i);
        if (hook->fn_ == fn && hook->arg_ == arg) {
            cleanup_hooks_->erase(cleanup_hooks_->begin() + i);
            delete hook;
            return;
        }
    }
}

void Environment::InitEnv() {
    debugger_agent_ = new debugger::Agent(this);
#if HAVE_INSPECTOR
    inspector_agent_ = new debugger::Agent(this);
#endif

    destroy_ids_list_ = new std::vector<int64_t>();
    destroy_ids_list_->reserve(512);
}

void Environment::CleanEnv() {
    CleanupHandles();

    delete debugger_agent_;
#if HAVE_INSPECTOR
    delete inspector_agent_;
#endif

    delete destroy_ids_list_;

    while (get_cleanup_hooks() && !get_cleanup_hooks()->empty()) {
        // Copy into a vector, since we can't sort an unordered_set in-place.
        std::vector<Environment::CleanupHookCallback*> callbacks(get_cleanup_hooks()->begin(), get_cleanup_hooks()->end());
        // We can't erase the copied elements from `cleanup_hooks_` yet, because we
        // need to be able to check whether they were un-scheduled by another hook.
        std::sort(callbacks.begin(), callbacks.end(), &Environment::CleanupHookCallback::CompareGT);

        for (size_t i = 0; i < callbacks.size(); ++i) {
            const Environment::CleanupHookCallback* cb = callbacks[i];

            bool find = false;
            for (size_t j = 0; j < get_cleanup_hooks()->size(); ++j) {
                const Environment::CleanupHookCallback* cb2 = get_cleanup_hooks()->at(j);
                if (cb2 != cb)
                    continue;
                find = true;
                break;
            }
            if (!find) {
                // This hook was removed from the `cleanup_hooks_` set during another
                // hook that was run earlier. Nothing to do here.
                continue;
            }

            cb->fn_(cb->arg_);
            RemoveCleanupHook(cb->fn_, cb->arg_);
        }
        CleanupHandles();
    }
}

void AddLiveSet(intptr_t obj) {
    net::ActivatingObjCheck::inst()->add((intptr_t)obj);
}

void RemoveLiveSet(intptr_t obj) {
    net::ActivatingObjCheck::inst()->remove((intptr_t)obj);
}

bool IsLiveObj(intptr_t obj) {
    return net::ActivatingObjCheck::inst()->isActivating((intptr_t)obj);
}

#ifndef MINIBLINK_NOT_IMPLEMENTED

void BlinkMicrotaskSuppressionEnterFunc(Environment* self) {
    if (!self->blink_microtask_suppression_handle_)
        self->blink_microtask_suppression_handle_ = new std::list<BlinkMicrotaskSuppressionHandle>();
    std::list<BlinkMicrotaskSuppressionHandle>* handleList = (std::list<BlinkMicrotaskSuppressionHandle>*)self->blink_microtask_suppression_handle_;
    handleList->push_back(nodeBlinkMicrotaskSuppressionEnter(self->isolate()));
}

void BlinkMicrotaskSuppressionLeaveFunc(Environment* self) {
    std::list<BlinkMicrotaskSuppressionHandle>* handleList = (std::list<BlinkMicrotaskSuppressionHandle>*)self->blink_microtask_suppression_handle_;
    BlinkMicrotaskSuppressionHandle handle = (handleList->back());
    handleList->pop_back();
    
    if (0 == handleList->size()) {
        delete handleList;
        self->blink_microtask_suppression_handle_ = nullptr;
    }

    nodeBlinkMicrotaskSuppressionLeave(handle);
}

void Environment::InitBlinkMicrotaskSuppression() {
    BlinkMicrotaskSuppressionEnter = BlinkMicrotaskSuppressionEnterFunc;
    BlinkMicrotaskSuppressionLeave = BlinkMicrotaskSuppressionLeaveFunc;
}
#endif

}  // namespace node
