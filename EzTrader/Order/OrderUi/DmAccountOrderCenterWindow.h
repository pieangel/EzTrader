/*
#pragma once
class DmAccountOrderCenterWindow
{
};
*/
#pragma once
#include <map>
#include <string>
#include <set>
#include <memory>
//#include "../../SmGrid/SmOrderArea.h"
#include "../../resource.h"
#include "../../StatusBar/extstatuscontrolbar.h"
//#include "../../SmGrid/SmPositionArea.h"
//#include "../../SmGrid/SmQuoteArea.h"
#include "../SmOrderSetGrid.h"
#include "../SmOrderSettings.h"
#include "../OrderWndConst.h"
#include "../SmFilledRemainButton.h"
#include "../SmRemainButton.h"
#include "../../View/SymbolOrderView.h"
#include "../../View/SymbolPositionView.h"
#include "../../View/SymbolTickView.h"

// SmOrderWnd dialog
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
}


class SmOrderSetDialog;
class SmSymbolTableDialog;
class DmAccountOrderWindow;
class DmFundOrderWindow;
class DmAccountOrderCenterWindow : public CBCGPDialog
{
	DECLARE_DYNAMIC(DmAccountOrderCenterWindow)

public:
	static int DeltaOrderArea;
	DmAccountOrderCenterWindow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DmAccountOrderCenterWindow();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DM_ACNT_ORDER_CENTER};
#endif
public:
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return account_; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	bool Selected() const { return selected_; }
	void Selected(bool val);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	//CBCGPGridCtrl m_wndPositionGrid;
	//CBCGPGridCtrl m_wndOrderGrid;


public:
	void ArrangeCenterValue();
	void CreateResource();
	virtual BOOL OnInitDialog();
	void SetQuote(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void SetHoga(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void OnClickSymbol(const std::string& symbol_info);
	//CComboBox _ComboAccount;
	SymbolOrderView symbol_order_view_;
	SymbolTickView symbol_tick_view_;
	SymbolPositionView  symbol_position_view_;
	SmOrderSetGrid order_set_view_;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnEnterSizeMove(WPARAM, LPARAM);
	afx_msg LRESULT OnExitSizeMove(WPARAM, LPARAM);

	void SetMainDialog(DmAccountOrderWindow* main_dialog);
	int ID() const { return id_; }
	void ID(int val) { id_ = val; }
	void SetSelected(const bool& selected);
	void OnSymbolClicked(const std::string& symbol_code);
	void SetOrderAmount(const int& count);
	int GetPositionCount();
private:
	void set_symbol(std::shared_ptr<DarkHorse::SmSymbol>symbol);
	void set_symbol_name(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	std::string make_symbol_name(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	int add_to_symbol_combo(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void init_control();
	void init_views();
	void init_dm_symbol();
	void set_symbol_from_out(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	bool selected_ = false;
	void UpdateOrderSettings();
	//DarkHorse::SmOrderSettings _OrderSettings;
	void SetCutMode();
	int id_{ 0 };
	int profit_loss_cut_mode_{ 0 };
	bool resizing_ = false;
	///void ClearOldHoga(Hoga_Type hoga_type) const noexcept;
	//int FindValueRow(const int& value) const noexcept;
	/// <summary>
	/// key : combobox index, value : symbol info including symbol code
	/// </summary>
	//std::map<int, std::string> _ComboIndexToSymbolMap;
	// key : combobox index, value : symbol object
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> index_to_symbol_;
	std::map<std::string, int> symbol_to_index_;
	int current_combo_index_{ -1 };
	/// <summary>
	/// Key : row index, Value : Quote Value in integer.
	/// </summary>
	std::map<int, int> quote_to_row_map;
	int close_row_{ 15 };
	int value_start_row_{ 1 };
	std::set<int> old_hoga_buy_row;
	std::set<int> old_hoga_sell_row;
	std::shared_ptr<DarkHorse::SmSymbol> symbol_ = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> account_ = nullptr;
	//CExtStatusControlBar m_bar;
	bool init_dialog_ = false;
	bool show_symbol_tick_view_ = true;
	std::shared_ptr< SmSymbolTableDialog> symbol_table_dialog_ = nullptr;
	void set_symbol_info(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void request_dm_symbol_master(const std::string symbol_code);

	SmFilledRemainButton filled_remain_button_;
	SmRemainButton remain_button_;
	std::shared_ptr< SmOrderSetDialog> order_set_dialog_ = nullptr;

public:
	void SetRowWide();
	void SetRowNarrow();
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	int RecalcOrderAreaHeight(CWnd* wnd, bool bottom_up = true);
	afx_msg void OnCbnSelchangeComboSymbol();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CBCGPStatic _StaticAccountName;
	CBCGPStatic static_symbol_name_;
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
	CBCGPComboBox combo_symbol_;
	afx_msg void OnStnClickedStaticFilledRemain();
	afx_msg void OnBnClickedCheckFixHoga();
};


