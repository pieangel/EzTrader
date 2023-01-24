#include "stdafx.h"
#include "SmTaskRequestManager.h"
#include "../Global/SmTotalManager.h"
#include "../Client/ViStockClient.h"
using namespace DarkHorse;

DarkHorse::SmTaskRequestManager::SmTaskRequestManager()
{
	StartProcess();
}

DarkHorse::SmTaskRequestManager::~SmTaskRequestManager()
{
	StopProcess();
}

void DarkHorse::SmTaskRequestManager::StartProcess() noexcept
{
	start();
}

void DarkHorse::SmTaskRequestManager::StopProcess() noexcept
{
	if (!m_runMode) return;
	// 먼저 큐를 비운다.
	ClearTasks();
	m_stop = true;
	SmTaskArg empty_arg;
	empty_arg.TaskType = SmTaskType::None;
	_TaskQueue.add(std::move(empty_arg));
	// 쓰레드가 끝날때까지 기다린다.
	if (m_thread.joinable())
		m_thread.join();
	if (m_runMode)
		m_runMode = false;
}

unsigned int DarkHorse::SmTaskRequestManager::ThreadHandlerProc(void)
{
	while (true) {
		// 종료 신호를 보내면 루프를 나간다.
		if (isStop()) {
			break;
		}
		SmTaskArg task;
		_TaskQueue.take(task);
		ProcessTask(std::move(task));
	}

	return 1;
}

void DarkHorse::SmTaskRequestManager::AddTask(SmTaskArg&& task) noexcept
{
	if (!_Enable) return;

	_TaskQueue.add(task);
}

void DarkHorse::SmTaskRequestManager::ClearTasks() noexcept
{
	_TaskQueue.flush();
}

void DarkHorse::SmTaskRequestManager::ProcessTask(SmTaskArg&& task)
{
	mainApp.Client()->ExecTask(std::move(task));
}

bool DarkHorse::SmTaskRequestManager::ProcessTask(const std::array<SmTaskArg, BulkReqSize>& arr, const int& taken)
{
	return true;
}
