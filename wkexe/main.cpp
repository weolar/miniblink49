
#include "app.h"

int main()
{
    wkeInitialize();
    {
        Application app;
        RunApplication(&app);
    }
    wkeFinalize();

    return 0;
}
