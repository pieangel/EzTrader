#include "stdafx.h"
#include "TotalOrderManager.h"
#include "AccountOrderManager.h"
#include "../OrderRequest/OrderRequestManager.h"
#include "../OrderRequest/OrderRequest.h"
#include "../../Global/SmTotalManager.h"
#include "../Order.h"
namespace DarkHorse 
{
using account_order_manager_p = std::shared_ptr<AccountOrderManager>;
void TotalOrderManager::on_order_event(const order_event& order_info)
{
	const std::string custom_info = order_info["custom_info"];
	order_request_p order_request = mainApp.order_request_manager()->find_order_request(custom_info);
}

account_order_manager_p TotalOrderManager::find_order_manager(const std::string& account_no)
{
	auto it = account_order_manager_map_.find(account_no);
	return it != account_order_manager_map_.end() ? it->second : nullptr;
}
account_order_manager_p TotalOrderManager::create_order_manager(const std::string& account_no)
{
	return std::make_shared<AccountOrderManager>();
}
account_order_manager_p TotalOrderManager::get_order_manager(const std::string& account_no)
{
	account_order_manager_p order_manager = find_order_manager(account_no);
	if (order_manager) return order_manager;
	return create_order_manager(account_no);
}
order_p TotalOrderManager::make_inner_order(order_request_p order_request, const order_event& order_info)
{
	const std::string order_no = order_info["order_no"];
	const int order_event = order_info["order_event"];
	order_p order = mainApp.total_order_manager()->get_order(order_no);
	order->order_no = order_no;
	order->account_no = order_info["account_no"];
	order->symbol_code = order_info["symbol_code"];
	// 주문 유형 - 매수 / 매도
	const std::string position_type = order_info["position_type"];
	if (position_type.compare("1") == 0) order->position_type = SmPositionType::Buy;
	else if (position_type.compare("2") == 0) order->position_type = SmPositionType::Sell;
	order->order_price = order_info["order_price"];
	order->order_amount = order_info["order_amount"];
	order->original_order_no = order_info["original_order_no"];
	order->first_order_no = order_info["first_order_no"];
	order->order_time = order_info["order_time"];

	// 주문 타입 : 신규/정정/취소
	const std::string order_type = order_info["order_type"];
	if (order_type.compare("1") == 0) order->order_type = SmOrderType::New;
	else if (order_type.compare("2") == 0) order->order_type = SmOrderType::Modify;
	else if (order_type.compare("3") == 0) order->order_type = SmOrderType::Cancel;

	return order;
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
	return std::make_shared<Order>();
}
}