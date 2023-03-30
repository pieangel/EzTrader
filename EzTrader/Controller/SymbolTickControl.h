#pragma once
#include "../Quote/SmQuote.h"
#include <vector>

class SymbolTickView;
namespace DarkHorse {
	class SymbolTickControl
	{
	public:
		SymbolTickControl();
		~SymbolTickControl();
		void update_tick(SmTick tick);
		
		const int get_id()
		{
			return id_;
		}
		void symbol_tick_view(SymbolTickView* symbol_tick_view_p) {
			symbol_tick_view_ = symbol_tick_view_p;
		}
		void set_symbol_id(const int symbol_id) {
			symbol_id_ = symbol_id;
		}
		void set_symbol_decimal(const int symbol_decimal) {
			symbol_decimal_ = symbol_decimal;
		}
		const std::vector<SmTick>& get_tick_vec() {
			return tick_vec_;
		}
		int get_symbol_decimal() {
			return symbol_decimal_;
		}
	private:
		int symbol_decimal_{ 0 };
		std::vector<SmTick> tick_vec_;
		int symbol_id_{ 0 };
		void subscribe_tick_control();
		int id_;
		SymbolTickView* symbol_tick_view_{ nullptr };
	};
}
