#pragma once
#include <map>
#include <string>
#include "Symbol.h"
namespace DarkHorse {
	class SymbolManager
	{
	public:
		static int createId() { return ++_id; }
	private:
		static int _id;
	public:
		void readDomesticMasterFile();
		void addSymbol(Symbol&& symbol);
	private:
		std::map<std::string, Symbol> _SymbolMap;
	};
}

