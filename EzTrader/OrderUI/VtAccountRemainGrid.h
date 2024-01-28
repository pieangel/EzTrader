#pragma once
#include <memory>
#include "afxwin.h"
#include "../UGrid/VtGrid.h"
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}

using symbol_p = std::shared_ptr<DarkHorse::SmSymbol>;
using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;

class VtOrderConfigManager;
class VtAccount;
class VtSymbol;
struct VtOrder;
class VtAccountRemainGrid : public VtGrid
{
public:
	VtAccountRemainGrid();
	~VtAccountRemainGrid();

	// 콜백 함수 취소
	void UnregisterAllCallback();

	void RegisterQuoteCallback();
	void UnregisterQuoteCallback();
	void OnQuoteEvent(symbol_p symbol);


	void RegisterOrderallback();
	void UnregisterOrderCallback();
	void OnOrderEvent(VtOrder* order);

	virtual void OnSetup();
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	void SetRowTitle();
	int _ColCount = 2;
	int _RowCount = 6;
	CFont _defFont;
	CFont _titleFont;

	void QuickRedrawCell(int col, long row);

	void ShowAccountInfo(account_p acnt);

	void SetOrderConfigMgr(VtOrderConfigManager* val);
	void OnReceiveAccountDeposit(account_p acnt);

	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);

private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	void UpdateAccount(VtOrder* order);
	void UpdateAccount(symbol_p symbol);
	void UpdateAccountInfo();

DECLARE_MESSAGE_MAP()
};

