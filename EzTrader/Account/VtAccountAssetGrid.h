#pragma once
#include "../UGrid/VtGrid.h"
#include <vector>
#include <memory>
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
	class AccountAssetControl;
	class AccountProfitLossControl;
}
class VtAssetPage;
class VtAccountAssetGrid : public VtGrid
{
public:
	VtAccountAssetGrid();
	virtual ~VtAccountAssetGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);

	void SetRowTitle();
	int _ColCount = 4;
	int _RowCount = 5;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	VtAssetPage* AssetPage() const { return _AssetPage; }
	void AssetPage(VtAssetPage* val) { _AssetPage = val; }
private:
	VtAssetPage* _AssetPage = nullptr;
	std::vector<int> _ColWidVec;
public:
	void InitGrid();
public:
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return fund_; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
public:
	void refresh();
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return account_; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	void SetAssetInfo();
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void on_update_account_profit_loss();
private:
	bool updating_ = false;
	void update_account_profit_loss();
	void SetAccountAssetInfo();
	void SetFundAssetInfo();
	// 0 : account, 1 : fund
	int _Mode = 0;
	bool _EnableOrderShow = false;
	bool _EnableQuoteShow = false;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> account_ = nullptr;
	std::shared_ptr<DarkHorse::SmFund> fund_ = nullptr;

	std::shared_ptr<DarkHorse::AccountProfitLossControl> account_profit_loss_control_;
	std::shared_ptr<DarkHorse::AccountAssetControl> asset_control_;

	bool enable_account_profit_loss_show_ = false;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

