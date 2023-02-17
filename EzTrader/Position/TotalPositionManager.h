#pragma once
#include <string>
#include <map>
#include <memory>
namespace DarkHorse {
class AccountPositionManager;
using account_position_manager_p = std::shared_ptr<AccountPositionManager>;
class TotalPositionManager
{
public:
	account_position_manager_p get_position_manager(const std::string& account_no);
private:
	account_position_manager_p find_position_manager(const std::string& account_no);
	account_position_manager_p create_position_manager(const std::string& account_no);
	// key : account_no, value : account position manager object.
	std::map<std::string, account_position_manager_p> position_manager_map_;
};
}

