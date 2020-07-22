#include <windows.h>

int main(int argc, char **argv)
{
    HWND wSmbx;
    if(argc <= 1)
        return 1;

    wSmbx = (HWND)strtol(argv[1], NULL, 10);
    ShowWindow(wSmbx, SW_SHOW);
    SetForegroundWindow(wSmbx);
    SetActiveWindow(wSmbx);
    SetFocus(wSmbx);
    return 0;
}
