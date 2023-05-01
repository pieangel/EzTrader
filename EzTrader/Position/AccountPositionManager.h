#pragma once
#include <string>
#include <map>
#include <memory>
namespace DarkHorse {
	struct Position;
	using position_p = std::shared_ptr<Position>;
	struct Order;
	struct OrderRequest;
	using order_p = std::shared_ptr<Order>;
	using order_request_p = std::shared_ptr<OrderRequest>;
class AccountPositionManager
{
public:
	position_p get_position(const std::string& symbol_code);
	void update_position(order_p order);
	const std::map<std::string, position_p>& get_position_map() {
		return position_map_;
	}
private:
	void add_cut_stop_order(order_p order);
	order_request_p make_loss_cut_stop_order(order_request_p prev_order_req, order_p order);
	order_request_p make_profit_cut_stop_order(order_request_p prev_order_req, order_p order);
	void set_symbol_id(position_p position, const std::string& symbol_code);
	void set_account_id(position_p position, const std::string& account_no);
	int calculate_position_count(order_p order, position_p position);
	int calculate_unsettled_count(order_p order, position_p position);
	int calculate_traded_count(order_p order, position_p position);
	double calculate_traded_profit_loss(order_p order, position_p position);
	double calculate_average_price(order_p order, position_p position);
	void update_open_profit_loss(position_p position);
	position_p find_position(const std::string& symbol_code);
	position_p create_position(const std::string& symbol_code);
	// key : symbol coode, value : position object.
	std::map<std::string, position_p> position_map_;
};
}

