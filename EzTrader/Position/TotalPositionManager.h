#pragma once
#include <string>
#include <map>
#include <memory>
#include "../Json/json.hpp"
#include "../ViewModel/VmPosition.h"
namespace DarkHorse {
class AccountPositionManager;
struct Order;
struct SmQuote;
struct FundPosition;
using order_p = std::shared_ptr<Order>;
using quote_p = std::shared_ptr<SmQuote>;
struct Position;
struct ParentPosition;
struct FundPosition;
using account_position_manager_p = std::shared_ptr<AccountPositionManager>;
using position_p = std::shared_ptr<Position>;
using fund_position_p = std::shared_ptr<FundPosition>;
using account_position_p = std::shared_ptr<ParentPosition>;

struct PairCompare {
	bool operator()(const std::pair<std::string, std::string>& lhs, const std::pair<std::string, std::string>& rhs) const {
		if (lhs.first != rhs.first) {
			return lhs.first < rhs.first;
		}
		return lhs.second < rhs.second;
	}
};

class TotalPositionManager
{
public:
	position_p get_position(const std::string& account_no, const std::string& symbol_code);
	
	void get_position_from_fund(const std::string& fund_name, const std::string& symbol_code, VmPosition& position, std::map<std::string, std::shared_ptr<Position>>& position_map);
	void get_position_from_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position, std::map<std::string, std::shared_ptr<Position>>& position_map);
	std::shared_ptr<Position> get_position_from_account(const std::string& account_no, const std::string& symbol_code);
	void get_position_from_parent_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position, std::map<std::string, std::shared_ptr<Position>>& position_map);

	account_position_manager_p get_account_position_manager(const std::string& account_no);
	/// <summary>
	/// 순서는 update_position->update_account_position->update_fund_position로 진행한다. 
	/// 반드시 이 순서를 지켜야 한다. 그리고 모든 주문에 대하여 이 세개의 함수를 모두 불러야 하는지 
	/// 확인하고 불러야 된다면 반드시 순서를 지켜서 불러야 한다.
	/// </summary>
	/// <param name="order"></param>
	void update_position(order_p order);
	void update_position(quote_p quote);
	void on_symbol_position(nlohmann::json&& arg);
	void on_symbol_profit_loss(nlohmann::json&& arg);
	static double calculate_symbol_open_profit_loss
	(
		const int& position_open_quantity,
		const int& symbol_close,
		const double& position_average_price,
		const int& symbol_seungsu,
		const int& symbol_decimal
	);
	void update_account_profit_loss(const std::string& account_no);
	position_p find_position_by_id(const int& position_id);

	void update_position2(order_p order);
	void update_position2(quote_p quote);

	position_p find_position(const std::string& source_name, const std::string& symbol_code);
	position_p update_parent_position(order_p order, position_p position);
	position_p update_parent_position(position_p position);
private:
	std::shared_ptr<Position> find_parent_position(position_p position);
	std::shared_ptr<Position> add_to_main_account(order_p order, position_p position, const std::string& account_no);
	std::shared_ptr<Position> add_to_fund(order_p order, position_p position, const std::string& fund_name);
	position_p find_position(const std::pair<std::string, std::string>& key);
	void add_to_symbol_position_map(position_p position);
	// key : position id, value : position object.
	typedef std::map<int, position_p> position_list;
	void update_position_sub_account(order_p order);
	void update_position_main_account(const std::string& account_no, const std::string& symbol_code);
	void update_position_fund(const std::string& fund_name, const std::string& symbol_code);
	void update_position(position_p main_position, std::vector<position_p> position_list);
	void calculate_parent_position(std::shared_ptr<Position> parent_position);
	// update position related to the current position. for example, main account or fund.
	void update_parent_profit_loss(order_p order);
	position_p get_position(order_p order);
	position_p create_position(const std::pair<std::string, std::string >& key, order_p order);
	position_p create_position_for_main_account(order_p order);
	position_p create_position_for_fund(order_p order);
	int calculate_position_count(order_p order, position_p position);
	int calculate_unsettled_count(order_p order, position_p position);
	int calculate_traded_count(order_p order, position_p position);
	double calculate_traded_profit_loss(order_p order, position_p position, const int& symbol_decimal, const int& symbol_seungsu);
	double calculate_average_price(order_p order, position_p position, const int& new_open_quantity, const int& unsettled_count);
	void update_open_profit_loss(position_p position);

	// key : account_no/fund_name and symbol code, value : position id.
	std::map<std::pair<std::string, std::string>, position_p, PairCompare> total_position_map_;
	// key : symbol code, value : position list related to the symbol.
	std::map<std::string, position_list> symbol_position_map_;
	// key : position id, value : position object.
	std::map<int, position_p> position_map_;
	account_position_manager_p find_position_manager(const std::string& account_no);
	account_position_manager_p create_position_manager(const std::string& account_no);
	// key : account_no, value : account position manager object.
	std::map<std::string, account_position_manager_p> position_manager_map_;
	// key : account no with symbol code, value : ParentPosition object.
	std::map<std::pair<std::string, std::string>, std::shared_ptr<Position>, PairCompare> account_position_map_;
	// key : account no with symbol code, value : ParentPosition object.
	std::map<std::pair<std::string, std::string>, std::shared_ptr<Position>, PairCompare> fund_position_map_;
};
}

