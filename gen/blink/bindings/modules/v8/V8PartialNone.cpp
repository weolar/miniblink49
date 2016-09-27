
#include "config.h"

#include "V8NavigatorPartial.h"
#include "bindings/core/v8/V8Navigator.h"
#include "V8ScreenPartial.h"
#include "bindings/core/v8/V8Screen.h"
#include "V8HTMLInputElementPartial.h"
#include "bindings/core/v8/V8HTMLInputElement.h"
#include "V8HTMLMediaElementPartial.h"
#include "bindings/core/v8/V8HTMLMediaElement.h"

#include "wtf/GetPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

// void V8NavigatorPartial::installV8NavigatorTemplate(v8::Local<v8::FunctionTemplate> functionTemplate, v8::Isolate* isolate)
// {
//     V8Navigator::installV8NavigatorTemplate(functionTemplate, isolate);
// }
// 
// void V8NavigatorPartial::preparePrototypeObject(v8::Isolate* isolate, v8::Local<v8::Object> prototypeObject, v8::Local<v8::FunctionTemplate> interfaceTemplate)
// {
//     V8Navigator::preparePrototypeObject(isolate, prototypeObject, interfaceTemplate);
// }
// 
// void V8NavigatorPartial::initialize()
// {
//     // Should be invoked from initModules.
//     V8Navigator::updateWrapperTypeInfo(
//         &V8NavigatorPartial::installV8NavigatorTemplate,
//         &V8NavigatorPartial::preparePrototypeObject);
// }

#define GEN_PARTIAL_NONE(type) \
\
void V8##type##Partial::installV8##type##Template(v8::Local<v8::FunctionTemplate> functionTemplate, v8::Isolate* isolate) \
{ \
    V8##type##::installV8##type##Template(functionTemplate, isolate); \
} \
\
void V8##type##Partial::preparePrototypeObject(v8::Isolate* isolate, v8::Local<v8::Object> prototypeObject, v8::Local<v8::FunctionTemplate> interfaceTemplate) \
{ \
    V8##type##::preparePrototypeObject(isolate, prototypeObject, interfaceTemplate); \
} \
\
void V8##type##Partial::initialize() \
{ \
    V8##type##::updateWrapperTypeInfo( \
        &V8##type##Partial::installV8##type##Template, \
        &V8##type##Partial::preparePrototypeObject); \
}

GEN_PARTIAL_NONE(HTMLInputElement)
GEN_PARTIAL_NONE(Screen)
GEN_PARTIAL_NONE(HTMLMediaElement)

} // namespace blink