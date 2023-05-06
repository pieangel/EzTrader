#include "stdafx.h"
#include "ViServerDataReceiver.h"
#include <vector>
#include "VtProgressDlg.h"
#include "../MainFrm.h"
#include "../Task/SmTaskDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
namespace DarkHorse {
	int ViServerDataReceiver::_TaskId = 0;

	void ViServerDataReceiver::ExecRequest(DhTaskArg& arg)
	{
		arg.Requested = true;
		switch (arg.TaskType)
		{
		case DhTaskType::HdSymbolMaster:
			//mainApp.Client().GetAbSymbolMaster(arg);
			break;
		case DhTaskType::HdSymbolSise:
			//mainApp.Client().GetAbSymbolSise(arg);
			break;
		case DhTaskType::HdSymbolHoga:
			//mainApp.Client().GetAbSymbolHoga(arg);
			break;
		case DhTaskType::HdAccountAsset:
			//mainApp.Client().GetAbAsset(arg);
			break;
		case DhTaskType::HdAccountProfitLoss:
			//mainApp.Client().GetAbAccountProfitLoss(arg);
			break;
		case DhTaskType::HdAccountSymbolRemain:
			//mainApp.Client().GetAbOutStanding(arg);
			break;
		case DhTaskType::HdAcceptedOrderList:
			//mainApp.Client().GetAbAccepted(arg);
			break;
		case DhTaskType::HdSymbolChartData:
			//mainApp.Client().GetAbChartData(arg);
			break;
		case DhTaskType::HdFilledOrderList:
			//mainApp.Client().GetAbFilled(arg);
			break;
		case DhTaskType::HdFilledDetail:
			//mainApp.Client().GetAbFilledOrderDetail(arg);
			break;
		default:
			break;
		}
	}

	void ViServerDataReceiver::SetTaskState()
	{
		if (!_ProgressDlg)
			return;
		_ProgressDlg->TotalCount(_TaskInfo.TotalTaskCount);
		_ProgressDlg->RemainCount(_TaskInfo.RemainTaskCount);
		_ProgressDlg->TaskTitle(_TaskInfo.TaskTitle);
		_ProgressDlg->TaskDetail(_TaskInfo.TaskDetail);
		_ProgressDlg->RefreshProgress();
	}

	void ViServerDataReceiver::DoNextGroupTask()
	{
		// ���� ��û ����� ���� ��� ���ش�.
		_TaskInfo.argMap.clear();

		if (_TaskInfo.TaskType == DhTaskType::HdSymbolMaster) {
			StartGetCSise();
		}
		else if (_TaskInfo.TaskType == DhTaskType::HdSymbolSise) {
			StartGetHoga();
		}
		else if (_TaskInfo.TaskType == DhTaskType::HdSymbolHoga) {
			StartGetAccountAsset();
		}
		else if (_TaskInfo.TaskType == DhTaskType::HdAccountAsset) {
			StartGetAccountProfitLoss();
		}
		else if (_TaskInfo.TaskType == DhTaskType::HdAccountProfitLoss) {
			StartGetProductRemain();
		}
		else if (_TaskInfo.TaskType == DhTaskType::HdAccountSymbolRemain) {
			StartGetAccountOrder();
		}
		else if (_TaskInfo.TaskType == DhTaskType::HdAcceptedOrderList) {
			StartGetFilledOrder();
		}
		else if (_TaskInfo.TaskType == DhTaskType::HdFilledOrderList) {
			//EndAllTask();
			StartGetChartData();
		}
		else if (_TaskInfo.TaskType == DhTaskType::HdSymbolChartData) {
			StartGetFilledDetails();
		}
		else if (_TaskInfo.TaskType == DhTaskType::HdFilledDetail) {
			EndAllTask();
		}
	}

	void ViServerDataReceiver::InitRequestQ()
	{

	}

	void ViServerDataReceiver::ExecNext()
	{
		if (_TaskInfo.argMap.size() == 0)
			return;
		// ���� ������ ��Ҹ� �����´�.
		auto front = _TaskInfo.argMap.begin();
		DhTaskArg& arg = front->second;
		_TaskInfo.TaskDetail = arg.Detail;
		// �۾� ���¸� �����Ѵ�.
		SetTaskState();
		// ��û���� �ʾҴٸ� ��û�� �Ѵ�.
		if (!arg.Requested) {
			ExecRequest(arg);
		}
	}

