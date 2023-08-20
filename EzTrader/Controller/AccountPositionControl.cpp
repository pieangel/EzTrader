#include "stdafx.h"
#include "AccountPositionControl.h"
#include "../Position/Position.h"
#include "../Position/AccountPositionManager.h"
#include "../Position/TotalPositionManager.h"
#include "../Global/SmTotalManager.h"
#include "../Util/IdGenerator.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Account/SmAccount.h"
#include "../Fund/SmFund.h"
#include "../Position/GroupPositionManager.h"
namespace DarkHorse {
using account_position_manager_p = std::shared_ptr<AccountPositionManager>;
using group_position_manager_p = std::shared_ptr<GroupPositionManager>;
AccountPositionControl::AccountPositionControl()
	: id_(IdGenerator::get_id())
{
	mainApp.event_hub()->subscribe_position_event_handler
	(
		id_,
		std::bind(&AccountPositionControl::update_position, this, std::placeholders::_1)
	);

	mainApp.event_hub()->subscribe_quote_event_handler
	(
		id_,
		std::bind(&AccountPositionControl::update_profit_loss, this, std::placeholders::_1)
	);
}

AccountPositionControl::~AccountPositionControl()
{
	mainApp.event_hub()->unsubscribe_position_event_handler(id_);
	mainApp.event_hub()->unsubscribe_quote_event_handler(id_);
}

void AccountPositionControl::load_position_from_account(const std::string& account_no)
{
	account_no_set_.clear();
	account_no_set_.insert(account_no);
	position_map_.clear();
	account_position_manager_p acnt_position_mgr = mainApp.total_position_manager()->get_account_position_manager(account_no);
	const std::map<std::string, position_p>& position_map = acnt_position_mgr->get_position_map();
	for (auto it = position_map.begin(); it != position_map.end(); it++) {
		if (it->second->open_quantity == 0) continue;
		position_map_[it->second->symbol_code] = it->second;
	}
}

void AccountPositionControl::load_position_from_parent_account(const std::string& account_no)
{
	account_no_set_.clear();
	position_map_.clear();
	group_position_manager_p group_position_mgr = mainApp.total_position_manager()->find_account_group_position_manager(account_no);
	if (!group_position_mgr) return;
	const std::map<std::string, position_p>& position_map = group_position_mgr->get_group_position_map();
	for (auto it = position_map.begin(); it != position_map.end(); it++) {
		account_no_set_.insert(it->second->account_no);
		if (it->second->open_quantity == 0) continue;
		position_map_[it->second->symbol_code] = it->second;
	}
}

void AccountPositionControl::load_position_from_fund(const std::string& fund_name)
{
	account_no_set_.clear();
	position_map_.clear();
	group_position_manager_p group_position_mgr = mainApp.total_position_manager()->find_fund_group_position_manager(fund_name);
	if (!group_position_mgr) return;
	const std::map<std::string, position_p>& position_map = group_position_mgr->get_group_position_map();
	for (auto it = position_map.begin(); it != position_map.end(); it++) {
		account_no_set_.insert(it->second->account_no);
		if (it->second->open_quantity == 0) continue;
		position_map_[it->second->symbol_code] = it->second;
	}
}

void AccountPositionControl::update_position(position_p position)
{
	if (!position) return;
	auto found = account_no_set_.find(position->account_no);
	if (position->order_source_type == DarkHorse::OrderType::SubAccount) {
		if (account_) 
			load_position_from_account(account_->No());
		if (fund_)
			load_position_from_fund(fund_->Name());
	}
	else if (position->order_source_type == DarkHorse::OrderType::MainAccount) {
		if (account_)
			load_position_from_parent_account(account_->No());
	}
	else if (position->order_source_type == DarkHorse::OrderType::Fund) {
		if (account_)
			load_position_from_parent_account(account_->No());
		if (fund_)
			load_position_from_fund(fund_->Name());
	}
	else return;
	
	if (event_handler_) event_handler_();
}

void AccountPositionControl::update_profit_loss(quote_p quote)
{
	if (!quote) return;
	auto symbol = mainApp.SymMgr()->FindSymbol(quote->symbol_code);
	if (!symbol) return;
	position_p position = get_position(quote->symbol_code);
	if (!position) return;

	double open_profit_loss = 0;
	open_profit_loss = position->open_quantity * (quote->close - position->average_price) * symbol->seung_su();
	open_profit_loss = open_profit_loss / pow(10, symbol->decimal());
	position->open_profit_loss = open_profit_loss;

	if (event_handler_) event_handler_();
}

void AccountPositionControl::set_account(std::shared_ptr<SmAccount> account)
{
	if (!account) return;
	account_ = account;
	if (account_->is_subaccount())
		load_position_from_account(account_->No());
	else
		load_position_from_parent_account(account_->No());
}

void AccountPositionControl::set_fund(std::shared_ptr<SmFund> fund)
{
	if (!fund) return;
	fund_ = fund;
	load_position_from_fund(fund_->Name());
}

position_p AccountPositionControl::get_position(const std::string& symbol_code)
{
	auto it = position_map_.find(symbol_code);
	if (it == position_map_.end()) return nullptr;
	return it->second;
}
}