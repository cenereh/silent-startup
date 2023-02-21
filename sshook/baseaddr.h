#pragma once
#include <Windows.h>

#include "debug.h"

// Retreives the base address of Taskmgr.exe. Returns not null if successful
BYTE* SsGetBaseAddress(debug d);
