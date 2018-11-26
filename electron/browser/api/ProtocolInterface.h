
#ifndef net_ProtocolInterface_h
#define net_ProtocolInterface_h

namespace atom {

class ProtocolInterface {
public:
    static ProtocolInterface* inst() {
        return m_inst;
    }

    virtual bool handleLoadUrlBegin(void* param, const char* url, void* job) = 0;

protected:
    static ProtocolInterface* m_inst;
};

}

#endif // net_ProtocolInterface_h