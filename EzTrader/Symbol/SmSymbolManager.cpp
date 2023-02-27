#include "stdafx.h"
#include "SmSymbolManager.h"
#include "SmMarket.h"
#include "SmProduct.h"
#include "../Global/SmTotalManager.h"
#include "SmSymbolReader.h"
#include "SmSymbol.h"
#include "../Task/SmTaskRequestMaker.h"
#include "../Task/SmServerDataReceiver.h"
#include "../Client/ViStockClient.h"
#include "../Config/SmConfigManager.h"
#include "../Log/MyLogger.h"
#include "../Util/VtStringUtil.h"
using namespace DarkHorse;
int SmSymbolManager::_Id = 0;

std::shared_ptr<SmMarket> SmSymbolManager::AddMarket(const std::string& market_name)
{
	const auto found = _MarketMap.find(market_name);
	if (found != _MarketMap.end()) return found->second;
	else {
		auto market = std::make_shared<SmMarket>(market_name);
		_MarketMap[market_name] = market;
		return market;
	}
}

SmSymbolManager::SmSymbolManager()
{
	InitFavoriteProduct();
	InitDomesticProducts();
}

SmSymbolManager::~SmSymbolManager()
{
	
}

void DarkHorse::SmSymbolManager::InitDomesticProducts()
{
	_DomesticProductVec.push_back("101F");
}

void SmSymbolManager::ParseYearMonth(
	const std::string product_code, 
	const std::string symbol_name_en, 
	std::shared_ptr<SmSymbol>)
{
	if (product_code == "101") { // KOSPI 200 F 202303
		;
	}
	else if (product_code == "105") { // MINI KOSPI F 202303
		;
	}
	else if (product_code == "106") { // KOSDAQ150 F 202303
		;
	}
	else if (product_code == "167") { // KTB10      F 202303
		;
	}
	else if (product_code == "175") { // USD F 202303
		;
	}
	else if (product_code == "201") { // KOSPI 200 C 202303 160.0 
		;
	}
	else if (product_code == "301") { // KOSPI 200 P 202303 340.0
		;
	}
	else if (product_code == "205") { // MINI KOSPI C 202303 200.0 
		;
	}
	else if (product_code == "305") { // MINI KOSPI P 202303 200.0
		;
	}
	else if (product_code == "209") { // KOSPI WEEKLY C 2303W1 275.0
		;
	}
	else if (product_code == "309") { // KOSPI WEEKLY P 2303W1 337.5
		;
	}
	else if (product_code == "206") { // KOSDAQ150 C 202303 1,275
		;
	}
	else if (product_code == "306") { // KOSDAQ150 P 202303 1,275
		;
	}
}

void SmSymbolManager::read_domestic_masterfile()
{
	try {
		std::string file_path;
		file_path = SmConfigManager::GetApplicationPath();
		file_path.append(_T("\\"));
		file_path.append(_T("mst"));
		file_path.append(_T("\\"));
		std::string file_name = "chocode.cod";
		//TRACE(file_name.c_str());
		std::string full_name = file_path + file_name;
		std::ifstream infile(full_name);
		std::string line;
		int index = 0;
		std::string value;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);
			
			value = line.substr(index, 9); index += 9;
			VtStringUtil::trim(value);
			std::string symbol_code = value;
			std::shared_ptr<SmSymbol> symbol = std::make_shared<SmSymbol>(std::move(symbol_code));
			symbol->SymbolCode(value);
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol->FullCode(value);
			value = line.substr(index, 30); index += 30;
			VtStringUtil::trim(value);
			symbol->SymbolNameKr(value);
			value = line.substr(index, 30); index += 30;
			VtStringUtil::trim(value);
			symbol->SymbolNameEn(value);
			value = line.substr(index, 5); index += 5;
			VtStringUtil::trim(value);
			symbol->RemainDays(_ttoi(value.c_str()));
			value = line.substr(index, 8); index += 8;
			VtStringUtil::trim(value);
			symbol->LastTradeDay(value);
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol->HighLimitPrice(value);
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol->LowLimitPrice(value);
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol->PreDayClose(value);
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol->StandardPrice(value);
			value = line.substr(index, 17); index += 17;
			VtStringUtil::trim(value);
			symbol->Strike(value);
			value = line.substr(index, 1); index += 1;
			VtStringUtil::trim(value);
			symbol->AtmType(_ttoi(value.c_str()));
			value = line.substr(index, 1); index += 1;
			VtStringUtil::trim(value);
			symbol->RecentMonth(_ttoi(value.c_str()));
			value = line.substr(index, 8);
			symbol->ExpireDate(value);
			AddSymbol(symbol);
			index = 0;
			LOGINFO(CMyLogger::getInstance(), "read symbol %s complete!", symbol->SymbolCode().c_str());
		}

		LOGINFO(CMyLogger::getInstance(), "read %s file complete!", full_name.c_str());
	}
	catch (std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}

