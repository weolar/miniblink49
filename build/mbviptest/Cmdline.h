#ifndef WKEXE_CMDLINE_H
#define WKEXE_CMDLINE_H

#include <windows.h>

const int kMaxUrlLegth = MAX_PATH * 5;

typedef struct {
    int transparent;
    WCHAR htmlFile[kMaxUrlLegth];
    int showHelp;

} CommandOptions;

void parseOptions(int argc, LPWSTR* argv, CommandOptions* options);
void printHelp();


#endif//#ifndef WKEXE_CMDLINE_H