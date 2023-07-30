#include "stdafx.h"
#include "TotalPositionManager.h"
#include "AccountPositionManager.h"
#include "../Order/Order.h"
#include "Position.h"
#include "../Log/MyLogger.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../Order/OrderProcess/TotalOrderManager.h"
#include "../Order/OrderProcess/AccountOrderManager.h"
#include "../Order/OrderProcess/SymbolOrderManager.h"
#include "../Fund/SmFundManager.h"
#include "../Fund/SmFund.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"

namespace DarkHorse {
account_position_manager_p TotalPositionManager::get_account_position_manager(const std::string& account_no)
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
	account_position_manager_p position_manager = std::make_shared<AccountPositionManager>(account_no);
	position_manager_map_[account_no] = position_manager;
	return position_manager;
}

position_p TotalPositionManager::get_position(const std::string& account_no, const std::string& symbol_code)
{
	account_position_manager_p position_manager = get_account_position_manager(account_no);
	position_p position = position_manager->get_position(symbol_code);
	position_map_[position->id] = position;
	return position;
}

void TotalPositionManager::get_position_from_fund(const std::string& fund_name, const std::string& symbol_code, VmPosition& position)
{
	auto fund = mainApp.FundMgr()->FindAddFund(fund_name);
	position.average_price = 0.0;
	position.open_quantity = 0;
	position.open_profit_loss = 0.0;
	position.trade_profit_loss = 0.0;
	const std::vector<std::shared_ptr<SmAccount>>& subAcntVector = fund->GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it)
	{
		auto sub_account = *it;
		auto account_position_manager_p = get_account_position_manager(sub_account->No());
		auto sub_account_position = account_position_manager_p->get_position(symbol_code);
		if (sub_account_position->open_quantity != 0)
		{
			position.open_quantity += sub_account_position->open_quantity;
			position.average_price += sub_account_position->average_price * sub_account_position->open_quantity;
			
			position.open_profit_loss += sub_account_position->open_profit_loss;
			position.trade_profit_loss += sub_account_position->trade_profit_loss;
		}
	}

	if (position.open_quantity != 0)
		position.average_price = std::abs(position.average_price) / std::abs(position.open_quantity);
	//return fund_position;
}

void TotalPositionManager::get_position_from_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position)
{
	account_position_manager_p position_manager = get_account_position_manager(account_no);
	const auto& symbol_position = position_manager->get_position(symbol_code);
	position.open_quantity = symbol_position->open_quantity;
	position.average_price = symbol_position->average_price;
	position.open_profit_loss = symbol_position->open_profit_loss;
	position.trade_profit_loss = symbol_position->trade_profit_loss;
}

void TotalPositionManager::get_position_from_parent_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position)
{
	auto parent_account = mainApp.AcntMgr()->FindAccount(account_no);
	//auto account_position = parent_account->position();
	position.average_price = 0.0;
	position.open_quantity = 0;
	position.open_profit_loss = 0.0;
	position.trade_profit_loss = 0.0;
	const std::map<std::string, std::shared_ptr<SmAccount>>& subAcntVector = parent_account->get_sub_accounts();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it)
	{
		auto sub_account = it->second;
		auto account_position_manager_p = get_account_position_manager(sub_account->No());
		auto sub_account_position = account_position_manager_p->get_position(symbol_code);
		if (sub_account_position->open_quantity != 0)
		{
			position.open_quantity += sub_account_position->open_quantity;
			position.average_price += sub_account_position->average_price * sub_account_position->open_quantity;

			position.open_profit_loss += sub_account_position->open_profit_loss;
			position.trade_profit_loss += sub_account_position->trade_profit_loss;
		}
	}

	if (position.open_quantity != 0)
		position.average_price = std::abs(position.average_price) / std::abs(position.open_quantity);
	//return account_position;
}

void TotalPositionManager::get_position_from_fund(const std::string& fund_name, const std::string& symbol_code, VmPosition& position, std::map<int, std::shared_ptr<Position>>& position_map)
{
	position_map.clear();
	auto fund = mainApp.FundMgr()->FindAddFund(fund_name);
	const std::vector<std::shared_ptr<SmAccount>>& subAcntVector = fund->GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it)
	{
		auto sub_account = *it;
		auto account_position_manager_p = get_account_position_manager(sub_account->No());
		auto sub_account_position = account_position_manager_p->get_position(symbol_code);
		if (sub_account_position->open_quantity != 0)
		{
			get_position_from_account(sub_account->No(), symbol_code, position, position_map);
		}
	}

	if (position.open_quantity != 0)
		position.average_price = std::abs(position.average_price) / std::abs(position.open_quantity);
	//return fund_position;
}

