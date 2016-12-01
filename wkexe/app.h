#ifndef WKEXE_APP_H
#define WKEXE_APP_H

#include "cmdline.h"
#include "wke.h"

typedef struct
{
    wkeWebView window;
    CommandOptions options;
    WCHAR url[MAX_PATH+1];

} Application;


void RunApplication(Application* app);

#endif//#ifndef WKEXE_APP_H