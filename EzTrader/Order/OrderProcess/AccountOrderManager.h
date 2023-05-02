#pragma once
#include <string>
#include <map>
#include <memory>
#include "../SmOrderConst.h"
#include "../../Json/json.hpp"

namespace DarkHorse {
	using order_event = nlohmann::json;
	class SymbolOrderManager;
	struct Order;
	using order_p = std::shared_ptr<Order>;
class AccountOrderManager
{
	using symbol_order_manager_p = std::shared_ptr<SymbolOrderManager>;
public:
	void dispatch_order(const OrderEvent order_event, order_p order);
	symbol_order_manager_p get_order_manager(const std::string& symbol_code);
	const std::map<std::string, symbol_order_manager_p>& get_symbol_order_manager_map() {
		return symbol_order_manager_map_;
	}
private:
	std::map<std::string, symbol_order_manager_p> symbol_order_manager_map_;
	symbol_order_manager_p find_order_manager(const std::string& symbol_code);
	symbol_order_manager_p create_order_manager(const std::string& symbol_code);
};
}

