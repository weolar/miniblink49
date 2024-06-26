#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>


/*
* some good values for block size and count
*/
#define BLOCK_SIZE 8192
#define BLOCK_COUNT 20

/*
* function prototypes
*/
static void CALLBACK waveOutProc(HWAVEOUT, UINT, DWORD, DWORD, DWORD);
static WAVEHDR* allocateBlocks(int size, int count);
static void freeBlocks(WAVEHDR* blockArray);
static void writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size);

/*
* module level variables
*/
static CRITICAL_SECTION s_waveCriticalSection;
static WAVEHDR* s_waveBlocks;
static volatile int s_waveFreeBlockCount;
static int s_waveCurrentBlock;

static void CALLBACK waveOutProc(
    HWAVEOUT hWaveOut,
    UINT uMsg,
    DWORD dwInstance,
    DWORD dwParam1,
    DWORD dwParam2
    )
{
    /*
    * pointer to free block counter
    */
    int* freeBlockCounter = (int*)dwInstance;
    /*
    * ignore calls that occur due to openining and closing the
    * device.
    */
    if (uMsg != WOM_DONE)
        return;
    EnterCriticalSection(&s_waveCriticalSection);
    (*freeBlockCounter)++;
    LeaveCriticalSection(&s_waveCriticalSection);
}

WAVEHDR* allocateBlocks(int size, int count)
{
    unsigned char* buffer;
    int i;
    WAVEHDR* blocks;
    DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;
    /*
    * allocate memory for the entire set in one go
    */
    if ((buffer = (unsigned char*)HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        totalBufferSize
        )) == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        ExitProcess(1);
    }
    /*
    * and set up the pointers to each bit
    */
    blocks = (WAVEHDR*)buffer;
    buffer += sizeof(WAVEHDR) * count;
    for (i = 0; i < count; i++) {
        blocks[i].dwBufferLength = size;
        blocks[i].lpData = (LPSTR)buffer;
        buffer += size;
    }
    return blocks;
}

void freeBlocks(WAVEHDR* blockArray)
{
    /*
    * and this is why allocateBlocks works the way it does
    */
    HeapFree(GetProcessHeap(), 0, blockArray);
}

void writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size)
{
    WAVEHDR* current;
    int remain;
    current = &s_waveBlocks[s_waveCurrentBlock];
    while (size > 0) {
        /*
        * first make sure the header we're going to use is unprepared
        */
        if (current->dwFlags & WHDR_PREPARED)
            waveOutUnprepareHeader(hWaveOut, current, sizeof(WAVEHDR));

        if (size < (int)(BLOCK_SIZE - current->dwUser)) {
            memcpy(current->lpData + current->dwUser, data, size);
            current->dwUser += size;
            break;
        }
        remain = BLOCK_SIZE - current->dwUser;
        memcpy(current->lpData + current->dwUser, data, remain);
        size -= remain;
        data += remain;
        current->dwBufferLength = BLOCK_SIZE;
        waveOutPrepareHeader(hWaveOut, current, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, current, sizeof(WAVEHDR));
        EnterCriticalSection(&s_waveCriticalSection);
        s_waveFreeBlockCount--;
        LeaveCriticalSection(&s_waveCriticalSection);

        /*
        * wait for a block to become free
        */
        while (!s_waveFreeBlockCount)
            Sleep(10);

        /*
        * point to the next block
        */
        s_waveCurrentBlock++;
        s_waveCurrentBlock %= BLOCK_COUNT;
        current = &s_waveBlocks[s_waveCurrentBlock];
        current->dwUser = 0;
    }
}


int TestAudio()
{
    HWAVEOUT hWaveOut; /* device handle */
    HANDLE hFile;/* file handle */
    WAVEFORMATEX wfx; /* look this up in your documentation */
    char buffer[1024]; /* intermediate buffer for reading */
    int i;

    /*
    * initialise the module variables
    */
    s_waveBlocks = allocateBlocks(BLOCK_SIZE, BLOCK_COUNT);
    s_waveFreeBlockCount = BLOCK_COUNT;
    s_waveCurrentBlock = 0;
    InitializeCriticalSection(&s_waveCriticalSection);
    /*
    * try and open the file
    */
    if ((hFile = CreateFileW(
        L"F:\\chromium_70_support_xp\\src\\third_party\\webrtc\\data\\voice_engine\\audio_long16.wav",
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
        )) == INVALID_HANDLE_VALUE) {
        //fprintf(stderr, "%s: unable to open file '%s'\n", argv[0], argv[1]);
        ExitProcess(1);
        return 0;
    }

    /*
    * set up the WAVEFORMATEX structure.
    */
    wfx.nSamplesPerSec = 44100; /* sample rate */
    wfx.wBitsPerSample = 16; /* sample size */
    wfx.nChannels = 1; /* channels*/
    wfx.cbSize = 0; /* size of _extra_ info */
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) >> 3;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

    /*
    * try to open the default wave device. WAVE_MAPPER is
    * a constant defined in mmsystem.h, it always points to the
    * default wave device on the system (some people have 2 or
    * more sound cards).
    */
    if (waveOutOpen(
        &hWaveOut,
        WAVE_MAPPER,
        &wfx,
        (DWORD_PTR)waveOutProc,
        (DWORD_PTR)&s_waveFreeBlockCount,
        CALLBACK_FUNCTION
        ) != MMSYSERR_NOERROR) {
        //fprintf(stderr, "%s: unable to open file '%s'\n", argv[0], argv[1]);
        ExitProcess(1);
        return 0;
    }

    /*
    * playback loop
    */
    while (1) {
        DWORD readBytes;
        if (!ReadFile(hFile, buffer, sizeof(buffer), &readBytes, NULL))
            break;
        if (readBytes == 0)
            break;
        if (readBytes < sizeof(buffer)) {
            printf("at end of buffer\n");
            memset(buffer + readBytes, 0, sizeof(buffer) - readBytes);
            printf("after memcpy\n");
        }
        writeAudio(hWaveOut, buffer, sizeof(buffer));
    }

    /*
    * wait for all blocks to complete
    */
    while (s_waveFreeBlockCount < BLOCK_COUNT)
        Sleep(10);

    /*
    * unprepare any blocks that are still prepared
    */
    for (i = 0; i < s_waveFreeBlockCount; i++) {
        if (s_waveBlocks[i].dwFlags & WHDR_PREPARED)
            waveOutUnprepareHeader(hWaveOut, &s_waveBlocks[i], sizeof(WAVEHDR));
    }

    DeleteCriticalSection(&s_waveCriticalSection);
    freeBlocks(s_waveBlocks);
    waveOutClose(hWaveOut);
    CloseHandle(hFile);
    return 0;
}