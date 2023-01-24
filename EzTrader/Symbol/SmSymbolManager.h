#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <map>
#include <set>
#include <vector>

namespace DarkHorse {
	class SmSymbol;
	class SmMarket;
	class SmProduct;
	class SmSymbolManager
	{
	public:
		SmSymbolManager();
		~SmSymbolManager();
		// Static Members
		static int _Id;
		static int GetId() { return ++_Id; }
	private:
		/// <summary>
		/// Symbol Map.
		/// Key : Symbol Code, value : Symbol Object.
		/// </summary>
		std::map<std::string, std::shared_ptr<SmSymbol>> SymbolMap_;
		// key : unique symbol id, value : SmSymbol Object
		std::map<int, std::shared_ptr<SmSymbol>> _SymbolIdMap;
		// key : market name, value : Market Object
		std::map<std::string, std::shared_ptr<SmMarket>> _MarketMap;
		std::set<std::string> _RegisteredSymbolMap;
		// key : symbol unique id, value : SmSymbol Object
		std::map<int, std::shared_ptr<SmSymbol>> _FavoriteMap;
		std::set<std::string> _FavoriteProduct;
		// key : domestic product code, value : symbol code list
		std::map<std::string, std::vector<std::string>> _DomesticSymbolCodeMap;
		std::vector<std::string> _DomesticProductVec;
	private:
		void InitDomesticProducts();
	public:
		const std::vector<std::string> GetDomesticProductVec() {
			return _DomesticProductVec;
		}
		void AddDomesticSymbolCode(const std::string& product_code, const std::string& symbol_code);
		void MakeFavorite();
		void InitFavoriteProduct();
		void Clear() {
			
		}
		const std::map<int, std::shared_ptr<SmSymbol>>& GetFavoriteMap() {
			return _FavoriteMap;
		}
		void AddFavorite(const int& symbol_id);
		void RemoveFavorite(const std::string& symbol_code);
		std::shared_ptr<SmSymbol> FindSymbolById(const int& id);
		void AddSymbol(std::shared_ptr<SmSymbol> symbol);
		std::shared_ptr<SmSymbol> FindSymbol(const std::string& symbol_code) const;
		void GetRecentSymbolVector(std::vector<std::shared_ptr<SmSymbol>>& symbol_list);
		std::shared_ptr<SmSymbol> GetRecentSymbol(const std::string& product_code);
		std::string GetRecentSymbolCode(const std::string& product_code);
		std::string GetNextSymbolCode(const std::string& product_code);
		const std::map<std::string, std::shared_ptr<SmMarket>>& GetMarketMap() const { return _MarketMap; }
		std::shared_ptr<SmProduct> FindProduct(const std::string& product_code);
		std::shared_ptr<SmMarket> AddMarket(const std::string& market_name);
		std::shared_ptr<SmMarket> FindMarket(const std::string& market_name);
		void ReadAbroadSymbols() const noexcept;
		void MakeDomesticMarket();
		void RegisterSymbolToServer(const std::string& symbol_code, const bool& reg);
	};
}

