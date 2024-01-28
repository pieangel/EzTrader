#include "stdafx.h"
#include "SmOrderGridOut.h"
#include "../VtDefine.h"
//#include "../Global/MainBeetle.h"
//#include "../Symbol/VtSymbol.h"
//#include "../Symbol/VtSymbolManager.h"
//#include "../Quote/VtQuote.h"
//#include "../Symbol/VtSymbolMaster.h"
//#include "../Hoga/VtHoga.h"
//#include "../UGrid/VtCellLabel.h"
//#include "../Account/VtAccountManager.h"
//#include "../Order/VtOrder.h"
//#include "../Order/VtOrderManager.h"
//#include "../Order/VtOrderManagerSelector.h"
//#include "../Account/VtAccount.h"
//#include "../Order/VtOrderManagerSelector.h"
//#include "../Order/VtProductOrderManagerSelector.h"
//#include "../Order/VtProductOrderManager.h"
#include <chrono>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include "../resource.h"
#include "VtStopOrderManager.h"
//#include "../Position/VtPosition.h"
#include "VtOrderConfigManager.h"
//#include "../Order/VtFundOrderManager.h"
//#include "../Fund/VtFund.h"
//#include "../Account/VtSubAccountManager.h"
//#include "../Format/format.h"
#include "VtCutManager.h"
//#include "../Format/XFormatNumber.h"
#include <algorithm> 
#include <functional>
//#include "../Task/SmCallbackManager.h"
//#include "../Order/VtFundOrderManager.h"
//#include "../Log/loguru.hpp"
#include "SmOrderPanelOut.h"
#include "VtOrderWnd.h"
#include "../MessageDefine.h"
//#include "../Main/MainBeetle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace std::placeholders;

constexpr int Round(float x) { return static_cast<int>(x + 0.5f); }

SmOrderGridOut::SmOrderGridOut()
{
	BuyColor.push_back(RGB(253, 173, 176));
	BuyColor.push_back(RGB(255, 196, 199));
	BuyColor.push_back(RGB(255, 214, 212));
	BuyColor.push_back(RGB(255, 224, 225));
	BuyColor.push_back(RGB(255, 232, 232));

	SellColor.push_back(RGB(159, 214, 255));
	SellColor.push_back(RGB(185, 224, 255));
	SellColor.push_back(RGB(204, 230, 250));
	SellColor.push_back(RGB(221, 243, 255));
	SellColor.push_back(RGB(230, 247, 255));

	_CellHeight = 20;

	m_bMouseTracking = FALSE;

	_OldIndexRow = -1;
	_OldClickedCell.row = -1;
	_OldClickedCell.col = -1;
	_OldMMCell.row = -1;
	_OldMMCell.col = -1;
	_OldMovingCellCenter.row = -1;
	_OldMovingCellCenter.col = -1;
	_OldMovingCellSide.row = -1;
	_OldMovingCellSide.col = -1;

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"));
	_Init = false;

	_OldPositionCell.row = -1;
	_OldPositionCell.col = -1;

	_GridColMap[SmOrderGridCol::STOP] = 48;
	_GridColMap[SmOrderGridCol::ORDER] = 60;
	_GridColMap[SmOrderGridCol::COUNT] = 45;
	_GridColMap[SmOrderGridCol::QUANTITY] = 45;
	_GridColMap[SmOrderGridCol::CENTER] = 80;

	_HeadHeight = 20;

	_CutMgr = new VtCutManager();

	_StopOrderMgr = new VtStopOrderManager();
}


SmOrderGridOut::~SmOrderGridOut()
{
	if (_StopOrderMgr) {
		_StopOrderMgr->RemoveAllHd();
		delete _StopOrderMgr;
		_StopOrderMgr = nullptr;
	}

	if (_CutMgr) {
		delete _CutMgr;
		_CutMgr = nullptr;
	}
}

void SmOrderGridOut::OnSymbolMaster(symbol_p sym)
{
	if (!sym)
		return;
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	/*
	symbol_p symbol = _CenterWnd->Symbol();
	if (symbol->ShortCode.compare(sym->ShortCode) != 0) {
		return;
	}
	*/
	ResetByCenterRow();
}

void SmOrderGridOut::SetAutoStopOnFilled(VtOrder* order)
{
	if (!order)
		return;
	//_CutMgr->AddStopOrderForFilled(_CenterWnd->Symbol(), order);
}

void SmOrderGridOut::UnregisterAllCallback()
{
	UnregisterHogaCallback();
	UnregisterOrderCallback();
	UnregisterQuoteCallback();
}

void SmOrderGridOut::RegisterQuoteCallback()
{
	//mainApp.CallbackMgr().SubscribeQuoteCallback((long)this, std::bind(&SmOrderGridOut::OnQuoteEvent, this, _1));
	//mainApp.CallbackMgr().SubscribeQuoteWndCallback(GetSafeHwnd());
	//mainApp.CallbackMgr().SubscribeOrderWndCallback(GetSafeHwnd());
}

void SmOrderGridOut::UnregisterQuoteCallback()
{
	//mainApp.CallbackMgr().UnsubscribeQuoteCallback((long)this);
}

void SmOrderGridOut::OnQuoteEvent(symbol_p symbol)
{
	if (!_Init || !symbol)
		return;

	if (!_CenterWnd || !_CutMgr || !_CenterWnd->Symbol()) {
		return;
	}
	/*
	if (_CenterWnd->Symbol()->ShortCode.compare(symbol->ShortCode) != 0)
		return;

	// �ܰ��� ���Ͽ� ������ ������ Ȯ���Ѵ�.
	// ����, ���� ��ž �ֹ��� ������ ��Ҵ��� �˻��Ѵ�.
	CheckProfitLossTouchHd(_CenterWnd->Symbol()->Quote.intClose);
	std::set<std::pair<int, int>> refreshSet;
	// ��ž �ֹ��� ������ ��Ҵ��� �˻��Ѵ�.
	if (CheckStopTouchedHd(_CenterWnd->Symbol()->Quote.intClose)) {
		// ��ž�ֹ��� �ٽ� �׷��ش�.
		//ClearOldStopOrders(refreshSet);
		//SetStopOrderInfo(refreshSet);
		//CalcPosStopOrders(refreshSet);
		//RefreshCells(refreshSet);
	}

// 	ClearQuotes(refreshSet);
// 	// �߾� �����϶��� ���� ���� ���� ã�Ƽ� ��Ī���ش�.
// 	if (_CenterWnd->FixedCenter()) {
// 		_IndexRow = FindIndexRow();
// 		SetCenterValue(_CenterWnd->Symbol(), refreshSet);
// 	}
// 	SetQuoteColor(_CenterWnd->Symbol(), refreshSet);
// 
// 	RefreshCells(refreshSet);
*/
}

void SmOrderGridOut::RegisterHogaCallback()
{
	//mainApp.CallbackMgr().SubscribeHogaWndCallback(GetSafeHwnd());
}

void SmOrderGridOut::UnregisterHogaCallback()
{
	//mainApp.CallbackMgr().UnsubscribeHogaCallback((long)this);
}

void SmOrderGridOut::OnHogaEvent(symbol_p symbol)
{
	if (!_Init || !symbol)
		return;

	if (!_CenterWnd || !_CutMgr || !_CenterWnd->Symbol()) {
		return;
	}
	/*
	if (_CenterWnd->Symbol()->ShortCode.compare(symbol->ShortCode) != 0)
		return;

// 	std::set<std::pair<int, int>> refreshSet;
// 	ClearHogas(refreshSet);
// 	SetHogaInfo(symbol, refreshSet);
// 	RefreshCells(refreshSet);
// 	Invalidate();
*/
}

void SmOrderGridOut::RegisterOrderallback()
{
	//mainApp.CallbackMgr().SubscribeOrderCallback((long)this, std::bind(&SmOrderGridOut::OnOrderEvent, this, _1));
}

void SmOrderGridOut::UnregisterOrderCallback()
{
	//mainApp.CallbackMgr().UnsubscribeOrderCallback((long)this);
}

void SmOrderGridOut::OnOrderEvent(VtOrder* order)
{
	if (!_Init || !order || !_OrderConfigMgr)
		return;

	if (!_CenterWnd || !_CutMgr || !_CenterWnd->Symbol()) {
		return;
	}


	// ����â ����Ȯ�� ����� ������Ʈ �Ѵ�.
	_OrderConfigMgr->leftWnd->RefreshAcceptedList();
	_OrderConfigMgr->leftWnd->RefreshRemainList();

	std::set<std::pair<int, int>> refreshSet;
	ClearOldOrders(refreshSet);
	SetOrderInfo(refreshSet);
	ClearPositionInfo(refreshSet);
	SetPositionInfo(refreshSet);
	RefreshCells(refreshSet);
	/*
	if (order->Type == -1 || order->Type == 0) {
		if (!_OrderConfigMgr->Account())
			return;
		// �ɺ��� ���°� ���� ������ �������� �ʴ´�.
		if (_CenterWnd->Symbol()->ShortCode.compare(order->shortCode) != 0 ||
			_OrderConfigMgr->Account()->AccountNo.compare(order->AccountNo) != 0)
			return;
	}
	else if (order->Type == 1) {
		if (!_OrderConfigMgr->Account())
			return;
		// �ɺ��� ���°� ���� ������ �������� �ʴ´�.
		if (_CenterWnd->Symbol()->ShortCode.compare(order->shortCode) != 0 ||
			_OrderConfigMgr->Account()->AccountNo.compare(order->SubAccountNo) != 0)
			return;
	}
	else if (order->Type == 2) {
		if (!_OrderConfigMgr->Fund())
			return;
		// �ɺ��� ���°� ���� ������ �������� �ʴ´�.
		if (_CenterWnd->Symbol()->ShortCode.compare(order->shortCode) != 0 ||
			_OrderConfigMgr->Fund()->Name.compare(order->FundName) != 0)
			return;
	}
	else
		return;

	if (order->state == VtOrderState::Filled) {
		refreshSet.clear();
		_CutMgr->AddStopOrderForFilled(_CenterWnd->Symbol(), order);
		ClearOldStopOrders(refreshSet);
		SetStopOrderInfo(refreshSet);
		CalcPosStopOrders(refreshSet);
		RefreshCells(refreshSet);
	}
	*/
}

void SmOrderGridOut::Init()
{
	// ���� �ʱ�ȭ ���� �ʾ��� ���� �׸��忡 ������ �Ѵ�.
	if (!_Init) {
		UnregisterButtons();
		_RowCount = GetMaxValueRowCount() + 1;
		_IndexRow = FindIndexRow();
		_EndRowForValue = _RowCount - 3;
		SetRowCount(_RowCount);
		SetColumnCount(_ColCount);
		SetFixedRowCount(_FixedRow);
		SetFixedColumnCount(_FixedCol);
		SetColTitle(_Init);
		EnableScrollBars(SB_VERT, FALSE);
		EnableScrollBars(SB_HORZ, FALSE);
		SetFixedColumnSelection(FALSE);
		SetFixedRowSelection(FALSE);
		EnableColumnHide();
		SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
		SetDoubleBuffering(TRUE);
		EnableSelection(FALSE);
		SetEditable(FALSE);
		EnableTitleTips(FALSE);
		SetColumnResize(FALSE);
		SetRowResize(FALSE);
		AllowReorderColumn(false);

		for (int i = 1; i < m_nRows; ++i) {
			SetRowHeight(i, _CellHeight);
		}

		RegisterButtons();

		SetFont(&_defFont);
		SetOrderAreaColor();


		RegisterQuoteCallback();
		RegisterHogaCallback();
		RegisterOrderallback();
	}

	// �׸��� ������� ä���.
	SetCenterValue();

	_Init = true;
}


