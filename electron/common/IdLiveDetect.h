#ifndef common_IdLiveDetect_h
#define common_IdLiveDetect_h

#include <map>
#include <windows.h>

namespace atom {

class IdLiveDetect {
public:
    IdLiveDetect();
    ~IdLiveDetect();

    int constructed(void* ptr);
    void deconstructed(int id);

    void* getPtrById(int id);

    bool isLive(int id);
    static IdLiveDetect* get();

private:
    int m_idGen;
    std::map<int, void*> m_liveSelf;
    CRITICAL_SECTION m_liveSelfLock;
    static IdLiveDetect* m_inst;
};   

}

#endif // common_IdLiveDetect_h