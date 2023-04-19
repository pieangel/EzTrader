#include "stdafx.h"
#include "OrderControl.h"
#include "../Order/SmOrder.h"
namespace DarkHorse {
	void OrderControl::on_order_accepted(std::shared_ptr<SmOrder> order)
	{
		if (!order) return;
		AcceptedOrder order_t;
		order_t.account_no = order->AccountNo;
		order_t.symbol_code = order->SymbolCode;
		order_t.order_no = order->OrderNo;
		order_t.order_amount = order->OrderAmount;
		order_t.position = order->PositionType;
		order_t.order_price = order->OrderPrice;
		if (order->PositionType == SmPositionType::Buy)
			buy_order_control_.add_order(order->OrderPrice, std::move(order_t));
		else
			sell_order_control_.add_order(order->OrderPrice, std::move(order_t));
		if (event_handler_) event_handler_();
	}
	void OrderControl::on_order_unfilled(std::shared_ptr<SmOrder> order)
	{
		if (!order) return;

		if (order->PositionType == SmPositionType::Buy)
			buy_order_control_.remove_order(order->OrderPrice, order->OrderNo);
		else
			sell_order_control_.remove_order(order->OrderPrice, order->OrderNo);
		if (event_handler_) event_handler_();
	}
}