void SmOrderGridOut::RegisterButtons()
{
	// ���Ĺ�ư ���
	RegisterButton(1, 0, CenterCol, GetSysColor(COLOR_BTNFACE), RGB(0, 0, 0), "����");
	MergeCells(1, CenterCol - 2, 1, CenterCol - 1);
	MergeCells(_EndRowForValue + 2, CenterCol - 2, _EndRowForValue + 2, CenterCol - 1);
	// ���尡 �ŵ�
	std::string title;
	if (_CenterWnd && _CenterWnd->ShowOrderCountArea())
		title = "���尡�ŵ�";
	else
		title = "���尡";
	RegisterButton(2, 1, CenterCol - 2, RGB(19, 137, 255), RGB(255, 255, 255), title.c_str());
	CGridCellBase* pCell = GetCell(1, CenterCol);
	if (pCell) {
		pCell->SetText("���尡");
		// �ؽ�Ʈ ����
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		// �� ��� �� ����
		pCell->SetBackClr(GetSysColor(COLOR_BTNFACE));
	}
	MergeCells(1, CenterCol + 1, 1, CenterCol + 2);
	MergeCells(_EndRowForValue + 2, CenterCol + 1, _EndRowForValue + 2, CenterCol + 2);
	// ���尡 �ż� 
	if (_CenterWnd && _CenterWnd->ShowOrderCountArea())
		title = "���尡�ż�";
	else
		title = "���尡";
	RegisterButton(3, 1, CenterCol + 1, RGB(240, 51, 58), RGB(255, 255, 255), title.c_str());
	// �ŵ���ž ��� 
	RegisterButton(4, _EndRowForValue + 2, CenterCol - 4, RGB(190, 190, 245), "ST���");
	// �ŵ��ֹ� ��� 
	RegisterButton(5, _EndRowForValue + 2, CenterCol - 3, RGB(190, 190, 245), "�ϰ����");
	// ����ֹ� ���
	RegisterButton(6, _EndRowForValue + 2, CenterCol, RGB(190, 190, 245), "��ü���");
	// �ż��ֹ����
	RegisterButton(7, _EndRowForValue + 2, CenterCol + 3, RGB(252, 190, 190), "�ϰ����");
	// �ż���ž ���
	RegisterButton(8, _EndRowForValue + 2, CenterCol + 4, RGB(252, 190, 190), "ST���");
}

void SmOrderGridOut::UnregisterButtons()
{
	for (int id = 1; id <= 9; ++id) {
		UnregisterButton(id);
	}

	RestoreCells(1, CenterCol - 2, 1, CenterCol - 1);
	RestoreCells(_EndRowForValue + 2, CenterCol - 2, _EndRowForValue + 2, CenterCol - 1);

	RestoreCells(1, CenterCol + 1, 1, CenterCol + 2);
	RestoreCells(_EndRowForValue + 2, CenterCol + 1, _EndRowForValue + 2, CenterCol + 2);
}


void SmOrderGridOut::BuyAtMarket()
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;
	PutOrder(0, VtPositionType::Buy, VtPriceType::Market);
}

void SmOrderGridOut::SellAtMarket()
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;
	PutOrder(0, VtPositionType::Sell, VtPriceType::Market);
}

void SmOrderGridOut::CancelAllSellStop()
{
	_StopOrderMgr->RemoveSellAll();
	_CutMgr->RemoveSellAllHd();
	RefreshStopOrder();
}

void SmOrderGridOut::CancelAllBuyStop()
{
	_StopOrderMgr->RemoveBuyAll();
	_CutMgr->RemoveBuyAllHd();
	RefreshStopOrder();
}

void SmOrderGridOut::CancelAllSellAccepted()
{
	if (!_CenterWnd)
		return;

	/*
	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		if (order->orderPosition == VtPositionType::Sell)
			CancelOrder(order);
	}
	*/
}

void SmOrderGridOut::CancelAllBuyAccepted()
{
	if (!_CenterWnd)
		return;
	/*
	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		if (order->orderPosition == VtPositionType::Buy)
			CancelOrder(order);
	}
	*/
}

void SmOrderGridOut::CancelAllAccepted()
{
	if (!_CenterWnd)
		return;
	/*
	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		CancelOrder(order);
	}
	*/
}

void SmOrderGridOut::RefreshStopOrder()
{
	std::set<std::pair<int, int>> refreshSet;
	ClearOldStopOrders(refreshSet);
	SetStopOrderInfo(refreshSet);
	CalcPosStopOrders(refreshSet);
	RefreshCells(refreshSet);
}

void SmOrderGridOut::CheckProfitLossTouchHd(int intClose)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_StopOrderMgr)
		return;
	/*
	symbol_p sym = _CenterWnd->Symbol();

	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(), next_it = it; it != _StopOrderMgr->StopOrderMapHd.end(); it = next_it) {
		std::shared_ptr<HdOrderRequest> stop = it->second;
		++next_it;

		if (stop->Price == intClose) {
			if (stop->Position == VtPositionType::Buy) {
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, stop->slip, sym);
			}
			else {
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, -stop->slip, sym);
			}
			_OrderConfigMgr->OrderMgr()->PutOrder(stop);
			// ������ ���� ��ž �ֹ��� ��Ͽ��� ���� �ش�.
			_StopOrderMgr->StopOrderMapHd.erase(it);
		}
	}
	*/
}

bool SmOrderGridOut::CheckStopTouchedHd(int intClose)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_CutMgr)
		return false;
	/*
	std::vector<int> removeList;
	symbol_p sym = _CenterWnd->Symbol();
	std::map<int, std::shared_ptr<HdOrderRequest>>& profitLossMap = _CutMgr->GetStopOrderMap();
	for (auto it = profitLossMap.begin(); it != profitLossMap.end(); ++it) {
		std::shared_ptr<HdOrderRequest> stop = it->second;
		//LOG_F(INFO, _T("CheckProfitLossTouchHd : ������������ %d"), stop->Price);
		if (stop->Price == intClose) { // ������ ��ġ�ϴ� ���
			if (stop->Position == VtPositionType::Buy) { // �ż� �������� ���
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, stop->slip, sym);
			}
			else { // �ŵ� �������� ���
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, -stop->slip, sym);
			}

			removeList.push_back(stop->RequestId);  // ���� ��Ͽ� �����Ѵ�.

			if (stop->Paired && stop->PairedReq) { // ����, ���� ������ ����Ǿ� �ִ� ��� ������ ������� �ٸ� ���ʵ� ���� ��Ͽ� �־��ش�.
				removeList.push_back(stop->PairedReq->RequestId);
			}
			_OrderConfigMgr->OrderMgr()->PutOrder(stop);
		}
	}
	bool result = false;
	for (auto it = removeList.begin(); it != removeList.end(); ++it) { // ������ ��ϵ� ���� ��ϵ��� ��Ͽ��� �����ش�.
		_CutMgr->RemoveOrderHd(*it, false);
		result = true;
	}
	
	return result;
	*/
	return false;
}

void SmOrderGridOut::SetColTitle(bool init)
{
	const int ColCount = _ColCount;
	LPCTSTR title[9] = { "STOP", "�ֹ�", "�Ǽ�", "�ܷ�", "����", "�ܷ�", "�Ǽ�", "�ֹ�", "STOP" };
	int colWidth[9] = {
		_GridColMap[SmOrderGridCol::STOP],
		_GridColMap[SmOrderGridCol::ORDER],
		_GridColMap[SmOrderGridCol::COUNT],
		_GridColMap[SmOrderGridCol::QUANTITY],
		_GridColMap[SmOrderGridCol::CENTER],
		_GridColMap[SmOrderGridCol::QUANTITY],
		_GridColMap[SmOrderGridCol::COUNT],
		_GridColMap[SmOrderGridCol::ORDER],
		_GridColMap[SmOrderGridCol::STOP]
	};

	SetRowHeight(0, _HeadHeight);

	/*
	for (int i = 0; i < _ColCount; i++) {
		if (!init)
			SetColumnWidth(i, colWidth[i]);

		CGridCellBase* pCell = GetCell(0, i);
		pCell->SetText(title[i]);
		// �ؽ�Ʈ ����
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		// �� ��� �� ����
		pCell->SetBackClr(MainBeetle::GridTitleBackColor);
		// �� ���ڻ� ����
		pCell->SetTextClr(MainBeetle::GridTitleTextColor);

		InvalidateCellRect(0, i);
	}
	*/
}

void SmOrderGridOut::SetCenterValue(const symbol_p symbol, std::set<std::pair<int, int>>& refreshSet)
{
	if (!symbol)
		return;
	/*
	std::string code = symbol->ShortCode.substr(0, 1);
	if ((code.compare(_T("2")) == 0) || (code.compare(_T("3")) == 0)) {
		SetCenterValueForOption(symbol, refreshSet);
	}
	else
	{
		SetCenterValueForFuture(symbol, refreshSet);
	}
	*/
}

void SmOrderGridOut::SetCenterValue()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	symbol_p symbol = _CenterWnd->Symbol();
	if (symbol) {
		std::set<std::pair<int, int>> refreshSet;
		SetCenterValue(symbol, refreshSet);
		ClearQuotes(refreshSet);
		SetQuoteColor(symbol, refreshSet);

		ClearHogas(refreshSet);
		SetHogaInfo(symbol, refreshSet);

		ClearOldOrders(refreshSet);
		SetOrderInfo(refreshSet);

		ClearOldStopOrders(refreshSet);
		SetStopOrderInfo(refreshSet);
		CalcPosStopOrders(refreshSet);

		ClearPositionInfo(refreshSet);
		SetPositionInfo(refreshSet);

		RefreshCells(refreshSet);
	}
}

void SmOrderGridOut::SetCenterValueByFixed(const symbol_p symbol, std::set<std::pair<int, int>>& refreshSet)
{
	if (!symbol)
		return;
	/*
	std::string code = symbol->ShortCode.substr(0, 1);
	if ((code.compare(_T("2")) == 0) || (code.compare(_T("3")) == 0)) {
		SetCenterValueByFixedForOption(symbol, refreshSet);
	}
	else {
		SetCenterValueByFixedForFuture(symbol, refreshSet);
	}
	*/
}

void SmOrderGridOut::ClearQuotes(std::set<std::pair<int, int>>& refreshSet)
{
	for (auto it = _QuotePos.begin(); it != _QuotePos.end(); ++it) {
		std::pair<int, int> pos = *it;
		if (pos.first < _StartRowForValue ||
			pos.first > _EndRowForValue)
			continue;
		refreshSet.insert(std::make_pair(pos.first, pos.second));
		CGridCellBase* pCell = GetCell(pos.first, pos.second);
		if (pCell) {
			pCell->SetTextClr(RGB(0, 0, 0));
			pCell->SetLabel("");
		}
	}
}

void SmOrderGridOut::SetCenterValueForFuture(const symbol_p sym, std::set<std::pair<int, int>>& refreshSet)
{
	if (!sym)
		return;
	/*
	// Set the close value on the center of the center grid
	int startValue = sym->Quote.intClose + (sym->intTickSize * (_IndexRow - _StartRowForValue));

	ValueToRowMap.clear();
	RowToValueMap.clear();
	for (int i = _StartRowForValue; i <= _EndRowForValue; i++) {
		int curValue = startValue - sym->intTickSize * (i - 1);
		CGridCellBase* pCell = GetCell(i, CenterCol);
		std::string strVal = fmt::format("{:.{}f}", curValue / std::pow(10, sym->Decimal), sym->Decimal);
		pCell->SetText(strVal.c_str());
		pCell->SetLongValue(curValue);
		pCell->SetTextClr(RGB(0, 0, 0));
		// �ؽ�Ʈ ����
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//RedrawCell(i, CenterCol);
		refreshSet.insert(std::make_pair(i, CenterCol));
		ValueToRowMap[curValue] = i;
		RowToValueMap[i] = curValue;
	}
	*/
}

void SmOrderGridOut::SetCenterValueForOption(const symbol_p sym, std::set<std::pair<int, int>>& refreshSet)
{
	if (!sym)
		return;
	/*
	int endValue = sym->Quote.intClose;
	int endRow = _EndRowForValue - 1;
	int zeroRow = _EndRowForValue;
	if (_IndexRow < endRow) {
		for (int r = _IndexRow; r < endRow; ++r) {
			// 0.01 ������ �ȳ����� ��
			if (endValue == 1 || endValue == 0) {
				zeroRow = r;
				break;
			}
			if (endValue <= 1000)
				endValue -= sym->intTickSize;
			else
				endValue -= 5;
		}

		if (zeroRow < endRow) {
			_IndexRow = endRow - (zeroRow - _IndexRow) + 1;
		}
	}

	int startValue = sym->Quote.intClose;
	if (_IndexRow > _StartRowForValue) {
		for (int r = _IndexRow; r > _StartRowForValue; --r) {
			if (startValue < 1000)
				startValue += sym->intTickSize;
			else
				startValue += 5;
		}
	}
	else if (_IndexRow <= _StartRowForValue) {
		for (int r = _IndexRow; r < _StartRowForValue; ++r) {
			if (startValue <= 1000)
				startValue -= sym->intTickSize;
			else
				startValue -= 5;
		}
	}
	ValueToRowMap.clear();
	RowToValueMap.clear();
	int curVal = startValue;
	for (int i = _StartRowForValue; i <= _EndRowForValue; ++i) {
		CGridCellBase* pCell = GetCell(i, CenterCol);
		ValueToRowMap[curVal] = i;
		RowToValueMap[i] = curVal;

		std::string strVal = fmt::format("{:.{}f}", curVal / std::pow(10, sym->Decimal), sym->Decimal);

		pCell->SetText(strVal.c_str());
		pCell->SetLongValue(curVal);
		pCell->SetTextClr(RGB(0, 0, 0));
		// �ؽ�Ʈ ����
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		if (curVal > 1000)
			curVal = curVal - 5;
		else
			curVal = curVal - sym->intTickSize;
		//RedrawCell(i, CenterCol);
		refreshSet.insert(std::make_pair(i, CenterCol));
	}
	*/
}

