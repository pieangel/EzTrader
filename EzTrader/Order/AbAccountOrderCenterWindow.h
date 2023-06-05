#pragma once
#include <map>
#include <string>
#include <set>
#include <memory>
#include "../SmGrid/SmOrderArea.h"
#include "../resource.h"
#include "../StatusBar/extstatuscontrolbar.h"
#include "../SmGrid/SmPositionArea.h"
#include "../SmGrid/SmQuoteArea.h"
#include "OrderSetView.h"
#include "SmOrderSettings.h"
#include "OrderWndConst.h"
#include "SmFilledRemainButton.h"
#include "SmRemainButton.h"
#include "../View/SymbolOrderView.h"
#include "../View/SymbolTickView.h"
#include "../View/SymbolPositionView.h"

// SmOrderWnd dialog
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
}


class SmOrderSetDialog;
class SmSymbolTableDialog;
class AbAccountOrderWindow;
class SmFundOrderDialog;
class AbAccountOrderCenterWindow : public CBCGPDialog
{
	DECLARE_DYNAMIC(AbAccountOrderCenterWindow)

public:
	static int DeltaOrderArea;
	AbAccountOrderCenterWindow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~AbAccountOrderCenterWindow();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_CENTER };
#endif
public:
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	bool Selected() const { return _Selected; }
	void Selected(bool val);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	//CBCGPGridCtrl m_wndPositionGrid;
	//CBCGPGridCtrl m_wndOrderGrid;
	void on_paramter_event(const DarkHorse::OrderSetEvent& event, const std::string& event_message, const bool enable);
	
public:
	void ArrangeCenterValue();
	void CreateResource();
	virtual BOOL OnInitDialog();
	void SetQuote(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void SetHoga(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void OnClickSymbol(const std::string& symbol_info);
	//CComboBox _ComboAccount;
	CBCGPComboBox _ComboSymbol;
	SymbolOrderView symbol_order_view_;
	SymbolTickView symbol_tick_view_;
	SymbolPositionView  symbol_position_view_;
	OrderSetView order_set_view_;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnEnterSizeMove(WPARAM, LPARAM);
	afx_msg LRESULT OnExitSizeMove(WPARAM, LPARAM);

	void SetMainDialog(AbAccountOrderWindow* main_dialog);
	void SetFundDialog(SmFundOrderDialog* fund_dialog);
	int ID() const { return _ID; }
	void ID(int val) { _ID = val; }
	void SetSelected(const bool& selected);
	void OnSymbolClicked(const std::string& symbol_code);
	void SetOrderAmount(const int& count);
	int GetPositionCount();
private:
	void SetSymbolInfo(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	bool _Selected = false;
	void UpdateOrderSettings();
	//DarkHorse::SmOrderSettings _OrderSettings;
	void SetCutMode();
	int _ID{ 0 };
	int _CutMode{ 0 };
	bool _Resizing = false;
	///void ClearOldHoga(Hoga_Type hoga_type) const noexcept;
	//int FindValueRow(const int& value) const noexcept;
	/// <summary>
	/// key : combobox index, value : symbol info including symbol code
	/// </summary>
	//std::map<int, std::string> _ComboIndexToSymbolMap;
	// key : combobox index, value : symbol object
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _IndexToSymbolMap;
	int _CurrentIndex{ -1 };
	/// <summary>
	/// Key : row index, Value : Quote Value in integer.
	/// </summary>
	std::map<int, int> _QuoteToRowIndexMap;
	int _CloseRow{ 15 };
	int _ValueStartRow{ 1 };
	std::set<int> _OldHogaBuyRowIndex;
	std::set<int> _OldHogaSellRowIndex;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	//CExtStatusControlBar m_bar;
	bool _Init = false;
	bool _ShowQuoteArea = true;
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	void SetInfo(std::shared_ptr<DarkHorse::SmSymbol> symbol);

	SmFilledRemainButton _FilledRemainButton;
	SmRemainButton _RemainButton;
	std::shared_ptr< SmOrderSetDialog> order_set_dialog_ = nullptr;

public:
	void SetRowWide();
	void SetRowNarrow();
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	int RecalcOrderAreaHeight(CWnd* wnd, bool bottom_up = true);
	afx_msg void OnCbnSelchangeComboSymbol();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CBCGPStatic _StaticAccountName;
	CBCGPStatic _StaticSymbolName;
	afx_msg void OnBnClickedCheckShowRealQuote();
	CBCGPButton _CheckShowRealTick;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedBtnRefreshOrder();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtnSymbol();
	afx_msg LRESULT OnUmSymbolSelected(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnAmount1();
	afx_msg void OnBnClickedBtnAmount2();
	afx_msg void OnBnClickedBtnAmount5();
	afx_msg void OnBnClickedBtnAmount3();
	afx_msg void OnBnClickedBtnAmount4();
	CBCGPSpinButtonCtrl _OrderAmountSpin;
	afx_msg void OnEnChangeEditAmount();
	CBCGPStatic _StaticFilledCount;
	afx_msg void OnBnClickedBtnLiqSymbolPosition();
	//CBCGPGroup _GroupFillSet;
	CBCGPButton _RadioMarket;
	CBCGPButton _RadioPrice;
	CBCGPButton _CheckLoss;
	CBCGPButton _CheckProfit;
	CBCGPEdit _EditLoss;
	CBCGPEdit _EditProfit;
	CBCGPEdit _EditSlip;
	CBCGPEdit _EditAmount;
	CBCGPSpinButtonCtrl _SpinLossCut;
	CBCGPSpinButtonCtrl _SpinProfitCut;
	CBCGPSpinButtonCtrl _SpinSlip;
	afx_msg void OnBnClickedCheckProfit();
	afx_msg void OnBnClickedCheckLoss();
	afx_msg void OnBnClickedRadioMarket();
	afx_msg void OnBnClickedRadioPrice();
	afx_msg void OnEnChangeEditProfit();
	afx_msg void OnEnChangeEditLoss();
	afx_msg void OnEnChangeEditSlip();
	CBCGPButton _BtnSearch;
	afx_msg void OnBnClickedBtnSearch();
	afx_msg void OnBnClickedBtnSet();
	CBCGPGroup _Group2;
	CBCGPGroup _Group3;
	CBCGPGroup _Group4;
	CBCGPSpinButtonCtrl _BuyAvail;
	CBCGPSpinButtonCtrl _SellAvail;
	afx_msg void OnStnClickedStaticFilledRemain();
	afx_msg void OnBnClickedCheckFixHoga();
};
