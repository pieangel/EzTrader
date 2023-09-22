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
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "GroupPositionManager.h"
#include "../ViewModel/VmPosition.h"
#include "../Event/SmCallbackManager.h"
#include <algorithm>

namespace DarkHorse {
account_position_manager_p TotalPositionManager::get_account_position_manager(const std::string& account_no)
{
	account_position_manager_p position_manager = find_position_manager(account_no);
	if (position_manager) return position_manager;
	return create_position_manager(account_no);
}

group_position_manager_p TotalPositionManager::find_fund_group_position_manager(const std::string& fund_name)
{
	auto group_position_manager = fund_group_position_manager_map_.find(fund_name);
	return group_position_manager != fund_group_position_manager_map_.end() ? group_position_manager->second : nullptr;
}

DarkHorse::group_position_manager_p TotalPositionManager::find_account_group_position_manager(const std::string& account_no)
{
	auto group_position_manager = account_group_position_manager_map_.find(account_no);
	return group_position_manager != account_group_position_manager_map_.end() ? group_position_manager->second : nullptr;
}

DarkHorse::group_position_manager_p TotalPositionManager::find_add_account_group_position_manager(const std::string& account_no)
{
	auto found = account_group_position_manager_map_.find(account_no);
	if (found != account_group_position_manager_map_.end()) {
		return found->second;
	}
	else {
		return create_account_group_position_manager(account_no);
	}
}

DarkHorse::group_position_manager_p TotalPositionManager::create_account_group_position_manager(const std::string& account_no)
{
	auto group_position_manager = std::make_shared<GroupPositionManager>(*this);
	group_position_manager->set_account_no(account_no);
	account_group_position_manager_map_[account_no] = group_position_manager;
	return group_position_manager;
}

DarkHorse::group_position_manager_p TotalPositionManager::find_add_fund_group_position_manager(const std::string& fund_name)
{
	auto found = fund_group_position_manager_map_.find(fund_name);
	if (found != fund_group_position_manager_map_.end()) {
		return found->second;
	}
	else {
		return create_fund_group_position_manager(fund_name);
	}
}

DarkHorse::group_position_manager_p TotalPositionManager::create_fund_group_position_manager(const std::string& fund_name)
{
	auto group_position_manager = std::make_shared<GroupPositionManager>(*this);
	group_position_manager->set_account_no(fund_name);
	fund_group_position_manager_map_[fund_name] = group_position_manager;
	return group_position_manager;
}

group_position_manager_p TotalPositionManager::find_add_group_position_manager(std::shared_ptr<Position> position)
{
	if (!position) return nullptr;
	if (position->order_source_type == OrderType::SubAccount) {
		auto found = account_group_position_manager_map_.find(position->parent_account_no);
		if (found != account_group_position_manager_map_.end()) {
			return found->second;
		}
		else {
			return create_group_position_manager(position);
		}
	}
	else if (position->order_source_type == OrderType::MainAccount) {
		auto found = account_group_position_manager_map_.find(position->account_no);
		if (found != account_group_position_manager_map_.end()) {
			return found->second;
		}
		else {
			return create_group_position_manager(position);
		}
	}
	else if (position->order_source_type == OrderType::Fund) {
		auto found = fund_group_position_manager_map_.find(position->fund_name);
		if (found != fund_group_position_manager_map_.end()) {
			return found->second;
		}
		else {
			return create_group_position_manager(position);
		}
	}
	else return nullptr;
}

group_position_manager_p TotalPositionManager::create_group_position_manager(std::shared_ptr<Position> position)
{
	if (!position) return nullptr;
	auto group_position_manager = std::make_shared<GroupPositionManager>(*this);
	if (position->order_source_type == OrderType::SubAccount) {
		group_position_manager->set_account_no(position->parent_account_no);
		account_group_position_manager_map_[position->parent_account_no] = group_position_manager;
	}
	else if (position->order_source_type == OrderType::MainAccount) {
		group_position_manager->set_account_no(position->account_no);
		account_group_position_manager_map_[position->account_no] = group_position_manager;
	}
	else if (position->order_source_type == OrderType::Fund) {
		group_position_manager->set_account_no(position->fund_name);
		fund_group_position_manager_map_[position->fund_name] = group_position_manager;
	}
	
	return group_position_manager;
}

account_position_manager_p TotalPositionManager::find_position_manager(const std::string& account_no)
{
	auto it = account_position_manager_map_.find(account_no);
	return it != account_position_manager_map_.end() ? it->second : nullptr;
}
account_position_manager_p TotalPositionManager::create_position_manager(const std::string& account_no)
{
	account_position_manager_p position_manager = std::make_shared<AccountPositionManager>(*this, account_no);
	account_position_manager_map_[account_no] = position_manager;
	return position_manager;
}

position_p TotalPositionManager::get_position(const std::string& account_no, const std::string& symbol_code)
{
	account_position_manager_p position_manager = get_account_position_manager(account_no);
	position_p position = position_manager->get_position(symbol_code);
	position_map_[position->id] = position;
	return position;
}

void TotalPositionManager::get_position_from_fund(const std::string& fund_name, const std::string& symbol_code, VmPosition& position, std::map<std::string, std::shared_ptr<Position>>& position_map)
{
	//std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
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
}



void TotalPositionManager::get_position_from_fund(const std::string& fund_name, std::map<std::string, std::shared_ptr<Position>>& position_map)
{
	position_map.clear();
	auto group_position_manager = find_fund_group_position_manager(fund_name);
	if (!group_position_manager) return;
	const auto& source_position_map = group_position_manager->get_group_position_map();
	std::copy(source_position_map.begin(), source_position_map.end(), std::inserter(position_map, position_map.end()));
}

void TotalPositionManager::get_position_from_fund(const std::string& fund_name, const std::string& symbol_code, VmPosition& position)
{
	//std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
	auto group_position_manager = mainApp.total_position_manager()->find_fund_group_position_manager(fund_name);
	auto group_position = group_position_manager->get_group_position(symbol_code);
	if (!group_position) return;
	group_position_manager->update_group_position_by_symbol(group_position, position);
}

void TotalPositionManager::get_position_from_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position, std::map<std::string, std::shared_ptr<Position>>& position_map)
{
	account_position_manager_p position_manager = get_account_position_manager(account_no);
	const auto& symbol_position = position_manager->get_position(symbol_code);
	position.open_quantity += symbol_position->open_quantity;
	position.average_price += symbol_position->average_price;
	position.open_profit_loss += symbol_position->open_profit_loss;
	position.trade_profit_loss += symbol_position->trade_profit_loss;

	position_map[symbol_position->account_no] = symbol_position;
}

