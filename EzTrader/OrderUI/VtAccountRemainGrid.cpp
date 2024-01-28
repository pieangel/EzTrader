#include "stdafx.h"
#include "VtAccountRemainGrid.h"
#include "VtOrderConfigManager.h"
//#include "../Account/VtAccount.h"
#include "../UGrid/UGStrOp.h"
//#include "../Global/MainBeetle.h"
#include <functional>
//#include "../Task/SmCallbackManager.h"
#include "VtOrderConfigManager.h"
#include "../Global/SmTotalManager.h"
#include "../MessageDefine.h"
using namespace std;
using namespace std::placeholders;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(VtAccountRemainGrid, VtGrid)
	ON_MESSAGE(WM_QUOTE_CHANGED, OnQuoteChangedMessage)
END_MESSAGE_MAP()

VtAccountRemainGrid::VtAccountRemainGrid()
{
	
}


VtAccountRemainGrid::~VtAccountRemainGrid()
{
	UnregisterAllCallback();
}

void VtAccountRemainGrid::UnregisterAllCallback()
{
	UnregisterOrderCallback();
	UnregisterQuoteCallback();
}

void VtAccountRemainGrid::RegisterQuoteCallback()
{
	//SmCallbackManager::GetInstance()->SubscribeQuoteCallback((long)this, std::bind(&VtAccountRemainGrid::OnQuoteEvent, this, _1));
	//mainApp.CallbackMgr().SubscribeQuoteWndCallback(GetSafeHwnd());
}

void VtAccountRemainGrid::UnregisterQuoteCallback()
{
	//SmCallbackManager::GetInstance()->UnsubscribeQuoteCallback((long)this);
	//mainApp.CallbackMgr().UnsubscribeQuoteWndCallback(GetSafeHwnd());
}

void VtAccountRemainGrid::OnQuoteEvent(symbol_p symbol)
{
	if (!symbol)
		return;
	UpdateAccount(symbol);
}

LRESULT VtAccountRemainGrid::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	UpdateAccountInfo();
	return 1;
}

void VtAccountRemainGrid::RegisterOrderallback()
{
	//mainApp.CallbackMgr().SubscribeOrderCallback((long)this, std::bind(&VtAccountRemainGrid::OnOrderEvent, this, _1));
}

void VtAccountRemainGrid::UnregisterOrderCallback()
{
	//mainApp.CallbackMgr().UnsubscribeOrderCallback((long)this);
}

void VtAccountRemainGrid::OnOrderEvent(VtOrder* order)
{
	if (!order)
		return;
	UpdateAccount(order);
}

void VtAccountRemainGrid::OnSetup()
{
	SetUGMode(UGMode::OrderNormal);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);



	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetTH_Height(0);
	SetRowTitle();
	SetVS_Width(0);

	EnableMenu(TRUE);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			if (xIndex != 0)
			{
				QuickSetFont(xIndex, yIndex, &_defFont);
				QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
			}
		}
	}

	RedrawAll();

	RegisterQuoteCallback();
	RegisterOrderallback();
}


void VtAccountRemainGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	return;
}

void VtAccountRemainGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	CUGCell cell;
	LPCTSTR title[6] = { "예탁금잔액", "평가손익", "청산손익", "수수료", "주문가능액", "통화코드" };
	SetColWidth(0, 60);
	SetColWidth(1, 103);
	for (int i = 0; i < _RowCount; i++)
	{
		QuickSetText(0, i, title[i]);
		GetCell(0, i, &cell);
		cell.SetBackColor(DarkHorse::SmTotalManager::GridTitleBackColor);
		cell.SetTextColor(DarkHorse::SmTotalManager::GridTitleTextColor);
		SetCell(0, i, &cell);
		QuickSetAlignment(0, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetFont(0, i, &_titleFont);
		RedrawCell(0, i);
	}
}

void VtAccountRemainGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtAccountRemainGrid::ShowAccountInfo(account_p acnt)
{
	if (!_OrderConfigMgr || !acnt)
		return;
	/*
	if (_OrderConfigMgr->Account()->AccountNo.compare(acnt->AccountNo) != 0)
		return;

	NUMBERFMT numberformat2 = { 0 };
	TCHAR bufILZero[3] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILZERO, bufILZero, 3);
	numberformat2.LeadingZero = _ttoi(bufILZero);
	TCHAR bufINegNum[3] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, bufINegNum, 3);
	numberformat2.NegativeOrder = _ttoi(bufINegNum);
	numberformat2.Grouping = 3;
	TCHAR bufSThousands[5] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, bufSThousands, 5);
	numberformat2.lpThousandSep = bufSThousands;
	TCHAR bufSDecimal[5] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, bufSDecimal, 5);
	numberformat2.lpDecimalSep = bufSDecimal;
	numberformat2.NumDigits = 2;
	CTextNumber::s_lpNumberFormat = &numberformat2;
	CTextNumber tn;

	CUGCell cell;
	CString msg;
	tn = acnt->OpenDeposit;
	msg = tn.GetString();
	QuickSetText(1, 0, msg);
	QuickSetAlignment(1, 0, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	RedrawCell(1, 0);

	tn = acnt->OpenPL;
	msg = tn.GetString();
	QuickSetText(1, 1, msg);
	if (acnt->OpenPL < 0)
		QuickSetTextColor(1, 1, RGB(0, 0, 255));
	else if (acnt->OpenPL > 0)
		QuickSetTextColor(1, 1, RGB(255, 0, 0));
	QuickSetAlignment(1, 1, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	RedrawCell(1, 1);

	tn = acnt->TradePL;
	msg = tn.GetString();
	QuickSetText(1, 2, msg);
	if (acnt->TradePL < 0)
		QuickSetTextColor(1, 2, RGB(0, 0, 255));
	else if (acnt->TradePL > 0)
		QuickSetTextColor(1, 2, RGB(255, 0, 0));
	QuickSetAlignment(1, 2, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	RedrawCell(1, 2);

	tn = acnt->Fee;
	msg = tn.GetString();
	QuickSetText(1, 3, msg);
	QuickSetAlignment(1, 3, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	RedrawCell(1, 3);

	tn = acnt->OrdableAmt;
	msg = tn.GetString();
	QuickSetText(1, 4, msg);
	QuickSetAlignment(1, 4, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	RedrawCell(1, 4);

	QuickSetText(1, 5, acnt->CurrencyCode.c_str());
	RedrawCell(1, 5);
	*/
}

void VtAccountRemainGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void VtAccountRemainGrid::OnReceiveAccountDeposit(account_p acnt)
{
	if (!_OrderConfigMgr)
		return;
	if (_OrderConfigMgr->Account() != acnt)
		return;

	ShowAccountInfo(acnt);
}

void VtAccountRemainGrid::UpdateAccount(VtOrder* order)
{
	UpdateAccountInfo();
}

void VtAccountRemainGrid::UpdateAccount(symbol_p symbol)
{
	UpdateAccountInfo();
}

void VtAccountRemainGrid::UpdateAccountInfo()
{
	if (!_OrderConfigMgr)
		return;
	account_p acnt = _OrderConfigMgr->Account();
	if (!acnt)
		return;

	NUMBERFMT numberformat2 = { 0 };
	TCHAR bufILZero[3] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILZERO, bufILZero, 3);
	numberformat2.LeadingZero = _ttoi(bufILZero);
	TCHAR bufINegNum[3] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, bufINegNum, 3);
	numberformat2.NegativeOrder = _ttoi(bufINegNum);
	numberformat2.Grouping = 3;
	TCHAR bufSThousands[5] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, bufSThousands, 5);
	numberformat2.lpThousandSep = bufSThousands;
	TCHAR bufSDecimal[5] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, bufSDecimal, 5);
	numberformat2.lpDecimalSep = bufSDecimal;
	numberformat2.NumDigits = 2;
	/*
	CTextNumber::s_lpNumberFormat = &numberformat2;
	CTextNumber tn;

	CUGCell cell;
	CString msg;

	tn = acnt->OpenPL;
	msg = tn.GetString();
	QuickSetText(1, 1, msg);
	if (acnt->OpenPL < 0)
		QuickSetTextColor(1, 1, RGB(0, 0, 255));
	else if (acnt->OpenPL > 0)
		QuickSetTextColor(1, 1, RGB(255, 0, 0));
	QuickSetAlignment(1, 1, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	RedrawCell(1, 1);

	tn = acnt->TradePL;
	msg = tn.GetString();
	QuickSetText(1, 2, msg);
	if (acnt->TradePL < 0)
		QuickSetTextColor(1, 2, RGB(0, 0, 255));
	else if (acnt->TradePL > 0)
		QuickSetTextColor(1, 2, RGB(255, 0, 0));
	QuickSetAlignment(1, 2, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	RedrawCell(1, 2);

	tn = acnt->Fee;
	msg = tn.GetString();
	QuickSetText(1, 3, msg);
	QuickSetAlignment(1, 3, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	RedrawCell(1, 3);

	tn = acnt->OrdableAmt;
	msg = tn.GetString();
	QuickSetText(1, 4, msg);
	QuickSetAlignment(1, 4, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	RedrawCell(1, 4);

	QuickSetText(1, 5, acnt->CurrencyCode.c_str());
	RedrawCell(1, 5);
	*/
}

