#pragma once
#include "../Grid/GridCtrl.h"

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
	class AccountAssetControl;
	class AccountProfitLossControl;
}

using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;

class VtOrderConfigManager;
class SmAssetGrid : public CGridCtrl
{
public:
	SmAssetGrid();
	~SmAssetGrid();

	void Init();

	void SetRowTitle();
	int _ColCount = 2;
	int _RowCount = 2;
	CFont _defFont;
	CFont _titleFont;
	int _CellHeight;
	void InitGrid();
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	void OnReceiveAccountInfo();
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;

	void QuickSetBackColor(int row, int col, COLORREF color);
	void QuickSetText(int row, int col, LPCTSTR text);
	void QuickSetNumber(int row, int col, int number);
	void QuickSetTextColor(int row, int col, COLORREF color);
public:
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return fund_; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
public:
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return account_; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void SetAssetInfo();
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void on_update_account_profit_loss();
private:
	bool updating_ = false;
	void update_account_profit_loss();
	std::shared_ptr<DarkHorse::AccountProfitLossControl> account_profit_loss_control_;
	std::shared_ptr<DarkHorse::AccountAssetControl> asset_control_;
	void SetAccountAssetInfo();
	void SetFundAssetInfo();
	// 0 : account, 1 : fund
	int _Mode = 0;
	bool enable_account_profit_loss_show_ = false;
	bool _EnableQuoteShow = false;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> account_ = nullptr;
	std::shared_ptr<DarkHorse::SmFund> fund_ = nullptr;
};

