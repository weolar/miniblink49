#pragma once

#include "wke.h"
#include "nodeblink.h"

namespace atom {

typedef void *(*CoreMainTask)(void *data);//主线程任务回调

//调用主线程任务
void mainAsyncCall(CoreMainTask call, void *data);

//等待主线程任务完成
void* mainAsyncWait();

void* mainSyncCall(CoreMainTask call, void* data);

} // atom