
#define __INIT_node_perfctr_provider_IMP
#include "node_counters.h"
#include "node_win32_perfctr_provider.h"

#include <perflib.h>

#include "node_perfctr_provider.h"

namespace node {

void NODE_COUNT_GC_PERCENTTIME(unsigned int percent)
{
}

uint64_t NODE_COUNT_GET_GC_RAWTIME()
{
    return 0;
}

void NODE_COUNT_HTTP_CLIENT_REQUEST()
{
}

void NODE_COUNT_HTTP_CLIENT_RESPONSE()
{
}

void NODE_COUNT_HTTP_SERVER_REQUEST()
{
}

void NODE_COUNT_HTTP_SERVER_RESPONSE()
{
}

void NODE_COUNT_NET_BYTES_RECV(int bytes)
{
}

void NODE_COUNT_NET_BYTES_SENT(int bytes)
{
}

void NODE_COUNT_PIPE_BYTES_RECV(int bytes)
{
}

void NODE_COUNT_PIPE_BYTES_SENT(int bytes)
{
}

void NODE_COUNT_SERVER_CONN_CLOSE()
{
}

void NODE_COUNT_SERVER_CONN_OPEN()
{
}

void InitPerfCountersWin32()
{
}

void TermPerfCountersWin32()
{
}

}