#include "stdafx.h"
#include "OrderControl.h"
#include "../Order/Order.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Util/IdGenerator.h"
#include "../Event/EventHub.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Order/OrderProcess/TotalOrderManager.h"
#include "../Order/OrderProcess/AccountOrderManager.h"
#include "../Order/OrderProcess/SymbolOrderManager.h"
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

	void OrderControl::clear()
	{
		buy_order_control_.clear();
		sell_order_control_.clear();
	}

	void OrderControl::load_from_account(const std::string& account_no, const std::string& symbol_code)
	{
		clear();
		auto account_order_manager = mainApp.total_order_manager()->get_account_order_manager(account_no);
		auto symbol_order_manager = account_order_manager->get_symbol_order_manager(symbol_code);
		const std::map<std::string, order_p>& accepted_order_map = symbol_order_manager->get_accepted_order_map();
		for (auto it = accepted_order_map.begin(); it != accepted_order_map.end(); it++) {
			add_order(it->second);
		}

		if (event_handler_) event_handler_();
	}

	void OrderControl::set_symbol(std::shared_ptr<SmSymbol> symbol)
	{
		if (!symbol) return;
		symbol_ = symbol;
		if (!account_) return;
		load_from_account(account_->No(), symbol_->SymbolCode());
	}

	void OrderControl::set_account(std::shared_ptr<SmAccount> account)
	{
		if (!account) return;
		account_ = account;
		if (!symbol_) return;
		load_from_account(account_->No(), symbol_->SymbolCode());
	}

	void OrderControl::update_order(std::shared_ptr<Order> order, OrderEvent order_event)
	{
		if (!order || !symbol_ || !account_) return;
		if (symbol_->SymbolCode() != order->symbol_code) return;
		if (account_->is_subaccount()) {
			if (account_->No() != order->account_no) return;
		}
		else {
			if (!(account_->No() == order->account_no || account_->No() == order->order_context.parent_account_no))
				return;
		}

		if (order_event == OrderEvent::OE_Accepted)
			on_order_accepted(order);
		else if (order_event == OrderEvent::OE_Unfilled)
			on_order_unfilled(order);

		if (event_handler_) event_handler_();
	}
	void OrderControl::on_order_unfilled(std::shared_ptr<Order> order)
	{
		if (order->remain_count == 0)
			remove_order(order);
		else
			add_order(order);
	}

	void OrderControl::on_order_accepted(std::shared_ptr<Order> order)
	{
		add_order(order);
	}

	void OrderControl::add_account_id(const int account_id)
	{
		//account_id_set_.insert(account_id);
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

	void OrderControl::add_order(std::shared_ptr<Order> order)
	{
		if (order->position == SmPositionType::Buy)
			buy_order_control_.add_order(order->order_price, order);
		else
			sell_order_control_.add_order(order->order_price, order);
	}

	void OrderControl::remove_order(std::shared_ptr<Order> order)
	{
		if (order->position == SmPositionType::Buy)
			buy_order_control_.remove_order(order->order_price, order->order_no);
		else
			sell_order_control_.remove_order(order->order_price, order->order_no);
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