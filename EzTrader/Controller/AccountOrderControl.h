#pragma once
#include <map>
#include <string>
#include <memory>
#include <functional>
#include "../Order/SmOrderConst.h"
namespace DarkHorse {
struct Order;
using order_p = std::shared_ptr<Order>;
class AccountOrderControl
{
public:
	AccountOrderControl();
	~AccountOrderControl();
	void load_order_from_account(const std::string& account_no);
	void update_order(order_p order, OrderEvent order_event);
	void set_event_handler(std::function<void()> event_handler) {
		event_handler_ = event_handler;
	}
	const std::map<std::string, order_p>& get_order_map() {
		return order_map_;
	}
private:
	void on_order_unfilled(std::shared_ptr<Order> order);
	void on_order_accepted(std::shared_ptr<Order> order);
	void add_accepted_order(const std::map<std::string, order_p>& accepted_order_map);
	order_p get_order(const std::string& order_no);
	void add_order(order_p order);
	void remove_order(order_p order);
	// key : order no, value : order object.
	std::map<std::string, order_p> order_map_;
	std::function<void()> event_handler_;
	int id_{ 0 };
};
}
