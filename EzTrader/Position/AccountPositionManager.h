#pragma once
#include <string>
#include <map>
#include <memory>
namespace DarkHorse {
	struct Position;
	struct SmQuote;
	struct AccountProfitLoss;
	using quote_p = std::shared_ptr<SmQuote>;
	using position_p = std::shared_ptr<Position>;
	using account_profit_loss_p = std::shared_ptr<AccountProfitLoss>;
	struct Order;
	struct OrderRequest;
	using order_p = std::shared_ptr<Order>;
	using order_request_p = std::shared_ptr<OrderRequest>;
	class TotalPositionManager;
	struct GroupPosition;
class AccountPositionManager
{
public:
	AccountPositionManager(TotalPositionManager& total_position_manager, const std::string& account_no);
	~AccountPositionManager();
	position_p get_position(const std::string& symbol_code);
	void update_position(order_p order);
	void update_position(quote_p quote);
	const std::map<std::string, position_p>& get_position_map() {
		return position_map_;
	}
	account_profit_loss_p get_account_profit_loss() {
		return account_profit_loss_;
	}
	void update_account_profit_loss();
	position_p find_position(const std::string& symbol_code);
private:
	std::shared_ptr<GroupPosition> create_group_position(const std::string& account_no, const std::string& symbol_code);
	void set_symbol_id(position_p position, const std::string& symbol_code);
	void set_account_id(position_p position, const std::string& account_no);
	int calculate_position_count(order_p order, position_p position);
	int calculate_unsettled_count(order_p order, position_p position);
	int calculate_traded_count(order_p order, position_p position);
	double calculate_traded_profit_loss(order_p order, position_p position, const int& symbol_decimal, const int& symbol_seungsu);
	double calculate_average_price(order_p order, position_p position, const int& position_count, const int& unsettled_count);
	void update_open_profit_loss(position_p position);
	position_p create_position(const std::string& symbol_code);
	// key : symbol coode, value : position object.
	std::map<std::string, position_p> position_map_;
	account_profit_loss_p account_profit_loss_{ nullptr };
	int id_{ 0 };
	std::string account_no_;
	TotalPositionManager& total_position_manager_;
	// key : symbol code, value : group position object.
	std::map<std::string, std::shared_ptr<GroupPosition>> group_position_map_;
};
}

