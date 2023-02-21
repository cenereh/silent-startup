#include "error.h"
#include <iostream>

#include "color.hpp"

void SsPrintErr(std::string e)
{
	std::cout << dye::black_on_red("error") << ": " << e;
}

void SsPrintWinapiErr(std::string api, int err)
{
	std::cout << dye::black_on_red("error") << ": " << dye::yellow(api) << " failed with error code 0x" << std::hex << dye::red(err) << "\n";
}
