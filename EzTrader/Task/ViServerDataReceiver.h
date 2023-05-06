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
		std::map<int, DhTaskArg> 	_RequestMap;
		void ExecRequest(DhTaskArg& arg);
		// ���� ��Ȳ�� ���³��� ��ȭ����
		//VtProgressDlg* _ProgressDlg;
		std::shared_ptr<VtProgressDlg> _ProgressDlg;
		// ���� �۾� ���� �۾��� ������ ����Ѵ�.
		DhTaskInfo _TaskInfo;
		// ���� �۾� ���¸� ǥ���Ѵ�.
		void SetTaskState();
		// ���� �׷� �۾��� �����Ѵ�.
		void DoNextGroupTask();
	public:
		//SmAppState AppState() const { return _AppState; }
		//void AppState(SmAppState val) { _AppState = val; }
		void InitRequestQ();
		// ť�� ���� �ִ� �۾��� �� ������ �۾��� ���� �մϴ�.
		// ť�� ��� ������ �ƹ��ϵ� ���� �ʽ��ϴ�.
		void ExecNext();
		DhTaskArg* FindTask(const int& reqId);
		void OnTaskComplete(const int& reqId);
		void OnTaskError(const int& reqId);
		int GetTaskCount() {
			return _RequestMap.size();
		}
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
		void MakeAccoutProfitLoss();
		// �ܰ� �� �򰡼��� ��û ����� �����.
		void MakeRemainRequests();
		// ���º� ��ü�� �ֹ� ��û ����� �����.
		void MakeAccountOrders();
		// ���º� ü�� �ֹ� ��û ����� �����.
		void MakeFilledOrders();

		// ü�� �� ������ �����´�.
		void MakeFilledOrderDatails();

		// �������� ���� �������� ������ �����Ų��.
		void EndAllTask();
		// ��Ʈ������ ��û ����� �����.
		void MakeChartDataRequests();
		/// <summary>
		/// ������100 ��Ʈ �����͸� �����´�.
		/// </summary>
		void MakeChartDataRequestsNasdaq();

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
		static int _TaskId;
		static int GetId() {
			return _TaskId++;
		}
		std::shared_ptr<VtProgressDlg> ProgressDlg() const { return _ProgressDlg; }
		void ProgressDlg(std::shared_ptr<VtProgressDlg> val) { _ProgressDlg = val; }
	};
}
