typedef void (__cdecl *POGOAUTOSWEEPPROCW)(const unsigned short *);
typedef void (__cdecl *POGOAUTOSWEEPPROCA)(const char *);

#ifdef __cplusplus
extern "C" 
#endif
POGOAUTOSWEEPPROCW PogoAutoSweepW;
#ifdef __cplusplus
extern "C" 
#endif
POGOAUTOSWEEPPROCA PogoAutoSweepA;

#ifdef UNICODE
#define PogoAutoSweep PogoAutoSweepW
#else
#define PogoAutoSweep PogoAutoSweepA
#endif