void SmOrderGridOut::SetCenterValueByFixedForFuture(const symbol_p sym, std::set<std::pair<int, int>>& refreshSet)
{
	if (!sym)
		return;

	_IndexRow = FindIndexRow();
	/*
	// Set the close value on the center of the center grid
	int startValue = sym->Quote.intClose + (sym->intTickSize * (_IndexRow - _StartRowForValue));

	int lowRow = FindRowFromCenterValue(sym, sym->Quote.intLow);
	int highRow = FindRowFromCenterValue(sym, sym->Quote.intHigh);
	int rangeStart = max(highRow - 1, _StartRowForValue);
	int rangeEnd = min(lowRow + 1, _EndRowForValue);
	ValueToRowMap.clear();
	RowToValueMap.clear();
	for (int i = _StartRowForValue; i <= _EndRowForValue; i++) {
		int curValue = startValue - sym->intTickSize * (i - 1);
		CGridCellBase* pCell = GetCell(i, CenterCol);

		std::string strVal = fmt::format("{:.{}f}", curValue / std::pow(10, sym->Decimal), sym->Decimal);

		pCell->SetText(strVal.c_str());
		pCell->SetLongValue(curValue);
		pCell->SetTextClr(RGB(0, 0, 0));
		// �ؽ�Ʈ ����
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//RedrawCell(i, CenterCol);
		refreshSet.insert(std::make_pair(i, CenterCol));
		ValueToRowMap[curValue] = i;
		RowToValueMap[i] = curValue;
	}
	*/
}

void SmOrderGridOut::SetCenterValueByFixedForOption(const symbol_p sym, std::set<std::pair<int, int>>& refreshSet)
{
	if (!sym)
		return;

	_IndexRow = FindIndexRow();
	_IndexRow = -1 * _IndexRow;
	/*
	// ȣ���� �ٲ�� �� - �ɼ��� 10.0���� ȣ���� 0.05�� �� ���ϴ� 0.01�� ����� �ȴ�.
	int Row10 = 0;
	if (1000 <= sym->Quote.intClose) {
		int delta = sym->Quote.intClose - 1000;
		int deltaRow = delta / 5;
		Row10 = _IndexRow - deltaRow;
	}
	else {
		int delta = 1000 - sym->Quote.intClose;
		int deltaRow = delta / sym->intTickSize;
		Row10 = _IndexRow + deltaRow;
	}

	int startValue = 0;
	if (_StartRowForValue >= Row10) {
		startValue = (_StartRowForValue - Row10) * 5 + 1000;
	}
	else {
		startValue = 1000 - (Row10 - _StartRowForValue) * sym->intTickSize;
	}

	ValueToRowMap.clear();
	RowToValueMap.clear();
	int curVal = startValue;
	for (int i = _StartRowForValue; i <= _EndRowForValue; ++i) {
		ValueToRowMap[curVal] = i;
		RowToValueMap[i] = curVal;
		CGridCellBase* pCell = GetCell(i, CenterCol);

		std::string strVal = fmt::format("{:.{}f}", curVal / std::pow(10, sym->Decimal), sym->Decimal);

		pCell->SetText(strVal.c_str());
		pCell->SetLongValue(curVal);
		pCell->SetTextClr(RGB(0, 0, 0));
		// �ؽ�Ʈ ����
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//RedrawCell(i, CenterCol);
		refreshSet.insert(std::make_pair(i, CenterCol));
		if (curVal > 1000)
			curVal = curVal - 5;
		else
			curVal = curVal - sym->intTickSize;
	}
	*/
}

int SmOrderGridOut::FindIndexRow()
{
	int count = GetMaxValueRowCount();

	return ((int)(count / 2) + 1);
}

int SmOrderGridOut::GetMaxValueRowCount()
{
	RECT rect;
	GetClientRect(&rect);


	double pureHeight = rect.bottom - rect.top - _HeadHeight;

	int count = (int)(pureHeight / _CellHeight);

	return count;
}


void SmOrderGridOut::SetHogaInfo(const symbol_p sym, std::set<std::pair<int, int>>& refreshSet)
{
	if (!sym)
		return;
	_HogaPos.clear();
	/*
	for (int i = 0; i < 5; i++) {
		int pos = FindRowFromCenterValue(sym->Hoga.Ary[i].IntBuyPrice);
		if (pos >= _StartRowForValue && pos <= _EndRowForValue) {
			CGridCellBase* pCell = GetCell(pos, CenterCol + 2);
			std::string strVal = fmt::format("{}", sym->Hoga.Ary[i].BuyNo);
			pCell->SetText(strVal.c_str());
			pCell->SetBackClr(BuyColor[i]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(pos, CenterCol + 2));
			refreshSet.insert(std::make_pair(pos, CenterCol + 2));

			pCell = GetCell(pos, CenterCol + 1);
			strVal = fmt::format("{}", sym->Hoga.Ary[i].BuyQty);
			pCell->SetText(strVal.c_str());
			pCell->SetBackClr(BuyColor[i]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(pos, CenterCol + 1));
			refreshSet.insert(std::make_pair(pos, CenterCol + 1));
		}

		pos = FindRowFromCenterValue(sym->Hoga.Ary[i].IntSellPrice);
		if (i == 0) {
			_CloseLineRow = pos;
			for (int k = 0; k < m_nCols; ++k) {
				CGridCellBase* pCell = GetCell(pos, k);
				if (pCell)
					pCell->Style(1);
			}
		}
		if (pos >= _StartRowForValue && pos <= _EndRowForValue) {
			CGridCellBase* pCell = GetCell(pos, CenterCol - 2);
			std::string strVal = fmt::format("{}", sym->Hoga.Ary[i].SellNo);
			pCell->SetText(strVal.c_str());
			pCell->SetBackClr(SellColor[i]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(pos, CenterCol - 2));
			refreshSet.insert(std::make_pair(pos, CenterCol - 2));

			pCell = GetCell(pos, CenterCol - 1);
			strVal = fmt::format("{}", sym->Hoga.Ary[i].SellQty);
			pCell->SetText(strVal.c_str());
			pCell->SetBackClr(SellColor[i]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(pos, CenterCol - 1));
			refreshSet.insert(std::make_pair(pos, CenterCol - 1));
		}
	}

	// ��ȣ�� �Ǽ�, ���� ����
	{
		CGridCellBase* pCell = GetCell(_EndRowForValue + 1, CenterCol - 2);
		std::string strVal = fmt::format("{}", sym->Hoga.TotSellNo);
		if (pCell) {
			pCell->SetText(strVal.c_str());
			//pCell->SetBackClr(SellColor[0]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(_EndRowForValue + 1, CenterCol - 2));
			refreshSet.insert(std::make_pair(_EndRowForValue + 1, CenterCol - 2));
		}

		pCell = GetCell(_EndRowForValue + 1, CenterCol - 1);
		if (pCell) {
			strVal = fmt::format("{}", sym->Hoga.TotSellQty);
			pCell->SetText(strVal.c_str());
			//pCell->SetBackClr(SellColor[0]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(_EndRowForValue + 1, CenterCol - 1));
			refreshSet.insert(std::make_pair(_EndRowForValue + 1, CenterCol - 1));
		}

		pCell = GetCell(_EndRowForValue + 1, CenterCol + 1);
		if (pCell) {
			strVal = fmt::format("{}", sym->Hoga.TotBuyQty);
			pCell->SetText(strVal.c_str());
			//pCell->SetBackClr(BuyColor[0]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 1));
			refreshSet.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 1));
		}

		pCell = GetCell(_EndRowForValue + 1, CenterCol + 2);
		if (pCell) {
			strVal = fmt::format("{}", sym->Hoga.TotBuyNo);
			pCell->SetText(strVal.c_str());
			//pCell->SetBackClr(BuyColor[0]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 2));
			refreshSet.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 2));
		}

		int dif = sym->Hoga.TotBuyQty - sym->Hoga.TotSellQty;

		pCell = GetCell(_EndRowForValue + 1, CenterCol);
		if (pCell) {
			strVal = fmt::format("{}", dif);
			pCell->SetText(strVal.c_str());
			//pCell->SetBackClr(BuyColor[0]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			if (dif > 0)
			{
				pCell->SetTextClr(RGB(255, 0, 0));
			}
			else
			{
				pCell->SetTextClr(RGB(0, 0, 255));
			}
			_HogaPos.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 2));
			refreshSet.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 2));
		}
	}
	*/
}

int SmOrderGridOut::FindRowFromCenterValue(int value)
{
	auto it = ValueToRowMap.find(value);
	if (it != ValueToRowMap.end()) {
		return it->second;
	}
	else {
		return -2;
	}
}

int SmOrderGridOut::FindRowFromCenterValue(const symbol_p sym, int value)
{
	if (!sym || ValueToRowMap.size() == 0)
		return 0;
	/*
	if (sym->intTickSize == 0)
		return 0;

	auto it = ValueToRowMap.find(value);
	if (it != ValueToRowMap.end()) { // ���� ���̴� ���� �ȿ� ���� ��
		return it->second;
	}
	else { // ���� ���̴� ���� �ۿ� ���� ��
		auto itr = ValueToRowMap.rbegin();
		int bigVal = itr->first;
		int bigRow = itr->second;
		int thousandRow = 0;
		// ������ 10�� ���� ã�� ���� - 10�̻��̸� �¼��� ���Ѵ�.
		if (bigVal >= 1000) { // �ֻ��� ���� 10���� �̻��� ���
			int delta = bigVal - 1000;
			int deltaRow = delta / sym->intTickSize;
			thousandRow = bigRow + deltaRow;
		}
		else { // �ֻ��� ���� 10�̸��� ���
			int delta = 1000 - bigVal;
			thousandRow = bigRow - delta;
		}

		if (value >= 1000) { // ������ 10 �̻��� �ִ� ��� - ������ ƽũ�� ��ŭ ����
			int delta = value - 1000;
			int deltaRow = delta / sym->intTickSize;
			return thousandRow - deltaRow;
		}
		else { // ������ 10 �̸��� ��� - ���� ������� 1�� ����
			int delta = 1000 - value;
			return thousandRow + delta;
		}
	}
	*/
	return 0;
}

void SmOrderGridOut::SetQuoteColor(const symbol_p sym, std::set<std::pair<int, int>>& refreshSet)
{
	/*
	if (!sym)
		return;

	if (ValueToRowMap.size() == 0)
		return;

	CUGCell cell;

	int lowRow = FindRowFromCenterValue(sym, sym->Quote.intLow);
	int highRow = FindRowFromCenterValue(sym, sym->Quote.intHigh);
	int closeRow = FindRowFromCenterValue(sym, sym->Quote.intClose);
	int openRow = FindRowFromCenterValue(sym, sym->Quote.intOpen);
	int preCloseRow = FindRowFromCenterValue(sym, sym->Quote.intPreClose);

	if (sym->Quote.intClose > sym->Quote.intOpen) { // ���
		for (auto it = ValueToRowMap.rbegin(); it != ValueToRowMap.rend(); ++it) {
			if (it->second < highRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
			else if (it->second < closeRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else if (it->second <= openRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(252, 226, 228));
			}
			else if (it->second < lowRow + 1) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
		}

	}
	else if (sym->Quote.intClose < sym->Quote.intOpen) { // ����
		for (auto it = ValueToRowMap.rbegin(); it != ValueToRowMap.rend(); ++it) {
			if (it->second < highRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
			else if (it->second < openRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else if (it->second <= closeRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(218, 226, 245));
			}
			else if (it->second < lowRow + 1) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
		}
	}
	else { // ����
		for (auto it = ValueToRowMap.rbegin(); it != ValueToRowMap.rend(); ++it) {
			if (it->second < highRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
			else if (it->second < closeRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else if (it->second <= openRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(252, 226, 228));
			}
			else if (it->second < lowRow + 1) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
		}
	}

	// ���� ���� ������
	CGridCellBase* pCell = GetCell(closeRow, CenterCol);
	if (pCell && closeRow >= _StartRowForValue && closeRow <= _EndRowForValue) {
		pCell->SetBackClr(RGB(255, 255, 0));
		_QuotePos.insert(std::make_pair(closeRow, CenterCol));
	}
	// ���� �ؽ�Ʈ ���� ����
	SetSiseLabel(highRow, "H", RGB(255, 0, 0));

	// ���� �ؽ�Ʈ ���� ����
	SetSiseLabel(lowRow, "L", RGB(0, 0, 255));

	// ���۰� ���̺� ����
	SetSiseLabel(openRow, "O", RGB(0, 0, 0));

	// ���� ���� ���̺� ����
	SetSiseLabel(preCloseRow, "C", RGB(0, 0, 0));

	if (_CenterWnd->FixedCenter()) {
		for (auto it = ValueToRowMap.begin(); it != ValueToRowMap.end(); ++it) {
			refreshSet.insert(std::make_pair(it->second, CenterCol));
		}
	}
	else {
		int min_row = min(closeRow, _OldIndexRow);
		int max_row = max(closeRow, _OldIndexRow);
		if (min_row < _StartRowForValue)
			min_row = _StartRowForValue;
		if (max_row > _EndRowForValue)
			max_row = _EndRowForValue;
		_OldIndexRow = closeRow;
		// ǥ�� ������ ����� ���� �������� �ʴ´�.
		if (max_row < _StartRowForValue || min_row > _EndRowForValue)
			return;
		// ǥ�� ���� �ȿ� �ִ� �͸� ������Ʈ �Ѵ�.
		for (int i = min_row; i <= max_row; ++i) {
			refreshSet.insert(std::make_pair(i, CenterCol));
		}
	}
	*/
}

