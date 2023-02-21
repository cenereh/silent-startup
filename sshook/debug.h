#pragma once
#include <string>
#include <stdio.h>

class debug
{
public:
	inline debug() { }

	inline void DebugInit() 
	{ 
		_dbgInst = new debug_internal();
	}

	template <typename... Args>
	inline void out(std::string content, Args... a) 
	{
		if (_dbgInst != nullptr)
			_dbgInst->print(content, a...);
	}

private:

	class debug_internal
	{
	public:
		inline debug_internal() { 
			AllocConsole();
			FILE* newStdout = nullptr;
			freopen_s(&newStdout, "CONOUT$", "w", stdout);
		}

		template <typename... Args>
		inline void print(std::string s, Args... a) { printf(s.c_str(), a...); }		
	};

	debug_internal* _dbgInst = nullptr;
};

