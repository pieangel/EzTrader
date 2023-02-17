#pragma once
#include <string>
#include <map>
#include <memory>
namespace DarkHorse {
class AccountPositionManager;
struct Order;
using order_p = std::shared_ptr<Order>;
struct Position;
using account_position_manager_p = std::shared_ptr<AccountPositionManager>;
using position_p = std::shared_ptr<Position>;
class TotalPositionManager
{
public:
	position_p get_position(const std::string& account_no, const std::string& symbol_code);
	account_position_manager_p get_position_manager(const std::string& account_no);
	void update_position(order_p order);
private:
	account_position_manager_p find_position_manager(const std::string& account_no);
	account_position_manager_p create_position_manager(const std::string& account_no);
	// key : account_no, value : account position manager object.
	std::map<std::string, account_position_manager_p> position_manager_map_;
};
}

