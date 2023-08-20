#pragma once
#include <string>
#include <map>
#include <memory>

namespace DarkHorse {
	class TotalPositionManager;
	struct GroupPosition;
	struct Position;
	struct AccountProfitLoss;
	using account_profit_loss_p = std::shared_ptr<AccountProfitLoss>;
	class GroupPositionManager
	{
	public:
		GroupPositionManager(TotalPositionManager& total_position_manager);
		~GroupPositionManager();
		void update_group_position(std::shared_ptr<Position> position);
		void set_fund_name(const std::string& fund_name) {
			fund_name_ = fund_name;
		}
		void set_account_no(const std::string& account_no) {
			account_no_ = account_no;
		}
		const std::map<std::string, std::shared_ptr<Position>>& get_group_position_map() {
			return group_position_map_;
		}
		void update_whole_group_position();
		account_profit_loss_p get_whole_profit_loss() {
			return whole_profit_loss_;
		}
	private:
		account_profit_loss_p whole_profit_loss_{ nullptr };
		std::shared_ptr<Position> create_group_position(std::shared_ptr<Position> position);
		std::shared_ptr<Position> add_position(std::shared_ptr<Position> position);
		void update_group_position_by(std::shared_ptr<Position> group_position);
		std::string fund_name_;
		std::string account_no_;
		TotalPositionManager& total_position_manager_;
		// key : symbol code, value : group position object.
		std::map<std::string, std::shared_ptr<Position>> group_position_map_;
	};
}