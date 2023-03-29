#include "stdafx.h"
#include "SymbolTickControl.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../View/SymbolOrderView.h"
#include "../Quote/SmQuote.h"
#include "../Util/IdGenerator.h"
#include "../View/SymbolTickView.h"
#include <functional>
namespace DarkHorse {
	SymbolTickControl::SymbolTickControl()
		: id_(IdGenerator::get_id())
	{
		subscribe_tick_control();
		tick_vec_.resize(26);
	}
	void SymbolTickControl::update_tick(SmTick tick)
	{
		//if (tick.symbol_id != symbol_id_) return;

		std::rotate(tick_vec_.rbegin(), tick_vec_.rbegin() + 1, tick_vec_.rend());
		tick_vec_[0] = std::move(tick);

		if (symbol_tick_view_) symbol_tick_view_->update_tick();
	}

	void SymbolTickControl::subscribe_tick_control()
	{
		mainApp.event_hub()->subscribe_tick_event_handler
		(
			id_,
			std::bind(&SymbolTickControl::update_tick, this, std::placeholders::_1)
		);
	}

}