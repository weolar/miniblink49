
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

// #define GEN_PARTIAL_NONE(type) \
// \
// void V8##type##Partial::installV8##type##Template(v8::Local<v8::FunctionTemplate> functionTemplate, v8::Isolate* isolate) \
// { \
//     V8##type##::installV8##type##Template(functionTemplate, isolate); \
// } \
// \
// void V8##type##Partial::preparePrototypeObject(v8::Isolate* isolate, v8::Local<v8::Object> prototypeObject, v8::Local<v8::FunctionTemplate> interfaceTemplate) \
// { \
//     V8##type##::preparePrototypeObject(isolate, prototypeObject, interfaceTemplate); \
// } \
// \
// void V8##type##Partial::initialize() \
// { \
//     V8##type##::updateWrapperTypeInfo( \
//         &V8##type##Partial::installV8##type##Template, \
//         &V8##type##Partial::preparePrototypeObject); \
// }
// 
// GEN_PARTIAL_NONE(HTMLInputElement)
// GEN_PARTIAL_NONE(Screen)
// GEN_PARTIAL_NONE(HTMLMediaElement)


void V8HTMLInputElementPartial::installV8HTMLInputElementTemplate(v8::Local<v8::FunctionTemplate> functionTemplate, v8::Isolate* isolate)
{
    V8HTMLInputElement::installV8HTMLInputElementTemplate(functionTemplate, isolate);
}

void V8HTMLInputElementPartial::preparePrototypeObject(v8::Isolate* isolate, v8::Local<v8::Object> prototypeObject, v8::Local<v8::FunctionTemplate> interfaceTemplate)
{
    V8HTMLInputElement::preparePrototypeObject(isolate, prototypeObject, interfaceTemplate);
}

void V8HTMLInputElementPartial::initialize()
{
    V8HTMLInputElement::updateWrapperTypeInfo(
        &V8HTMLInputElementPartial::installV8HTMLInputElementTemplate,
        &V8HTMLInputElementPartial::preparePrototypeObject);
}

//////////////////////////////////////////////////////////////////////////

void V8ScreenPartial::installV8ScreenTemplate(v8::Local<v8::FunctionTemplate> functionTemplate, v8::Isolate* isolate)
{
    V8Screen::installV8ScreenTemplate(functionTemplate, isolate);
}

void V8ScreenPartial::preparePrototypeObject(v8::Isolate* isolate, v8::Local<v8::Object> prototypeObject, v8::Local<v8::FunctionTemplate> interfaceTemplate)
{
    V8Screen::preparePrototypeObject(isolate, prototypeObject, interfaceTemplate);
}

void V8ScreenPartial::initialize()
{
    V8Screen::updateWrapperTypeInfo(
        &V8ScreenPartial::installV8ScreenTemplate,
        &V8ScreenPartial::preparePrototypeObject);
}

//////////////////////////////////////////////////////////////////////////

void V8HTMLMediaElementPartial::installV8HTMLMediaElementTemplate(v8::Local<v8::FunctionTemplate> functionTemplate, v8::Isolate* isolate)
{
    V8HTMLMediaElement::installV8HTMLMediaElementTemplate(functionTemplate, isolate);
}

void V8HTMLMediaElementPartial::preparePrototypeObject(v8::Isolate* isolate, v8::Local<v8::Object> prototypeObject, v8::Local<v8::FunctionTemplate> interfaceTemplate)
{
    V8HTMLMediaElement::preparePrototypeObject(isolate, prototypeObject, interfaceTemplate);
}

void V8HTMLMediaElementPartial::initialize()
{
    V8HTMLMediaElement::updateWrapperTypeInfo(
        &V8HTMLMediaElementPartial::installV8HTMLMediaElementTemplate,
        &V8HTMLMediaElementPartial::preparePrototypeObject);
}

} // namespace blink