#pragma once
#include <map>
#include <string>
#include <memory>
#include <functional>
namespace DarkHorse {
struct Position;
struct SmQuote;
using position_p = std::shared_ptr<Position>;
using quote_p = std::shared_ptr<SmQuote>;
class AccountPositionControl
{
public:
	AccountPositionControl();
	~AccountPositionControl();
	void load_position_from_account(const std::string& account_no);
	void update_position(position_p position);
	void update_profit_loss(quote_p quote);
	void set_event_handler(std::function<void()> event_handler) {
		event_handler_ = event_handler;
	}
	const std::map<std::string, position_p>& get_position_map() {
		return position_map_;
	}
private:
	position_p get_position(const std::string& symbol_code);
	void add_position(position_p position);
	void remove_position(const std::string& symbol_code);
	// key : symbol code, value : position object.
	std::map<std::string, position_p> position_map_;
	std::function<void()> event_handler_;
	int id_{ 0 };
};
}
