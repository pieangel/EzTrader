#include "stdafx.h"
#include "AccountPositionManager.h"
#include "Position.h"
namespace DarkHorse {
position_p AccountPositionManager::get_position(const std::string& symbol_code)
{
	position_p position = find_position(symbol_code);
	if (position) return position;
	return create_position(symbol_code);
}
position_p AccountPositionManager::find_position(const std::string& symbol_code)
{
	auto it = position_map_.find(symbol_code);
	return it != position_map_.end() ? it->second : nullptr;
}

position_p AccountPositionManager::create_position(const std::string& symbol_code)
{
	position_p position = std::make_shared<Position>();
	position_map_[symbol_code] = position;
	return position;
}

void AccountPositionManager::update_position(order_p order)
{
	if (!order) return;
}

int AccountPositionManager::calculate_position_count(order_p order, position_p position)
{
	return 0;
}
int AccountPositionManager::calculate_unsettled_count(order_p order, position_p position)
{
	return 0;
}
int AccountPositionManager::calculate_traded_count(order_p order, position_p position)
{
	return 0;
}
double AccountPositionManager::calculate_traded_profit_loss(order_p order, position_p position)
{
	return 0.0;
}
double AccountPositionManager::calculate_average_price(order_p order, position_p position)
{
	return 0.0;
}
double AccountPositionManager::calculate_open_profit_loss(order_p order, position_p position)
{
	return 0.0;
}

}