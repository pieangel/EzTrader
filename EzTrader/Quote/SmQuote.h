#pragma once
#include <string>
namespace DarkHorse {
	struct SmQuote
	{
		int high{ 0 };
		int low{ 0 };
		int close{ 0 };
		int open{ 0 };
		int pre_day_close{ 0 };
	};

	struct SmTick 
	{
		std::string time;
		int close{ 0 };
		int qty{ 0 };
		int updown{ 0 };
	};
}

