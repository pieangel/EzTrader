// MainFrm.h : interface of the CMainFrame class
//


#pragma once
#include "SymbolBar.h"
#include "WorkSpaceBar2.h"
#include "OutputBar.h"
#include "PropertiesViewBar.h"
#include <memory>
#include <map>
namespace DarkHorse {
class SmChartData;
}

class CTestDialog;
class SmOrderWnd;
class OrderLeftDialog;
class OrderRightDialog;
class SmMainOrderDialog;
class SmFundOrderDialog;
class SmFundDialog;
class SmOrderCompMainDialog;
class SmFilledListDialog;
class SmTotalAssetDialog;
class SmJangGoDialog;
class SmFundCompMainDialog;
class GaSpreadChart;
class SmMultiSpreadChart;
class SmCompChart;
class SmGigaSpreadChart;
class SmGigaCompChart;
class RealTimeMultiChart;
class SmStockChart;
class SmTrendChart;
class DmAccountOrderWindow;
class DmFundOrderWindow;
class CMainFrame : public CBCGPMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CBCGPStatusBar			m_wndStatusBar;
	CBCGPMenuBar			m_wndMenuBar;
	CBCGPExplorerToolBar			m_wndToolBar;
	CSymbolBar			    m_wndWorkSpace;
	CWorkSpaceBar2			m_wndWorkSpace2;
	COutputBar				m_wndOutput;
	CBCGPPropBar			m_wndPropGrid;

// Generated message map functions
protected:
	int						m_nColorTheme;
	BOOL					m_bIsDarkTheme;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	afx_msg void OnWindowManager();
	afx_msg void OnMdiMoveToNextGroup();
	afx_msg void OnMdiMoveToPrevGroup();
	afx_msg void OnMdiNewHorzTabGroup();
	afx_msg void OnMdiNewVertGroup();
	afx_msg void OnMdiCancel();
	afx_msg void OnClose();
	afx_msg void OnColorThemeCombo();
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop);
	virtual CBCGPMDIChildWnd* CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* /*pObj*/);

public:
	// window id
	static int _Id;
	static int GetId() { return ++_Id; }

	void StartLoad();
	int	GetColorTheme() const
	{
		return m_nColorTheme;
	}

	BOOL IsDarkTheme() const
	{
		return m_bIsDarkTheme;
	}
	void StartDataRequest();
	void SetMarketTree();
	void SetAccountInfo();
	void UpdateChart();
	void SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& window_id, const int& series_index = 0);
	afx_msg void OnTestOrderwnd();
	std::shared_ptr<SmOrderWnd> _OrderWnd = nullptr;
	afx_msg void OnTestLeftorderdialog();

	std::shared_ptr<OrderLeftDialog> _LeftOrderWnd = nullptr;
	std::shared_ptr<OrderRightDialog> _RightOrderWnd = nullptr;
	std::shared_ptr<SmMainOrderDialog> _MainOrderWnd = nullptr;
	std::shared_ptr<SmFundOrderDialog> _FundOrderWnd = nullptr;
	std::shared_ptr< SmOrderCompMainDialog> _CompOrderWnd = nullptr;
	std::shared_ptr<CTestDialog> _TestDlg = nullptr;
	std::shared_ptr< SmFundDialog> _FundDialog = nullptr;
	std::shared_ptr< SmFilledListDialog> _FilledDialog = nullptr;
	std::shared_ptr< SmTotalAssetDialog> _TotalAssetDialog = nullptr;
	std::shared_ptr< SmJangGoDialog> _AccountJangoDialog = nullptr;
	std::shared_ptr< SmJangGoDialog> _FundJangoDialog = nullptr;
	std::shared_ptr< SmFundCompMainDialog> _FundCompOrderDialog = nullptr;

	std::shared_ptr<DmAccountOrderWindow> dm_account_order_wnd_p_ = nullptr;
	std::shared_ptr<DmFundOrderWindow> dm_fund_order_wnd_p = nullptr;
	std::map<HWND, std::shared_ptr<DmAccountOrderWindow>> dm_account_order_wnd_map_;
	std::map<HWND, std::shared_ptr<DmFundOrderWindow>> dm_fund_order_wnd_map_;

	std::map<HWND, std::shared_ptr<SmMainOrderDialog>> _OrderWndMap;

	std::map<HWND, std::shared_ptr<SmOrderCompMainDialog>> _CompOrderWndMap;
	std::map<HWND, std::shared_ptr<SmJangGoDialog>> _JangoWndMap;
	std::map<HWND, std::shared_ptr<SmFilledListDialog>> _FilledWndMap;

	std::map<HWND, std::shared_ptr<SmFundOrderDialog>> _FundOrderWndMap;

	std::map<HWND, std::shared_ptr<SmFundCompMainDialog>> _FundCompOrderWndMap;

	std::map<HWND, std::shared_ptr<SmTotalAssetDialog>> _AssetWndMap;

	std::map<HWND, std::shared_ptr<GaSpreadChart>> _ChartMap;
	std::map<HWND, std::shared_ptr<SmMultiSpreadChart>> _SpreadChartMap;

	std::map<HWND, std::shared_ptr<SmCompChart>> _CompChartMap;
	std::map<HWND, std::shared_ptr<SmGigaSpreadChart>> _GigaSpreadChartMap;
	std::map<HWND, std::shared_ptr<SmStockChart>> _StockChartMap;

	std::map<HWND, std::shared_ptr<RealTimeMultiChart>> _RealTimeChartMap;

	std::map<HWND, std::shared_ptr<SmTrendChart>> _TrendChartMap;

	void RemoveOrderWnd(HWND wnd);
	void RemoveFundOrderWnd(HWND wnd);
	void RemoveJangoWnd(HWND wnd);
	void RemoveFilledWnd(HWND wnd);
	void RemoveCompWnd(HWND wnd);
	void RemoveFundCompWnd(HWND wnd);
	void RemoveAssetWnd(HWND wnd);
	afx_msg void OnTestRightorderdialog();
	afx_msg void OnTestOrdermain();
	void ReconnectOrderEvent();
	afx_msg void OnTestSymboltable();
	afx_msg void OnTestFund();
	afx_msg void OnOrderOrdermain();
	afx_msg void OnOrderOrderfund();
	afx_msg void OnOrderEditfund();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnFilledList();
	afx_msg void OnMultiOrderForAccount();
	afx_msg void OnServerMsg();
	afx_msg void OnOpenSettings();
	afx_msg void OnAsset();
	afx_msg void OnManagePwd();
	afx_msg void OnOrderWnd();
	afx_msg void OnComplexOrder();
	afx_msg void OnOrderRemain();
	afx_msg void OnEditFund();
	afx_msg void OnFundOrder();
	afx_msg void OnFundRemain();
	afx_msg void OnFundPl();
	afx_msg void OnNewChart();
	afx_msg void OnStratege1();
	afx_msg void OnFundComplexOrder();
	afx_msg void OnSpreadChart();
	afx_msg void OnChartComp();
	afx_msg void OnGigaSpreadChart();
	afx_msg void OnGigaCompChart();
	afx_msg void OnStockChart();
	afx_msg void OnRealtimeMultiChart();
	afx_msg void OnMulticolorChart();
	afx_msg void OnDomesticAccountOrder();
	afx_msg void OnDomesticFundOrder();
};
