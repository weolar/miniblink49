#ifndef net_StorageDef_h
#define net_StorageDef_h

#include "wtf/HashMap.h"
#include "wtf/text/WTFString.h"
#include "wtf/text/StringHash.h"

namespace net {

// The quota for each storage area.
// This value is enforced in renderer processes and the browser process.
const size_t kPerStorageAreaQuota = 10 * 1024 * 1024;

// In the browser process we allow some overage to
// accomodate concurrent writes from different renderers
// that were allowed because the limit imposed in the renderer
// wasn't exceeded.
const size_t kPerStorageAreaOverQuotaAllowance = 100 * 1024;

// Value to indicate the localstorage namespace vs non-zero
// values for sessionstorage namespaces.
const int64 kLocalStorageNamespaceId = 0;

const int64 kInvalidSessionStorageNamespaceId = kLocalStorageNamespaceId;

// Start purging memory if the number of in-memory areas exceeds this.
const int64 kMaxInMemoryStorageAreas = 100;

typedef HashMap<String, HashMap<String, String>* > DOMStorageMap;

}

#endif