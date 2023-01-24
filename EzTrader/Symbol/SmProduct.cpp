#include "stdafx.h"
#include "SmProduct.h"
#include "SmSymbol.h"
#include "SmSymbolManager.h"
#include "../Global/SmTotalManager.h"
#include "../Log/MyLogger.h"
using namespace DarkHorse;

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
		LOGINFO(CMyLogger::getInstance(), "error = %s",	error);
	}

	return nullptr;
}
