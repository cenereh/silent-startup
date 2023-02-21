// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "debug.h"
#include "baseaddr.h"
#include "dllhook.h"

int SsSehFilter(uint32_t ExceptionCode, PEXCEPTION_POINTERS ExceptionPointers)
{
    printf("Fatal 0x%08x exception at 0x%p: terminating program.\nPress any button to continue...\n", ExceptionCode,
        ExceptionPointers->ExceptionRecord->ExceptionAddress);

    getchar();

    return EXCEPTION_EXECUTE_HANDLER;
}

void BeginHookProcedure()
{
    debug d = debug();
    d.DebugInit();

    d.out("[I] Hooking procedure started.\n");

    BYTE* BaseAddress = SsGetBaseAddress(d);

    if (!BaseAddress) 
        return;

    if (!SsPlaceHook(BaseAddress, d))
    {
        return;
    }

    while (true)
    {
        Sleep(10);
    }

    return;
}

DWORD WINAPI MainThread(LPVOID param)
{
    __try
    {
        BeginHookProcedure();
    }
    __except (SsSehFilter(GetExceptionCode(), GetExceptionInformation()))
    {
        goto end;
    }

end:
    FreeLibraryAndExitThread((HMODULE)param, 0);
    return 0;
    
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, MainThread, hModule, 0, 0);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

