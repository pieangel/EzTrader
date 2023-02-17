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
	// ����Ȯ�� �ֹ��� �߰��Ѵ�.
	void add_accepted_order(order_p order);
	// ����Ȯ�� �ֹ��� ��ȭ�� ������ �� �̸� �ݿ��Ѵ�.
	// ����Ȯ�� �ֹ��� ��� ü��Ǹ� ����Ȯ�� ��Ͽ��� �����ְ�
	// �κ�ü��Ǿ��� ��쿡�� �ֹ� ������ ������ �ְ� ��Ͽ��� ������ �ش�.
	void update_accepted_order(order_p order);
	// key : order_no, value : order object.
	std::map<std::string, order_p> accepted_order_map_;
};
}

