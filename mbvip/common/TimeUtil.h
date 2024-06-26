
#ifndef common_TimeUtil_h
#define common_TimeUtil_h

#include <windows.h>
#include <time.h>

namespace common {

inline int64_t myGetTime()
{
    const int64_t msPerSecond = 1000;
    SYSTEMTIME systemTime;
    ::GetSystemTime(&systemTime);
    struct tm tmtime;
    tmtime.tm_year = systemTime.wYear - 1900;
    tmtime.tm_mon = systemTime.wMonth - 1;
    tmtime.tm_mday = systemTime.wDay;
    tmtime.tm_wday = systemTime.wDayOfWeek;
    tmtime.tm_hour = systemTime.wHour;
    tmtime.tm_min = systemTime.wMinute;
    tmtime.tm_sec = systemTime.wSecond;
    time_t timet = mktime(&tmtime);
    return timet * msPerSecond + systemTime.wMilliseconds;
}

inline void msSleep(DWORD milliseconds)
{
    int count = 0;
    int64_t tick = myGetTime();
    while (true) {
        ::Sleep(1);
        count++;
        int64_t tick2 = myGetTime();
        if (tick2 - tick > milliseconds)
            break;
    }
}

}

#endif // common_TimeUtil_h