void SmOrderGridOut::SetSiseCellBackColor(int minRow, int maxRow, int start, int end, COLORREF color)
{
	if (end < minRow || start > maxRow) {
		return;
	}

	start = max(minRow, start);
	end = min(maxRow, end);

	for (int i = start; i < end; ++i) {
		CGridCellBase* pCell = GetCell(i, CenterCol);
		pCell->SetBackClr(color);
	}
}

void SmOrderGridOut::SetSiseLabel(int row, LPCTSTR text, COLORREF textColor)
{
	if (row < _StartRowForValue || row > _EndRowForValue)
		return;
	CGridCellBase* pCell = GetCell(row, CenterCol);
	if (pCell) {
		pCell->SetLabel(text);
		pCell->SetTextClr(textColor);
		_QuotePos.insert(std::make_pair(row, CenterCol));
	}
}

void SmOrderGridOut::ClearHogas(std::set<std::pair<int, int>>& refreshSet)
{
	// ȣ�� ǥ�õ� ���� ���� �ʱ�ȭ ��Ų��.
	for (auto it = _HogaPos.begin(); it != _HogaPos.end(); ++it) {
		std::pair<int, int> pos = *it;
		CCellID cell(pos.first, pos.second);
		if (cell.IsValid() == TRUE) {
			CGridCellBase* pCell = GetCell(cell.row, cell.col);
			if (pCell) {
				pCell->SetText("");
				pCell->SetBackClr(RGB(255, 255, 255));
				refreshSet.insert(std::make_pair(pos.first, pos.second));
			}
		}
	}

	// ȣ�� ������ �ٽ� �������� �ǵ��� ���´�.
	for (int k = 0; k < m_nCols; ++k) {
		CCellID cell(_CloseLineRow, k);
		if (cell.IsValid() == TRUE) {
			CGridCellBase* pCell = GetCell(_CloseLineRow, k);
			if (pCell)
				pCell->Style(0);
		}
	}
}

void SmOrderGridOut::SetOrderInfo(std::set<std::pair<int, int>>& refreshSet)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	/*
	symbol_p sym = _CenterWnd->Symbol();

	_OrderPos.clear();

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(sym->ShortCode);

	int buy_count = 0, sell_count = 0;
	CGridCellBase* pCell = nullptr;
	std::string strVal;
	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		if (order->amount == 0)
			continue;
		order->orderPosition == VtPositionType::Buy ? buy_count += order->amount : sell_count += order->amount;
		int row = FindRowFromCenterValue(sym, order->intOrderPrice);
		if (row >= _StartRowForValue && row <= _EndRowForValue) {
			if (order->orderPosition == VtPositionType::Sell) {
				pCell = GetCell(row, CenterCol - 3);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->AddOrder(order);
				_OrderPos.insert(std::make_pair(row, CenterCol - 3));
				refreshSet.insert(std::make_pair(row, CenterCol - 3));
			}
			else if (order->orderPosition == VtPositionType::Buy) {
				pCell = GetCell(row, CenterCol + 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->AddOrder(order);
				_OrderPos.insert(std::make_pair(row, CenterCol + 3));
				refreshSet.insert(std::make_pair(row, CenterCol + 3));
			}
			// �ֹ� ���� ����
			strVal = fmt::format("{}", pCell->GetOrderCount());
			pCell->SetText(strVal.c_str());
		}
	}

	// ��ü �ֹ� ���� ����
	pCell = GetCell(1, CenterCol + 3);
	if (pCell) {
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		_OrderPos.insert(std::make_pair(1, CenterCol + 3));
		refreshSet.insert(std::make_pair(1, CenterCol + 3));
		strVal = fmt::format("{}", buy_count);
		pCell->SetText(strVal.c_str());
	}

	pCell = GetCell(1, CenterCol - 3);
	if (pCell) {
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		_OrderPos.insert(std::make_pair(1, CenterCol - 3));
		refreshSet.insert(std::make_pair(1, CenterCol - 3));
		strVal = fmt::format("{}", sell_count);
		pCell->SetText(strVal.c_str());
	}
	*/
}

void SmOrderGridOut::ClearOldOrders(std::set<std::pair<int, int>>& refreshSet)
{
	for (auto it = _OrderPos.begin(); it != _OrderPos.end(); ++it) {
		std::pair<int, int> pos = *it;
		CGridCellBase* pCell = GetCell(pos.first, pos.second);
		if (pCell) {
			pCell->SetText("");
			pCell->ClearOrders();
			refreshSet.insert(std::make_pair(pos.first, pos.second));
		}
	}
}

void SmOrderGridOut::SetStopOrderInfo(std::set<std::pair<int, int>>& refreshSet)
{
	_StopOrderPos.clear();
	CGridCellBase* pCell = nullptr;
	std::string strVal;
	int buy_count = 0, sell_count = 0;
	/*
	// �Ϲ� ��ž�ֹ��� ǥ���� �ش�.
	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(); it != _StopOrderMgr->StopOrderMapHd.end(); ++it) {
		std::shared_ptr<HdOrderRequest> order = it->second;
		order->Position == VtPositionType::Buy ? buy_count += order->Amount : sell_count += order->Amount;
		int row = FindRowFromCenterValue(order->Price);
		if (row >= _StartRowForValue && row <= _EndRowForValue) {
			if (order->Position == VtPositionType::Sell) {
				pCell = GetCell(row, CenterCol - 4);
				pCell->AddStopOrder(order);
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				_StopOrderPos.insert(std::make_pair(row, CenterCol - 4));
				refreshSet.insert(std::make_pair(row, CenterCol - 4));
			}
			else if (order->Position == VtPositionType::Buy) {
				pCell = GetCell(row, CenterCol + 4);
				pCell->AddStopOrder(order);
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				_StopOrderPos.insert(std::make_pair(row, CenterCol + 4));
				refreshSet.insert(std::make_pair(row, CenterCol + 4));
			}
			// ��ž �ֹ� ���� ����
			strVal = fmt::format("{}({})", pCell->GetStopOrderCount(), _StopOrderMgr->Slip());
			pCell->SetText(strVal.c_str());
		}
	}

	// �ڵ� ������ ���� ��ž�ֹ��� ���� �߰��� �ش�.
	std::map<int, std::shared_ptr<HdOrderRequest>>& profitLossMap = _CutMgr->GetStopOrderMap();
	for (auto it = profitLossMap.begin(); it != profitLossMap.end(); ++it) {
		std::shared_ptr<HdOrderRequest> order = it->second;
		order->Position == VtPositionType::Buy ? buy_count += order->Amount : sell_count += order->Amount;
		int row = FindRowFromCenterValue(order->Price);
		if (row >= _StartRowForValue && row <= _EndRowForValue) {
			if (order->Position == VtPositionType::Sell) {
				pCell = GetCell(row, CenterCol - 4);
				pCell->AddStopOrder(order);
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				_StopOrderPos.insert(std::make_pair(row, CenterCol - 4));
				refreshSet.insert(std::make_pair(row, CenterCol - 4));
			}
			else if (order->Position == VtPositionType::Buy) {
				pCell = GetCell(row, CenterCol + 4);
				pCell->AddStopOrder(order);
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				_StopOrderPos.insert(std::make_pair(row, CenterCol + 4));
				refreshSet.insert(std::make_pair(row, CenterCol + 4));
			}
			// ��ž �ֹ� ���� ����
			strVal = fmt::format("{}({})", pCell->GetStopOrderCount(), _StopOrderMgr->Slip());
			pCell->SetText(strVal.c_str());
		}
	}

	// ��ü �ֹ� ���� ����
	pCell = GetCell(1, CenterCol + 4);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	_StopOrderPos.insert(std::make_pair(1, CenterCol + 4));
	refreshSet.insert(std::make_pair(1, CenterCol + 4));
	strVal = fmt::format("{}", buy_count);
	pCell->SetText(strVal.c_str());

	pCell = GetCell(1, CenterCol - 4);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	_StopOrderPos.insert(std::make_pair(1, CenterCol - 4));
	refreshSet.insert(std::make_pair(1, CenterCol - 4));
	strVal = fmt::format("{}", sell_count);
	pCell->SetText(strVal.c_str());
	*/
}

void SmOrderGridOut::ClearOldStopOrders(std::set<std::pair<int, int>>& refreshSet)
{
	for (auto it = _StopOrderPos.begin(); it != _StopOrderPos.end(); ++it) {
		std::pair<int, int> pos = *it;
		CGridCellBase* pCell = GetCell(pos.first, pos.second);
		pCell->SetText("");
		pCell->ClearStopOrders();
		refreshSet.insert(std::make_pair(pos.first, pos.second));
	}
}

void SmOrderGridOut::CalcPosStopOrders(std::set<std::pair<int, int>>& refreshSet)
{
	ClearStopOrderVectors();
	/*
	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(); it != _StopOrderMgr->StopOrderMapHd.end(); ++it) {
		std::shared_ptr<HdOrderRequest> order = it->second;
		int order_row = FindRowFromCenterValue(order->Price);
		CCellID order_cell, slip_cell;
		order_cell.row = order_row;
		if (order->Position == VtPositionType::Buy) {
			order_cell.col = CenterCol + 4;
			slip_cell.col = CenterCol + 3;
			order->slip > 0 ? slip_cell.row = order_row - order->slip : slip_cell.row = order_row + order->slip;
		}
		else if (order->Position == VtPositionType::Sell) {
			order_cell.col = CenterCol - 4;
			slip_cell.col = CenterCol - 3;
			order->slip > 0 ? slip_cell.row = order_row + order->slip : slip_cell.row = order_row - order->slip;
		}

		if (order_cell.row < _StartRowForValue || order_cell.row > _EndRowForValue ||
			slip_cell.row  < _StartRowForValue || slip_cell.row > _EndRowForValue)
			continue;

		PutStopOrderVector(std::make_pair(order_cell, slip_cell));

		int min_row = min(order_cell.row, slip_cell.row);
		int max_row = max(order_cell.row, slip_cell.row);
		int min_col = min(order_cell.col, slip_cell.col);
		int max_col = max(order_cell.col, slip_cell.col);
		for (int r = min_row; r <= max_row; ++r) {
			for (int c = min_col; c <= max_col; ++c) {
				refreshSet.insert(std::make_pair(r, c));
			}
		}
	}

	std::map<int, std::shared_ptr<HdOrderRequest>>& profitLossMap = _CutMgr->GetStopOrderMap();
	for (auto it = profitLossMap.begin(); it != profitLossMap.end(); ++it) {
		std::shared_ptr<HdOrderRequest> order = it->second;
		int order_row = FindRowFromCenterValue(order->Price);
		CCellID order_cell, slip_cell;
		order_cell.row = order_row;
		if (order->Position == VtPositionType::Buy) {
			order_cell.col = CenterCol + 4;
			slip_cell.col = CenterCol + 3;
			order->slip > 0 ? slip_cell.row = order_row - order->slip : slip_cell.row = order_row + order->slip;
		}
		else if (order->Position == VtPositionType::Sell) {
			order_cell.col = CenterCol - 4;
			slip_cell.col = CenterCol - 3;
			order->slip > 0 ? slip_cell.row = order_row + order->slip : slip_cell.row = order_row - order->slip;
		}

		if (order_cell.row < _StartRowForValue || order_cell.row > _EndRowForValue ||
			slip_cell.row  < _StartRowForValue || slip_cell.row > _EndRowForValue)
			continue;

		PutStopOrderVector(std::make_pair(order_cell, slip_cell));

		int min_row = min(order_cell.row, slip_cell.row);
		int max_row = max(order_cell.row, slip_cell.row);
		int min_col = min(order_cell.col, slip_cell.col);
		int max_col = max(order_cell.col, slip_cell.col);
		for (int r = min_row; r <= max_row; ++r) {
			for (int c = min_col; c <= max_col; ++c) {
				refreshSet.insert(std::make_pair(r, c));
			}
		}
	}
	*/
	Invalidate();
}

