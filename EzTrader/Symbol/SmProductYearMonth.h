#pragma once
#include <string>
#include <memory>
#include <vector>
namespace DarkHorse {
	class SmSymbol;
	class SmProductYearMonth
	{
	public:
		SmProductYearMonth(const std::string& name) : _Name(name) {};
		~SmProductYearMonth() {};
		void AddSymbol(std::shared_ptr<DarkHorse::SmSymbol> symbol) {
			_SymbolVec.push_back(symbol);
		}
		const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& get_symbol_vector()
		{
			return _SymbolVec;
		}
		const std::string& get_name() {
			return _Name;
		}
		void sort_symbol_vector();
	private:
		std::string _Name;
		std::vector<std::shared_ptr<DarkHorse::SmSymbol>> _SymbolVec;
	};
}

