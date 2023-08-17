#include "stdafx.h"
#include "FundPositionManager.h"
#include "Position.h"
namespace DarkHorse {
FundPositionManager::FundPositionManager(TotalPositionManager& total_position_manager, const std::string& fund_name)
	: total_position_manager_(total_position_manager)
	, fund_name_(fund_name)
{

}

FundPositionManager::~FundPositionManager()
{

}

std::shared_ptr<DarkHorse::GroupPosition> FundPositionManager::create_group_position(const std::string& fund_name, const std::string& symbol_code)
{
	auto group_position = std::make_shared<GroupPosition>();
	group_position->fund_name = fund_name;
	group_position->symbol_code = symbol_code;
	group_position_map_[symbol_code] = group_position;

	return group_position;
}

void FundPositionManager::add_position(std::shared_ptr<Position> position)
{	
	if (!position) return;
	if (position->fund_name != fund_name_) return;
	std::shared_ptr<DarkHorse::GroupPosition> group_position = nullptr;
	auto found = group_position_map_.find(position->symbol_code);
	if (found == group_position_map_.end()) {
		group_position = create_group_position(fund_name_, position->symbol_code);
	}
	else {
		group_position = found->second;
	}
}

void FundPositionManager::update_group_position(std::shared_ptr<GroupPosition> group_position)
{
	if (!group_position) return;

	auto sub_positions = group_position->sub_positions;
	double trade_profit_loss{ 0.0f };       //매매(청산)손익
	double open_profit_loss{ 0.0f };		//평가손익
	double pure_trade_profit_loss{ 0.0f };  // 청산 순손익
	double trade_fee{ 0.0f };               // 청산 수수료
	for (auto& sub_position : sub_positions) {
		auto position = sub_position.second;
		//update_open_profit_loss(position);
		trade_profit_loss += position->trade_profit_loss;
		open_profit_loss += position->open_profit_loss;
		trade_fee += position->trade_fee;
		pure_trade_profit_loss += position->pure_trade_profit_loss;
	}
	group_position->trade_profit_loss = trade_profit_loss;
	group_position->open_profit_loss = open_profit_loss;
	group_position->trade_fee = trade_fee;
	group_position->pure_trade_profit_loss = pure_trade_profit_loss;
}

}