void SmOrderGridOut::RefreshCells(std::set<std::pair<int, int>>& refreshSet)
{
	for (auto it = refreshSet.begin(); it != refreshSet.end(); ++it) {
		std::pair<int, int> pos = *it;
		CGridCellBase* pCell = GetCell(pos.first, pos.second);
		InvalidateCellRect(pos.first, pos.second);
	}
}

void SmOrderGridOut::PutOrder(int price, VtPositionType position, VtPriceType priceType /*= VtPriceType::Price*/)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;
	/*
	if (_OrderConfigMgr->Type() == 0) { // �ǰ��³� ���� ���� �� ��
		if (_OrderConfigMgr->Account()) {
			VtAccount* acnt = _OrderConfigMgr->Account();

			std::shared_ptr<HdOrderRequest> request = std::make_shared<HdOrderRequest>();
			request->Market = 1; // �ؿ� ���� ���� ����
			request->Price = price;
			request->Position = position;
			request->Amount = _CenterWnd->OrderAmount();
			if (acnt->AccountLevel() == 0) { // �ǰ��� �� ��
				request->AccountNo = acnt->AccountNo;
				request->Password = acnt->Password;
				request->Type = 0;
			}
			else { // ���� ���� �� ��
				VtAccount* parentAcnt = acnt->ParentAccount();
				if (parentAcnt) { // �������� ������ �־� �ش�.
					request->AccountNo = parentAcnt->AccountNo;
					request->Password = parentAcnt->Password;
				}
				request->Type = 1;
			}
			request->SymbolCode = _CenterWnd->Symbol()->ShortCode;
			request->FillCondition = _CenterWnd->FillCondition();
			request->PriceType = priceType;

			request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
			request->SourceId = (long)this;
			if (acnt->AccountLevel() == 0) { // �ǰ����� ��
				request->SubAccountNo = _T("");
				request->FundName = _T("");
			}
			else { // ���� ���� �� ��
				request->SubAccountNo = acnt->AccountNo;
				request->FundName = _T("");
			}

			request->AccountLevel = _OrderConfigMgr->Account()->AccountLevel();
			request->orderType = VtOrderType::New;
			_OrderConfigMgr->OrderMgr()->PutOrder(request);
		}
	}
	else { // �ݵ� �ֹ� �� ��
		if (_CenterWnd->OrderByRemain()) { // ü��� �ܰ��� �����Ǿ� ���� ���� �� û�� ��ƾ�� ź��.
			LiqudAll(VtPriceType::Price, price);
			_CenterWnd->ResetRemainFund();
			return;
		}

		if (_OrderConfigMgr->Fund()) {
			const std::vector<VtAccount*>& fundAcntList = _OrderConfigMgr->Fund()->GetFundAccountVector();
			for (auto it = fundAcntList.begin(); it != fundAcntList.end(); ++it) { // �ݵ� ���¸� ���鼭 �ֹ��� ���ش�.
				VtAccount* subAcnt = *it;
				VtAccount* parentAcnt = subAcnt->ParentAccount();
				VtOrderManager* orderMgr = mainApp.TotalOrderMgr().FindAddAccountOrderManger(subAcnt->AccountNo);

				std::shared_ptr<HdOrderRequest> request = std::make_shared<HdOrderRequest>();
				request->Market = 1; // �ؿ� ���� ���� ����
				request->Type = 2;
				request->Price = price;
				request->Position = position;
				request->Amount = _CenterWnd->OrderAmount() * subAcnt->SeungSu;
				if (parentAcnt) { // �θ� ������ ������ �־� �ش�.
					request->AccountNo = parentAcnt->AccountNo;
					request->Password = parentAcnt->Password;
				}
				else
					continue;
				request->SymbolCode = _CenterWnd->Symbol()->ShortCode;
				request->FillCondition = _CenterWnd->FillCondition();
				request->PriceType = priceType;

				request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
				request->SourceId = (long)this;
				request->SubAccountNo = subAcnt->AccountNo;
				request->FundName = _OrderConfigMgr->Fund()->Name;
				request->AccountLevel = _OrderConfigMgr->Account()->AccountLevel();
				request->orderType = VtOrderType::New;
				orderMgr->PutOrder(std::move(request));
			}
		}
	}
	*/
}

void SmOrderGridOut::PutOrder(VtPosition* posi, int price, bool liqud /*= false*/)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !posi || !_OrderConfigMgr)
		return;
	/*
	if (posi->OpenQty == 0)
		return;

	symbol_p sym = _CenterWnd->Symbol();
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

	request->SymbolCode = _CenterWnd->Symbol()->ShortCode;
	request->FillCondition = _CenterWnd->FillCondition();

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

void SmOrderGridOut::RefreshOrderPosition()
{
	std::set<std::pair<int, int>> refreshSet;
	ClearOldOrders(refreshSet);
	SetOrderInfo(refreshSet);
	RefreshCells(refreshSet);

	//LOG_F(INFO, _T("�ֹ��׸��� ����"));
}

void SmOrderGridOut::SetOrderArea(int height, int width)
{
	_CellHeight = height;
	_GridColMap[SmOrderGridCol::ORDER] = width;
}

void SmOrderGridOut::AddOrderToCell(VtOrder* order)
{
	if (!order)
		return;
	/*
	if (order->amount == 0)
		return;
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	symbol_p sym = _CenterWnd->Symbol();
	CGridCellBase* pCell = nullptr;
	int row = FindRowFromCenterValue(sym, order->intOrderPrice);
	if (row >= _StartRowForValue && row <= _EndRowForValue) {
		if (order->orderPosition == VtPositionType::Sell) {
			pCell = GetCell(row, CenterCol - 3);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->AddOrder(order);
			_OrderPos.insert(std::make_pair(row, CenterCol - 3));
			// �ֹ� ���� ����
			std::string strVal = fmt::format("{}", pCell->GetOrderCount());
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(row, CenterCol - 3);
		}
		else if (order->orderPosition == VtPositionType::Buy) {
			pCell = GetCell(row, CenterCol + 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->AddOrder(order);
			_OrderPos.insert(std::make_pair(row, CenterCol + 3));
			// �ֹ� ���� ����
			std::string strVal = fmt::format("{}", pCell->GetOrderCount());
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(row, CenterCol - 3);
		}
	}
	*/
}

void SmOrderGridOut::RemoveOrderFromCell(VtOrder* order)
{
	if (!order)
		return;
	/*
	if (order->amount == 0)
		return;
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	symbol_p sym = _CenterWnd->Symbol();
	CGridCellBase* pCell = nullptr;
	int row = FindRowFromCenterValue(sym, order->intOrderPrice);
	if (row >= _StartRowForValue && row <= _EndRowForValue) {
		if (order->orderPosition == VtPositionType::Sell) {
			pCell = GetCell(row, CenterCol - 3);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->RemoveOrder(order->orderNo);
			_OrderPos.insert(std::make_pair(row, CenterCol - 3));
			// �ֹ� ���� ����
			std::string strVal = fmt::format("{}", pCell->GetOrderCount());
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(row, CenterCol - 3);
		}
		else if (order->orderPosition == VtPositionType::Buy) {
			pCell = GetCell(row, CenterCol + 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->RemoveOrder(order->orderNo);
			_OrderPos.insert(std::make_pair(row, CenterCol + 3));
			// �ֹ� ���� ����
			std::string strVal = fmt::format("{}", pCell->GetOrderCount());
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(row, CenterCol + 3);
		}
	}
	*/
}

void SmOrderGridOut::RefreshTotalOrderCount(VtOrder* input_order)
{
	if (!input_order)
		return;

	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;

	/*
	symbol_p sym = _CenterWnd->Symbol();

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(sym->ShortCode);

	int buy_count = 0, sell_count = 0;
	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		order->orderPosition == VtPositionType::Buy ? buy_count += order->amount : sell_count += order->amount;
	}

	CGridCellBase* pCell = nullptr;
	std::string strVal;
	if (input_order->orderPosition == VtPositionType::Buy) {
		// ��ü �ֹ� ���� ����
		pCell = GetCell(1, CenterCol + 3);
		if (pCell) {
			_OrderPos.insert(std::make_pair(1, CenterCol + 3));
			strVal = fmt::format("{}", buy_count);
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(1, CenterCol + 3);
		}
	}

	if (input_order->orderPosition == VtPositionType::Sell) {
		pCell = GetCell(1, CenterCol - 3);
		if (pCell) {
			_OrderPos.insert(std::make_pair(1, CenterCol - 3));
			strVal = fmt::format("{}", sell_count);
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(1, CenterCol - 3);
		}
	}
	*/
}

void SmOrderGridOut::SetOrderAreaColor()
{
	CGridCellBase* pCell = nullptr;
	for (int i = 1; i <= _EndRowForValue + 1; ++i) {
		pCell = GetCell(i, CenterCol - 3);
		pCell->SetBackClr(RGB(218, 226, 245));
		pCell = GetCell(i, CenterCol + 3);
		pCell->SetBackClr(RGB(252, 226, 228));
	}

	pCell = GetCell(1, CenterCol - 4);
	pCell->SetBackClr(RGB(218, 226, 245));
	pCell = GetCell(1, CenterCol + 4);
	pCell->SetBackClr(RGB(252, 226, 228));
}

void SmOrderGridOut::SetPositionInfo(std::set<std::pair<int, int>>& refreshSet)
{
	if (!_OrderConfigMgr)
		return;
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	/*
	if (_OrderConfigMgr->Type() == 0)
	{
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;
		VtPosition* posi = acnt->FindPosition(_CenterWnd->Symbol()->ShortCode);
		ShowPosition(refreshSet, posi, _CenterWnd->Symbol());
	}
	else
	{
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		int count = 0;
		VtPosition posi = fund->GetPosition(_CenterWnd->Symbol()->ShortCode, count);
		if (count == 0)
			ShowPosition(refreshSet, nullptr, _CenterWnd->Symbol());
		else
			ShowPosition(refreshSet, &posi, _CenterWnd->Symbol());
	}
	*/
}

void SmOrderGridOut::ClearPositionInfo(std::set<std::pair<int, int>>& refreshSet)
{
	if (_OldPositionCell.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(_OldPositionCell.row, _OldPositionCell.col);
		pCell->SetPosition(0);
		refreshSet.insert(std::make_pair(_OldPositionCell.row, _OldPositionCell.col));
		_OldPositionCell.row = -1;
		_OldPositionCell.col = -1;
	}
}

void SmOrderGridOut::ShowPosition(std::set<std::pair<int, int>>& refreshSet, VtPosition* posi, symbol_p sym)
{
	if (!sym)
		return;
	/*
	if (!posi || posi->OpenQty == 0 || posi->Position == VtPositionType::None) {
		return;
	}

	int position_row = FindPositionRow(posi);
	CCellID cell;
	cell.row = position_row;
	cell.col = CenterCol;
	if (cell.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(cell.row, cell.col);
		posi->Position == VtPositionType::Buy ? pCell->SetPosition(1) : pCell->SetPosition(2);
		refreshSet.insert(std::make_pair(cell.row, cell.col));
		_OldPositionCell = cell;
	}
	*/
}

void SmOrderGridOut::OrderBySpaceBar()
{
	if (_MouseIn) {
		if (_OrderConfigMgr->UseSpaceBarOrder) {
			if (_OrderConfigMgr->SpaceBarorderType == VtSpaceByOrderType::ByMouseClick) {
				OrderByMouseClick();
			}
			else if (_OrderConfigMgr->SpaceBarorderType == VtSpaceByOrderType::ByMousePositon) {
				OrderByMousePosition();
			}
		}
		else {
			int newCenter = FindIndexRow();
			//SetCenterRow(newCenter);
			//ChangeCenter(_CloseRow, newCenter);
		}
	}
}

// ���������� ������ �ʴ´�.
// �� ������ �þ��.
void SmOrderGridOut::ResizeGrid()
{
	UnregisterButtons();
	_RowCount = GetMaxValueRowCount() + 1;
	_EndRowForValue = _RowCount - 3;
	SetRowCount(_RowCount);
	for (int i = 1; i < m_nRows; ++i) {
		SetRowHeight(i, _CellHeight);
	}
	SetOrderAreaColor();
	SetCenterValue();
	RegisterButtons();
}

