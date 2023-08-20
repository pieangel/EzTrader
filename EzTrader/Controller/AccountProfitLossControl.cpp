#include "stdafx.h"
#include "AccountProfitLossControl.h"
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

AccountProfitLossControl::AccountProfitLossControl()
	: id_(IdGenerator::get_id())
{
	mainApp.event_hub()->subscribe_position_event_handler
	(
		id_,
		std::bind(&AccountProfitLossControl::update_position, this, std::placeholders::_1)
	);

	mainApp.event_hub()->subscribe_quote_event_handler
	(
		id_,
		std::bind(&AccountProfitLossControl::update_profit_loss, this, std::placeholders::_1)
	);

	mainApp.event_hub()->subscribe_account_profit_loss_event_handler
	(
		id_,
		std::bind(&AccountProfitLossControl::update_account_profit_loss, this)
	);
}

AccountProfitLossControl::~AccountProfitLossControl()
{
	mainApp.event_hub()->unsubscribe_position_event_handler(id_);
	mainApp.event_hub()->unsubscribe_quote_event_handler(id_);
	mainApp.event_hub()->unsubscribe_account_profit_loss_event_handler(id_);
}

position_p AccountProfitLossControl::get_position(const std::string& symbol_code)
{
	auto it = position_map_.find(symbol_code);
	if (it == position_map_.end()) return nullptr;
	return it->second;
}

void AccountProfitLossControl::load_position_from_account(const std::string& account_no)
{
	account_no_set_.clear();
	account_no_set_.insert(account_no);
	account_position_manager_p acnt_position_mgr = mainApp.total_position_manager()->get_account_position_manager(account_no);
	account_profit_loss_p account_profit_loss = acnt_position_mgr->get_account_profit_loss();
	refresh_account_profit_loss(account_profit_loss);

	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::load_position_from_parent_account(const std::string& account_no)
{
	account_no_set_.clear();
	account_no_set_.insert(account_no);
	if (account_) {
		auto sub_accounts = account_->get_sub_accounts();
		for (auto it = sub_accounts.begin(); it != sub_accounts.end(); it++) {
			account_no_set_.insert((*it)->No());
		}
	}
	group_position_manager_p group_position_mgr = mainApp.total_position_manager()->find_account_group_position_manager(account_no);
	if (!group_position_mgr) { reset_account_profit_loss();  return; }
	account_profit_loss_p account_profit_loss = group_position_mgr->get_whole_profit_loss();
	refresh_account_profit_loss(account_profit_loss);

	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::load_position_from_fund(const std::string& fund_name)
{
	account_no_set_.clear();
	if (fund_) {
		auto sub_accounts = fund_->GetAccountVector();
		for (auto it = sub_accounts.begin(); it != sub_accounts.end(); it++) {
			account_no_set_.insert((*it)->No());
		}
	}
	group_position_manager_p group_position_mgr = mainApp.total_position_manager()->find_fund_group_position_manager(fund_name);
	if (!group_position_mgr) { reset_account_profit_loss();  return; }
	account_profit_loss_p fund_profit_loss = group_position_mgr->get_whole_profit_loss();
	refresh_account_profit_loss(fund_profit_loss);

	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::update_position(position_p position)
{
	if (!position) return;
	auto found = account_no_set_.find(position->account_no);
	if (found == account_no_set_.end()) return;

	if (position->order_source_type == DarkHorse::OrderType::SubAccount) {
		if (account_) {
			if (account_->is_subaccount())
				load_position_from_account(account_->No());
			else
				load_position_from_parent_account(account_->No());
		}
		if (fund_)
			load_position_from_fund(fund_->Name());
	}
	else if (position->order_source_type == DarkHorse::OrderType::MainAccount) {
		if (account_ && !account_->is_subaccount())
			load_position_from_parent_account(account_->No());
	}
	else if (position->order_source_type == DarkHorse::OrderType::Fund) {
		if (account_) {
			if (account_->is_subaccount())
				load_position_from_account(account_->No());
			else
				load_position_from_parent_account(account_->No());
		}
		if (fund_)
			load_position_from_fund(fund_->Name());
	}
	else return;

	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::update_profit_loss(quote_p quote)
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

void AccountProfitLossControl::set_account(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;
	account_ = account;
	account_id_ = account->id();
	if (account_->is_subaccount())
		load_position_from_account(account_->No());
	else
		load_position_from_parent_account(account_->No());
}

void AccountProfitLossControl::set_fund(std::shared_ptr<SmFund> fund)
{
	if (!fund) return;
	fund_ = fund;
	load_position_from_fund(fund_->Name());
}

void AccountProfitLossControl::refresh_account_profit_loss(account_profit_loss_p account_profit_loss)
{
	if (!account_profit_loss) return;
	account_profit_loss_.trade_profit_loss = account_profit_loss->trade_profit_loss;
	account_profit_loss_.open_profit_loss = account_profit_loss->open_profit_loss;
	account_profit_loss_.trade_fee = account_profit_loss->trade_fee;
	account_profit_loss_.pure_trade_profit_loss = account_profit_loss->pure_trade_profit_loss;
}

void AccountProfitLossControl::reset_account_profit_loss()
{
	account_profit_loss_.trade_profit_loss = 0;
	account_profit_loss_.open_profit_loss = 0;
	account_profit_loss_.trade_fee = 0;
	account_profit_loss_.pure_trade_profit_loss = 0;
}

void AccountProfitLossControl::update_account_profit_loss()
{
// 	if (!account_) return;
// 	account_position_manager_p acnt_position_mgr = mainApp.total_position_manager()->get_account_position_manager(account_->No());
// 	const std::map<std::string, position_p>& position_map = acnt_position_mgr->get_position_map();
// 
// 	if (position_map_.size() != position_map.size())
// 		return load_position_from_account(account_->No());
// 	refresh_account_profit_loss();
// 	if (event_handler_) event_handler_();
}
}