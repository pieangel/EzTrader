

#include "stdafx.h"
#include "VtProductRemainGridEx.h"
#include "../Resource.h"
//#include "../Order/VtOrder.h"
//#include "../Order/VtOrderManager.h"
//#include "../Symbol/VtSymbolManager.h"
//#include "../Symbol/VtSymbolMaster.h"
//#include "../Symbol/VtSymbol.h"
//#include "../Account/VtAccountManager.h"
//#include "../Account/VtAccount.h"
//#include "../Position/VtPosition.h"
//#include "../Order/VtOrderManagerSelector.h"
#include "VtOrderConfigManager.h"
#include "VtOrderWnd.h"
#include "VtOrderLeftWnd.h"
//#include "../Hoga/VtHoga.h"
//#include "../Global/MainBeetle.h"
#include <algorithm> 
#include <functional>
//#include "../Task/SmCallbackManager.h"
//#include "../Log/loguru.hpp"
//#include "../Account/VtSubAccountManager.h"
//#include "../Fund/VtFund.h"
//#include "../Format/format.h"
#include "../Global/SmTotalManager.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace std::placeholders;



VtProductRemainGridEx::VtProductRemainGridEx()
{
	RegisterQuoteCallback();
}


VtProductRemainGridEx::~VtProductRemainGridEx()
{
	UnregisterAllCallback();
}


void VtProductRemainGridEx::UnregisterAllCallback()
{
	UnregisterQuoteCallback();
}

void VtProductRemainGridEx::RegisterQuoteCallback()
{
	//mainApp.CallbackMgr().SubscribeQuoteCallback((long)this, std::bind(&VtProductRemainGridEx::OnQuoteEvent, this, _1));
}

void VtProductRemainGridEx::UnregisterQuoteCallback()
{
	//mainApp.CallbackMgr().UnsubscribeQuoteCallback((long)this);
}

void VtProductRemainGridEx::OnQuoteEvent(VtSymbol* symbol)
{
	if (!symbol)
		return;

	// ���⼭ �� ������ ������Ʈ �Ѵ�.
	for (auto it = _RowToPositionMap.begin(); it != _RowToPositionMap.end(); ++it) {
		VtPosition* posi = it->second;
		int row = it->first;
		/*
		if (posi->OpenQty != 0 && posi->ShortCode.compare(symbol->ShortCode) == 0) {
			std::string strVal = fmt::format("{:.{}f}", posi->OpenProfitLoss, symbol->Decimal);
			if (posi->OpenProfitLoss > 0)
				QuickSetTextColor(3, row, RGB(255, 0, 0));
			else
				QuickSetTextColor(3, row, RGB(0, 0, 255));
			
			QuickSetText(3, row, strVal.c_str());
			RedrawCell(3, row);
		}
		*/
	}
}

void VtProductRemainGridEx::OnSetup()
{
	//m_OutBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_OUT));
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		QuickSetCellType(0, yIndex, UGCT_CHECKBOX);
		QuickSetCellTypeEx(0, yIndex, UGCT_CHECKBOXCHECKMARK);
		QuickSetBackColor(0, yIndex, RGB(255, 255, 255));
		QuickSetAlignment(0, yIndex, UG_ALIGNVCENTER | UG_ALIGNCENTER);
		QuickRedrawCell(0, yIndex);

		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			//m_data[yIndex].m_colData[xIndex] = rand() % 100;
			CString msg;
			msg.Format(_T("%d, %d value"), yIndex, xIndex);
			//QuickSetText(xIndex, yIndex, msg);
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER);
			
		}
	}

	SetVS_Width(0);
	SetHS_Height(0);
	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
}



void VtProductRemainGridEx::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (updn)
	{
		CString msg;
		msg = QuickGetText(1, row);
		if (msg.GetLength() > 0)
		{
			std::string shorCode = (const char*)msg;
			/*
			VtSymbol* sym = mainApp.SymbolMgr().FindSymbol(shorCode);
			if (sym && _OrderConfigMgr) {
				_OrderConfigMgr->Symbol(sym);

				if (_OrderConfigMgr->centerWnd) {
					_OrderConfigMgr->centerWnd->ChangeSymbol(sym);
				}
			}
			*/
		}
	}
}

int VtProductRemainGridEx::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	if (row == -1)
	{
		for (int i = 0; i < _DataCount; i++)
		{
			CUGCell cell;
			GetCell(0, i, &cell);
			int style = 0;
			if (cell.IsPropertySet(UGCELL_CELLTYPEEX_SET))
				style = cell.GetCellTypeEx();

			int val = (int)m_cell.GetNumber();
			cell.SetNumber((val = (val + 1) % ((style & UGCT_CHECKBOX3STATE) ? 3 : 2)));
			SetCell(0, i, &cell);
			RedrawCell(0, i);
		}
	}
	return TRUE;
}

