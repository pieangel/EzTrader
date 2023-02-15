#pragma once
#include <string>
#include <map>
#include <memory>
#include "../../Json/json.hpp"

namespace DarkHorse {
	using order_event = nlohmann::json;
	class SymbolOrderManager;
class AccountOrderManager
{
	using symbol_order_manager_p = std::shared_ptr<SymbolOrderManager>;
public:
	void on_order_event(const order_event& order_info);
	symbol_order_manager_p get_order_manager(const std::string& symbol_code);
private:
	std::map<std::string, symbol_order_manager_p> symbol_order_manager_map_;
	symbol_order_manager_p find_order_manager(const std::string& symbol_code);
	symbol_order_manager_p create_order_manager(const std::string& symbol_code);
};
}

