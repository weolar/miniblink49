#ifndef WKEXE_CMDLINE_H
#define WKEXE_CMDLINE_H


#include <windows.h>


typedef struct
{
    int transparent;
    WCHAR htmlFile[MAX_PATH];
    int showHelp;

} CommandOptions;

void ParseOptions(int argc, LPWSTR* argv, CommandOptions* options);
void PrintHelp();


#endif//#ifndef WKEXE_CMDLINE_H