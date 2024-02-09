#include "stdafx.h"
#include "SmOptionGrid.h"
#include "VtOrderConfigManager.h"
#include "VtOrderLeftWndHd.h"

#include "SmOrderPanel.h"
#include <functional>
#include "../MessageDefine.h"

#include "../Symbol/SmSymbol.h"
#include "../Account/SmAccount.h"

#include "../Event/SmCallbackManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmProduct.h"
#include "../Symbol/SmProductYearMonth.h"
#include <format>
#include <algorithm>

#include <functional>
#include "../Fund/SmFund.h"
#include "../Event/EventHub.h"

#include "../Controller/QuoteControl.h"
#include "../ViewModel/VmQuote.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Util/SmUtil.h"
#include "../Global/SmTotalManager.h"
#include "../Task/SmTaskArg.h"
#include "../Task/SmTaskRequestManager.h"
#include "../Util/IdGenerator.h"
#include "../Controller/SymbolPositionControl.h"
#include "../Log/MyLogger.h"
#include "../Order/OrderProcess/TotalOrderManager.h"
#include "../Order/OrderProcess/AccountOrderManager.h"
#include "../Order/OrderProcess/SymbolOrderManager.h"
#include "../Order/Order.h"
#include "../Position/TotalPositionManager.h"
#include "../Position/AccountPositionManager.h"
#include "../Position/Position.h"
#include "../ViewModel/VmQuote.h"
#include "../Util/IdGenerator.h"

#pragma warning(disable:4018)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;
using namespace std::placeholders;

SmOptionGrid::SmOptionGrid()
	: id_(DarkHorse::IdGenerator::get_id())
{
	quote_control_ = std::make_shared<DarkHorse::QuoteControl>();
	quote_control_->symbol_type(DarkHorse::SymbolType::Domestic);
	quote_control_->set_event_handler(std::bind(&SmOptionGrid::on_update_quote, this));

	position_control_ = std::make_shared<DarkHorse::SymbolPositionControl>();
	position_control_->set_vm_option_event_handler(std::bind(&SmOptionGrid::on_update_position_vm, this, _1));

	mainApp.event_hub()->subscribe_expected_event_handler
	(
		id_,
		std::bind(&SmOptionGrid::update_expected, this, std::placeholders::_1)
	);

	mainApp.event_hub()->subscribe_order_event_handler
	(
		id_,
		std::bind(&SmOptionGrid::update_order, this, std::placeholders::_1, std::placeholders::_2)
	);
}


SmOptionGrid::~SmOptionGrid()
{
	mainApp.event_hub()->unsubscribe_order_event_handler(id_);
	mainApp.event_hub()->unsubscribe_expected_event_handler(id_);
}

void SmOptionGrid::RegisterMasterCallback()
{
	//mainApp.CallbackMgr().SubscribeMasterCallback((long)this, std::bind(&SmOptionGrid::OnMasterEvent, this, _1));
}

void SmOptionGrid::UnregisterAllCallback()
{
	//mainApp.CallbackMgr().UnsubscribeOrderCallback((long)this);
	//mainApp.CallbackMgr().UnsubscribeMasterCallback((long)this);
	//mainApp.CallbackMgr().UnsubscribeQuoteWndCallback(GetSafeHwnd());
}

void SmOptionGrid::RegisterQuoteCallback()
{
	//mainApp.CallbackMgr().SubscribeQuoteWndCallback(GetSafeHwnd());
}

void SmOptionGrid::RegisterOrderCallback()
{
	//mainApp.CallbackMgr().SubscribeOrderCallback((long)this, std::bind(&SmOptionGrid::OnOrderEvent, this, _1));
}

void SmOptionGrid::OnMasterEvent(symbol_p sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 1) {
		ShowCurrent(sym);
	}
}

void SmOptionGrid::OnOrderEvent(VtOrder* order)
{
	try {
		CString msg;
		//msg.Format("OnOrderEvent code = %s\n", order->shortCode.c_str());
		//TRACE(msg);
		if (!_OrderConfigMgr || !order)
			return;
		// 잔고 모드가 아니면 반응하지 않음
		if (_Mode != 0)
			return;

		// 체결 이벤트
		/*
		if (order->state == VtOrderState::Filled) {
			if (_OrderConfigMgr->Type() == 0) {
				VtAccount* acnt = _OrderConfigMgr->Account();
				if (!acnt)
					return;

				VtPosition* posi = acnt->FindPosition(order->shortCode);
				if (posi)
					SetRemain(posi);
			}
			else {
				VtFund* fund = _OrderConfigMgr->Fund();
				if (!fund)
					return;
				int count = 0;
				VtPosition posi = fund->GetPosition(order->shortCode, count);
				SetRemain(&posi);
			}
		}
		else { // 나머지 주문 이벤트 
			symbol_p symbol = mainApp.SymbolMgr().FindSymbol(order->shortCode);
			if (symbol) {
				SetRemain(symbol);
			}
		}
		*/
	} 
	catch (...)
	{
		auto expPtr = std::current_exception();
		try
		{
			if (expPtr) std::rethrow_exception(expPtr);
		}
		catch (const std::exception& e) //it would not work if you pass by value
		{
			std::string msg = e.what();
		}
	}
}

void SmOptionGrid::OnQuoteEvent(symbol_p sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 1) {
		ShowCurrent(sym);
	}
}

BEGIN_MESSAGE_MAP(SmOptionGrid, CGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_QUOTE_CHANGED, OnQuoteChangedMessage)
END_MESSAGE_MAP()

