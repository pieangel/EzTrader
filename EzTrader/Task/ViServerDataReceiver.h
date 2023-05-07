#pragma once

//#include "../Global/TemplateSingleton.h"
//#include "../EBCtrl/EbGlobal.h"
#include <map>
#include <memory>
#include "../Task/SmTaskDefine.h"
// 서버에서 데이터를 가져오는 작업은 그룹별로 진행된다.
// 하나의 작업 그룹을 큐에 넣고 그 작업 그룹이 완성이 되면 다음 작업 그룹을 진행한다.
// 작업 그룹이 진행되는 과정은 프로그레스바를 통해서 표시된다.
class VtProgressDlg;

/*
HdSymbolMaster, // 해외 종목 정보
	HdSymbolSise, // 해외 종목 시세
	HdSymbolHoga, // 해외 종목 호가
	HdSymbolChartData, // 해외 종목 차트 데이터
	HdAccountAsset, // 해외 계좌
	HdAccountProfitLoss, // 해외 계좌 손익
	HdAccountSymbolRemain, // 해외 종목 잔고
	HdAcceptedOrderList // 해외 미체결 주문
*/
namespace DarkHorse {
	class ViServerDataReceiver // : public TemplateSingleton<ViServerDataReceiver>
	{
	private:
		// 앱의 상태를 나타낸다.
		//SmAppState _AppState = SmAppState::None;
		// Key : id, value : argument
		//std::map<int, DhTaskArg> 	_RequestMap;
		void execute_request(DhTaskArg& arg);
		// 진행 상황을 나태내는 대화상자
		//VtProgressDlg* _ProgressDlg;
		std::shared_ptr<VtProgressDlg> progress_dialog_;
		// 현재 작업 중인 작업의 정보를 기억한다.
		DhTaskInfo task_info_;
		// 현재 작업 상태를 표시한다.
		void set_task_state();
		// 다음 그룹 작업을 시작한다.
		void do_next_group_task();
	public:
		// 큐에 남아 있는 작업중 젤 상위의 작업을 실행 합니다.
		// 큐가 비어 있으면 아무일도 하지 않습니다.
		void execute_next();
		DhTaskArg* find_task(const int& argument_id);
		void on_task_complete(const int& argument_id);
		void on_task_error(const int& argument_id);
		// 서버에서 정보 가져오는 과정을 종료시킨다.
		void end_all_task();
		static int argument_id_;
		static int get_argument_id() {
			return argument_id_++;
		}
		std::shared_ptr<VtProgressDlg> progress_dialog() const { return progress_dialog_; }
		void progress_dialog(std::shared_ptr<VtProgressDlg> val) { progress_dialog_ = val; }

		void StartGetSymbolMaster();
		void StartGetSymbol();
		void StartGetCSise();
		void StartGetHoga();
		void StartGetAccountAsset();
		void StartGetProductRemain();
		void StartGetAccountOrder();
		void StartGetAccountProfitLoss();
		void StartGetChartData();
		void StartGetFilledOrder();
		void StartGetFilledDetails();
		void start_dm_symbol_master_file_download();
	private:
		void make_dm_file_download();
		void make_ab_file_download();
		// 차트데이터 요청 목록을 만든다.
		void MakeChartDataRequests();
		/// <summary>
		/// 나스닥100 차트 데이터를 가져온다.
		/// </summary>
		void MakeChartDataRequestsNasdaq();
		void MakeSymbolMasterRequests();
		// 시세 요청 목록을 만든다.
		void MakeSymbolSiseRequests();
		// 호가 요청 목록을 만든다.
		void MakeSymbolHogaRequests();
		// 심볼 요청 목록을 만든다.
		void MakeSymbolRequests();
		// 예탁 잔고 신청 목록을 만든다.
		void MakeAccountAssetRequests();
		// 일별 손익을 가져옵니다.
		void make_ab_account_profit_loss();
		// 잔고 및 평가손익 신청 목록을 만든다.
		void MakeRemainRequests();
		// 계좌별 미체결 주문 요청 목록을 만든다.
		void MakeAccountOrders();
		// 계좌별 체결 주문 요청 목록을 만든다.
		void MakeFilledOrders();

		// 체결 상세 내역을 가져온다.
		void MakeFilledOrderDatails();
	};
}
