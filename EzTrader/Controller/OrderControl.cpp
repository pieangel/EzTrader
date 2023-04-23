#include "stdafx.h"
#include "OrderControl.h"
#include "../Order/Order.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Util/IdGenerator.h"
#include "../Event/EventHub.h"
namespace DarkHorse {

	OrderControl::OrderControl()
		: id_(IdGenerator::get_id())
	{
		buy_order_control_.control_type = SubOrderControlType::CT_BUY;
		sell_order_control_.control_type = SubOrderControlType::CT_SELL;

		mainApp.event_hub()->subscribe_order_event_handler
		(
			id_,
			std::bind(&OrderControl::update_order, this, std::placeholders::_1, std::placeholders::_2)
		);
	}

	OrderControl::~OrderControl()
	{
		mainApp.event_hub()->unsubscribe_order_event_handler(id_);
	}

	void OrderControl::update_order(std::shared_ptr<Order> order, OrderEvent order_event)
	{
		if (!order) return;
		if (order_event == OrderEvent::DM_Accepted)
			on_order_accepted(order);
		else if (order_event == OrderEvent::DM_Unfilled)
			on_order_unfilled(order);

		if (event_handler_) event_handler_();
	}
	void OrderControl::on_order_unfilled(std::shared_ptr<Order> order)
	{
		if (order->position == SmPositionType::Buy)
			buy_order_control_.remove_order(order->order_price, order->order_no);
		else
			sell_order_control_.remove_order(order->order_price, order->order_no);
	}

	void OrderControl::on_order_accepted(std::shared_ptr<Order> order)
	{
		AcceptedOrder order_t;
		order_t.account_no = order->account_no;
		order_t.symbol_code = order->symbol_code;
		order_t.order_no = order->order_no;
		order_t.order_amount = order->order_amount;
		order_t.position = order->position;
		order_t.order_price = order->order_price;
		if (order->position == SmPositionType::Buy)
			buy_order_control_.add_order(order->order_price, std::move(order_t));
		else
			sell_order_control_.add_order(order->order_price, std::move(order_t));
	}

	void OrderControl::add_account_id(const int account_id)
	{
		account_id_set_.insert(account_id);
	}

}