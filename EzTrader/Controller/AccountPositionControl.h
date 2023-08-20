#pragma once
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <set>
namespace DarkHorse {
struct Position;
struct SmQuote;
using position_p = std::shared_ptr<Position>;
using quote_p = std::shared_ptr<SmQuote>;
class SmFund;
class SmAccount;
class AccountPositionControl
{
public:
	AccountPositionControl();
	~AccountPositionControl();
	void update_position(position_p position);
	void update_profit_loss(quote_p quote);
	void set_event_handler(std::function<void()> event_handler) {
		event_handler_ = event_handler;
	}
	const std::map<std::string, position_p>& get_position_map() {
		return position_map_;
	}
	void set_account(std::shared_ptr<SmAccount> account);
	void set_fund(std::shared_ptr<SmFund> fund);
private:
	void load_position_from_account(const std::string& account_no);
	void load_position_from_parent_account(const std::string& account_no);
	void load_position_from_fund(const std::string& fund_name);
	position_p get_position(const std::string& symbol_code);
	// key : symbol code, value : position object.
	std::map<std::string, position_p> position_map_;
	std::function<void()> event_handler_;
	int id_{ 0 };
	std::string account_no_;
	std::set<std::string> account_no_set_;
	std::shared_ptr<SmAccount> account_;
	std::shared_ptr<SmFund> fund_;
};
}