	DhTaskArg* ViServerDataReceiver::FindTask(const int& id)
	{
		auto it = _TaskInfo.argMap.find(id);
		if (it != _TaskInfo.argMap.end())
			return &it->second;

		return nullptr;
	}

	void ViServerDataReceiver::OnTaskComplete(const int& reqId)
	{
		// ��û�� �Ϸ�� ���� ã�� ���� �ʿ��� ���� �ش�.
		auto it = _TaskInfo.argMap.find(reqId);
		if (it != _TaskInfo.argMap.end()) {
			_TaskInfo.argMap.erase(it);
		}

		// ���� ������ �ٽ� ������ �ش�.
		_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
		SetTaskState();
		if (_TaskInfo.RemainTaskCount == 0) {
			((CMainFrame*)AfxGetMainWnd())->StopTimer();
			DoNextGroupTask();
		}
	}

	void ViServerDataReceiver::OnTaskError(const int& reqId)
	{
		// ������ ���� �ٽ� ��û�� ���ش�.
		auto it = _TaskInfo.argMap.find(reqId);
		if (it != _TaskInfo.argMap.end()) {
			DhTaskArg& arg = it->second;
			arg.Requested = false;
		}
	}

	void ViServerDataReceiver::MakeSymbolMasterRequests()
	{

	}

	void ViServerDataReceiver::MakeSymbolSiseRequests()
	{


	}

	void ViServerDataReceiver::MakeSymbolHogaRequests()
	{

	}
	void ViServerDataReceiver::MakeSymbolRequests()
	{

	}

	void ViServerDataReceiver::MakeAccountAssetRequests()
	{

	}

	void ViServerDataReceiver::MakeAccoutProfitLoss()
	{

	}

	void ViServerDataReceiver::MakeRemainRequests()
	{

	}

	void ViServerDataReceiver::MakeAccountOrders()
	{

	}

	void ViServerDataReceiver::EndAllTask()
	{
		((CMainFrame*)AfxGetMainWnd())->LoadAfterServerData();
	}

	void ViServerDataReceiver::MakeChartDataRequests()
	{

	}

	void ViServerDataReceiver::MakeChartDataRequestsNasdaq()
	{

	}

	void ViServerDataReceiver::StartGetSymbolMaster()
	{
		MakeSymbolMasterRequests();
		((CMainFrame*)AfxGetMainWnd())->StartTimer(10);
	}

	void ViServerDataReceiver::StartGetSymbol()
	{
		MakeSymbolRequests();
		((CMainFrame*)AfxGetMainWnd())->StartTimer(1100);
	}

	void ViServerDataReceiver::StartGetCSise()
	{
		MakeSymbolSiseRequests();
		((CMainFrame*)AfxGetMainWnd())->StartTimer(10);
	}

	void ViServerDataReceiver::StartGetHoga()
	{
		MakeSymbolHogaRequests();
		((CMainFrame*)AfxGetMainWnd())->StartTimer(10);
	}

	void ViServerDataReceiver::StartGetAccountAsset()
	{
		MakeAccountAssetRequests();
		((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
	}

	void ViServerDataReceiver::StartGetProductRemain()
	{
		MakeRemainRequests();
		((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
	}

	void ViServerDataReceiver::StartGetAccountOrder()
	{
		MakeAccountOrders();
		((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
	}

	void ViServerDataReceiver::StartGetAccountProfitLoss()
	{
		MakeAccoutProfitLoss();
		((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
	}

	void ViServerDataReceiver::StartGetChartData()
	{
		MakeChartDataRequestsNasdaq();
		((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
	}

	void ViServerDataReceiver::MakeFilledOrders()
	{

	}

	void ViServerDataReceiver::StartGetFilledOrder()
	{
		MakeFilledOrders();
		((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
	}

	void ViServerDataReceiver::MakeFilledOrderDatails()
	{

	}

	void ViServerDataReceiver::StartGetFilledDetails()
	{
		MakeFilledOrderDatails();
		((CMainFrame*)AfxGetMainWnd())->StartTimer(700);
	}

}