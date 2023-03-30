#pragma once
#include "../ViewModel/VmQuote.h"
#include "../Util/IdGenerator.h"
#include <memory>
class SymbolOrderView;
class SymbolInfoView;
class DmFutureView;
class DmOptionView;
namespace DarkHorse {
	struct SmQuote;
class QuoteControl
{
public:
	QuoteControl();
	~QuoteControl();
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
	void symbol_info_view(SymbolInfoView* symbol_info_view_p) {
		symbol_info_view_ = symbol_info_view_p;
	}
	void dm_future_view(DmFutureView* dm_future_view) {
		dm_future_view_ = dm_future_view;
	}
	void dm_option_view(DmOptionView* dm_option_view) {
		dm_option_view_ = dm_option_view;
	}
	void set_symbol_id(const int symbol_id) {
		symbol_id_ = symbol_id;
	}
private:
	int symbol_id_{ 0 };
	VmQuote quote_;
	void subscribe_quote_control();
	int id_;
	SymbolOrderView* symbol_order_view_{ nullptr };
	SymbolInfoView* symbol_info_view_{ nullptr };
	DmFutureView* dm_future_view_{ nullptr };
	DmOptionView* dm_option_view_{ nullptr };
};
}

