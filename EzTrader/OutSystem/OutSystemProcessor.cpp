#include "stdafx.h"
#include "OutSystemProcessor.h"

#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Log/MyLogger.h"
#include "../MessageDefine.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Event/EventHub.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Position/TotalPositionManager.h"
namespace DarkHorse {

	OutSystemProcessor::OutSystemProcessor()
	{
		StartProcess();
	}

	OutSystemProcessor::~OutSystemProcessor()
	{
		StopProcess();
	}

	void OutSystemProcessor::StartProcess() noexcept
	{
		start();
	}

	void OutSystemProcessor::StopProcess() noexcept
	{
		if (!m_runMode) return;
		// 먼저 큐를 비운다.
		ClearTasks();
		m_stop = true;
		nlohmann::json empty_signal;
		empty_signal["symbol_code"] = "";
		_SignalQueue.add(std::move(empty_signal));
		// 쓰레드가 끝날때까지 기다린다.
		if (m_thread.joinable())
			m_thread.join();
		if (m_runMode)
			m_runMode = false;
	}

	unsigned int OutSystemProcessor::ThreadHandlerProc(void)
	{
		while (true) {
			// 종료 신호를 보내면 루프를 나간다.
			if (isStop()) {
				break;
			}
			nlohmann::json signal;
			_SignalQueue.take(signal);
			ProcessSignal(std::move(signal));
		}

		return 1;
	}

	void OutSystemProcessor::AddQuote(nlohmann::json&& signal) noexcept
	{
		if (!_Enable) return;

		_SignalQueue.try_add(signal);
	}

	void OutSystemProcessor::ProcessSignal(nlohmann::json&& signal)
	{
		try {
			const std::string& symbol_code = signal["symbol_code"];
			std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
			if (!symbol) return;
			std::shared_ptr<SmQuote> quote_p = mainApp.QuoteMgr()->get_quote(symbol_code);
			quote_p->symbol_code = symbol_code;
			quote_p->symbol_id = symbol->Id();
			quote_p->open = signal["open"];
			quote_p->high = signal["high"];
			quote_p->low = signal["low"];
			quote_p->close = signal["close"];
			//quote_p->pre_day_close = quote["pre_day_close"];

			mainApp.total_position_manager()->update_position(quote_p);

			mainApp.event_hub()->process_quote_event(quote_p);

			SmTick tick;
			tick.symbol_id = symbol->Id();
			tick.close = signal["close"];
			tick.time = signal["time"];
			tick.qty = signal["volume"];
			tick.updown = signal["up_down"];

			symbol->quote_requested(true);

			mainApp.event_hub()->process_tick_event(tick);

		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	bool OutSystemProcessor::ProcessSignal(const std::array<nlohmann::json, BulkOutSystemSize>& arr, const int& taken)
	{

		return true;
	}

	void OutSystemProcessor::ClearTasks()
	{
		_SignalQueue.flush();
	}
}