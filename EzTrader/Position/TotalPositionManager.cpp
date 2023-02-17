#include "stdafx.h"
#include "TotalPositionManager.h"
#include "AccountPositionManager.h"
#include "../Order/Order.h"
namespace DarkHorse {
account_position_manager_p TotalPositionManager::get_position_manager(const std::string& account_no)
{
	account_position_manager_p position_manager = find_position_manager(account_no);
	if (position_manager) return position_manager;
	return create_position_manager(account_no);
}
account_position_manager_p TotalPositionManager::find_position_manager(const std::string& account_no)
{
	auto it = position_manager_map_.find(account_no);
	return it != position_manager_map_.end() ? it->second : nullptr;
}
account_position_manager_p TotalPositionManager::create_position_manager(const std::string& account_no)
{
	account_position_manager_p position_manager = std::make_shared<AccountPositionManager>();
	position_manager_map_[account_no] = position_manager;
	return position_manager;
}
position_p TotalPositionManager::get_position(const std::string& account_no, const std::string& symbol_code)
{
	account_position_manager_p position_manager = get_position_manager(account_no);
	return position_manager->get_position(symbol_code);
}
void TotalPositionManager::update_position(order_p order)
{
	account_position_manager_p position_manager = get_position_manager(order->account_no);
	position_manager->update_position(order);
}
}