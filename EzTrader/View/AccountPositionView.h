/*
#pragma once
class AccountPositionView
{
};
*/
#pragma once



#pragma once
#include <BCGCBProInc.h>
#include <memory>
#include <vector>
#include <string>
#include <set>
#include <map>

namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	struct SmPosition;
	class SmFund;
	struct Position;
	class AccountPositionControl;
}

class DmAccountOrderWindow;
class DmFundOrderWindow;
class SmOrderCompMainDialog;
class SmFundCompMainDialog;
class AccountPositionView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(AccountPositionView)

		// Construction
public:
	void start_timer();
	AccountPositionView();

	std::shared_ptr<DarkHorse::SmFund> Fund() const { return _Fund; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	SmOrderCompMainDialog* CompOrderWnd() const { return _CompOrderWnd; }
	void CompOrderWnd(SmOrderCompMainDialog* val) { _CompOrderWnd = val; }
	// Attributes
protected:
	BOOL m_bExtendedPadding;

	// Operations
public:
	void SetFundOrderWnd(DmFundOrderWindow* fund_order_wnd) {
		_FundOrderWnd = fund_order_wnd;
	}

	void SetMainWnd(DmAccountOrderWindow* main_wnd) {
		_OrderWnd = main_wnd;
	}
	void ToggleExtendedPadding();
	BOOL IsExtendedPadding() const
	{
		return m_bExtendedPadding;
	}

	// Overrides
	virtual int GetTextMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetTextMargin();
	}

	virtual int GetTextVMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetTextVMargin();
	}

	virtual int GetImageMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetImageMargin();
	}

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicGridCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~AccountPositionView();

	virtual void OnHeaderCheckBoxClick(int 	nColumn);

	virtual void OnRowCheckBoxClick(CBCGPGridRow* pRow);


	std::shared_ptr<DarkHorse::SmAccount> Account() const { return account_; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }

	void UpdateAcceptedOrder();
	void UpdatePositionInfo();
	void LiqSelPositions();
	void LiqAll();
	void on_update_account_position();
	// Generated message map functions
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void on_timer();
	void ClearCheck();
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void OnQuoteEvent(const std::string& symbol_code);
	SmFundCompMainDialog* CompFundWnd() const { return _CompFundWnd; }
	void CompFundWnd(SmFundCompMainDialog* val) { _CompFundWnd = val; }
	void set_column_widths(std::vector<int> column_width_vector);
private:
	void LiqSelPositionsForAccount();
	void LiqSelPositionsForFund();
	void LiqAllForAccount();
	void LiqAllForFund();
	SmFundCompMainDialog* _CompFundWnd = nullptr;
	DmFundOrderWindow* _FundOrderWnd = nullptr;
	DmAccountOrderWindow* _OrderWnd = nullptr;
	SmOrderCompMainDialog* _CompOrderWnd = nullptr;
	// 0 : account , 1 : fund
	int _Mode = 0;
	void update_account_position();
	void UpdateAccountPositionInfo();
	void UpdateFundPositionInfo();
	bool _EnableQuoteShow = false;
	bool enable_position_show_ = false;
	COLORREF _DefaultBackColor;
	bool _HeaderCheck = false;
	int _OldMaxRow = -1;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> account_ = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;

	std::set<int> _OldContentRowSet;
	void ClearOldContents();
	void ClearOldContents(const int& last_index);
	// key : row, value : SmPosition Object
	std::map<int, std::shared_ptr<DarkHorse::SmPosition>> _RowToPositionMap;
	std::shared_ptr<DarkHorse::AccountPositionControl> account_position_control_;
	std::map<int, std::shared_ptr<DarkHorse::Position>> row_to_position_;
	std::vector<int> column_widths_vector_;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