void SmOrderGridOut::ResizeGrid(int cellHeight, int orderAreaWidth)
{
	_CellHeight = cellHeight;
	_OrderWidth = orderAreaWidth;
	UnregisterButtons();
	_RowCount = GetMaxValueRowCount() + 1;
	_EndRowForValue = _RowCount - 3;
	SetRowCount(_RowCount);
	for (int i = 1; i < m_nRows; ++i) {
		SetRowHeight(i, _CellHeight);
	}
	_IndexRow = FindIndexRow();
	SetOrderAreaColor();
	SetCenterValue();
	RegisterButtons();
}

void SmOrderGridOut::LiqudAll(VtPriceType priceType, int price /*= 0*/)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	/*
	symbol_p sym = _CenterWnd->Symbol();
	if (_OrderConfigMgr->Type() == 0) { // ���� �ֹ��� ��
		VtAccount* acnt = _OrderConfigMgr->Account();
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);

		if (!posi || posi->OpenQty == 0)
			return;
		if (priceType == VtPriceType::Market) // ���尡
			PutOrder(posi, 0, true);
		else // ������
			PutOrder(posi, price, false);
	}
	else {
		if (_OrderConfigMgr->Fund()) { // �ݵ� �ֹ��� ��
			const std::vector<VtAccount*>& fundAcntList = _OrderConfigMgr->Fund()->GetFundAccountVector();
			for (auto it = fundAcntList.begin(); it != fundAcntList.end(); ++it) { // ���� ���� ����� ���鼭 �� ��������� �ܰ��� û���� �ش�.
				VtAccount* subAcnt = *it;
				VtPosition* posi = subAcnt->FindPosition(sym->ShortCode);

				if (!posi || posi->OpenQty == 0)
					continue;
				if (priceType == VtPriceType::Market) // ���尡
					PutOrder(posi, 0, true);
				else // ������
					PutOrder(posi, price, false);
			}
		}
	}
	try {
	// ��� ��ž �ֹ� ���
	_StopOrderMgr->RemoveAll();
	// ��� ����,���� ��ž ���
	_CutMgr->RemoveAllHd();
	// ��ž �ֹ� ��� ����
	std::set<std::pair<int, int>> refreshSet;
	ClearOldStopOrders(refreshSet);
	SetStopOrderInfo(refreshSet);
	CalcPosStopOrders(refreshSet);
	RefreshCells(refreshSet);

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s �˼����� ����"), __FUNCTION__);
	}
	*/
}

void SmOrderGridOut::OrderByMouseClick()
{
	if (_OldClickedCell.IsValid() == FALSE)
		return;
	auto it = RowToValueMap.find(_OldClickedCell.row);
	int price = it->second;

	if (_OldClickedCell.col == CenterCol - 3) {
		PutOrder(price, VtPositionType::Sell, VtPriceType::Price);
	}
	else if (_OldClickedCell.col == CenterCol + 3) {
		PutOrder(price, VtPositionType::Buy, VtPriceType::Price);
	}
	else if (_OldClickedCell.col == CenterCol - 4) {
		AddStopOrder(price, VtPositionType::Sell);
	}
	else if (_OldClickedCell.col == CenterCol + 4) {
		AddStopOrder(price, VtPositionType::Buy);
	}
}

void SmOrderGridOut::OrderByMousePosition()
{
	if (_OldMMCell.IsValid() == FALSE)
		return;
	auto it = RowToValueMap.find(_OldMMCell.row);
	int price = it->second;

	if (_OldMMCell.col == CenterCol - 3)
	{
		PutOrder(price, VtPositionType::Sell, VtPriceType::Price);
	}
	else if (_OldMMCell.col == CenterCol + 3)
	{
		PutOrder(price, VtPositionType::Buy, VtPriceType::Price);
	}
	else if (_OldMMCell.col == CenterCol - 4)
	{
		AddStopOrder(price, VtPositionType::Sell);
	}
	else if (_OldMMCell.col == CenterCol + 4)
	{
		AddStopOrder(price, VtPositionType::Buy);
	}
}

void SmOrderGridOut::RefreshAllValues()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	std::set<std::pair<int, int>> refreshSet;
	ClearQuotes(refreshSet);
	SetQuoteColor(_CenterWnd->Symbol(), refreshSet);

	ClearHogas(refreshSet);
	SetHogaInfo(_CenterWnd->Symbol(), refreshSet);

	ClearOldOrders(refreshSet);
	SetOrderInfo(refreshSet);

	ClearOldStopOrders(refreshSet);
	SetStopOrderInfo(refreshSet);
	CalcPosStopOrders(refreshSet);

	RefreshCells(refreshSet);
}

BEGIN_MESSAGE_MAP(SmOrderGridOut, CGridCtrl)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_QUOTE_CHANGED, OnQuoteChangedMessage)
	ON_MESSAGE(WM_HOGA_CHANGED, OnHogaChangedMessage)
	ON_MESSAGE(WM_ORDER_CHANGED, OnOrderChangedMessage)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL SmOrderGridOut::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	int distance = zDelta / 60;
	if (abs(zDelta) > 120)
		distance = zDelta / 120;
	else
		distance = zDelta / 40;
	_IndexRow = _IndexRow + (distance);
	InvalidateClickedCell();
	SetCenterValue();
	return CGridCtrl::OnMouseWheel(nFlags, zDelta, pt);
}


void SmOrderGridOut::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CCellID cell = GetCellFromPt(point);
	if (!IsValid(cell))
	{
		//ASSERT(FALSE);
		return;
	}

	//CGridCtrl::OnLButtonDblClk(nFlags, point);

	CGridCellBase* pCell = GetCell(cell.row, cell.col);
	if (!pCell)
		return;

	//SendMessageToParent(cell.row, cell.col, NM_DBLCLK);

	if (cell.row < _StartRowForValue || cell.row >= _EndRowForValue)
		return;
	auto it = RowToValueMap.find(cell.row);
	int price = it->second;

	if (cell.col == CenterCol - 3) // For Sell
	{
		PutOrder(price, VtPositionType::Sell, VtPriceType::Price);
	}
	else if (cell.col == CenterCol + 3) // For Buy
	{
		PutOrder(price, VtPositionType::Buy, VtPriceType::Price);
	}
	else if (cell.col == CenterCol + 4)
	{
		AddStopOrder(price, VtPositionType::Buy);
	}
	else if (cell.col == CenterCol - 4)
	{
		AddStopOrder(price, VtPositionType::Sell);
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}


void SmOrderGridOut::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (_OrderConfigMgr && _OrderConfigMgr->orderWnd) {
		_OrderConfigMgr->orderWnd->SetActiveCenterWnd(_CenterWnd);
	}
	CCellID cell = GetCellFromPt(point);
	CGridCellBase* pCell = GetCell(cell.row, cell.col);
	if (pCell) {
		// Ŭ���� ���� ǥ���ϰ� �����Ѵ�.
		if (cell.row >= _StartRowForValue &&
			cell.row <= _EndRowForValue && (
				cell.col == CenterCol - 4 ||
				cell.col == CenterCol - 3 ||
				cell.col == CenterCol + 4 ||
				cell.col == CenterCol + 3)) {

			pCell->SetClicked(true);
			if (_OldClickedCell.IsValid() == TRUE) {
				CGridCellBase* pOldClickedCell = GetCell(_OldClickedCell.row, _OldClickedCell.col);
				if (pOldClickedCell) {
					pOldClickedCell->SetClicked(false);
					InvalidateCellRect(_OldClickedCell);
				}
			}
			InvalidateCellRect(cell.row, cell.col);
			_OldClickedCell = cell;
		}
	}
	if (IsValid(cell) == TRUE) {
		CGridCellBase* pCell = GetCell(cell.row, cell.col);
		if (pCell->GetOrderCount() > 0 || pCell->GetStopOrderCount() > 0) {
			GetCellRect(cell, _DragStartRect);
			_OrderDragStarted = true;
			OrderCellStart = cell;
		}
	}

	SetCapture();
	int nButtonID = FindButtonID(cell.row, cell.col);
	HandleButtonEvent(nButtonID);
	CGridCtrl::OnLButtonDown(nFlags, point);
}


void SmOrderGridOut::OnLButtonUp(UINT nFlags, CPoint point)
{
	CCellID cell = GetCellFromPt(point);
	if (IsValid(cell) == TRUE) {
		if (_OrderDragStarted) {
			Invalidate();
		}
	}

	if (_OrderDragStarted) {
		OrderCellEnd = cell;
		if (OrderCellStart.row == cell.row &&
			OrderCellStart.col == cell.col) {
			OrderCellStart.row = -1;
			OrderCellStart.col = -1;
			OrderCellEnd.row = -1;
			OrderCellEnd.col = -1;
			_OrderDragStarted = false;
			return;
		}

		if (OrderCellEnd.col == CenterCol - 3 && OrderCellStart.col == CenterCol - 3) {
			ChangeOrder();
		}
		else if (OrderCellEnd.col == CenterCol + 3 && OrderCellStart.col == CenterCol + 3) {
			ChangeOrder();
		}
		else if (OrderCellEnd.col == CenterCol - 4 && OrderCellStart.col == CenterCol - 4) {
			ChangeStopOrder();
		}
		else if (OrderCellEnd.col == CenterCol + 4 && OrderCellStart.col == CenterCol + 4) {
			ChangeStopOrder();
		}
		else {
			CancelOrder();
		}

		OrderCellStart.row = -1;
		OrderCellStart.col = -1;
		OrderCellEnd.row = -1;
		OrderCellEnd.col = -1;
		_OrderDragStarted = false;
		Invalidate();
	}
	ReleaseCapture();
	CGridCtrl::OnLButtonUp(nFlags, point);
}


void SmOrderGridOut::OnMove(int x, int y)
{
	CGridCtrl::OnMove(x, y);
}


void SmOrderGridOut::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	//CGridCtrl::OnRButtonDown(nFlags, point);
}


void SmOrderGridOut::OnRButtonUp(UINT nFlags, CPoint point)
{
	CCellID cell = GetCellFromPt(point);
	CGridCellBase* pSrcCell = GetCell(cell.row, cell.col);
	/*
	if (pSrcCell->GetOrderCount() > 0) {
		std::map<std::string, VtOrder*>& orderMap = pSrcCell->GetOrderMap();
		for (auto it = orderMap.begin(); it != orderMap.end(); ++it) {
			VtOrder* order = it->second;
			CancelOrder(order);
		}
		pSrcCell->ClearOrders();
	}

	if (pSrcCell->GetStopOrderCount() > 0) {
		std::map<int, std::shared_ptr<HdOrderRequest>>& stopOrderMap = pSrcCell->GetStopOrderMap();
		for (auto it = stopOrderMap.begin(); it != stopOrderMap.end(); ++it) {
			_StopOrderMgr->RemoveOrderHd(it->first);
			_CutMgr->RemoveOrderHd(it->first);
		}

		pSrcCell->ClearStopOrders();
		std::set<std::pair<int, int>> refreshSet;
		ClearOldStopOrders(refreshSet);
		SetStopOrderInfo(refreshSet);
		CalcPosStopOrders(refreshSet);
		RefreshCells(refreshSet);
	}

	//CGridCtrl::OnRButtonUp(nFlags, point);
	*/
}

void SmOrderGridOut::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_SPACE) {
		AfxMessageBox("space clicked");
	}

	CGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void SmOrderGridOut::DrawArrow(int direction, POINT *point, CDC* pdc, POINT p0, POINT p1, int head_length, int head_width)
{
	CBrush brush1;   // Must initialize!
	brush1.CreateSolidBrush(RGB(0, 0, 255));   // Blue brush.

	CBrush* pTempBrush = NULL;
	CBrush OrigBrush;

	//CString msg;
	//msg = _T("����");
	pTempBrush = (CBrush*)pdc->SelectObject(&brush1);
	// Save original brush.
	OrigBrush.FromHandle((HBRUSH)pTempBrush);
	POINT p2;
	float dx = 0.0;
	float dy = 0.0;
	if (direction == 1)
	{
		pdc->MoveTo(p0);
		pdc->LineTo(p1);
		dx = static_cast<float>(p1.x - p0.x);
		dy = static_cast<float>(p1.y - p0.y);
	}
	else
	{

		p2.y = p1.y;
		p2.x = p0.x;
		pdc->MoveTo(p0);
		pdc->LineTo(p2);

		//pdc->MoveTo(p2);
		pdc->LineTo(p1);
		//msg = _T("���");
		dx = static_cast<float>(p1.x - p2.x);
		dy = static_cast<float>(p1.y - p2.y);
	}

	const auto length = std::sqrt(dx*dx + dy*dy);
	if (head_length < 1 || length < head_length) return;

	// ux,uy is a unit vector parallel to the line.
	const auto ux = dx / length;
	const auto uy = dy / length;

	// vx,vy is a unit vector perpendicular to ux,uy
	const auto vx = -uy;
	const auto vy = ux;

	const auto half_width = 0.5f * head_width;

	const POINT arrow[3] =
	{ p1,
		POINT{ Round(p1.x - head_length*ux + half_width*vx),
		Round(p1.y - head_length*uy + half_width*vy) },
		POINT{ Round(p1.x - head_length*ux - half_width*vx),
		Round(p1.y - head_length*uy - half_width*vy) }
	};
	pdc->Polygon(arrow, 3);
	pdc->SetBkMode(TRANSPARENT);
	pdc->SelectObject(&OrigBrush);
}


