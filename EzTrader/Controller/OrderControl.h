#pragma once
#include <map>
#include <set>
#include <string>
#include <memory>
#include "SubOrderControl.h"
namespace DarkHorse {
	class SmOrder;
	struct AcceptedOrder;
	using order_p = std::shared_ptr<AcceptedOrder>;
	class OrderControl
	{
	public:
		void on_order_accepted(std::shared_ptr<SmOrder> order);
		void on_order_unfilled(std::shared_ptr<SmOrder> order);
	private:
		SubOrderControl buy_order_control_;
		SubOrderControl sell_order_control_;
		int symbol_id_{ 0 };
		std::set<int> account_id_set_;
		// key : order no, value : AcceptedOrder object pointer.
		std::map<std::string, order_p> order_list_;
	};
}