std::shared_ptr<Position> TotalPositionManager::get_position_from_account(const std::string& account_no, const std::string& symbol_code)
{
	account_position_manager_p position_manager = get_account_position_manager(account_no);
	return position_manager->get_position(symbol_code);
}

void TotalPositionManager::get_position_from_account(const std::string& account_no, std::map<std::string, std::shared_ptr<Position>>& position_map)
{
	position_map.clear();
	auto account_position_manager = get_account_position_manager(account_no);
	if (!account_position_manager) return;
	const auto& source_position_map = account_position_manager->get_position_map();
	std::copy(source_position_map.begin(), source_position_map.end(), std::inserter(position_map, position_map.end()));
}

void TotalPositionManager::get_position_from_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position)
{
	//std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
	auto account_position_manager = mainApp.total_position_manager()->get_account_position_manager(account_no);
	auto symbol_position = account_position_manager->find_position(symbol_code);
	if (!symbol_position) return;
	account_position_manager->update_position(symbol_position, position);
}

void TotalPositionManager::get_position_from_parent_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position, std::map<std::string, std::shared_ptr<Position>>& position_map)
{
	//std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
	get_position_from_account(account_no, symbol_code, position, position_map);
	auto parent_account = mainApp.AcntMgr()->FindAccount(account_no);
	const std::vector<std::shared_ptr<SmAccount>>& subAcntVector = parent_account->get_sub_accounts();
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
}

void TotalPositionManager::get_position_from_parent_account(const std::string& account_no, std::map<std::string, std::shared_ptr<Position>>& position_map)
{
	position_map.clear();
	auto group_position_manager = find_account_group_position_manager(account_no);
	if (!group_position_manager) return;
	const auto& source_position_map = group_position_manager->get_group_position_map();
	std::copy(source_position_map.begin(), source_position_map.end(), std::inserter(position_map, position_map.end()));
}

