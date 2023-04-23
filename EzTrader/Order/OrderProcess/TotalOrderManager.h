#pragma once
#include <string>
#include <map>
#include <memory>
#include "../../Json/json.hpp"
namespace DarkHorse {
using order_event = nlohmann::json;
struct Order;
struct OrderRequest;

using order_request_p = std::shared_ptr<OrderRequest>;
using order_p = std::shared_ptr<Order>;

class AccountOrderManager;
class TotalOrderManager
{
	using account_order_manager_p = std::shared_ptr<AccountOrderManager>;
public:
	void on_order_event(const order_event& order_info);
	void on_order_event(order_event&& order_info);
	account_order_manager_p get_order_manager(const std::string& account_no);
private:
	void write_order_history(const int order_event, order_p order) ;
	void dispatch_order(const int order_event, order_p order) ;
	order_p get_order(const std::string& order_no) ;
	order_p find_order(const std::string& order_no) ;
	order_p create_order(const std::string& order_no);
	order_p make_order(const order_event& order_info);
	void set_order_request_info(const std::string& custom_info, order_p order);
	account_order_manager_p find_order_manager(const std::string& account_no);
	account_order_manager_p create_order_manager(const std::string& account_no);
	/// <summary>
	/// key : order_no, value : order_object.
	/// </summary>
	std::map<std::string, order_p> order_map_;
	/// <summary>
	/// key : account_no, value : account_order_manager object.
	/// </summary>
	std::map<std::string, account_order_manager_p> order_manager_map_;
};
}
