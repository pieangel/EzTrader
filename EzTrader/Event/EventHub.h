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
#include "eventpp/eventdispatcher.h"
#include "eventpp/eventqueue.h"
#include <functional>
#include <map>
#include "../Order/OrderUi/ControlId.h"

namespace DarkHorse {
	using event_dispatcher = eventpp::EventDispatcher<int, void()>;
class EventHub
{
public:
	const event_dispatcher& sample_dispatcher() const {
		return sample_dispatcher_;
	}
	void push_remove_order_event(const int order_source_id, const std::string& order_no)
	{
		remove_order_event_queue_.enqueue(order_source_id, order_no);
	}
	void subscribe_remove_order_event_handler(const int order_source_id, std::function<void(const std::string&)>&& handler)
	{
		remove_order_event_queue_.appendListener(order_source_id, handler);
	}
	void process_remove_order_event()
	{
		remove_order_event_queue_.process();
	}
private:
	event_dispatcher sample_dispatcher_;
	eventpp::EventQueue<int, void(const std::string& order_no)> remove_order_event_queue_;
};
}

