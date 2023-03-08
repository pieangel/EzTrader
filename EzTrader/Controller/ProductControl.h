#pragma once
#include "../ViewModel/VmProduct.h"
#include "../Util/IdGenerator.h"

#include <memory>
class SymbolOrderView;
namespace DarkHorse {
	class SmSymbol;
	class ProductControl
	{
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
	private:
		void subscribe_product_control();
		VmProduct product_;
		int id_;
		SymbolOrderView* symbol_order_view_{ nullptr };
	};
}
