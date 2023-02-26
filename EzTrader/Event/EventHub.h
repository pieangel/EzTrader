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
#include <functional>
#include <map>
#include "../Order/OrderUi/ControlId.h"
#include "../Json/json.hpp"

namespace DarkHorse {
	using RemoveOrderCBL = eventpp::CallbackList<void(const std::string&)>;
	using RemoveOrderCBH = eventpp::CallbackList<void(const std::string&)>::Handle;

	using AddOrderCBL = eventpp::CallbackList<void(const std::string&)>;
	using AddOrderCBH = eventpp::CallbackList<void(const std::string&)>::Handle;

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
class EventHub
{
public:
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
private:
	RemoveOrderCBL remove_order_callback_list_;
	std::map<int, RemoveOrderCBH> remove_order_callback_handle_map_;
};
}

