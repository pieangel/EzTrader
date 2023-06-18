#pragma once
#include <string>
namespace DarkHorse {
	struct OrderSetEvent {
		int window_id{ 0 };
		std::string message{""};
		int grid_height{ 22 };
		int stop_width{40};
		int order_width{ 55 };
		int count_width{ 35 };
		int qty_width{ 35 };
		int quote_width{ 60 };
		bool stop_as_real_order{ true };
	};
}