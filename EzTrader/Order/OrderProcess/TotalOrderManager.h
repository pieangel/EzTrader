#pragma once
#include <string>
#include "../../Json/json.hpp"
namespace DarkHorse {
	using order_event = nlohmann::json;
class TotalOrderManager
{
public:
	void on_order_event(const order_event& order_info);
};
}
