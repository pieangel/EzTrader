#include "stdafx.h"
#include "TotalOrderManager.h"
#include "AccountOrderManager.h"
#include "../../Log/MyLogger.h"
#include "../OrderRequest/OrderRequestManager.h"
#include "../OrderRequest/OrderRequest.h"
#include "../../Global/SmTotalManager.h"
#include "../Order.h"
namespace DarkHorse 
{
using account_order_manager_p = std::shared_ptr<AccountOrderManager>;
void TotalOrderManager::on_order_event(const order_event& order_info)
{
	order_p order = make_order(order_info);
	if (!order) return;
	const std::string& custom_info = order_info["custom_info"];
	set_order_request_info(custom_info, order);
	const OrderEvent order_event = order_info["order_event"];
	write_order_history(order_event, order);
	dispatch_order(order_event, order);
}

void TotalOrderManager::on_order_event(order_event&& order_info)
{
	order_p order = make_order(order_info);
	if (!order) return;
	const std::string& custom_info = order_info["custom_info"];
	set_order_request_info(custom_info, order);
	const OrderEvent order_event = order_info["order_event"];
	write_order_history(order_event, order);
	dispatch_order(order_event, order);
}

void TotalOrderManager::dispatch_order(const OrderEvent order_event, order_p order)
{
	assert(order);
	account_order_manager_p order_manager = get_order_manager(order->account_no);
	order_manager->dispatch_order(order_event, order);
}
void TotalOrderManager::write_order_history(const OrderEvent order_event, order_p order)
{
	assert(order);
}

account_order_manager_p TotalOrderManager::find_order_manager(const std::string& account_no) 
{
	auto it = order_manager_map_.find(account_no);
	return it != order_manager_map_.end() ? it->second : nullptr;
}
account_order_manager_p TotalOrderManager::create_order_manager(const std::string& account_no)
{
	account_order_manager_p order_manager = std::make_shared<AccountOrderManager>();
	order_manager_map_[account_no] = order_manager;
	return order_manager;
}
account_order_manager_p TotalOrderManager::get_order_manager(const std::string& account_no) 
{
	account_order_manager_p order_manager = find_order_manager(account_no);
	if (order_manager) return order_manager;
	return create_order_manager(account_no);
}
order_p TotalOrderManager::make_order(const order_event& order_info)
{
	try {
		const std::string order_no = order_info["order_no"];
		const int order_event = order_info["order_event"];
		order_p order = mainApp.total_order_manager()->get_order(order_no);
		order->order_no = order_no;
		order->account_no = order_info["account_no"];
		order->symbol_code = order_info["symbol_code"];
		// 주문 유형 - 매수 / 매도
		const std::string position_type = order_info["position_type"];
		if (position_type.compare("1") == 0) order->position = SmPositionType::Buy;
		else if (position_type.compare("2") == 0) order->position = SmPositionType::Sell;

		switch (order_event) {
		case OrderEvent::DM_Accepted: { // 국내 접수확인
			order->order_price = order_info["order_price"];
			order->order_amount = order_info["order_amount"];
			order->order_time = order_info["order_time"];
		}
		break;
		case OrderEvent::AB_Accepted: { // 해외 접수 확인
			// 주문 타입 : 신규/정정/취소
			const std::string order_type = order_info["order_type"];
			if (order_type.compare("1") == 0) order->order_type = SmOrderType::New;
			else if (order_type.compare("2") == 0) order->order_type = SmOrderType::Modify;
			else if (order_type.compare("3") == 0) order->order_type = SmOrderType::Cancel;

			order->order_price = order_info["order_price"];
			order->order_amount = order_info["order_amount"];
			order->order_date = order_info["order_date"];
			order->order_time = order_info["order_time"];
		}
		break;
		case OrderEvent::DM_Unfilled: { // 국내 미체결
			order->original_order_no = order_info["original_order_no"];
			order->first_order_no = order_info["first_order_no"];

			order->remain_count = order_info["remain_count"];
			order->modified_count = order_info["modified_count"];
			order->cancelled_count = order_info["cancelled_count"];
			order->filled_count = order_info["filled_count"];
		}
		break;
		case OrderEvent::AB_Unfilled: { // 해외 미체결
			order->original_order_no = order_info["original_order_no"];
			order->first_order_no = order_info["first_order_no"];
			order->remain_count = order_info["remain_count"];
			order->filled_count = order_info["filled_count"];
		}
		break;
		case OrderEvent::DM_Filled: { // 국내 체결
			order->filled_price = order_info["filled_price"];
			order->filled_count = order_info["filled_count"];
			order->filled_time = order_info["filled_time"];
		}
		break;
		case OrderEvent::AB_Filled: { // 해외 체결
			order->filled_price = order_info["filled_price"];
			order->filled_count = order_info["filled_count"];
			order->filled_date = order_info["filled_date"];
			order->filled_time = order_info["filled_time"];
		}
		break;
		}

		return order;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return nullptr;
}

void TotalOrderManager::set_order_request_info(const std::string& custom_info, order_p order)
{
	if (!order) return;
	order_request_p order_request = mainApp.order_request_manager()->find_order_request(custom_info);
	if (!order_request) return;
	order->order_amount = order_request->order_amount;
	order->order_type = order_request->order_type;
	order->order_request_id = order_request->request_id;
}

order_p TotalOrderManager::get_order(const std::string& order_no)
{
	order_p order = find_order(order_no);
	if (order) return order;
	return create_order(order_no);
}
order_p TotalOrderManager::find_order(const std::string& order_no)
{
	auto found = order_map_.find(order_no);
	return (found != order_map_.end()) ? found->second : nullptr;
}
order_p TotalOrderManager::create_order(const std::string& order_no)
{
	order_p order = std::make_shared<Order>();
	order->order_no = order_no;
	order_map_[order_no] = order;
	return order;
}
}