void DarkHorse::SmSymbolManager::AddDomesticSymbolCode(const std::string& product_code, const std::string& symbol_code)
{
	auto found = _DomesticSymbolCodeMap.find(product_code);
	if (found != _DomesticSymbolCodeMap.end()) {
		std::vector<std::string>& symbol_code_vec = found->second;
		symbol_code_vec.push_back(symbol_code);
	}
	else {
		std::vector<std::string> symbol_code_vec;
		symbol_code_vec.push_back(symbol_code);
		_DomesticSymbolCodeMap[product_code] = std::move(symbol_code_vec);
	}
}

void DarkHorse::SmSymbolManager::MakeFavorite()
{
	for (auto it = _FavoriteProduct.begin(); it != _FavoriteProduct.end(); it++) {
		std::string product_code = *it;
		std::shared_ptr<SmProduct> product = FindProduct(product_code);
		if (!product) continue;
		const auto& symbol = GetRecentSymbol(product_code);
		if (symbol) _FavoriteMap[symbol->Id()] = symbol;
	}
}

void DarkHorse::SmSymbolManager::InitFavoriteProduct()
{
	std::string section = _T("FAVORITE");
	std::string favorite;
	std::string name = _T("SYMBOL");
	//favorite = mainApp.ConfigMgr().getString(section, name);
	if (favorite.length() == 0) {
		_FavoriteProduct.insert("101");
		_FavoriteProduct.insert("HSI");
		_FavoriteProduct.insert("CL");
		_FavoriteProduct.insert("MNQ");
		_FavoriteProduct.insert("CN");
		_FavoriteProduct.insert("GC");
		_FavoriteProduct.insert("NKD");
		_FavoriteProduct.insert("NQ");
	}
}

void DarkHorse::SmSymbolManager::AddFavorite(const int& symbol_id)
{
	auto symbol = FindSymbolById(symbol_id);
	if (!symbol) return;
	_FavoriteMap[symbol_id] = symbol;
}

void DarkHorse::SmSymbolManager::RemoveFavorite(const std::string& symbol_code)
{
	auto symbol = FindSymbol(symbol_code);
	if (!symbol) return;
	auto found = _FavoriteMap.find(symbol->Id());
	if (found != _FavoriteMap.end()) {
		_FavoriteMap.erase(found);
	}
}

std::shared_ptr<DarkHorse::SmSymbol> DarkHorse::SmSymbolManager::FindSymbolById(const int& id)
{
	auto found = _SymbolIdMap.find(id);
	if (found != _SymbolIdMap.end())
		return found->second;
	else
		return nullptr;
}

void DarkHorse::SmSymbolManager::AddSymbol(std::shared_ptr<SmSymbol> symbol)
{
	if (!symbol) return;
	SymbolMap_[symbol->SymbolCode()] = symbol;
	_SymbolIdMap[symbol->Id()] = symbol;
}

std::shared_ptr<DarkHorse::SmSymbol> DarkHorse::SmSymbolManager::FindSymbol(const std::string& symbol_code) const
{
	const auto it = SymbolMap_.find(symbol_code);
	if (it != SymbolMap_.end())
		return it->second;
	else
		return nullptr;
}

void DarkHorse::SmSymbolManager::GetRecentSymbolVector(std::vector<std::shared_ptr<SmSymbol>>& symbol_list)
{
	for (auto it = _MarketMap.begin(); it != _MarketMap.end(); ++it) {
		const auto market = it->second;
		const std::map<std::string, std::shared_ptr<DarkHorse::SmProduct>>& product_map = market->GetProductMap();
		for (auto it2 = product_map.begin(); it2 != product_map.end(); ++it2) {
			const auto product = it2->second;
			const auto symbol = GetRecentSymbol(product->ProductCode());
			if (!symbol) continue;
			symbol_list.push_back(symbol);
		}
	}
}

