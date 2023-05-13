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
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbolReader.h"
#include "../Symbol/SmSymbol.h"
#include <set>

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
		case DhTaskType::AbSymbolMasterFileDownload:
			mainApp.Client()->ab_symbol_master_file_download(arg);
			break;
		case DhTaskType::AbSymbolMaster:
			mainApp.Client()->ab_symbol_master(arg);
			break;
		case DhTaskType::DmAccountAsset:
			mainApp.Client()->dm_account_asset(arg);
			break;
		case DhTaskType::AbAccountAsset:
			mainApp.Client()->ab_account_asset(arg);
			break;
		case DhTaskType::AbAccountProfitLoss:
			mainApp.Client()->ab_account_profit_loss(arg);
			break;
		case DhTaskType::DmApiCustomerProfitLoss:
			mainApp.Client()->dm_account_profit_loss(arg);
			break;
		case DhTaskType::AbAcceptedOrderList:
			mainApp.Client()->ab_accepted_order(arg);
			break;
		case DhTaskType::DmAcceptedOrderList:
			mainApp.Client()->dm_accepted_order(arg);
			break;
		case DhTaskType::AbSymbolQuote:
			mainApp.Client()->ab_symbol_quote(arg);
			break;
		case DhTaskType::AbSymbolHoga:
			mainApp.Client()->ab_symbol_hoga(arg);
			break;	
		case DhTaskType::AbSymbolPosition:
			mainApp.Client()->ab_symbol_position(arg);
			break;
		case DhTaskType::DmSymbolPosition:
			mainApp.Client()->dm_symbol_position(arg);
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
		// ���� ��û ����� ���� ��� ���ش�.
		task_info_.argument_map.clear();
		switch (task_info_.task_type)
		{
		case DhTaskType::DmSymbolMasterFileDownload:
		{
			start_ab_symbol_master_file_download();
		}
			break;
		case DhTaskType::AbSymbolMasterFileDownload:
		{
			mainApp.SymMgr()->MakeDomesticMarket();
			mainApp.SymMgr()->ReadAbroadSymbols();
			mainApp.SymMgr()->read_domestic_productfile();
			mainApp.SymMgr()->read_domestic_masterfile();
			mainApp.SymMgr()->sort_dm_option_symbol_vector();
			mainApp.SymMgr()->MakeAbFavorite();
			start_ab_symbol_master();
		}
			break;
		case DhTaskType::AbSymbolMaster:
		{
			start_dm_account_asset();
		}
			break;
		case DhTaskType::DmAccountAsset:
		{
			start_ab_account_asset();
		}
			break;
		case DhTaskType::AbAccountAsset:
		{
			start_ab_account_profit_loss();
		}
			break;
		case DhTaskType::AbAccountProfitLoss:
		{
			start_dm_account_profit_loss();
		}
			break;
		case DhTaskType::DmApiCustomerProfitLoss:
		{
			start_ab_accepted_order();
		}
			break;
		case DhTaskType::AbAcceptedOrderList:
		{
			start_dm_accepted_order();
		}
			break;
		case DhTaskType::DmAcceptedOrderList:
		{
			start_ab_symbol_position();
		}
			break;
		case DhTaskType::AbSymbolPosition:
		{
			start_dm_symbol_position();
		}
		break;
		case DhTaskType::DmSymbolPosition:
		{
			end_all_task();
		}
		break;
		
		default:
			break;
		}
	}

	void ViServerDataReceiver::execute_next()
	{
		if (task_info_.argument_map.size() == 0)
			return;
		// ���� ������ ��Ҹ� �����´�.
		auto front = task_info_.argument_map.begin();
		DhTaskArg& arg = front->second;
		task_info_.task_detail = arg.detail_task_description;
		// �۾� ���¸� �����Ѵ�.
		set_task_state();
		// ��û���� �ʾҴٸ� ��û�� �Ѵ�.
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
		// ��û�� �Ϸ�� ���� ã�� ���� �ʿ��� ���� �ش�.
		auto it = task_info_.argument_map.find(argument_id);
		if (it != task_info_.argument_map.end()) {
			task_info_.argument_map.erase(it);
		}

		// ���� ������ �ٽ� ������ �ش�.
		task_info_.remain_task_count = task_info_.argument_map.size();
		set_task_state();
		if (task_info_.remain_task_count == 0) {
			((CMainFrame*)AfxGetMainWnd())->stop_timer();
			do_next_group_task();
		}
	}

	void ViServerDataReceiver::on_task_error(const int& argument_id)
	{
		// ������ ���� �ǳ� �ڴ�. 
		// ��û�� �Ϸ�� ���� ã�� ���� �ʿ��� ���� �ش�.
		auto it = task_info_.argument_map.find(argument_id);
		if (it != task_info_.argument_map.end()) {
			task_info_.argument_map.erase(it);
		}

		// ���� ������ �ٽ� ������ �ش�.
		task_info_.remain_task_count = task_info_.argument_map.size();
		set_task_state();
		if (task_info_.remain_task_count == 0) {
			((CMainFrame*)AfxGetMainWnd())->stop_timer();
			do_next_group_task();
		}
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

		task_info_.task_title = "�ؿ� ���º� ������ �������� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbAccountProfitLoss;
	}

	void ViServerDataReceiver::make_dm_account_profit_loss()
	{
		const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
		for (auto it = account_map.begin(); it != account_map.end(); it++) {
			std::shared_ptr<SmAccount> account = it->second;
			if (account->Type() != "9") continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = ViServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::DmApiCustomerProfitLoss;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "���� ���º� ������ �������� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmApiCustomerProfitLoss;
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
	

		task_info_.task_title = "���� �ɺ� ������ ���� �ٿ�ε� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmSymbolMasterFileDownload;
	}

	void ViServerDataReceiver::make_ab_file_download()
	{
		const std::string file_name = "futures";
		DhTaskArg arg;
		arg.detail_task_description = file_name;
		arg.argument_id = ViServerDataReceiver::get_argument_id();
		arg.task_type = DhTaskType::AbSymbolMasterFileDownload;
		arg.parameter_map["file_name"] = file_name;

		task_info_.argument_map[arg.argument_id] = arg;


		task_info_.task_title = "�ؿ� �ɺ� ������ ���� �ٿ�ε� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbSymbolMasterFileDownload;
	}
	
	void ViServerDataReceiver::make_ab_symbol_master()
	{
		const std::map<int, std::shared_ptr<SmSymbol>>& ab_symbol_favorite = mainApp.SymMgr()->get_ab_favorite_map();
		for (auto it = ab_symbol_favorite.begin(); it != ab_symbol_favorite.end(); it++) {
			DhTaskArg arg;
			arg.detail_task_description = it->second->SymbolCode();
			arg.argument_id = ViServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbSymbolMaster;
			arg.parameter_map["symbol_code"] = it->second->SymbolCode();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "�ؿ� �ɺ� ������ �ٿ�ε� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbSymbolMaster;


		std::set<std::shared_ptr<SmSymbol>> ab_symbol_set;
		mainApp.SymMgr()->get_ab_recent_symbols(ab_symbol_set);
		if (ab_symbol_set.empty()) return;

		for (auto it = ab_symbol_set.begin(); it != ab_symbol_set.end(); it++) {
			DhTaskArg arg;
			arg.detail_task_description = (*it)->SymbolCode();
			arg.argument_id = ViServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbSymbolMaster;
			arg.parameter_map["symbol_code"] = (*it)->SymbolCode();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "�ؿ� �ɺ� ������ �ٿ�ε� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbSymbolMaster;
	}



	void ViServerDataReceiver::make_dm_account_asset() 
	{
		const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
		for (auto it = account_map.begin(); it != account_map.end(); it++) {
			std::shared_ptr<SmAccount> account = it->second;
			if (account->Type() != "9") continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = ViServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::DmAccountAsset;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "���� ���º� �ڻ������� �������� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmAccountAsset;
	}
	void ViServerDataReceiver::make_ab_account_asset() 
	{
		const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
		for (auto it = account_map.begin(); it != account_map.end(); it++) {
			std::shared_ptr<SmAccount> account = it->second;
			if (account->Type() != "1") continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = ViServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbAccountAsset;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "�ؿ� ���º� �ڻ� ������ �������� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbAccountAsset;
	}

	void ViServerDataReceiver::make_dm_accepted_order()
	{
		const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
		for (auto it = account_map.begin(); it != account_map.end(); it++) {
			std::shared_ptr<SmAccount> account = it->second;
			if (account->Type() != "9") continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = ViServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::DmAcceptedOrderList;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "���� �����ֹ��� �������� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmAcceptedOrderList;
	}
	void ViServerDataReceiver::make_ab_accepted_order()
	{
		const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
		for (auto it = account_map.begin(); it != account_map.end(); it++) {
			std::shared_ptr<SmAccount> account = it->second;
			if (account->Type() != "1") continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = ViServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbAcceptedOrderList;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "�ؿ� �����ֹ��� �������� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbAcceptedOrderList;
	}

	void ViServerDataReceiver::make_ab_symbol_position()
	{
		const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
		for (auto it = account_map.begin(); it != account_map.end(); it++) {
			std::shared_ptr<SmAccount> account = it->second;
			if (account->Type() != "1") continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = ViServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::AbSymbolPosition;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "�ؿ� ��ü�� ����� �������� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::AbSymbolPosition;
	}

	void ViServerDataReceiver::make_dm_symbol_position()
	{
		const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
		for (auto it = account_map.begin(); it != account_map.end(); it++) {
			std::shared_ptr<SmAccount> account = it->second;
			if (account->Type() != "9") continue;
			DhTaskArg arg;
			arg.detail_task_description = account->No();
			arg.argument_id = ViServerDataReceiver::get_argument_id();
			arg.task_type = DhTaskType::DmSymbolPosition;
			arg.parameter_map["account_no"] = account->No();
			arg.parameter_map["password"] = account->Pwd();

			task_info_.argument_map[arg.argument_id] = arg;
		}

		task_info_.task_title = "���� ��ü�� ����� �������� ���Դϴ�.";
		task_info_.total_task_count = task_info_.argument_map.size();
		task_info_.remain_task_count = task_info_.argument_map.size();
		task_info_.task_type = DhTaskType::DmSymbolPosition;
	}


	void ViServerDataReceiver::end_all_task()
	{
		((CMainFrame*)AfxGetMainWnd())->LoadAfterServerData();
	}

	void ViServerDataReceiver::start_dm_symbol_master_file_download()
	{
		make_dm_file_download();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void ViServerDataReceiver::start_ab_symbol_master_file_download()
	{
		make_ab_file_download();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void ViServerDataReceiver::start_ab_symbol_master()
	{
		make_ab_symbol_master();
		((CMainFrame*)AfxGetMainWnd())->start_timer(10);
	}

	void ViServerDataReceiver::start_dm_account_asset()
	{
		make_dm_account_asset();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}
	void ViServerDataReceiver::start_ab_account_asset()
	{
		make_ab_account_asset();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::start_ab_account_profit_loss()
	{
		make_ab_account_profit_loss();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::start_dm_account_profit_loss()
	{
		make_dm_account_profit_loss();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::start_dm_accepted_order()
	{
		make_dm_accepted_order();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}
	void ViServerDataReceiver::start_ab_accepted_order()
	{
		make_ab_accepted_order();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::start_ab_symbol_position()
	{
		make_ab_symbol_position();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}

	void ViServerDataReceiver::start_dm_symbol_position()
	{
		make_dm_symbol_position();
		((CMainFrame*)AfxGetMainWnd())->start_timer(700);
	}


}