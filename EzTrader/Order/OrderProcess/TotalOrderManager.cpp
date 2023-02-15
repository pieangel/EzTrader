#include "stdafx.h"
#include "TotalOrderManager.h"
#include "AccountOrderManager.h"
#include "../OrderRequest/OrderRequestManager.h"
#include "../OrderRequest/OrderRequest.h"
#include "../../Global/SmTotalManager.h"
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
}