#include "inject.h"
#include "error.h"

#include <string>

typedef LONG (NTAPI *pNtSuspendProcess )( HANDLE ProcessHandle );
typedef LONG (NTAPI *pNtResumeProcess )( HANDLE ProcessHandle );

#define PANIC(Func)             SsPrintWinapiErr(Func, GetLastError());\
                                return 0;

// https://stackoverflow.com/questions/875249/how-to-get-current-directory
std::wstring ExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName( NULL, buffer, MAX_PATH );
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

// Injects a DLL in a remote process using a remote call to LoadLibraryA.
bool SsInjectInRemoteProcess(HANDLE HandleToProcess)
{
    pNtSuspendProcess NtSuspendProcess;
    pNtResumeProcess NtResumeProcess;

    NtSuspendProcess = (pNtSuspendProcess)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtSuspendProcess");

    if (!NtSuspendProcess)
    {
        PANIC("GetProcAddress (NSP)")
    }

    NtResumeProcess = (pNtResumeProcess)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtResumeProcess");

    if (!NtResumeProcess)
    {
        PANIC("GetProcAddress (NRP)")
    }

    LONG hResult = NtSuspendProcess(HandleToProcess);       // does not seem to work all the time.
    if (!SUCCEEDED(hResult))
    {
        SsPrintWinapiErr("NtSuspendProcess", hResult);
        return false;
    }

    // Get the current folder and attach the dll used to hook Task Manager
    std::wstring DllToInject = ExePath() + L"\\sshook.dll";

    // Allocate space in the remote process to write the full DLL path.
    // fixme: does not work sometimes
    void* dllPathInRemoteProc = VirtualAllocEx(HandleToProcess, 0, DllToInject.length() * 2, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if (dllPathInRemoteProc == nullptr)
    {
        PANIC("VirtualAllocEx")
    }

    // Write the DLL path to the allocated space of memory in the remote process
    if (!WriteProcessMemory(HandleToProcess, dllPathInRemoteProc, DllToInject.c_str(), DllToInject.length() * 2, nullptr))
    {
        PANIC("WriteProcessMemory")
    }

    // todo: should parse the PEB for this.

    // Get the local address for LoadLibraryW as kernel32.dll loads at the same address in each process. (thanks MS ;))
    void* loadLibraryWAddress = (void*)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");

    if (!loadLibraryWAddress)
    {
        PANIC("GetProcAddress")
    }

    hResult = NtResumeProcess(HandleToProcess);
    if (!SUCCEEDED(hResult))
    {
        SsPrintWinapiErr("NtResumeProcess", hResult);
        return false;
    }

    // Make a remote call to LoadLibraryW to inject our DLL.
    HANDLE remoteThread = CreateRemoteThread(HandleToProcess, 0, 0, (LPTHREAD_START_ROUTINE)loadLibraryWAddress, dllPathInRemoteProc, 0, 0);

    if (!remoteThread)
    {
        PANIC("CreateRemoteThread")
    }

    return true;
}
