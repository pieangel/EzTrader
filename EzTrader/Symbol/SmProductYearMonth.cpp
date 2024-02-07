#include "stdafx.h"
#include "SmProductYearMonth.h"
#include "SmSymbol.h"
#include <algorithm>
#include "../Log/MyLogger.h"

namespace DarkHorse {

void SmProductYearMonth::sort_symbol_vector()
{
	if (_SymbolVec.size() == 0) return;

	//std::sort(_SymbolVec.begin(), _SymbolVec.end(), DarkHorse::strike_less_than_key());

	std::sort(_SymbolVec.begin(), _SymbolVec.end(), [](const std::shared_ptr<SmSymbol>& lhs, const std::shared_ptr<SmSymbol>& rhs)
	{
		return std::stoi(lhs->Strike()) < std::stoi(rhs->Strike());
	});

	for (auto symbol : _SymbolVec) {
		LOGINFO(CMyLogger::getInstance(), "symbol_code[%s], strike[%s]", symbol->SymbolCode().c_str(), symbol->Strike().c_str());
	}
}
}
