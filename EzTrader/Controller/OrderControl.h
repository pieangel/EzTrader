#pragma once
#include <map>
#include <set>
#include <string>
#include <memory>
#include <functional>
#include "SubOrderControl.h"
#include "../ViewModel/VmOrder.h"
namespace DarkHorse {
	class SmOrder;
	struct AcceptedOrder;
	using order_p = AcceptedOrder;
	class OrderControl
	{
	public:
		OrderControl() {
			buy_order_control_.control_type = SubOrderControlType::CT_BUY;
			sell_order_control_.control_type = SubOrderControlType::CT_SELL;
		}
		~OrderControl() {};
		void on_order_accepted(std::shared_ptr<SmOrder> order);
		void on_order_unfilled(std::shared_ptr<SmOrder> order);
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
		SubOrderControl* get_buy_order_control() {
			return &buy_order_control_;
		}
		SubOrderControl* get_sell_order_control() {
			return &sell_order_control_;
		}
	private:
		SubOrderControl buy_order_control_;
		SubOrderControl sell_order_control_;
		int symbol_id_{ 0 };
		// the set of account id of which belonged to the account.
		std::set<int> account_id_set_;
		std::function<void()> event_handler_;
	};
}

