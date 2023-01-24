#include "stdafx.h"
#include "SmSymbol.h"
#include "../Chart/SmChartData.h"
using namespace DarkHorse;


void DarkHorse::SmSymbol::ShiftDown(SmTick&& newValue)
{
	//SmTick* data = TickVec.data();
	//const size_t len = TickVec.size();
	//memmove(data + 1, data, sizeof(*data) * (len - 1));
	std::rotate(TickVec.rbegin(), TickVec.rbegin() + 1, TickVec.rend());
	TickVec[0] = std::move(newValue);
}

DarkHorse::SmSymbol::~SmSymbol()
{
	int i = 0; 
	i = i + 0;

	CString msg;
	msg.Format("SymbolCode = %s\n", SymbolCode().c_str());
	//TRACE(msg);
}

void DarkHorse::SmSymbol::AddChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data)
{
	if (!chart_data) return;

	_ChartDataMap[chart_data->Id()] = chart_data;
}

void DarkHorse::SmSymbol::UpdateChartData(DarkHorse::SmTick&& tick_data)
{
	for (auto it = _ChartDataMap.begin(); it != _ChartDataMap.end(); it++) {
		it->second->OnTickData(tick_data);
	}
}
