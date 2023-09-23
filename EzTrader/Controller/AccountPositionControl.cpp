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
#include "../Event/SmCallbackManager.h"
namespace DarkHorse {
using account_position_manager_p = std::shared_ptr<AccountPositionManager>;
using group_position_manager_p = std::shared_ptr<GroupPositionManager>;
AccountPositionControl::AccountPositionControl()
	: id_(IdGenerator::get_id())
{
	mainApp.CallbackMgr()->subscribe_position_event_handler
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
	mainApp.CallbackMgr()->unsubscribe_position_event_handler(id_);
	mainApp.event_hub()->unsubscribe_quote_event_handler(id_);
}

void AccountPositionControl::update_position(position_p position)
{
	if (!position) return;
	if (position_type_ == OrderType::None) return;
	if (position_type_ == OrderType::SubAccount) {
		if (!account_ || account_->No() != position->account_no) return;
		auto position_manager = mainApp.total_position_manager()->find_position_manager(position->account_no);
		if (!position_manager) return;
		active_position_vector_.clear();
		position_manager->get_active_positions(active_position_vector_);
	}
	else if (position_type_ == OrderType::Fund) {
		if (!fund_ || fund_->Name() != position->fund_name) return;
		auto position_manager = mainApp.total_position_manager()->find_fund_group_position_manager(position->fund_name);
		if (!position_manager) return;
		active_position_vector_.clear();
		position_manager->get_active_positions(active_position_vector_);
	}
	else{
		if (!account_ || account_->No() != position->account_no) return;
		auto position_manager = mainApp.total_position_manager()->find_account_group_position_manager(position->account_no);
		if (!position_manager) return;
		active_position_vector_.clear();
		position_manager->get_active_positions(active_position_vector_);
	}
	
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

const std::vector<position_p>& AccountPositionControl::get_active_position_vector()
{
	load_position();
	return active_position_vector_;
}

void AccountPositionControl::set_account(std::shared_ptr<SmAccount> account)
{
	if (!account) return;
	account_ = account;
	if (account_->is_subaccount()) {
		position_type_ = OrderType::SubAccount;
	}
	else {
		position_type_ = OrderType::MainAccount;
	}
	load_position();
}

void AccountPositionControl::set_fund(std::shared_ptr<SmFund> fund)
{
	if (!fund) return;
	fund_ = fund;
	position_type_ = OrderType::Fund;
	load_position();
}

void AccountPositionControl::load_position()
{
	if (position_type_ == OrderType::None) return;
	if (position_type_ == OrderType::SubAccount) {
		if (!account_ ) return;
		auto position_manager = mainApp.total_position_manager()->find_position_manager(account_->No());
		if (!position_manager) return;
		active_position_vector_.clear();
		position_manager->get_active_positions(active_position_vector_);
	}
	else if (position_type_ == OrderType::Fund) {
		if (!fund_ ) return;
		auto position_manager = mainApp.total_position_manager()->find_fund_group_position_manager(fund_->Name());
		if (!position_manager) return;
		active_position_vector_.clear();
		position_manager->get_active_positions(active_position_vector_);
	}
	else {
		if (!account_ ) return;
		auto position_manager = mainApp.total_position_manager()->find_account_group_position_manager(account_->No());
		if (!position_manager) return;
		active_position_vector_.clear();
		position_manager->get_active_positions(active_position_vector_);
	}

	if (event_handler_) event_handler_();
}

position_p AccountPositionControl::get_position(const std::string& symbol_code)
{
	auto it = position_map_.find(symbol_code);
	if (it == position_map_.end()) return nullptr;
	return it->second;
}
}