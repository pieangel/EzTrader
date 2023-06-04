#pragma once
#include <string>
#include <map>
#include <memory>
#include "../../Json/json.hpp"
#include "../SmOrderConst.h"
namespace DarkHorse {
using order_event = nlohmann::json;
struct Order;
using order_p = std::shared_ptr<Order>;
class SymbolOrderManager
{
public:
    void dispatch_order(const OrderEvent order_event, order_p order);
	const std::map<std::string, order_p>& get_accepted_order_map() {
		return accepted_order_map_;
	}
	void set_ordered_before(bool ordered_before) {
		ordered_before_ = ordered_before;
	}
	void set_symbol_code(const std::string& symbol_code) {
		symbol_code_ = symbol_code;
	}
private:
	// 이전에 주문이 나갔는지 여부. 한번이라도 나갔다면 true, 아니면 false.
	bool ordered_before_{ false };
	void on_order_accepted(order_p order, OrderEvent order_event);
	void on_order_unfilled(order_p order, OrderEvent order_event);
	void on_order_filled(order_p order, OrderEvent order_event);
	// 접수확인 주문을 추가한다.
	void add_accepted_order(order_p order);
	// 접수확인 주문에 변화가 생겼을 때 이를 반영한다.
	// 접수확인 주문이 모두 체결되면 접수확인 목록에서 없애주고
	// 부분체결되었을 경우에는 주문 갯수만 갱신해 주고 목록에는 유지해 준다.
	void update_accepted_order(order_p order);
	// 완전히 체결된 주문은 접수확인 목록에서 없애 준다. 
	void remove_accepted_order(order_p order);
	// key : order_no, value : order object.
	std::map<std::string, order_p> accepted_order_map_;
	std::string symbol_code_;
};
}

