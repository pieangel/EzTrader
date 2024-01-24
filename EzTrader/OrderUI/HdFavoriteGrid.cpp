#include "stdafx.h"
#include "HdFavoriteGrid.h"
//#include "../Account/VtAccount.h"
//#include "VtFDSubAcntPage.h"
//#include "../Global/MainBeetle.h"
//#include "../Symbol/SmMarketManager.h"
//#include "../Symbol/VtSymbol.h"
//#include "../Format/format.h"
//#include "../Task/SmCallbackManager.h"
#include "VtOrderConfigManager.h"
#include "SmOrderPanelOut.h"
#include "../Global/SmTotalManager.h"
#include "../MessageDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HdFavoriteGrid::HdFavoriteGrid()
{
	_SubAcntPage = nullptr;
	_ParentAccount = nullptr;
}


HdFavoriteGrid::~HdFavoriteGrid()
{
}

BEGIN_MESSAGE_MAP(HdFavoriteGrid, VtGrid)
	ON_MESSAGE(WM_QUOTE_CHANGED, OnQuoteChangedMessage)
	//ON_MESSAGE(WM_HOGA_CHANGED, OnHogaChangedMessage)
	//ON_MESSAGE(WM_ORDER_CHANGED, OnOrderChangedMessage)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void HdFavoriteGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	m_noteBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_GLASS));
	m_nButtonIndex = AddCellType(&m_button);

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	CUGCell cell;
	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}



	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(0);
	SetHS_Height(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);

	InitGrid();
}

void HdFavoriteGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void HdFavoriteGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (updn == FALSE)
		return;

	if (_ClickedRow == row)
		return;

	if (_ClickedRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _ClickedRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _ClickedRow);
		}
	}
	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, row, _ClickedColor);
		QuickRedrawCell(i, row);
	}
	_ClickedRow = row;

	CUGCell cell;
	GetCell(0, _ClickedRow, &cell);
	/*
	VtSymbol* sym = (VtSymbol*)cell.Tag();
	if (sym) {
		if (sym->Quote.intClose == 0)
			sym->GetSymbolMaster();
		if (_OrderConfigMgr) {
			_OrderConfigMgr->Symbol(sym);

			if (_OrderConfigMgr->centerWnd) {
				_OrderConfigMgr->centerWnd->ChangeSymbol(sym);
			}
		}
	}
	*/
}

void HdFavoriteGrid::OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (updn == FALSE)
		return;

	if (_ClickedRow == row)
		return;

	if (_ClickedRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _ClickedRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _ClickedRow);
		}
	}
	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, row, _ClickedColor);
		QuickRedrawCell(i, row);
	}
	_ClickedRow = row;

	CUGCell cell;
	GetCell(0, _ClickedRow, &cell);
	/*
	VtAccount* subAcnt = (VtAccount*)cell.Tag();
	if (subAcnt && _ParentAccount)
	{
		if (subAcnt->Prime())
		{
			AfxMessageBox(_T("기본계좌는 지울 수 없습니다.!"));
			return;
		}
		_ParentAccount->RemoveSubAccount(subAcnt->AccountNo);
// 		if (_SubAcntPage)
// 		{
// 			_SubAcntPage->RefreshSubAccountList(_ParentAccount);
// 		}
	}
	*/

}

void HdFavoriteGrid::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed /*= 0*/)
{
	if (_OldSelRow == row)
		return;

	if (_OldSelRow != _ClickedRow && _OldSelRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _OldSelRow);
		}
	}

	if (row != _ClickedRow) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, row, _SelColor);
			QuickRedrawCell(i, row);
		}
	}
	else {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, row, _ClickedColor);
			QuickRedrawCell(i, row);
		}
	}

	_OldSelRow = row;
}

void HdFavoriteGrid::OnMouseLeaveFromMainGrid()
{
	if (_OldSelRow == _ClickedRow)
		return;

	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
		QuickRedrawCell(i, _OldSelRow);
	}

	_OldSelRow = -2;
}

void HdFavoriteGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[2] = { "종목", "현재가" };
	int colWidth[2] = { 100, 61 };


	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[i]);
		GetCell(i, -1, &cell);
		cell.SetText(title[i]);
		cell.SetBackColor(DarkHorse::SmTotalManager::GridTitleBackColor);
		cell.SetTextColor(DarkHorse::SmTotalManager::GridTitleTextColor);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetFont(&_titleFont);
		SetCell(i, -1, &cell);
		QuickRedrawCell(i, -1);
	}
}

void HdFavoriteGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);

}

