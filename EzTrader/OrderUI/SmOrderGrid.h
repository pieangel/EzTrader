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
struct VtOrder;
class VtOrderCenterWndHd;
class VtSymbol;
class VtAccount;
class VtOrderConfigManager;
class VtStopOrderManager;
struct VtPosition;
class SmOrderPanel;
class SmOrderPanelOut;
class VtCutManager;
class SmOrderGrid : public CGridCtrl
{
public:
	SmOrderGrid();
	~SmOrderGrid();
	SmOrderPanel* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(SmOrderPanel* val) { _CenterWnd = val; }
	VtCutManager* CutMgr() const { return _CutMgr; }
	void CutMgr(VtCutManager* val) { _CutMgr = val; }
	bool ShowOrderQuantityCol() const { return _ShowOrderQuantityCol; }
	void ShowOrderQuantityCol(bool val) { _ShowOrderQuantityCol = val; }
	int OrderWidth() const { return _OrderWidth; }
	void OrderWidth(int val) { _OrderWidth = val; }
	int CellHeight() const { return _CellHeight; }
	void CellHeight(int val) { _CellHeight = val; }
	int OrderAmount() const { return _OrderAmount; }
	void OrderAmount(int val) { _OrderAmount = val; }
	SmOrderPanelOut* CenterWndOut() const { return _CenterWndOut; }
	void CenterWndOut(SmOrderPanelOut* val) { _CenterWndOut = val; }
public:
	int order_count = 1;
	int filled_count = 1;
	// �ɺ������� �̺�Ʈ ó��
	void OnSymbolMaster(symbol_p symbol);
	// �ڵ���ž �ֹ� ó��
	void SetAutoStopOnFilled(VtOrder* order);
	// �ݹ� �Լ� ���
	void UnregisterAllCallback();

	void RegisterQuoteCallback();
	void UnregisterQuoteCallback();
	void OnQuoteEvent(symbol_p symbol);
	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);

	void RegisterHogaCallback();
	void UnregisterHogaCallback();
	void OnHogaEvent(symbol_p symbol);
	afx_msg LRESULT OnHogaChangedMessage(WPARAM wParam, LPARAM lParam);

	void RegisterOrderallback();
	void UnregisterOrderCallback();
	void OnOrderEvent(VtOrder* order);

	void RegisterOrderWndCallback();
	void UnregisterOrderWndCallback();
	void OnOrderWndEvent(symbol_p symbol);
	afx_msg LRESULT OnOrderChangedMessage(WPARAM wParam, LPARAM lParam);

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	void SetCenterValue();
	void RefreshAllValues();
	void Init();
	// �����̽��� �ֹ�
	void OrderBySpaceBar();
	// �׸����� ������ �ٽ� �����Ѵ�.
	void ResizeGrid();
	// ���� ���̿� �ֹ������� �ʺ� �ٽ� �����ϰ� �׸����� ������ �ٽ� �����Ѵ�.
	void ResizeGrid(int cellHeight, int orderAreaWidth);
	void LiqudAll(VtPriceType priceType, int price = 0);
	// ������ û�� �Լ� : liqud -> true �� ���尡, false �� ������
	void PutOrder(VtPosition* posi, int price, bool liqud = false);
	void RefreshOrderPosition();
	void SetOrderArea(int height, int width);
	void AddOrderToCell(VtOrder* order);
	void RemoveOrderFromCell(VtOrder* order);
	void RefreshTotalOrderCount(VtOrder* order);
	
