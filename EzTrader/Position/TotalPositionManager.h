#pragma once
#include <string>
#include <map>
#include <memory>
#include "../Json/json.hpp"
namespace DarkHorse {
class AccountPositionManager;
struct Order;
using order_p = std::shared_ptr<Order>;
struct Position;
using account_position_manager_p = std::shared_ptr<AccountPositionManager>;
using position_p = std::shared_ptr<Position>;
class TotalPositionManager
{
public:
	position_p get_position(const std::string& account_no, const std::string& symbol_code);
	account_position_manager_p get_account_position_manager(const std::string& account_no);
	/// <summary>
	/// ������ update_position->update_account_position->update_fund_position�� �����Ѵ�. 
	/// �ݵ�� �� ������ ���Ѿ� �Ѵ�. �׸��� ��� �ֹ��� ���Ͽ� �� ������ �Լ��� ��� �ҷ��� �ϴ��� 
	/// Ȯ���ϰ� �ҷ��� �ȴٸ� �ݵ�� ������ ���Ѽ� �ҷ��� �Ѵ�.
	/// </summary>
	/// <param name="order"></param>
	void update_position(order_p order);
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
private:
	account_position_manager_p find_position_manager(const std::string& account_no);
	account_position_manager_p create_position_manager(const std::string& account_no);
	// key : account_no, value : account position manager object.
	std::map<std::string, account_position_manager_p> position_manager_map_;
};
}

