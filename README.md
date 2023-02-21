# silent-startup
Hooks Windows task manager to hide an arbitrary file in the startup menu.

## How does it work?
Task Manager checks for registry startup items by calling it's internal function called TmRegGetSZAndExpValue at offset 0x5C274 in memory.
SilentStartup hooks TmRegGetSZAndExpValue, overrides the call to the Windows API RegQueryValueExW and checks if the registry key's name that is being checked is equal to "Windows Defender" (random name that I chose, for no specific reason).
If the registry key's name is equal to "Windows Defender" (or any other name you can choose by editing the hook.asm file), the hook will load a bogus value in the RAX register, tricking TmRegGetSZAndExpValue into thinking that the call to RegQueryValueExW failed, then returns execution to the original function.

In the opposite case, the hook will simply execute the functions it has overwritten and it will call RegQueryValueExW with the arguments provided by TmRegGetSZAndExpValue, showing all other items that should not be hidden by the hook.

## Issues
- Sometimes the "loader" attempts to hook Task Manager too early in it's execution, failing to open an handle to the process.
- Admin privileges are required.

## Disclaimer
This code is for educational purposes only. I do not take any responsability for any misuse of this code by any third party, please be civil and only use this piece of code in legitimate software without causing any harm or damage to anyone or anything.

