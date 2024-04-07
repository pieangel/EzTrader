#pragma once
#include "../UGrid/VtGrid.h"
#include <memory>
class HdAccountPLDlg;
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
	class AccountProfitLossControl;
}
class HdAccountPLGrid : public VtGrid
{
public:
	HdAccountPLGrid();
	virtual ~HdAccountPLGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);

	void SetRowTitle();
	int _ColCount = 1;
	int _RowCount = 4;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);

	void Begin();
	void End();

	void InitGrid();

	HdAccountPLDlg* AcntPLDlg() const { return _AcntPLDlg; }
	void AcntPLDlg(HdAccountPLDlg* val) { _AcntPLDlg = val; }
	void ClearValues();
private:
	HdAccountPLDlg* _AcntPLDlg = nullptr;
public:
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return fund_; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
public:
	void on_update_account_profit_loss();
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return account_; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	int order_window_id() const { return order_window_id_; }
	void order_window_id(int val) { order_window_id_ = val; }
	void update_account_profit_loss();
	void update_fund_profit_loss();
private:
	bool updating_ = false;
	int order_window_id_ = 0;
	std::string get_format_price(const double& value);
	bool enable_account_profit_loss_show_ = false;
	bool _EnableQuoteShow = false;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> account_ = nullptr;
	std::shared_ptr<DarkHorse::SmFund> fund_ = nullptr;
	std::shared_ptr<DarkHorse::AccountProfitLossControl> account_profit_loss_control_;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

