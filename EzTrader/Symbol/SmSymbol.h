#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../Quote/SmQuote.h"
#include "../Hoga/SmHoga.h"
namespace DarkHorse {
	class SmChartData;
	class SmSymbol
	{
	public:
		// shared_ptr에서 static 함수는 쓰레드에서 사용하지 않는게 좋다. 
		// 쓰레드에서 사용하고 있을 경우 shared_ptr은 메모리에서 해제되지 않고 두번 해제되는등 
		// 예상치 못한 행동을 할 수 있다. static 함수를 사용에 신중하길 바란다.
		//static void ShiftData(SmTick* data, const int& len, SmTick&& newValue);
		void ShiftDown(SmTick&& newValue);
		SmSymbol(std::string&& symbol_code) : _SymbolCode(symbol_code) { TickVec.resize(26); }
		~SmSymbol();
		std::string SymbolCode() const { return _SymbolCode; }
		void SymbolCode(std::string val) { _SymbolCode = val; }
		std::string MarketName() const { return _MarketName; }
		void MarketName(std::string val) { _MarketName = val; }
		std::string ProductCode() const { return _ProductCode; }
		void ProductCode(std::string val) { _ProductCode = val; }
		int SeungSu() const { return _SeungSu; }
		void SeungSu(int val) { _SeungSu = val; }
		double CtrtSize() const { return _CtrtSize; }
		void CtrtSize(double val) { _CtrtSize = val; }
		double TickValue() const { return _TickValue; }
		void TickValue(double val) { _TickValue = val; }
		double TickSize() const { return _TickSize; }
		void TickSize(double val) { _TickSize = val; }
		int Decimal() const { return _Decimal; }
		void Decimal(int val) { _Decimal = val; }
		std::string ExpireDate() const { return _ExpireDate; }
		void ExpireDate(std::string val) { _ExpireDate = val; }
		std::string SymbolNameKr() const { return _SymbolNameKr; }
		void SymbolNameKr(std::string val) { _SymbolNameKr = val; }
		std::string SymbolNameEn() const { return _SymbolNameEn; }
		void SymbolNameEn(std::string val) { _SymbolNameEn = val; }
		int Id() const { return _Id; }
		void Id(int val) { _Id = val; }
		std::string StartTime() const { return _StartTime; }
		void StartTime(std::string val) { _StartTime = val; }
		std::string EndTime() const { return _EndTime; }
		void EndTime(std::string val) { _EndTime = val; }
		int TotalVolume() const { return _TotalVolume; }
		void TotalVolume(int val) { _TotalVolume = val; }
		
		
		std::string Deposit() const { return _Deposit; }
		void Deposit(std::string val) { _Deposit = val; }
		std::string Currency() const { return _Currency; }
		void Currency(std::string val) { _Currency = val; }
		std::string Exchange() const { return _Exchange; }
		void Exchange(std::string val) { _Exchange = val; }
		std::string PreDayRate() const { return _PreDayRate; }
		void PreDayRate(std::string val) { _PreDayRate = val; }
		int PreDayVolume() const { return _PreDayVolume; }
		void PreDayVolume(int val) { _PreDayVolume = val; }

	private:
		int _Id{ 0 };
		int _SeungSu{ 250000 };
		int _Decimal{ 2 };
		double _CtrtSize{ 0.05 };
		double _TickValue{ 12500 };
		double _TickSize{ 0.05 };
		std::string _SymbolNameKr;
		std::string _SymbolNameEn;
		std::string _SymbolCode;
		std::string _MarketName;
		std::string _ProductCode;
		std::string _ExpireDate;
		int _TotalVolume{ 0 };
		int _PreDayVolume{ 0 };

		std::string _Deposit;
		
		std::string _StartTime;
		std::string _EndTime;
		std::string _PreDayRate;
		std::string _Currency;
		std::string _Exchange;
		std::map<int, std::shared_ptr<DarkHorse::SmChartData>> _ChartDataMap;
	public:
		void AddChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data);
		void UpdateChartData(DarkHorse::SmTick&& tick_data);
		SmHoga Hoga;
		SmQuote Qoute;
		std::vector<SmTick> TickVec;
	};
}

