#pragma once
#include <string>
namespace DarkHorse {
	struct OrderSetEvent {
		int window_id;
		std::string message;
		int grid_height;
		int stop_width;
		int order_width;
		int count_width;
		int qty_width;
		int quote_width;
		bool stop_as_real_order;
	};
}