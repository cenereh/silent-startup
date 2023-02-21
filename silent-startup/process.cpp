#include "process.h"
#include <process.h>
#include <TlHelp32.h>
#include <string>
#include <iostream>

#include "color.hpp"
#include "error.h"

// If an error occurs.
#define PANIC(Func)             SsPrintWinapiErr(Func, GetLastError()); \
                                return -1;

// Finds a process using TLHelp32. Returns the PID if found, or zero if not/an error occured.
DWORD SsFindProcess(std::wstring ProcessName)
{
    // Create a snapshot using Toolhelp32Snapshot
    HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Snapshot == INVALID_HANDLE_VALUE)
    {
        PANIC("CreateToolhelp32Snapshot")
    }

    // Init the processentry32w structure.
    PROCESSENTRY32W entry;
    int entrySize = sizeof(entry);
    ZeroMemory(&entry, entrySize);
    entry.dwSize = entrySize;

    // Go through the snapshot list of processes. If the requested process is found, return the PID.
    if (Process32FirstW(Snapshot, &entry))
    {
        do
        {
            if (!_wcsicmp(entry.szExeFile, ProcessName.c_str()))
            {
                CloseHandle(Snapshot);
                return entry.th32ProcessID;
            }

        } while (Process32NextW(Snapshot, &entry));
    }
    else
    {
        PANIC("Process32FirstW")
    }

    // If the process has not been found, return 0.
    CloseHandle(Snapshot);
    return 0;
}

// Returns an handle to task manager or zero if an error occured.
HANDLE SsOpenHandleToProcess()
{
    DWORD procPid = 0;

    while (procPid == 0)
    {
        procPid = SsFindProcess(L"Taskmgr.exe");

        // error occured, exit.
        if (procPid == -1)
            return nullptr;
    }

    // Sometimes it gets an invalid PID (apparently 0? or something else?) and fails.
    HANDLE TaskmgrHandle = OpenProcess(PROCESS_ALL_ACCESS | SYNCHRONIZE, false, procPid);

    if (!TaskmgrHandle)
    {
        SsPrintWinapiErr("OpenProcess", GetLastError());
        return nullptr;
    }

    std::cout << "Task Manager process found! " << dye::green("PID " + std::to_string(procPid)) << ", " << dye::aqua("Handle " + std::to_string((DWORD)TaskmgrHandle)) << "\n";

    // todo: Suspend the process for better hooking

    return TaskmgrHandle;
}
