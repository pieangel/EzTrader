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
	class SmAccount;
	class SmSymbol;
	class OrderControl
	{
	public:
		OrderControl();
		~OrderControl();
		void update_order(std::shared_ptr<Order> order, OrderEvent order_event);
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
		SubOrderControl& get_buy_order_control() {
			return buy_order_control_;
		}
		SubOrderControl& get_sell_order_control() {
			return sell_order_control_;
		}
		void clear();
		void add_account_id(const int account_id);
		void set_account(std::shared_ptr<SmAccount> account);
		void set_symbol(std::shared_ptr<SmSymbol> symbol);
		std::pair<int, int> get_order_count(const SmPositionType& position, const int price);
		std::shared_ptr<PriceOrderMap> get_order_map(const SmPositionType& position, const int price);
		void load_from_account(const bool is_sub_account, const std::string& account_no, const std::string& symbol_code);
		void load_from_fund(const std::string& fund_name, const std::string& symbol_code);
	private:
		void add_order(const std::map<std::string, std::shared_ptr<Order>>& accepted_order_map);
		void add_order(const std::string& account_no, const std::string& symbol_code);
		void add_order(std::shared_ptr<Order> order);
		void on_order_unfilled(std::shared_ptr<Order> order);
		void on_order_accepted(std::shared_ptr<Order> order);
		void remove_order(std::shared_ptr<Order> order);
		std::shared_ptr<PriceOrderMap> get_order_map(DarkHorse::SubOrderControl& order_control, const int price);
		std::pair<int, int> get_order_count(DarkHorse::SubOrderControl& order_control, const int price);
		SubOrderControl buy_order_control_;
		SubOrderControl sell_order_control_;
		//int symbol_id_{ 0 };
		std::shared_ptr<SmSymbol> symbol_{nullptr};
		std::shared_ptr<SmAccount> account_{ nullptr };
		int id_{ 0 };
		// the set of account no of which belongs to the account.
		//std::set<std::string> account_no_set_;
		std::function<void()> event_handler_;
	};
}

