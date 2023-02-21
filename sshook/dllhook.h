#pragma once
#include "debug.h"

// Places a hook at offset 0xbbe9d, where items are added to the Task Manager startup window.
bool SsPlaceHook(unsigned char* BaseAddress, debug d);

