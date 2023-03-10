#pragma once
#include <string>
#include <map>
#include <memory>
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
	account_position_manager_p get_position_manager(const std::string& account_no);
	/// <summary>
	/// 순서는 update_position->update_account_position->update_fund_position로 진행한다. 
	/// 반드시 이 순서를 지켜야 한다. 그리고 모든 주문에 대하여 이 세개의 함수를 모두 불러야 하는지 
	/// 확인하고 불러야 된다면 반드시 순서를 지켜서 불러야 한다.
	/// </summary>
	/// <param name="order"></param>
	void update_position(order_p order);
	void update_account_position(const std::string& account_no);
	void update_fund_position(const std::string& fund_name);
private:
	account_position_manager_p find_position_manager(const std::string& account_no);
	account_position_manager_p create_position_manager(const std::string& account_no);
	// key : account_no, value : account position manager object.
	std::map<std::string, account_position_manager_p> position_manager_map_;
};
}

