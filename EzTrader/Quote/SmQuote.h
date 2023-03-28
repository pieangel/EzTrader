#pragma once
#include <string>
namespace DarkHorse {
	struct SmQuote
	{
		int symbol_id{ 0 };
		int high{ 0 };
		int low{ 0 };
		int close{ 0 };
		int open{ 0 };
		int pre_day_close{ 0 };
	};

	struct SmTick 
	{
		int symbol_id{ 0 };
		std::string time;
		int close{ 0 };
		int qty{ 0 };
		int updown{ 0 };
	};
}

