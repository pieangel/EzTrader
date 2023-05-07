#pragma once

//#include "../Global/TemplateSingleton.h"
//#include "../EBCtrl/EbGlobal.h"
#include <map>
#include <memory>
#include "../Task/SmTaskDefine.h"
// �������� �����͸� �������� �۾��� �׷캰�� ����ȴ�.
// �ϳ��� �۾� �׷��� ť�� �ְ� �� �۾� �׷��� �ϼ��� �Ǹ� ���� �۾� �׷��� �����Ѵ�.
// �۾� �׷��� ����Ǵ� ������ ���α׷����ٸ� ���ؼ� ǥ�õȴ�.
class VtProgressDlg;

/*
HdSymbolMaster, // �ؿ� ���� ����
	HdSymbolSise, // �ؿ� ���� �ü�
	HdSymbolHoga, // �ؿ� ���� ȣ��
	HdSymbolChartData, // �ؿ� ���� ��Ʈ ������
	HdAccountAsset, // �ؿ� ����
	HdAccountProfitLoss, // �ؿ� ���� ����
	HdAccountSymbolRemain, // �ؿ� ���� �ܰ�
	HdAcceptedOrderList // �ؿ� ��ü�� �ֹ�
*/
namespace DarkHorse {
	class ViServerDataReceiver // : public TemplateSingleton<ViServerDataReceiver>
	{
	private:
		// ���� ���¸� ��Ÿ����.
		//SmAppState _AppState = SmAppState::None;
		// Key : id, value : argument
		//std::map<int, DhTaskArg> 	_RequestMap;
		void execute_request(DhTaskArg& arg);
		// ���� ��Ȳ�� ���³��� ��ȭ����
		//VtProgressDlg* _ProgressDlg;
		std::shared_ptr<VtProgressDlg> progress_dialog_;
		// ���� �۾� ���� �۾��� ������ ����Ѵ�.
		DhTaskInfo task_info_;
		// ���� �۾� ���¸� ǥ���Ѵ�.
		void set_task_state();
		// ���� �׷� �۾��� �����Ѵ�.
		void do_next_group_task();
	public:
		// ť�� ���� �ִ� �۾��� �� ������ �۾��� ���� �մϴ�.
		// ť�� ��� ������ �ƹ��ϵ� ���� �ʽ��ϴ�.
		void execute_next();
		DhTaskArg* find_task(const int& argument_id);
		void on_task_complete(const int& argument_id);
		void on_task_error(const int& argument_id);
		// �������� ���� �������� ������ �����Ų��.
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
		// ��Ʈ������ ��û ����� �����.
		void MakeChartDataRequests();
		/// <summary>
		/// ������100 ��Ʈ �����͸� �����´�.
		/// </summary>
		void MakeChartDataRequestsNasdaq();
		void MakeSymbolMasterRequests();
		// �ü� ��û ����� �����.
		void MakeSymbolSiseRequests();
		// ȣ�� ��û ����� �����.
		void MakeSymbolHogaRequests();
		// �ɺ� ��û ����� �����.
		void MakeSymbolRequests();
		// ��Ź �ܰ� ��û ����� �����.
		void MakeAccountAssetRequests();
		// �Ϻ� ������ �����ɴϴ�.
		void make_ab_account_profit_loss();
		// �ܰ� �� �򰡼��� ��û ����� �����.
		void MakeRemainRequests();
		// ���º� ��ü�� �ֹ� ��û ����� �����.
		void MakeAccountOrders();
		// ���º� ü�� �ֹ� ��û ����� �����.
		void MakeFilledOrders();

		// ü�� �� ������ �����´�.
		void MakeFilledOrderDatails();
	};
}
