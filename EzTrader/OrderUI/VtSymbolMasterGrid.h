#pragma once
#include "../UGrid/VtGrid.h"
#include <memory>
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}

using symbol_p = std::shared_ptr<DarkHorse::SmSymbol>;
using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;

class VtSymbol;
struct VtRealtimeSymbolMaster;
class VtOrderConfigManager;
class VtSymbolMasterGrid : public VtGrid
{
public:
	VtSymbolMasterGrid();
	~VtSymbolMasterGrid();

	// 콜백 함수 취소
	void UnregisterAllCallback();

	void RegisterQuoteCallback();
	void UnregisterQuoteCallback();
	void OnQuoteEvent(symbol_p symbol);

	virtual void OnSetup();
	void SetRowTitle();
	int _ColCount = 2;
	int _RowCount = 11;
	CFont _defFont;
	CFont _titleFont;

	void QuickRedrawCell(int col, long row);
	void InitSymbol(symbol_p sym);
private:
	void UpdateSymbol(symbol_p sym);
	symbol_p _Symbol = nullptr;
public:
	void ResetSymbol();

	void SetOrderConfigMgr(VtOrderConfigManager* val);

	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()

private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
};

