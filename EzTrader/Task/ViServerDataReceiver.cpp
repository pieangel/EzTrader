#include "stdafx.h"
#include "ViServerDataReceiver.h"
#include <vector>
#include "VtProgressDlg.h"
#include "../MainFrm.h"
#include "../Task/SmTaskDefine.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Client/ViStockClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
namespace DarkHorse {
	int ViServerDataReceiver::argument_id_ = 0;

	void ViServerDataReceiver::execute_request(DhTaskArg& arg)
	{
		arg.requested = true;
		switch (arg.task_type)
		{
		case DhTaskType::DmSymbolMasterFileDownload:
			mainApp.Client()->dm_symbol_master_file_download(arg);
			break;
		case DhTaskType::AbSymbolMaster:
			//mainApp.Client().GetAbSymbolMaster(arg);
			break;
		case DhTaskType::AbSymbolQuote:
			//mainApp.Client().GetAbSymbolSise(arg);
			break;
		case DhTaskType::AbSymbolHoga:
			//mainApp.Client().GetAbSymbolHoga(arg);
			break;
		case DhTaskType::AbAccountAsset:
			//mainApp.Client().GetAbAsset(arg);
			break;
		case DhTaskType::AbAccountProfitLoss:
			//mainApp.Client().GetAbAccountProfitLoss(arg);
			break;
		case DhTaskType::AbAccountSymbolRemain:
			//mainApp.Client().GetAbOutStanding(arg);
			break;
		case DhTaskType::AbAcceptedOrderList:
			//mainApp.Client().GetAbAccepted(arg);
			break;
		case DhTaskType::AbSymbolChartData:
			//mainApp.Client().GetAbChartData(arg);
			break;
		case DhTaskType::AbFilledOrderList:
			//mainApp.Client().GetAbFilled(arg);
			break;
		case DhTaskType::AbFilledDetail:
			//mainApp.Client().GetAbFilledOrderDetail(arg);
			break;
		default:
			break;
		}
	}

	void ViServerDataReceiver::set_task_state()
	{
		if (!progress_dialog_)
			return;
		progress_dialog_->TotalCount(task_info_.total_task_count);
		progress_dialog_->RemainCount(task_info_.remain_task_count);
		progress_dialog_->TaskTitle(task_info_.task_title);
		progress_dialog_->TaskDetail(task_info_.task_detail);
		progress_dialog_->RefreshProgress();
	}

	void ViServerDataReceiver::do_next_group_task()
	{
		// 기존 요청 목록을 먼저 모두 없앤다.
		task_info_.argument_map.clear();

		if (task_info_.task_type == DhTaskType::AbSymbolMaster) {
			StartGetCSise();
		}
		else if (task_info_.task_type == DhTaskType::AbSymbolQuote) {
			StartGetHoga();
		}
		else if (task_info_.task_type == DhTaskType::AbSymbolHoga) {
			StartGetAccountAsset();
		}
		else if (task_info_.task_type == DhTaskType::AbAccountAsset) {
			StartGetAccountProfitLoss();
		}
		else if (task_info_.task_type == DhTaskType::AbAccountProfitLoss) {
			StartGetProductRemain();
		}
		else if (task_info_.task_type == DhTaskType::AbAccountSymbolRemain) {
			StartGetAccountOrder();
		}
		else if (task_info_.task_type == DhTaskType::AbAcceptedOrderList) {
			StartGetFilledOrder();
		}
		else if (task_info_.task_type == DhTaskType::AbFilledOrderList) {
			//EndAllTask();
			StartGetChartData();
		}
		else if (task_info_.task_type == DhTaskType::AbSymbolChartData) {
			StartGetFilledDetails();
		}
		else if (task_info_.task_type == DhTaskType::AbFilledDetail) {
			end_all_task();
		}
	}

