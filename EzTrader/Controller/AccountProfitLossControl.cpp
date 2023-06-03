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
	acnt_position_mgr->update_account_profit_loss();
	auto account_profit_loss_p = acnt_position_mgr->get_account_profit_loss();
	account_profit_loss_.trade_profit_loss = account_profit_loss_p->trade_profit_loss;
	account_profit_loss_.open_profit_loss = account_profit_loss_p->open_profit_loss;
	account_profit_loss_.trade_fee = account_profit_loss_p->trade_fee;
	account_profit_loss_.pure_trade_profit_loss = account_profit_loss_p->pure_trade_profit_loss;

	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::update_position(position_p position)
{
	if (!position || position->account_id != account_id_) return;

	if (position->open_quantity == 0) {
		update_account_profit_loss();
		remove_position(position->symbol_code);
	}
	else {
		add_position(position);
		update_account_profit_loss();
	}
	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::update_profit_loss(quote_p quote)
{
	if (!quote) return;
	auto symbol = mainApp.SymMgr()->FindSymbol(quote->symbol_code);
	if (!symbol) return;
	position_p position = get_position(quote->symbol_code);
	if (!position) return;

	//double open_profit_loss = 0;
	//open_profit_loss = position->open_quantity * (quote->close - position->average_price) * symbol->seung_su();
	//open_profit_loss = open_profit_loss / pow(10, symbol->decimal());
	//position->open_profit_loss = open_profit_loss;

	position->open_profit_loss = TotalPositionManager::calculate_symbol_open_profit_loss(position->open_quantity, quote->close, position->average_price, symbol->seung_su(), symbol->decimal());

	update_account_profit_loss();

	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::update_account_profit_loss()
{
	double trade_profit_loss{ 0.0f };       //매매(청산)손익
	double open_profit_loss{ 0.0f };		//평가손익
	double pure_trade_profit_loss{ 0.0f };  // 청산 순손익
	double trade_fee{ 0.0f };               // 청산 수수료
	for (auto it = position_map_.begin(); it != position_map_.end(); it++) {
		auto position = it->second;
		trade_profit_loss += position->trade_profit_loss;
		open_profit_loss += position->open_profit_loss;
		trade_fee += position->trade_fee;
		pure_trade_profit_loss += position->pure_trade_profit_loss;
	}
	account_profit_loss_.trade_profit_loss = trade_profit_loss;
	account_profit_loss_.open_profit_loss = open_profit_loss;
	account_profit_loss_.trade_fee = trade_fee;
	account_profit_loss_.pure_trade_profit_loss = pure_trade_profit_loss;
}

DarkHorse::position_p AccountProfitLossControl::get_position(const std::string& symbol_code)
{
	auto it = position_map_.find(symbol_code);
	if (it == position_map_.end()) return nullptr;
	return it->second;
}

void AccountProfitLossControl::add_position(position_p position)
{
	auto it = position_map_.find(position->symbol_code);
	if (it != position_map_.end()) return;

	position_map_[position->symbol_code] = position;
}

void AccountProfitLossControl::remove_position(const std::string& symbol_code)
{
	auto it = position_map_.find(symbol_code);
	if (it == position_map_.end()) return;
	position_map_.erase(it);
}

}