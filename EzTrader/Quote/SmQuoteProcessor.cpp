#include "stdafx.h"
#include "SmQuoteProcessor.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Log/MyLogger.h"
#include "../MessageDefine.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Event/SmCallbackManager.h"

using namespace DarkHorse;

SmQuoteProcessor::SmQuoteProcessor() 
{
	StartProcess();
}

SmQuoteProcessor::~SmQuoteProcessor()
{
	StopProcess();
}

void SmQuoteProcessor::StartProcess() noexcept
{
	start();
}

void SmQuoteProcessor::StopProcess() noexcept
{
	if (!m_runMode) return;
	// 먼저 큐를 비운다.
	ClearTasks();
	m_stop = true;
	nlohmann::json empty_quote;
	empty_quote["symbol_code"] = "";
	_QuoteQueue.add(std::move(empty_quote));
	// 쓰레드가 끝날때까지 기다린다.
	if (m_thread.joinable())
		m_thread.join();
	if (m_runMode)
		m_runMode = false;
}

unsigned int SmQuoteProcessor::ThreadHandlerProc(void)
{
	while (true) {
		// 종료 신호를 보내면 루프를 나간다.
		if (isStop()) {
			break;
		}
		nlohmann::json quote;
		_QuoteQueue.take(quote);
		ProcessQuote(std::move(quote));
	}

	return 1;
}

void SmQuoteProcessor::AddQuote(nlohmann::json&& quote) noexcept
{
	if (!_Enable) return;

	_QuoteQueue.try_add(quote);
}

void SmQuoteProcessor::ProcessQuote(nlohmann::json&& quote)
{
	try {
		
		const std::string& symbol_code = quote["symbol_code"];
		std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
		if (!symbol) return;

		symbol->Qoute.open = quote["open"];
		symbol->Qoute.high = quote["high"];
		symbol->Qoute.low = quote["low"];
		symbol->Qoute.close = quote["close"];
		symbol->TotalVolume(quote["cumulative_amount"]);
		symbol->PreDayRate(quote["updown_rate"]);
		
		
		SmTick tick, tick2;
		
		tick2.close = tick.close = quote["close"];
		tick2.time = tick.time = quote["time"];
		tick.qty = quote["volume"];
		tick.updown = quote["up_down"];
		// 이 함수 호출은 종료할 때 문제가 생김
		symbol->ShiftDown(std::move(tick));	
		symbol->UpdateChartData(std::move(tick2));
		
		CString msg;
		msg.Format(">>>>>> symbol_code = %s, close = %d\n", symbol_code.c_str(), symbol->Qoute.close);
		//TRACE(msg);
		mainApp.TotalPosiMgr()->UpdatePosition(symbol_code);
		mainApp.TotalOrderMgr()->CheckStopOrder(symbol);
		mainApp.CallbackMgr()->OnQuoteEvent(symbol_code);
		
		
	}
	catch (const std::exception & e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

bool SmQuoteProcessor::ProcessQuote(const std::array<nlohmann::json, BulkQuoteSize> & arr, const int& taken)
{
	
	return true;
}

void DarkHorse::SmQuoteProcessor::ClearTasks()
{
	_QuoteQueue.flush();
}

void DarkHorse::SmQuoteProcessor::SendQuoteChangeEvent()
{
	for (auto it = _WndQuoteSet.begin(); it != _WndQuoteSet.end(); ++it) {
		HWND hWnd = *it;
		::SendMessage(hWnd, WM_QUOTE_CHANGED, 0, 0);
	}
}