	void ViServerDataReceiver::execute_next()
	{
		if (task_info_.argument_map.size() == 0)
			return;
		// 가장 오래된 요소를 가져온다.
		auto front = task_info_.argument_map.begin();
		DhTaskArg& arg = front->second;
		task_info_.task_detail = arg.detail_task_description;
		// 작업 상태를 설정한다.
		set_task_state();
		// 요청하지 않았다면 요청을 한다.
		if (!arg.requested) {
			execute_request(arg);
		}
	}

	DhTaskArg* ViServerDataReceiver::find_task(const int& argument_id)
	{
		auto it = task_info_.argument_map.find(argument_id);
		if (it != task_info_.argument_map.end())
			return &it->second;

		return nullptr;
	}

	void ViServerDataReceiver::on_task_complete(const int& argument_id)
	{
		// 요청이 완료된 일은 찾아 내어 맵에서 없애 준다.
		auto it = task_info_.argument_map.find(argument_id);
		if (it != task_info_.argument_map.end()) {
			task_info_.argument_map.erase(it);
		}

		// 남은 걧수를 다시 설정해 준다.
		task_info_.remain_task_count = task_info_.argument_map.size();
		set_task_state();
		if (task_info_.remain_task_count == 0) {
			((CMainFrame*)AfxGetMainWnd())->stop_timer();
			do_next_group_task();
		}
	}

	void ViServerDataReceiver::on_task_error(const int& argument_id)
	{
		// 오류가 나면 다시 요청을 해준다.
		auto it = task_info_.argument_map.find(argument_id);
		if (it != task_info_.argument_map.end()) {
			DhTaskArg& arg = it->second;
			arg.requested = false;
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

	void ViServerDataReceiver::make_ab_account_profit_loss()
	{
		const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
		for (auto it = account_map.begin(); it != account_map.end(); it++) {
			std::shared_ptr<SmAccount> account = it->second;
			if (account->Type() != "1") continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = ViServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbAccountProfitLoss;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "계좌별 손익을 가져오는 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbAccountProfitLoss;
	}

	void ViServerDataReceiver::MakeRemainRequests()
	{

	}

	void ViServerDataReceiver::MakeAccountOrders()
	{

	}

	void ViServerDataReceiver::end_all_task()
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
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void ViServerDataReceiver::StartGetSymbol()
	{
		MakeSymbolRequests();
		((CMainFrame*)AfxGetMainWnd())->start_timer(1100);
	}

	void ViServerDataReceiver::StartGetCSise()
	{
		MakeSymbolSiseRequests();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void ViServerDataReceiver::StartGetHoga()
	{
		MakeSymbolHogaRequests();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void ViServerDataReceiver::StartGetAccountAsset()
	{
		MakeAccountAssetRequests();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::StartGetProductRemain()
	{
		MakeRemainRequests();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::StartGetAccountOrder()
	{
		MakeAccountOrders();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::StartGetAccountProfitLoss()
	{
		make_ab_account_profit_loss();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::StartGetChartData()
	{
		MakeChartDataRequestsNasdaq();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::MakeFilledOrders()
	{

	}

	void ViServerDataReceiver::StartGetFilledOrder()
	{
		MakeFilledOrders();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::MakeFilledOrderDatails()
	{

	}

	void ViServerDataReceiver::StartGetFilledDetails()
	{
		MakeFilledOrderDatails();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::make_dm_file_download()
	{
		const std::string file_name = "chocode.cod";
		DhTaskArg arg;
		arg.detail_task_description = file_name;
		arg.argument_id = ViServerDataReceiver::get_argument_id();
		arg.task_type = DhTaskType::DmSymbolMasterFileDownload;
		arg.parameter_map["file_name"] = file_name;

		task_info_.argument_map[arg.argument_id] = arg;
	

		task_info_.task_title = "심볼 마스터 파일 다운로드 중입니다.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmSymbolMasterFileDownload;
	}

	void ViServerDataReceiver::make_ab_file_download()
	{

	}

	void ViServerDataReceiver::start_dm_symbol_master_file_download()
	{
		make_dm_file_download();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

}