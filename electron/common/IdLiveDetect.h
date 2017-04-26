#ifndef common_IdLiveDetect_h
#define common_IdLiveDetect_h

#include <set>
#include <windows.h>

namespace atom {

class IdLiveDetect {
public:
    IdLiveDetect();
    ~IdLiveDetect();

    int constructed();
    void deconstructed(int id);

    bool isLive(int id);
    static IdLiveDetect* get();

private:
    int m_idGen;
    std::set<int> m_liveSelf;
    CRITICAL_SECTION m_liveSelfLock;
    static IdLiveDetect* m_inst;
};   

}

#endif // common_IdLiveDetect_h