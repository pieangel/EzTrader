#include "stdafx.h"
#include "SymbolOrderManager.h"
#include "../Order.h"
#include "../SmOrderConst.h"
namespace DarkHorse {
void SymbolOrderManager::dispatch_order(const int order_event, order_p order)
{
	switch (order_event) {
	case OrderEvent::AB_Accepted:
		on_ab_order_accepted(order);
		break;
	case OrderEvent::AB_Unfilled:
		on_ab_order_unfilled(order);
		break;
	case OrderEvent::AB_Filled:
		on_ab_order_filled(order);
		break;
	case OrderEvent::DM_Accepted:
		on_dm_order_accepted(order);
		break;
	case OrderEvent::DM_Unfilled:
		on_dm_order_unfilled(order);
		break;
	case OrderEvent::DM_Filled:
		on_dm_order_filled(order);
		break;
	}
}

void SymbolOrderManager::on_ab_order_accepted(order_p order)
{
	order->order_state = SmOrderState::Accepted;
	add_accepted_order(order);
}
void SymbolOrderManager::on_ab_order_unfilled(order_p order)
{
	;
}
void SymbolOrderManager::on_ab_order_filled(order_p order)
{
	order->order_state = SmOrderState::Filled;
}
void SymbolOrderManager::on_dm_order_accepted(order_p order)
{
	order->order_state = SmOrderState::Accepted;
}
void SymbolOrderManager::on_dm_order_unfilled(order_p order)
{
	;
}
void SymbolOrderManager::on_dm_order_filled(order_p order)
{
	order->order_state = SmOrderState::Filled;
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