#include "stdafx.h"
#include "VtProductRemainGrid.h"
//#include "../Position/VtPosition.h"
//#include "../Account/VtAccount.h"
//#include "../Symbol/VtSymbol.h"
//#include "../Symbol/VtSymbolManager.h"
//#include "../Symbol/VtSymbolMaster.h"
//#include "../Quote/VtQuote.h"
//#include "../Global/VtDefine.h"
#include "VtOrderConfigManager.h"
//#include "../Format/XFormatNumber.h"
//#include "../Format/format.h"
//#include "../Fund/VtFund.h"
#include <array>
#include <numeric>
#include <functional>
//#include "../Global/MainBeetle.h"
//#include "SmOrderPanel.h"
//#include "../Task/SmCallbackManager.h"
//#include "../Order/VtOrder.h"
#include "SmOrderPanelOut.h"
#include "../Global/SmTotalManager.h"
#include "../MessageDefine.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace std::placeholders;


BEGIN_MESSAGE_MAP(VtProductRemainGrid, VtGrid)
	ON_MESSAGE(WM_QUOTE_CHANGED, OnQuoteChangedMessage)
END_MESSAGE_MAP()

void VtProductRemainGrid::UnregisterAllCallback()
{
	//mainApp.CallbackMgr().UnsubscribeOrderCallback((long)this);
	//mainApp.CallbackMgr().UnsubscribeQuoteWndCallback(GetSafeHwnd());
	//mainApp.CallbackMgr().UnsubscribeOrderWndCallback(GetSafeHwnd());
}

void VtProductRemainGrid::RegisterQuoteCallback()
{
	//mainApp.CallbackMgr().SubscribeQuoteWndCallback(GetSafeHwnd());
	//mainApp.CallbackMgr().SubscribeOrderWndCallback(GetSafeHwnd());
}

void VtProductRemainGrid::OnQuoteEvent(const VtSymbol* symbol)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr) {
		return;
	}

	//if (_CenterWnd->Symbol()->ShortCode.compare(symbol->ShortCode) != 0)
	//	return;

	if (_OrderConfigMgr->Type() == 0)
		ShowSinglePosition();
	else
		ShowFundPosition();
}

void VtProductRemainGrid::RegisterOrderallback()
{
	//mainApp.CallbackMgr().SubscribeOrderCallback((long)this, std::bind(&VtProductRemainGrid::OnOrderEvent, this, _1));
}

void VtProductRemainGrid::OnOrderEvent(const VtOrder* order)
{
	if (!order || !_OrderConfigMgr || !_CenterWnd || !_CenterWnd->Symbol())
		return;
	/*
	if (order->Type == -1 || order->Type == 0) { // 일반 계좌
		if (!_OrderConfigMgr->Account())
			return;
		// 심볼과 계좌가 같지 않으면 진행하지 않는다.
		if (_CenterWnd->Symbol()->ShortCode.compare(order->shortCode) != 0 ||
			_OrderConfigMgr->Account()->AccountNo.compare(order->AccountNo) != 0)
			return;
	}
	else if (order->Type == 1) { // 서브 계좌
		if (!_OrderConfigMgr->Account())
			return;
		// 심볼과 계좌가 같지 않으면 진행하지 않는다.
		if (_CenterWnd->Symbol()->ShortCode.compare(order->shortCode) != 0 ||
			_OrderConfigMgr->Account()->AccountNo.compare(order->SubAccountNo) != 0)
			return;
	}
	else if (order->Type == 2) { // 펀드
		if (!_OrderConfigMgr->Fund())
			return;
		// 심볼과 계좌가 같지 않으면 진행하지 않는다.
		if (_CenterWnd->Symbol()->ShortCode.compare(order->shortCode) != 0 ||
			_OrderConfigMgr->Fund()->Name.compare(order->FundName) != 0)
			return;
	}
	else
		return;
	*/
	ShowPosition();
}

VtProductRemainGrid::VtProductRemainGrid()
{
	_CenterWnd = nullptr;
	_OrderConfigMgr = nullptr;
}


VtProductRemainGrid::~VtProductRemainGrid()
{
}

void VtProductRemainGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetUGMode(UGMode::Normal);
	SetVScrollMode(UG_SCROLLJOYSTICK);

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);
	//_RowCount = GetMaxRow();
	SetHS_Height(0);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	QuickSetAlignment(0, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	for (int i = 1; i < 6; i++) {
		QuickSetAlignment(i, 0, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(0);
	SetColTitle(4);
	RegisterOrderallback();
	RegisterQuoteCallback();
}

void VtProductRemainGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

int VtProductRemainGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

void VtProductRemainGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[9] = { "종목", "구분", "잔고", "평균가", "현재가", "평가손익" };
	int colWidth[9] = {60, 35, 40, 70, 90, 90 };


	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(DarkHorse::SmTotalManager::GridTitleBackColor);
		cell.SetTextColor(DarkHorse::SmTotalManager::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		QuickSetAlignment(i, -1, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		RedrawCell(i, -1);
	}
}


int VtProductRemainGrid::FindLevel(int ParentWidth)
{
	std::array<int, 4> WndWidth = { 174, 294, 390, 481 };
	int delta = 0, maxDelta = 1000;
	int index = 4;
	for (int i = 0; i < 4; i++)
	{
		delta = std::abs(WndWidth[i] - ParentWidth);
		if (delta < maxDelta)
		{
			maxDelta = delta;
			index = i + 1;
		}
	}

	return index;
}

int VtProductRemainGrid::FindLevel(std::vector<bool>& colOptions)
{
	return std::count_if(colOptions.begin(), colOptions.end(), [](auto i) { return i; });
}

void VtProductRemainGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtProductRemainGrid::ShowPosition(VtPosition* posi)
{

	if (!posi)
		return;
	
}

void VtProductRemainGrid::ShowPosition()
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0)
		ShowSinglePosition();
	else
		ShowFundPosition();
}

void VtProductRemainGrid::ShowPosition(VtPosition* posi, VtSymbol* sym)
{
	if (!posi || !sym)
		return;
	/*
	if (!posi) {
		QuickSetText(0, 0, _T(""));
		QuickSetText(1, 0, _T(""));
		QuickSetText(2, 0, _T(""));
		QuickSetText(3, 0, _T(""));
		QuickSetText(4, 0, _T(""));
		QuickSetText(5, 0, _T(""));

		QuickRedrawCell(0, 0);
		QuickRedrawCell(1, 0);
		QuickRedrawCell(2, 0);
		QuickRedrawCell(3, 0);
		QuickRedrawCell(4, 0);
		QuickRedrawCell(5, 0);
		_CenterWnd->SetRemain(0);
		return;
	}


	QuickSetText(0, 0, sym->ShortCode.c_str());
	QuickRedrawCell(0, 0);

	if (posi->OpenQty == 0) {
		QuickSetText(0, 0, _T(""));
		QuickSetText(1, 0, _T(""));
		QuickSetText(2, 0, _T(""));
		QuickSetText(3, 0, _T(""));
		QuickSetText(4, 0, _T(""));
		QuickSetText(5, 0, _T(""));

		QuickRedrawCell(0, 0);
		QuickRedrawCell(1, 0);
		QuickRedrawCell(2, 0);
		QuickRedrawCell(3, 0);
		QuickRedrawCell(4, 0);
		QuickRedrawCell(5, 0);
		_CenterWnd->SetRemain(posi->OpenQty);
		return;
	}
	if (posi->Position == VtPositionType::Buy) {
		QuickSetText(1, 0, _T("매수"));
		QuickSetTextColor(1, 0, RGB(255, 0, 0));
		QuickSetTextColor(2, 0, RGB(255, 0, 0));
		QuickSetTextColor(3, 0, RGB(255, 0, 0));
	} else if (posi->Position == VtPositionType::Sell) {
		QuickSetText(1, 0, _T("매도"));
		QuickSetTextColor(1, 0, RGB(0, 0, 255));
		QuickSetTextColor(2, 0, RGB(0, 0, 255));
		QuickSetTextColor(3, 0, RGB(0, 0, 255));
	}
	QuickRedrawCell(1, 0);
	QuickSetNumber(2, 0, std::abs(posi->OpenQty));
	QuickRedrawCell(2, 0);
	CString thVal;
	std::string temp = fmt::format("{:.{}f}", posi->AvgPrice, sym->Decimal);

	thVal = XFormatNumber(temp.c_str(), sym->Decimal);
	QuickSetText(3, 0, thVal);
	QuickRedrawCell(3, 0);


	CUGCell cell;
	GetCell(4, 0, &cell);
	int curValue = sym->Quote.intClose;
	cell.SetNumberDecimals(sym->Decimal);
	temp = fmt::format("{:.{}f}", curValue / std::pow(10, sym->Decimal), sym->Decimal);
	thVal = XFormatNumber(temp.c_str(), sym->Decimal);
	cell.SetText(thVal);
	cell.LongValue(curValue);
	SetCell(4, 0, &cell);
	QuickRedrawCell(4, 0);

	temp = fmt::format("{:.{}f}", posi->OpenProfitLoss, 2);
	CString profitLoss = XFormatNumber(temp.c_str(), -1);

	if (posi->OpenProfitLoss > 0) {
		QuickSetTextColor(5, 0, RGB(255, 0, 0));
		QuickSetText(5, 0, profitLoss);
	} else if (posi->OpenProfitLoss < 0) {
		QuickSetTextColor(5, 0, RGB(0, 0, 255));
		QuickSetText(5, 0, profitLoss);
	} else {
		QuickSetTextColor(5, 0, RGB(0, 0, 0));
		QuickSetText(5, 0, profitLoss);
	}
	QuickRedrawCell(5, 0);
	
	_CenterWnd->SetRemain(posi->OpenQty);
	_CenterWnd->RefreshOrderPositon();
	*/
	//LOG_F(INFO, _T("잔고그리드 갱신"));
}

