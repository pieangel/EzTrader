#include "stdafx.h"
#include "VtSymbolMasterGrid.h"
//#include "../Symbol/VtSymbolMaster.h"
//#include "../Symbol/VtSymbolManager.h"
//#include "../Symbol/VtSymbol.h"
#include "../Util/VtTime.h"
//#include "../Global/MainBeetle.h"
#include <functional>
//#include "../Task/SmCallbackManager.h"
//#include "../Symbol/VtSymbol.h"
//#include "../Order/VtFundOrderManager.h"
//#include "../Log/loguru.hpp"
#include "SmOrderPanelOut.h"
#include "VtOrderWnd.h"
//#include "../Format/XFormatNumber.h"
//#include "../Format/format.h"
#include "../Global/SmTotalManager.h"
#include "../MessageDefine.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace std::placeholders;


BEGIN_MESSAGE_MAP(VtSymbolMasterGrid, VtGrid)
	ON_MESSAGE(WM_QUOTE_CHANGED, OnQuoteChangedMessage)
	//ON_MESSAGE(WM_HOGA_CHANGED, OnHogaChangedMessage)
	//ON_MESSAGE(WM_ORDER_CHANGED, OnOrderChangedMessage)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

VtSymbolMasterGrid::VtSymbolMasterGrid()
{
	_Symbol = nullptr;
}


VtSymbolMasterGrid::~VtSymbolMasterGrid()
{
	_Symbol = nullptr;
}


void VtSymbolMasterGrid::UnregisterAllCallback()
{
	UnregisterQuoteCallback();
}

void VtSymbolMasterGrid::RegisterQuoteCallback()
{
	//mainApp.CallbackMgr().SubscribeQuoteWndCallback(GetSafeHwnd());
}

void VtSymbolMasterGrid::UnregisterQuoteCallback()
{
	//mainApp.CallbackMgr().UnsubscribeQuoteCallback((long)this);
}

void VtSymbolMasterGrid::OnQuoteEvent(VtSymbol* symbol)
{
	if (!_Symbol || !symbol)
		return;
	//if (_Symbol->ShortCode.compare(symbol->ShortCode) != 0)
	//	return;
	UpdateSymbol(symbol);
}

void VtSymbolMasterGrid::OnSetup()
{
	SetUGMode(UGMode::OrderNormal);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	SetDoubleBufferMode(TRUE);
	SetDefColWidth(60);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);



	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetTH_Height(0);
	SetRowTitle();
	SetVS_Width(0);

	JoinCells(0, 0, 1, 0);
	SetRowHeight(0, 40);
	EnableMenu(TRUE);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}

	RedrawAll();

	RegisterQuoteCallback();
}


void VtSymbolMasterGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	CUGCell cell;
	LPCTSTR title[11] = { "상품명", "시가", "고가", "저가", "종가", "전일대비", "총거래량", "틱가치", "틱크기", "거래소", "만기일" };

	SetColWidth(0, 60);
	SetColWidth(1, 103);

	for (int i = 0; i < _RowCount; i++)
	{
		QuickSetText(0, i, title[i]);
		GetCell(0, i, &cell);
		cell.SetBackColor(DarkHorse::SmTotalManager::GridTitleBackColor);
		cell.SetTextColor(DarkHorse::SmTotalManager::GridTitleTextColor);
		SetCell(0, i, &cell);
		QuickSetFont(0, i, &_titleFont);
		QuickSetAlignment(0, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		RedrawCell(0, i);
	}

	QuickSetCellTypeEx(0, 0, UGCT_NORMALMULTILINE);
}

void VtSymbolMasterGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}


void VtSymbolMasterGrid::InitSymbol(VtSymbol* sym)
{
	if (!sym)
		return;

	_Symbol = sym;
	/*
	try
	{
		QuickSetText(0, 0, sym->Name.c_str());
		CString thVal;

		std::string temp = fmt::format("{:.{}f}", sym->Quote.intOpen / std::pow(10, sym->Decimal), sym->Decimal);

		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 1, thVal);
		temp = fmt::format("{:.{}f}", sym->Quote.intHigh / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 2, thVal);
		temp = fmt::format("{:.{}f}", sym->Quote.intLow / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 3, thVal);
		temp = fmt::format("{:.{}f}", sym->Quote.intClose / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 4, thVal);
		temp = fmt::format("{:.{}f}", sym->ComToPrev / std::pow(10, sym->Decimal), sym->Decimal);
		QuickSetText(1, 5, temp.c_str());

		// 누적 거래량
		
		QuickSetNumber(1, 6, sym->AccAmount);
		// 틱가치
		temp = fmt::format("{:.{}f}", sym->TickValue, sym->Decimal);
		QuickSetText(1, 7, temp.c_str());
		// 틱크기
		temp = fmt::format("{:.{}f}", sym->intTickSize / std::pow(10, sym->Decimal), sym->Decimal);
		QuickSetText(1, 8, temp.c_str());
		// 거래소
		QuickSetText(1, 9, sym->Exchange.c_str());
		// 만기일
		std::string ex_date;
		if (sym->ExpDate.length() >= 8) {
			ex_date.append(sym->ExpDate.substr(0, 4));
			ex_date.append("년");
			ex_date.append(sym->ExpDate.substr(4, 2));
			ex_date.append("월");
			ex_date.append(sym->ExpDate.substr(6, 2));
			ex_date.append("일");
			QuickSetText(1, 10, ex_date.c_str());
		}
		for (int i = 0; i < 11; i++)
			QuickRedrawCell(1, i);
	}
	catch (const std::exception& e)
	{
		std::string msg = e.what();
		AfxMessageBox(msg.c_str());
	}
	*/
}

void VtSymbolMasterGrid::UpdateSymbol(VtSymbol* sym)
{
	if (!sym || !_Symbol)
		return;
	/*
	if (_Symbol->ShortCode.compare(sym->ShortCode) != 0)
		return;

	try
	{
		QuickSetText(0, 0, sym->Name.c_str());
		CString thVal;
		std::string temp = fmt::format("{:.{}f}", sym->Quote.intOpen / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 1, thVal);
		temp = fmt::format("{:.{}f}", sym->Quote.intHigh / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 2, thVal);
		temp = fmt::format("{:.{}f}", sym->Quote.intLow / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 3, thVal);
		temp = fmt::format("{:.{}f}", sym->Quote.intClose / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 4, thVal);
		temp = fmt::format("{:.{}f}", sym->ComToPrev / std::pow(10, sym->Decimal), sym->Decimal);
		QuickSetText(1, 5, temp.c_str());

		// 누적 거래량

		QuickSetNumber(1, 6, sym->AccAmount);
		
		for (int i = 1; i < 7; i++)
			QuickRedrawCell(1, i);
	}
	catch (const std::exception& e)
	{
		std::string msg = e.what();
		AfxMessageBox(msg.c_str());
	}
	*/
}

void VtSymbolMasterGrid::ResetSymbol()
{
	_Symbol = nullptr;
}

void VtSymbolMasterGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

LRESULT VtSymbolMasterGrid::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	VtSymbol* symbol = (VtSymbol*)lParam;
	UpdateSymbol(symbol);
	return 1;
}

void VtSymbolMasterGrid::OnClose()
{
	//mainApp.CallbackMgr().UnsubscribeQuoteWndCallback(GetSafeHwnd());
}

