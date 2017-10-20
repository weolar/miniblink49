#ifndef wkeJsBindFreeTempObject_h
#define wkeJsBindFreeTempObject_h

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

namespace wke {

void freeV8TempObejctOnOneFrameBefore();

class AutoDisableFreeV8TempObejct {
public:
    AutoDisableFreeV8TempObejct();
    ~AutoDisableFreeV8TempObejct();

    static bool isDisable() { return m_isDisable; }

private:
    static bool m_isDisable;
};

}

#endif
#endif // wkeJsBindFreeTempObject_h