#pragma once
#include "../Grid/GridCtrl.h"
#include <vector>
#include <map>
#include <set>
#include <functional>
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

struct VtQuote;
class VtOrderConfigManager;
class VtSymbol;
class VtOrderCenterWndHd;
class SmOrderPanelOut;

class SmRealtickGrid : public CGridCtrl
{
public:
	SmRealtickGrid();
	~SmRealtickGrid();

	void UnregisterAllCallback();

	void RegisterQuoteCallback();
	void OnQuoteEvent(symbol_p symbol);

	void Init();


	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 20;
	CFont _defFont;
	CFont _titleFont;
	void OnReceiveQuote(symbol_p sym);
	void SetOrderConfigMgr(VtOrderConfigManager* val);
	void ClearText();
	int MaxRow() const { return _RowCount; }
	void MaxRow(int val); // { _MaxRow = val; }
	void ClearValues();
	SmOrderPanelOut* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(SmOrderPanelOut* val) { _CenterWnd = val; }
	int GetGridWidth();
	symbol_p Symbol() const { return _Symbol; }
	void Symbol(symbol_p val);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
private:
	symbol_p _Symbol = nullptr;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	int GetMaxRow();
	int _CellHeight;
	std::vector<int> _ColWidths;
	SmOrderPanelOut* _CenterWnd = nullptr;
};