void SmOptionGrid::Init()
{
	_EqualIndex = -1;
	_EqualCell.row = -1;
	_EqualCell.col = -1;
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

	for (int i = 0; i < _RowCount; i++) {
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
	RegisterMasterCallback();
	RegisterOrderCallback();

	SetColTitle();
}
/*
void SmOptionGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	CCellID cell = GetCellFromPt(point);
	CGridCellBase* pCell = GetCell(cell.row, cell.col);
	
	symbol_p sym = (symbol_p)pCell->GetData();
	if (sym) {
		sym->GetSymbolMaster();
		if (_OrderConfigMgr->_HdCenterWnd) {
			_OrderConfigMgr->Symbol(sym);
			_OrderConfigMgr->_HdCenterWnd->ChangeSymbol(sym);
		}
	}

	
}
*/
void SmOptionGrid::OnRButtonDown(UINT nFlags, CPoint point)
{
	return;
}

BOOL SmOptionGrid::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int distance = zDelta / 60;
	if (abs(zDelta) > 120)
		distance = zDelta / 120;
	else
		distance = zDelta / 40;
	set_strike_start_index(distance);
	set_option_view();
	Invalidate();
	return CGridCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

LRESULT SmOptionGrid::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	symbol_p* pData = reinterpret_cast<symbol_p*>(lParam);
	symbol_p symbol = *pData;
	
	if (_Mode == 0) {
		SetRemain(symbol);
	}
	else if (_Mode == 1) {
		ShowCurrent(symbol);
	}

	return 1;
}

LRESULT SmOptionGrid::OnOrderChangedMessage(WPARAM wParam, LPARAM lParam)
{
	return 1;
}

void SmOptionGrid::SetColTitle()
{
	LPCTSTR title[3] = { "Call", "행사가", "Put" };
	int colWidth[9] = { 60, 50, 60 };

	for (int i = 0; i < _ColCount; i++)
	{
		SetColumnWidth(i, colWidth[i]);
		CGridCellBase* pCell = GetCell(0, i);
		pCell->SetText(title[i]);
		if (i == 0) {
			pCell->SetTextClr(RGB(255, 255, 255));
			pCell->SetBackClr(RGB(240, 51, 58));
		}
		else if (i == 2) {
			pCell->SetTextClr(RGB(255, 255, 255));
			pCell->SetBackClr(RGB(19, 137, 255));
		}
		else {
			pCell->SetTextClr(DarkHorse::SmTotalManager::GridTitleBackColor);
			pCell->SetBackClr(DarkHorse::SmTotalManager::GridTitleTextColor);
		}
		InvalidateCellRect(0, i);
	}
}

void SmOptionGrid::InitGrid()
{
	CRect rcGrid;
	GetClientRect(rcGrid);
	InitGrid(rcGrid.Height());
}

void SmOptionGrid::InitGrid(int height)
{
	calcMaxRow(height);
	calcMaxSymbol();
	calcStartIndex();
	set_option_view();
}

void SmOptionGrid::calcMaxRow(const int height)
{
	_maxRow = (int)(height / _CellHeight);
}

void SmOptionGrid::calcMaxSymbol()
{
	_maxSymbol = call_symbol_vector_.size();
}

int SmOptionGrid::findAtmIndex()
{
	_atmIndex = (int)(_maxSymbol / 2);
	for (size_t i = 0; i < call_symbol_vector_.size(); i++) {
		if (call_symbol_vector_[i].atm == 1) {
			_atmIndex = i;
			break;
		}
	}

	return _atmIndex;
}

int SmOptionGrid::calcStartIndex()
{
	if (_maxSymbol > _maxRow) {
		int atmIndex = findAtmIndex();
		int gridCenterIndex = (int)(_maxRow / 2);
		_startIndex = atmIndex - gridCenterIndex;
		if (_startIndex < 0)
			_startIndex = 0;
	}
	else
		_startIndex = 0;

	return _startIndex;
}

void SmOptionGrid::InitSymbol()
{
	if (!_LeftWnd)
		return;
	/*
	for (auto it = _RunInfo.begin(); it != _RunInfo.end(); ++it) {
		SmRunInfo& run_info = *it;
		int index = _LeftWnd->_ComboProduct.AddString(run_info.Name.c_str());
	}
	*/

	_LeftWnd->combo_option_market_.SetCurSel(0);
	_SelectedProduct = 0;
}

void SmOptionGrid::InitYearMonth()
{
	
}

void SmOptionGrid::SetYearMonth()
{
	int cur_sel = _LeftWnd->combo_option_market_.GetCurSel();
	if (cur_sel < 0)
		return;
	/*
	SmProduct* product = mainApp.SymbolMgr().MrktMgr().FindProductFromMap(_RunInfo[cur_sel].CallCode);
	if (!product)
		return;
	_LeftWnd->_ComboOptionMonth.ResetContent();

	std::map<std::string, SmProductYearMonth*>& year_month_map = product->GetYearMonthMap();
	for (auto it = year_month_map.begin(); it != year_month_map.end(); ++it) {
		SmProductYearMonth* year_month = it->second;

		std::string code = it->first;

		CString yearMon(code.c_str());
		int index = _LeftWnd->_ComboOptionMonth.AddString(yearMon);
	}
	*/

	
}

void SmOptionGrid::SetProductSection()
{
	if (!_LeftWnd)
		return;
	_SelectedProduct = _LeftWnd->combo_option_market_.GetCurSel();
}

void SmOptionGrid::GetSymbolMaster()
{
	if (_EqualIndex == -1)
		return;

	
}

void SmOptionGrid::RefreshMode()
{
	ResetRemainCells();
	if (_Mode == 0)
		SetRemain2();
	else if (_Mode == 1)
		SetCurrent2();
	else
		SetExpected2();
}

std::pair<int, int> SmOptionGrid::FindValueStartRow(int height)
{
	int selMon = _LeftWnd->combo_option_month_.GetCurSel();
	if (selMon == -1)
		return std::make_pair(0, 0);

	
	return std::make_pair(0, 0);
}

