#include "stdafx.h"
#include <memory>
#include "OrderProcessor.h"
#include "../../Log/MyLogger.h"
#include "../../Global/SmTotalManager.h"
#include "TotalOrderManager.h"
namespace DarkHorse {
using total_order_manager_p = std::shared_ptr<TotalOrderManager>;
bool OrderProcessor::handle_order_info(const std::array<order_info, BulkOrderProcessSize>& arr, int taken)
{
	if (taken == 0) return true;
	for (const auto& order_info_item : arr) {
		handle_order_info(order_info_item);
	}
	return true;
}

bool OrderProcessor::handle_order_info(const order_info& order_info_item)
{
	try {
		total_order_manager_p total_order_manager = mainApp.total_order_manager();
		total_order_manager->on_order_event(order_info_item);
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		return false;
	}
	return true;
}

int OrderProcessor::get_order_request_id(const std::string& custom_info)
{
	if (custom_info.length() == 0) return -1;
	try {
		const int zero_pos = custom_info.find_first_of('0');
		const int order_request_id_pos = custom_info.find_first_of('k');
		const std::string order_request_id_t = custom_info.substr(zero_pos, order_request_id_pos - zero_pos);
		return std::stoi(order_request_id_t);
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
	return -1;
}

}
