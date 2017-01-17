#pragma once

#include "wke.h"
#include "nodeblink.h"

namespace atom {

typedef void *(*CoreMainTask)(void *data);//主线程任务回调

//调用主线程任务
void callUiThreadAsync(CoreMainTask call, void *data);

//等待主线程任务完成
void* waitForCallUiThreadAsync();

void* callUiThreadSync(CoreMainTask call, void* data);
bool callUiThreadSync(v8::FunctionCallback call, const v8::FunctionCallbackInfo<v8::Value>& args);
} // atom