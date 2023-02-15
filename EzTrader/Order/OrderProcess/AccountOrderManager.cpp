#include "stdafx.h"
#include "AccountOrderManager.h"
#include "SymbolOrderManager.h"
namespace DarkHorse {
using symbol_order_manager_p = std::shared_ptr<SymbolOrderManager>;
void AccountOrderManager::on_order_event(const order_event& order_info)
{
	std::string symbol_code = order_info["symbol_code"];
	symbol_order_manager_p symbol_order_manager = get_order_manager(symbol_code);
	symbol_order_manager->on_order_event(order_info);
}

symbol_order_manager_p AccountOrderManager::find_order_manager(const std::string& symbol_code)
{
	auto it = symbol_order_manager_map_.find(symbol_code);
	return it != symbol_order_manager_map_.end() ? it->second : nullptr;
}
symbol_order_manager_p AccountOrderManager::create_order_manager(const std::string& symbol_code)
{
	return std::make_shared<SymbolOrderManager>();
}
symbol_order_manager_p AccountOrderManager::get_order_manager(const std::string& symbol_code)
{
	symbol_order_manager_p order_manager = find_order_manager(symbol_code);
	if (order_manager) return order_manager;
	return create_order_manager(symbol_code);
}
}
