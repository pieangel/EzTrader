#pragma once
#include "../ViewModel/VmQuote.h"
#include "../Util/IdGenerator.h"
#include <memory>
class SymbolOrderView;
namespace DarkHorse {
	struct SmQuote;
class QuoteControl
{
public:
	QuoteControl();
	~QuoteControl() {};
	void update_quote(std::shared_ptr<SmQuote> quote);
	const VmQuote& get_quote()
	{
		return quote_;
	}
	const int get_id()
	{
		return id_;
	}
	void symbol_order_view(SymbolOrderView* symbol_order_view_p) {
		symbol_order_view_ = symbol_order_view_p;
	}
private:
	VmQuote quote_;
	void subscribe_quote_control();
	int id_;
	SymbolOrderView* symbol_order_view_{ nullptr };
};
}

