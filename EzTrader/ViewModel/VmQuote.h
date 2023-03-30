#pragma once
#include <string>
namespace DarkHorse {
	struct VmQuote
	{
		int high{ 0 };
		int low{ 0 };
		int close{ 0 };
		int open{ 0 };
		int pre_day_close{ 0 };
		bool symbol_id{ 0 };
	};
}
