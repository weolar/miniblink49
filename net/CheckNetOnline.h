
#ifndef net_CheckNetOnline_h
#define net_CheckNetOnline_h

#include <Netlistmgr.h>

static BOOL checkIsNetwork(INetworkListManager* pNetworkListManager)
{
    static DWORD s_lastCheck = 0;
    DWORD lastCheck = ::GetTickCount();
    if (lastCheck - s_lastCheck < 15000)
        return -1;
    s_lastCheck = lastCheck;

    if (!pNetworkListManager)
        return -1;

    HRESULT hresult = E_FAIL;
    BOOL isOnline = FALSE;
    VARIANT_BOOL isConnect = VARIANT_FALSE;
    if (pNetworkListManager)
        hresult = pNetworkListManager->get_IsConnectedToInternet(&isConnect);
    if (SUCCEEDED(hresult))
        isOnline = (isConnect == VARIANT_TRUE) ? TRUE : FALSE;

    return isOnline;
}

static INetworkListManager* getNetworkList(IUnknown** ppUnknown)
{
    const GUID CLSID_NetworkListManagerXp = { 0xdcb00c01, 0x570f, 0x4a9b, { 0x8d, 0x69, 0x19, 0x9f, 0xdb, 0xa5, 0x72, 0x3b } };
    const IID IID_INetworkListManagerXp = { 0xdcb00000, 0x570f, 0x4a9b, { 0x8d, 0x69, 0x19, 0x9f, 0xdb, 0xa5, 0x72, 0x3b } };

    BOOL bOnline = TRUE;
    HRESULT hresult = CoCreateInstance(CLSID_NetworkListManagerXp, NULL, CLSCTX_ALL, IID_IUnknown, (void**)ppUnknown);
    if (!(SUCCEEDED(hresult)))
        return nullptr;

    INetworkListManager* pNetworkListManager = NULL;
    if (*ppUnknown)
        hresult = (*ppUnknown)->QueryInterface(IID_INetworkListManagerXp, (void**)&pNetworkListManager);

    if (SUCCEEDED(hresult))
        return pNetworkListManager;

    return nullptr;
}

#endif // net_CheckNetOnline_h