#pragma once
#include <string>
#include <memory>

namespace DarkHorse {
struct Position;
class AccountOrderControl;
class AccountPositionControl;
class AccountProfitLossControl;
class AssetControl;
class BuyHogaControl;
class BuyOrderControl;
class BuyStopOrderControl;
class FavoriteSymbolControl;
class FutureCloseControl;
class FuturePositionControl;
class FutureExpectedControl;
class OptionCloseControl;
class OptionPositionControl;
class OptionExpectedControl;
class PriceRangeControl;
class SellHogaControl;
class SellOrderControl;
class SellStopOrderControl;
class SymbolInfoControl;
class SymbolPositionControl;
class SymbolProfitLossControl;
class SymbolTickControl;
class TotalHogaInfoControl;
class OrderWindowControl
{
public:
	OrderWindowControl(const int id);
	~OrderWindowControl();
	void remove_order(const std::string& order_no);
	void update_position(std::shared_ptr<Position> position);
private:
	void subscribe_remove_order_event();
private:
	int _id;
	/*
	std::shared_ptr<AccountOrderControl> account_order_control_;
	std::shared_ptr<AccountPositionControl> account_position_control_;
	std::shared_ptr<AccountProfitLossControl> account_position_control_;
	std::shared_ptr<AssetControl> account_position_control_;
	std::shared_ptr<BuyHogaControl> account_position_control_;
	std::shared_ptr<BuyOrderControl> account_position_control_;
	std::shared_ptr<BuyStopOrderControl> account_position_control_;
	std::shared_ptr<FavoriteSymbolControl> account_position_control_;
	std::shared_ptr<FutureCloseControl> account_position_control_;
	std::shared_ptr<FuturePositionControl> account_position_control_;
	std::shared_ptr<FutureExpectedControl> account_position_control_;
	std::shared_ptr<OptionCloseControl> account_position_control_;
	std::shared_ptr<OptionPositionControl> account_position_control_;
	std::shared_ptr<OptionExpectedControl> account_position_control_;
	std::shared_ptr<SellHogaControl> account_position_control_;
	std::shared_ptr<SellOrderControl> account_position_control_;
	std::shared_ptr<SellStopOrderControl> account_position_control_;
	std::shared_ptr<SymbolInfoControl> account_position_control_;
	std::shared_ptr<SymbolPositionControl> account_position_control_;
	std::shared_ptr<SymbolProfitLossControl> account_position_control_;
	std::shared_ptr<SymbolTickControl> account_position_control_;
	std::shared_ptr<TotalHogaInfoControl> account_position_control_;
	*/
};
}

