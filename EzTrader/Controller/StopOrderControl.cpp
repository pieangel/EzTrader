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

order_request_p StopOrderControl::make_profit_cut_order_request(order_p order)
{
	std::shared_ptr<OrderRequest> profit_order_req = nullptr;
	if (!order) return nullptr;
	const auto symbol = mainApp.SymMgr()->FindSymbol(order->symbol_code);
	if (!symbol) return nullptr;
	const auto account = mainApp.AcntMgr()->FindAccount(order->account_no);
	if (!account) return nullptr;

	auto old_req = mainApp.order_request_manager()->find_order_request(order->order_request_id);
	if (!old_req || old_req->cut_mode == SmCutMode::None) return nullptr;
	const int int_tick_size = static_cast<int>(symbol->TickSize() * pow(10, symbol->decimal()));
	const int profit_cut_price = order->filled_price + old_req->profit_cut_tick * int_tick_size;
	const int loss_cut_price = order->filled_price - old_req->loss_cut_tick * int_tick_size;
	profit_order_req = OrderRequestManager::make_default_sell_order_request(account->No(), account->Pwd(), symbol->SymbolCode(), profit_cut_price);
	profit_order_req->cut_mode = SmCutMode::None;
	profit_order_req->cut_slip = old_req->cut_slip;
	profit_order_req->profit_cut_tick = old_req->profit_cut_tick;
	profit_order_req->order_amount = abs(order->unsettled_count);
	return profit_order_req;
}

order_request_p StopOrderControl::make_loss_cut_order_request(order_p order)
{
	std::shared_ptr<OrderRequest> profit_order_req = nullptr;
	if (!order) return nullptr;
	const auto symbol = mainApp.SymMgr()->FindSymbol(order->symbol_code);
	if (!symbol) return nullptr;
	const auto account = mainApp.AcntMgr()->FindAccount(order->account_no);
	if (!account) return nullptr;

	auto old_req = mainApp.order_request_manager()->find_order_request(order->order_request_id);
	if (!old_req || old_req->cut_mode == SmCutMode::None) return nullptr;
	const int int_tick_size = static_cast<int>(symbol->TickSize() * pow(10, symbol->decimal()));
	const int profit_cut_price = order->filled_price + old_req->profit_cut_tick * int_tick_size;
	const int loss_cut_price = order->filled_price - old_req->loss_cut_tick * int_tick_size;
	profit_order_req = OrderRequestManager::make_default_sell_order_request(account->No(), account->Pwd(), symbol->SymbolCode(), profit_cut_price);
	profit_order_req->cut_mode = SmCutMode::None;
	profit_order_req->cut_slip = old_req->cut_slip;
	profit_order_req->profit_cut_tick = old_req->profit_cut_tick;
	profit_order_req->order_amount = abs(order->unsettled_count);
	return profit_order_req;
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