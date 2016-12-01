#ifndef WKEXE_PATH_H
#define WKEXE_PATH_H

#include <windows.h>
#include <stdlib.h>


LPCWSTR GetWorkingDirectory(LPWSTR buffer, size_t bufferSize);
LPCWSTR GetWorkingPath(LPWSTR buffer, size_t bufferSize, LPCWSTR relatedPath);
LPCWSTR FormatWorkingPath(LPWSTR buffer, size_t bufferSize, LPCWSTR fmt, ...);

LPCWSTR GetProgramDirectory(LPWSTR buffer, size_t bufferSize);
LPCWSTR GetProgramPath(LPWSTR buffer, size_t bufferSize, LPCWSTR relatedPath);
LPCWSTR FormatProgramPath(LPWSTR buffer, size_t bufferSize, LPCWSTR fmt, ...);


#endif//#ifndef WKEXE_PATH_H