void VtProductRemainGridEx::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[4] = { "û��", "����", "����", "�򰡼���" };
	int colWidth[4] = { 21, 46, 32, 58 };

	QuickSetCellType(0, -1, UGCT_CHECKBOX);
	QuickSetCellTypeEx(0, -1, UGCT_CHECKBOXCHECKMARK);
	QuickSetBackColor(0, -1, RGB(255, 255, 255));
	QuickSetAlignment(0, -1, UG_ALIGNVCENTER | UG_ALIGNCENTER);
	QuickRedrawCell(0, -1);

	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		if (i != 0) {
			cell.SetBackColor(DarkHorse::SmTotalManager::GridTitleBackColor);
			cell.SetTextColor(DarkHorse::SmTotalManager::GridTitleTextColor);
		}
		else {
			cell.SetBackColor(RGB(255, 255, 255));
			cell.SetTextColor(RGB(255, 255, 255));
		}
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		RedrawCell(i, -1);
	}
}

void VtProductRemainGridEx::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtProductRemainGridEx::ResetCellText()
{
	for (int i = 0; i < DataCount(); i++)
	{
		for (int j = 1; j < _ColCount; j++)
		{
			QuickSetText(j, i, _T(""));
			QuickRedrawCell(j, i);
		}
	}
}

void VtProductRemainGridEx::SetRemainList()
{
	VtAccount* acnt = _OrderConfigMgr->Account();
	if (!acnt)
		return;

	ResetCellText();
	_RowToPositionMap.clear();

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
	//CTextNumber::s_lpNumberFormat = &numberformat2;
	//CTextNumber tn;

	int row = 0;
	CUGCell cell;
	/*
	for (auto it = acnt->PositionMap.begin(); it != acnt->PositionMap.end(); ++it)
	{
		VtPosition* posi = it->second;
		VtSymbol* sym = mainApp.SymbolMgr().FindSymbol(posi->ShortCode);
		if (!sym) continue;
		if (posi->OpenQty == 0) continue;


		QuickSetText(1, row, sym->ShortCode.c_str());
		GetCell(1, row, &cell);
		cell.Tag((void*)sym);
		SetCell(1, row, &cell);

		std::string position;
		switch (posi->Position)
		{
		case VtPositionType::Buy:
			{
			position = _T("�ż�");
			QuickSetTextColor(1, row, RGB(255, 0, 0));
			QuickSetTextColor(2, row, RGB(255, 0, 0));
			}
			break;
		case VtPositionType::Sell:
			{
			position = _T("�ŵ�");
			QuickSetTextColor(1, row, RGB(0, 0, 255));
			QuickSetTextColor(2, row, RGB(0, 0, 255));

			}
			break;
		default:
			position = _T("����");
			break;
		}


		QuickSetText(2, row, position.c_str());

		tn = posi->OpenProfitLoss;
		if (posi->OpenProfitLoss > 0)
			QuickSetTextColor(3, row, RGB(255, 0, 0));
		else
			QuickSetTextColor(3, row, RGB(0, 0, 255));
		QuickSetAlignment(3, row, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetText(3, row, tn.GetString());


		GetCell(0, row, &cell);
		cell.Tag((void*)posi);
		SetCell(0, row, &cell);

		_RowToPositionMap[row] = posi;

		QuickRedrawCell(1, row);
		QuickRedrawCell(2, row);
		QuickRedrawCell(3, row);
		row++;
	}

	DataCount(row);
	*/

}


void VtProductRemainGridEx::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void VtProductRemainGridEx::LiqSelectedPos()
{
	if (_DataCount == 0)
		return;
	_SelectedPosList.clear();
	CUGCell cell;
	for (int i = 0; i < DataCount(); i++)
	{
		GetCell(0, i, &cell);
		int val = (int)cell.GetNumber();
		VtPosition* posi = (VtPosition*)cell.Tag();
		if (val == 1 && posi)
		{
			_SelectedPosList.push_back(posi);
		}
	}

	LiqList();

}

void VtProductRemainGridEx::LiqAll()
{
	if (_DataCount == 0)
		return;

	_SelectedPosList.clear();
	CUGCell cell;
	for (int i = 0; i < DataCount(); i++)
	{
		GetCell(0, i, &cell);
		VtPosition* posi = (VtPosition*)cell.Tag();
		_SelectedPosList.push_back(posi);
	}

	GetCell(0, -1, &cell);
	cell.SetNumber(0);
	SetCell(0, -1, &cell);
	RedrawCell(0, -1);

	ResetCellText();

	LiqList();
}

