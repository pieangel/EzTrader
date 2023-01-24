#pragma once
#include <string>
#include <vector>

namespace DarkHorse {
	enum Hoga_Type {
		SELL = 0,
		BUY
	};

	enum OrderGridHeader 
	{
		SELL_STOP = 0,
		SELL_ORDER,
		SELL_CNT,
		SELL_QTY,
		//SELL_FILLED,
		QUOTE,
		//BUY_FILLED,
		BUY_QTY,
		BUY_CNT,
		BUY_ORDER,
		BUY_STOP
	};

	typedef struct _OrderGridHeaderInfo 
	{
		OrderGridHeader index;
		std::string title;
		int width;
	} OrderGridHeaderInfo;

	static std::vector<OrderGridHeaderInfo> OrderGridHeaderVector 
	{
		{OrderGridHeader::SELL_STOP, "STOP", 40},
		{OrderGridHeader::SELL_ORDER, "주문", 55},
		{OrderGridHeader::SELL_CNT, "건수", 35},
		{OrderGridHeader::SELL_QTY, "잔량", 35},
		//{OrderGridHeader::SELL_FILLED, "체결", 35},
		{OrderGridHeader::QUOTE, "시세", 80},
		//{OrderGridHeader::BUY_FILLED, "체결", 35},
		{OrderGridHeader::BUY_QTY, "잔량", 35},
		{OrderGridHeader::BUY_CNT, "건수", 35},
		{OrderGridHeader::BUY_ORDER, "주문", 55},
		{OrderGridHeader::BUY_STOP, "STOP", 40}
	};
}