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
#include "../Util/IdGenerator.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Quote/SmQuoteManager.h"
#include "../Quote/SmQuote.h"

namespace DarkHorse {
account_position_manager_p TotalPositionManager::get_account_position_manager(const std::string& account_no)
{
	account_position_manager_p position_manager = find_position_manager(account_no);
	if (position_manager) return position_manager;
	return create_position_manager(account_no);
}

int TotalPositionManager::calculate_position_count(order_p order, position_p position)
{
	const int order_filled_sign = order->position == SmPositionType::Buy ? 1 : -1;
	const int signed_filled_count = order->filled_count * order_filled_sign;
	return signed_filled_count + position->open_quantity;
}

int TotalPositionManager::calculate_unsettled_count(order_p order, position_p position)
{
	const int order_filled_sign = order->position == SmPositionType::Buy ? 1 : -1;
	const int signed_filled_count = order->filled_count * order_filled_sign;
	if (position->open_quantity == 0) return signed_filled_count;
	if (position->open_quantity * signed_filled_count > 0) return signed_filled_count;
	if (abs(position->open_quantity) >= order->filled_count) return 0;
	return position->open_quantity + signed_filled_count;
}

int TotalPositionManager::calculate_traded_count(order_p order, position_p position)
{
	const int order_filled_sign = position->open_quantity > 0 ? 1 : -1;
	const int signed_filled_count = order->filled_count * order_filled_sign;
	if (position->open_quantity * signed_filled_count >= 0) return 0;
	return min(abs(position->open_quantity), abs(signed_filled_count));
}

double TotalPositionManager::calculate_traded_profit_loss(order_p order, position_p position, const int& symbol_decimal, const int& symbol_seungsu)
{
	const int traded_count = calculate_traded_count(order, position);
	const double price_gap = (order->filled_price - position->average_price) / pow(10, symbol_decimal);
	double trade_profit_loss = price_gap * traded_count * symbol_seungsu;
	// 매도는 계산이 반대로 이루어짐. 
	if (position->open_quantity < 0) trade_profit_loss *= -1;
	return trade_profit_loss;
}

double TotalPositionManager::calculate_average_price(order_p order, position_p position, const int& new_open_quantity, const int& unsettled_count)
{
	if (new_open_quantity == 0) return 0;
	if (abs(new_open_quantity) > abs(position->open_quantity)) {
		const double position_average_price = position->average_price * abs(position->open_quantity);
		const double filled_price = order->filled_price * abs(unsettled_count);
		return (position_average_price + filled_price) / (abs(new_open_quantity));
	}
	else
		return position->average_price;
}

void TotalPositionManager::update_open_profit_loss(position_p position)
{
	if (!position) return;
	std::shared_ptr<SmQuote> quote = mainApp.QuoteMgr()->get_quote(position->symbol_code);
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(position->symbol_code);
	if (!quote || !symbol) return;

	position->open_profit_loss = calculate_symbol_open_profit_loss(position->open_quantity, quote->close, position->average_price, symbol->seung_su(), symbol->decimal());
}

account_position_manager_p TotalPositionManager::find_position_manager(const std::string& account_no)
{
	auto it = position_manager_map_.find(account_no);
	return it != position_manager_map_.end() ? it->second : nullptr;
}
account_position_manager_p TotalPositionManager::create_position_manager(const std::string& account_no)
{
	account_position_manager_p position_manager = std::make_shared<AccountPositionManager>(*this, account_no);
	position_manager_map_[account_no] = position_manager;
	return position_manager;
}

void TotalPositionManager::add_to_symbol_position_map(position_p position)
{
	if (!position) return;

	auto it = symbol_position_map_.find(position->symbol_code);
	if (it == symbol_position_map_.end()) {
		position_list symbol_position_list;
		symbol_position_list[position->id] = position;
		symbol_position_map_[position->symbol_code] = std::move(symbol_position_list);
	}
	else {
		it->second[position->id] = position;
	}
}

void TotalPositionManager::update_position_sub_account(order_p order)
{
	auto sub_account = mainApp.AcntMgr()->FindAccount(order->account_no);
	if (!sub_account) return;
	if (sub_account->parent_account().lock()) {
		create_position_for_main_account(order);
		update_position_main_account(order->order_context.parent_account_no, order->symbol_code);
	}
	if (sub_account->fund()) {
		create_position_for_fund(order);
		update_position_fund(sub_account->fund()->Name(), order->symbol_code);
	}
}

void TotalPositionManager::update_position_main_account(const std::string& account_no, const std::string& symbol_code)
{
	auto main_account = mainApp.AcntMgr()->FindAccount(account_no);
	if (!main_account) return;
	std::vector<position_p> position_vector;
	auto main_position = find_position(account_no, symbol_code);
	if (main_position) position_vector.push_back(main_position);
	for(auto& sub_account : main_account->get_sub_accounts()) {
		auto position = find_position(sub_account->No(), symbol_code);
		if (position) position_vector.push_back(position);
	}
}

void TotalPositionManager::update_position_fund(const std::string& fund_name, const std::string& symbol_code)
{
	auto fund = mainApp.FundMgr()->FindFund(fund_name);
	if (!fund) return;
	std::vector<position_p> position_vector;
	auto fund_position = find_position(fund_name, symbol_code);
	if (fund_position) position_vector.push_back(fund_position);
	for (auto& sub_account : fund->GetAccountVector()) {
		auto position = find_position(sub_account->No(), symbol_code);
		if (position) position_vector.push_back(position);
	}
}

void TotalPositionManager::update_parent_profit_loss(order_p order)
{
	if (!order) return;
	if (order->order_context.order_source_type == OrderType::SubAccount) {
		update_position_sub_account(order);
	}
	else if (order->order_context.order_source_type == OrderType::MainAccount) {
		update_position_main_account(order->account_no, order->symbol_code);
	}
	else if (order->order_context.order_source_type == OrderType::Fund) {
		create_position_for_main_account(order);
		create_position_for_fund(order);
		update_position_main_account(order->order_context.parent_account_no, order->symbol_code);
		update_position_fund(order->order_context.fund_name, order->symbol_code);
	}
}

position_p TotalPositionManager::get_position(const std::string& account_no, const std::string& symbol_code)
{
	account_position_manager_p position_manager = get_account_position_manager(account_no);
	position_p position = position_manager->get_position(symbol_code);
	position_map_[position->id] = position;
	return position;
}

position_p TotalPositionManager::get_position(order_p order)
{
	std::pair<std::string, std::string > key;
	if (order->order_context.order_source_type == OrderType::MainAccount || 
		order->order_context.order_source_type == OrderType::SubAccount) {
		key.first = order->account_no;
	}
	else {
		key.first = order->order_context.fund_name;
	}
	key.second = order->symbol_code;

	auto it = total_position_map_.find(key);
	if (it != total_position_map_.end())
		return it->second;
	else
		return create_position(key, order);
}

position_p TotalPositionManager::create_position(const std::pair<std::string, std::string >& key, order_p order)
{
	position_p position = std::make_shared<Position>();
	position->symbol_code = order->symbol_code;
	position->account_no = order->account_no;
	position->id = IdGenerator::get_id();
	position->order_source_type = order->order_context.order_source_type;
	if (order->order_context.order_source_type == OrderType::Fund ||
		order->order_context.order_source_type == OrderType::SubAccount) {
		position->parent_account_no = order->order_context.parent_account_no;
	}
	if (order->order_context.order_source_type == OrderType::Fund) {
		position->fund_name = order->order_context.fund_name;
		position->fund_id = order->order_context.fund_id;
	}
	
	total_position_map_[key] = position;
	return position;
}

position_p TotalPositionManager::create_position_for_main_account(order_p order)
{
	std::pair<std::string, std::string > key;
	key.first = order->order_context.parent_account_no;
	key.second = order->symbol_code;
	position_p position = find_position(key);
	if (position) return position;

	position = std::make_shared<Position>();
	position->symbol_code = order->symbol_code;
	position->account_no = order->order_context.parent_account_no;
	position->id = IdGenerator::get_id();
	position->order_source_type = OrderType::MainAccount;

	total_position_map_[key] = position;
	return position;
}

position_p TotalPositionManager::create_position_for_fund(order_p order)
{
	std::pair<std::string, std::string > key;
	key.first = order->order_context.fund_name;
	key.second = order->symbol_code;
	position_p position = find_position(key);
	if (position) return position;

	position = std::make_shared<Position>();
	position->symbol_code = order->symbol_code;
	position->fund_name = order->order_context.fund_name;
	position->id = IdGenerator::get_id();
	position->order_source_type = OrderType::Fund;

	total_position_map_[std::make_pair(order->order_context.fund_name, order->symbol_code)] = position;
	return position;
}

void TotalPositionManager::get_position_from_fund(const std::string& fund_name, const std::string& symbol_code, VmPosition& position, std::map<std::string, std::shared_ptr<Position>>& position_map)
{
	//position_map.clear();
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



void TotalPositionManager::get_position_from_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position, std::map<std::string, std::shared_ptr<Position>>& position_map)
{
	//position_map.clear();
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

void TotalPositionManager::get_position_from_parent_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position, std::map<std::string, std::shared_ptr<Position>>& position_map)
{
	//position_map.clear();
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
		position->order_source_type = OrderType::MainAccount;
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


void TotalPositionManager::update_position2(order_p order)
{
	if (!order || order->order_context.order_source_type == OrderType::None) return;

	position_p position = get_position(order);
	//set_symbol_id(position, order->symbol_code);
	//set_account_id(position, order->account_no);
	auto symbol = mainApp.SymMgr()->FindSymbol(order->symbol_code);
	if (!symbol) return;
	//LOGINFO(CMyLogger::getInstance(), "position_count = [%d], filled_count = [%d], average_price = [%.2f], filled_price = [%d]", position->open_quantity, order->filled_count, position->average_price, order->filled_price);

	const int new_position_count = calculate_position_count(order, position);
	const int unsettled_count = calculate_unsettled_count(order, position);
	const double traded_profit_loss = calculate_traded_profit_loss(order, position, symbol->decimal(), symbol->seung_su());
	const double new_average_price = calculate_average_price(order, position, new_position_count, unsettled_count);

	LOGINFO(CMyLogger::getInstance(), "position_count = [%d], unsettled_count = [%d], trade_profit_loss = [%.2f], average_price = [%.2f]", new_position_count, unsettled_count, traded_profit_loss, new_average_price);

	position->trade_profit_loss += traded_profit_loss;
	position->open_quantity = new_position_count;
	position->average_price = new_average_price;
	update_open_profit_loss(position);
	// update the involved order.
	order->unsettled_count = unsettled_count;
	if (order->unsettled_count == 0)
		order->order_state = SmOrderState::Settled;
	else
		mainApp.event_hub()->process_stop_order_event(order);
	update_parent_profit_loss(order);
	mainApp.event_hub()->process_position_event(position);
}

void TotalPositionManager::update_position2(quote_p quote)
{
	if (!quote) return;

	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(quote->symbol_code);
	if (!symbol) return;

	auto found = symbol_position_map_.find(quote->symbol_code);
	if (found == symbol_position_map_.end()) return;

	position_list symbol_position_list = found->second;
	if (symbol_position_list.empty()) return;

	for(auto & position : symbol_position_list) {
		position.second->open_profit_loss = calculate_symbol_open_profit_loss(position.second->open_quantity, quote->close, position.second->average_price, symbol->seung_su(), symbol->decimal());
	}
}

void TotalPositionManager::update_position(position_p main_position, std::vector<position_p> position_list)
{
	if (!main_position || position_list.empty()) return;


}



position_p TotalPositionManager::find_position(const std::string& source_name, const std::string& symbol_code)
{
	auto found = total_position_map_.find(std::make_pair(source_name, symbol_code));
	if (found == total_position_map_.end()) return nullptr;
	else return found->second;
}

position_p TotalPositionManager::find_position(const std::pair<std::string, std::string>& key)
{
	auto found = total_position_map_.find(key);
	if (found == total_position_map_.end()) return nullptr;
	else return found->second;
}

position_p TotalPositionManager::update_parent_position(order_p order, position_p position)
{
	if (!order || !position) return nullptr;
	if (order->order_context.order_source_type == OrderType::None) return nullptr;
	std::shared_ptr<Position> parent_position = nullptr;
	if (order->order_context.order_source_type == OrderType::MainAccount) {
		parent_position = add_to_main_account(order, position, order->account_no);
		calculate_parent_position(parent_position);
	}
	else if (order->order_context.order_source_type == OrderType::Fund) {
		parent_position = add_to_main_account(order, position, order->order_context.parent_account_no);
		calculate_parent_position(parent_position);
		parent_position = add_to_fund(order, position, order->order_context.fund_name);
		calculate_parent_position(parent_position);
	}
	else if (order->order_context.order_source_type == OrderType::SubAccount) {
		auto sub_account = mainApp.AcntMgr()->FindAccount(order->account_no);
		if (!sub_account) return nullptr;
		if (sub_account->parent_account().lock()) {
			parent_position = add_to_main_account(order, position, order->order_context.parent_account_no);
			calculate_parent_position(parent_position);
		}
		if (sub_account->fund()) {
			parent_position = add_to_fund(order, position, sub_account->fund()->Name());
			calculate_parent_position(parent_position);
		}
	}

	LOGINFO(CMyLogger::getInstance(), "account_no = %s, fund_name = %s, symbol_code = %s,  open_quantity = [%d], position->average_price = [%.2f], open_profit_loss = [%.2f]", parent_position->account_no.c_str(), parent_position->fund_name.c_str(), parent_position->symbol_code.c_str(), parent_position->open_quantity, parent_position->average_price, parent_position->open_profit_loss);

	return parent_position;
}


std::shared_ptr<Position> TotalPositionManager::add_to_main_account(order_p order, position_p position, const std::string& account_no)
{
	std::shared_ptr<Position> parent_position = nullptr;
	std::pair<std::string, std::string> key = std::make_pair(account_no, order->symbol_code);
	auto it = account_position_map_.find(key);
	if (it == account_position_map_.end()) {
		parent_position = std::make_shared<Position>();
		parent_position->account_no = account_no;
		parent_position->symbol_code = order->symbol_code;
		account_position_map_[key] = parent_position;
	}
	else
		parent_position = it->second;
	parent_position->sub_positions[position->account_no] = position;

	return parent_position;
}

std::shared_ptr<Position> TotalPositionManager::add_to_fund(order_p order, position_p position, const std::string& fund_name)
{
	std::shared_ptr<Position> parent_position = nullptr;
	std::pair<std::string, std::string> key = std::make_pair(fund_name, order->symbol_code);
	auto it = fund_position_map_.find(key);
	if (it == fund_position_map_.end()) {
		parent_position = std::make_shared<Position>();
		parent_position->fund_name = order->order_context.fund_name;
		parent_position->symbol_code = order->symbol_code;
		fund_position_map_[key] = parent_position;
	}
	else
		parent_position = it->second;
	parent_position->sub_positions[position->account_no] = position;

	return parent_position;
}


void TotalPositionManager::calculate_parent_position(std::shared_ptr<Position> parent_position)
{
	if (!parent_position) return;
	if (parent_position->sub_positions.empty()) return;

	const std::map<std::string, std::shared_ptr<Position>>& position_map = parent_position->sub_positions;

	double trade_profit_loss{ 0.0f };       //매매(청산)손익
	double open_profit_loss{ 0.0f };		//평가손익
	double pure_trade_profit_loss{ 0.0f };  // 청산 순손익
	double trade_fee{ 0.0f };               // 청산 수수료
	double average_price{ 0.0f };           // 평균가
	int open_quantity{ 0 };           // 보유수량
	for (auto it = position_map.begin(); it != position_map.end(); it++) {
		auto position = it->second;
		if (!position) continue;
		update_open_profit_loss(position);
		trade_profit_loss += position->trade_profit_loss;
		open_profit_loss += position->open_profit_loss;
		trade_fee += position->trade_fee;
		pure_trade_profit_loss += position->pure_trade_profit_loss;

		open_quantity += position->open_quantity;
		average_price += position->average_price;
	}
	parent_position->trade_profit_loss = trade_profit_loss;
	parent_position->open_profit_loss = open_profit_loss;
	parent_position->trade_fee = trade_fee;
	parent_position->pure_trade_profit_loss = pure_trade_profit_loss;
	parent_position->open_quantity = open_quantity;
	parent_position->average_price = average_price;

	if (parent_position->open_quantity != 0)
		parent_position->average_price = std::abs(parent_position->average_price) / std::abs(parent_position->open_quantity);

}

std::shared_ptr<DarkHorse::Position> TotalPositionManager::find_parent_position(position_p position)
{
	std::shared_ptr<Position> parent_position = nullptr;
	return parent_position;
}


position_p TotalPositionManager::update_parent_position(position_p position)
{
	if (!position) return nullptr;
	if (position->order_source_type == OrderType::None) return nullptr;
	std::shared_ptr<Position> parent_position = nullptr;
	std::pair<std::string, std::string> key;
	if (position->order_source_type == OrderType::MainAccount) {
		key.first = position->account_no;
		key.second = position->symbol_code;
		auto found = account_position_map_.find(key);
		if (found == account_position_map_.end()) return nullptr;
		parent_position = found->second;
		calculate_parent_position(parent_position);
	}
	else if (position->order_source_type == OrderType::Fund) {
		key.first = position->parent_account_no;
		key.second = position->symbol_code;
		auto found = account_position_map_.find(key);
		if (found != account_position_map_.end()) {
			parent_position = found->second;
			calculate_parent_position(parent_position);
		}
		key.first = position->fund_name;
		key.second = position->symbol_code;
		found = fund_position_map_.find(key);
		if (found != fund_position_map_.end()) {
			parent_position = found->second;
			calculate_parent_position(parent_position);
		}
	}
	else if (position->order_source_type == OrderType::SubAccount) {
		auto sub_account = mainApp.AcntMgr()->FindAccount(position->account_no);
		if (!sub_account) return nullptr;
		if (sub_account->parent_account().lock()) {
			key.first = position->parent_account_no;
			key.second = position->symbol_code;
			auto found = account_position_map_.find(key);
			if (found != account_position_map_.end()) {
				parent_position = found->second;
				calculate_parent_position(parent_position);
			}
			key.first = position->fund_name;
		}
		if (sub_account->fund()) {
			key.first = sub_account->fund()->Name();
			key.second = position->symbol_code;
			auto found = fund_position_map_.find(key);
			if (found != fund_position_map_.end()) {
				parent_position = found->second;
				calculate_parent_position(parent_position);
			}
		}
	}

	return parent_position;
}



}