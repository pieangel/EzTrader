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
namespace DarkHorse {
	using event_dispatcher = eventpp::EventDispatcher<int, void()>;
class EventHub
{
public:
	const event_dispatcher& sample_dispatcher() const {
		return sample_dispatcher_;
	}
private:
	event_dispatcher sample_dispatcher_;
};
}

