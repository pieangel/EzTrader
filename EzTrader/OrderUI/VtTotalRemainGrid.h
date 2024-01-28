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
}

using symbol_p = std::shared_ptr<DarkHorse::SmSymbol>;
using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;

class VtOrderConfigManager;
class VtSymbol;

class VtTotalRemainGrid : public CGridCtrl
{
public:
	VtTotalRemainGrid();
	~VtTotalRemainGrid();

	void Init();

	void SetRowTitle();
	int _ColCount = 2;
	int _RowCount = 4;
	CFont _defFont;
	CFont _titleFont;
	int _CellHeight;

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }

	void InitGrid();
	void OnOutstanding();
	void OnReceiveQuote(VtSymbol* sym);
	void OnReceiveAccountInfo();

	// 콜백 함수 취소
	void UnregisterAllCallback();

	void RegisterQuoteCallback();
	void UnregisterQuoteCallback();
	void OnQuoteEvent(VtSymbol* symbol);

private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	void ShowAccountProfitLoss();
	void ShowFundProfitLoss();

	void QuickSetBackColor(int row, int col, COLORREF color);
	void QuickSetText(int row, int col, LPCTSTR text);
	void QuickSetNumber(int row, int col, int number);
	void QuickSetTextColor(int row, int col, COLORREF color);
};


