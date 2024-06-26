#ifndef verify_SqBind_h
#define verify_SqBind_h

#include "squirrel.h"
#include <string>

class SqBind {
public:
    SqBind();
    ~SqBind();

    static SqBind* getInst();

    void initSqScript();
    void uninit();
    void timerFire();
    std::string createRequestCode(const char* regInfo);
    std::string createLicense(const std::string& requestCode);
    bool verifyLicense(const std::string& license);
    bool loadLicenseAndVerify(const std::wstring& defaultPath);
    static bool verifyLicenseImpl(HSQUIRRELVM v, const std::string& license);

private:
    static SqBind* m_inst;
    HSQUIRRELVM m_v;
};

#endif // verify_SqBind_h