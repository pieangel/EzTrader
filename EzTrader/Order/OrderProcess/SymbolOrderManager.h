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
private:
	
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
};
}

