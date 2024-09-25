
#include "default-platform-wrap.h"
#include "default-platform.h"
#include "include/v8.h"

namespace v8 {

void InitializeDefaultPlatform() {
  v8::V8::InitializePlatform(new v8::platform::DefaultPlatform());
}

}