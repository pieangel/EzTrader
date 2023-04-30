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
		buy_order_control_.set_control_type( SubOrderControlType::CT_BUY );
		sell_order_control_.set_control_type( SubOrderControlType::CT_SELL );

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
		if (order->remain_count != 0) return;
		if (order->position == SmPositionType::Buy)
			buy_order_control_.remove_order(order->order_price, order->order_no);
		else
			sell_order_control_.remove_order(order->order_price, order->order_no);
	}

	void OrderControl::on_order_accepted(std::shared_ptr<Order> order)
	{
		if (order->position == SmPositionType::Buy)
			buy_order_control_.add_order(order->order_price, order);
		else
			sell_order_control_.add_order(order->order_price, order);
	}

	void OrderControl::add_account_id(const int account_id)
	{
		account_id_set_.insert(account_id);
	}

	std::pair<int, int> OrderControl::get_order_count(const SmPositionType& position, const int price)
	{
		if (position == SmPositionType::Buy)
			return get_order_count(buy_order_control_, price);
		else
			return get_order_count(sell_order_control_, price);
	}

	std::pair<int, int> OrderControl::get_order_count(DarkHorse::SubOrderControl& order_control, const int price)
	{
		const std::map<int, std::shared_ptr<PriceOrderMap>>& order_map = order_control.get_order_map();
		auto it_price = order_map.find(price);
		if (it_price == order_map.end()) return std::make_pair(0, 0);
		const std::shared_ptr<DarkHorse::PriceOrderMap>& price_order_map = it_price->second;
		return std::make_pair(it_price->first, price_order_map->count());
	}

	std::shared_ptr<PriceOrderMap> OrderControl::get_order_map(const SmPositionType& position, const int price)
	{
		if (position == SmPositionType::Buy)
			return get_order_map(buy_order_control_, price);
		else
			return get_order_map(sell_order_control_, price);
	}

	std::shared_ptr<DarkHorse::PriceOrderMap> OrderControl::get_order_map(DarkHorse::SubOrderControl& order_control, const int price)
	{
		const std::map<int, std::shared_ptr<PriceOrderMap>>& order_map = order_control.get_order_map();
		auto it_price = order_map.find(price);
		if (it_price == order_map.end()) return nullptr;
		return it_price->second;
	}

}