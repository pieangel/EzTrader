#pragma once
#include <memory>
#include <map>
#include <set>
#include <string>
#include <any>
#include <windows.h>
#include "hdfcommagent.h"
#include "afxdialogex.h"
#include "ViStockClient.h"
#include "../Json/json.hpp"
#include "../Task/SmTaskArg.h"
#include "../Task/SmTaskDefine.h"
// ViClient dialog
namespace DarkHorse {
	/// <summary>
	/// ������ ������ �����ض�. 
	/// ��û ������ ��� ���� �� �����ϴ� ������ ��û ������ �������� ���� ��
	/// ��û ������ �ӽ÷� �����ߴٰ� �ٽ� ����Ѵ�. 
	/// ����� ���ʷ� ��û�� ���� ������ �ٽ� ���� ������ ã�� ���� �ʴ´�.
	/// �̹� ���� ��û�� �ٽ� ã�� ���� �ؼ��� ����� �ȵȴ�.
	/// ���� �ݵ�� �����Ѵ�. 
	/// ���� �ǹ̸� ���ϴ� ������ ���� ������ ���� �� �ݵ�� ���� ���� �̸��� ����� ��
	/// �̷��� ���� ������ �ſ� ȥ���� ����Ű�Ƿ� �ݵ�� �� ��Ģ�� ��ų ��.
	/// ���� �̸��� ���Ͽ� ǥ�� ������. �׷��� �ݵ�� ������ ���̴�.
	/// �̸��� ���Ͽ� �ϰ����� ��ų ��
	/// ���� �ǹ̿� ���Ͽ� ���⼭�� �̷��� ���⼭�� ������ ���� �� ��.
	/// </summary>
	class ViStockClient;
	struct SmOrderRequest;
	typedef std::map<std::string, std::any> task_arg;
	struct OrderRequest;
	using order_request_p = std::shared_ptr<OrderRequest>;
	class ViClient : public CDialog
	{

		DECLARE_DYNAMIC(ViClient)

		DECLARE_EVENTSINK_MAP()
		afx_msg void OnDataRecv(LPCTSTR sTrCode, LONG nRqID);
		afx_msg void OnGetBroadData(LPCTSTR strKey, LONG nRealType);
		afx_msg void OnGetMsg(LPCTSTR strCode, LPCTSTR strMsg);
		afx_msg void OnGetMsgWithRqId(int nRqId, LPCTSTR strCode, LPCTSTR strMsg);

	public:
		ViClient(CWnd* pParent = nullptr);   // standard constructor
		virtual ~ViClient();

		// Dialog Data
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_VI_CLIENT };
#endif
	public:
		int Login(const std::string& id, const std::string& pwd, const std::string& cert);
		int Login(task_arg&& arg);
		
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		void on_task_complete(const int& nRqId);
		void on_task_error(const int& server_request_id, const int request_id);
		int dm_symbol_master_file_download(DhTaskArg arg);
		int ab_symbol_master_file_download(DhTaskArg arg);
		int ab_symbol_master(DhTaskArg arg);
		int GetDomesticSymbolCode(task_arg&& arg);
		int GetFilledOrderList(const std::string& account_no, const std::string& pwd);
		int CheckAccountPassword(task_arg&& arg);
		int CheckDmAccountPassword(task_arg&& arg);
		void RemoveTask(const int& task_id);
		int DownloadFile(task_arg&& arg);
		int DownloadMasterFile(task_arg&& arg);
		int GetSymbolMaster(task_arg&& arg) ;
		int GetSymbolQuote(task_arg&& arg) ;
		int GetSymbolHoga(task_arg&& arg) ;
		int GetAccountAsset(task_arg&& arg) ;
		int GetDeposit(task_arg&& arg) ;
		int GetAccountProfitLoss(task_arg&& arg) ;
		int GetSymbolProfitLoss(task_arg&& arg) ;
		int GetAcceptedOrderList(task_arg&& arg) ;


