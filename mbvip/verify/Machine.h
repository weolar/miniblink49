
#ifndef verify_machine_h
#define verify_machine_h

class Machine {
public:
    static size_t getVolumeID(char* buffer, size_t length);
    static size_t getAllMacId(char *buffer, size_t length);
    static size_t getDiskId(char* buffer, size_t length);
    static size_t getAllCpuId(char* buffer, size_t length);

    static const wchar_t* getParentName();
};

#endif // verify_machine_machine_h