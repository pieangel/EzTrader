#pragma once
#include <string>
#include <map>
#include <memory>
#include "../../Json/json.hpp"
namespace DarkHorse {
using order_event = nlohmann::json;
struct Order;
using order_p = std::shared_ptr<Order>;
class SymbolOrderManager
{
public:
    void dispatch_order(const int order_event, order_p order);
private:
	void on_ab_order_accepted(order_p order);
	void on_ab_order_unfilled(order_p order);
	void on_ab_order_filled(order_p order);
	void on_dm_order_accepted(order_p order);
	void on_dm_order_unfilled(order_p order);
	void on_dm_order_filled(order_p order);
};
}

