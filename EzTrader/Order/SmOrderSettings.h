#pragma once
#include "SmOrderConst.h"
namespace DarkHorse {
struct SmOrderSettings {
	// Enable Loss Cut
	bool LossCut{ false };
	// Loss Cut Tick
	int LossCutTick{ 2 };
	// Enable Profit Cut
	bool ProfitCut{ false };
	// Profit Cut Tick
	int ProfitCutTick{ 2 };
	// When Order Type is at-price, this value is used
	int SlipTick{ 2 };
	// Loss Cut / Profit Cut Price Type
	SmPriceType PriceType = SmPriceType::Price;
};
}