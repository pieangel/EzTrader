#pragma once
#include "../../Json/json.hpp"
#include "../../Common/BlockingCollection.h"
using namespace code_machina;
namespace DarkHorse {
constexpr int BulkOrderProcessSize = 500;
class OrderProcessor
{
	using order_info = nlohmann::json;
private:
	BlockingCollection<order_info> order_info_q;
	bool handle_order_info(const std::array<order_info, BulkOrderProcessSize>& arr, int taken);
	bool handle_order_info(const order_info& order_info_item);
	int get_order_request_id(const std::string& custom_info);
};
}

