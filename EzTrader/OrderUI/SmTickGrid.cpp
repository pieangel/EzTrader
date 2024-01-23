#include "pch.h"
#include "SmTickGrid.h"
#include "../Quote/VtQuote.h"
#include "VtOrderConfigManager.h"
#include "../Symbol/VtSymbol.h"
#include "VtOrderConfigManager.h"
#include "VtOrderWndHd.h"
//#include "../Global/MainBeetle.h"
#include <numeric>
#include <functional>
#include "SmOrderPanel.h"
#include "../Task/SmCallbackManager.h"
#include "../Format/format.h"
#include "../Main/MainBeetle.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace std::placeholders;

#pragma warning(disable:4018)

SmTickGrid::SmTickGrid()
{
}


SmTickGrid::~SmTickGrid()
{
}


void SmTickGrid::UnregisterAllCallback()
{
	mainApp.CallbackMgr().UnsubscribeQuoteWndCallback(GetSafeHwnd());
}

void SmTickGrid::RegisterQuoteCallback()
{
	mainApp.CallbackMgr().SubscribeQuoteWndCallback(GetSafeHwnd());
}

void SmTickGrid::OnQuoteEvent(VtSymbol* sym)
{
	if (!sym || !_CenterWnd)
		return;

	if (_CenterWnd->Symbol()->ShortCode.compare(sym->ShortCode) != 0)
		return;

	int i = 1;
	for (auto it = sym->Quote.QuoteItemQ.begin(); it != sym->Quote.QuoteItemQ.end(); ++it) {
		VtQuoteItem item = *it;
		CGridCellBase* pCell = nullptr;
		pCell = GetCell(i, 0);
		if (!pCell)
			break;
		pCell->SetText(item.Time.c_str());
		InvalidateCellRect(i, 0);

		std::string close = fmt::format("{:.{}f}", item.ClosePrice / std::pow(10, sym->Decimal), sym->Decimal);

		pCell = GetCell(i, 1);
		pCell->SetText(close.c_str());
		InvalidateCellRect(i, 1);
		CString qty;
		qty.Format("%d", item.ContQty);
		pCell = GetCell(i, 2);
		pCell->SetText(qty);
		if (item.MatchKind == 1)
			pCell->SetTextClr(RGB(255, 0, 0));
		else
			pCell->SetTextClr(RGB(0, 0, 255));
		InvalidateCellRect(i, 2);
		i++;
	}
}

BEGIN_MESSAGE_MAP(SmTickGrid, CGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_QUOTE_CHANGED, OnQuoteChangedMessage)
END_MESSAGE_MAP()

void SmTickGrid::Init()
{
	_CellHeight = 18;
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetRowCount(_RowCount);
	SetColumnCount(_ColCount);
	SetFixedRowCount(0);
	SetFixedColumnCount(0);
	SetColTitle();
	EnableScrollBars(SB_VERT, FALSE);
	EnableScrollBars(SB_HORZ, FALSE);
	SetFixedColumnSelection(FALSE);
	SetFixedRowSelection(FALSE);
	SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	SetDoubleBuffering(TRUE);
	EnableSelection(FALSE);
	SetEditable(FALSE);
	EnableTitleTips(FALSE);
	SetColumnResize(FALSE);
	SetRowResize(FALSE);
	AllowReorderColumn(false);

	for (int i = 1; i < _RowCount; i++) {
		SetRowHeight(i, _CellHeight);
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				// 텍스트 정렬
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				// 셀 배경 색 설정
				pCell->SetBackClr(RGB(255, 255, 255));
				// 셀 글자색 설정
				pCell->SetTextClr(RGB(0, 0, 0));

				InvalidateCellRect(i, j);
			}
		}
	}

	SetFont(&_defFont);
	RegisterQuoteCallback();
}

void SmTickGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	LPCTSTR title[3] = { "시각", "체결가", "체결" };
	_ColWidths.push_back(60);
	_ColWidths.push_back(55);
	_ColWidths.push_back(35);
	SetRowHeight(0, 22);

	for (int i = 0; i < _ColCount; i++) {
		SetColumnWidth(i, _ColWidths[i]);
		CGridCellBase* pCell = GetCell(0, i);
		pCell->SetText(title[i]);
		pCell->SetBackClr(MainBeetle::GridTitleBackColor);
		pCell->SetTextClr(MainBeetle::GridTitleTextColor);
		InvalidateCellRect(0, i);
	}
}

void SmTickGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;

	int i = 1;
	for (auto it = sym->Quote.QuoteItemQ.begin(); it != sym->Quote.QuoteItemQ.end(); ++it) {
		VtQuoteItem item = *it;
		CGridCellBase* pCell = nullptr;
		pCell = GetCell(i, 0);
		pCell->SetText(item.Time.c_str());
		InvalidateCellRect(i, 0);

		std::string close = fmt::format("{:.{}f}", item.ClosePrice / std::pow(10, sym->Decimal), sym->Decimal);

		pCell = GetCell(i, 1);
		pCell->SetText(close.c_str());
		InvalidateCellRect(i, 1);
		CString qty;
		qty.Format("%d", item.ContQty);
		pCell = GetCell(i, 2);
		pCell->SetText(qty);
		if (item.MatchKind == 1)
			pCell->SetTextClr(RGB(255, 0, 0));
		else
			pCell->SetTextClr(RGB(0, 0, 255));
		InvalidateCellRect(i, 2);
		i++;
		if (m_nRows == i)
			break;
	}

	if (sym->Quote.QuoteItemQ.size() > m_nRows)
		sym->Quote.QuoteItemQ.pop_back();
}

void SmTickGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void SmTickGrid::ClearText()
{
	for (int i = 1; i < _RowCount; i++) {
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			pCell->SetText("");
			InvalidateCellRect(i, j);
		}
	}
}

void SmTickGrid::MaxRow(int val)
{
	_RowCount = val;
	SetRowCount(val);
	for (int i = 1; i < _RowCount; i++) {
		SetRowHeight(i, _CellHeight);
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				// 텍스트 정렬
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				// 셀 배경 색 설정
				pCell->SetBackClr(RGB(255, 255, 255));
				// 셀 글자색 설정
				pCell->SetTextClr(RGB(0, 0, 0));

				InvalidateCellRect(i, j);
			}
		}
	}
}

void SmTickGrid::ClearValues()
{
	for (int i = 1; i < _RowCount; i++) {
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			pCell->SetText("");
			InvalidateCellRect(i, j);
		}
	}
}

int SmTickGrid::GetGridWidth()
{
	return std::accumulate(_ColWidths.begin(), _ColWidths.end(), 0) + 6;
}

void SmTickGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd && _CenterWnd) {
		_OrderConfigMgr->_HdOrderWnd->SetActiveCenterWnd(_CenterWnd);
	}
}

void SmTickGrid::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd && _CenterWnd) {
		_OrderConfigMgr->_HdOrderWnd->SetActiveCenterWnd(_CenterWnd);
	}
}

LRESULT SmTickGrid::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	if (!_CenterWnd)
		return 1;

	VtSymbol* sym = _CenterWnd->Symbol();
	if (!sym)
		return 1;

	VtSymbol* symbol = (VtSymbol*)lParam;
	if (_CenterWnd->Symbol()->ShortCode.compare(symbol->ShortCode) != 0) {
		return 1;
	}

	int i = 1;
	for (auto it = sym->Quote.QuoteItemQ.begin(); it != sym->Quote.QuoteItemQ.end(); ++it) {
		VtQuoteItem item = *it;
		CGridCellBase* pCell = nullptr;
		pCell = GetCell(i, 0);
		if (!pCell)
			break;
		pCell->SetText(item.Time.c_str());
		InvalidateCellRect(i, 0);

		std::string close = fmt::format("{:.{}f}", item.ClosePrice / std::pow(10, sym->Decimal), sym->Decimal);

		pCell = GetCell(i, 1);
		pCell->SetText(close.c_str());
		InvalidateCellRect(i, 1);
		CString qty;
		qty.Format("%d", item.ContQty);
		pCell = GetCell(i, 2);
		pCell->SetText(qty);
		if (item.MatchKind == 1)
			pCell->SetTextClr(RGB(255, 0, 0));
		else
			pCell->SetTextClr(RGB(0, 0, 255));
		InvalidateCellRect(i, 2);
		i++;
	}
	return 1;
}

void SmTickGrid::RefreshValues()
{
	if (!_CenterWnd)
		return;

	VtSymbol* sym = _CenterWnd->Symbol();
	if (!sym)
		return;

	int i = 1;
	for (auto it = sym->Quote.QuoteItemQ.begin(); it != sym->Quote.QuoteItemQ.end(); ++it) {
		VtQuoteItem item = *it;
		CGridCellBase* pCell = nullptr;
		pCell = GetCell(i, 0);
		if (!pCell)
			break;
		pCell->SetText(item.Time.c_str());
		InvalidateCellRect(i, 0);

		std::string close = fmt::format("{:.{}f}", item.ClosePrice / std::pow(10, sym->Decimal), sym->Decimal);

		pCell = GetCell(i, 1);
		pCell->SetText(close.c_str());
		InvalidateCellRect(i, 1);
		CString qty;
		qty.Format("%d", item.ContQty);
		pCell = GetCell(i, 2);
		pCell->SetText(qty);
		if (item.MatchKind == 1)
			pCell->SetTextClr(RGB(255, 0, 0));
		else
			pCell->SetTextClr(RGB(0, 0, 255));
		InvalidateCellRect(i, 2);
		i++;
	}
	return;
}

int SmTickGrid::GetMaxRow()
{
	int rowHeight = _CellHeight;

	RECT rect;
	GetWindowRect(&rect);

	int count = (rect.bottom - rect.top) / rowHeight;

	return count - 1;
}
