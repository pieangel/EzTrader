#pragma once
#include "../ViewModel/VmProduct.h"
#include "../Util/IdGenerator.h"

#include <memory>
class SymbolOrderView;
namespace DarkHorse {
	class SmSymbol;
	class ProductControl
	{
		enum class ValueType {
			None,
			Future,
			KospiOption,
			MiniKospiOption,
			Kosdaq,
			KospiWeekly
		};

	public:
		ProductControl();
		~ProductControl() {};
		void update_product(std::shared_ptr<SmSymbol> symbol);
		const VmProduct& get_product()
		{
			return product_;
		}
		const int get_id()
		{
			return id_;
		}
		void symbol_order_view(SymbolOrderView* symbol_order_view_p) {
			symbol_order_view_ = symbol_order_view_p;
		}
		int get_next_up_value(const int value);
		int get_next_down_value(const int value);
	private:
		void set_value_type(const std::string& symbol_code);
		void subscribe_product_control();
		VmProduct product_;
		int id_;
		ValueType value_type_ = ValueType::None;
		SymbolOrderView* symbol_order_view_{ nullptr };
	};
}
