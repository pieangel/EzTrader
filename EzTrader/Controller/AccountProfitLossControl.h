#pragma once
#include <map>
#include <string>
#include <memory>
#include <functional>
#include "../ViewModel/VmAccountProfitLoss.h"
#include "../Account/SmAccount.h"
namespace DarkHorse {
	struct Position;
	struct SmQuote;
	class SmAccount;
	class SmFund;
	using position_p = std::shared_ptr<Position>;
	using quote_p = std::shared_ptr<SmQuote>;
	class AccountProfitLossControl
	{
	public:
		AccountProfitLossControl();
		~AccountProfitLossControl();
		void load_position_from_account(const std::string& account_no);
		void update_position(position_p position);
		void update_profit_loss(quote_p quote);
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
		const std::map<std::string, position_p>& get_position_map() {
			return position_map_;
		}
		const VmAccountProfitLoss& get_account_profit_loss() {
			return account_profit_loss_;
		}
		void set_account_id(const int account_id) {
			account_id_ = account_id;
		}
		void set_account(std::shared_ptr<DarkHorse::SmAccount> account) {
			account_ = account;
			account_id_ = account->id();
		}
		void set_fund(std::shared_ptr<SmFund> fund) {
			fund_ = fund;
		}
	private:
		void refresh_account_profit_loss();
		void update_account_profit_loss();
		// key : symbol code, value : position object.
		std::map<std::string, position_p> position_map_;
		std::function<void()> event_handler_;
		int id_{ 0 };
		int account_id_{ 0 };
		std::shared_ptr<DarkHorse::SmAccount> account_{nullptr};
		VmAccountProfitLoss account_profit_loss_;
		std::shared_ptr<SmFund> fund_;
	};
}
