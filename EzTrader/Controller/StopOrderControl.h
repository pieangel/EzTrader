#pragma once
#include <map>
#include <string>
#include <memory>
#include "../Order/SmOrderConst.h"

namespace DarkHorse {
struct OrderRequest;
using order_request_p = std::shared_ptr<OrderRequest>;

class PriceOrderRequestMap {
private:
	int price_{ 0 };
	size_t count_{ 0 };
	// key : request id, value : accepted order object.
	std::map<int, order_request_p> order_request_map_;
public:
	void clear() {
		order_request_map_.clear();
		count_ = 0;
	}
	int get_price() const { return price_; }
	void set_price(const int price) {
		price_ = price;
	}
	size_t count() const { return count_; }
	const std::map<int, order_request_p>& get_order_request_map() {
		return order_request_map_;
	}
	void add_order_request(order_request_p order_request);
	void remove_order_request(const int request_id);
};

using price_order_request_map_p = std::shared_ptr<PriceOrderRequestMap>;
struct Order;
using order_p = std::shared_ptr<Order>;

class StopOrderControl
{
private:
	SubOrderControlType control_type_{ CT_NONE };
	size_t total_count_{ 0 };
	// key : price as integer, value : order request list on the price. 
	std::map<int, price_order_request_map_p> order_req_map_;
	order_request_p make_profit_cut_order_request(order_p order);
	order_request_p make_loss_cut_order_request(order_p order);
public:
	void set_control_type(const SubOrderControlType control_type) {
		control_type_ = control_type;
	}
	int total_count() const { return total_count_; }
	const std::map<int, price_order_request_map_p>& get_order_map() {
		return order_req_map_;
	}
	void add_order_request(const int order_price, order_request_p order_request);

	void remove_order_request(const int order_price, const int& request_id);

	void calculate_total_count();

};
}

