#include "stdafx.h"
#include "GroupPositionManager.h"
#include "Position.h"
#include "TotalPositionManager.h"
namespace DarkHorse {
GroupPositionManager::GroupPositionManager(TotalPositionManager& total_position_manager)
	: total_position_manager_(total_position_manager)
{
	whole_profit_loss_ = std::make_shared<AccountProfitLoss>();
}

GroupPositionManager::~GroupPositionManager()
{

}

void GroupPositionManager::update_group_position_by_symbol(std::shared_ptr<Position> group_position)
{
	if (!group_position) return;

	auto sub_positions = group_position->sub_positions;
	double trade_profit_loss{ 0.0f };       //매매(청산)손익
	double open_profit_loss{ 0.0f };		//평가손익
	double pure_trade_profit_loss{ 0.0f };  // 청산 순손익
	double trade_fee{ 0.0f };   // 청산 수수료
	int open_quantity{ 0 };
	double average_price{ 0.0f };
	for (auto& sub_position : sub_positions) {
		auto position = sub_position.second;
		TotalPositionManager::calculate_symbol_open_profit_loss(position);
		trade_profit_loss += position->trade_profit_loss;
		open_profit_loss += position->open_profit_loss;
		trade_fee += position->trade_fee;
		pure_trade_profit_loss += position->pure_trade_profit_loss;

		open_quantity += position->open_quantity;
		average_price += position->average_price;
	}
	group_position->trade_profit_loss = trade_profit_loss;
	group_position->open_profit_loss = open_profit_loss;
	group_position->trade_fee = trade_fee;
	group_position->pure_trade_profit_loss = pure_trade_profit_loss;
	group_position->open_quantity = open_quantity;
	if (open_quantity == 0)
		group_position->average_price = 0.0f;
	else
		group_position->average_price = average_price / group_position->open_quantity;

	update_whole_group_position();
}


void GroupPositionManager::update_group_position_by_symbol(std::shared_ptr<Position> group_position, VmPosition& dest_position)
{
	if (!group_position) return;

	auto sub_positions = group_position->sub_positions;
	double trade_profit_loss{ 0.0f };       //매매(청산)손익
	double open_profit_loss{ 0.0f };		//평가손익
	double pure_trade_profit_loss{ 0.0f };  // 청산 순손익
	double trade_fee{ 0.0f };   // 청산 수수료
	int open_quantity{ 0 };
	double average_price{ 0.0f };
	for (auto& sub_position : sub_positions) {
		auto position = sub_position.second;
		TotalPositionManager::calculate_symbol_open_profit_loss(position);
		trade_profit_loss += position->trade_profit_loss;
		open_profit_loss += position->open_profit_loss;
		trade_fee += position->trade_fee;
		pure_trade_profit_loss += position->pure_trade_profit_loss;

		open_quantity += position->open_quantity;
		average_price += position->open_quantity * position->average_price;
	}
	dest_position.trade_profit_loss = trade_profit_loss;
	dest_position.open_profit_loss = open_profit_loss;
	dest_position.trade_fee = trade_fee;
	dest_position.pure_trade_profit_loss = pure_trade_profit_loss;
	dest_position.open_quantity = open_quantity;
	if (open_quantity == 0)
		dest_position.average_price = 0.0f;
	else
		dest_position.average_price = average_price / dest_position.open_quantity;

	update_whole_group_position();
}

std::shared_ptr<DarkHorse::Position> GroupPositionManager::get_group_position(const std::string& symbol_code)
{
	auto found = group_position_map_.find(symbol_code);
	if (found == group_position_map_.end()) return nullptr;
	else return found->second;
}

void GroupPositionManager::update_group_position(const std::shared_ptr<Position>& group_position, const std::shared_ptr<Position>& position)
{
	if (!group_position || !position) return;
	group_position->sub_positions[position->account_no] = position;
	update_group_position_by_symbol(group_position);
}

void GroupPositionManager::update_whole_group_position()
{
	double trade_profit_loss{ 0.0f };       //매매(청산)손익
	double open_profit_loss{ 0.0f };		//평가손익
	double pure_trade_profit_loss{ 0.0f };  // 청산 순손익
	double trade_fee{ 0.0f };   // 청산 수수료
	int open_quantity{ 0 };
	double average_price{ 0.0f };
	for (auto& sub_position : group_position_map_) {
		auto position = sub_position.second;
		//update_open_profit_loss(position);
		trade_profit_loss += position->trade_profit_loss;
		open_profit_loss += position->open_profit_loss;
		trade_fee += position->trade_fee;
		pure_trade_profit_loss += position->pure_trade_profit_loss;

		open_quantity += position->open_quantity;
		average_price += position->open_quantity * position->average_price;
	}
	whole_profit_loss_->trade_profit_loss = trade_profit_loss;
	whole_profit_loss_->open_profit_loss = open_profit_loss;
	whole_profit_loss_->trade_fee = trade_fee;
	whole_profit_loss_->pure_trade_profit_loss = pure_trade_profit_loss;

	whole_profit_loss_->trade_profit_loss = trade_profit_loss;
	whole_profit_loss_->open_profit_loss = open_profit_loss;
	whole_profit_loss_->trade_fee = trade_fee;
	whole_profit_loss_->pure_trade_profit_loss = pure_trade_profit_loss;
	whole_profit_loss_->open_quantity = open_quantity;
	if (open_quantity == 0)
		whole_profit_loss_->average_price = 0.0f;
	else
		whole_profit_loss_->average_price = average_price / whole_profit_loss_->open_quantity;
}

std::shared_ptr<Position> GroupPositionManager::create_account_group_position(const std::string& account_no, const std::string symbol_code)
{
	std::shared_ptr<Position> group_position = nullptr;
	auto found = group_position_map_.find(symbol_code);
	if (found == group_position_map_.end()) {
		group_position = std::make_shared<Position>();
		group_position->account_no = account_no;
		group_position->symbol_code = symbol_code;
		group_position->order_source_type = OrderType::MainAccount;
		group_position->is_group = true;
		group_position_map_[symbol_code] = group_position;
	}
	else {
		group_position = found->second;
	}
	return group_position;
}

std::shared_ptr<Position> GroupPositionManager::create_fund_group_position(const std::string& fund_name, const std::string& symbol_code)
{
	std::shared_ptr<Position> group_position = nullptr;
	auto found = group_position_map_.find(symbol_code);
	if (found == group_position_map_.end()) {
		group_position = std::make_shared<Position>();
		group_position->fund_name = fund_name;
		group_position->symbol_code = symbol_code;
		group_position->order_source_type = OrderType::Fund;
		group_position->is_group = true;
		group_position_map_[symbol_code] = group_position;
	}
	else {
		group_position = found->second;
	}
	return group_position;
}

}