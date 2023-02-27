#include "stdafx.h"
#include "SmProduct.h"
#include "SmSymbol.h"
#include "SmSymbolManager.h"
#include "SmProductYearMonth.h"
#include "../Global/SmTotalManager.h"
#include "../Log/MyLogger.h"
namespace DarkHorse {

std::shared_ptr<DarkHorse::SmSymbol> SmProduct::AddSymbol(std::string&& symbol_code)
{
	try {
		auto symbol = std::make_shared<DarkHorse::SmSymbol>(std::move(symbol_code));
		if (!symbol) return nullptr;

		symbol->Id(SmSymbolManager::GetId());
		_SymbolVec.push_back(symbol);
		mainApp.SymMgr()->AddSymbol(symbol);
		return symbol;
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error);
	}

	return nullptr;
}

std::shared_ptr<SmProductYearMonth> SmProduct::add_year_month(const std::string& year_month_name)
{
	auto it = _YearMonthMap.find(year_month_name);
	if (it != _YearMonthMap.end()) return it->second;
	return std::make_shared<SmProductYearMonth>(year_month_name);
}
}