void SmOptionGrid::ClearAllText()
{
	for (int i = 1; i < _RowCount; ++i) {
		for (int j = 0; j < _ColCount; ++j) {
			QuickSetText(i, j, "");
			InvalidateCellRect(i, j);
		}
	}
}

void SmOptionGrid::ResetRemainCells()
{
	for (auto it = _RemainPos.begin(); it != _RemainPos.end(); ++it) {
		std::pair<int, int> pos = *it;
		if (pos.second == 0)
			QuickSetBackColor(pos.first, pos.second, RGB(252, 226, 228));
		else
			QuickSetBackColor(pos.first, pos.second, RGB(218, 226, 245));

		QuickSetText(pos.first, pos.second, _T(""));
	}
}

void SmOptionGrid::ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode)
{
	CUGCell cell;
	auto it = _SymbolRowMap.find(symCode);
	if (it != _SymbolRowMap.end()) {
		auto pos = _SymbolRowMap[symCode];
		int col = std::get<1>(pos);
		int row = std::get<0>(pos);
		if (col == 0) { // 콜일 경우
			if (!posi) {
				if (acptCnt > 0) { // 포지션 없고 접수확인 주문만 있을 때 - 회색 배경에 0
					QuickSetText(row, col, _T("0"));
					QuickSetBackColor(row, col, RGB(212, 186, 188));
				}
				else { // 접수 확인 주문이 없을 때
					if (init) { // 한번이라도 주문이 나갔으면 흰색배경에 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
					else { // 한번도 주문이 안 나갔으면 매수 배경색
						QuickSetText(row, col, _T(""));
						QuickSetBackColor(row, col, RGB(252, 226, 228));
					}
				}
			}
			else { // 포지션이 있는 경우 - 최소 한번이라도 주문이 나간것으로 간주한다. - 이전에 나갔어도 
				/*
				if (std::abs(posi->OpenQty) > 0) { // 잔고가 있는 경우
					QuickSetNumber(row, col, posi->OpenQty);
					if (acptCnt > 0) // 접수 확인 주문이 있는 경우  회색배경에 잔고 갯수
						QuickSetBackColor(row, col, RGB(212, 186, 188));
					else // 접수 확인 주문이 없는 경우는 흰색 배경에 잔고 갯수
						QuickSetBackColor(row, col, RGB(255, 255, 255));
				}
				else { // 잔고가 없는 경우
					if (acptCnt > 0) { // 한번이라도 주문이 나간경우 회색배경에 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(212, 186, 188));
					}
					else { // 잔고, 접수확인 둘 다 없을 때 흰색 배경에 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
				}
				*/
				int i = 0;
			}
		}
		else { // 풋일 경우
			/*
			if (!posi) { // 포지션이 없는 경우 - 주문이 나간적이 없을 경우
				if (acptCnt > 0) { // 접수확인된 주문이 있을 경우 - 0에 회색 배경
					QuickSetText(row, col, _T("0"));
					QuickSetBackColor(row, col, RGB(178, 186, 205));
				}
				else {
					if (init) { // 잔고 없이 한번이라도 주문이 나간경우 - 0에 흰색 배경
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
					else { // 잔고도 없고 한번도 주문이 나가지 않은 경우 - 숫자없이 매도 배경색
						QuickSetText(row, col, _T(""));
						QuickSetBackColor(row, col, RGB(218, 226, 245));
					}
				}
			}
			else { // 포지션이 있는 경우 - 주문이 나간적이 있는 경우 - 한번이라도 주문이 나갔다
				if (std::abs(posi->OpenQty) > 0) { // 잔고가 있는 경우
					QuickSetNumber(row, col, posi->OpenQty);
					if (acptCnt > 0) // 접수확인이 있는 경우 회색 배경
						QuickSetBackColor(row, col, RGB(178, 186, 205));
					else // 잔고만 있을 때는 흰색 배경
						QuickSetBackColor(row, col, RGB(255, 255, 255));
				}
				else { // 잔고가 없는 경우
					if (acptCnt > 0) { // 접수확인이 있는 경우 - 회색 배경에 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(178, 186, 205));
					}
					else { // 잔고, 접수확인 없을 때 흰색 배경에 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
				}
			}
			*/
		}
		InvalidateCellRect(row, col);
		_RemainPos.insert(std::make_pair(row, col));
	}
}

void SmOptionGrid::ShowExpected(symbol_p sym)
{
	if (!sym)
		return;
	CUGCell cell;
	/*
	auto it = _SymbolRowMap.find(sym->ShortCode);
	if (it != _SymbolRowMap.end()) {
		auto pos = _SymbolRowMap[sym->ShortCode];
		ShowExpected(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
	}
	*/
}

void SmOptionGrid::ShowExpected(int row, int col, symbol_p sym)
{
	CGridCellBase* pCell = GetCell(row, col);
	/*
	if (pCell) {
		int curValue = sym->Quote.intExpected;

		std::string strVal = fmt::format("{:.{}f}", curValue / std::pow(10, sym->Decimal), sym->Decimal);

		pCell->SetText(strVal.c_str());
		InvalidateCellRect(row, col);
	}
	*/
}

void SmOptionGrid::ShowCurrent(symbol_p sym)
{
	if (!sym)
		return;
	CUGCell cell;
	/*
	auto it = _SymbolRowMap.find(sym->ShortCode);
	if (it != _SymbolRowMap.end()) {
		auto pos = _SymbolRowMap[sym->ShortCode];
		ShowCurrent(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
	}
	*/
}

void SmOptionGrid::ShowCurrent(int row, int col, symbol_p sym)
{
	CGridCellBase* pCell = GetCell(row, col);
	/*
	if (pCell) {
		int curValue = sym->Quote.intClose;

		std::string strVal = fmt::format("{:.{}f}", curValue / std::pow(10, sym->Decimal), sym->Decimal);

		pCell->SetText(strVal.c_str());
		InvalidateCellRect(row, col);
	}
	*/
}

void SmOptionGrid::SetRemain(VtPosition* posi)
{
	if (!posi)
		return;

	if (_Mode != 0)
		return;

	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;
	/*
	auto it = _SymbolRowMap.find(posi->ShortCode);
	if (it != _SymbolRowMap.end())
	{
		auto pos = _SymbolRowMap[posi->ShortCode];
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (std::abs(posi->OpenQty) > 0) {
			QuickSetNumber(std::get<0>(pos), std::get<1>(pos), posi->OpenQty);
			if (acnt->GetAcceptedOrderCount(posi->ShortCode) > 0)
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(212, 186, 188));
			else
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
		}
		else {
			QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
			QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
		}
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));

		CString msg;
		msg.Format("SetRemain row = %d, col = %d\n", std::get<0>(pos), std::get<1>(pos));
		TRACE(msg);
	}
	*/
}

void SmOptionGrid::SetRemain(VtOrder* order)
{
	if (!order || _Mode != 0 || !_OrderConfigMgr)
		return;
	/*
	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtOrderManager* orderMgr = mainApp.TotalOrderMgr().FindAddAccountOrderManger(acnt->AccountNo);
		if (!orderMgr)
			return;
		
		VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(order->shortCode);
		if (!prdtOrderMgr)
			return;
		VtPosition* posi = acnt->FindPosition(order->shortCode);
		ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, order->shortCode);

	}
	else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;

		std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(order->shortCode);
		int count = 0;
		VtPosition posi = fund->GetPosition(order->shortCode, count);
		if (count != 0)
			ShowPosition(std::get<0>(result), std::get<1>(result), &posi, order->shortCode);
		else
			ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, order->shortCode);
	}
	*/
}

void SmOptionGrid::SetRemain(symbol_p sym)
{
	if (!sym || !_OrderConfigMgr)
		return;
	/*
	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtOrderManager* orderMgr = mainApp.TotalOrderMgr().FindAddAccountOrderManger(acnt->AccountNo);
		if (!orderMgr)
			return;
		VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(sym->ShortCode);
		if (!prdtOrderMgr)
			return;
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);
		ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, sym->ShortCode);
	}
	else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;

		std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(sym->ShortCode);
		int count = 0;
		VtPosition posi = fund->GetPosition(sym->ShortCode, count);
		if (count != 0)
			ShowPosition(std::get<0>(result), std::get<1>(result), &posi, sym->ShortCode);
		else
			ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, sym->ShortCode);
	}
	*/
}

void SmOptionGrid::SetRemain2()
{
	if (_Mode != 0 || !_OrderConfigMgr)
		return;
	/*
	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it) {
		auto pos = it->second;
		symbol_p sym = std::get<2>(pos);

		if (_OrderConfigMgr->Type() == 0) {
			VtAccount* acnt = _OrderConfigMgr->Account();
			if (!acnt)
				return;

			VtOrderManager* orderMgr = mainApp.TotalOrderMgr().FindAddAccountOrderManger(acnt->AccountNo);
			if (!orderMgr)
				return;
			VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(sym->ShortCode);
			VtPosition* posi = acnt->FindPosition(sym->ShortCode);
			if (prdtOrderMgr)
				ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, sym->ShortCode);
			else
				ShowPosition(false, 0, nullptr, sym->ShortCode);
		}
		else {
			VtFund* fund = _OrderConfigMgr->Fund();
			if (!fund)
				return;

			std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(sym->ShortCode);
			int count = 0;
			VtPosition posi = fund->GetPosition(sym->ShortCode, count);
			if (count != 0)
				ShowPosition(std::get<0>(result), std::get<1>(result), &posi, sym->ShortCode);
			else
				ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, sym->ShortCode);
		}
	}
	*/
}

void SmOptionGrid::SetCurrent2()
{
	if (_Mode != 1)
		return;

	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it) {
		auto pos = it->second;
		symbol_p sym = std::get<2>(pos);
		ShowCurrent(sym);
	}
}

