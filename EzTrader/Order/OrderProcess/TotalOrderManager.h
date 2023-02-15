#pragma once
#include <string>
#include <map>
#include <memory>
#include "../../Json/json.hpp"
namespace DarkHorse {
using order_event = nlohmann::json;
class AccountOrderManager;
class TotalOrderManager
{
	using account_order_manager_p = std::shared_ptr<AccountOrderManager>;
public:
	void on_order_event(const order_event& order_info);
	account_order_manager_p get_order_manager(const std::string& account_no);
private:
	std::map<std::string, account_order_manager_p> account_order_manager_map_;
	account_order_manager_p find_order_manager(const std::string& account_no);
	account_order_manager_p create_order_manager(const std::string& account_no);
};
}
