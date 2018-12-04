
#ifndef net_CancelledReason_h
#define net_CancelledReason_h

namespace net {

enum CancelledReason {
    kNoCancelled,
    kNormalCancelled,
    kHookRedirectCancelled,
};

}

#endif // net_CancelledReason_h