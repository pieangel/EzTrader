#include "stdafx.h"
#include "OrderRequest.h"
#include "OrderRequestManager.h"
#include "../../Global/SmTotalManager.h"
#include "../../Client/ViStockClient.h"
#include "../../Global/SmTotalManager.h"
#include "../../Json/json.hpp"
#include "../../Global/SmTotalManager.h"
#include "../OrderProcess/OrderProcessor.h"
#include "../../Client/ViClient.h"
#include "../../Util/VtStringUtil.h"

namespace DarkHorse {
int OrderRequestManager::id_ = 0;
//using order_request_p = std::shared_ptr<OrderRequest>;
order_request_p OrderRequestManager::find_order_request(const int order_request_id)
{
	std::unique_lock<std::mutex> lock(order_request_map_mutex_);
	auto found = order_request_map.find(order_request_id);
	return (found != order_request_map.end()) ? found->second : nullptr;
}

void OrderRequestManager::add_order_request(order_request_p order_request)
{
	if (!order_request) return;
	// q 내부에 mutex 장치가 있음. 
	order_request_q.add(order_request);
	// 추후 재사용을 위해서 맵에 넣어 줌. 
	add_order_request_map(order_request);
}

void OrderRequestManager::remove_order_request(int order_request_id)
{
	std::unique_lock<std::mutex> lock(order_request_map_mutex_);
	auto found = order_request_map.find(order_request_id);
	if (found == order_request_map.end()) return;
	order_request_map.erase(found);
}

unsigned int OrderRequestManager::ThreadHandlerProc(void)
{
	while (true) {
		// 종료 신호를 보내면 루프를 나간다.
		if (isStop()) break;
		if (bulk_operation_) {
			std::array<order_request_p, BulkOrderRequestSize> order_request_arr;
			size_t taken{ 0 };
			auto status = order_request_q.try_take_bulk(order_request_arr.begin(), order_request_arr.size(), taken);
			if (status != BlockingCollectionStatus::Ok) continue;
			handle_order_request(order_request_arr, taken);
		}
		else {
			order_request_p order_request;
			auto status = order_request_q.try_take(order_request);
			if (status != BlockingCollectionStatus::Ok) continue;
			handle_order_request(order_request);
		}
	}

	return 1;
}

void OrderRequestManager::clear_order_requests() noexcept
{
	order_request_q.flush();
}

bool OrderRequestManager::handle_order_request(order_request_p order_request)
{
	if (!order_request) return false;

	if (simulation_) { handle_order_simulation(order_request); return true; }

	switch (order_request->order_type) {
	case SmOrderType::New: mainApp.Client()->NewOrder(order_request); break;
	case SmOrderType::Modify: mainApp.Client()->ChangeOrder(order_request); break;
	case SmOrderType::Cancel: mainApp.Client()->CancelOrder(order_request); break;
	}
	return true;
}

bool OrderRequestManager::handle_order_request(
	std::array<order_request_p, BulkOrderRequestSize>& arr, 
	int taken)
{
	if (taken == 0) return true;
	for (auto order_request : arr) {
		switch (order_request->order_type) {
		case SmOrderType::New: 
			mainApp.Client()->NewOrder(order_request); 
			break;
		case SmOrderType::Modify:
			mainApp.Client()->ChangeOrder(order_request);
			break;
		case SmOrderType::Cancel:
			mainApp.Client()->CancelOrder(order_request);
			break;
		}
	}
	return true;
}

bool OrderRequestManager::handle_order_simulation(order_request_p order_request)
{
	if (!order_request) return false;

	switch (order_request->order_type) {
	case SmOrderType::New: on_new_order(order_request); break;
	case SmOrderType::Modify: on_change_order(order_request); break;
	case SmOrderType::Cancel: on_cancel_order(order_request); break;
	}
	return true;
}

order_request_p OrderRequestManager::make_dummy_order_request()
{
	return std::make_shared<OrderRequest>();
}

void OrderRequestManager::add_order_request_map(order_request_p order_request)
{
	if (!order_request) return;
	std::unique_lock<std::mutex> lock(order_request_map_mutex_);
	order_request_map[order_request->request_id] = order_request;
}

void OrderRequestManager::dm_make_accepted_order_event(order_request_p order_request)
{
	nlohmann::json order_info;
	order_info["order_event"] = OrderEvent::DM_Accepted;
	order_info["account_no"] = order_request->account_no;
	order_info["order_no"] =  std::to_string(get_order_no());
	order_info["symbol_code"] = order_request->symbol_code;
	order_info["order_price"] = order_request->order_price;
	order_info["order_amount"] = order_request->order_amount;
	order_info["positon_type"] = order_request->position_type == SmPositionType::Buy ? "1" : "2";
	//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
	order_info["original_order_no"] = "";
	order_info["first_order_no"] = "";
	//order_info["order_date"] = static_cast<const char*>(strOrderDate.Trim());
	order_info["order_time"] = "13:05:05";
	order_info["order_type"] = "20230423";
	//order_info["filled_price"] = static_cast<const char*>(strFilledPrice.Trim());
	//order_info["filled_amount"] = static_cast<const char*>(strFilledAmount.Trim());

	//order_info["filled_date"] = static_cast<const char*>(strFilledDate.Trim());
	//order_info["filled_time"] = static_cast<const char*>(strFilledTime.Trim());
	std::string user_defined;
	ViClient::make_custom_order_info(order_request, user_defined);
	std::string temp = VtStringUtil::PadRight(user_defined, '0', 60);
	order_info["custom_info"] = temp;

	mainApp.order_processor()->add_order_event(std::move(order_info));
}

void OrderRequestManager::dm_make_unfilled_order_event(order_request_p order_request)
{

}

void OrderRequestManager::dm_make_filled_order_event(order_request_p order_request)
{

}

void OrderRequestManager::ab_make_accepted_order_event(order_request_p order_request)
{

}

void OrderRequestManager::ab_make_unfilled_order_event(order_request_p order_request)
{

}

void OrderRequestManager::ab_make_filled_order_event(order_request_p order_request)
{

}

void OrderRequestManager::on_new_order(order_request_p order_request)
{
	if (order_request->request_type == OrderRequestType::Abroad)
		ab_make_accepted_order_event(order_request);
	else
		dm_make_accepted_order_event(order_request);
}

void OrderRequestManager::on_change_order(order_request_p order_request)
{
	if (order_request->request_type == OrderRequestType::Abroad)
		ab_make_accepted_order_event(order_request);
	else
		ab_make_accepted_order_event(order_request);
}

void OrderRequestManager::on_cancel_order(order_request_p order_request)
{
	if (order_request->request_type == OrderRequestType::Abroad)
		ab_make_accepted_order_event(order_request);
	else
		ab_make_accepted_order_event(order_request);
}

void OrderRequestManager::start_handle_order_request() noexcept
{
	start();
}

void OrderRequestManager::stop_handle_order_request() noexcept
{
	if (!m_runMode) return;
	// 먼저 큐를 비운다.
	clear_order_requests();
	m_stop = true;
	order_request_p empty_order_request = make_dummy_order_request();
	// 쓰레드를 깨운다. 
	order_request_q.add(empty_order_request);
	// 쓰레드가 끝날때까지 기다린다.
	if (m_thread.joinable()) m_thread.join();
	// 쓰레드 런모드를 설정한다. 
	if (m_runMode) m_runMode = false;
}

order_request_p OrderRequestManager::make_order_request(
	const int order_price, 
	const int order_amount, 
	const std::string& symbol_code, 
	const SmPositionType& position_type, 
	const SmOrderType& order_type, 
	const SmPriceType& price_type, 
	const SmFilledCondition& fill_condition)
{
	std::shared_ptr<OrderRequest> order_req = std::make_shared<OrderRequest>();
	order_req->request_id = get_id();
	order_req->order_price = order_price;
	order_req->order_amount = order_amount;
	order_req->symbol_code = symbol_code;
	order_req->position_type = position_type;
	order_req->order_type = order_type;
	order_req->price_type = price_type;
	order_req->fill_condition = fill_condition;
	return order_req;
}

order_request_p OrderRequestManager::make_order_request(
	const std::string& account_no, 
	const std::string& password, 
	const int order_price, 
	const int order_amount, 
	const std::string& symbol_code, 
	const SmPositionType& position_type, 
	const SmOrderType& order_type, 
	const SmPriceType& price_type, 
	const SmFilledCondition& fill_condition)
{
	std::shared_ptr<OrderRequest> order_req = std::make_shared<OrderRequest>();
	order_req->request_id = get_id();
	order_req->order_price = order_price;
	order_req->order_amount = order_amount;
	order_req->symbol_code = symbol_code;
	order_req->position_type = position_type;
	order_req->order_type = order_type;
	order_req->price_type = price_type;
	order_req->fill_condition = fill_condition;
	return order_req;
}

order_request_p OrderRequestManager::make_default_sell_order_request(
	const std::string& symbol_code, 
	const int order_price, 
	const SmPositionType& position_type /*= SmPositionType::Sell*/, 
	const int order_amount /*= 1*/, 
	const SmOrderType& order_type /*= SmOrderType::New*/, 
	const SmPriceType& price_type /*= SmPriceType::Price*/, 
	const SmFilledCondition& fill_condition /*= SmFilledCondition::Day*/)
{
	std::shared_ptr<OrderRequest> order_req = std::make_shared<OrderRequest>();
	order_req->request_id = get_id();
	order_req->order_price = order_price;
	order_req->order_amount = order_amount;
	order_req->symbol_code = symbol_code;
	order_req->position_type = position_type;
	order_req->order_type = order_type;
	order_req->price_type = price_type;
	order_req->fill_condition = fill_condition;
	return order_req;
}

order_request_p OrderRequestManager::make_default_sell_order_request(
	const std::string& account_no, 
	const std::string& password, 
	const std::string& symbol_code, 
	const int order_price, 
	const int order_amount /*= 1*/, 
	const SmPriceType& price_type /*= SmPriceType::Price*/, 
	const SmPositionType& position_type /*= SmPositionType::Sell*/, 
	const SmOrderType& order_type /*= SmOrderType::New*/, 
	const SmFilledCondition& fill_condition /*= SmFilledCondition::Day*/)
{
	std::shared_ptr<OrderRequest> order_req = std::make_shared<OrderRequest>();
	order_req->request_id = get_id();
	order_req->order_price = order_price;
	order_req->order_amount = order_amount;
	order_req->symbol_code = symbol_code;
	order_req->position_type = position_type;
	order_req->order_type = order_type;
	order_req->price_type = price_type;
	order_req->fill_condition = fill_condition;
	return order_req;
}

order_request_p OrderRequestManager::make_default_buy_order_request(
	const std::string& symbol_code, 
	const int order_price, 
	const SmPositionType& position_type /*= SmPositionType::Buy*/, 
	const int order_amount /*= 1*/, 
	const SmOrderType& order_type /*= SmOrderType::New*/, 
	const SmPriceType& price_type /*= SmPriceType::Price*/, 
	const SmFilledCondition& fill_condition /*= SmFilledCondition::Day*/)
{
	std::shared_ptr<OrderRequest> order_req = std::make_shared<OrderRequest>();
	order_req->request_id = get_id();
	order_req->order_price = order_price;
	order_req->order_amount = order_amount;
	order_req->symbol_code = symbol_code;
	order_req->position_type = position_type;
	order_req->order_type = order_type;
	order_req->price_type = price_type;
	order_req->fill_condition = fill_condition;
	return order_req;
}

order_request_p OrderRequestManager::make_default_buy_order_request(
	const std::string& account_no, 
	const std::string& password, 
	const std::string& symbol_code, 
	const int order_price, 
	const int order_amount /*= 1*/, 
	const SmPriceType& price_type /*= SmPriceType::Price*/, 
	const SmPositionType& position_type /*= SmPositionType::Buy*/, 
	const SmOrderType& order_type /*= SmOrderType::New*/, 
	const SmFilledCondition& fill_condition /*= SmFilledCondition::Day*/)
{
	std::shared_ptr<OrderRequest> order_req = std::make_shared<OrderRequest>();
	order_req->request_id = get_id();
	order_req->order_price = order_price;
	order_req->order_amount = order_amount;
	order_req->symbol_code = symbol_code;
	order_req->position_type = position_type;
	order_req->order_type = order_type;
	order_req->price_type = price_type;
	order_req->fill_condition = fill_condition;
	return order_req;
}

order_request_p OrderRequestManager::make_change_order_request(
	const std::string& account_no, 
	const std::string& password, 
	const std::string& symbol_code, 
	const std::string& ori_order_no, 
	const int order_price, 
	const SmPositionType& position_type /*= SmPositionType::Buy*/, 
	const int order_amount /*= 1*/, 
	const SmOrderType& order_type /*= SmOrderType::Modify*/, 
	const SmPriceType& price_type /*= SmPriceType::Price*/, 
	const SmFilledCondition& fill_condition /*= SmFilledCondition::Day*/)
{
	std::shared_ptr<OrderRequest> order_req = std::make_shared<OrderRequest>();
	order_req->request_id = get_id();
	order_req->order_price = order_price;
	order_req->order_amount = order_amount;
	order_req->symbol_code = symbol_code;
	order_req->position_type = position_type;
	order_req->order_type = order_type;
	order_req->price_type = price_type;
	order_req->fill_condition = fill_condition;
	return order_req;
}

order_request_p OrderRequestManager::make_cancel_order_request(
	const std::string& account_no, 
	const std::string& password, 
	const std::string& symbol_code, 
	const std::string& ori_order_no, 
	const int order_price, 
	const SmPositionType& position_type /*= SmPositionType::Buy*/, 
	const int order_amount /*= 1*/, 
	const SmOrderType& order_type /*= SmOrderType::Cancel*/, 
	const SmPriceType& price_type /*= SmPriceType::Price*/, 
	const SmFilledCondition& fill_condition /*= SmFilledCondition::Day*/)
{
	std::shared_ptr<OrderRequest> order_req = std::make_shared<OrderRequest>();
	order_req->request_id = get_id();
	order_req->order_price = order_price;
	order_req->order_amount = order_amount;
	order_req->symbol_code = symbol_code;
	order_req->position_type = position_type;
	order_req->order_type = order_type;
	order_req->price_type = price_type;
	order_req->fill_condition = fill_condition;
	return order_req;
}

order_request_p OrderRequestManager::find_order_request(const std::string& custom_info)
{
	if (custom_info.empty()) return nullptr;

	const int zero_pos = custom_info.find_first_of('0');
	const int order_request_position = custom_info.find_first_of('k');
	const std::string order_request_string = custom_info.substr(zero_pos, order_request_position - zero_pos);
	const int order_request_id = std::stoi(order_request_string);
	
	return find_order_request(order_request_id);
}
}