		int GetFilledOrderList(task_arg&& arg) ;
		int GetChartData(task_arg&& arg) ;
		int GetChartData(SmTaskArg&& arg);
		int GetDmSymbolMaster(SmTaskArg&& arg);
		void RegisterSymbol(SmTaskArg&& arg);
		void NewOrder(task_arg&& arg) ;
		void ChangeOrder(task_arg&& arg) ;
		void CancelOrder(task_arg&& arg) ;
		void RegisterSymbol(task_arg&& arg) ;
		void UnregisterSymbol(task_arg&& arg) ;
		void RegisterAccount(task_arg&& arg) ;
		void UnregisterAccount(task_arg&& arg) ;

		void RegisterSymbol(const std::string& symbol_code);
		void RegisterAccount(const std::string& account_no);
		void UnregisterSymbol(const std::string& symbol_code);
		void UnregisterAccount(const std::string& account_no);

		void NewOrder(const std::shared_ptr<SmOrderRequest>& order_req);
		void ChangeOrder(const std::shared_ptr<SmOrderRequest>& order_req);
		void CancelOrder(const std::shared_ptr<SmOrderRequest>& order_req);

		void NewOrder(order_request_p order_req);
		void ChangeOrder(order_request_p order_req);
		void CancelOrder(order_request_p order_req);

		void dm_new_order(order_request_p order_req);
		void dm_change_order(order_request_p order_req);
		void dm_cancel_order(order_request_p order_req);

		void ab_new_order(order_request_p order_req);
		void ab_change_order(order_request_p order_req);
		void ab_cancel_order(order_request_p order_req);

		static void make_custom_order_info(const order_request_p& order_req, std::string& custom_order_info);

		int ConnectToServer();
		void Client(std::weak_ptr<ViStockClient> wp)
		{
			_Client = wp;
		}

		int ConvertToInt(CString& strSymbolCode, CString& strValue);
		void UnregAllSymbol();
		void UnregAllAccount();
		bool Enable() const { return _Enable; }
		void Enable(bool val) { _Enable = val; }
	private:
		int GetAbAcceptedOrderList(task_arg arg);
		int GetDmAcceptedOrderList(task_arg arg);

		int CheckDmAccountPassword(const std::string& account_no, const std::string& pwd);
		int CheckAbAccountPassword(const std::string& account_no, const std::string& pwd);

		bool _Enable{ true };

		int _CheckPwdReqId{ -1 };
		int GetChartDataDomestic(task_arg&& arg);
		int GetChartDataAbroadShort(task_arg&& arg);
		int GetChartDataAbroadLong(task_arg&& arg);

		int GetChartDataDomestic(SmTaskArg&& arg);
		int GetChartDataAbroadShort(SmTaskArg&& arg);
		int GetChartDataAbroadLong(SmTaskArg&& arg);
		/// <summary>
		/// �ϳ��� ��û�� ������ �� ȣ��Ǵ� �Լ�
		/// </summary>
		/// <param name="nRqId">�����簡 ���� ��û��ȣ</param>
		void OnTaskComplete(const int& nRqId);
		void OnChartTaskComplete(const int& nRqId);
		/// <summary>
		/// Ŭ���̾�Ʈ ��ü
		/// </summary>
		std::weak_ptr<ViStockClient> _Client;
		/// <summary>
		/// ���ǻ� ��ü
		/// </summary>
		CHDFCommAgent m_CommAgent;
		void OnDomesticSymbolCode(const CString& sTrCode, const LONG& nRqID);
		void OnAccountListReceived(const CString& sTrCode, const LONG& nRqID) ;
		void OnSymbolMaster(const CString& sTrCode, const LONG& nRqID);
		void on_ab_symbol_master(const CString& server_trade_code, const LONG& server_request_id);
		void OnDmSymbolMaster(const CString& sTrCode, const LONG& nRqID);
		void on_dm_symbol_master_file(const CString& server_trade_code, const LONG& server_request_id);
		void OnMasterFile(const CString& sTrCode, const LONG& nRqID);
		void OnSymbolQuote(const CString& sTrCode, const LONG& nRqID);
		void OnSymbolHoga(const CString& sTrCode, const LONG& nRqID);
		void OnAccountAsset(const CString& sTrCode, const LONG& nRqID);
		void OnDmAccountAsset(const CString& sTrCode, const LONG& nRqID);
		void OnAccountProfitLoss(const CString& sTrCode, const LONG& nRqID);
		void OnSymbolProfitLoss(const CString& sTrCode, const LONG& nRqID);
		void OnAcceptedList(const CString& sTrCode, const LONG& nRqID);
		void OnFilledList(const CString& sTrCode, const LONG& nRqID);
		void OnChartDataLong(const CString& sTrCode, const LONG& nRqID);
		void OnChartDataShort(const CString& sTrCode, const LONG& nRqID);
		void OnDomesticChartData(const CString& sTrCode, const LONG& nRqID);
		/// <summary>
		/// �ʱ⿡ �ؿ� �����͸� ���� �� ó���ϴ� �Լ� 
		/// �ʱ⿡�� ��û ���̵� �˻��ؼ� �ִٸ� �۾� �Ϸ� �뺸�� ���ش�. 
		/// </summary>
		/// <param name="sTrCode"></param>
		/// <param name="nRqID"></param>
		void OnChartDataLong_Init(const CString& sTrCode, const LONG& nRqID);
		/// <summary>
		/// �ʱ⿡ �ؿ� �����͸� ���� �� ó���ϴ� �Լ� 
		/// �ʱ⿡�� ��û ���̵� �˻��ؼ� �ִٸ� �۾� �Ϸ� �뺸�� ���ش�. 
		/// </summary>
		/// <param name="sTrCode"></param>
		/// <param name="nRqID"></param>
		void OnChartDataShort_Init(const CString& sTrCode, const LONG& nRqID);
		/// <summary>
		/// �ʱ⿡ ���� �����͸� ���� �� ó���ϴ� �Լ� 
		/// �ʱ⿡�� ��û ���̵� �˻��ؼ� �ִٸ� �۾� �Ϸ� �뺸�� ���ش�. 
		/// </summary>
		/// <param name="sTrCode"></param>
		/// <param name="nRqID"></param>
		void OnDomesticChartData_Init(const CString& sTrCode, const LONG& nRqID);

