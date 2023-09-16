#include "stdafx.h"
#include "VtTotalSignalGrid.h"
#include <array>
#include <numeric>
#include "VtSignalConnectionGrid.h"
#include "../OutSystem/SmOutSystem.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../OutSystem/SmOutSignalDef.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccount.h"
#include "../Fund/SmFund.h"
#include "../Dialog/HdSymbolSelecter.h"
#include "../Util/IdGenerator.h"
#include "../Event/EventHub.h"
#include "../Order/SmOrderConst.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"

COLORREF GridTitleBackColor = RGB(210, 224, 237);
COLORREF GridTitleTextColor = RGB(0, 0, 0);
COLORREF SelDialogBackColor = RGB(210, 224, 237);
COLORREF SelCellBackColor = RGB(252, 252, 190);
COLORREF MainBackColor = RGB(220, 220, 220);
COLORREF MainTextColor = RGB(0, 0, 0);

VtTotalSignalGrid::VtTotalSignalGrid()
{
}


VtTotalSignalGrid::~VtTotalSignalGrid()
{
}

void VtTotalSignalGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);
	_RowCount = 100;
	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	//Push Button cell type
	m_nButtonIndex = AddCellType(&m_button);
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
	SetVS_Width(GetSystemMetrics(SM_CXVSCROLL));
	SetHS_Height(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
	InitGrid();
}

void VtTotalSignalGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

int VtTotalSignalGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	if (ID == m_nButtonIndex) {
		return OnButton(ID, col, row, msg, param);
	}

	return TRUE;
}

void VtTotalSignalGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[10] = { "계좌번호", "종목", "포지션", "평균단가", "현재가", "평가손익", "매도주문", "매수주문", "신호차트", "청산" };
	int colWidth[10] = { 100, 100, 40, 100, 100, 100, 100, 100, 124 - GetSystemMetrics(SM_CXVSCROLL), 80 };


	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[i]);
		GetCell(i, -1, &cell);
		cell.SetText(title[i]);
		cell.SetBackColor(GridTitleBackColor);
		cell.SetTextColor(GridTitleTextColor);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetFont(&_titleFont);
		SetCell(i, -1, &cell);
		QuickRedrawCell(i, -1);
	}
}

void VtTotalSignalGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtTotalSignalGrid::Refresh()
{
	ClearCells();
	InitGrid();
}

