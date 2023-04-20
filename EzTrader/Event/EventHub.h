#pragma once
/*
#include <functional>
using namespace std::placeholders;

void function1(std::function<void(int, int)> fun)
{
	fun(1, 1);
}

int main (int argc, const char * argv[])
{
   ...

   aClass a;
   auto fp = std::bind(&aClass::test, a, _1, _2);
   function1(fp);

   return 0;
}
*/
#include "eventpp/callbacklist.h"
#include "eventpp/eventdispatcher.h"
#include <functional>
#include <map>
#include <vector>
#include "../Order/OrderUi/ControlId.h"
#include "../Json/json.hpp"
#include "../Quote/SmQuote.h"

namespace DarkHorse {

	class AccountOrderControl;
	class AccountPositionControl;
	class AccountProfitLossControl;
	class AssetControl;
	class BuyHogaControl;
	class BuyOrderControl;
	class BuyStopOrderControl;
	class FavoriteSymbolControl;
	class FutureCloseControl;
	class FuturePositionControl;
	class FutureExpectedControl;
	class OptionCloseControl;
	class OptionPositionControl;
	class OptionExpectedControl;
	class PriceRangeControl;
	class SellHogaControl;
	class SellOrderControl;
	class SellStopOrderControl;
	class SymbolInfoControl;
	class SymbolPositionControl;
	class SymbolProfitLossControl;
	class SymbolTickControl;
	class TotalHogaInfoControl;
	struct SmHoga;
	struct SmQuote;
	struct SmTick;
	class SmSymbol;
	struct Position;


	using SymbolMasterCBL = eventpp::CallbackList<void(std::shared_ptr<SmSymbol> quote)>;
	using SymbolMasterCBH = eventpp::CallbackList<void(std::shared_ptr<SmSymbol> quote)>::Handle;

	using TickCBL = eventpp::CallbackList<void(SmTick tick)>;
	using TickCBH = eventpp::CallbackList<void(SmTick tick)>::Handle;

	using RemoveOrderCBL = eventpp::CallbackList<void(const std::string&)>;
	using RemoveOrderCBH = eventpp::CallbackList<void(const std::string&)>::Handle;

	using AddOrderCBL = eventpp::CallbackList<void(const std::string&)>;
	using AddOrderCBH = eventpp::CallbackList<void(const std::string&)>::Handle;

	using HogaCBL = eventpp::CallbackList<void(std::shared_ptr<SmHoga> hoga)>;
	using HogaCBH = eventpp::CallbackList<void(std::shared_ptr<SmHoga> hoga)>::Handle;

	using QuoteCBL = eventpp::CallbackList<void(std::shared_ptr<SmQuote> quote)>;
	using QuoteCBH = eventpp::CallbackList<void(std::shared_ptr<SmQuote> quote)>::Handle;

	using ExpectedCBL = eventpp::CallbackList<void(std::shared_ptr<SmQuote> quote)>;
	using ExpectedCBH = eventpp::CallbackList<void(std::shared_ptr<SmQuote> quote)>::Handle;

	using PositionCBL = eventpp::CallbackList<void(std::shared_ptr<Position> position)>;
	using PositionCBH = eventpp::CallbackList<void(std::shared_ptr<Position> position)>::Handle;

	using SymbolCBL = eventpp::CallbackList<void(std::shared_ptr<SmSymbol> symbol)>;

class EventHub
{
public:

	void subscribe_tick_event_handler(const int tick_control_id, std::function<void(SmTick tick)>&& handler)
	{
		TickCBH handle = tick_cb_list_.append(handler);
		tick_cb_handle_map_[tick_control_id] = handle;
	}
	void unsubscribe_tick_event_handler(const int tick_control_id)
	{
		auto found = tick_cb_handle_map_.find(tick_control_id);
		if (found == tick_cb_handle_map_.end()) return;
		tick_cb_list_.remove(found->second);
	}
	void process_tick_event(SmTick tick)
	{
		tick_cb_list_(tick);
	}

	void subscribe_remove_order_event_handler(const int order_source_id, std::function<void(const std::string&)>&& handler)
	{
		RemoveOrderCBH handle = remove_order_callback_list_.append(handler);
		remove_order_callback_handle_map_[order_source_id] = handle;
	}
	void unsubscribe_remove_order_event_handler(const int order_source_id)
	{
		auto found = remove_order_callback_handle_map_.find(order_source_id);
		if (found == remove_order_callback_handle_map_.end()) return;
		remove_order_callback_list_.remove(found->second);
	}
	void process_remove_order_event(const std::string& order_no)
	{
		remove_order_callback_list_(order_no);
	}

