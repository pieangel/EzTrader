#include "stdafx.h"
#include "StopOrderControl.h"
#include "../Order/OrderRequest/OrderRequest.h"
#include "../Global/SmTotalManager.h"
#include "../Order/OrderRequest/OrderRequestManager.h"
#include "../Order/Order.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../Util/IdGenerator.h"
#include "../Account/SmAccount.h"
#include "../Symbol/SmSymbol.h"


namespace DarkHorse {

void PriceOrderRequestMap::add_order_request(order_request_p order_request)
{
	if (order_request->order_price != price_) return;
	order_request_map_[order_request->request_id] = order_request;
	count_ = order_request_map_.size();
}

void PriceOrderRequestMap::remove_order_request(const int request_id)
{
	auto it = order_request_map_.find(request_id);
	if (it == order_request_map_.end()) return;
	order_request_map_.erase(it);
	count_ = order_request_map_.size();
}
StopOrderControl::StopOrderControl()
	: id_(IdGenerator::get_id())
{
	mainApp.event_hub()->subscribe_stop_order_event_handler
	(
		id_,
		std::bind(&StopOrderControl::on_cut_stop_order_request, this, std::placeholders::_1)
	);

	mainApp.event_hub()->subscribe_quote_event_handler
	(
		id_,
		std::bind(&StopOrderControl::update_quote, this, std::placeholders::_1)
	);
}
StopOrderControl::~StopOrderControl()
{
	mainApp.event_hub()->unsubscribe_stop_order_event_handler(id_);
	mainApp.event_hub()->unsubscribe_quote_event_handler(id_);
}
order_request_p StopOrderControl::make_profit_cut_order_request(order_p order)
{
	if (!order) return nullptr;
	const auto symbol = mainApp.SymMgr()->FindSymbol(order->symbol_code);
	if (!symbol) return nullptr;
	const auto account = mainApp.AcntMgr()->FindAccount(order->account_no);
	if (!account) return nullptr;

	auto old_req = mainApp.order_request_manager()->find_order_request(order->order_request_id);
	if (!old_req || old_req->cut_mode == SmCutMode::None) return nullptr;
	const int int_tick_size = static_cast<int>(symbol->TickSize() * pow(10, symbol->decimal()));
	int profit_cut_price = order->filled_price;
	std::shared_ptr<OrderRequest> profit_order_req = nullptr;
	if (order->position == SmPositionType::Buy) {
		profit_cut_price += old_req->profit_cut_tick * int_tick_size;
		profit_order_req = OrderRequestManager::make_default_sell_order_request(
			account->No(), 
			account->Pwd(), 
			symbol->SymbolCode(), 
			profit_cut_price);
	}
	else {
		profit_cut_price -= old_req->profit_cut_tick * int_tick_size;
		profit_order_req = OrderRequestManager::make_default_buy_order_request(
			account->No(), 
			account->Pwd(), 
			symbol->SymbolCode(), 
			profit_cut_price);
	}
	profit_order_req->cut_mode = SmCutMode::None;
	profit_order_req->cut_slip = old_req->cut_slip;
	profit_order_req->profit_cut_tick = old_req->profit_cut_tick;
	profit_order_req->order_amount = abs(order->unsettled_count);
	return profit_order_req;
}

order_request_p StopOrderControl::make_loss_cut_order_request(order_p order)
{
	if (!order) return nullptr;
	const auto symbol = mainApp.SymMgr()->FindSymbol(order->symbol_code);
	if (!symbol) return nullptr;
	const auto account = mainApp.AcntMgr()->FindAccount(order->account_no);
	if (!account) return nullptr;

	auto old_req = mainApp.order_request_manager()->find_order_request(order->order_request_id);
	if (!old_req || old_req->cut_mode == SmCutMode::None) return nullptr;
	const int int_tick_size = static_cast<int>(symbol->TickSize() * pow(10, symbol->decimal()));
	int loss_cut_price = order->filled_price;
	std::shared_ptr<OrderRequest> loss_order_req = nullptr;
	if (order->position == SmPositionType::Buy) {
		loss_cut_price -= old_req->loss_cut_tick * int_tick_size;
		loss_order_req = OrderRequestManager::make_default_sell_order_request(
			account->No(), 
			account->Pwd(), 
			symbol->SymbolCode(), 
			loss_cut_price);
	}
	else {
		loss_cut_price += old_req->loss_cut_tick * int_tick_size;
		loss_order_req = OrderRequestManager::make_default_buy_order_request(
			account->No(), 
			account->Pwd(), 
			symbol->SymbolCode(), 
			loss_cut_price);
	}
	loss_order_req->cut_mode = SmCutMode::None;
	loss_order_req->cut_slip = old_req->cut_slip;
	loss_order_req->loss_cut_tick = old_req->loss_cut_tick;
	loss_order_req->order_amount = abs(order->unsettled_count);
	return loss_order_req;
}

void StopOrderControl::update_quote(std::shared_ptr<SmQuote> quote)
{
	check_stop_order_request(quote);
	if (event_handler_) event_handler_();
}

void StopOrderControl::check_stop_order_request(std::shared_ptr<SmQuote> quote)
{
	;
}

void StopOrderControl::add_stop_order_request
(
	account_p account,
	symbol_p symbol,
	const DarkHorse::SmPositionType& type,
	const int price,
	const int order_amount,
	const int cut_slip
)
{
	std::shared_ptr<OrderRequest> order_req = nullptr;
	if (type == SmPositionType::Sell) 
		order_req = OrderRequestManager::make_default_sell_order_request(
			account->No(), 
			account->Pwd(), 
			symbol->SymbolCode(), 
			price, 
			order_amount * account->SeungSu());
	else 
		order_req = OrderRequestManager::make_default_buy_order_request(
			account->No(), 
			account->Pwd(), 
			symbol->SymbolCode(), 
			price, 
			order_amount * account->SeungSu());
	order_req->cut_slip = cut_slip;
}

void StopOrderControl::on_cut_stop_order_request(order_p order)
{
	if (!order) return;
	if (order->order_context.order_control_id != order_control_id_) return;

	std::lock_guard<std::mutex> lock(mutex_);
	auto old_order_req = mainApp.order_request_manager()->find_order_request(order->order_request_id);
	if (!old_order_req || old_order_req->cut_mode == SmCutMode::None) return;
	if (old_order_req->cut_mode == SmCutMode::ProfitCut) {
		auto profit_order_req = make_profit_cut_order_request(order);
		add_order_request(profit_order_req->order_price, profit_order_req);
	}
	else if (old_order_req->cut_mode == SmCutMode::LossCut) {
		auto loss_order_req = make_loss_cut_order_request(order);
		add_order_request(loss_order_req->order_price, loss_order_req);
	}
	else if (old_order_req->cut_mode == SmCutMode::BothCut) {
		auto profit_order_req = make_profit_cut_order_request(order);
		auto loss_order_req = make_loss_cut_order_request(order);
		profit_order_req->counter_request_id = loss_order_req->request_id;
		loss_order_req->counter_request_id = profit_order_req->request_id;
		add_order_request(profit_order_req->order_price, profit_order_req);
		add_order_request(loss_order_req->order_price, loss_order_req);
	}

	if (event_handler_) event_handler_();
}

void StopOrderControl::add_order_request(const int order_price, order_request_p order_request)
{
	auto it = order_req_map_.find(order_price);
	if (it == order_req_map_.end()) {
		price_order_request_map_p price_order_map = std::make_shared<PriceOrderRequestMap>();;
		price_order_map->set_price(order_price);
		price_order_map->add_order_request(order_request);
		order_req_map_[order_price] = price_order_map;
	}
	else {
		price_order_request_map_p price_order_map = it->second;
		price_order_map->add_order_request(order_request);
	}
	calculate_total_count();
}

void StopOrderControl::remove_order_request(const int order_price, const int& request_id)
{
	auto it = order_req_map_.find(order_price);
	if (it == order_req_map_.end()) return;
	price_order_request_map_p price_order_map = it->second;
	price_order_map->remove_order_request(request_id);
	calculate_total_count();
}

void StopOrderControl::calculate_total_count()
{
	total_count_ = 0;
	for (auto it : order_req_map_)
		total_count_ += it.second->count();
}

}