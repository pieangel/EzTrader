#include "stdafx.h"
#include "AccountOrderControl.h"
#include "../Util/IdGenerator.h"
#include "../Global/SmTotalManager.h"
#include "../Order/OrderProcess/TotalOrderManager.h"
#include "../Order/OrderProcess/AccountOrderManager.h"
#include "../Order/OrderProcess/SymbolOrderManager.h"
#include "../Order/Order.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"

namespace DarkHorse {
AccountOrderControl::AccountOrderControl()
	: id_(IdGenerator::get_id())
{
	mainApp.event_hub()->subscribe_order_event_handler
	(
		id_,
		std::bind(&AccountOrderControl::update_order, this, std::placeholders::_1, std::placeholders::_2)
	);
}

AccountOrderControl::~AccountOrderControl()
{
	mainApp.event_hub()->unsubscribe_order_event_handler(id_);
}

void AccountOrderControl::load_order_from_account(const std::string& account_no)
{
	auto account_order_manager = mainApp.total_order_manager()->get_account_order_manager(account_no);
	auto symbol_order_manager_map = account_order_manager->get_symbol_order_manager_map();
	for (auto it = symbol_order_manager_map.begin(); it != symbol_order_manager_map.end(); it++) {
		auto accepted_order_map = it->second->get_accepted_order_map();
		add_accepted_order(accepted_order_map);
	}
}

void AccountOrderControl::update_order(order_p order, OrderEvent order_event)
{
	if (!order) return;
	if (order_event == OrderEvent::OE_Accepted)
		on_order_accepted(order);
	else if (order_event == OrderEvent::OE_Unfilled)
		on_order_unfilled(order);

	if (event_handler_) event_handler_();
}

void AccountOrderControl::on_order_unfilled(std::shared_ptr<Order> order)
{
	if (order->remain_count == 0)
		remove_order(order);
	else
		add_order(order);
}

void AccountOrderControl::on_order_accepted(std::shared_ptr<Order> order)
{
	add_order(order);
}

void AccountOrderControl::add_accepted_order(const std::map<std::string, order_p>& accepted_order_map)
{
	if (accepted_order_map.empty()) return;
	for (auto it = accepted_order_map.begin(); it != accepted_order_map.end(); it++) {
		add_order(it->second);
	}
}

DarkHorse::order_p AccountOrderControl::get_order(const std::string& order_no)
{
	auto it = order_map_.find(order_no);
	if (it == order_map_.end()) return nullptr;
	return it->second;
}

void AccountOrderControl::add_order(order_p order)
{
	auto it = order_map_.find(order->order_no);
	if (it != order_map_.end()) return;
	order_map_[order->order_no] = order;
}

void AccountOrderControl::remove_order(order_p order)
{
	auto it = order_map_.find(order->order_no);
	if (it == order_map_.end()) return;
	order_map_.erase(it);
}

}