#pragma once
#include <string>
#include <map>
#include <memory>
#include "../../Json/json.hpp"
namespace DarkHorse {
using order_event = nlohmann::json;
struct Order;
using order_p = std::shared_ptr<Order>;
class SymbolOrderManager
{
public:
    void dispatch_order(const int order_event, order_p order);
private:
	void on_ab_order_accepted(order_p order);
	void on_ab_order_unfilled(order_p order);
	void on_ab_order_filled(order_p order);
	void on_dm_order_accepted(order_p order);
	void on_dm_order_unfilled(order_p order);
	void on_dm_order_filled(order_p order);
	// 접수확인 주문을 추가한다.
	void add_accepted_order(order_p order);
	// 접수확인 주문에 변화가 생겼을 때 이를 반영한다.
	// 접수확인 주문이 모두 체결되면 접수확인 목록에서 없애주고
	// 부분체결되었을 경우에는 주문 갯수만 갱신해 주고 목록에는 유지해 준다.
	void update_accepted_order(order_p order);
	// key : order_no, value : order object.
	std::map<std::string, order_p> accepted_order_map_;
};
}

