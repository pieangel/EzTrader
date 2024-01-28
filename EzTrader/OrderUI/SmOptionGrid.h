#pragma once
#include "../Grid/GridCtrl.h"
#include <vector>
#include <map>
#include <set>
#include <functional>
//#include "../Global/VtDefine.h"
#include <memory>
#include <map>
#include <tuple>
//#include "../Global/VtDefine.h"
#include <set>
#include <string>
//#include "../Symbol/SmRunInfo.h"

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}

using symbol_p = std::shared_ptr<DarkHorse::SmSymbol>;
using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;

class VtOrderLeftWndHd;
class VtProductSection;
class VtOrderConfigManager;
class VtSymbol;
struct VtPosition;
struct VtOrder;
class VtProductOrderManager;

class SmOptionGrid : public CGridCtrl
{
public:
	SmOptionGrid();
	~SmOptionGrid();
	void RegisterMasterCallback();
	void RegisterQuoteCallback();
	void RegisterOrderCallback();

	void OnMasterEvent(symbol_p sym);
	void OnOrderEvent(VtOrder* order);
	void OnQuoteEvent(symbol_p sym);

	// �ݹ� �Լ� ���
	void UnregisterAllCallback();
	
	// �׸��� �ʱ�ȭ
	void Init();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOrderChangedMessage(WPARAM wParam, LPARAM lParam);

	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 100;
	int _CellHeight = 18;
	CFont _defFont;
	CFont _titleFont;
	CPen m_Pen;
public:
	symbol_p _EqualSymbol = nullptr;
	void InitGrid();
	void InitSymbol();
	void InitYearMonth();
	void SetYearMonth();
	void SetProductSection();
	void GetSymbolMaster();
	void RefreshMode();
	VtOrderLeftWndHd* LeftWnd() const { return _LeftWnd; }
	void LeftWnd(VtOrderLeftWndHd* val) { _LeftWnd = val; }
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	
	void SetRemain(VtPosition* posi);
	void SetRemain(VtOrder* order);

	void InitGrid(int height);
	void SetRemain2();
	void SetCurrent2();
	void SetExpected2();
private:
	int _SelectedProduct = 0;
	//std::vector<SmRunInfo> _RunInfo;
	CCellID _EqualCell;
	VtOrderLeftWndHd* _LeftWnd = nullptr;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	VtProductSection* _CurPrdtSec = nullptr;
	void SetEqualRow(int equalRow);
	int GetMaxRow();

	std::map<std::string, std::tuple<int, int, symbol_p>> _SymbolRowMap;
	void MakeSymbolRowMap(int start_index, int max_row);
public:
	void OnSymbolMaster(symbol_p sym);
	void OnOrderFilled(VtOrder* order);
	void OnReceiveQuote(symbol_p sym);
	void OnExpected(symbol_p sym);
	std::pair<int, int> FindValueStartRow(int height);
private:
	void ClearAllText();
	int _ValueStartRow = 0;
	int _ValueMaxRow = 0;
	int _EqualIndex = 0;
	int _MaxIndex = 0;
	int _MaxRow = 0;
	int _Mode = 1;
	std::set<std::pair<int, int>> _RemainPos;
	void ResetRemainCells();
	void ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode);
	void ShowCurrent(symbol_p sym);
	void ShowExpected(symbol_p sym);
	void ShowExpected(int row, int col, symbol_p sym);
	void ShowCurrent(int row, int col, symbol_p sym);
	void SetRemain(symbol_p sym);
	void QuickSetBackColor(int row, int col, COLORREF color);
	void QuickSetText(int row, int col, LPCTSTR text);
	void QuickSetNumber(int row, int col, int number);
	void QuickSetTextColor(int row, int col, COLORREF color);
};
