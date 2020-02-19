// LunaLoader.cpp : Loader for SMBX with LunaDLL
//

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <string>
#include <vector>
#include "LunaLoaderPatch.h"

#define ErrorBox(msg, title) MessageBoxW(NULL, msg, title, MB_ICONERROR)

std::vector<std::wstring> splitCmdArgs(const std::wstring &str)
{
    std::vector<std::wstring> args;
    int argc     = 0;
    wchar_t **argvW = CommandLineToArgvW(str.c_str(), &argc);
    args.reserve(argc);
    for(int i = 0; i < argc; i++)
        args.push_back(argvW[i]);
    return args;
}

std::wstring dirnameOf(const std::wstring& fname)
{
    size_t pos = fname.find_last_of(L"\\/");
    return (std::wstring::npos == pos) ? L"" : fname.substr(0, pos);
}

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lCmdLine*/, int /*nCmdShow*/)
{
    LPWSTR lpCmdLine = GetCommandLineW();
    std::vector<std::wstring> cmdArgs = splitCmdArgs(lpCmdLine);
    std::wstring newCmdLine = lpCmdLine;

    std::wstring curPath;

    WCHAR szFileName[MAX_PATH];
    GetModuleFileNameW(NULL, szFileName, MAX_PATH);
    WCHAR** lppPart = { NULL };
    WCHAR fulPath_s[4096] = L"";

    GetFullPathNameW(szFileName, 4096, fulPath_s, lppPart);
    curPath = dirnameOf(fulPath_s);

    std::wstring pathToSMBX = curPath+L"\\smbx.exe"; //use either smbx.exe or the first arg

    if(pathToSMBX == fulPath_s) // If LunaLoader.exe renamed into "SMBX.exe"
        pathToSMBX = curPath + L"\\smbx.legacy";

    if (cmdArgs.size() > 1)
    { //if more than one arg then possible of a smbx path
        if (cmdArgs[1].find(L"--") == std::wstring::npos)
        { //if the first arg starting with "--" then no smbx path --> just starting with argument
            pathToSMBX = cmdArgs[1];
            newCmdLine = L""; //reset the arg path
            if (cmdArgs.size() > 1)
            { //strip the smbx-path from the arguments
                for (unsigned int i = 1; i < cmdArgs.size(); ++i)
                {
                    newCmdLine += cmdArgs[i] + std::wstring(L" ");
                    if (i == cmdArgs.size() - 1)
                    { //if the last argument of the args list then remove the last space.
                        std::wstring::iterator it = newCmdLine.end();
                        newCmdLine.erase(--it);
                    }
                }
            }
        }
    }

    FILE *test;
#ifdef _MSC_VER
    _wfopen_s(&test, pathToSMBX.c_str(), L"rb");
#else
    test = _wfopen(pathToSMBX.c_str(), L"rb");
#endif
    if(!test)
    {
        pathToSMBX = curPath + L"\\smbx.legacy";
    }
    else
        fclose(test);

    LunaLoaderResult ret = LunaLoaderRun(pathToSMBX.c_str(), newCmdLine.c_str(), curPath.c_str());

    switch(ret)
    {
    case LUNALOADER_OK:
        return 0;
    case LUNALOADER_CREATEPROCESS_FAIL:
#ifdef LUNALOADER_EXEC
        ErrorBox(L"Error: Couldn't run SMBX",
                 L"Couldn't run SMBX");
#else
        ErrorBox(L"Error: Couldn't run SMBX\n"
                 "However you can try to drag and drop the SMBX exe file to LunaLoader.exe\n"
                 "If this doesn't work then ask on the forums.",
                 L"Couldn't run SMBX");
#endif
        return 1;
    case LUNALOADER_PATCH_FAIL:
        ErrorBox(L"Error: Failed to patch SMBX",
                 L"Failed to patch SMBX");
        return 2;
    default:
        ErrorBox(L"Unknown Error in LunaLoader.",
                 L"Unknown Error");
        return 3;
    }
}
