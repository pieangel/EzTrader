#include "stdafx.h"
#include "SmHogaManager.h"
#include "SmHogaProcessor.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Log/MyLogger.h"

using namespace DarkHorse;

SmHogaManager::SmHogaManager()
{
	StartProcess();
}

SmHogaManager::~SmHogaManager()
{
	StopProcess();
}

void SmHogaManager::StartProcess() noexcept
{
	start();
}

void SmHogaManager::StopProcess() noexcept
{
	if (!m_runMode) return;
	// ���� ť�� ����.
	ClearTasks();
	m_stop = true;
	nlohmann::json empty_hoga;
	empty_hoga["symbol_code"] = "";
	_HogaQueue.add(std::move(empty_hoga));
	// �����尡 ���������� ��ٸ���.
	if (m_thread.joinable())
		m_thread.join();
	if (m_runMode)
		m_runMode = false;
}

unsigned int SmHogaManager::ThreadHandlerProc(void)
{
	while (true) {
		// ���� ��ȣ�� ������ ������ ������.
		if (isStop()) {
			break;
		}
		nlohmann::json hoga;
		_HogaQueue.take(hoga);
		ProcessHoga(std::move(hoga));
	}

	return 1;
}

void SmHogaManager::AddHoga(nlohmann::json&& hoga) noexcept
{
	if (!_Enable) return;

	_HogaQueue.try_add(hoga);
}

void SmHogaManager::ProcessHoga(nlohmann::json&& hoga)
{
	try {
		const std::string& symbol_code = hoga["symbol_code"];
		const auto found = _HogaProcessorMap.find(symbol_code);
		if (found != _HogaProcessorMap.end()) {
			const std::shared_ptr<SmHogaProcessor>& hoga_processor = found->second;
			hoga_processor->AddHoga(std::move(hoga));
		}
		else {
			std::shared_ptr<SmHogaProcessor> hoga_processor = std::make_shared<SmHogaProcessor>();
			_HogaProcessorMap[symbol_code] = hoga_processor;
			hoga_processor->StartProcess();
			hoga_processor->AddHoga(std::move(hoga));
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

bool SmHogaManager::ProcessHoga(const std::array<nlohmann::json, BulkHogaTaskSize>& arr, const int& taken)
{

	return true;
}

void DarkHorse::SmHogaManager::StopAllHogaProcess()
{
	for (auto it = _HogaProcessorMap.begin(); it != _HogaProcessorMap.end(); it++) {
		it->second->StopProcess();
	}
}

void DarkHorse::SmHogaManager::ClearTasks()
{
	_HogaQueue.flush();
}