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
#include "../Fund/SmFund.h"

namespace DarkHorse {
	SymbolPositionControl::SymbolPositionControl()
		: id_(IdGenerator::get_id())
	{
		mainApp.event_hub()->subscribe_position_event_handler
		(
			id_,
			std::bind(&SymbolPositionControl::update_position, this, std::placeholders::_1)
		);
		mainApp.event_hub()->subscribe_quote_event_handler
		(
			id_,
			std::bind(&SymbolPositionControl::update_profit_loss, this, std::placeholders::_1)
		);

		LOGINFO(CMyLogger::getInstance(), "SymbolPositionControl contructor id = %d", id_);
	}

	SymbolPositionControl::~SymbolPositionControl()
	{
		mainApp.event_hub()->unsubscribe_position_event_handler(id_);
		mainApp.event_hub()->unsubscribe_quote_event_handler(id_);

		LOGINFO(CMyLogger::getInstance(), "SymbolPositionControl destructor id = %d", id_);
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
			//LOGINFO(CMyLogger::getInstance(), "SymbolPositionControl update_position id = %d , account_no = %s, fund_name = %s", id_, position->account_no.c_str(), position->fund_name.c_str());

			auto it = account_map_.find(position->account_no);
			if (it == account_map_.end()) return;

			clear_position();
			
			if (position_type_ == PositionType::SubAccount) {
				if (!account_ || !symbol_) return;
				mainApp.total_position_manager()->get_position_from_account(account_->No(), symbol_->SymbolCode(), position_, position_map_);
			}
			else if (position_type_ == PositionType::MainAccount) {
				if (!account_ || !symbol_) return;
				mainApp.total_position_manager()->get_position_from_parent_account(account_->No(), symbol_->SymbolCode(), position_, position_map_);
			}
			else {
				if (!fund_ || !symbol_) return;
				mainApp.total_position_manager()->get_position_from_fund(fund_->Name(), symbol_->SymbolCode(), position_, position_map_);
			}

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
			mainApp.total_position_manager()->get_position_from_account(account_no, symbol_code, position_, position_map_);
		else 
			mainApp.total_position_manager()->get_position_from_parent_account(account_no, symbol_code, position_, position_map_);
	}

	void SymbolPositionControl::update_position_from_account(std::shared_ptr<SmAccount> account, std::shared_ptr<SmSymbol> symbol)
	{
		if (!account || !symbol) return;
		clear_position();
		account_ = account;
		symbol_ = symbol;
		position_.account_id = account->id();
		position_.symbol_id = symbol->Id();
		position_.account_no = account->No();
		position_.symbol_code = symbol->SymbolCode();
		if (account->is_subaccount()) {
			mainApp.total_position_manager()->get_position_from_account(account->No(), symbol->SymbolCode(), position_, position_map_);
			position_type_ = PositionType::SubAccount;
		}
		else {
			mainApp.total_position_manager()->get_position_from_parent_account(account->No(), symbol->SymbolCode(), position_, position_map_);
			position_type_ = PositionType::MainAccount;
		}
		if (event_handler_) event_handler_();
	}

	void SymbolPositionControl::update_position_from_fund(const std::string& fund_name, const std::string& symbol_code)
	{
		mainApp.total_position_manager()->get_position_from_fund(fund_name, symbol_code, position_, position_map_);
		position_type_ = PositionType::Fund;
	}

	void SymbolPositionControl::update_position_from_fund(std::shared_ptr<SmFund> fund, std::shared_ptr<SmSymbol> symbol)
	{
		if (!fund || !symbol) return;
		clear_position();
		fund_ = fund;
		symbol_ = symbol;
		position_.account_id = fund->Id();
		position_.symbol_id = symbol->Id();
		position_.fund_name = fund->Name();
		position_.symbol_code = symbol->SymbolCode();
		position_type_ = PositionType::Fund;
		mainApp.total_position_manager()->get_position_from_fund(fund->Name(), symbol->SymbolCode(), position_, position_map_);
		if (event_handler_) event_handler_();
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

	void SymbolPositionControl::set_account(std::shared_ptr<SmAccount> account)
	{
		if (!account) return;
		account_ = account;
		account_id_ = account_->id();
		if (account_->is_subaccount()) {
			account_map_.clear();
			account_map_[account_->No()] = account_;
		}
		else {
			account_map_[account_->No()] = account_;
			const auto& account_vector = account_->get_sub_accounts();
			for (auto it = account_vector.begin(); it != account_vector.end(); ++it) {
				auto sub_account = *it;
				account_map_[sub_account->No()] = sub_account;
			}
		}
	}

	void SymbolPositionControl::set_fund(std::shared_ptr<SmFund> fund)
	{
		if (!fund) return;
		fund_ = fund;
		const auto& account_vector = fund->GetAccountVector();
		for (auto it = account_vector.begin(); it != account_vector.end(); ++it) {
			auto sub_account = *it;
			account_map_[sub_account->No()] = sub_account;
		}
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

	void SymbolPositionControl::clear_position()
	{
		position_map_.clear();

		position_.average_price = 0.0;
		position_.open_quantity = 0;
		position_.open_profit_loss = 0.0;
		position_.trade_profit_loss = 0.0;
	}

}