#include "stdafx.h"
#include "ProductControl.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../View/SymbolOrderView.h"
#include "../Symbol/SmSymbol.h"
#include <functional>

namespace DarkHorse {
	ProductControl::ProductControl()
		: id_(IdGenerator::get_id())
	{
		subscribe_product_control();
	}
	void ProductControl::subscribe_product_control()
	{
		/*
		mainApp.event_hub()->subscribe_product_event_handler
		(
			id_,
			std::bind(&ProductControl::update_product, this, std::placeholders::_1)
		);
		*/
	}
	void ProductControl::update_product(std::shared_ptr<SmSymbol> symbol)
	{
		if (!symbol) return;

		product_.decimal = symbol->Decimal();
		//product_.hoga_unit = symbol->hoga_unit;
		//product_.int_tick_size = symbol->TickSize();
		//product_.tick_size = symbol->TickSize();
		product_.seung_su = symbol->SeungSu();
		product_.tick_value = (int)symbol->TickValue();

	}
}