#include <windows.h>
#include "setup.h"
#define _CRT_SECURE_NO_DEPRECATE

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  SPY_Setup();
  return 0;
}