std::shared_ptr<DarkHorse::SmSymbol> SmSymbolManager::GetRecentSymbol(const std::string& product_code)
{
	std::shared_ptr<DarkHorse::SmProduct> product = FindProduct(product_code);

	const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec = product->GetSymbolVec();
	if (symbol_vec.empty()) return nullptr;
	int preday_volume = symbol_vec.front()->PreDayVolume();
	std::shared_ptr<DarkHorse::SmSymbol> symbol = symbol_vec.front();
	for (size_t i = 0; i < symbol_vec.size(); i++) {
		if (symbol_vec[i]->PreDayVolume() > preday_volume) {
			symbol = symbol_vec[i];
			preday_volume = symbol_vec[i]->PreDayVolume();
		}
		// 차월물 중에 거래량이 제일 많은 것을 검색하지만 근월물로부터 4번째까지만 검사하고 더이상 검사하지 않는다. 
		if (i > 3) break;
	}
	return symbol;
}

std::string DarkHorse::SmSymbolManager::GetRecentSymbolCode(const std::string& product_code)
{
	std::shared_ptr<DarkHorse::SmProduct> product = FindProduct(product_code);

	const auto& symbol = GetRecentSymbol(product->ProductCode());
	if (symbol) return symbol->SymbolCode();
	return "";
}

std::string SmSymbolManager::GetNextSymbolCode(const std::string& product_code)
{
	std::string symbol_code("");

	std::shared_ptr<DarkHorse::SmProduct> product = FindProduct(product_code);

	const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec = product->GetSymbolVec();
	if (symbol_vec.empty() || symbol_vec.size() == 1) symbol_code;

	return (* (symbol_vec.begin() + 1))->SymbolCode();
}

std::shared_ptr<DarkHorse::SmProduct> DarkHorse::SmSymbolManager::FindProduct(const std::string& product_code)
{
	for(auto it = _MarketMap.begin(); it != _MarketMap.end(); ++it) {
		const auto found = it->second->FindProduct(product_code);
		if (found) return found;
	}

	return nullptr;
}

std::shared_ptr<DarkHorse::SmMarket> DarkHorse::SmSymbolManager::FindMarket(const std::string& market_name)
{
	auto found = _MarketMap.find(market_name);
	if (found != _MarketMap.end())
		return found->second;
	else
		return nullptr;
}

void DarkHorse::SmSymbolManager::ReadAbroadSymbols() const noexcept
{
	mainApp.SymRdr()->ReadAbroadMarketFile();
	Sleep(500);
	mainApp.SymRdr()->ReadAbroadSymbolFile();
}

void DarkHorse::SmSymbolManager::MakeDomesticMarket()
{
	std::string market_name = "국내선물";
	std::shared_ptr<SmMarket> market = mainApp.SymMgr()->AddMarket(market_name);
	std::shared_ptr<SmProduct> product = market->AddProduct("101");
	product->MarketName(market_name);
	product = market->AddProduct("105");
	product->MarketName(market_name);
	product = market->AddProduct("106");
	product->MarketName(market_name);
	product = market->AddProduct("167");
	product->MarketName(market_name);
	product = market->AddProduct("175");
	product->MarketName(market_name);
	market_name = "국내옵션";
	market = mainApp.SymMgr()->AddMarket(market_name);
	product = market->AddProduct("201");
	product->MarketName(market_name);
	product = market->AddProduct("301");
	product->MarketName(market_name);

	product = market->AddProduct("205");
	product->MarketName(market_name);
	product = market->AddProduct("305");
	product->MarketName(market_name);

	product = market->AddProduct("209");
	product->MarketName(market_name);
	product = market->AddProduct("309");
	product->MarketName(market_name);

	product = market->AddProduct("206");
	product->MarketName(market_name);
	product = market->AddProduct("306");
	product->MarketName(market_name);
}

void DarkHorse::SmSymbolManager::RegisterSymbolToServer(const std::string& symbol_code, const bool& reg)
{
	if (reg) {
		auto found = _RegisteredSymbolMap.find(symbol_code);
		if (found != _RegisteredSymbolMap.end()) return;

		mainApp.Client()->RegisterSymbol(symbol_code);
	}
	else {
		auto found = _RegisteredSymbolMap.find(symbol_code);
		if (found == _RegisteredSymbolMap.end()) return;
	}
}