void TotalPositionManager::get_position_from_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position, std::map<int, std::shared_ptr<Position>>& position_map)
{
	position_map.clear();
	account_position_manager_p position_manager = get_account_position_manager(account_no);
	const auto& symbol_position = position_manager->get_position(symbol_code);
	position.open_quantity += symbol_position->open_quantity;
	position.average_price += symbol_position->average_price;
	position.open_profit_loss += symbol_position->open_profit_loss;
	position.trade_profit_loss += symbol_position->trade_profit_loss;

	position_map[symbol_position->id] = symbol_position;
}

void TotalPositionManager::get_position_from_parent_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position, std::map<int, std::shared_ptr<Position>>& position_map)
{
	position_map.clear();
	auto parent_account = mainApp.AcntMgr()->FindAccount(account_no);
	const std::map<std::string, std::shared_ptr<SmAccount>>& subAcntVector = parent_account->get_sub_accounts();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it)
	{
		auto sub_account = it->second;
		auto account_position_manager_p = get_account_position_manager(sub_account->No());
		auto sub_account_position = account_position_manager_p->get_position(symbol_code);
		if (sub_account_position->open_quantity != 0)
		{
			get_position_from_account(sub_account->No(), symbol_code, position, position_map);
		}
	}

	if (position.open_quantity != 0)
		position.average_price = std::abs(position.average_price) / std::abs(position.open_quantity);
	//return account_position;
}

void TotalPositionManager::update_position(order_p order)
{
	account_position_manager_p position_manager = get_account_position_manager(order->account_no);
	position_manager->update_position(order);
}

void TotalPositionManager::update_position(quote_p quote)
{
	for (auto& position_manager : position_manager_map_)
	{
		position_manager.second->update_position(quote);
	}
}

void TotalPositionManager::on_symbol_position(nlohmann::json&& arg)
{
	try {
		const std::string symbol_code = arg["symbol_code"];
		const std::string account_no = arg["account_no"];
		const int average_price = arg["symbol_avg_price"];
		const int order_position = arg["symbol_position"]; // buy : 1, sell : -1
		const int pre_open_qty = arg["symbol_pre_open_qty"];
		const int today_open_qty = arg["symbol_open_qty"];
		const int open_quantity = today_open_qty;
		const double open_profit_loss = arg["symbol_open_profit_loss"];


		std::shared_ptr<Position> position = get_position(account_no, symbol_code);
		position->account_no = account_no;
		position->symbol_code = symbol_code;
		position->average_price = average_price;
		position->open_quantity = open_quantity * order_position;
		position->pre_day_open_quantity = pre_open_qty;
		//position->ordered_before = true;
		//position->open_profit_loss = open_profit_loss;
		auto account_order_manager = mainApp.total_order_manager()->get_account_order_manager(account_no);
		auto symbol_order_manager = account_order_manager->get_symbol_order_manager(symbol_code);
		// Set the flag to true to indicate that the order has been placed before.
		symbol_order_manager->set_ordered_before(true);
		
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void TotalPositionManager::on_symbol_profit_loss(nlohmann::json&& arg)
{
	try {
		const std::string& account_no = arg["account_no"];
		const std::string& symbol_code = arg["symbol_code"];
		auto position = get_position(account_no, symbol_code);
		if (position) {
			position->trade_profit_loss = arg["trade_profit_loss"];
			position->pure_trade_profit_loss = arg["pure_trade_profit_loss"];
			position->trade_fee = arg["trade_fee"];
			position->open_profit_loss = arg["open_profit_loss"];
			//position-> = arg["unsettled_fee"];
			//position-> = arg["pure_unsettled_profit_loss"];
			mainApp.event_hub()->process_position_event(position);
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}


double TotalPositionManager::calculate_symbol_open_profit_loss(
	const int& position_open_quantity, 
	const int& symbol_close, 
	const double& position_average_price, 
	const int& symbol_seungsu, 
	const int& symbol_decimal)
{
	double open_profit_loss = 0.0;
	open_profit_loss = position_open_quantity * (symbol_close - position_average_price) * symbol_seungsu;
	return open_profit_loss / pow(10, symbol_decimal);
}

void TotalPositionManager::update_account_profit_loss(const std::string& account_no)
{
	account_position_manager_p position_manager = get_account_position_manager(account_no);
	position_manager->update_account_profit_loss();
	mainApp.event_hub()->process_account_profit_loss_event();
}

position_p TotalPositionManager::find_position_by_id(const int& position_id)
{
	auto it = position_map_.find(position_id);
	if (it != position_map_.end())
		return it->second;
	else
		return nullptr;
}

}