#include "stdafx.h"
#include "SymbolPositionControl.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../Position/Position.h"
#include "../Log/MyLogger.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Position/TotalPositionManager.h"

namespace DarkHorse {
	SymbolPositionControl::SymbolPositionControl()
		: id_(IdGenerator::get_id())
	{
		subscribe_position_control();
		mainApp.event_hub()->subscribe_quote_event_handler
		(
			id_,
			std::bind(&SymbolPositionControl::update_profit_loss, this, std::placeholders::_1)
		);
	}

	SymbolPositionControl::~SymbolPositionControl()
	{
		mainApp.event_hub()->unsubscribe_position_event_handler(id_);
		mainApp.event_hub()->unsubscribe_quote_event_handler(id_);
	}

	void SymbolPositionControl::update_profit_loss(std::shared_ptr<SmQuote> quote)
	{
		if (symbol_id_ != 0 && quote->symbol_id != symbol_id_ ) return;
		position_.open_profit_loss = TotalPositionManager::calculate_symbol_open_profit_loss(position_.open_quantity, quote->close, position_.average_price, symbol_seung_su_, symbol_decimal_);
		if (event_handler_) event_handler_();
	}

	void SymbolPositionControl::update_position(std::shared_ptr<Position> position)
	{
		try {
			if (!position) return;
			if (symbol_id_ != 0 && 
				(position->symbol_id != symbol_id_ || 
				 position->account_id != account_id_)) 
				return;
			position_.account_id = position->account_id;
			position_.symbol_id = position->symbol_id;
			position_.account_no = position->account_no;
			position_.symbol_code = position->symbol_code;
			position_.open_quantity = position->open_quantity;
			position_.average_price = position->average_price;
			position_.open_profit_loss = position->open_profit_loss;
			position_.trade_fee = position->trade_fee;
			position_.trade_profit_loss = position->trade_profit_loss;
			position_.pure_trade_profit_loss = position->pure_trade_profit_loss;

			if (event_handler_) event_handler_();
		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	void SymbolPositionControl::update_position_from_account(const bool is_sub_account, const std::string& account_no, const std::string& symbol_code)
	{
		if (is_sub_account)
			mainApp.total_position_manager()->get_position_from_account(account_no, symbol_code, position_);
		else 
			mainApp.total_position_manager()->get_position_from_parent_account(account_no, symbol_code, position_);
	}

	void SymbolPositionControl::update_position_from_fund(const std::string& fund_name, const std::string& symbol_code)
	{
		mainApp.total_position_manager()->get_position_from_fund(fund_name, symbol_code, position_);
	}

	void SymbolPositionControl::set_symbol_id(const int symbol_id)
	{
		symbol_id_ = symbol_id;
		auto symbol = mainApp.SymMgr()->FindSymbolById(symbol_id_);
		if (!symbol) return;
		symbol_decimal_ = symbol->decimal();
		symbol_seung_su_ = symbol->seung_su();
		reset_position();
	}

	void SymbolPositionControl::set_account_id(const int account_id)
	{
		account_id_ = account_id;
		reset_position();
	}

	void SymbolPositionControl::reset_position()
	{
		if (account_id_ == 0 || symbol_id_ == 0) return;

		auto account = mainApp.AcntMgr()->FindAccountById(account_id_);
		auto symbol = mainApp.SymMgr()->FindSymbolById(symbol_id_);
		if (!account || !symbol) return;

		auto position = mainApp.total_position_manager()->get_position(account->No(), symbol->SymbolCode());
		if (!position) return;

		update_position(position);
	}

	void SymbolPositionControl::subscribe_position_control()
	{
		mainApp.event_hub()->subscribe_position_event_handler
		(
			id_,
			std::bind(&SymbolPositionControl::update_position, this, std::placeholders::_1)
		);
	}

}