void VtProductRemainGrid::SetSymbol(VtSymbol* sym)
{
	if (!sym || !_CenterWnd || !_OrderConfigMgr)
		return;

	/*
	VtAccount* acnt = _OrderConfigMgr->Account();
	if (!acnt)
		return;
	VtPosition* posi = acnt->FindPosition(sym->ShortCode);
	if (!posi)
		return;

	QuickSetText(0, 0, sym->ShortCode.c_str());
	QuickRedrawCell(0, 0);
	*/
}

void VtProductRemainGrid::ShowSinglePosition()
{
	if (!_OrderConfigMgr->Account())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();
	VtAccount* acnt = _OrderConfigMgr->Account();
	//VtPosition* posi = acnt->FindPosition(sym->ShortCode);
	//ShowPosition(posi, sym);
}

void VtProductRemainGrid::ShowFundPosition()
{
	if (!_OrderConfigMgr->Fund())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();
	int count = 0;
	/*
	VtPosition posi = _OrderConfigMgr->Fund()->GetPosition(sym->ShortCode, count);
	if (count == 0) {
		ShowPosition(nullptr, sym);
		return;
	}
	
	ShowPosition(&posi, sym);
	*/
}

void VtProductRemainGrid::ResetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[9] = { "종목", "구분", "잔고", "평균가", "현재가", "평가손익" };
	int colWidth[9] = { 60, 0, 35, 0, 60, 60 };


	for (int i = 0; i < _ColCount; i++) {
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(DarkHorse::SmTotalManager::GridTitleBackColor);
		cell.SetTextColor(DarkHorse::SmTotalManager::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		QuickSetAlignment(i, -1, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		RedrawCell(i, -1);
	}
}

int VtProductRemainGrid::SetColTitleForLevel(int parentWidth)
{
	int level = FindLevel(parentWidth);
	SetColTitle(level);

	return level;
}

int VtProductRemainGrid::SetColTitleForLevel(std::vector<bool>& colOptions)
{
	int level = FindLevel(colOptions) + 1;
	SetColTitle(level);
	return level;
}

void VtProductRemainGrid::OnReceiveRealtimeQuote(VtQuote* quote)
{
	if (!_OrderConfigMgr->Master() || !quote)
		return;

	//std::string strClose = fmt::format("{:.{}f}", quote->close, _OrderConfigMgr->Master()->decimal);

	//QuickSetText(4, 0, strClose.c_str());
	//QuickRedrawCell(4, 0);

	
}

void VtProductRemainGrid::InitPosition()
{
	ShowPosition();
}

void VtProductRemainGrid::ClearPosition()
{
	QuickSetText(0, 0, _T(""));
	QuickSetText(1, 0, _T(""));
	QuickSetText(2, 0, _T(""));
	QuickSetText(3, 0, _T(""));
	QuickSetText(4, 0, _T(""));
	QuickSetText(5, 0, _T(""));

	QuickRedrawCell(0, 0);
	QuickRedrawCell(1, 0);
	QuickRedrawCell(2, 0);
	QuickRedrawCell(3, 0);
	QuickRedrawCell(4, 0);
	QuickRedrawCell(5, 0);

	_CenterWnd->SetRemain(0);
}

void VtProductRemainGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;

	if (_OrderConfigMgr->Type() == 0)
		ShowSinglePosition();
	else
		ShowFundPosition();
}

void VtProductRemainGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void VtProductRemainGrid::OnRemain(VtPosition* posi)
{
	ShowPosition();
}

void VtProductRemainGrid::SetShowAvage(bool flag)
{
	if (flag)
		SetColTitle();
	else
	{
		SetColWidth(3, 0);
		ResetColTitle();
	}
}

void VtProductRemainGrid::SetShowRemainType(bool flag)
{
	if (flag)
		SetColTitle();
	else
	{
		SetColWidth(1, 0);
		ResetColTitle();
	}
}

void VtProductRemainGrid::OnReceiveAccountInfo()
{
	ShowPosition();
}

int VtProductRemainGrid::GetGridWidth(int parentWidth)
{
	int level = FindLevel(parentWidth);
	int gridWidth = 60 + 35 + 40 + 70 + 80 + 80;
	if (level == 1) {
		gridWidth = 40 + 60;
	} else if (level == 2) {
		gridWidth = 40 + 80 + 80;
	} else if (level == 3) {
		gridWidth = 35 + 40 + 70 + 80 + 80;
	}

	return gridWidth;
}

int VtProductRemainGrid::GetGridWidth(std::vector<bool>& colOptions)
{
	int gridWidth = 60 + 35 + 40 + 70 + 80 + 80;
	int colCnt = std::count_if(colOptions.begin(), colOptions.end(), [](auto i) { return i; });
	if (colCnt == 0) {
		gridWidth = 40 + 60;
	}
	else if (colCnt == 1) {
		gridWidth = 40 + 50 + 50 + 60;
	} 
	else if (colCnt == 2) {
		gridWidth = 35 + 40 + 60 + 80 + 80;
	}

	return gridWidth;
}

LRESULT VtProductRemainGrid::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	VtSymbol* symbol = (VtSymbol*)lParam;
		
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr) {
		return 1;
	}

	//if (_CenterWnd->Symbol()->ShortCode.compare(symbol->ShortCode) != 0)
	//	return 1;

	if (_OrderConfigMgr->Type() == 0)
		ShowSinglePosition();
	else
		ShowFundPosition();

	return 1;
}

LRESULT VtProductRemainGrid::OnOrderChangedMessage(WPARAM wParam, LPARAM lParam)
{
	ShowPosition();

	return 1;
}

void VtProductRemainGrid::SetColTitle(int level)
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[9] = { "종목", "구분", "잔고", "평균가", "현재가", "평가손익" };
	CRect rcWnd;
	GetWindowRect(rcWnd);
	std::array<int, 6> ColWidth;
	if (level == 1) {
		ColWidth = { 0, 0, 40, 0, 0, 60 };
		SetWindowPos(nullptr, 0, 0, std::accumulate(ColWidth.begin(), ColWidth.end(), 0), rcWnd.Height(), SWP_NOMOVE);
	} else if (level == 2) {
		ColWidth = { 0, 0, 40, 50, 50, 60 };
		SetWindowPos(nullptr, 0, 0, std::accumulate(ColWidth.begin(), ColWidth.end(), 0), rcWnd.Height(), SWP_NOMOVE);
	} else if (level == 3) {
		ColWidth = { 0, 35, 40, 60, 80, 80 };
		SetWindowPos(nullptr, 0, 0, std::accumulate(ColWidth.begin(), ColWidth.end(), 0), rcWnd.Height(), SWP_NOMOVE);
	} else {
		ColWidth = { 60, 35, 40, 70, 80, 80 };
		SetWindowPos(nullptr, 0, 0, std::accumulate(ColWidth.begin(), ColWidth.end(), 0), rcWnd.Height(), SWP_NOMOVE);
	}

	for (int i = 0; i < _ColCount; i++) {
		SetColWidth(i, ColWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(DarkHorse::SmTotalManager::GridTitleBackColor);
		cell.SetTextColor(DarkHorse::SmTotalManager::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_defFont);
		QuickSetAlignment(i, -1, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		RedrawCell(i, -1);
	}
}
