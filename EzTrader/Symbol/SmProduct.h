#pragma once
#include <string>
#include <memory>
#include <vector>

namespace DarkHorse {
	class SmSymbol;
	class SmProduct
	{
	public:
		const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& GetSymbolVec() const { return _SymbolVec; }
		std::shared_ptr<DarkHorse::SmSymbol> AddSymbol(std::string&& symbol_code);
		
		SmProduct(std::string&& product_code) : _ProductCode(product_code) {}
		std::string ProductCode() const { return _ProductCode; }
		void ProductCode(std::string val) { _ProductCode = val; }
		std::string ProductNameEn() const { return _ProductNameEn; }
		void ProductNameEn(std::string val) { _ProductNameEn = val; }
		std::string ProductNameKr() const { return _ProductNameKr; }
		void ProductNameKr(std::string val) { _ProductNameKr = val; }
		std::string MarketName() const { return _MarketName; }
		void MarketName(std::string val) { _MarketName = val; }
		std::string ExchangeName() const { return _ExchangeName; }
		void ExchangeName(std::string val) { _ExchangeName = val; }
	private:
		std::string _ProductCode;
		std::string _ProductNameEn;
		std::string _ProductNameKr;
		std::string _MarketName;
		std::string _ExchangeName;
		std::vector<std::shared_ptr<DarkHorse::SmSymbol>> _SymbolVec;
	};
}