void VtProductRemainGridEx::PutOrder(VtPosition* posi, int price, bool liqud /*= false*/)
{
	if (!posi || !_OrderConfigMgr)
		return;
	/*
	if (posi->OpenQty == 0)
		return;

	VtAccount* acnt = nullptr;
	acnt = mainApp.AcntMgr().FindAccount(posi->AccountNo);
	if (!acnt) { // �����¿� ���� ��� ���� ���¸� ã�� ����.
		acnt = mainApp.AcntMgr().SubAcntMgr().FindAccount(posi->AccountNo);
	}

	if (!acnt) // ���°� ���� ��� �ƹ��ϵ� ���� ����.
		return;

	std::shared_ptr<HdOrderRequest> request = std::make_shared<HdOrderRequest>();
	request->Market = 1; // �ؿ� ���� ���� ����
	request->Amount = std::abs(posi->OpenQty);
	if (_OrderConfigMgr->Type() == 0) { // �����³� ���� �����϶�
		if (acnt->AccountLevel() == 0) { // ������ �� ��
			request->AccountNo = acnt->AccountNo;
			request->Password = acnt->Password;
			request->Type = 0;
		}
		else { // ���� ���� �� ��
			VtAccount* parentAcnt = acnt->ParentAccount();
			if (parentAcnt) { // �θ� ���°� �ִ��� Ȯ�� �Ѵ�.
				request->AccountNo = parentAcnt->AccountNo;
				request->Password = parentAcnt->Password;
			}
			request->Type = 1;
		}
	}
	else { // �ݵ� �ֹ��϶�
		VtAccount* parentAcnt = acnt->ParentAccount();
		if (parentAcnt) { // �θ� ���°� �ִ��� Ȯ�� �Ѵ�.
			request->AccountNo = parentAcnt->AccountNo;
			request->Password = parentAcnt->Password;
		}
		request->Type = 2;
	}

	request->SymbolCode = posi->ShortCode;
	request->FillCondition = VtFilledCondition::Day;

	if (liqud) { // ���尡 û���� ���
		request->Price = 0;
		request->PriceType = VtPriceType::Market;
	}
	else { // ������ û���� ���
		request->PriceType = VtPriceType::Price;
		request->Price = price;
	}

	if (posi->Position == VtPositionType::Buy) { // �ż� û���� ���
		request->Position = VtPositionType::Sell;
	}
	else if (posi->Position == VtPositionType::Sell) { // �ŵ� û���� ���
		request->Position = VtPositionType::Buy;
	}

	request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
	request->SourceId = (long)this;

	if (acnt->AccountLevel() == 0) { // ������ �� ���
		request->SubAccountNo = _T("");
		request->FundName = _T("");
	}
	else { // ���� ������ ���
		request->SubAccountNo = acnt->AccountNo;
		if (acnt->Fund())
			request->FundName = acnt->Fund()->Name;
		else
			request->FundName = _T("");
	}

	request->orderType = VtOrderType::New;
	// ���� �ֹ��̳� û��
	// 0 : �Ϲ� , 1 : ����, 2 : ����- û�� �ֹ��� ���� ���� ������ Ȱ��ȭ �Ǿ� �־ ó������ �ʴ´�.
	request->RequestType = 1;

	_OrderConfigMgr->OrderMgr()->PutOrder(request);
	*/
}

void VtProductRemainGridEx::LiqList()
{
	if (_SelectedPosList.size() == 0)
		return;

	for (auto it = _SelectedPosList.begin(); it != _SelectedPosList.end(); ++it)
	{
		VtPosition* posi = *it;
		LiqudRemain(posi);
	}

	_SelectedPosList.clear();
}

void VtProductRemainGridEx::LiqudRemain(VtPosition* posi)
{
	if (!posi || !_OrderConfigMgr->Symbol())
		return;
	PutOrder(posi, 0, true);
}

void VtProductRemainGridEx::SetFilledOrderList()
{
	std::string symbol_code;
	/*
	VtOrderManager* orderMgr = mainApp.TotalOrderMgr().FindAddAccountOrderManger(symbol_code);
	int row = 0;
	CUGCell cell;
	for (auto it = orderMgr->FilledList.begin(); it != orderMgr->FilledList.end(); ++it)
	{
		VtOrder* order = *it;
		QuickSetText(1, row, order->shortCode.c_str());
		std::string position;
		switch (order->orderPosition)
		{
		case VtPositionType::Buy:
			position = _T("�ż�");
			break;
		case VtPositionType::Sell:
			position = _T("�ŵ�");
			break;
		default:
			position = _T("����");
			break;
		}
		QuickSetText(2, row, position.c_str());

		std::string remainQty = fmt::format("{:.{}f}", order->unacceptedQty, 0);
		QuickSetText(3, row, remainQty.c_str());

		VtSymbol* symbol = mainApp.SymbolMgr().FindSymbol(order->shortCode);
		std::string price = fmt::format("{:.{}f}", order->orderPrice, symbol->Decimal);
		QuickSetText(4, row, price.c_str());

		std::string amount = fmt::format("{:.{}f}", order->amount, 0);
		QuickSetText(5, row, amount.c_str());

		QuickSetText(6, row, order->tradeTime.c_str());
		std::string orderNo = order->orderNo; // NumberFormatter::format(order->orderNo, 5);
		QuickSetText(7, row, orderNo.c_str());

		std::string oriOrderNo = order->oriOrderNo; // NumberFormatter::format(order->oriOrderNo, 5);
		QuickSetText(8, row, oriOrderNo.c_str());

		GetCell(0, row, &cell);
		cell.Tag((void*)order);

		QuickRedrawCell(1, row);
		QuickRedrawCell(2, row);
		QuickRedrawCell(3, row);
		QuickRedrawCell(4, row);
		QuickRedrawCell(5, row);
		QuickRedrawCell(6, row);
		QuickRedrawCell(7, row);
		QuickRedrawCell(8, row);

		row++;
	}

	DataCount(row);
	*/
}


