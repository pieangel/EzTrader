#include "stdafx.h"
#include "OrderRequest.h"
#include "OrderRequestManager.h"
#include "../../Global/SmTotalManager.h"
#include "../../Client/ViStockClient.h"
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

		std::array<order_request_p, BulkOrderRequestSize> order_request_arr;
		size_t taken{ 0 };
		auto status = order_request_q.try_take_bulk(order_request_arr.begin(), order_request_arr.size(), taken);
		if (status != BlockingCollectionStatus::Ok) continue;
 		handle_order_request(order_request_arr, taken);
	}

	return 1;
}

void OrderRequestManager::clear_order_requests() noexcept
{
	order_request_q.flush();
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