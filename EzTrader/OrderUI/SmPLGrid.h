#pragma once
#include "../Grid/GridCtrl.h"
#include <vector>
#include <map>
#include <set>
#include <functional>
#include "../VtDefine.h"
#include <memory>
#include <map>
#include <tuple>
#include <set>
#include <string>

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
	class AccountProfitLossControl;
}

using symbol_p = std::shared_ptr<DarkHorse::SmSymbol>;
using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;
class VtOrderConfigManager;
class VtSymbol;

class SmPLGrid : public CGridCtrl
{
public:
	SmPLGrid();
	~SmPLGrid();

	void Init();

	void SetRowTitle();
	int _ColCount = 2;
	int _RowCount = 3;
	CFont _defFont;
	CFont _titleFont;
	int _CellHeight;

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }

	void InitGrid();
	void OnOutstanding();
	void OnReceiveQuote(symbol_p sym);
	void OnReceiveAccountInfo();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	void ShowAccountProfitLoss();
	void ShowFundProfitLoss();

	void QuickSetBackColor(int row, int col, COLORREF color);
	void QuickSetText(int row, int col, LPCTSTR text);
	void QuickSetNumber(int row, int col, int number);
	void QuickSetTextColor(int row, int col, COLORREF color);
public:
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return fund_; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
public:
	void on_update_account_profit_loss();
	account_p Account() const { return account_; }
	void Account(account_p val);
	symbol_p Symbol() const { return _Symbol; }
	void Symbol(symbol_p val) { _Symbol = val; }
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
};

