#pragma once
#include "afxdialogex.h"
#include "IYOA.h"
#include <memory>
#include <map>
#include <set>
#include <string>
#include <any>
#include <windows.h>
#include "afxdialogex.h"
#include "../Json/json.hpp"
#include "../Task/SmTaskArg.h"
#include "../Task/SmTaskDefine.h"
using order_event = nlohmann::json;
// YaClient dialog
enum class SERVER_REQ : int { 
	DM_ORDER_NEW,
	DM_ORDER_MOD,
	DM_ORDER_CANCEL,
	DM_ORDER_FILLED,
	DM_ORDER_ORDERABLE,
	DM_ASSET,
	DM_DEPOSIT,
	DM_PROVISIONAL_SETTLEMENT,
	DM_ACCEPTED,
	DM_POSITION_INFO,
	DM_DAILY_PROFIT_LOSS,
	DM_LIQUIDABLE_QTY,
	DM_TRADE_PROFIT_LOSS,
	DM_OUTSTANDING_ORDER,
	DM_FUT_SISE,
	DM_FUT_HOGA,
	DM_OPT_SISE,
	DM_OPT_HOGA,
	DM_COMMODITY_SISE,
	DM_COMMODITY_HOGA,
	DM_CHART_N_MIN,
	DM_ORDER_NEW_RES,
	DM_ORDER_MOD_RES,
	DM_ORDER_CANCEL_RES,
};

struct YA_REQ_INFO {
	SERVER_REQ req;
	std::string dso_name;
	std::string desc;
	int request_id;
};
namespace DarkHorse {
	class YaStockClient;
	class YaClient : public CDialog
	{
		DECLARE_DYNAMIC(YaClient)

	public:
		//YaClient(CWnd* pParent = nullptr);   // standard constructor
		YaClient(CWnd* pParent, YaStockClient& ya_stock_client);
		virtual ~YaClient();

		// Dialog Data
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_YA_CLIENT };
#endif

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()

	private:
		IYOA g_iYuantaAPI;
		BOOL g_bInitial;
		BOOL g_bLogin;
		int g_nStartMsgID;
		YaStockClient& ya_stock_client_;

		afx_msg LRESULT OnReceiveError(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnReceiveData(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnReceiveRealData(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnReceiveSystemMessage(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnLoginComplete(WPARAM wParam, LPARAM lParam);
	public:
		void init();
		void log_in();
		afx_msg void OnDestroy();
	};
}