void SmOrderGridOut::RedrawOrderTrackCells()
{
	int min_col = min(_DragStartCol, _OldMMCell.col);
	int max_col = max(_DragStartCol, _OldMMCell.col);
	int min_row = min(_DragStartRow, _OldMMCell.row);
	int max_row = max(_DragStartRow, _OldMMCell.row);
	for (int i = min_row; i <= max_row; ++i) {
		for (int j = min_col; j <= max_col; ++j) {
			InvalidateCellRect(i, j);
		}
	}
}

void SmOrderGridOut::AddStopOrder(int price, VtPositionType posi)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;
	/*
	if (_OrderConfigMgr->Type() == 0) { // ���� �ֹ�
		if (!_OrderConfigMgr->Account())
			return;
		const VtAccount* acnt = _OrderConfigMgr->Account();
		std::shared_ptr<HdOrderRequest> request = std::make_shared<HdOrderRequest>();
		request->Market = 1; // �ؿ� ���� ���� ����
		if (acnt->AccountLevel() == 0) { // �������� ���
			request->Type = 0;
			request->AccountNo = acnt->AccountNo;
			request->Password = acnt->Password;
		}
		else { // ������� �� ���
			request->Type = 1;
			VtAccount* parentAcnt = acnt->ParentAccount();
			if (parentAcnt) {
				request->AccountNo = parentAcnt->AccountNo;
				request->Password = parentAcnt->Password;
			}
		}
		request->Price = price;
		request->Position = posi;
		request->Amount = _CenterWnd->OrderAmount();
		request->SymbolCode = _CenterWnd->Symbol()->ShortCode;
		request->FillCondition = _CenterWnd->FillCondition();
		request->PriceType = _CenterWnd->PriceType();
		request->slip = _CenterWnd->StopVal();
		request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
		request->SourceId = (long)this;
		request->SubAccountNo = _T("");
		request->FundName = _T("");
		_StopOrderMgr->AddOrderHd(request);

	}
	else { // �ݵ� �ֹ�
		if (!_OrderConfigMgr->Fund())
			return;
		VtFund* const fund = _OrderConfigMgr->Fund();
		const std::vector<VtAccount*>& acntVec = fund->GetFundAccountVector();
		for (auto it = acntVec.begin(); it != acntVec.end(); ++it) {
			VtAccount* acnt = *it;
			// �θ� ���°� ���� ���� �ֹ��� �� �� ����.
			if (!acnt->ParentAccount())
				continue;

			std::shared_ptr<HdOrderRequest> request = std::make_shared<HdOrderRequest>();
			request->Market = 1; // �ؿ� ���� ���� ����
			request->Type = 2;
			request->Price = price;
			request->Position = posi;
			// �ݵ�� ������ �¼��� �־� �ش�.
			request->Amount = acnt->SeungSu;
			request->AccountNo = acnt->ParentAccount()->AccountNo;
			request->Password = acnt->ParentAccount()->Password;
			request->SymbolCode = _CenterWnd->Symbol()->ShortCode;
			request->FillCondition = _CenterWnd->FillCondition();
			request->PriceType = _CenterWnd->PriceType();
			request->slip = _CenterWnd->StopVal();
			request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
			request->SourceId = (long)this;
			request->SubAccountNo = acnt->AccountNo;
			request->FundName = fund->Name;
			_StopOrderMgr->AddOrderHd(request);
		}
	}
	*/

	RefreshStopOrder();
}

void SmOrderGridOut::InvalidateClickedCell()
{
	if (_OldClickedCell.IsValid() == TRUE) {
		CGridCellBase* pOldClickedCell = GetCell(_OldClickedCell.row, _OldClickedCell.col);
		if (pOldClickedCell) {
			pOldClickedCell->SetClicked(false);
			InvalidateCellRect(_OldClickedCell);
		}
	}
	_OldClickedCell.row = -1;
	_OldClickedCell.col = -1;
}

void SmOrderGridOut::SetMovingCell(CCellID cell)
{
	if (_OldMovingCellCenter.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(_OldMovingCellCenter.row, _OldMovingCellCenter.col);
		if (pCell) {
			pCell->SetMoving(false);
			InvalidateCellRect(_OldMovingCellCenter);
		}
	}

	if (_OldMovingCellSide.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(_OldMovingCellSide.row, _OldMovingCellSide.col);
		if (pCell) {
			pCell->SetMoving(false);
			InvalidateCellRect(_OldMovingCellSide);
		}
	}

	if (cell.IsValid() == FALSE)
		return;

	switch (cell.col)
	{
	case CenterCol:
		_OldMovingCellCenter = cell;
		break;
	case CenterCol - 4:
		_OldMovingCellCenter.row = cell.row;
		_OldMovingCellCenter.col = CenterCol;
		_OldMovingCellSide.row = cell.row;
		_OldMovingCellSide.col = CenterCol - 4;
		break;
	case CenterCol - 3:
		_OldMovingCellCenter.row = cell.row;
		_OldMovingCellCenter.col = CenterCol;
		_OldMovingCellSide.row = cell.row;
		_OldMovingCellSide.col = CenterCol - 3;
		break;
	case CenterCol + 3:
		_OldMovingCellCenter.row = cell.row;
		_OldMovingCellCenter.col = CenterCol;
		_OldMovingCellSide.row = cell.row;
		_OldMovingCellSide.col = CenterCol + 3;
		break;
	case CenterCol + 4:
		_OldMovingCellCenter.row = cell.row;
		_OldMovingCellCenter.col = CenterCol;
		_OldMovingCellSide.row = cell.row;
		_OldMovingCellSide.col = CenterCol + 4;
		break;
	default:
		_OldMovingCellCenter.row = -1;
		_OldMovingCellCenter.col = -1;
		_OldMovingCellSide.row = -1;
		_OldMovingCellSide.col = -1;
		break;
	}

	if (_OldMovingCellCenter.row < _StartRowForValue ||
		_OldMovingCellCenter.row > _EndRowForValue) {
		_OldMovingCellCenter.row = -1;
		_OldMovingCellCenter.col = -1;
		_OldMovingCellSide.row = -1;
		_OldMovingCellSide.col = -1;
	}

	if (_OldMovingCellCenter.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(_OldMovingCellCenter.row, _OldMovingCellCenter.col);
		if (pCell) {
			pCell->SetMoving(true);
			InvalidateCellRect(_OldMovingCellCenter);
		}
	}

	if (_OldMovingCellSide.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(_OldMovingCellSide.row, _OldMovingCellSide.col);
		if (pCell) {
			pCell->SetMoving(true);
			InvalidateCellRect(_OldMovingCellSide);
		}
	}
}

void SmOrderGridOut::HandleButtonEvent(int button_id)
{
	if (button_id == -1)
		return;
	switch (button_id)
	{
	case 1: // ����
		ResetByCenterRow();
		break;
	case 2: // ���尡 �ŵ�
		SellAtMarket();
		break;
	case 3: // ���尡 �ż�
		BuyAtMarket();
		break;
	case 4: // �ŵ� ��ž ���
		CancelAllSellStop();
		break;
	case 5: // �ŵ� �ֹ� ���
		CancelAllSellAccepted();
		break;
	case 6: // ��ü �ֹ� ���
		CancelAllAccepted();
		break;
	case 7: // �ż� �ֹ� ���
		CancelAllBuyAccepted();
		break;
	case 8: // �ż� ��ž ���
		CancelAllBuyStop();
		break;
	default:
		break;
	}
}

