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
namespace DarkHorse {
using account_position_manager_p = std::shared_ptr<AccountPositionManager>;
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
	account_no_ = account_no;
	position_map_.clear();
	account_position_manager_p acnt_position_mgr = mainApp.total_position_manager()->get_account_position_manager(account_no);
	const std::map<std::string, position_p>& position_map = acnt_position_mgr->get_position_map();
	for (auto it = position_map.begin(); it != position_map.end(); it++) {
		position_map_[it->second->symbol_code] = it->second;
	}
}

void AccountPositionControl::update_position(position_p position)
{
	if (!position) return;
	if (position->order_source_type == DarkHorse::OrderType::MainAccount) {
		if (account_no_ != position->account_no) return;
	}
	else {
		if (account_no_ != position->parent_account_no) return;
	}
	auto symbol_position = mainApp.total_position_manager()->get_position(account_no_, position->symbol_code);
	if (position->open_quantity == 0)
		remove_position(symbol_position->symbol_code);
	else
		add_position(symbol_position);
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

position_p AccountPositionControl::get_position(const std::string& symbol_code)
{
	auto it = position_map_.find(symbol_code);
	if (it == position_map_.end()) return nullptr;
	return it->second;
}

void AccountPositionControl::add_position(position_p position)
{
	position_map_[position->symbol_code] = position;
}

void AccountPositionControl::remove_position(const std::string& symbol_code)
{
	auto it = position_map_.find(symbol_code);
	if (it == position_map_.end()) return;
	position_map_.erase(it);
}

}