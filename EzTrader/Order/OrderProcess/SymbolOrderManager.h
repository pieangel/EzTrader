#pragma once
#include <string>
#include <map>
#include <memory>
#include "../../Json/json.hpp"
#include "../SmOrderConst.h"
namespace DarkHorse {
using order_event = nlohmann::json;
struct Order;
struct Position;
using order_p = std::shared_ptr<Order>;
using position_p = std::shared_ptr<Position>;
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
	OrderBackGround get_order_background(const int position_open_qty);
private:
	// ������ �ֹ��� �������� ����. �ѹ��̶� �����ٸ� true, �ƴϸ� false.
	bool ordered_before_{ false };
	void on_order_accepted(order_p order, OrderEvent order_event);
	void on_order_unfilled(order_p order, OrderEvent order_event);
	void on_order_filled(order_p order, OrderEvent order_event);
	// ����Ȯ�� �ֹ��� �߰��Ѵ�.
	void add_accepted_order(order_p order);
	// ����Ȯ�� �ֹ��� ��ȭ�� ������ �� �̸� �ݿ��Ѵ�.
	// ����Ȯ�� �ֹ��� ��� ü��Ǹ� ����Ȯ�� ��Ͽ��� �����ְ�
	// �κ�ü��Ǿ��� ��쿡�� �ֹ� ������ ������ �ְ� ��Ͽ��� ������ �ش�.
	void update_accepted_order(order_p order);
	// ������ ü��� �ֹ��� ����Ȯ�� ��Ͽ��� ���� �ش�. 
	void remove_accepted_order(order_p order);
	// key : order_no, value : order object.
	std::map<std::string, order_p> accepted_order_map_;
	std::string symbol_code_;
};
}

