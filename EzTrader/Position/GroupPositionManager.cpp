#include "stdafx.h"
#include "GroupPositionManager.h"
#include "Position.h"
namespace DarkHorse {
GroupPositionManager::GroupPositionManager(TotalPositionManager& total_position_manager, const std::string& fund_name)
	: total_position_manager_(total_position_manager)
	, fund_name_(fund_name)
{

}

GroupPositionManager::~GroupPositionManager()
{

}

std::shared_ptr<DarkHorse::GroupPosition> GroupPositionManager::create_group_position(const std::string& fund_name, const std::string& symbol_code)
{
	auto group_position = std::make_shared<GroupPosition>();
	group_position->fund_name = fund_name;
	group_position->symbol_code = symbol_code;
	group_position_map_[symbol_code] = group_position;

	return group_position;
}

std::shared_ptr<GroupPosition> GroupPositionManager::add_position(std::shared_ptr<Position> position)
{	
	if (!position) return nullptr;
	if (position->fund_name != fund_name_) return nullptr;
	std::shared_ptr<DarkHorse::GroupPosition> group_position = nullptr;
	auto found = group_position_map_.find(position->symbol_code);
	if (found == group_position_map_.end()) {
		group_position = create_group_position(fund_name_, position->symbol_code);
	}
	else {
		group_position = found->second;
	}

	group_position->sub_positions[position->symbol_code] = position;

	return group_position;
}

void GroupPositionManager::update_group_position(std::shared_ptr<GroupPosition> group_position)
{
	if (!group_position) return;

	auto sub_positions = group_position->sub_positions;
	double trade_profit_loss{ 0.0f };       //�Ÿ�(û��)����
	double open_profit_loss{ 0.0f };		//�򰡼���
	double pure_trade_profit_loss{ 0.0f };  // û�� ������
	double trade_fee{ 0.0f };               // û�� ������
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

void GroupPositionManager::update_group_position(std::shared_ptr<Position> position)
{
	if (!position) return;

	auto group_position = add_position(position);
	update_group_position(group_position);
}

}