void SmOptionGrid::SetExpected2()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;

	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it) {
		auto pos = it->second;
		QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));
	}
}

void SmOptionGrid::QuickSetBackColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetBackClr(color);
	}
}

void SmOptionGrid::QuickSetText(int row, int col, LPCTSTR text)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetText(text);
	}
}

void SmOptionGrid::QuickSetNumber(int row, int col, int number)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		CString text;
		text.Format("%d", number);
		pCell->SetText(text);
	}
}

void SmOptionGrid::QuickSetTextColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetTextClr(color);
	}
}

void SmOptionGrid::SetEqualRow(int equalRow)
{
	int maxRow = GetMaxRow();

	int centerRow = maxRow / 2;
}

int SmOptionGrid::GetMaxRow()
{
	int rowHeight = 21; // DefaultCellHeight;

	if (_OrderConfigMgr)
		rowHeight = _OrderConfigMgr->OrderCellHeight;


	RECT rect;
	GetClientRect(&rect);

	int count = (rect.bottom - rect.top) / rowHeight;

	return count;
}

void SmOptionGrid::MakeSymbolRowMap(int start_index, int max_row)
{
	
}

void SmOptionGrid::OnSymbolMaster(symbol_p sym)
{
	ShowCurrent(sym);
}

void SmOptionGrid::OnOrderFilled(VtOrder* order)
{
	if (!_OrderConfigMgr || !order)
		return;
	/*
	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtPosition* posi = acnt->FindPosition(order->shortCode);
		if (posi)
			SetRemain(posi);
	}
	else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		int count = 0;
		VtPosition posi = fund->GetPosition(order->shortCode, count);
		SetRemain(&posi);
	}
	*/
}

