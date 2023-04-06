#pragma once
#include <map>
#include <string>
#include <memory>
#include "../ViewModel/VmOrder.h"
namespace DarkHorse {
	class SmOrder;
	struct PriceOrder {
		int price;
		int count;
		// key : order no, value : accepted order object.
		std::map<std::string, AcceptedOrder> order_map;
	};
	class AcceptedOrderControl
	{
	public:
		int get_total_count() {
			return total_count_;
		}
		void update_order(std::shared_ptr<SmOrder> order_p);
	private:
		int total_count_{ 0 };
		// key : price as integer, value : order list on the price. 
		std::map<int, PriceOrder> order_map_;
	};
}

