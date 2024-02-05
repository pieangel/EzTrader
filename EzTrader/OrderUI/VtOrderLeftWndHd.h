#pragma once
#include "afxwin.h"
#include "SmOptionGrid.h"
#include "SmFutureGrid.h"
#include "SmPLGrid.h"
#include "SmAssetGrid.h"
#include <memory>

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}

using symbol_p = std::shared_ptr<DarkHorse::SmSymbol>;
using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;

// VtOrderLeftHd dialog
class VtOrderConfigManager;
class VtSymbol;
struct VtPosition;
struct VtOrder;
const int FixedHeight = 313;
class VtOrderLeftWndHd : public CDialog
{
	DECLARE_DYNAMIC(VtOrderLeftWndHd)

public:
	VtOrderLeftWndHd(CWnd* pParent = NULL);   // standard constructor
	//VtOrderLeftWndHd();
	virtual ~VtOrderLeftWndHd();
	//virtual int CRHGetDialogID();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_LEFT_HD };
#endif

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	int DefaultWidth() const { return _DefaultWidth; }
	void DefaultWidth(int val) { _DefaultWidth = val; }
	int DefaultHeight() const { return _DefaultHeight; }
	void DefaultHeight(int val) { _DefaultHeight = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	void OnReceiveQuote(symbol_p sym);
	void OnOutstanding();
	void OnExpected(symbol_p sym);
	void BlockEvent();
private:
	//HdSymbolFutureGrid _SymbolFutureGrid;
	SmFutureGrid _SymbolFutureGrid;
	//HdProfitLossGrid _ProfitLossGrid;
	SmPLGrid _ProfitLossGrid;
	//HdAssetGrid      _AssetGrid;
	SmAssetGrid      _AssetGrid;
	//HdSymbolOptionGrid _SymbolOptionGrid;
	SmOptionGrid _SymbolOptionGrid;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;

	void Begin();
	void End();

	int _FutureSymbolMode = 1;
	int _Mode = 1;
	void set_option_view();
	// key : option year-month combo index, value : year-month name
	std::map<int, std::string> option_yearmonth_index_map;
	int year_month_index{ 0 };
	int option_market_index{ 0 };
	void init_option_market();
public:
	CComboBox combo_option_market_;
	CComboBox combo_option_month_;
	afx_msg void OnCbnSelchangeComboProduct();
	afx_msg void OnCbnSelchangeComboOption();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedRadioBalance();
	afx_msg void OnBnClickedRadioCurrent();
	afx_msg void OnBnClickedRadioExpect();
	afx_msg LRESULT OnAccountChangedMessage(WPARAM wParam, LPARAM lParam);
	void OnSymbolMaster(symbol_p sym);
public:
	void OnRemain(VtPosition* posi);
	void OnOrderAccepted(VtOrder* order);
	void OnOrderUnfilled(VtOrder* order);
	void OnOrderFilled(VtOrder* order);
	void OnReceiveAccountInfo();
	void RefreshProfitLoss();
	void RefreshAsset();
	void RefreshLayout();
	void OnAccountChanged();
private:
	int _EventSeq = 0;
	void OnOrderEvent(VtOrder* order);
	int _DefaultWidth = 174;
	int _DefaultHeight = 774;
};
