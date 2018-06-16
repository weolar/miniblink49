#include "env.h"
#include "env-inl.h"
#include "v8.h"

#if defined(_MSC_VER)
#define getpid GetCurrentProcessId
#else
#include <unistd.h>
#endif

#include <stdio.h>

#ifndef MINIBLINK_NOT_IMPLEMENTED
#include "node/nodeblink.h"
#include <list>
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

#ifndef MINIBLINK_NOT_IMPLEMENTED
// Environment::MicrotaskSuppressionHandle Environment::BlinkMicrotaskSuppressionEnter(v8::Isolate* isolate) {
//     return nodeBlinkMicrotaskSuppressionEnter(isolate);
// }
// 
// void Environment::BlinkMicrotaskSuppressionLeave(MicrotaskSuppressionHandle handle) {
//     nodeBlinkMicrotaskSuppressionLeave(handle);
// }

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