void HdFavoriteGrid::InitGrid(VtAccount* acnt)
{
	if (!acnt)
		return;
	_ParentAccount = acnt;

	_RefreshCells.clear();

	ClearCells();

	int i = 0;
	CUGCell cell;
	/*
	std::vector<VtAccount*>& acntMap = acnt->GetSubAccountList();
	for (auto it = acntMap.begin(); it != acntMap.end(); ++it)
	{
		VtAccount* acnt = *it;
		GetCell(0, i, &cell);
		cell.Tag(acnt);
		SetCell(0, i, &cell);

		QuickSetText(0, i, acnt->AccountNo.c_str());
		QuickSetText(1, i, acnt->AccountName.c_str());
		if (acnt->Prime())
			QuickSetText(2, i, _T("기본"));
		else
			QuickSetText(2, i, _T("일반"));
		if (i == _ClickedRow && acntMap.size() > 0)
		{
			for (int j = 0; j < _ColCount; j++)
			{
				QuickSetBackColor(j, i, _ClickedColor);
			}
		}

		_RefreshCells.insert(std::make_pair(0, i));
		_RefreshCells.insert(std::make_pair(1, i));
		_RefreshCells.insert(std::make_pair(2, i));
		i++;
	}

	for (auto it = _RefreshCells.begin(); it != _RefreshCells.end(); ++it) {
		std::pair<int, int> cellInfo = *it;
		QuickRedrawCell(cellInfo.first, cellInfo.second);
	}
	*/
}

void HdFavoriteGrid::InitGrid()
{
	ClearCells();
	CUGCell cell;
	/*
	std::vector<VtSymbol*>& fav_list = mainApp.SymbolMgr().MrktMgr().GetFavoriteList();
	for (size_t i = 0; i < fav_list.size(); ++i) {
		VtSymbol* symbol = fav_list[i];
		if (!symbol)
			continue;

		QuickSetText(0, i, symbol->ShortCode.c_str());
		std::string temp = fmt::format("{:.{}f}", symbol->Quote.close, symbol->Decimal);
		QuickSetText(1, i, temp.c_str());
		_CodeIntMap[symbol->ShortCode] = i;

		GetCell(0, i, &cell);
		cell.Tag(symbol);
		SetCell(0, i, &cell);

		RedrawCell(0, i);
		RedrawCell(1, i);
	}

	mainApp.CallbackMgr().SubscribeQuoteWndCallback(GetSafeHwnd());
	*/
}

void HdFavoriteGrid::ClearCells()
{
	CUGCell cell;
	for (int i = 0; i < _RowCount; i++) {
		for (int j = 0; j < _ColCount; j++) {
			QuickSetText(j, i, _T(""));
			QuickSetBackColor(j, i, RGB(255, 255, 255));
			_RefreshCells.insert(std::make_pair(j, i));
			GetCell(j, i, &cell);
			cell.Tag(nullptr);
			SetCell(j, i, &cell);
		}
	}
}

VtAccount* HdFavoriteGrid::GetSelectedAccount()
{
	CUGCell cell;
	GetCell(0, _ClickedRow, &cell);
	return (VtAccount*)cell.Tag();
}

void HdFavoriteGrid::SetDefaultSelect()
{
	ChangeSelectedRow(_ClickedRow, 0);
	_ClickedRow = 0;
	CUGCell cell;
	GetCell(0, _ClickedRow, &cell);
// 	if (_SubAcntPage) {
// 		_SubAcntPage->SelSubAccount((VtAccount*)cell.Tag());
// 	}
}

LRESULT HdFavoriteGrid::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	/*
	VtSymbol* symbol = (VtSymbol*)lParam;
	auto it = _CodeIntMap.find(symbol->ShortCode);
	if (it != _CodeIntMap.end()) {
		int row = it->second;
		std::string temp = fmt::format("{:.{}f}", symbol->Quote.close, symbol->Decimal);
		QuickSetText(1, row, temp.c_str());
		QuickRedrawCell(1, row);
	}
	*/
	return 1;
}

void HdFavoriteGrid::OnClose()
{
	//mainApp.CallbackMgr().UnsubscribeQuoteWndCallback(GetSafeHwnd());
}

void HdFavoriteGrid::ChangeSelectedRow(int oldRow, int newRow)
{
	for (int j = 0; j < _ColCount; j++) {
		QuickSetBackColor(j, oldRow, RGB(255, 255, 255));
		QuickRedrawCell(j, oldRow);
	}

	for (int j = 0; j < _ColCount; j++) {
		QuickSetBackColor(j, newRow, _SelColor);
		QuickRedrawCell(j, newRow);
	}
}
