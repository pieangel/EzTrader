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

class VtOrderLeftWndHd;
class VtProductSection;
class VtOrderConfigManager;
class VtSymbol;
struct VtPosition;
struct VtOrder;
class VtProductOrderManager;

class SmFutureGrid : public CGridCtrl
{
public:
	SmFutureGrid();
	~SmFutureGrid();

	
	void RegisterMasterCallback();
	void RegisterQuoteCallback();
	void RegisterOrderCallback();

	void OnMasterEvent(symbol_p sym);
	void OnOrderEvent(VtOrder* order);
	void OnQuoteEvent(symbol_p sym);

	// 콜백 함수 취소
	void UnregisterAllCallback();
	
	void Init();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnOrderChangedMessage(WPARAM wParam, LPARAM lParam);
	
	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 50;
	int _CellHeight = 21;
	CFont _defFont;
public:
	void InitGrid();

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	std::map<std::string, std::tuple<int, int, symbol_p>> _FutureCurrentValueRowMap;
	int _Mode = 1;
	COLORREF _SellColor;
	COLORREF _BuyColor;
	COLORREF _DefaultColor;
	void ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode);
	void GetSymbolMaster(symbol_p sym);

	void QuickSetBackColor(int row, int col, COLORREF color);
	void QuickSetText(int row, int col, LPCTSTR text);
	void QuickSetNumber(int row, int col, int number);
	void QuickSetTextColor(int row, int col, COLORREF color);

public:
	void OnReceiveQuote(symbol_p sym);
	void OnSymbolMaster(symbol_p sym);
	void SetRemain(VtPosition* posi);
	void SetRemain(VtOrder* order);
	void SetRemain(std::string symbol_code);
	void ShowRemain(symbol_p sym);
	void ShowCurrent(symbol_p sym, int row);
	void OnOrderFilled(VtOrder* order);
	void OnExpected(symbol_p sym);
	void ShowExpected(symbol_p sym, int row);
};