void SmOptionGrid::OnReceiveQuote(symbol_p sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 1) {
		ShowCurrent(sym);
	}
}

void SmOptionGrid::OnExpected(symbol_p sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 2) {
		ShowExpected(sym);
	}
}

void SmOptionGrid::on_update_quote()
{
	enable_show_ = true;
}
void SmOptionGrid::update_quote()
{
	if (!quote_control_) return;
	try {
		const DarkHorse::VmQuote& quote = quote_control_->get_quote();
		//CString msg;
		//msg.Format("SmOptionGrid::update_quote :: %s ::  close : %d\n", quote.symbol_code.c_str(), quote.close);
		//TRACE(msg);
		if (view_mode_ != ViewMode::VM_Close) return;
		update_close(quote);
	}
	catch (const std::out_of_range& e) {
		// Handling the exception
		const std::string error = e.what();
		LOGINFO(DarkHorse::CMyLogger::getInstance(), "Caught std::out_of_range exception: %s", error.c_str());
	}
	catch (const std::exception& e) {
		// Handling other exceptions derived from std::exception
		const std::string error = e.what();
		LOGINFO(DarkHorse::CMyLogger::getInstance(), "Caught exception: %s", error.c_str());
	}
	catch (...) {
		// Catch-all block for any other exceptions
		LOGINFO(DarkHorse::CMyLogger::getInstance(), "Caught unknown exception");
	}

}

std::string SmOptionGrid::get_position_text(const DarkHorse::VmOption& option_info)
{
	if (option_info.position != 0)
		return std::to_string(option_info.position);
	else if (option_info.accepted_count > 0 ||
		option_info.ordered)
		return "0";
	else return "";
}

void SmOptionGrid::get_option_position_info(DarkHorse::VmOption& option_info)
{
	get_init_accepted_order_count(option_info);
	if (order_type_ == DarkHorse::OrderType::None) return;
	if (order_type_ == DarkHorse::OrderType::MainAccount) {
		VmPosition position;
		mainApp.total_position_manager()->get_position_from_parent_account(option_info.account_no, option_info.symbol_code, position);
		option_info.position = position.open_quantity;
	}
	else if (order_type_ == DarkHorse::OrderType::SubAccount) {
		VmPosition position;
		mainApp.total_position_manager()->get_position_from_account(option_info.account_no, option_info.symbol_code, position);
		option_info.position = position.open_quantity;
	}
	else if (order_type_ == DarkHorse::OrderType::Fund) {
		VmPosition position;
		mainApp.total_position_manager()->get_position_from_fund(option_info.fund_name, option_info.symbol_code, position);
		option_info.position = position.open_quantity;
	}
}

void SmOptionGrid::get_init_accepted_order_count(DarkHorse::VmOption& option_info)
{
	if (order_type_ == DarkHorse::OrderType::None) return;
	if (order_type_ == DarkHorse::OrderType::MainAccount) {
		if (!_Account) return;
		option_info.account_no = _Account->No();
		option_info.fund_name = "";
		auto init_and_count = mainApp.total_order_manager()->get_init_and_acpt_order_count_from_parent_account(option_info.account_no, option_info.symbol_code);
		option_info.ordered = init_and_count.first;
		option_info.accepted_count = init_and_count.second;
	}
	else if (order_type_ == DarkHorse::OrderType::SubAccount) {
		if (!_Account) return;
		option_info.account_no = _Account->No();
		option_info.fund_name = "";
		auto init_and_count = mainApp.total_order_manager()->get_init_and_acpt_order_count_from_account(option_info.account_no, option_info.symbol_code);
		option_info.ordered = init_and_count.first;
		option_info.accepted_count = init_and_count.second;
	}
	else if (order_type_ == DarkHorse::OrderType::Fund) {
		if (!_Fund) return;
		option_info.fund_name = _Fund->Name();
		option_info.account_no = "";
		auto init_and_count = mainApp.total_order_manager()->get_init_and_acpt_order_count_from_fund(option_info.fund_name, option_info.symbol_code);
		option_info.ordered = init_and_count.first;
		option_info.accepted_count = init_and_count.second;
	}
}

void SmOptionGrid::set_call_put_area()
{
	for (int i = 1; i < _RowCount; i++) {
		QuickSetBackColor(i, 0, RGB(252, 226, 228));
		QuickSetBackColor(i, 1, RGB(255, 255, 255));
		QuickSetBackColor(i, 2, RGB(218, 226, 245));
	}
}

