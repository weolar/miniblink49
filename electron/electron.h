#pragma once
#include "wke.h"
#include "nodeblink.h"
typedef void *(*core_main_task)(void *data);//主线程任务回调
//调用主线程任务
void main_async_call(core_main_task call, void *data);
//等待主线程任务完成
void *main_async_wait();