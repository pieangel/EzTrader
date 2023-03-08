#include "stdafx.h"
#include "HogaControl.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../View/SymbolOrderView.h"
#include "../Hoga/SmHoga.h"
#include <functional>

namespace DarkHorse {
	HogaControl::HogaControl()
		: id_(IdGenerator::get_id())
	{
		subscribe_hoga_control();
	}
	void HogaControl::subscribe_hoga_control()
	{
		mainApp.event_hub()->subscribe_hoga_event_handler
		(
			id_,
			std::bind(&HogaControl::update_hoga, this, std::placeholders::_1)
		);
	}
	void HogaControl::update_hoga(std::shared_ptr<SmHoga> hoga)
	{
		if (!hoga) return;

		for (int i = 0; i < 5; i++) {
			hoga_.Ary[i].BuyCnt = hoga->Ary[i].BuyCnt;
			hoga_.Ary[i].BuyPrice = hoga->Ary[i].BuyPrice;
			hoga_.Ary[i].BuyQty = hoga->Ary[i].BuyQty;
			hoga_.Ary[i].SellCnt = hoga->Ary[i].SellCnt;
			hoga_.Ary[i].SellPrice = hoga->Ary[i].SellPrice;
			hoga_.Ary[i].SellQty = hoga->Ary[i].SellQty;
		}

		hoga_.TotSellQty = hoga->TotSellQty;
		hoga_.TotBuyQty = hoga->TotBuyQty;
		hoga_.TotSellCnt = hoga->TotSellCnt;
		hoga_.TotBuyCnt = hoga->TotBuyCnt;
		hoga_.HogaTime = hoga->HogaTime;

		if (symbol_order_view_) symbol_order_view_->update_hoga(hoga);
	}
}