void TotalPositionManager::get_position_from_parent_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position)
{
	//std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
	auto group_position_manager = mainApp.total_position_manager()->find_account_group_position_manager(account_no);
	auto group_position = group_position_manager->get_group_position(symbol_code);
	if (!group_position) return;
	group_position_manager->update_group_position_by_symbol(group_position, position);
}

void TotalPositionManager::update_position(order_p order)
{
	//std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
	account_position_manager_p position_manager = get_account_position_manager(order->account_no);
	position_manager->update_position(order);
}

void TotalPositionManager::update_position(quote_p quote)
{
	for (auto& position_manager : account_position_manager_map_)
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
		position->order_source_type = OrderType::MainAccount;
		//position->ordered_before = true;
		//position->open_profit_loss = open_profit_loss;

		update_group_position(position);
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
			mainApp.CallbackMgr()->process_position_event(position);
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

void TotalPositionManager::calculate_symbol_open_profit_loss(const std::shared_ptr<Position>& position)
{
	if (!position) return;
	std::shared_ptr<SmQuote> quote = mainApp.QuoteMgr()->get_quote(position->symbol_code);
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(position->symbol_code);
	if (!quote || !symbol) return;

	double open_profit_loss = 0.0;
	open_profit_loss = position->open_quantity * (quote->close - position->average_price) * symbol->seung_su();
	position->open_profit_loss = open_profit_loss / pow(10, symbol->decimal());
}

void TotalPositionManager::calculate_symbol_open_profit_loss(VmPosition& position)
{
	std::shared_ptr<SmQuote> quote = mainApp.QuoteMgr()->get_quote(position.symbol_code);
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(position.symbol_code);
	if (!quote || !symbol) return;

	double open_profit_loss = 0.0;
	open_profit_loss = position.open_quantity * (quote->close - position.average_price) * symbol->seung_su();
	position.open_profit_loss = open_profit_loss / pow(10, symbol->decimal());
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

void TotalPositionManager::update_group_position(std::shared_ptr<Position> position)
{
	if (!position) return;
	if (position->order_source_type == OrderType::SubAccount) {
		group_position_manager_p group_position_manager = find_add_account_group_position_manager(position->parent_account_no);
		auto account_group_position = group_position_manager->create_account_group_position(position->parent_account_no, position->symbol_code);
		group_position_manager->update_group_position(account_group_position, position);
		mainApp.CallbackMgr()->process_position_event(account_group_position);
		auto sub_account = mainApp.AcntMgr()->FindAccount(position->account_no);
		if (!sub_account || sub_account->fund_name().empty()) return;
		group_position_manager = find_add_fund_group_position_manager(sub_account->fund_name());
		auto fund_group_position = group_position_manager->create_fund_group_position(position->fund_name, position->symbol_code);
		group_position_manager->update_group_position(fund_group_position, position);
		mainApp.CallbackMgr()->process_position_event(fund_group_position);
	}
	else if (position->order_source_type == OrderType::Fund) {
		group_position_manager_p group_position_manager = find_add_account_group_position_manager(position->parent_account_no);
		auto account_group_position = group_position_manager->create_account_group_position(position->parent_account_no, position->symbol_code);
		group_position_manager->update_group_position(account_group_position, position);
		mainApp.CallbackMgr()->process_position_event(account_group_position);
		group_position_manager = find_add_fund_group_position_manager(position->fund_name);
		auto fund_group_position = group_position_manager->create_fund_group_position(position->fund_name, position->symbol_code);
		group_position_manager->update_group_position(fund_group_position, position);
		mainApp.CallbackMgr()->process_position_event(fund_group_position);
	}
	else {
		group_position_manager_p group_position_manager = find_add_account_group_position_manager(position->account_no);
		auto account_group_position = group_position_manager->create_account_group_position(position->account_no, position->symbol_code);
		group_position_manager->update_group_position(account_group_position, position);
		mainApp.CallbackMgr()->process_position_event(account_group_position);
	}
}

}