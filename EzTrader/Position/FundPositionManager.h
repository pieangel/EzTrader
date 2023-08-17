#pragma once
#include <string>
#include <map>
#include <memory>

namespace DarkHorse {
	class TotalPositionManager;
	struct GroupPosition;
	struct Position;
	class FundPositionManager
	{
	public:
		FundPositionManager(TotalPositionManager& total_position_manager, const std::string& fund_name);
		~FundPositionManager();
		std::shared_ptr<GroupPosition> create_group_position(const std::string& fund_name, const std::string& symbol_code);
		void add_position(std::shared_ptr<Position> position);
	private:
		void update_group_position(std::shared_ptr<GroupPosition> group_position);
		std::string fund_name_;
		TotalPositionManager& total_position_manager_;
		// key : symbol code, value : group position object.
		std::map<std::string, std::shared_ptr<GroupPosition>> group_position_map_;
	};
}
