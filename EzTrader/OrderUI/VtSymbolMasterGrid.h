#pragma once
#include "../UGrid/VtGrid.h"
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
	void OnQuoteEvent(VtSymbol* symbol);

	virtual void OnSetup();
	void SetRowTitle();
	int _ColCount = 2;
	int _RowCount = 11;
	CFont _defFont;
	CFont _titleFont;

	void QuickRedrawCell(int col, long row);
	void InitSymbol(VtSymbol* sym);
private:
	void UpdateSymbol(VtSymbol* sym);
	VtSymbol* _Symbol = nullptr;
public:
	void ResetSymbol();

	void SetOrderConfigMgr(VtOrderConfigManager* val);

	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()

private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
};

