#pragma once
#include "../Quote/SmQuote.h"
#include <memory>
namespace DarkHorse {
class QuoteControl
{
public:
	void update_quote(std::shared_ptr<SmQuote> quote);
	const SmQuote& get_quote()
	{
		return quote_;
	}
private:
	SmQuote quote_;
};
}

