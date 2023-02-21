#include "pch.h"
#include "baseaddr.h"

BYTE* SsGetBaseAddress(debug d)
{
    void* ret = GetModuleHandleW(L"Taskmgr.exe");

    if (ret)
        d.out("[I] Taskmgr.exe base address 0x%p\n", ret);
    else
        d.out("[E] Taskmgr.exe module not found.\n");

    return (BYTE*)ret;
}
