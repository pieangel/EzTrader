#include "stdafx.h"
#include "SymbolOrderManager.h"
#include "../Order.h"
#include "../SmOrderConst.h"
#include "../../Global/SmTotalManager.h"
#include "../../Position/TotalPositionManager.h"

namespace DarkHorse {
	using total_position_manager_p = std::shared_ptr<TotalPositionManager>;
void SymbolOrderManager::dispatch_order(const int order_event, order_p order)
{
	switch (order_event) {
	case OrderEvent::AB_Accepted:
	case OrderEvent::DM_Accepted:
		on_order_accepted(order);
		break;
	case OrderEvent::AB_Unfilled:
	case OrderEvent::DM_Unfilled:
		on_order_unfilled(order);
		break;
	case OrderEvent::AB_Filled:
	case OrderEvent::DM_Filled:
		on_order_filled(order);
		break;
	}
}

void SymbolOrderManager::on_order_accepted(order_p order)
{
	order->order_state = SmOrderState::Accepted;
	add_accepted_order(order);
}
void SymbolOrderManager::on_order_unfilled(order_p order)
{
	;
}
void SymbolOrderManager::on_order_filled(order_p order)
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
}