private:
	bool _Wheeling = false;
	void CheckProfitLossTouchHd(int intClose);
	bool CheckStopTouchedHd(int intClose);
	int _OrderWidth = 60;
	bool _ShowOrderQuantityCol = true;
	// �÷� Ÿ��Ʋ ����
	void SetColTitle(bool init);

	// �߰��� ����
	void SetCenterValue(const symbol_p, std::set<std::pair<int, int>>& refreshSet);
	void SetCenterValueByFixed(const symbol_p symbol, std::set<std::pair<int, int>>& refreshSet);

	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	std::vector<COLORREF> BuyColor;
	std::vector<COLORREF> SellColor;
	std::map<SmOrderGridCol, int> _GridColMap;
	int _ColCount = 9;
	int _RowCount = 60;
	int _HeadHeight = 20;
	int _FixedCol = 0;
	int _FixedRow = 1;

	CFont _defFont;
	CFont _titleFont;
	CFont _CenterFont;
	CFont _CursorFont;
	bool _Init = false;

	int _StartRowForValue = 2;
	int _EndRowForValue = 0;

	// �� ���� �������� �ü��� �׷�������.
	int _IndexRow = 0;
	int _OldIndexRow = -1;
	int _CellHeight = 0;

	SmOrderPanel* _CenterWnd = nullptr;
	SmOrderPanelOut* _CenterWndOut = nullptr;
	std::map<int, int> ValueToRowMap;
	std::map<int, int> RowToValueMap;
	std::set<std::pair<int, int>> _OrderPos;
	std::set<std::pair<int, int>> _HogaPos;
	std::set<std::pair<int, int>> _StopOrderPos;
	std::set<std::pair<int, int>> _QuotePos;

	void ClearQuotes(std::set<std::pair<int, int>>& refreshSet);
	// ���� �߰��� ����
	void SetCenterValueForFuture(const symbol_p symbol, std::set<std::pair<int, int>>& refreshSet);
	// �ɼ� �߰��� ����
	void SetCenterValueForOption(const symbol_p symbol, std::set<std::pair<int, int>>& refreshSet);
	// �ü� ������ ���� �߰��� ����
	void SetCenterValueByFixedForFuture(const symbol_p symbol, std::set<std::pair<int, int>>& refreshSet);
	// �ü� ������ �ɼ� �߰��� ����
	void SetCenterValueByFixedForOption(const symbol_p symbol, std::set<std::pair<int, int>>& refreshSet);
	// �߰��� ã�� �Լ�
	int FindIndexRow();
	// ���� �ִ� �హ�� ���ϱ�
	int GetMaxValueRowCount();
	// ȣ�� ���� ����
	void SetHogaInfo(const symbol_p symbol, std::set<std::pair<int, int>>& refreshSet);
	int FindRowFromCenterValue(int value);
	int FindRowFromCenterValue(const symbol_p sym, int value);
	// �ü� ���� ����
	void SetQuoteColor(const symbol_p symbol, std::set<std::pair<int, int>>& refreshSet);
	// �� ���� ����
	void SetSiseCellBackColor(int minRow, int maxRow, int start, int end, COLORREF color);
	// �ü� �ؽ�Ʈ ����� ���̺� ����
	void SetSiseLabel(int row, LPCTSTR text, COLORREF textColor);
	// ȣ�� ǥ�ø� ���ش�.
	void ClearHogas(std::set<std::pair<int, int>>& refreshSet);
	// �ֹ� ����
	void SetOrderInfo(std::set<std::pair<int, int>>& refreshSet);
	// �ֹ� ǥ�� ����
	void ClearOldOrders(std::set<std::pair<int, int>>& refreshSet);

	// ��ž �ֹ� ����
	void SetStopOrderInfo(std::set<std::pair<int, int>>& refreshSet);
	// ��ž �ֹ� ǥ�� ����
	void ClearOldStopOrders(std::set<std::pair<int, int>>& refreshSet);
	// ��ž �ֹ� ���� �׸���
	void CalcPosStopOrders(std::set<std::pair<int, int>>& refreshSet);
	// ���Ÿ�Ͽ� �ִ� ������ ��� �����Ѵ�.
	void RefreshCells(std::set<std::pair<int, int>>& refreshSet);
	// �ֹ��� ����.
	void PutOrder(int price, VtPositionType position, VtPriceType priceType = VtPriceType::Price);
	// �ֹ������� ���� ��ĥ�Ѵ�.
	void SetOrderAreaColor();

	// ������ ǥ��
	void SetPositionInfo(std::set<std::pair<int, int>>& refreshSet);
	// ������ ���ֱ�
	void ClearPositionInfo(std::set<std::pair<int, int>>& refreshSet);
	// �������� �����ش�. �ŵ�, �ż� �������� ���� ���
	void ShowPosition(std::set<std::pair<int, int>>& refreshSet, VtPosition* posi, symbol_p sym);
	// ���콺 Ŭ������ �ֹ��� ����.
	void OrderByMouseClick();
	// ���콺 ��ġ�� �ֹ��� ����.
	void OrderByMousePosition();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

private:
	//symbol_p _Symbol = nullptr;
	//std::string _SymbolCode;
	// VtAccount* _Account = nullptr;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
private:
	int _OrderAmount;
	bool _MouseIn = true;
	CRect _DragStartRect;
	CRect _DragEndRect;
	int _DragStartCol = 0;
	int _DragStartRow = 0;
	// �ֹ��� �׸���.
	void DrawArrow(int direction, POINT *point, CDC* pdc, POINT p0, POINT p1, int head_length, int head_width);
	BOOL m_bMouseTracking;
	void RedrawOrderTrackCells();
	// ��ž�ֹ��� �߰��Ѵ�.
	void AddStopOrder(int price, VtPositionType posi);
	CCellID _OldClickedCell;
	CCellID _OldMMCell;
	CCellID _OldMovingCellCenter;
	CCellID _OldMovingCellSide;
	CCellID _OldPositionCell;
	void InvalidateClickedCell();
	void SetMovingCell(CCellID cell);
	void HandleButtonEvent(int button_id);
	void ChangeOrder(VtOrder* order, int newPrice);
	void CancelOrder(VtOrder* order);
	void ChangeOrder();
	void ChangeStopOrder();
	void CancelOrder();
	void RegisterButtons();
	void UnregisterButtons();

	void BuyAtMarket();
	void SellAtMarket();
	void CancelAllSellStop();
	void CancelAllBuyStop();
	void CancelAllSellAccepted();
	void CancelAllBuyAccepted();
	void CancelAllAccepted();
	void RefreshStopOrder();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

private:
	VtStopOrderManager* _StopOrderMgr = nullptr;
	int FindPositionRow(VtPosition* posi);
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
public:
	int ShowHideOrderGrid(std::vector<bool>& colOptions);
	int GetGridWidth(std::vector<bool>& colOptions);
	int GetGridHeight();
	void ApplyProfitLossForPosition();
	void ResetByCenterRow();
private:
	VtCutManager* _CutMgr;
	int _HogaCount = 0;
	bool _HogaShow = false;
};