void VtTotalSignalGrid::RefreshOrders()
{
	auto out_system_vector = mainApp.out_system_manager()->get_out_system_vector();
	int i = 0;
	for (auto it = out_system_vector.begin(); it != out_system_vector.end(); ++it) {
		auto sys = *it;
		if (sys->order_type() == DarkHorse::OrderType::MainAccount || sys->order_type() == DarkHorse::OrderType::SubAccount) {
			if (sys->account()) QuickSetText(0, i, sys->account()->No().c_str());
		}
		else {
			if (sys->fund()) QuickSetText(0, i, sys->fund()->Name().c_str());
		}

		if (sys->symbol()) QuickSetText(1, i, sys->symbol()->SymbolCode().c_str());

		// 포지션 표시
		VtPosition posi = sys->GetPosition();
		posi.Position == VtPositionType::None ? QuickSetText(2, i, _T("없음")) : posi.Position == VtPositionType::Buy ? QuickSetText(2, i, _T("매수")) : QuickSetText(2, i, _T("매도"));
		CString thVal;
		std::string temp;
		if (sys->Symbol()) {
			temp = std::format(posi.AvgPrice, sys->symbol()->decimal());
			thVal = XFormatNumber(temp.c_str(), sys->Symbol()->Decimal);
		}
		else {
			temp = NumberFormatter::format(0, 0);
			thVal = XFormatNumber(temp.c_str(), -1);
		}
		QuickSetText(3, i, thVal);


		// 현재가 표시
		CUGCell cell;
		GetCell(4, i, &cell);
		int curValue = sys->Symbol() ? sys->Symbol()->Quote.intClose : 0;
		if (sys->Symbol()) {
			temp = NumberFormatter::format(curValue / std::pow(10, sys->Symbol()->Decimal), sys->Symbol()->Decimal);
			thVal = XFormatNumber(temp.c_str(), sys->Symbol()->Decimal);
		}
		else {
			temp = NumberFormatter::format(0, 0);
			thVal = XFormatNumber(temp.c_str(), -1);
		}
		cell.SetText(thVal);
		cell.LongValue(curValue);
		SetCell(4, i, &cell);

		temp = NumberFormatter::format(posi.OpenProfitLoss, 0);
		CString profitLoss = XFormatNumber(temp.c_str(), -1);
		// 평가손익 표시
		if (posi.OpenProfitLoss > 0) {
			QuickSetTextColor(5, i, RGB(255, 0, 0));
			QuickSetText(5, i, profitLoss);
		}
		else if (posi.OpenProfitLoss < 0) {
			QuickSetTextColor(5, i, RGB(0, 0, 255));
			QuickSetText(5, i, profitLoss);
		}
		else {
			QuickSetTextColor(5, i, RGB(0, 0, 0));
			QuickSetNumber(5, i, 0);
		}

		// 잔고 표시
		if (posi.OpenQty != 0) {
			if (posi.OpenQty > 0) {
				QuickSetNumber(7, i, std::abs(posi.OpenQty));
				QuickSetNumber(6, i, 0);
			}
			else if (posi.OpenQty < 0) {
				QuickSetNumber(6, i, std::abs(posi.OpenQty));
				QuickSetNumber(7, i, 0);
			}
		}
		else {
			QuickSetNumber(6, i, 0);
			QuickSetNumber(7, i, 0);
		}
		// 시그널 이름 표시
		QuickSetText(8, i, sys->OutSignal()->SignalName.c_str());

		GetCell(9, i, &cell);
		cell.SetCellType(m_nButtonIndex);
		cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetFont(&_titleFont);
		cell.SetTextColor(RGB(0, 0, 0));
		cell.SetBackColor(RGB(218, 226, 245));
		cell.Tag((void*)sys.get());
		cell.SetText("청산");
		SetCell(9, i, &cell);

		for (int j = 0; j < _ColCount; ++j) {
			QuickRedrawCell(j, i);
		}
		// 인덱스를 증가 시킨다.
		i++;
	}

	_RowNumber = sysMap.size();
}

int VtTotalSignalGrid::OnButton(long ID, int col, long row, long msg, long param)
{
	if (msg != 1)
		return -1;

	CString log;
	log.Format("OnButton col = %d, row = %d, msg = %d, param = %d \n", col, row, msg, param);
	TRACE(log);

	CUGCell cell;
	GetCell(col, row, &cell);
	VtSystem* sys = (VtSystem*)cell.Tag();
	if (sys) {
		VtOutSystemOrderManager* outSysOrderMgr = VtOutSystemOrderManager::GetInstance();
		// 시스템을 청산시킨다.
		sys->LiqudAll();
		// 시스템을 없애 버린다.
		outSysOrderMgr->RemoveSystem(sys->Id());
		// 연결창에서 체크를 풀고 시스템을 비활성화 시킨다.
		if (_ConnectGrid) {
			_ConnectGrid->ClearCheck(sys);
		}
		// 모든 셀을 지운다.
		ClearCells();
		// 그리드를 다시 그린다.
		RefreshOrders();
	}
	return 1;
}

void VtTotalSignalGrid::InitGrid()
{
	RefreshOrders();
}

void VtTotalSignalGrid::ClearCells()
{
	CUGCell cell;
	for (int i = 0; i < _RowNumber + 1; i++)
	{
		for (int j = 0; j < _ColCount; j++)
		{
			QuickSetText(j, i, _T(""));
			QuickSetBackColor(j, i, RGB(255, 255, 255));
			QuickRedrawCell(j, i);
			GetCell(j, i, &cell);
			cell.Tag(nullptr);
			cell.SetCellType(UGCT_NORMAL);
			cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
			SetCell(j, i, &cell);
		}
	}
}
