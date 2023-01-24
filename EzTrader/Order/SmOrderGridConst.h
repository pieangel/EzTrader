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
		{OrderGridHeader::SELL_ORDER, "�ֹ�", 55},
		{OrderGridHeader::SELL_CNT, "�Ǽ�", 35},
		{OrderGridHeader::SELL_QTY, "�ܷ�", 35},
		//{OrderGridHeader::SELL_FILLED, "ü��", 35},
		{OrderGridHeader::QUOTE, "�ü�", 80},
		//{OrderGridHeader::BUY_FILLED, "ü��", 35},
		{OrderGridHeader::BUY_QTY, "�ܷ�", 35},
		{OrderGridHeader::BUY_CNT, "�Ǽ�", 35},
		{OrderGridHeader::BUY_ORDER, "�ֹ�", 55},
		{OrderGridHeader::BUY_STOP, "STOP", 40}
	};
}