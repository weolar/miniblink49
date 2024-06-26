
#include <windows.h>

namespace atom {

class FileUtil {
public:
    static bool isDirExist(const std::wstring& fileName)
    {
        WIN32_FIND_DATA fd = { 0 };
        HANDLE hFind = FindFirstFile(fileName.c_str(), &fd);
        if (hFind == INVALID_HANDLE_VALUE)
            return false;

        bool ret = false;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            ret = true;
        FindClose(hFind);
        return ret;
    }
};

} // atom