		// �ؿ� �ֹ� ����Ȯ��
		void on_ab_order_accepted(const CString& strKey, const LONG& nRealType);
		// �ؿ� �ֹ� ��ü�� 
		void on_ab_order_unfilled(const CString& strKey, const LONG& nRealType);
		// �ؿ� �ֹ� ü��
		void on_ab_order_filled(const CString& strKey, const LONG& nRealType);

		// ���� �ֹ� ����Ȯ��
		void on_dm_order_accepted(const CString& strKey, const LONG& nRealType);
		// ���� �ֹ� ��ü�� 
		void on_dm_order_unfilled(const CString& strKey, const LONG& nRealType);
		// ���� �ֹ� ü��
		void on_dm_order_filled(const CString& strKey, const LONG& nRealType);

		// �ؿ� �ǽð� �ü�
		void on_ab_future_quote(const CString& strKey, const LONG& nRealType);
		// �ؿ� �ǽð� ȣ��
		void on_ab_future_hoga(const CString& strKey, const LONG& nRealType);

		// ���� �ǽð� �ü�
		void on_dm_future_quote(const CString& strKey, const LONG& nRealType);
		// ���� �ǽð� ȣ��
		void on_dm_future_hoga(const CString& strKey, const LONG& nRealType);

		void on_dm_expected(const CString& strKey, const LONG& nRealType);

		void on_dm_commodity_future_quote(const CString& strKey, const LONG& nRealType);
		void on_dm_commodity_future_hoga(const CString& strKey, const LONG& nRealType);

		void on_dm_option_quote(const CString& strKey, const LONG& nRealType);
		void on_dm_option_hoga(const CString& strKey, const LONG& nRealType);

		void on_dm_order_position(const CString& strKey, const LONG& nRealType);
		std::map<int, DhTaskArg> request_map_;
		// Key : ������ ��û ��ȣ, value : argument map.
		std::map<int, task_arg> _ReqMap;
		// Key : Order Request Id, value : order request object
		std::map<int, std::shared_ptr<SmOrderRequest>> _OrderReqMap;
		// Key : req id, value : chart argument.
		std::map<int, SmChartDataReq> _ChartReqMap;
		// Key : ������ ��û ��ȣ, value : order request object.
		std::map<int, order_request_p> order_request_map;
		std::set<std::string> _RegSymbolSet;
		std::set<std::string> _RegAccountSet;
	};

}