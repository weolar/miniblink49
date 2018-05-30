#ifndef wkeJsBind_h
#define wkeJsBind_h

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

#include "wke/wkeJsBindFreeTempObject.h"

namespace blink {
class WebLocalFrame;
}

namespace content {
class WebFrameClientImpl;
}

namespace wke {

void onCreateGlobalObjectInMainFrame(content::WebFrameClientImpl* client, blink::WebLocalFrame* frame, v8::Local<v8::Context> context, int extensionGroup, int worldId);
void onCreateGlobalObjectInSubFrame(content::WebFrameClientImpl* client, blink::WebLocalFrame* frame, v8::Local<v8::Context> context, int extensionGroup, int worldId);
void onReleaseGlobalObject(content::WebFrameClientImpl* client, blink::WebLocalFrame* frame, v8::Local<v8::Context> context, int worldId);
jsExecState createTempExecStateByV8Context(v8::Local<v8::Context> context);
jsValue createJsValueString(v8::Local<v8::Context> context, const utf8* str);
jsValue v8ValueToJsValue(v8::Local<v8::Context> context, v8::Local<v8::Value> v8Value);

}

#endif

#endif // wkeJsBind_h