void SmOrderGridOut::ChangeOrder(VtOrder* order, int newPrice)
{
	if (!order)
		return;

	if (!_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	/*
	if (_OrderConfigMgr->Type() == 0)
	{
		if (!_OrderConfigMgr->Account())
			return;
		VtAccount* acnt = _OrderConfigMgr->Account();

		std::shared_ptr<HdOrderRequest> request = std::make_shared<HdOrderRequest>();
		request->Market = 1; // �ؿ� ���� ���� ����
		request->Price = newPrice;
		request->Position = order->orderPosition;
		request->Amount = order->amount;
		request->RequestType = order->RequestType;

		if (acnt->AccountLevel() == 0)
		{
			request->Type = 0;
			request->AccountNo = acnt->AccountNo;
			request->Password = acnt->Password;
		}
		else
		{
			VtAccount* parentAcnt = acnt->ParentAccount();
			if (parentAcnt)
			{
				request->AccountNo = parentAcnt->AccountNo;
				request->Password = parentAcnt->Password;
			}
			request->Type = 1;
		}
		request->SymbolCode = order->shortCode;
		request->FillCondition = _CenterWnd->FillCondition();
		request->PriceType = VtPriceType::Price;
		request->OrderNo = order->orderNo;
		request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
		request->SourceId = (long)this;
		if (acnt->AccountLevel() == 0)
		{
			request->SubAccountNo = order->SubAccountNo;
			request->FundName = order->FundName;
		}
		else
			request->SubAccountNo = acnt->AccountNo;

		request->orderType = VtOrderType::Change;

		_OrderConfigMgr->OrderMgr()->ChangeOrder(std::move(request));
	}
	else
	{
		if (_OrderConfigMgr->Fund())
		{
			VtAccount* subAcnt = mainApp.AcntMgr().SubAcntMgr().FindAccount(order->SubAccountNo);
			if (!subAcnt)
				return;
			VtAccount* parentAcnt = subAcnt->ParentAccount();
			if (!parentAcnt)
				return;

			std::shared_ptr<HdOrderRequest> request = std::make_shared<HdOrderRequest>();
			request->Market = 1; // �ؿ� ���� ���� ����
			request->Type = 2;
			request->RequestType = order->RequestType;
			request->Price = newPrice;
			request->Position = order->orderPosition;
			request->Amount = order->amount;
			request->AccountNo = parentAcnt->AccountNo;
			request->Password = parentAcnt->Password;
			request->SymbolCode = order->shortCode;
			request->FillCondition = _CenterWnd->FillCondition();
			request->PriceType = _CenterWnd->PriceType();
			request->OrderNo = order->orderNo;
			request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
			request->SourceId = (long)this;
			request->SubAccountNo = subAcnt->AccountNo;
			request->FundName = _OrderConfigMgr->Fund()->Name;
			request->orderType = VtOrderType::Change;

			_OrderConfigMgr->OrderMgr()->ChangeOrder(std::move(request));
		}
	}
	*/
}

void SmOrderGridOut::ChangeOrder()
{
	/*
	try
	{
		CGridCellBase* pSrcCell = GetCell(OrderCellStart.row, OrderCellStart.col);
		if (pSrcCell->GetOrderCount() > 0) {
			auto it = RowToValueMap.find(OrderCellEnd.row);
			if (it != RowToValueMap.end()) {
				int price = it->second;
				std::map<std::string, VtOrder*> orderMap = pSrcCell->GetOrderMap();
				for (auto it = orderMap.begin(); it != orderMap.end(); ++it) {
					VtOrder* order = it->second;
					//if (order->state == VtOrderState::Accepted)
					ChangeOrder(order, price);
				}

				pSrcCell->ClearOrders();
			}
			else {
				int i = 0;
				i = i + 1;
			}
		}
	}
	catch (std::exception& e)
	{
		std::string error = e.what();
	}
	*/
}

void SmOrderGridOut::ChangeStopOrder()
{
	/*
	try
	{
		CGridCellBase* pSrcCell = GetCell(OrderCellStart.row, OrderCellStart.col);
		CGridCellBase* pTgtCell = GetCell(OrderCellEnd.row, OrderCellEnd.col);
		if (pSrcCell->GetStopOrderCount() > 0) {
			auto it = RowToValueMap.find(OrderCellEnd.row);
			if (it != RowToValueMap.end()) {
				int price = it->second;
				std::map<int, std::shared_ptr<HdOrderRequest>> stopOrderMap = pSrcCell->GetStopOrderMap();
				for (auto it = stopOrderMap.begin(); it != stopOrderMap.end(); ++it) {
					std::shared_ptr<HdOrderRequest> order = it->second;
					order->Price = price;
				}
				std::set<std::pair<int, int>> refreshSet;
				ClearOldStopOrders(refreshSet);
				SetStopOrderInfo(refreshSet);
				CalcPosStopOrders(refreshSet);
				RefreshCells(refreshSet);
			}
		}
	}
	catch (std::exception& e)
	{
		std::string error = e.what();
	}
	*/
}

void SmOrderGridOut::CancelOrder(VtOrder* order)
{
	if (!order || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	/*
	try
	{

		if (_OrderConfigMgr->Type() == 0)
		{
			if (!_OrderConfigMgr->Account())
				return;
			VtAccount* acnt = _OrderConfigMgr->Account();

			std::shared_ptr<HdOrderRequest> request = std::make_shared<HdOrderRequest>();
			request->Market = 1; // �ؿ� ���� ���� ����
			request->Price = order->intOrderPrice;
			request->Position = order->orderPosition;
			request->Amount = order->amount;
			request->RequestType = order->RequestType;

			if (acnt->AccountLevel() == 0)
			{
				request->Type = 0;
				request->AccountNo = acnt->AccountNo;
				request->Password = acnt->Password;
			}
			else
			{
				VtAccount* parentAcnt = acnt->ParentAccount();
				if (parentAcnt)
				{
					request->AccountNo = parentAcnt->AccountNo;
					request->Password = parentAcnt->Password;
				}
				request->Type = 1;
			}
			request->SymbolCode = order->shortCode;
			request->FillCondition = VtFilledCondition::Fas;
			request->PriceType = VtPriceType::Price;
			request->OrderNo = order->orderNo;
			request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
			request->SourceId = (long)this;
			if (acnt->AccountLevel() == 0)
			{
				request->SubAccountNo = order->SubAccountNo;
				request->FundName = order->FundName;
			}
			else
				request->SubAccountNo = acnt->AccountNo;

			request->orderType = VtOrderType::Cancel;

			_OrderConfigMgr->OrderMgr()->CancelOrder(std::move(request));
		}
		else
		{
			if (_OrderConfigMgr->Fund())
			{
				VtAccount* subAcnt = mainApp.AcntMgr().SubAcntMgr().FindAccount(order->SubAccountNo);
				if (!subAcnt)
					return;
				VtAccount* parentAcnt = subAcnt->ParentAccount();
				if (!parentAcnt)
					return;

				std::shared_ptr<HdOrderRequest> request = std::make_shared<HdOrderRequest>();
				request->Market = 1; // �ؿ� ���� ���� ����
				request->Type = 2;
				request->RequestType = order->RequestType;
				request->Price = order->intOrderPrice;
				request->Position = order->orderPosition;
				request->Amount = order->amount;
				request->AccountNo = parentAcnt->AccountNo;
				request->Password = parentAcnt->Password;
				request->SymbolCode = order->shortCode;
				request->FillCondition = VtFilledCondition::Fas;
				request->PriceType = VtPriceType::Price;
				request->OrderNo = order->orderNo;
				request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
				request->SourceId = (long)this;
				request->SubAccountNo = subAcnt->AccountNo;
				request->FundName = _OrderConfigMgr->Fund()->Name;
				request->orderType = VtOrderType::Cancel;

				_OrderConfigMgr->OrderMgr()->CancelOrder(std::move(request));
			}
		}
	}
	catch (std::exception& e) {
		std::string error = e.what();
	}
	*/
}

void SmOrderGridOut::CancelOrder()
{
	/*
	try
	{
		CGridCellBase* pSrcCell = GetCell(OrderCellStart.row, OrderCellStart.col);
		if (pSrcCell->GetOrderCount() > 0) {
			std::map<std::string, VtOrder*> orderMap = pSrcCell->GetOrderMap();
			for (auto it = orderMap.begin(); it != orderMap.end(); ++it) {
				VtOrder* order = it->second;
				if (order->state == VtOrderState::Accepted)
					CancelOrder(order);
			}
			pSrcCell->ClearOrders();
		}

		if (pSrcCell->GetStopOrderCount() > 0) {
			std::map<int, std::shared_ptr<HdOrderRequest>>& stopOrderMap = pSrcCell->GetStopOrderMap();
			for (auto it = stopOrderMap.begin(); it != stopOrderMap.end(); ++it) {
				_StopOrderMgr->RemoveOrderHd(it->first);
			}
			pSrcCell->ClearStopOrders();
			std::set<std::pair<int, int>> refreshSet;
			ClearOldStopOrders(refreshSet);
			SetStopOrderInfo(refreshSet);
			CalcPosStopOrders(refreshSet);
			RefreshCells(refreshSet);
		}
	}
	catch (std::exception& e)
	{
		std::string error = e.what();
	}
	*/
}

void SmOrderGridOut::OnMouseMove(UINT nFlags, CPoint point)
{
	_MouseIn = true;

	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = this->m_hWnd;

		if (::_TrackMouseEvent(&tme)) {
			m_bMouseTracking = TRUE;
		}
	}

	CCellID cell = GetCellFromPt(point);

	SetMovingCell(cell);

	_OldMMCell = cell;
	if (_OrderDragStarted && cell.IsValid() == TRUE) {
		OrderCellEnd = cell;
	}

	if (_OrderDragStarted) {
		Invalidate();
	}

	CGridCtrl::OnMouseMove(nFlags, point);
}

LRESULT SmOrderGridOut::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	_MouseIn = false;
	m_bMouseTracking = FALSE;
	CCellID cell;
	cell.row = -1;
	cell.col = -1;
	SetMovingCell(cell);
	return 1;
}


int SmOrderGridOut::FindPositionRow(VtPosition* posi)
{
	if (!posi || ValueToRowMap.size() == 0)
		return -1;
	/*
	symbol_p sym = mainApp.SymbolMgr().FindSymbol(posi->ShortCode);
	if (!sym)
		return -1;

	int intAvg = static_cast<int>(ROUNDING(posi->AvgPrice * std::pow(10, sym->Decimal), sym->Decimal));
	intAvg = intAvg - intAvg % sym->intTickSize;

	auto it = ValueToRowMap.find(intAvg);
	if (it != ValueToRowMap.end())
		return it->second;
	else
		return -1;
		*/
	return -1;
}

void SmOrderGridOut::OnClose()
{
	int i = 0;
	i = i + 1;
	//UnregisterQuoteCallback();

	CGridCtrl::OnClose();
}


void SmOrderGridOut::OnDestroy()
{
	UnregisterQuoteCallback();
	CGridCtrl::OnDestroy();

	// TODO: Add your message handler code here
}

LRESULT SmOrderGridOut::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	if (!_Init)
		return 1;

	if (!_CenterWnd || !_CutMgr || !_CenterWnd->Symbol()) {
		return 1;
	}
	/*
	symbol_p symbol = (symbol_p)lParam;
	if (_CenterWnd->Symbol()->ShortCode.compare(symbol->ShortCode) != 0) {
		return 1;
	}

	std::set<std::pair<int, int>> refreshSet;

	// �߾� �����϶��� ���� ���� ���� ã�Ƽ� ��Ī���ش�.
	if (_CenterWnd->FixedCenter()) {
		_IndexRow = FindIndexRow();
		SetCenterValue(_CenterWnd->Symbol(), refreshSet);
	}

	// ���� �ü��� �����.
	ClearQuotes(refreshSet);
	// ���� ��Ȳ�� ���� �ü��� �����Ѵ�.
	SetQuoteColor(_CenterWnd->Symbol(), refreshSet);

	// �������� �ִٸ� ������ ǥ�ø� �ٽ� �׸���.
	ClearPositionInfo(refreshSet);
	SetPositionInfo(refreshSet);

	// ��ž �ֹ��� ������ ���� �ֱ� ������ ��ž �ֹ��� �ٽ� �׷��ش�.
	ClearOldStopOrders(refreshSet);
	SetStopOrderInfo(refreshSet);
	CalcPosStopOrders(refreshSet);

	// ���õ� ������ ������Ʈ �Ѵ�.
	RefreshCells(refreshSet);
	*/
	return 1;
}

LRESULT SmOrderGridOut::OnHogaChangedMessage(WPARAM wParam, LPARAM lParam)
{
	if (!_Init) {
		return 1;
	}

	if (!_CenterWnd || !_CutMgr || !_CenterWnd->Symbol()) {
		return 1;
	}
	/*
	symbol_p symbol = (symbol_p)lParam;
	if (_CenterWnd->Symbol()->ShortCode.compare(symbol->ShortCode) != 0) {
		return 1;
	}

	std::set<std::pair<int, int>> refreshSet;
	

	ClearHogas(refreshSet);
	SetHogaInfo(symbol, refreshSet);

	ClearOldOrders(refreshSet);
	SetOrderInfo(refreshSet);

	ClearOldStopOrders(refreshSet);
	SetStopOrderInfo(refreshSet);
	CalcPosStopOrders(refreshSet);

	 
	RefreshCells(refreshSet);
	*/
	return 1;
}

LRESULT SmOrderGridOut::OnOrderChangedMessage(WPARAM wParam, LPARAM lParam)
{
	if (!_Init)
		return 1;

	if (!_CenterWnd || !_CutMgr || !_CenterWnd->Symbol()) {
		return 1;
	}
	/*
	VtOrder* order = (VtOrder*)wParam;
	if (_CenterWnd->Symbol()->ShortCode.compare(order->shortCode) != 0)
		return 1;

	std::set<std::pair<int, int>> refreshSet;

	// �������� �ִٸ� ������ ǥ�ø� �ٽ� �׸���.
	ClearPositionInfo(refreshSet);
	SetPositionInfo(refreshSet);

	// �ֹ��� �ٽ� �׷��ش�.
	ClearOldOrders(refreshSet);
	SetOrderInfo(refreshSet);

	// ��ž �ֹ��� ������ ���� �ֱ� ������ ��ž �ֹ��� �ٽ� �׷��ش�.
	ClearOldStopOrders(refreshSet);
	SetStopOrderInfo(refreshSet);
	CalcPosStopOrders(refreshSet);

	// ���õ� ������ ������Ʈ �Ѵ�.
	RefreshCells(refreshSet);
	*/
	return 1;
}

int SmOrderGridOut::ShowHideOrderGrid(std::vector<bool>& colOptions)
{
	for (size_t i = 0; i < colOptions.size(); ++i) {
		if (i == 0) {
			if (colOptions[i]) { // for order column
				SetColumnWidth(1, _GridColMap[SmOrderGridCol::ORDER]);
				SetColumnWidth(7, _GridColMap[SmOrderGridCol::ORDER]);
			}
			else {
				SetColumnWidth(1, 0);
				SetColumnWidth(7, 0);
			}
		}
		else if (i == 1) { // for stop column
			if (colOptions[1]) {
				SetColumnWidth(0, _GridColMap[SmOrderGridCol::STOP]);
				SetColumnWidth(8, _GridColMap[SmOrderGridCol::STOP]);
			}
			else {
				SetColumnWidth(0, 0);
				SetColumnWidth(8, 0);
			}
		}
		else if (i == 2) { // for order count column
			if (colOptions[2]) {
				SetColumnWidth(2, _GridColMap[SmOrderGridCol::COUNT]);
				SetColumnWidth(6, _GridColMap[SmOrderGridCol::COUNT]);
			}
			else {
				SetColumnWidth(2, 1);
				SetColumnWidth(6, 1);
			}
		}
	}

	return 0;
}

int SmOrderGridOut::GetGridWidth(std::vector<bool>& colOptions)
{
	int totalWidth = _GridColMap[SmOrderGridCol::CENTER] + _GridColMap[SmOrderGridCol::QUANTITY] * 2;
	for (size_t i = 0; i < colOptions.size(); ++i) {
		if (i == 0) { // for order
			totalWidth += colOptions[i] ? _GridColMap[SmOrderGridCol::ORDER] * 2 : 0;
		}
		else if (i == 1) { // for stop
			totalWidth += colOptions[i] ? _GridColMap[SmOrderGridCol::STOP] * 2 : 0;
		}
		else if (i == 2) { // for count and quantity.
			totalWidth += colOptions[i] ? _GridColMap[SmOrderGridCol::COUNT] * 2 : 0;
		}
	}

	return totalWidth + 5;
}

int SmOrderGridOut::GetGridHeight()
{
	int total = _HeadHeight;
	total += _CellHeight * _RowCount;

	return total;
}

void SmOrderGridOut::ApplyProfitLossForPosition()
{
	if (!_CutMgr || !_CenterWnd || !_CenterWnd->Symbol())
		return;
	// ��� ����,���� ��ž ���
	//_CutMgr->RemoveAllHd();
	//_CutMgr->MakePositionStopByRemain(_CenterWnd->Symbol());
	std::set<std::pair<int, int>> refreshSet;
	ClearOldOrders(refreshSet);
	SetOrderInfo(refreshSet);
	ClearPositionInfo(refreshSet);
	SetPositionInfo(refreshSet);
	RefreshCells(refreshSet);
}

void SmOrderGridOut::ResetByCenterRow()
{
	_IndexRow = FindIndexRow();
	SetCenterValue();
}
