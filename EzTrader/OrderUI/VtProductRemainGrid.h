#pragma once
#include "../stdafx.h"
#include "../UGrid/VtGrid.h"
#include "../UGrid/CellTypes/UGCTSeperateText.h"
#include "../VtDefine.h"
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
class VtAccount;
struct VtQuote;
struct VtPosition;
class VtOrderConfigManager;
class VtOrderCenterWndHd;
class SmOrderPanel;
class SmOrderPanelOut;
struct VtOrder;
class VtProductRemainGrid : public VtGrid
{
public:
	void UnregisterAllCallback();

	void RegisterQuoteCallback();
	void OnQuoteEvent(const symbol_p symbol);

	void RegisterOrderallback();
	void OnOrderEvent(const VtOrder* order);


	VtProductRemainGrid();
	~VtProductRemainGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);

	void SetColTitle();
	int _ColCount = 6;
	int _RowCount = 1;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);

	//void Symbol(symbol_p val);
	//void Account(VtAccount* val) { _Account = val; }

	//void Position(VtPosition* val);
	void ShowPosition(VtPosition* posi);
	void ShowPosition();
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	SmOrderPanelOut* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(SmOrderPanelOut* val) { _CenterWnd = val; }
	void SetSymbol(symbol_p sym);
	//SmOrderPanelOut* CenterWndOut() const { return _CenterWndOut; }
	//void CenterWndOut(SmOrderPanelOut* val) { _CenterWndOut = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
private:
	void ShowSinglePosition();
	void ShowFundPosition();
	//symbol_p _Symbol = nullptr;
	//VtAccount* _Account = nullptr;
	//VtPosition* _Position = nullptr;
	SmOrderPanelOut* _CenterWnd;
	//SmOrderPanelOut* _CenterWndOut = nullptr;
	void ResetColTitle();
	void SetColTitle(int level);
	int FindLevel(int ParentWidth);
	int FindLevel(std::vector<bool>& colOptions);
	void ShowPosition(VtPosition* posi, symbol_p sym);
public:
	int SetColTitleForLevel(int parentWidth);
	int SetColTitleForLevel(std::vector<bool>& colOptions);
	void OnReceiveRealtimeQuote(VtQuote* quote);
	void InitPosition();
	void ClearPosition();
	void OnReceiveQuote(symbol_p sym);
	void SetOrderConfigMgr(VtOrderConfigManager* val);
	void OnRemain(VtPosition* posi);
	void SetShowAvage(bool flag);
	void SetShowRemainType(bool flag);
	void OnReceiveAccountInfo();
	int GetGridWidth(int parentWidth);
	int GetGridWidth(std::vector<bool>& colOptions);
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	// 모드 : 0 : 국내, 1 : 해외
	int _Mode = 0;
public:
	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOrderChangedMessage(WPARAM wParam, LPARAM lParam);
public:
	DECLARE_MESSAGE_MAP()
};

