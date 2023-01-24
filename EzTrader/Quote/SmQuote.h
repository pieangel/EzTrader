#pragma once
#include <string>
namespace DarkHorse {
	struct SmQuote
	{
		int high;
		int low;
		int close;
		int open;
		int preclose;
	};

	struct SmTick 
	{
		std::string time;
		int close;
		int qty;
		int updown;
	};
}

