#pragma once
#include <map>
#include <string>
#include <memory>
#include "../ViewModel/VmOrder.h"
#include "../Order/Order.h"
namespace DarkHorse {
	enum SubOrderControlType { CT_NONE = 0, CT_BUY, CT_SELL };
	struct PriceOrderMap {
		int price{ 0 };
		size_t count{ 0 };
		// key : order no, value : accepted order object.
		std::map<std::string, std::shared_ptr<Order>> order_map;
		void add_order(std::shared_ptr<Order> order) {
			if (order->order_price != price) return;
			order_map[order->order_no] = order;
			count = order_map.size();
		}
		void remove_order(const std::string& order_no) {
			auto it = order_map.find(order_no);
			if (it == order_map.end()) return;
			order_map.erase(it);
			count = order_map.size();
		}
	};
	/// <summary>
	/// ���ΰ��� �� ���� ���� ���¿� ���� ������ ��� �ֹ��� �����ؾ� �Ѵ�.
	/// �ݵ� �� ���� �ݵ忡 ���� ��� ������ �ֹ��� �����ؾ� �Ѵ�. 
	/// ��Ʈ�ѿ� �� ������ id ���� ������ ������ �ֹ��� ���͸� �ؾ� �Ѵ�. 
	/// </summary>
	struct SubOrderControl
	{
		SubOrderControlType control_type{ CT_NONE };
		size_t total_count{ 0 };
		// key : price as integer, value : order list on the price. 
		std::map<int, PriceOrderMap> order_map;
		void add_order(const int order_price, std::shared_ptr<Order> order) {
			auto it = order_map.find(order_price);
			if (it == order_map.end()) {
				PriceOrderMap price_order_map;
				price_order_map.price = order_price;
				price_order_map.add_order(order);
				order_map[order_price] = std::move(price_order_map);
			}
			else {
				PriceOrderMap& price_order_map = it->second;
				price_order_map.add_order(order);
			}
			calculate_total_count();
		}

		void remove_order(const int order_price, const std::string& order_no) {
			auto it = order_map.find(order_price);
			if (it == order_map.end()) return;
			PriceOrderMap& price_order_map = it->second;
			price_order_map.remove_order(order_no);
			calculate_total_count();
		}

		void calculate_total_count() {
			total_count = 0;
			for (auto it : order_map)
				total_count += it.second.count;
		}
	};
}

