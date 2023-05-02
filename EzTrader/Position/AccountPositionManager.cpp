#include "stdafx.h"
#include "AccountPositionManager.h"
#include "Position.h"
#include "../Order/Order.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Order/OrderRequest/OrderRequest.h"
#include "../Order/OrderRequest/OrderRequestManager.h"
#include "../Order/SmOrderConst.h"
#include <functional>
#include "../Util/IdGenerator.h"
namespace DarkHorse {

	AccountPositionManager::AccountPositionManager()
	{
		id_ = IdGenerator::get_id();
		/*
		mainApp.event_hub()->subscribe_quote_event_handler
		(
			id_,
			std::bind(&AccountPositionManager::update_profit_loss, this, std::placeholders::_1)
		);
		*/
	}

	AccountPositionManager::~AccountPositionManager()
	{
		//mainApp.event_hub()->unsubscribe_position_event_handler(id_);
	}

position_p AccountPositionManager::get_position(const std::string& symbol_code)
{
	position_p position = find_position(symbol_code);
	if (position) return position;
	return create_position(symbol_code);
}
position_p AccountPositionManager::find_position(const std::string& symbol_code)
{
	auto it = position_map_.find(symbol_code);
	return it != position_map_.end() ? it->second : nullptr;
}

position_p AccountPositionManager::create_position(const std::string& symbol_code)
{
	position_p position = std::make_shared<Position>();
	position->symbol_code = symbol_code;
	position_map_[symbol_code] = position;
	return position;
}

void AccountPositionManager::update_position(order_p order)
{
	if (!order) return;

	position_p position = get_position(order->symbol_code);
	position->account_no = order->account_no;
	set_symbol_id(position, order->symbol_code);
	set_account_id(position, order->account_no);
	auto symbol = mainApp.SymMgr()->FindSymbol(order->symbol_code);
	if (!symbol) return;
	const int position_count = calculate_position_count(order, position);
	const int unsettled_count = calculate_unsettled_count(order, position);
	const double traded_profit_loss = calculate_traded_profit_loss(order, position);
	const double average_price = calculate_average_price(order, position);
	
	position->trade_profit_loss += traded_profit_loss;
	position->open_quantity = position_count;
	position->average_price = average_price;
	update_open_profit_loss(position);
	// update the involved order.
	order->unsettled_count = unsettled_count;
	if (order->unsettled_count == 0)
		order->order_state = SmOrderState::Settled;

	mainApp.event_hub()->process_position_event(position);
}

void AccountPositionManager::update_profit_loss(std::shared_ptr<SmQuote> quote)
{
	if (!quote) return;
	auto symbol = mainApp.SymMgr()->FindSymbol(quote->symbol_code);
	if (!symbol) return;
	position_p position = get_position(quote->symbol_code);
	if (!position) return;
	double open_profit_loss = 0;
	open_profit_loss = position->open_quantity * (quote->close - position->average_price) * symbol->seung_su();
	open_profit_loss = open_profit_loss / pow(10, symbol->decimal());
	position->open_profit_loss = open_profit_loss;

	mainApp.event_hub()->process_position_event(position);
}

order_request_p AccountPositionManager::make_loss_cut_stop_order(order_request_p prev_order_req, order_p order)
{
	return nullptr;
}
order_request_p AccountPositionManager::make_profit_cut_stop_order(order_request_p prev_order_req, order_p order)
{
	return nullptr;
}

void AccountPositionManager::add_cut_stop_order(order_p order)
{
	auto old_req = mainApp.order_request_manager()->find_order_request(order->order_request_id);
	if (!old_req || old_req->cut_mode == SmCutMode::None) return;
	const auto symbol = mainApp.SymMgr()->FindSymbol(order->symbol_code);
	if (!symbol) return;
	const auto account = mainApp.AcntMgr()->FindAccount(order->account_no);
	if (!account) return;

	const int int_tick_size = static_cast<int>(symbol->TickSize() * pow(10, symbol->decimal()));
	int loss_cut_price = 0, profit_cut_price = 0;
	if (order->position == SmPositionType::Buy) {
		profit_cut_price = order->filled_price + old_req->profit_cut_tick * int_tick_size;
		loss_cut_price = order->filled_price - old_req->loss_cut_tick * int_tick_size;
		if (old_req->cut_mode == SmCutMode::ProfitCut) {
			std::shared_ptr<OrderRequest> profit_order_req = OrderRequestManager::make_default_sell_order_request(account->No(), account->Pwd(), symbol->SymbolCode(), profit_cut_price);
			profit_order_req->cut_mode = SmCutMode::None;
			profit_order_req->cut_slip = old_req->cut_slip;
			profit_order_req->profit_cut_tick = old_req->profit_cut_tick;
			profit_order_req->order_amount = abs(order->unsettled_count);
			//_SellStopOrderMgr->AddOrderRequest(profit_order_req);
		}
		else if (old_req->cut_mode == SmCutMode::LossCut) {
			std::shared_ptr<OrderRequest> loss_order_req = OrderRequestManager::make_default_sell_order_request(account->No(), account->Pwd(), symbol->SymbolCode(), loss_cut_price);
			loss_order_req->cut_mode = SmCutMode::None;
			loss_order_req->cut_slip = old_req->cut_slip;
			loss_order_req->loss_cut_tick = old_req->loss_cut_tick;
			loss_order_req->order_amount = abs(order->unsettled_count);
			//_SellStopOrderMgr->AddOrderRequest(loss_order_req);
		}
		else if (old_req->cut_mode == SmCutMode::BothCut) {
			std::shared_ptr<OrderRequest> profit_order_req = OrderRequestManager::make_default_sell_order_request(account->No(), account->Pwd(), symbol->SymbolCode(), profit_cut_price);
			std::shared_ptr<OrderRequest> loss_order_req = OrderRequestManager::make_default_sell_order_request(account->No(), account->Pwd(), symbol->SymbolCode(), loss_cut_price);
			profit_order_req->cut_mode = SmCutMode::None;
			profit_order_req->cut_slip = old_req->cut_slip;
			profit_order_req->profit_cut_tick = old_req->profit_cut_tick;
			profit_order_req->order_amount = abs(order->unsettled_count);

			loss_order_req->cut_mode = SmCutMode::None;
			loss_order_req->cut_slip = old_req->cut_slip;
			loss_order_req->loss_cut_tick = old_req->loss_cut_tick;
			loss_order_req->order_amount = abs(order->unsettled_count);

			profit_order_req->counter_request_id = loss_order_req->request_id;
			loss_order_req->counter_request_id = profit_order_req->request_id;
			//_SellStopOrderMgr->AddOrderRequest(profit_order_req);
			//_SellStopOrderMgr->AddOrderRequest(loss_order_req);
		}
	}
	/*
	else if (order->PositionType == SmPositionType::Sell) {
		profit_cut_price = order->FilledPrice - old_req->ProfitCutTick * int_tick_size;
		loss_cut_price = order->FilledPrice + old_req->LossCutTick * int_tick_size;
		if (old_req->CutMode == 1) {
			std::shared_ptr<SmOrderRequest> profit_order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), profit_cut_price);
			profit_order_req->CutMode = 0;
			profit_order_req->CutSlip = old_req->CutSlip;
			profit_order_req->OrderAmount = abs(order->FilledCount);
			profit_order_req->ProfitCutTick = old_req->ProfitCutTick;
			_BuyStopOrderMgr->AddOrderRequest(profit_order_req);
		}
		else if (old_req->CutMode == 2) {
			std::shared_ptr<SmOrderRequest> loss_order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), loss_cut_price);
			loss_order_req->CutMode = 0;
			loss_order_req->CutSlip = old_req->CutSlip;
			loss_order_req->LossCutTick = old_req->LossCutTick;
			loss_order_req->OrderAmount = abs(order->FilledCount);
			_BuyStopOrderMgr->AddOrderRequest(loss_order_req);
		}
		else if (old_req->CutMode == 3) {
			std::shared_ptr<SmOrderRequest> profit_order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), profit_cut_price);
			std::shared_ptr<SmOrderRequest> loss_order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), loss_cut_price);
			profit_order_req->CutMode = 0;
			profit_order_req->CutSlip = old_req->CutSlip;
			profit_order_req->OrderAmount = abs(order->FilledCount);
			profit_order_req->ProfitCutTick = old_req->ProfitCutTick;
			loss_order_req->CutMode = 0;
			loss_order_req->CutSlip = old_req->CutSlip;
			loss_order_req->OrderAmount = abs(order->FilledCount);
			loss_order_req->LossCutTick = old_req->LossCutTick;
			profit_order_req->CounterRequestId = loss_order_req->RequestId;
			loss_order_req->CounterRequestId = profit_order_req->RequestId;
			_BuyStopOrderMgr->AddOrderRequest(profit_order_req);
			_BuyStopOrderMgr->AddOrderRequest(loss_order_req);
		}
	}
	*/
}
void AccountPositionManager::set_symbol_id(position_p position, const std::string& symbol_code)
{
	if (!position || position->symbol_id != 0) return;

	auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (!symbol) return;
	position->symbol_id = symbol->Id();
}

void AccountPositionManager::set_account_id(position_p position, const std::string& account_no)
{
	if (!position || position->account_id != 0) return;

	auto account = mainApp.AcntMgr()->FindAccount(account_no);
	if (!account) return;

	position->account_id = account->id();
}

int AccountPositionManager::calculate_position_count(order_p order, position_p position)
{
	const int order_filled_sign = order->position == SmPositionType::Buy ? 1 : -1;
	const int signed_filled_count = order->filled_count * order_filled_sign;
	const int old_position_count = position->open_quantity;
	return signed_filled_count + old_position_count;
}
int AccountPositionManager::calculate_unsettled_count(order_p order, position_p position)
{
	if (abs(position->open_quantity) >= abs(order->filled_count)) return 0;
	return abs(order->filled_count) - abs(position->open_quantity);
}
int AccountPositionManager::calculate_traded_count(order_p order, position_p position)
{
	const int order_filled_sign = order->position == SmPositionType::Buy ? 1 : -1;
	const int signed_filled_count = order->filled_count * order_filled_sign;
	const int old_position_count = position->open_quantity;
	if (old_position_count * signed_filled_count >= 0) return 0;
	return min(abs(old_position_count), abs(signed_filled_count));
}
double AccountPositionManager::calculate_traded_profit_loss(order_p order, position_p position)
{
	const int traded_count = calculate_traded_count(order, position);
	const double price_gap = abs(position->average_price - order->filled_price);
	return price_gap * traded_count; // * symbol->SeungSu() ¹Ýµå½Ã Symbol ½Â¼ö¸¦ °öÇØÁà¾ß ÇÔ. 
}
double AccountPositionManager::calculate_average_price(order_p order, position_p position)
{
	const double position_price = position->average_price * abs(position->open_quantity);
	const double filled_price = order->filled_price * order->filled_count;

	return (position_price + filled_price) / (abs(position->open_quantity) + order->filled_count);
}
void AccountPositionManager::update_open_profit_loss(position_p position)
{
	if (!position) return;
	std::shared_ptr<SmQuote> quote = mainApp.QuoteMgr()->get_quote(position->symbol_code);
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(position->symbol_code);
	if (!quote || !symbol) return;

	double open_profit_loss = 0;
	open_profit_loss = position->open_quantity * (quote->close - position->average_price) * symbol->seung_su();
	open_profit_loss = open_profit_loss / pow(10, symbol->decimal());
	position->open_profit_loss = open_profit_loss;
}

}