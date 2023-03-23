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

	int ProductControl::get_next_up_value(const int value)
	{
		if (value_type_ == ValueType::None) return value;

		if (value_type_ == ValueType::KospiOption ||
			value_type_ == ValueType::KospiWeekly) {
			if (value >= 1000) return value + 5;
			else return value + 1;
		}
		else if (value_type_ == ValueType::MiniKospiOption) {
			if (value >= 300) return value + 2;
			else return value + 1;
		}
		else if (value_type_ == ValueType::Kosdaq) {
			return value + 10;
		}
		else {
			return value + product_.int_tick_size;
		}
	}

	int ProductControl::get_next_down_value(const int value)
	{
		if (value_type_ == ValueType::None) return value;

		if (value_type_ == ValueType::KospiOption ||
			value_type_ == ValueType::KospiWeekly) {
			if (value <= 1000) return value - 1;
			else return value - 5;
		}
		else if (value_type_ == ValueType::MiniKospiOption) {
			if (value <= 300) return value - 1;
			else return value - 2;
		}
		else if (value_type_ == ValueType::Kosdaq) {
			return value - 10;
		}
		else {
			return value - product_.int_tick_size;
		}
	}

	int ProductControl::get_row(const int source_value, const int base_row, const int base_value)
	{
		int target_row = 0;

		if (value_type_ == ValueType::None) return source_value;

		if (value_type_ == ValueType::KospiOption ||
			value_type_ == ValueType::KospiWeekly) {
			//if (source_value <= 1000) return value - 1;
			//else return value - 5;
			//if (source_value > base_value)
			//if (base_value )
		}
		else if (value_type_ == ValueType::MiniKospiOption) {
			//if (value <= 300) return value - 1;
			//else return value - 2;
		}
		else if (value_type_ == ValueType::Kosdaq) {
			//return value - 10;
		}
		else {
			//return value - product_.int_tick_size;
		}

		return target_row;
	}

	int ProductControl::get_value(const int source_row, const int base_row, const int base_value)
	{
		int target_value = 0;

		return target_value;
	}

	void ProductControl::set_value_type(const std::string& symbol_code)
	{
		if (symbol_code.empty()) return;

		if (symbol_code.find("201") != std::string::npos ||
			symbol_code.find("301") != std::string::npos) 
			value_type_ = ValueType::KospiOption;
		else if (symbol_code.find("209") != std::string::npos ||
			symbol_code.find("309") != std::string::npos)
			value_type_ = ValueType::KospiWeekly;
		else if (symbol_code.find("205") != std::string::npos ||
			symbol_code.find("305") != std::string::npos)
			value_type_ = ValueType::MiniKospiOption;
		else if (symbol_code.find("206") != std::string::npos ||
			symbol_code.find("306") != std::string::npos)
			value_type_ = ValueType::Kosdaq;
		else
			value_type_ = ValueType::Future;
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
		product_.tick_size = symbol->TickSize();
		product_.int_tick_size = static_cast<int>(symbol->TickSize() * pow(10, symbol->Decimal()));
		product_.seung_su = symbol->SeungSu();
		product_.tick_value = static_cast<int>(symbol->TickValue());
		set_value_type(symbol->SymbolCode());
	}
}