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

void AccountProfitLossControl::load_position_from_account(const std::string& account_no)
{
	position_map_.clear();
	account_position_manager_p acnt_position_mgr = mainApp.total_position_manager()->get_account_position_manager(account_no);
	const std::map<std::string, position_p>& position_map = acnt_position_mgr->get_position_map();
	for (auto it = position_map.begin(); it != position_map.end(); it++) {
		position_map_[it->second->symbol_code] = it->second;
	}
	refresh_account_profit_loss();

	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::update_position(position_p position)
{
	if (!position || position->account_id != account_id_ || !account_) return;

	account_position_manager_p acnt_position_mgr = mainApp.total_position_manager()->get_account_position_manager(account_->No());
	const std::map<std::string, position_p>& position_map = acnt_position_mgr->get_position_map();

	if (position_map_.size() != position_map.size())
		return	load_position_from_account(account_->No());
	refresh_account_profit_loss();
	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::update_profit_loss(quote_p quote)
{
	if (!quote) return;
	auto symbol = mainApp.SymMgr()->FindSymbol(quote->symbol_code);
	if (!symbol) return;
	if (!account_) return;
	account_position_manager_p acnt_position_mgr = mainApp.total_position_manager()->get_account_position_manager(account_->No());
	const std::map<std::string, position_p>& position_map = acnt_position_mgr->get_position_map();

	if (position_map_.size() != position_map.size())
		return load_position_from_account(account_->No());
	refresh_account_profit_loss();
	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::set_account(std::shared_ptr<DarkHorse::SmAccount> account)
{
	account_ = account;
	account_id_ = account->id();
}

void AccountProfitLossControl::set_fund(std::shared_ptr<SmFund> fund)
{
	fund_ = fund;
}

void AccountProfitLossControl::refresh_account_profit_loss()
{
	if (!account_) return;
	account_position_manager_p acnt_position_mgr = mainApp.total_position_manager()->get_account_position_manager(account_->No());
	auto account_profit_loss_p = acnt_position_mgr->get_account_profit_loss();
	account_profit_loss_.trade_profit_loss = account_profit_loss_p->trade_profit_loss;
	account_profit_loss_.open_profit_loss = account_profit_loss_p->open_profit_loss;
	account_profit_loss_.trade_fee = account_profit_loss_p->trade_fee;
	account_profit_loss_.pure_trade_profit_loss = account_profit_loss_p->pure_trade_profit_loss;
}

void AccountProfitLossControl::update_account_profit_loss()
{
	if (!account_) return;
	account_position_manager_p acnt_position_mgr = mainApp.total_position_manager()->get_account_position_manager(account_->No());
	const std::map<std::string, position_p>& position_map = acnt_position_mgr->get_position_map();

	if (position_map_.size() != position_map.size())
		return load_position_from_account(account_->No());
	refresh_account_profit_loss();
	if (event_handler_) event_handler_();
}
}