void SmOptionGrid::update_expected(std::shared_ptr<DarkHorse::SmQuote> quote)
{
	if (!quote || view_mode_ != ViewMode::VM_Expected) return;
	try {
		if (quote->symbol_code.empty()) {
			LOGINFO(DarkHorse::CMyLogger::getInstance(), "update_expected symbol code is empty");
			return;
		}
		const std::string option_code = quote->symbol_code.substr(1, quote->symbol_code.length() - 1);
		auto found = symbol_vector_index_map_.find(option_code);
		if (found == symbol_vector_index_map_.end()) return;
		if (quote->symbol_code.at(0) == '2') {
			DarkHorse::VmOption& option_info = call_symbol_vector_[found->second];
			option_info.expected = quote->expected;
			update_value_cell(quote->symbol_id, option_info);
		}
		else {
			DarkHorse::VmOption& option_info = put_symbol_vector_[found->second];
			option_info.expected = quote->expected;
			update_value_cell(quote->symbol_id, option_info);
		}
		enable_show_ = true;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(DarkHorse::CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmOptionGrid::update_close(const DarkHorse::VmQuote& quote)
{
	try {
		//LOGINFO(CMyLogger::getInstance(), "update_close symbol code: %s", quote.symbol_code.c_str());
		if (quote.symbol_code.empty()) {
			//LOGINFO(CMyLogger::getInstance(), "update_close symbol code is empty");
			return;
		}
		const std::string option_code = quote.symbol_code.substr(1, quote.symbol_code.length() - 1);
		auto found = symbol_vector_index_map_.find(option_code);
		if (found == symbol_vector_index_map_.end()) return;
		if (quote.symbol_code.at(0) == '2') {
			DarkHorse::VmOption& option_info = call_symbol_vector_[found->second];
			option_info.close = quote.close;
			update_value_cell(quote.symbol_id, option_info);
		}
		else {
			DarkHorse::VmOption& option_info = put_symbol_vector_[found->second];
			option_info.close = quote.close;
			update_value_cell(quote.symbol_id, option_info);
		}
	}
	catch (const std::out_of_range& e) {
		// Handling the exception
		const std::string error = e.what();
		LOGINFO(DarkHorse::CMyLogger::getInstance(), "Caught std::out_of_range exception: %s", error.c_str());
	}
	catch (const std::exception& e) {
		// Handling other exceptions derived from std::exception
		const std::string error = e.what();
		LOGINFO(DarkHorse::CMyLogger::getInstance(), "Caught exception: %s", error.c_str());
	}
	catch (...) {
		// Catch-all block for any other exceptions
		LOGINFO(DarkHorse::CMyLogger::getInstance(), "Caught unknown exception");
	}
}

void SmOptionGrid::update_position()
{
	if (!position_control_) return;
	try {
		const VmPosition& position = position_control_->get_position();
		if (position.symbol_code.empty()) return;

		const std::string option_code = position.symbol_code.substr(1, position.symbol_code.length() - 1);
		auto found = symbol_vector_index_map_.find(option_code);
		if (found == symbol_vector_index_map_.end()) return;
		if (position.symbol_code.at(0) == '2') {
			DarkHorse::VmOption& option_info = call_symbol_vector_[found->second];
			option_info.position = position.open_quantity;
			update_value_cell(position.symbol_id, option_info);
		}
		else {
			DarkHorse::VmOption& option_info = put_symbol_vector_[found->second];
			option_info.position = position.open_quantity;
			update_value_cell(position.symbol_id, option_info);
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(DarkHorse::CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmOptionGrid::on_update_position_vm(const VmPosition& position)
{
	if (order_type_ != position.position_type) return;
	try {
		if (position.symbol_code.empty()) return;

		const std::string option_code = position.symbol_code.substr(1, position.symbol_code.length() - 1);
		auto found = symbol_vector_index_map_.find(option_code);
		if (found == symbol_vector_index_map_.end()) return;
		if (position.symbol_code.at(0) == '2') {
			DarkHorse::VmOption& option_info = call_symbol_vector_[found->second];
			option_info.position = position.open_quantity;
			update_value_cell(position.symbol_id, option_info);
		}
		else {
			DarkHorse::VmOption& option_info = put_symbol_vector_[found->second];
			option_info.position = position.open_quantity;
			update_value_cell(position.symbol_id, option_info);
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(DarkHorse::CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmOptionGrid::update_value_cell(const int symbol_id, const DarkHorse::VmOption& option_info)
{
	auto found = row_col_map_.find(symbol_id);
	if (found == row_col_map_.end()) return;
	show_value(found->second.first, found->second.second, option_info);
}

void SmOptionGrid::set_option_view(
	const int option_market_index,
	const std::string& year_month_name)
{
	if (year_month_name.empty()) {
		ClearAllText();
		return;
	}
	set_option_info(option_market_index, year_month_name);
	make_symbol_vec(true);
	make_symbol_vec(false);
	register_symbols(option_market_index);
	InitGrid();
	init_strike_index();
	set_option_view();
}

void SmOptionGrid::set_option_view()
{
	set_call_put_area();
	ClearAllText();
	showValues();
	//set_strike();
	//show_values();
	//register_symbols_to_server();
	Invalidate();
}

void SmOptionGrid::update_order(order_p order, DarkHorse::OrderEvent order_event)
{
	try {
		const std::string symbol_code = order->symbol_code;
		auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
		if (symbol == nullptr) return;

		const std::string option_code = symbol_code.substr(1, symbol_code.length() - 1);
		auto found = symbol_vector_index_map_.find(option_code);
		if (found == symbol_vector_index_map_.end()) return;
		if (symbol_code.at(0) == '2') {
			DarkHorse::VmOption& option_info = call_symbol_vector_[found->second];
			get_init_accepted_order_count(option_info);
			update_value_cell(symbol->Id(), option_info);
		}
		else {
			DarkHorse::VmOption& option_info = put_symbol_vector_[found->second];
			get_init_accepted_order_count(option_info);
			update_value_cell(symbol->Id(), option_info);
		}

		enable_show_ = true;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(DarkHorse::CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmOptionGrid::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	_Fund = val;
	order_type_ = DarkHorse::OrderType::Fund;
	position_control_->set_fund(_Fund);
	for (auto& option_info : call_symbol_vector_) {
		get_option_position_info(option_info);
		update_value_cell(option_info.symbol_id, option_info);
	}
	for (auto& option_info : put_symbol_vector_) {
		get_option_position_info(option_info);
		update_value_cell(option_info.symbol_id, option_info);
	}
	enable_show_ = true;
}

void SmOptionGrid::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	_Account = val;
	position_control_->set_account(_Account);
	if (_Account->is_subaccount())
		order_type_ = DarkHorse::OrderType::SubAccount;
	else
		order_type_ = DarkHorse::OrderType::MainAccount;

	for (auto& option_info : call_symbol_vector_) {
		get_option_position_info(option_info);
		update_value_cell(option_info.symbol_id, option_info);
	}
	for (auto& option_info : put_symbol_vector_) {
		get_option_position_info(option_info);
		update_value_cell(option_info.symbol_id, option_info);
	}
	enable_show_ = true;
}

void SmOptionGrid::UpdateSymbolInfo()
{
	if (!_Symbol) return;

// 	std::shared_ptr<DarkHorse::SmCell> cell = _Grid->FindCell(1, 0);
// 	if (cell) cell->Text(_Symbol->SymbolCode());
// 	cell = _Grid->FindCell(1, 4);
// 	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}

// void SmOptionGrid::UpdateAssetInfo()
// {
// 	_Mode == 0 ? UpdateAccountAssetInfo() : UpdateFundAssetInfo();
// }

void SmOptionGrid::OnQuoteEvent(const std::string& symbol_code)
{

}

void SmOptionGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	enable_show_ = true;
}

void SmOptionGrid::set_view_mode(ViewMode view_mode)
{
	view_mode_ = view_mode;
	show_values();
	Invalidate();
}

void SmOptionGrid::set_strike_start_index(const int distance)
{
	const int oldStartIndex = _startIndex;
	if (distance < 0)
		_startIndex += 1;
	else
		_startIndex -= 1;
	//_startIndex += distance;
	if (_startIndex < 0)
		_startIndex = 0;
	if (_maxSymbol <= _maxRow)
		_startIndex = 0;
	if (_startIndex + _maxRow - 1 > _maxSymbol)
		_startIndex = oldStartIndex;
}

void SmOptionGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	CCellID cell = GetCellFromPt(point);
	CGridCellBase* pCell = GetCell(cell.row, cell.col);
	
	auto cell_pos = std::make_pair(cell.row, cell.col);
	auto found = symbol_map_.find(cell_pos);
	if (found == symbol_map_.end()) return;
	if (_OrderConfigMgr->_HdCenterWnd) {
		_OrderConfigMgr->_HdCenterWnd->ChangeSymbol(found->second);
	}
}

void SmOptionGrid::register_symbols(const int option_market_index)
{
	auto found = registered_map_.find(option_market_index_);
	if (found != registered_map_.end()) return;

	for (size_t i = 0; i < call_symbol_vector_.size(); i++) {
		std::string symbol_code = call_symbol_vector_[i].symbol_p->SymbolCode();
		register_symbol(symbol_code);
		symbol_code = put_symbol_vector_[i].symbol_p->SymbolCode();
		register_symbol(symbol_code);
	}
	registered_map_[option_market_index] = option_market_index;
}

void SmOptionGrid::register_symbol(const std::string symbol_code)
{
	DhTaskArg arg;
	arg.detail_task_description = symbol_code;
	arg.task_type = DhTaskType::RegisterSymbol;
	arg.parameter_map["symbol_code"] = symbol_code;
	mainApp.TaskReqMgr()->AddTask(std::move(arg));
}

void SmOptionGrid::show_value(const int row, const int col, const DarkHorse::VmOption& option_info)
{
	//auto cell = FindCell(row, col);
	//if (!cell) return;

	std::string value;
	if (view_mode_ == ViewMode::VM_Close) {
		value = std::to_string(option_info.close);
		DarkHorse::SmUtil::insert_decimal(value, option_info.decimal);
	}
	else if (view_mode_ == ViewMode::VM_Expected) {
		value = std::to_string(option_info.expected);
		DarkHorse::SmUtil::insert_decimal(value, option_info.decimal);
	}
	else {
		value = get_position_text(option_info);
	}
	//set_background_color(cell, option_info);
	//cell->Text(value);
	QuickSetText(row, col, value.c_str());
}

// set the background color of the cell according to the order status
void SmOptionGrid::set_background_color(std::shared_ptr<DarkHorse::SmCell> cell, const DarkHorse::VmOption& option_info)
{
// 	if (option_info.accepted_count > 0)
// 		cell->CellType(CT_OE);
// 	else if (option_info.position != 0)
// 		cell->CellType(CT_PE);
// 	else if (option_info.ordered)
// 		cell->CellType(CT_EE);
// 	else if (option_info.call_put == 1)
// 		cell->CellType(CT_CD);
// 	else
// 		cell->CellType(CT_PD);
}

void SmOptionGrid::set_background_color(const int row, const int col, const DarkHorse::VmOption& option_info)
{
	if (option_info.accepted_count > 0)
		QuickSetBackColor(row, col, RGB(212, 186, 188));
	else if (option_info.position != 0) {
		if (option_info.accepted_count > 0)
			QuickSetBackColor(row, col, RGB(212, 186, 188));
		else
			QuickSetBackColor(row, col, RGB(255, 255, 255));
	}
	else if (option_info.ordered)
		QuickSetBackColor(row, col, RGB(255, 255, 255));
	else if (option_info.call_put == 1)
		QuickSetBackColor(row, col, RGB(252, 226, 228));
	else
		QuickSetBackColor(row, col, RGB(218, 226, 245));
}

void SmOptionGrid::show_strike(const int row, const int col, const DarkHorse::VmOption& option_info)
{
	if (option_info.atm == 1)
		QuickSetBackColor(row, col, RGB(255, 0, 0));
	else
		QuickSetBackColor(row, col, RGB(255, 255, 255));

	QuickSetText(row, col, option_info.strike.c_str());
}

void SmOptionGrid::show_values()
{
	if (call_symbol_vector_.empty() || put_symbol_vector_.empty()) return;

	for (int i = 1; i < _maxRow; i++) {
		int new_strike_index = strike_start_index_ + i - 1;
		const int vec_size = static_cast<int>(call_symbol_vector_.size());
		if (new_strike_index >= vec_size)
			new_strike_index = vec_size - 1;
		if (new_strike_index < 0)
			new_strike_index = 0;
		const DarkHorse::VmOption& call_info = call_symbol_vector_[new_strike_index];
		const DarkHorse::VmOption& put_info = put_symbol_vector_[new_strike_index];
		show_value(i, 0, call_info);
		show_value(i, 2, put_info);
	}
}

void SmOptionGrid::showValues()
{
	if (call_symbol_vector_.empty() || put_symbol_vector_.empty()) return;

	symbol_map_.clear();
	row_col_map_.clear();
	for (int i = 1; i <= _maxRow; i++) {
		int newStartIndex = _startIndex + i - 1;
		if (newStartIndex < 0) newStartIndex = 0;
		if (newStartIndex >= _maxSymbol)
			break;

		const DarkHorse::VmOption& call_info = call_symbol_vector_[newStartIndex];
		const DarkHorse::VmOption& put_info = put_symbol_vector_[newStartIndex];
		show_value(i, 0, call_info);
		show_value(i, 2, put_info);
		show_strike(i, 1, call_symbol_vector_[newStartIndex]);

		auto call_symbol = call_symbol_vector_[newStartIndex].symbol_p;
		auto put_symbol = put_symbol_vector_[newStartIndex].symbol_p;
		symbol_map_[std::make_pair(i, 0)] = call_symbol;
		symbol_map_[std::make_pair(i, 2)] = put_symbol;
		row_col_map_[call_symbol->Id()] = std::make_pair(i, 0);
		row_col_map_[put_symbol->Id()] = std::make_pair(i, 2);
	}
}

void SmOptionGrid::set_option_info(const int option_market_index, const std::string& year_month_name)
{
	option_market_index_ = option_market_index;
	year_month_name_ = year_month_name;
}

void SmOptionGrid::set_option_info(
	const size_t index,
	DarkHorse::VmOption& option_info,
	std::shared_ptr<DarkHorse::SmSymbol> symbol,
	std::shared_ptr<DarkHorse::SmQuote> quote)
{
	if (!symbol || !quote) return;

	option_info.strike = symbol->Strike();
	option_info.atm = symbol->AtmType();
	option_info.decimal = symbol->decimal();
	if (option_info.atm == 1) atm_index_ = index;
	option_info.close = quote->close;
	option_info.expected = quote->expected;
	option_info.ordered = false;
	option_info.position = 0;
	option_info.symbol_id = symbol->Id();
	option_info.symbol_p = symbol;
	option_info.symbol_code = symbol->SymbolCode();
}

void SmOptionGrid::make_symbol_vec(bool call_side)
{
	std::vector<DarkHorse::DmOption>& option_vec = mainApp.SymMgr()->get_dm_option_vec();
	if (option_market_index_ < 0 || option_market_index_ >= static_cast<const int>(option_vec.size())) return;

	const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map =
		call_side ?
		option_vec[option_market_index_].call_product->get_yearmonth_map() :
		option_vec[option_market_index_].put_product->get_yearmonth_map();
	auto it = year_month_map.find(year_month_name_);
	if (it == year_month_map.end()) return;
	if (call_side) call_symbol_vector_.clear();
	else put_symbol_vector_.clear();
	symbol_vector_index_map_.clear();
	const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec = it->second->get_symbol_vector();
	for (size_t i = 0; i < symbol_vec.size(); i++) {
		auto symbol = symbol_vec[i];
		auto quote = mainApp.QuoteMgr()->get_quote(symbol->SymbolCode());
		DarkHorse::VmOption option_info;
		set_option_info(i, option_info, symbol, quote);
		const std::string& symbol_code = symbol->SymbolCode();
		const std::string option_code = symbol_code.substr(1, symbol_code.length() - 1);
		symbol_vector_index_map_[option_code] = i;
		option_info.call_put = call_side ? 1 : 2;
		get_option_position_info(option_info);
		if (call_side) call_symbol_vector_.push_back(option_info);
		else put_symbol_vector_.push_back(option_info);
	}
}

void SmOptionGrid::init_strike_index()
{
	strike_start_index_ = calcStartIndex();
}



void SmOptionGrid::register_symbols_to_server()
{
	// 	for(auto& symbol : call_symbol_vector_)
	// 		mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
	// 	for (auto& symbol : put_symbol_vector_)
	// 		mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
}

void SmOptionGrid::set_strike()
{
	CString msg;
	msg.Format("set_strike begin\n");
	//TRACE(msg);
	if (call_symbol_vector_.empty() || put_symbol_vector_.empty()) return;

	symbol_map_.clear();
	row_col_map_.clear();
	for (int i = 1; i < _maxRow; i++) {
		int new_strike_index = strike_start_index_ + i - 1;
		const int vec_size = static_cast<int>(call_symbol_vector_.size());
		if (new_strike_index >= vec_size || new_strike_index < 0) {
			break;
		}

		show_strike(i, 1, call_symbol_vector_[new_strike_index]);

		auto call_symbol = call_symbol_vector_[new_strike_index].symbol_p;
		auto put_symbol = put_symbol_vector_[new_strike_index].symbol_p;
		symbol_map_[std::make_pair(i, 0)] = call_symbol;
		symbol_map_[std::make_pair(i, 2)] = put_symbol;
		row_col_map_[call_symbol->Id()] = std::make_pair(i, 0);
		row_col_map_[put_symbol->Id()] = std::make_pair(i, 2);
	}
	msg.Format("set_strike end\n");
	//TRACE(msg);
}

