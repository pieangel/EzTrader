#include "stdafx.h"
#include "QuoteControl.h"
namespace DarkHorse {
	void QuoteControl::update_quote(std::shared_ptr<SmQuote> quote)
	{
		if (!quote) return;

		quote_.high = quote->high;
		quote_.low = quote->low;
		quote_.open = quote->open;
		quote_.close = quote->close;
		quote_.preclose = quote->preclose;
	}
}