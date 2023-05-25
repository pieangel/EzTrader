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
#include "../Log/MyLogger.h"
namespace DarkHorse {

	AccountPositionManager::AccountPositionManager(const std::string& account_no)
		: account_no_(account_no)
	{
		id_ = IdGenerator::get_id();
	}

	AccountPositionManager::~AccountPositionManager()
	{
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
	position->account_no = account_no_;
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
	LOGINFO(CMyLogger::getInstance(), "position_count = [%d], filled_count = [%d], average_price = [%.2f], filled_price = [%.2f]", position->open_quantity, order->filled_count, position->average_price, order->filled_price);

	const int position_count = calculate_position_count(order, position);
	const int unsettled_count = calculate_unsettled_count(order, position);
	const double traded_profit_loss = calculate_traded_profit_loss(order, position, symbol->seung_su());
	const double average_price = calculate_average_price(order, position);

	LOGINFO(CMyLogger::getInstance(), "position_count = [%d], unsettled_count = [%d], trade_profit_loss = [%.2f], average_price = [%.2f]", position_count, unsettled_count, traded_profit_loss, average_price);
	
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
double AccountPositionManager::calculate_traded_profit_loss(order_p order, position_p position, const int& symbol_seungsu)
{
	const int traded_count = calculate_traded_count(order, position);
	const double price_gap = abs(position->average_price - order->filled_price);
	double trade_profit_loss = price_gap * traded_count * symbol_seungsu; // * symbol->SeungSu() ¹Ýµå½Ã Symbol ½Â¼ö¸¦ °öÇØÁà¾ß ÇÔ. 
	// decide it is a profit or a loss
	if (order->position == SmPositionType::Buy)
		if (order->filled_price < position->average_price)
			trade_profit_loss *= -1;
	else if (order->position == SmPositionType::Sell)
		if (order->filled_price > position->average_price)
			trade_profit_loss *= -1;
	return trade_profit_loss;
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