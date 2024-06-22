
#ifndef browser_api_ProtocolInterface_h
#define browser_api_ProtocolInterface_h

namespace atom {

class ProtocolInterface {
public:
    static ProtocolInterface* inst()
    {
        return m_inst;
    }

    virtual bool handleLoadUrlBegin(void* param, const char* url, void* job) = 0;
    virtual ~ProtocolInterface()
    {

    }

    virtual v8::Local<v8::Object> getWrapper(v8::Isolate* isolate) = 0;

protected:
    static ProtocolInterface* m_inst;
};

}

#endif // browser_api_ProtocolInterface_h