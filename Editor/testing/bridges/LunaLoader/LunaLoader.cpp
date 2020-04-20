// LunaLoader.cpp : Loader for SMBX with LunaDLL
//

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include "LunaLoaderPatch.h"

#define ErrorBox(msg, title) MessageBoxW(NULL, (msg), (title), MB_ICONERROR)

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

std::wstring escapeArg(const std::wstring &str)
{
    if ((str.find_first_of(L" \t\n\v\"") == str.npos) &&
        (str.find(L"\\\\") == str.npos) &&
        (str.find(L"\\\"") == str.npos)
        )
    {
        // No escaping needed
        return str;
    }

    std::wstring out = L"\"";
    for (wchar_t c : str)
    {
        switch (c)
        {
        case L'\\':
            out += L"\\\\";
            break;
        case L'"':
            out += L"\\\"";
            break;
        default:
            out += c;
        }
    }
    out += L"\"";
    return out;
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

    // Strip first arg which is just our own path
    if (cmdArgs.size() > 0)
    {
        cmdArgs.erase(cmdArgs.begin());
    }

    // If the first arg starting with "--" and contains .exe then no smbx path --> just starting with argument
    if ((cmdArgs.size() > 0) && (cmdArgs[0].find(L"--") == std::wstring::npos))
    {
        std::wstring tmpArg = cmdArgs[0];
        std::transform(tmpArg.begin(), tmpArg.end(), tmpArg.begin(), ::towlower);
        bool isExeFile = ((tmpArg.rfind(L".exe") == (tmpArg.size() - 4)) ||
                          (tmpArg.rfind(L".exe.legacy") == (tmpArg.size() - 11)));

        // Only interpret this as the smbx executable if this ends in .exe
        if (isExeFile)
        {
            pathToSMBX = cmdArgs[0];
            cmdArgs.erase(cmdArgs.begin());
        }
    }

    // Re-generate arg string from list
    newCmdLine = L"";
    for (unsigned int i = 0; i < cmdArgs.size(); ++i)
    {
        newCmdLine += escapeArg(cmdArgs[i]);
        if (i != cmdArgs.size() - 1)
        {
            newCmdLine += L" ";
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

    std::wstring errMsg;
    switch(ret)
    {
    case LUNALOADER_OK:
        return 0;
    case LUNALOADER_CREATEPROCESS_FAIL:
        errMsg = L"Error: Couldn't run SMBX\r\n\r\n";
        errMsg += LunaLoaderGetLastError();
        ErrorBox(errMsg.c_str(), L"LunaLoader Error");
        return 1;
    case LUNALOADER_PATCH_FAIL:
        errMsg = L"Error: Failed to patch SMBX\r\n\r\n";
        errMsg += LunaLoaderGetLastError();
        ErrorBox(errMsg.c_str(), L"LunaLoader Error");
        return 2;
    default:
        errMsg = L"Unknown Error in LunaLoader.\r\n\r\n";
        errMsg += LunaLoaderGetLastError();
        ErrorBox(errMsg.c_str(), L"LunaLoader Error");
        return 3;
    }
}
