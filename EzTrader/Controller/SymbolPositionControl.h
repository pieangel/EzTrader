#pragma once
#include "../ViewModel/VmPosition.h"
#include "../Util/IdGenerator.h"
#include "../Position/PositionConst.h"
#include <memory>
#include <functional>
#include <map>
#include <string>
namespace DarkHorse {
	struct Position;
	struct SmQuote;
	class SmAccount;
	class SmFund;
	class SmSymbol;
	class SymbolPositionControl
	{
	public:
		SymbolPositionControl();
		~SymbolPositionControl();
		void update_profit_loss(std::shared_ptr<SmQuote> quote);
		void update_position(std::shared_ptr<Position> position);
		void update_position_from_account(const bool is_sub_account, const std::string& account_no, const std::string& symbol_code);
		void update_position_from_fund(const std::string& fund_name, const std::string& symbol_code);

		void update_position_from_account(std::shared_ptr<SmAccount> account, std::shared_ptr<SmSymbol> symbol);
		void update_position_from_fund(std::shared_ptr<SmFund> account, std::shared_ptr<SmSymbol> symbol);
		const VmPosition& get_position()
		{
			return position_;
		}
		const int get_id()
		{
			return id_;
		}
		void set_symbol_id(const int symbol_id);
		//void set_account_id(const int account_id);
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
		PositionType Position_type() const { return position_type_; }
		void Position_type(PositionType val) { position_type_ = val; }
		void set_account(std::shared_ptr<SmAccount> account);
		void set_fund(std::shared_ptr<SmFund> fund);
		void set_out_system_event_handler(std::function<void(const int)> out_system_event_handler) {
			out_system_event_handler_ = out_system_event_handler;
		}
		void set_out_system_id(const int out_system_id) {
			out_system_id_ = out_system_id;
		};
	private:
		PositionType position_type_{ PositionType::None };
		// key : account no, value : position object.
		std::map<std::string, std::shared_ptr<Position>> position_map_;
		// key : account no, value : account object
		std::map<std::string, std::shared_ptr<SmAccount>> account_map_;
		void reset_position();
		int symbol_seung_su_{ 1 };
		int symbol_decimal_{ 1 };
		int id_{ 0 };
		int symbol_id_{ 0 };
		int account_id_{ 0 };
		int fund_id_{ 0 };
		int out_system_id_{ 0 };
		std::shared_ptr<SmAccount> account_{ nullptr };
		std::shared_ptr<SmFund> fund_{ nullptr };
		std::shared_ptr<SmSymbol> symbol_{ nullptr };
		VmPosition position_;
		void subscribe_position_control();
		void clear_position();
		std::function<void()> event_handler_;
		std::function<void(const int)> out_system_event_handler_;
	};
}