	void subscribe_hoga_event_handler(const int hoga_control_id, std::function<void(std::shared_ptr<SmHoga> hoga)>&& handler)
	{
		HogaCBH handle = hoga_cb_list_.append(handler);
		hoga_cb_handle_map_[hoga_control_id] = handle;
	}
	void unsubscribe_hoga_event_handler(const int hoga_control_id)
	{
		auto found = hoga_cb_handle_map_.find(hoga_control_id);
		if (found == hoga_cb_handle_map_.end()) return;
		hoga_cb_list_.remove(found->second);
	}
	void process_hoga_event(std::shared_ptr<SmHoga> hoga)
	{
		hoga_cb_list_(hoga);
	}

	void subscribe_quote_event_handler(const int quote_control_id, std::function<void(std::shared_ptr<SmQuote> quote)>&& handler)
	{
		QuoteCBH handle = quote_cb_list_.append(handler);
		quote_cb_handle_map_[quote_control_id] = handle;
	}
	void unsubscribe_quote_event_handler(const int quote_control_id)
	{
		auto found = quote_cb_handle_map_.find(quote_control_id);
		if (found == quote_cb_handle_map_.end()) return;
		quote_cb_list_.remove(found->second);
	}
	void process_quote_event(std::shared_ptr<SmQuote> quote)
	{
		quote_cb_list_(quote);
	}

	void subscribe_expected_event_handler(const int expected_control_id, std::function<void(std::shared_ptr<SmQuote> quote)>&& handler)
	{
		ExpectedCBH handle = expected_cb_list_.append(handler);
		expected_cb_handle_map_[expected_control_id] = handle;
	}
	void unsubscribe_expected_event_handler(const int expected_control_id)
	{
		auto found = expected_cb_handle_map_.find(expected_control_id);
		if (found == expected_cb_handle_map_.end()) return;
		expected_cb_list_.remove(found->second);
	}

	void process_expected_event(std::shared_ptr<SmQuote> quote)
	{
		expected_cb_list_(quote);
	}

	void add_symbol_event_handler(std::function<void(std::shared_ptr<SmSymbol> symbol)>&& handler)
	{
		symbol_cb_list_.append(handler);
	}
	void process_symbol_event(std::shared_ptr<SmSymbol> symbol)
	{
		symbol_cb_list_(symbol);
	}


	void subscribe_symbol_master_event_handler(const int symbol_master_control_id, std::function<void(std::shared_ptr<SmSymbol> symbol)>&& handler)
	{
		SymbolMasterCBH handle = symbol_master_cb_list_.append(handler);
		symbol_master_cb_handle_map_[symbol_master_control_id] = handle;
	}
	void unsubscribe_symbol_master_event_handler(const int symbol_master_control_id)
	{
		auto found = symbol_master_cb_handle_map_.find(symbol_master_control_id);
		if (found == symbol_master_cb_handle_map_.end()) return;
		symbol_master_cb_list_.remove(found->second);
	}

	void process_symbol_master_event(std::shared_ptr<SmSymbol> symbol)
	{
		symbol_master_cb_list_(symbol);
	}

	void subscribe_position_event_handler(const int position_control_id, std::function<void(std::shared_ptr<Position> position)>&& handler)
	{
		PositionCBH handle = position_cb_list_.append(handler);
		position_cb_handle_map_[position_control_id] = handle;
	}
	void unsubscribe_position_event_handler(const int position_control_id)
	{
		auto found = position_cb_handle_map_.find(position_control_id);
		if (found == position_cb_handle_map_.end()) return;
		position_cb_list_.remove(found->second);
	}

	void process_position_event(std::shared_ptr<Position> position)
	{
		position_cb_list_(position);
	}

private:
	RemoveOrderCBL remove_order_callback_list_;
	std::map<int, RemoveOrderCBH> remove_order_callback_handle_map_;
	HogaCBL hoga_cb_list_;
	std::map<int, HogaCBH> hoga_cb_handle_map_;

	QuoteCBL quote_cb_list_;
	std::map<int, QuoteCBH> quote_cb_handle_map_;

	TickCBL tick_cb_list_;
	std::map<int, TickCBH> tick_cb_handle_map_;

	SymbolCBL symbol_cb_list_;

	ExpectedCBL expected_cb_list_;
	std::map<int, ExpectedCBH> expected_cb_handle_map_;

	SymbolMasterCBL symbol_master_cb_list_;
	std::map<int, SymbolMasterCBH> symbol_master_cb_handle_map_;

	PositionCBL position_cb_list_;
	std::map<int, PositionCBH> position_cb_handle_map_;
};
}

