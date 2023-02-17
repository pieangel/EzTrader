#include "stdafx.h"
#include "SymbolOrderManager.h"
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
	;
}
void SymbolOrderManager::on_ab_order_unfilled(order_p order)
{
	;
}
void SymbolOrderManager::on_ab_order_filled(order_p order)
{
	;
}
void SymbolOrderManager::on_dm_order_accepted(order_p order)
{
	;
}
void SymbolOrderManager::on_dm_order_unfilled(order_p order)
{
	;
}
void SymbolOrderManager::on_dm_order_filled(order_p order)
{
	;
}
}