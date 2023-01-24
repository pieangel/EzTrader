#include "stdafx.h"
#include "SmHogaProcessor.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Log/MyLogger.h"
#include "../Event/SmCallbackManager.h"

using namespace DarkHorse;

SmHogaProcessor::SmHogaProcessor()
{
	StartProcess();
}

SmHogaProcessor::~SmHogaProcessor()
{
	StopProcess();
}

void SmHogaProcessor::StartProcess() noexcept
{
	start();
}

void SmHogaProcessor::StopProcess() noexcept
{
	if (!m_runMode) return;
	// 먼저 큐를 비운다.
	ClearTasks();
	m_stop = true;
	nlohmann::json empty_hoga;
	empty_hoga["symbol_code"] = "";
	_HogaQueue.add(std::move(empty_hoga));
	// 쓰레드가 끝날때까지 기다린다.
	if (m_thread.joinable())
		m_thread.join();
	if (m_runMode)
		m_runMode = false;
}

unsigned int SmHogaProcessor::ThreadHandlerProc(void)
{
	while (true) {
		// 종료 신호를 보내면 루프를 나간다.
		if (isStop()) {
			break;
		}
		nlohmann::json hoga;
		_HogaQueue.take(hoga);
		ProcessHoga(std::move(hoga));
	}

	return 1;
}

void SmHogaProcessor::AddHoga(nlohmann::json&& hoga) noexcept
{
	if (!_Enable) return;

	_HogaQueue.try_add(hoga);
}

void SmHogaProcessor::ProcessHoga(nlohmann::json&& hoga)
{
	try {
		const std::string& symbol_code = hoga["symbol_code"];
		std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
		if (!symbol) return;

		for (int i = 0; i < 5; i++) {
			symbol->Hoga.Ary[i].SellPrice = hoga["hoga_items"][i]["sell_price"];
			symbol->Hoga.Ary[i].BuyPrice = hoga["hoga_items"][i]["buy_price"];
			symbol->Hoga.Ary[i].SellQty = hoga["hoga_items"][i]["sell_qty"];
			symbol->Hoga.Ary[i].BuyQty = hoga["hoga_items"][i]["buy_qty"];
			symbol->Hoga.Ary[i].SellCnt = hoga["hoga_items"][i]["sell_cnt"];
			symbol->Hoga.Ary[i].BuyCnt = hoga["hoga_items"][i]["buy_cnt"];
		}

		symbol->Hoga.HogaTime = hoga["hoga_time"];
		symbol->Hoga.TotBuyQty = hoga["tot_buy_qty"];
		symbol->Hoga.TotSellQty = hoga["tot_sell_qty"];
		symbol->Hoga.TotBuyCnt = hoga["tot_buy_cnt"];
		symbol->Hoga.TotSellCnt = hoga["tot_sell_cnt"];

		
		//mainApp.CallbackMgr()->OnWndHogaEvent(symbol->Id());
		mainApp.CallbackMgr()->OnHogaEvent(symbol_code);

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

bool SmHogaProcessor::ProcessHoga(const std::array<nlohmann::json, BulkHogaSize>& arr, const int& taken)
{

	return true;
}

void DarkHorse::SmHogaProcessor::ClearTasks() noexcept
{
	_HogaQueue.flush();
}
