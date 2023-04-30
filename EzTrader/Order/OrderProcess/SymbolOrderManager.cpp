#include "stdafx.h"
#include "SymbolOrderManager.h"
#include "../Order.h"
#include "../SmOrderConst.h"
#include "../../Global/SmTotalManager.h"
#include "../../Position/TotalPositionManager.h"
#include "../../Event/EventHub.h"
#include "../SmOrder.h"

namespace DarkHorse {
	using total_position_manager_p = std::shared_ptr<TotalPositionManager>;
void SymbolOrderManager::dispatch_order(const OrderEvent order_event, order_p order)
{
	switch (order_event) {
	case OrderEvent::AB_Accepted:
	case OrderEvent::DM_Accepted:
		on_order_accepted(order, order_event);
		break;
	case OrderEvent::AB_Unfilled:
	case OrderEvent::DM_Unfilled:
		on_order_unfilled(order, order_event);
		break;
	case OrderEvent::AB_Filled:
	case OrderEvent::DM_Filled:
		on_order_filled(order, order_event);
		break;
	}
}

void SymbolOrderManager::on_order_accepted(order_p order, OrderEvent order_event)
{
	order->order_state = SmOrderState::Accepted;
	if (order->order_type != SmOrderType::Cancel) {
		add_accepted_order(order);
		mainApp.event_hub()->process_order_event(order, order_event);
	}
}
void SymbolOrderManager::on_order_unfilled(order_p order, OrderEvent order_event)
{
	if (order->remain_count == 0)
		remove_accepted_order(order);
	mainApp.event_hub()->process_order_event(order, order_event);
}
void SymbolOrderManager::on_order_filled(order_p order, OrderEvent order_event)
{
	order->order_state = SmOrderState::Filled;
	update_accepted_order(order);
	total_position_manager_p total_position_manager = mainApp.total_position_manager();
	total_position_manager->update_position(order);
}

void SymbolOrderManager::add_accepted_order(order_p order)
{
	accepted_order_map_[order->order_no] = order;
}

void SymbolOrderManager::update_accepted_order(order_p order)
{
	;
}

void SymbolOrderManager::remove_accepted_order(order_p order)
{
	auto it = accepted_order_map_.find(order->order_no);
	if (it == accepted_order_map_.end()) return;
	accepted_order_map_.erase(it);
}
}