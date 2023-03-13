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
// ViClient dialog
namespace DarkHorse {
	/// <summary>
	/// 데이터 범위를 한정해라. 
	/// 요청 정보가 들어 왔을 때 응답하는 정보가 요청 정보를 포함하지 않을 때
	/// 요청 정보를 임시로 저장했다가 다시 사용한다. 
	/// 절대로 최초로 요청을 내린 곳으로 다시 가서 정보를 찾아 오지 않는다.
	/// 이미 떠난 요청을 다시 찾는 수고를 해서는 절대로 안된다.
	/// 용어는 반드시 통일한다. 
	/// 같은 의미를 지니는 변수는 여러 곳에서 사용될 때 반드시 같은 변수 이름을 사용할 것
	/// 이렇게 하지 않으면 매우 혼란을 일으키므로 반드시 이 규칙을 지킬 것.
	/// 변수 이름에 대하여 표를 만들어라. 그러면 반드시 성공할 것이다.
	/// 이름에 대하여 일관성을 지킬 것
	/// 같은 의미에 대하여 여기서는 이렇게 저기서는 저렇게 짓지 말 것.
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
		/// 하나의 요청이 끝났을 때 호출되는 함수
		/// </summary>
		/// <param name="nRqId">선물사가 만든 요청번호</param>
		void OnTaskComplete(const int& nRqId);
		void OnChartTaskComplete(const int& nRqId);
		/// <summary>
		/// 클라이언트 객체
		/// </summary>
		std::weak_ptr<ViStockClient> _Client;
		/// <summary>
		/// 증권사 객체
		/// </summary>
		CHDFCommAgent m_CommAgent;
		void OnDomesticSymbolCode(const CString& sTrCode, const LONG& nRqID);
		void OnAccountListReceived(const CString& sTrCode, const LONG& nRqID) ;
		void OnSymbolMaster(const CString& sTrCode, const LONG& nRqID);
		void OnDmSymbolMaster(const CString& sTrCode, const LONG& nRqID);
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
		/// 초기에 해외 데이터를 받을 때 처리하는 함수 
		/// 초기에는 요청 아이디를 검사해서 있다면 작업 완료 통보를 해준다. 
		/// </summary>
		/// <param name="sTrCode"></param>
		/// <param name="nRqID"></param>
		void OnChartDataLong_Init(const CString& sTrCode, const LONG& nRqID);
		/// <summary>
		/// 초기에 해외 데이터를 받을 때 처리하는 함수 
		/// 초기에는 요청 아이디를 검사해서 있다면 작업 완료 통보를 해준다. 
		/// </summary>
		/// <param name="sTrCode"></param>
		/// <param name="nRqID"></param>
		void OnChartDataShort_Init(const CString& sTrCode, const LONG& nRqID);
		/// <summary>
		/// 초기에 국내 데이터를 받을 때 처리하는 함수 
		/// 초기에는 요청 아이디를 검사해서 있다면 작업 완료 통보를 해준다. 
		/// </summary>
		/// <param name="sTrCode"></param>
		/// <param name="nRqID"></param>
		void OnDomesticChartData_Init(const CString& sTrCode, const LONG& nRqID);

		// 해외 주문 접수확인
		void OnOrderAccepted(const CString& strKey, const LONG& nRealType);
		// 해외 주문 미체결 
		void OnOrderUnfilled(const CString& strKey, const LONG& nRealType);
		// 해외 주문 체결
		void OnOrderFilled(const CString& strKey, const LONG& nRealType);

		// 해외 실시간 시세
		void OnRealtimeQuote(const CString& strKey, const LONG& nRealType);
		// 해외 실시간 호가
		void OnRealtimeHoga(const CString& strKey, const LONG& nRealType);

		// 국내 실시간 시세
		void OnRealtimeDomesticQuote(const CString& strKey, const LONG& nRealType);
		// 국내 실시간 호가
		void OnRealtimeDomesticHoga(const CString& strKey, const LONG& nRealType);
		// Key : 선물사 요청 번호, value : argument map.
		std::map<int, task_arg> _ReqMap;
		// Key : Order Request Id, value : order request object
		std::map<int, std::shared_ptr<SmOrderRequest>> _OrderReqMap;
		// Key : req id, value : chart argument.
		std::map<int, SmChartDataReq> _ChartReqMap;
		// Key : 선물사 요청 번호, value : order request object.
		std::map<int, order_request_p> order_request_map;
		std::set<std::string> _RegSymbolSet;
		std::set<std::string> _RegAccountSet;
	};

}