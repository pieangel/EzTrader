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
using order_p = std::shared_ptr<Order>;
using quote_p = std::shared_ptr<SmQuote>;
struct Position;
using account_position_manager_p = std::shared_ptr<AccountPositionManager>;
using position_p = std::shared_ptr<Position>;
class TotalPositionManager
{
public:
	position_p get_position(const std::string& account_no, const std::string& symbol_code);
	void get_position_from_fund(const std::string& fund_name, const std::string& symbol_code, VmPosition& position);
	void get_position_from_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position);
	void get_position_from_parent_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position);
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
private:
	// key : position id, value : position object.
	std::map<int, position_p> position_map_;
	account_position_manager_p find_position_manager(const std::string& account_no);
	account_position_manager_p create_position_manager(const std::string& account_no);
	// key : account_no, value : account position manager object.
	std::map<std::string, account_position_manager_p> position_manager_map_;
};
}

