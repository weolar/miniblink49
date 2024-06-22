
#ifndef common_api_GetConstructorFromTls_h
#define common_api_GetConstructorFromTls_h

namespace atom {

class V8PersistentTls {
public:
    static v8::Persistent<v8::Function>* get(DWORD* constructorTlsKey)
    {
        v8::Persistent<v8::Function>* constructor = nullptr;
        if (0 == *constructorTlsKey)
            *constructorTlsKey = ::TlsAlloc();

        constructor = (v8::Persistent<v8::Function>*)::TlsGetValue(*constructorTlsKey);
        if (!constructor) {
            constructor = new v8::Persistent<v8::Function>();
            ::TlsSetValue(*constructorTlsKey, constructor);
        }
        return constructor;
    }
};

}

#endif // common_api_GetConstructorFromTls_h