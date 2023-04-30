#pragma once
#include <map>
#include <set>
#include <string>
#include <memory>
#include <functional>
#include "SubOrderControl.h"
#include "../Order/SmOrderConst.h"
#include "../ViewModel/VmOrder.h"
namespace DarkHorse {
	struct Order;
	struct AcceptedOrder;
	using order_p = AcceptedOrder;
	class OrderControl
	{
	public:
		OrderControl();
		~OrderControl();
		void update_order(std::shared_ptr<Order> order, OrderEvent order_event);
		void on_order_unfilled(std::shared_ptr<Order> order);
		void on_order_accepted(std::shared_ptr<Order> order);
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
		SubOrderControl& get_buy_order_control() {
			return buy_order_control_;
		}
		SubOrderControl& get_sell_order_control() {
			return sell_order_control_;
		}
		void add_account_id(const int account_id);
		std::pair<int, int> get_order_count(const SmPositionType& position, const int price);
		std::shared_ptr<PriceOrderMap> get_order_map(const SmPositionType& position, const int price);
	private:
		std::shared_ptr<PriceOrderMap> get_order_map(DarkHorse::SubOrderControl& order_control, const int price);
		std::pair<int, int> get_order_count(DarkHorse::SubOrderControl& order_control, const int price);
		SubOrderControl buy_order_control_;
		SubOrderControl sell_order_control_;
		int symbol_id_{ 0 };
		int id_{ 0 };
		// the set of account id of which belonged to the account.
		std::set<int> account_id_set_;
		std::function<void()> event_handler_;
	};
}

