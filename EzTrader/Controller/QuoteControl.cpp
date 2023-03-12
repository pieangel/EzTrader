#include "stdafx.h"
#include "QuoteControl.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../View/SymbolOrderView.h"
#include "../Quote/SmQuote.h"
#include <functional>

namespace DarkHorse {
	QuoteControl::QuoteControl()
		: id_(IdGenerator::get_id())
	{
		subscribe_quote_control();
	}
	void QuoteControl::update_quote(std::shared_ptr<SmQuote> quote)
	{
		if (!quote) return;

		quote_.high = quote->high;
		quote_.low = quote->low;
		quote_.open = quote->open;
		quote_.close = quote->close;
		quote_.pre_day_close = quote->pre_day_close;
		quote_.init = true;

		if (symbol_order_view_) symbol_order_view_->update_quote();
	}

	void QuoteControl::subscribe_quote_control()
	{
		mainApp.event_hub()->subscribe_quote_event_handler
		(
			id_,
			std::bind(&QuoteControl::update_quote, this, std::placeholders::_1)
		);
	}
	
}