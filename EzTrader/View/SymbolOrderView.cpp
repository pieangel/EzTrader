//#include "stdafx.h"
//#include "SymbolOrderView.h"

#include "stdafx.h"
#include "SymbolOrderView.h"
#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmOrderGridConst.h"
#include <math.h>
#include "../Log/MyLogger.h"
#include "../SmGrid/SmCell.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Order/SmStopOrderManager.h"
#include "../SmGrid/SmCellType.h"
#include "../Order/OrderUi/DmAccountOrderCenterWindow.h"
#include <windows.h>
#include "../Global/SmTotalManager.h"
#include "../Client/ViStockClient.h"
#include "../Account/SmAccount.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Order/SmOrder.h"
#include "../Order/SmOrderViewer.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/OrderUi/DmAccountOrderWindow.h"
#include "../Util/SmNumberFunc.h"
#include "../Event/SmCallbackManager.h"
#include "../Order/SmOrderRequest.h"
#include "../Quote/SmQuoteManager.h"
#include "../Quote/SmQuote.h"
#include "../Hoga/SmHoga.h"
#include "../Hoga/SmHogaManager.h"
#include "../Controller/QuoteControl.h"
#include "../Controller/HogaControl.h"
#include "../Controller/ProductControl.h"
#include "../ViewModel/VmQuote.h"
#include "../ViewModel/VmHoga.h"
#include "../ViewModel/VmProduct.h"
#include "../Event/EventHub.h"
#include <sstream>
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

constexpr int Round(float x) { return static_cast<int>(x + 0.5f); }

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SymbolOrderView, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_SYMBOL_MASTER_RECEIVED, &SymbolOrderView::OnWmSymbolMasterReceived)
	ON_MESSAGE(WM_QUOTE_RECEIVED, &SymbolOrderView::OnWmQuoteReceived)
	ON_MESSAGE(WM_HOGA_RECEIVED, &SymbolOrderView::OnWmHogaReceived)
	ON_MESSAGE(WM_ORDER_CHANGED, &SymbolOrderView::OnUmOrderChanged)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL SymbolOrderView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (_FixedMode) return TRUE;

	// TODO: Add your message handler code here and/or call default
	int distance = zDelta / 60;
	if (abs(zDelta) > 120)
		distance = zDelta / 120;
	else
		distance = zDelta / 40;
	CString msg;
	msg.Format("OnMouseWheel delta = %d\n", zDelta);
	//TRACE(msg);

	ClearOldQuote();
	ClearOldHoga();
	ClearOrders();
	ClearStopOrders();

	increase_close_row(distance);

	SetCenterValues();

	update_quote();
	update_hoga();
	SetOrder();
	SetStopOrder();

	//Invalidate();

	return CBCGPStatic::OnMouseWheel(nFlags, zDelta, pt);
}

void SymbolOrderView::Clear()
{
	ClearOldQuote();
	ClearOldHoga();
	ClearOrders();

	for (int row = 2; row < _Grid->RowCount() - 1; row++)
		for (int col = 0; col < _Grid->ColCount(); col++) {
			auto cell = _Grid->FindCell(row, col);
			if (cell) cell->Text("");
		}
	Invalidate();
}

SymbolOrderView::SymbolOrderView()
{
	_BuyStopOrderMgr = std::make_shared<DarkHorse::SmStopOrderManager>();
	_SellStopOrderMgr = std::make_shared<DarkHorse::SmStopOrderManager>();
	//_BuyOrderViewer = std::make_shared<SmOrderViewer>();
	//_SellOrderViewer = std::make_shared<SmOrderViewer>();
	hoga_control_ = std::make_shared<DarkHorse::HogaControl>();
	hoga_control_->symbol_order_view(this);
	quote_control_ = std::make_shared<DarkHorse::QuoteControl>();
	quote_control_->symbol_order_view(this);
	product_control_ = std::make_shared<DarkHorse::ProductControl>();
	m_pGM = CBCGPGraphicsManager::CreateInstance();

}

SymbolOrderView::~SymbolOrderView()
{
	KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeOrderWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->UnsubscribeSymbolMasterWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->UnsubscribeQuoteWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->UnsubscribeHogaWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeHogaCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeMasterCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}
void SymbolOrderView::update_quote()
{
	if (!quote_control_ || !product_control_) return;
	const VmQuote quote = quote_control_->get_quote();
	SetCenterValues();
	set_quote_value(quote.close, SmCellType::CT_QUOTE_CLOSE);
	set_quote_value(quote.open, SmCellType::CT_QUOTE_OPEN);
	set_quote_value(quote.high, SmCellType::CT_QUOTE_HIGH);
	set_quote_value(quote.low, SmCellType::CT_QUOTE_LOW);
	SetPosition();
	SetQuoteColor();
	Invalidate();
}

void SymbolOrderView::update_hoga()
{
	if (!hoga_control_ || price_to_row_.empty()) return;

	const VmHoga hoga = hoga_control_->get_hoga();

	std::shared_ptr<SmCell> pCell = nullptr;

	for (int i = 0; i < 5; i++) {
		int row_index = FindRow(hoga.Ary[i].SellPrice);
		pCell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::SELL_CNT);
		if (pCell && row_index > 1 && row_index < price_end_row_) {
			pCell->Text(std::to_string(hoga.Ary[i].SellCnt));
			pCell->CellType(SmCellType::CT_HOGA_SELL);
			_OldHogaSellRowIndex.insert(row_index);
		}
		pCell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::SELL_QTY);
		if (pCell && row_index > 1 && row_index < price_end_row_) {
			pCell->CellType(SmCellType::CT_HOGA_SELL);
			pCell->Text(std::to_string(hoga.Ary[i].SellQty));
		}
		row_index = FindRow(hoga.Ary[i].BuyPrice);
		pCell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::BUY_QTY);
		if (pCell && row_index > 1 && row_index < price_end_row_) {
			pCell->CellType(SmCellType::CT_HOGA_BUY);
			pCell->Text(std::to_string(hoga.Ary[i].BuyQty));
			_OldHogaBuyRowIndex.insert(row_index);
		}
		pCell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::BUY_CNT);
		if (pCell && row_index > 1 && row_index < price_end_row_) {
			pCell->CellType(SmCellType::CT_HOGA_BUY);
			pCell->Text(std::to_string(hoga.Ary[i].BuyCnt));
		}
	}

	pCell = _Grid->FindCell(price_end_row_, DarkHorse::OrderGridHeader::SELL_CNT);
	if (pCell) pCell->Text(std::to_string(hoga.TotSellCnt));
	pCell = _Grid->FindCell(price_end_row_, DarkHorse::OrderGridHeader::SELL_QTY);
	if (pCell) pCell->Text(std::to_string(hoga.TotSellQty));
	pCell = _Grid->FindCell(price_end_row_, DarkHorse::OrderGridHeader::BUY_CNT);
	if (pCell) pCell->Text(std::to_string(hoga.TotBuyCnt));
	pCell = _Grid->FindCell(price_end_row_, DarkHorse::OrderGridHeader::BUY_QTY);
	if (pCell) pCell->Text(std::to_string(hoga.TotBuyQty));

	const int delta_hoga = hoga.TotBuyQty - hoga.TotSellQty;
	pCell = _Grid->FindCell(price_end_row_, DarkHorse::OrderGridHeader::QUOTE);
	if (pCell) pCell->Text(std::to_string(delta_hoga));

	_TotalHogaMap.insert(std::make_pair(price_end_row_, DarkHorse::OrderGridHeader::SELL_CNT));
	_TotalHogaMap.insert(std::make_pair(price_end_row_, DarkHorse::OrderGridHeader::SELL_QTY));
	_TotalHogaMap.insert(std::make_pair(price_end_row_, DarkHorse::OrderGridHeader::QUOTE));
	_TotalHogaMap.insert(std::make_pair(price_end_row_, DarkHorse::OrderGridHeader::BUY_CNT));
	_TotalHogaMap.insert(std::make_pair(price_end_row_, DarkHorse::OrderGridHeader::BUY_QTY));

	Invalidate(FALSE);
}

void SymbolOrderView::SetPosition()
{
	if (!_Account || !_Symbol) return;

	std::shared_ptr<SmPosition> position = mainApp.TotalPosiMgr()->FindAddPosition(_Account->No(), _Symbol->SymbolCode());
	if (position->OpenQty == 0) return;
	int avg_price = static_cast<int>(position->AvgPrice);
	const int int_tick_size = static_cast<int>(_Symbol->TickSize() * pow(10, _Symbol->Decimal()));
	const int mod = avg_price % int_tick_size;
	const int half_tick_size = (int)(int_tick_size / 2);
	// 나머지 보다 작으면 빼주고 아니면 틱크기에서 나머지를 뺀 값을 더해 준다.
	if (mod < half_tick_size) avg_price = avg_price - avg_price % int_tick_size;
	else avg_price += (int_tick_size - mod);

	const int position_price_row = FindRow(avg_price);

	std::shared_ptr<SmCell> cell = _Grid->FindCell(position_price_row, DarkHorse::OrderGridHeader::QUOTE);
	if (position_price_row >= 2 && cell) {
		if (position->OpenQty > 0)
			cell->CellType(SmCellType::CT_POSITION_BUY);
		else if (position->OpenQty < 0)
			cell->CellType(SmCellType::CT_POSITION_SELL);
		cell_to_price.insert(std::make_pair(cell->Row(), cell->Col()));
	}
}

void SymbolOrderView::SetStopOrder()
{
	if (!_Account || !_Symbol) return;

	_StopRectVector.clear();

	const auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());

	const std::map<int, std::shared_ptr<SmOrderRequest>>& sell_order_req_map = symbol_order_mgr->SellStopOrderMgr()->GetOrderReqMap();

	int sell_stop_count = 0, buy_stop_count = 0;

	for (auto it = sell_order_req_map.begin(); it != sell_order_req_map.end(); ++it) {
		int row_index = FindRow(it->second->OrderPrice);
		sell_stop_count += it->second->OrderAmount;
		std::shared_ptr<SmCell> pCell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::SELL_STOP);
		if (pCell && row_index > 1 && row_index < price_end_row_) {
			pCell->AddOrderReq(it->second);
			pCell->Text(std::to_string(pCell->GetOrderReqCount()));
			_OldStopSellRowIndex.insert(row_index);

			std::shared_ptr<SmCell> pOrderCell = _Grid->FindCell(row_index + it->second->CutSlip, DarkHorse::OrderGridHeader::SELL_ORDER);
			if (pOrderCell && pOrderCell->Row() > 1 && pOrderCell->Row() < price_end_row_) {
				_StopRectVector.push_back(std::make_pair(pCell->GetCellRect(), pOrderCell->GetCellRect()));
			}
		}
	}


	std::shared_ptr<SmCell> order_cell = _Grid->FindCell(1, DarkHorse::OrderGridHeader::SELL_STOP);
	if (order_cell) order_cell->Text(std::to_string(sell_stop_count));


	const std::map<int, std::shared_ptr<SmOrderRequest>>& buy_order_req_map = symbol_order_mgr->BuyStopOrderMgr()->GetOrderReqMap();
	for (auto it = buy_order_req_map.begin(); it != buy_order_req_map.end(); ++it) {
		int row_index = FindRow(it->second->OrderPrice);
		buy_stop_count += it->second->OrderAmount;
		std::shared_ptr<SmCell> pCell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::BUY_STOP);
		if (pCell && row_index > 1 && row_index < price_end_row_) {
			pCell->AddOrderReq(it->second);
			pCell->Text(std::to_string(pCell->GetOrderReqCount()));
			_OldStopBuyRowIndex.insert(row_index);

			std::shared_ptr<SmCell> pOrderCell = _Grid->FindCell(row_index - it->second->CutSlip, DarkHorse::OrderGridHeader::BUY_ORDER);
			if (pOrderCell && pOrderCell->Row() > 1 && pOrderCell->Row() < price_end_row_) {
				_StopRectVector.push_back(std::make_pair(pCell->GetCellRect(), pOrderCell->GetCellRect()));
			}
		}
	}

	order_cell = _Grid->FindCell(1, DarkHorse::OrderGridHeader::BUY_STOP);
	if (order_cell) order_cell->Text(std::to_string(buy_stop_count));
}

void SymbolOrderView::SetOrder()
{
	if (!_Account || !_Symbol) return;
	if (_MovingOrder) return;

	auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());
	int buy_count = 0, sell_count = 0;
	std::shared_ptr<SmCell> cell = nullptr;
	const std::map<std::string, std::shared_ptr<SmOrder>>& accepted_order_map = symbol_order_mgr->GetAcceptedOrders();
	for (auto it = accepted_order_map.begin(); it != accepted_order_map.end(); ++it) {
		auto order = it->second;

		if (order->PositionType == SmPositionType::Buy) buy_count++;
		else sell_count++;

		int row_index = FindRow(order->OrderPrice);
		if (row_index > 1 && row_index < price_end_row_) {
			if (order->PositionType == SmPositionType::Buy) {
				cell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::BUY_ORDER);
				if (cell) {
					cell->AddOrder(order);
					cell->Text(std::to_string(cell->GetOrderCount()));
					_OldOrderBuyRowIndex.insert(row_index);
				}
				//buy_count++;
			}
			else {
				cell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::SELL_ORDER);
				if (cell) {
					cell->AddOrder(order);
					cell->Text(std::to_string(cell->GetOrderCount()));
					_OldOrderSellRowIndex.insert(row_index);
				}
				//sell_count++;
			}
		}

	}

	cell = _Grid->FindCell(1, DarkHorse::OrderGridHeader::BUY_ORDER);
	if (cell) cell->Text(std::to_string(buy_count));
	cell = _Grid->FindCell(1, DarkHorse::OrderGridHeader::SELL_ORDER);
	if (cell) cell->Text(std::to_string(sell_count));
}

void SymbolOrderView::ClearOldHoga(DarkHorse::Hoga_Type hoga_type) const noexcept
{
	std::shared_ptr<SmCell> pCell = nullptr;
	if (hoga_type == Hoga_Type::SELL) {
		for (auto it = _OldHogaSellRowIndex.begin(); it != _OldHogaSellRowIndex.end(); ++it) {
			const int& row_index = *it;
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::SELL_CNT);
			if (pCell) {
				pCell->Text(""); pCell->CellType(SmCellType::CT_NORMAL);
			}
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::SELL_QTY);
			if (pCell) {
				pCell->Text(""); pCell->CellType(SmCellType::CT_NORMAL);
			}
		}
	}
	else {
		for (auto it = _OldHogaBuyRowIndex.begin(); it != _OldHogaBuyRowIndex.end(); ++it) {
			const int& row_index = *it;
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::BUY_QTY);
			if (pCell) {
				pCell->Text(""); pCell->CellType(SmCellType::CT_NORMAL);
			}
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::BUY_CNT);
			if (pCell) {
				pCell->Text(""); pCell->CellType(SmCellType::CT_NORMAL);
			}
		}
	}
}

void SymbolOrderView::ClearOldHoga()
{
	// 이전 호가를 지운다.
	ClearOldHoga(Hoga_Type::SELL);
	ClearOldHoga(Hoga_Type::BUY);
	_OldHogaSellRowIndex.clear();
	_OldHogaBuyRowIndex.clear();
	std::shared_ptr<SmCell> pCell = nullptr;
	for (auto it = _TotalHogaMap.begin(); it != _TotalHogaMap.end(); ++it) {
		pCell = _Grid->FindCell(it->first, it->second);
		if (pCell) {
			pCell->Text(""); pCell->CellType(SmCellType::CT_NORMAL);
		}
	}
	_TotalHogaMap.clear();
}

void SymbolOrderView::ClearOldQuote()
{
	std::shared_ptr<SmCell> pCell = nullptr;
	for (auto it = cell_to_price.begin(); it != cell_to_price.end(); ++it) {
		pCell = _Grid->FindCell(it->first, it->second);
		if (pCell) {
			pCell->CellType(SmCellType::CT_NORMAL);
		}
	}
	cell_to_price.clear();
}

void SymbolOrderView::ClearStopOrders()
{
	std::shared_ptr<SmCell> pCell = nullptr;
	for (auto it = _OldStopSellRowIndex.begin(); it != _OldStopSellRowIndex.end(); ++it) {
		pCell = _Grid->FindCell(*it, DarkHorse::OrderGridHeader::SELL_STOP);
		if (pCell) {
			pCell->ClearOrderReq();
			pCell->Text("");
		}
	}
	_OldStopSellRowIndex.clear();

	for (auto it = _OldStopBuyRowIndex.begin(); it != _OldStopBuyRowIndex.end(); ++it) {
		pCell = _Grid->FindCell(*it, DarkHorse::OrderGridHeader::BUY_STOP);
		if (pCell) {
			pCell->ClearOrderReq();
			pCell->Text("");
		}
	}
	_OldStopBuyRowIndex.clear();
}

void SymbolOrderView::ClearOrders()
{
	if (_MovingOrder) return;

	std::shared_ptr<SmCell> pCell = nullptr;
	for (auto it = _OldOrderSellRowIndex.begin(); it != _OldOrderSellRowIndex.end(); ++it) {
		pCell = _Grid->FindCell(*it, DarkHorse::OrderGridHeader::SELL_ORDER);
		if (pCell) {
			pCell->ClearOrder();
			pCell->Text("");
		}
	}
	_OldOrderSellRowIndex.clear();

	for (auto it = _OldOrderBuyRowIndex.begin(); it != _OldOrderBuyRowIndex.end(); ++it) {
		pCell = _Grid->FindCell(*it, DarkHorse::OrderGridHeader::BUY_ORDER);
		if (pCell) {
			pCell->ClearOrder();
			pCell->Text("");
		}
	}
	_OldOrderBuyRowIndex.clear();
}

int SymbolOrderView::FindRow(const int& value) const noexcept
{
	int result = -1;
	auto it = price_to_row_.find(value);
	if (it != price_to_row_.end())
		return it->second;
	else
		return -1;
}

int SymbolOrderView::FindRowFromCenterValue(const int& value)
{
	if (!product_control_) return -1;
	if (!quote_control_) return -1;
	if (price_to_row_.empty() || value == 0) return -1;

	const int int_tick_size = product_control_->get_product().int_tick_size;
	if (int_tick_size == 0) return -1;
	auto it = price_to_row_.find(value);
	if (it != price_to_row_.end()) { // 값이 보이는 범위 안에 있을 때
		return it->second;
	}
	else { // 값이 보이는 범위 밖에 있을 때
		auto itr = price_to_row_.rbegin();
		int big_val = itr->first;
		int big_row = itr->second;


		int delta = big_val - value;
		int delta_row = delta / int_tick_size;

		int target_row = big_row + delta_row;
		return target_row;
	}
}

void SymbolOrderView::set_quote_value(const int value, const DarkHorse::SmCellType cell_type)
{
	const int row = FindRow(value);
	if (row < 2) return;
	std::shared_ptr<SmCell> cell = _Grid->FindCell(row, DarkHorse::OrderGridHeader::QUOTE);
	if (!cell) return;

	const int symbol_decimal = product_control_->get_product().decimal;
	std::string value_string;
	cell->CellType(cell_type);
	value_string = std::format("{0}", value);
	insert_decimal(value_string, symbol_decimal);
	cell->Text(value_string);
	cell_to_price.insert(std::make_pair(cell->Row(), cell->Col()));
}

void SymbolOrderView::set_quote_cell(const int row, const bool show_mark, const int mark_type)
{
	auto cell = _Grid->FindCell(row, DarkHorse::OrderGridHeader::QUOTE);
	if (cell) { cell->ShowMark(show_mark); cell->MarkType(mark_type); }
}

void SymbolOrderView::SetQuoteColor()
{
	try {
		if (!quote_control_) return;

		const VmQuote quote = quote_control_->get_quote();
		int lowRow = find_row(quote.low);
		int highRow = find_row(quote.high);
		int closeRow = find_row(quote.close);
		int openRow = find_row(quote.open);

		if (quote.close > quote.open) { // 양봉
			for (auto it = price_to_row_.rbegin(); it != price_to_row_.rend(); ++it) {
				if (it->second < highRow) set_quote_cell(it->second, true, 0);
				else if (it->second < closeRow) set_quote_cell(it->second, true, 3);
				else if (it->second <= openRow) set_quote_cell(it->second, true, 1);
				else if (it->second < lowRow + 1) set_quote_cell(it->second, true, 3);
				else set_quote_cell(it->second, true, 0);
			}

		}
		else if (quote.close < quote.open) { // 음봉
			for (auto it = price_to_row_.rbegin(); it != price_to_row_.rend(); ++it) {
				if (it->second < highRow) set_quote_cell(it->second, true, 0);
				else if (it->second < openRow) set_quote_cell(it->second, true, 3);
				else if (it->second <= closeRow) set_quote_cell(it->second, true, 2);
				else if (it->second < lowRow + 1) set_quote_cell(it->second, true, 3);
				else set_quote_cell(it->second, true, 0);
			}
		}
		else { // 도지
			for (auto it = price_to_row_.rbegin(); it != price_to_row_.rend(); ++it) {
				if (it->second < highRow) set_quote_cell(it->second, true, 0);
				else if (it->second < closeRow) set_quote_cell(it->second, true, 3);
				else if (it->second <= openRow) set_quote_cell(it->second, true, 3);
				else if (it->second < lowRow + 1) set_quote_cell(it->second, true, 3);
				else set_quote_cell(it->second, true, 0);
			}
		}
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

int SymbolOrderView::FindValue(const int& row) const noexcept
{
	int result = -1;
	auto it = row_to_price_.find(row);
	if (it != row_to_price_.end())
		return it->second;
	else
		return -1;
}

void SymbolOrderView::Refresh()
{
	_EnableOrderShow = true;
	_EnableQuoteShow = true;
	_EnableStopShow = true;
	_EnableHogaShow = true;
	ClearOldQuote();
	ClearOldHoga();
	ClearStopOrders();
	ClearOrders();
	update_quote();
	update_hoga();
	SetOrder();
	SetStopOrder();
	Invalidate(FALSE);
}

void SymbolOrderView::DrawHogaLine(const CRect& rect)
{
	if (!_Symbol) return;

	int row_index = FindRow(_Symbol->Hoga.Ary[0].SellPrice);
	auto pCell = _Grid->FindCell(row_index, DarkHorse::OrderGridHeader::SELL_CNT);
	if (pCell && row_index > 1) 
		m_pGM->DrawLine(
			0, 
			pCell->Y() + pCell->Height() + 1, 
			rect.Width(), 
			pCell->Y() + pCell->Height() + 1, 
			_Resource.SelectedBrush
		);
}

void SymbolOrderView::DrawFixedSelectedCell()
{
	const int row_index = FindRow(_SelectedValue.first);
	auto pCell = _Grid->FindCell(row_index, _SelectedValue.second);
	if (pCell) {
		_Grid->DrawCell(
			pCell, 
			m_pGM, 
			_Resource.SelectedBrush, 
			_Resource.SelectedBrush, 
			_Resource.TextBrush, 
			_Resource.TextFormat, 
			false, true, true);
	}
}

void SymbolOrderView::DrawMovingOrder()
{
	if (_DraggingOrder) {

		auto start_cell = _Grid->FindCellByPos(_StartX, _StartY);
		if (!start_cell) return;
		if (start_cell->Col() == DarkHorse::OrderGridHeader::BUY_ORDER ||
			start_cell->Col() == DarkHorse::OrderGridHeader::SELL_ORDER ||
			start_cell->Col() == DarkHorse::OrderGridHeader::SELL_STOP ||
			start_cell->Col() == DarkHorse::OrderGridHeader::BUY_STOP) {


			const CBCGPRect rect_start(start_cell->X(), start_cell->Y(), start_cell->X() + start_cell->Width(), start_cell->Y() + start_cell->Height());
			const CBCGPPoint start_point = rect_start.CenterPoint();

			const CBCGPPoint ex_point(_EndX, _EndY);

			const auto it = _Grid->FindRowCol(_EndX, _EndY);
			const auto end_cell = _Grid->FindCell(it.first, it.second);
			CBCGPPoint end_point;
			if (end_cell && end_cell->Col() == start_cell->Col()) {
				const CBCGPRect rect_end(end_cell->X(), end_cell->Y(), end_cell->X() + end_cell->Width(), end_cell->Y() + end_cell->Height());
				end_point = rect_end.CenterPoint();
			}
			else {
				end_point.x = _EndX;
				end_point.y = _EndY;
			}

			DrawArrow(start_point, end_point, 2.0f, 12);
		}
	}
}

void SymbolOrderView::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	//rect.right -= 1;
	rect.bottom -= 1;

	CreateResource();


	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 60, 9);

	ResetHeaderWidth(rect.Width());
	_Grid->HeaderMode(SmHeaderMode::HeaderColOnly);
	//_Grid->SetOrderHeaderTitles();
	_Grid->MakeRowHeightMap();
	_Grid->RecalRowCount(rect.Height(), true);
	// _MergedCellMap[std::make_pair(1, 2)] = std::make_pair(1, 2);
	//_MergedCellMap[std::make_pair(1, 5)] = std::make_pair(1, 2);
	_Grid->AddMergeCell(1, 2, 1, 2);
	_Grid->AddMergeCell(1, 5, 1, 2);
	_Grid->MakeColWidthMap();

	_Grid->CreateGrids();

	{

		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_STOP].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_ORDER].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_CNT].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_QTY].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderGridHeader::QUOTE].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_QTY].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_CNT].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_ORDER].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_STOP].title);

		_Grid->SetColHeaderTitles(_OrderTableHeader);
	}

	_Grid->RegisterOrderButtons(_ButtonMap);


	mainApp.CallbackMgr()->SubscribeSymbolMasterWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->SubscribeQuoteWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->SubscribeHogaWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->SubscribeOrderWndCallback(GetSafeHwnd());

	mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&SymbolOrderView::OnQuoteEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeHogaCallback((long)this, std::bind(&SymbolOrderView::OnHogaEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeMasterCallback((long)this, std::bind(&SymbolOrderView::OnSymbolMasterEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SymbolOrderView::OnOrderEvent, this, _1, _2));

	//SetTimer(1, 10, NULL);

	return;
}


int SymbolOrderView::RecalRowCount(const int& height)
{
	CRect rect;

	GetWindowRect(rect);

	//rect.right -= 1;
	rect.bottom -= 1;

	ClearOldQuote();
	ClearOldHoga();
	_Grid->ReleaseOrderButtons(_ButtonMap);
	const int extra_height = _Grid->RecalRowCount(height, false);
	price_start_row_ = 2;
	price_end_row_ = _Grid->RowCount() - 2;


	_Grid->CreateGrids();

	_Grid->RegisterOrderButtons(_ButtonMap);

	SetCenterValues();
	update_quote();
	update_hoga();

	//Invalidate(FALSE);

	DmAccountOrderCenterWindow::DeltaOrderArea = extra_height;

	return extra_height;
}

void SymbolOrderView::UpdateOrder(const std::string& symbol_code)
{
	if (!_Account || !_Symbol) return;

	if (_Symbol->SymbolCode().compare(symbol_code) != 0) return;

	std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());

	//symbol_order_mgr->UpdateOrder(_BuyOrderViewer, _SellOrderViewer);

	//ClearOrders();

	//SetOrder();

	//Invalidate();
}

void SymbolOrderView::ConnectOrderUpdateSignal()
{
	if (!_Account || !_Symbol) return;

	std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());
	symbol_order_mgr->order_update.Connect(this, &SymbolOrderView::UpdateOrder);
}

void SymbolOrderView::ArrangeCenterValue()
{
	ClearOldQuote();
	ClearOldHoga();
	ClearOrders();

	close_row_ = get_center_row();

	SetCenterValues();

	update_quote();
	update_hoga();
	SetOrder();
}

void SymbolOrderView::BuyByMarketPrice()
{
	PutOrder(SmPositionType::Buy, 0, SmPriceType::Market);
}

void SymbolOrderView::SellByMarketPrice()
{
	PutOrder(SmPositionType::Sell, 0, SmPriceType::Market);
}

void SymbolOrderView::CancelSellStop()
{
	if (!_Account || !_Symbol) return;

	std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());

	symbol_order_mgr->SellStopOrderMgr()->Clear();
	//Invalidate();
	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SymbolOrderView::CancelSellOrder()
{
	if (!_Account || !_Symbol) return;

	std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());

	const std::map<int, std::map<std::string, std::shared_ptr<SmOrder>>>& sell_order_map = symbol_order_mgr->SellOrderViewer()->GetOrderMap();
	for (auto it = sell_order_map.begin(); it != sell_order_map.end(); ++it) {
		const auto& order_vec = it->second;
		for (auto iv = order_vec.begin(); iv != order_vec.end(); iv++) {
			auto order_req = SmOrderRequestManager::MakeCancelOrderRequest(
				_Account->No(),
				_Account->Pwd(), _Symbol->SymbolCode(),
				iv->second->OrderNo, iv->second->OrderPrice, iv->second->PositionType,
				iv->second->OrderAmount);
			mainApp.Client()->CancelOrder(order_req);
		}
	}
	//Invalidate();
	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SymbolOrderView::CancelAllOrder()
{
	CancelBuyStop();
	CancelSellStop();
	CancelSellOrder();
	CancelBuyOrder();
}

void SymbolOrderView::CancelBuyStop()
{
	if (!_Account || !_Symbol) return;

	std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());

	symbol_order_mgr->BuyStopOrderMgr()->Clear();
	//Invalidate();
	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SymbolOrderView::CancelBuyOrder()
{
	if (!_Account || !_Symbol) return;

	std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());

	const std::map<int, std::map<std::string, std::shared_ptr<SmOrder>>>& sell_order_map = symbol_order_mgr->BuyOrderViewer()->GetOrderMap();
	for (auto it = sell_order_map.begin(); it != sell_order_map.end(); ++it) {
		const auto& order_vec = it->second;
		for (auto iv = order_vec.begin(); iv != order_vec.end(); iv++) {
			auto order_req = SmOrderRequestManager::MakeCancelOrderRequest(
				_Account->No(),
				_Account->Pwd(), _Symbol->SymbolCode(),
				iv->second->OrderNo, iv->second->OrderPrice, iv->second->PositionType,
				iv->second->OrderAmount);
			mainApp.Client()->CancelOrder(order_req);
		}
	}
	//Invalidate();
	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SymbolOrderView::ProcesButtonClick(const std::shared_ptr<SmCell>& cell)
{
	if (!cell) return;

	auto found = _ButtonMap.find(cell);
	if (found == _ButtonMap.end()) return;

	ProcessButtonMsg(found->second);
}

bool SymbolOrderView::ProcesButtonClickByPos(const CPoint& point)
{
	for (auto it = _ButtonMap.begin(); it != _ButtonMap.end(); ++it) {
		auto cell = it->first;
		RECT r{ cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height() };
		if (PtInRect(&r, point) == TRUE) {
			ProcessButtonMsg(it->second);
			return true;
		}
	}

	return false;
}

std::pair<int, int> SymbolOrderView::GetOrderCount(const std::shared_ptr<SmCell>& cell)
{
	auto found = row_to_price_.find(cell->Row());
	if (found == row_to_price_.end()) return std::make_pair(0, 0);
	if (cell->Col() == DarkHorse::OrderGridHeader::BUY_ORDER ||
		cell->Col() == DarkHorse::OrderGridHeader::SELL_ORDER) {
		return std::make_pair(found->second, cell->GetOrderCount());
	}
	else if (cell->Col() == DarkHorse::OrderGridHeader::BUY_STOP ||
		cell->Col() == DarkHorse::OrderGridHeader::SELL_STOP) {
		return std::make_pair(found->second, cell->GetOrderReqCount());
	}
	return std::make_pair(0, 0);
}

void SymbolOrderView::ResetHeaderWidth(const int& wnd_width)
{
	int width_sum = 0;
	_Grid->SetColWidth(DarkHorse::OrderGridHeader::SELL_STOP, OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_STOP].width);
	_Grid->SetColWidth(DarkHorse::OrderGridHeader::SELL_ORDER, OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_ORDER].width);
	_Grid->SetColWidth(DarkHorse::OrderGridHeader::SELL_CNT, OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_CNT].width);
	_Grid->SetColWidth(DarkHorse::OrderGridHeader::SELL_QTY, OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_QTY].width);
	_Grid->SetColWidth(DarkHorse::OrderGridHeader::QUOTE, OrderGridHeaderVector[DarkHorse::OrderGridHeader::QUOTE].width);
	_Grid->SetColWidth(DarkHorse::OrderGridHeader::BUY_QTY, OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_QTY].width);
	_Grid->SetColWidth(DarkHorse::OrderGridHeader::BUY_CNT, OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_CNT].width);
	_Grid->SetColWidth(DarkHorse::OrderGridHeader::BUY_ORDER, OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_ORDER].width);
	_Grid->SetColWidth(DarkHorse::OrderGridHeader::BUY_STOP, OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_STOP].width);


	width_sum += OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_STOP].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_ORDER].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_CNT].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderGridHeader::SELL_QTY].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_QTY].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_CNT].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_ORDER].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderGridHeader::BUY_STOP].width;
	width_sum += 1;
	width_sum += 1;
	width_sum += 2;

	_Grid->SetColWidth(DarkHorse::OrderGridHeader::QUOTE, wnd_width - width_sum);
}

void SymbolOrderView::insert_decimal(std::string& value, const int decimal)
{
	try {
		CString msg;
		msg.Format("value = %s\n", value.c_str());
		TRACE(msg);
		if (decimal <= 0) return;
		if (value.length() == static_cast<size_t>(decimal))
			value.insert(0, 1, '0');
		else if (value.length() < static_cast<size_t>(decimal))
			value.insert(0, 2, '0');
		value.insert(value.length() - decimal, 1, '.');
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}
void SymbolOrderView::SetCenterValues(const bool& make_row_map /*= true*/)
{
	if (!quote_control_ || !product_control_) return;

	const int& close = quote_control_->get_quote().close;
	if (close == 0) return;
	const int start_value = find_start_value();
	try {
		if (make_row_map) {
			price_to_row_.clear();
			row_to_price_.clear();
		}
		int next_value = start_value;
		for (int i = price_start_row_; i < price_end_row_; i++) {
			std::string value_string;
			value_string = std::format("{0}", next_value);
			insert_decimal(value_string, product_control_->get_product().decimal);
			_Grid->SetCellText(i, DarkHorse::OrderGridHeader::QUOTE, value_string);
			price_to_row_[next_value] = i;
			row_to_price_[i] = next_value;

			next_value = product_control_->get_next_down_value(next_value);
		}
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SymbolOrderView::PutOrderBySpaceBar()
{
	if (!_Account || !_Symbol) return;

	if (!_FixedSelectedCell) return;

	const int price = FindValue(_FixedSelectedCell->Row());

	if (_FixedSelectedCell->Col() == DarkHorse::OrderGridHeader::SELL_ORDER)
		PutOrder(SmPositionType::Sell, price);
	else if (_FixedSelectedCell->Col() == DarkHorse::OrderGridHeader::BUY_ORDER)
		PutOrder(SmPositionType::Buy, price);
	else if (_FixedSelectedCell->Col() == DarkHorse::OrderGridHeader::BUY_STOP)
		PutStopOrder(SmPositionType::Buy, price);
	else if (_FixedSelectedCell->Col() == DarkHorse::OrderGridHeader::SELL_STOP)
		PutStopOrder(SmPositionType::Sell, price);

	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SymbolOrderView::ChangeOrderByKey(const int up_down)
{
	if (!_Account || !_Symbol) return;
	Invalidate();
}

void SymbolOrderView::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	if (!_Account || !_Symbol) return;
	if (_Account->Id() != account_id || _Symbol->Id() != symbol_id) return;

	UpdateOrder(_Symbol->SymbolCode());
	Invalidate();
}

void SymbolOrderView::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	if (!_Account || !_Symbol) return;
	if (_Account->No() != account_no) return;
	if (_Symbol->SymbolCode() != symbol_code) return;

	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SymbolOrderView::OnQuoteEvent(const std::string& symbol_code)
{
	if (!_Symbol) return;
	if (_Symbol->SymbolCode() != symbol_code) return;
	if (_FixedMode) {
		ProcessFixedMode();
		return;
	}
	_EnableQuoteShow = true;
}

void SymbolOrderView::OnHogaEvent(const std::string& symbol_code)
{
	if (!_Symbol) return;
	if (_Symbol->SymbolCode() != symbol_code) return;
	_EnableHogaShow = true;
}

void SymbolOrderView::OnSymbolMasterEvent(const std::string& symbol_code)
{
	if (!_Symbol) return;
	if (_Symbol->SymbolCode() != symbol_code) return;
}

void SymbolOrderView::UpdateOrderSettings(SmOrderSettings settings)
{
	_OrderSettings = settings;
}

void SymbolOrderView::SetAllRowHeight(const int& row_height)
{
	_Grid->SetAllRowHeight(row_height);
}

void SymbolOrderView::FixedMode(bool val)
{
	_FixedMode = val;
	ArrangeCenterValue();
	Refresh();
}

void SymbolOrderView::init_quote_control(const std::string& symbol_code)
{
	auto quote = mainApp.QuoteMgr()->get_quote(symbol_code);
}

void SymbolOrderView::init_hoga_control(const std::string& symbol_code)
{
	auto hoga = mainApp.HogaMgr()->get_hoga(symbol_code);
}

void SymbolOrderView::Symbol(std::shared_ptr<DarkHorse::SmSymbol> val)
{
	_Symbol = val;
	auto quote = mainApp.QuoteMgr()->get_quote(_Symbol->SymbolCode());
	quote->symbol_id = val->Id();
	quote_control_->set_symbol_id(val->Id());
	quote_control_->update_quote(quote);
	auto hoga = mainApp.HogaMgr()->get_hoga(_Symbol->SymbolCode());
	hoga->symbol_id = val->Id();
	hoga_control_->set_symbol_id(val->Id());
	hoga_control_->update_hoga(hoga);
	product_control_->update_product(_Symbol);
	ArrangeCenterValue();
	Invalidate();
}

void SymbolOrderView::DrawStopOrder()
{
	for (size_t i = 0; i < _StopRectVector.size(); ++i) {
		CBCGPPoint start_point = _StopRectVector[i].first.CenterPoint();
		CBCGPPoint end_point = _StopRectVector[i].second.CenterPoint();
		DrawArrow(start_point, end_point, 1.0f, 6);
	}
}

void SymbolOrderView::DrawArrow(const CBCGPPoint& start_point, const CBCGPPoint& end_point, const double& stroke_width, const int& head_width)
{
	//const double stroke_width = 2.0f;
	m_pGM->DrawLine(start_point, end_point, _Resource.SelectedBrush, stroke_width);

	CBCGPPointsArray arrow_array;

	const int head_length = head_width; // , head_width = 6;

	const float dx = static_cast<float>(end_point.x - start_point.x);
	const float dy = static_cast<float>(end_point.y - start_point.y);
	const auto length = std::sqrt(dx * dx + dy * dy);

	// ux,uy is a unit vector parallel to the line.
	const auto ux = dx / length;
	const auto uy = dy / length;

	// vx,vy is a unit vector perpendicular to ux,uy
	const auto vx = -uy;
	const auto vy = ux;
	const auto half_width = 0.5f * head_width;

	const CBCGPPoint arrow[3] =
	{ end_point,
		CBCGPPoint{ (end_point.x - head_length * ux + half_width * vx),
		(end_point.y - head_length * uy + half_width * vy) },
		CBCGPPoint{ (end_point.x - head_length * ux - half_width * vx),
		(end_point.y - head_length * uy - half_width * vy) }
	};
	for (int i = 0; i < 3; i++) arrow_array.Add(arrow[i]);

	m_pGM->FillGeometry(CBCGPPolygonGeometry(arrow_array), _Resource.SelectedBrush);
}

int SymbolOrderView::find_close_row_from_end_row()
{
	const int close_value = quote_control_->get_quote().close;
	int next_value = 0;
	int target_row = price_end_row_;

	do {
		next_value = product_control_->get_next_up_value(next_value);
		target_row--;
	} while (next_value < close_value);

	return target_row;
}

int SymbolOrderView::find_zero_value_row()
{
	if (!product_control_) return -1;
	return product_control_->get_row(0, close_row_, quote_control_->get_quote().close);
}

int SymbolOrderView::find_row(const int target_value)
{
	if (!product_control_ || !quote_control_) return -1;
	return product_control_->get_row(target_value, close_row_, quote_control_->get_quote().close);
}

int SymbolOrderView::find_row2(const int target_value)
{
	assert(!quote_control_ || !product_control_);

	const int& close = quote_control_->get_quote().close;
	if (close == 0) return 0;
	int next_value = close;
	int next_row = _Grid->CloseRow();
	if (target_value == next_value) return next_row;
	return target_value;
}
int SymbolOrderView::find_value(const int target_row)
{
	return -1;
}

int SymbolOrderView::find_start_value()
{
	if (!quote_control_ || !product_control_) return 0;
	const int& close = quote_control_->get_quote().close;
	if (close == 0) return 0;
	return product_control_->get_value(price_start_row_, close_row_, close);
}

int SymbolOrderView::get_center_row()
{
	return static_cast<int>(_Grid->RowCount() / 2);
}
void SymbolOrderView::ProcessFixedMode()
{
	close_row_ = get_center_row();

	SetCenterValues(false);

	_EnableOrderShow = true;
	_EnableQuoteShow = true;
	_EnableHogaShow = true;
	_EnableStopShow = true;
}



void SymbolOrderView::SetProfitLossCut(std::shared_ptr<SmOrderRequest> order_req)
{
	order_req->CutSlip = _OrderSettings.SlipTick;
	order_req->ProfitCutTick = _OrderSettings.ProfitCutTick;
	order_req->LossCutTick = _OrderSettings.LossCutTick;
	order_req->CutPriceType = _OrderSettings.PriceType;
	if (_OrderSettings.ProfitCut) {
		if (_OrderSettings.LossCut)
			order_req->CutMode = 3;
		else
			order_req->CutMode = 1;
	}
	else if (_OrderSettings.LossCut) {
		if (_OrderSettings.ProfitCut)
			order_req->CutMode = 3;
		else
			order_req->CutMode = 2;
	}
	else
		order_req->CutMode = 0;
}

void SymbolOrderView::SetStopOrderCut(std::shared_ptr<SmOrderRequest> order_req)
{
	order_req->CutSlip = _OrderSettings.SlipTick;
}

void SymbolOrderView::PutStopOrder(const DarkHorse::SmPositionType& type, const int& price)
{
	if (!_Account || !_Symbol) return;
	const auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());

	std::shared_ptr<SmOrderRequest> order_req = nullptr;
	if (type == SmPositionType::Sell) {
		order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(_Account->No(), _Account->Pwd(), _Symbol->SymbolCode(), price, _OrderAmount * _Account->SeungSu());
		if (order_req) {
			SetStopOrderCut(order_req);
			symbol_order_mgr->SellStopOrderMgr()->AddOrderRequest(order_req);
		}
	}
	else {
		order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(_Account->No(), _Account->Pwd(), _Symbol->SymbolCode(), price, _OrderAmount * _Account->SeungSu());
		if (order_req) {
			SetStopOrderCut(order_req);
			symbol_order_mgr->BuyStopOrderMgr()->AddOrderRequest(order_req);
		}
	}
}

void SymbolOrderView::PutOrder(const SmPositionType& type, const int& price, const SmPriceType& price_type)
{
	if (!_Account || !_Symbol) return;

	std::shared_ptr<SmOrderRequest> order_req = nullptr;
	if (type == SmPositionType::Sell)
		order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(_Account->No(), _Account->Pwd(), _Symbol->SymbolCode(), price, _OrderAmount, price_type);
	else
		order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(_Account->No(), _Account->Pwd(), _Symbol->SymbolCode(), price, _OrderAmount, price_type);
	if (order_req) {
		SetProfitLossCut(order_req);
		mainApp.Client()->NewOrder(order_req);
	}
}

void SymbolOrderView::CreateResource()
{
	_Resource.OrderStroke.SetStartCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_ROUND);
	_Resource.OrderStroke.SetEndCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_TRIANGLE);
	CBCGPTextFormat fmt3(_T("굴림"), globalUtils.ScaleByDPI(30.0f));

	fmt3.SetFontSize(12);
	fmt3.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt3.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt3.SetWordWrap();
	fmt3.SetClipText();

	_Resource.TextFormat = fmt3;

	CBCGPTextFormat fmt(_T("Tahoma"), globalUtils.ScaleByDPI(30.0f));

	fmt.SetFontSize(14);
	fmt.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt.SetFontWeight(FW_BOLD);
	fmt.SetWordWrap();
	fmt.SetClipText();

	_Resource.QuoteTextFormat = fmt;


	CBCGPTextFormat fmt2(_T("굴림"), globalUtils.ScaleByDPI(30.0f));

	fmt2.SetFontSize(12);
	fmt2.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt2.SetWordWrap();
	fmt2.SetClipText();
	fmt2.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	//fmt2.SetFontWeight(FW_BOLD);

	_Resource.QuoteNumberFormat = fmt2;
}

void SymbolOrderView::increase_close_row(const int& delta)
{
	close_row_ += delta;
	if (quote_control_->get_quote().close == 0) return;
	_Grid->CloseRow(close_row_);
}

void SymbolOrderView::set_close_row()
{
	if (quote_control_->get_quote().close == 0) return;
}

void SymbolOrderView::ClearOldSelectedCells()
{
	for (size_t i = 0; i < _SelectedCells.size(); i++) {
		auto cell = _SelectedCells[i];
		_Grid->DrawCell(cell, m_pGM, _Resource.GridNormalBrush, _Resource.GridNormalBrush, _Resource.TextBrush, _Resource.TextFormat, true, true, true);
	}
	_SelectedCells.clear();
}

void SymbolOrderView::DrawMovingRect()
{
	if (_MouseMode == MouseMode::Normal) {
		ClearOldSelectedCells();
		auto cell = _Grid->FindCellByPos(_X, _Y);
		if (!cell) return;
		if (cell->Row() <= 0) return;

		if (cell->Col() == DarkHorse::OrderGridHeader::SELL_ORDER) {
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderGridHeader::BUY_ORDER);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderGridHeader::QUOTE);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
		}
		else if (cell->Col() == DarkHorse::OrderGridHeader::BUY_ORDER) {
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderGridHeader::SELL_ORDER);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderGridHeader::QUOTE);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
		}
		else if (cell->Col() == DarkHorse::OrderGridHeader::QUOTE) {
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
		}
		else if (cell->Col() == DarkHorse::OrderGridHeader::BUY_STOP) {
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderGridHeader::SELL_STOP);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderGridHeader::QUOTE);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
		}
		else if (cell->Col() == DarkHorse::OrderGridHeader::SELL_STOP) {
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderGridHeader::BUY_STOP);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderGridHeader::QUOTE);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
		}
	}
}

void SymbolOrderView::CancelOrder(const std::shared_ptr<DarkHorse::SmCell>& src_cell)
{
	if (!src_cell) return;

	if (src_cell->GetOrderCount() == 0) return;

	const std::map<int, std::shared_ptr<SmOrder>>& order_map = src_cell->GetOrderMap();
	for (auto it = order_map.begin(); it != order_map.end(); it++) {
		auto order_req = SmOrderRequestManager::MakeChangeOrderRequest(
			_Account->No(),
			_Account->Pwd(), _Symbol->SymbolCode(),
			it->second->OrderNo, it->second->OrderPrice, it->second->PositionType,
			it->second->OrderAmount);
		mainApp.Client()->CancelOrder(order_req);
	}

	src_cell->ClearOrder();
}

void SymbolOrderView::ChangeOrder(const std::shared_ptr<DarkHorse::SmCell>& src_cell, const int& tgt_price)
{
	if (!src_cell) return;
	if (src_cell->GetOrderCount() == 0) return;
	const std::map<int, std::shared_ptr<SmOrder>>& order_map = src_cell->GetOrderMap();
	for (auto it = order_map.begin(); it != order_map.end(); it++) {
		auto order_req = SmOrderRequestManager::MakeChangeOrderRequest(
			_Account->No(),
			_Account->Pwd(), _Symbol->SymbolCode(),
			it->second->OrderNo, tgt_price, it->second->PositionType,
			it->second->OrderAmount);
		SetProfitLossCut(order_req);
		mainApp.Client()->ChangeOrder(order_req);
	}

	src_cell->ClearOrder();
	_EnableOrderShow = true;
}

void SymbolOrderView::ChangeStop(const std::shared_ptr<DarkHorse::SmCell>& src_cell, const std::shared_ptr<DarkHorse::SmCell>& tgt_cell, const int& src_price, const int& tgt_price)
{
	if (!src_cell) return;

	if (src_cell->GetOrderReqCount() == 0) return;

	const std::map<int, std::shared_ptr<SmOrderRequest>>& order_req_map = src_cell->GetOrderReqMap();
	for (auto it = order_req_map.begin(); it != order_req_map.end(); ++it) {
		it->second->OrderPrice = tgt_price;
		tgt_cell->AddOrderReq(it->second);
	}

	src_cell->ClearOrderReq();
}


void SymbolOrderView::CancelStop(const std::shared_ptr<DarkHorse::SmCell>& src_cell)
{
	if (!src_cell) return;

	if (!_Account || !_Symbol) return;

	std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());

	const std::map<int, std::shared_ptr<SmOrderRequest>>& order_req_map = src_cell->GetOrderReqMap();
	for (auto it = order_req_map.begin(); it != order_req_map.end(); ++it) {
		symbol_order_mgr->RemoveStopOrder(it->second->RequestId);
	}
	src_cell->ClearOrderReq();
}

void SymbolOrderView::ProcessButtonMsg(const BUTTON_ID& id)
{
	switch (id)
	{
	case BUTTON_ID::ARRANGE: ArrangeCenterValue(); break;
	case BUTTON_ID::BUY: BuyByMarketPrice(); break;
	case BUTTON_ID::SELL: SellByMarketPrice(); break;
	case BUTTON_ID::CANCEL_SELL_STOP: CancelSellStop(); break;
	case BUTTON_ID::CANCEL_SELL_ORDER: CancelSellOrder(); break;
	case BUTTON_ID::CANCEL_ALL: CancelAllOrder(); break;
	case BUTTON_ID::CANCEL_BUY_ORDER: CancelBuyOrder(); break;
	case BUTTON_ID::CANCEL_BUY_STOP: CancelBuyStop(); break;
	}
}

void SymbolOrderView::OnPaint()
{
	if (_Editing) return;

	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rect;
	GetClientRect(rect);

	if (m_pGM == NULL)
	{
		return;
	}

	try {

		m_pGM->BindDC(pDC, rect);



		if (!m_pGM->BeginDraw())
		{
			return;
		}

		m_pGM->Clear();

		m_pGM->FillRectangle(rect, _Resource.GridNormalBrush);
		rect.right -= 1;
		rect.bottom -= 1;
		_Grid->SetColHeaderTitles(_OrderTableHeader);
		_Grid->DrawGrid(m_pGM, rect);
		_Grid->DrawCells(m_pGM, rect);
		_Grid->DrawBorder(m_pGM, rect, _Selected);
		DrawFixedSelectedCell();

		if (_Hover) DrawMovingRect();

		DrawMovingOrder();

		DrawStopOrder();

		DrawHogaLine(rect);


		m_pGM->EndDraw();
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SymbolOrderView::OnMouseMove(UINT nFlags, CPoint point)
{
	CString msg;
	msg.Format("OnMouseMove %d, %d\n", point.x, point.y);
	//TRACE(msg);

	_X = point.x;
	_Y = point.y;

	_EndX = point.x;
	_EndY = point.y;

	if (!m_bTrackMouse)
	{

		TRACKMOUSEEVENT tme;

		tme.cbSize = sizeof(tme);

		tme.hwndTrack = m_hWnd;

		tme.dwFlags = TME_LEAVE | TME_HOVER;

		tme.dwHoverTime = 1;

		if (TrackMouseEvent(&tme))

		{

			m_bTrackMouse = TRUE;

		}

	}
	_EnableOrderShow = true;
	_EnableQuoteShow = true;
	_EnableHogaShow = true;
	_EnableStopShow = true;
	//Invalidate();
}

void SymbolOrderView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString msg;
	msg.Format("OnKeyDown %d, %d\n", nChar, nRepCnt);
	TRACE(msg);
	Invalidate();
}

void SymbolOrderView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString msg;
	msg.Format("OnChar %d, %d\n", nChar, nRepCnt);
	TRACE(msg);
	Invalidate();
}


void SymbolOrderView::OnMouseHover(UINT nFlags, CPoint point)
{
	_Hover = true;
	Invalidate();
}

void SymbolOrderView::OnMouseLeave()
{
	if (_MouseMode == MouseMode::Normal) {
		ClearOldSelectedCells();
		_FixedSelectedCell = nullptr;
		_SelectedValue.first = -1;
		_SelectedValue.second = -1;
	}
	m_bTrackMouse = FALSE;
	_Hover = false;
	Invalidate();
}

void SymbolOrderView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	if (ProcesButtonClickByPos(point)) return;

	if (_MainDialog) {

		if (_Symbol) _MainDialog->ChangedSymbol(_Symbol);

		DmAccountOrderCenterWindow* wnd = (DmAccountOrderCenterWindow*)GetParent();
		_MainDialog->ChangedCenterWindow(wnd->ID());
	}

	auto cell = _Grid->FindCellByPos(_X, _Y);
	if (!cell) return;

	if (cell) { _FixedSelectedCell = cell; }

	auto cell_pos = _Grid->FindRowCol(point.x, point.y);
	if (cell_pos.second == DarkHorse::OrderGridHeader::SELL_ORDER ||
		cell_pos.second == DarkHorse::OrderGridHeader::BUY_ORDER ||
		cell_pos.second == DarkHorse::OrderGridHeader::SELL_STOP ||
		cell_pos.second == DarkHorse::OrderGridHeader::BUY_STOP) {
		const int price = FindValue(cell_pos.first);
		_SelectedValue.first = price;
		_SelectedValue.second = cell_pos.second;
	}

	const auto order_count = GetOrderCount(cell);

	if (order_count.second > 0) {
		_MovingOrder = true;
		_OrderStartCol = cell->Col();
		_OrderStartRow = cell->Row();
		_OrderStartPrice = order_count.first;
		_DraggingOrder = true;

		_StartX = point.x;
		_StartY = point.y;

		SetCapture();
	}

	CBCGPStatic::OnLButtonDown(nFlags, point);
}


void SymbolOrderView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (_MouseMode == MouseMode::Normal) {
		///ClearOldSelectedCells();

		auto cell = _Grid->FindCellByPos(_X, _Y);
		if (cell) { _FixedSelectedCell = cell; }

		const auto order_count = GetOrderCount(cell);

		if (order_count.second > 0) {
			if (cell->Col() == DarkHorse::OrderGridHeader::SELL_ORDER)
				CancelOrder(_FixedSelectedCell);
			else if (cell->Col() == DarkHorse::OrderGridHeader::BUY_ORDER)
				CancelOrder(_FixedSelectedCell);
			else if (cell->Col() == DarkHorse::OrderGridHeader::BUY_STOP)
				CancelStop(_FixedSelectedCell);
			else if (cell->Col() == DarkHorse::OrderGridHeader::SELL_STOP)
				CancelStop(_FixedSelectedCell);

			_EnableOrderShow = true;
			_EnableStopShow = true;
		}
	}

	CBCGPStatic::OnRButtonDown(nFlags, point);
}


void SymbolOrderView::OnLButtonUp(UINT nFlags, CPoint point)
{

	if (_DraggingOrder) {
		_EndX = point.x;
		_EndY = point.y;

		auto cell = _Grid->FindCellByPos(_EndX, _EndY);
		if (!cell) {
			if (_OrderStartCol == DarkHorse::OrderGridHeader::SELL_ORDER)
				CancelOrder(_FixedSelectedCell);
			else if (_OrderStartCol == DarkHorse::OrderGridHeader::BUY_ORDER)
				CancelOrder(_FixedSelectedCell);
			else if (_OrderStartCol == DarkHorse::OrderGridHeader::BUY_STOP)
				CancelStop(_FixedSelectedCell);
			else if (_OrderStartCol == DarkHorse::OrderGridHeader::SELL_STOP)
				CancelStop(_FixedSelectedCell);
		}
		else {
			if (cell->Col() == _OrderStartCol) {
				auto found = row_to_price_.find(cell->Row());
				if (found == row_to_price_.end()) return;

				if (_OrderStartCol == DarkHorse::OrderGridHeader::SELL_ORDER)
					ChangeOrder(_FixedSelectedCell, found->second);
				else if (_OrderStartCol == DarkHorse::OrderGridHeader::BUY_ORDER)
					ChangeOrder(_FixedSelectedCell, found->second);
				else if (_OrderStartCol == DarkHorse::OrderGridHeader::BUY_STOP)
					ChangeStop(_FixedSelectedCell, cell, _OrderStartPrice, found->second);
				else if (_OrderStartCol == DarkHorse::OrderGridHeader::SELL_STOP)
					ChangeStop(_FixedSelectedCell, cell, _OrderStartPrice, found->second);
			}
			else {
				if (_OrderStartCol == DarkHorse::OrderGridHeader::SELL_ORDER)
					CancelOrder(_FixedSelectedCell);
				else if (_OrderStartCol == DarkHorse::OrderGridHeader::BUY_ORDER)
					CancelOrder(_FixedSelectedCell);
				else if (_OrderStartCol == DarkHorse::OrderGridHeader::BUY_STOP)
					CancelStop(_FixedSelectedCell);
				else if (_OrderStartCol == DarkHorse::OrderGridHeader::SELL_STOP)
					CancelStop(_FixedSelectedCell);
			}
		}

		_DraggingOrder = false;
		ReleaseCapture();
	}
	_EnableStopShow = true;
	_EnableOrderShow = true;
	Invalidate();
	_MovingOrder = false;

	CBCGPStatic::OnLButtonUp(nFlags, point);
}


int SymbolOrderView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rect;
	GetClientRect(rect);
	return 0;
}


void SymbolOrderView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!_Grid) return;

	auto cell_pos = _Grid->FindRowCol(point.x, point.y);
	if (cell_pos.second == DarkHorse::OrderGridHeader::SELL_STOP) {
		const int price = FindValue(cell_pos.first);
		PutStopOrder(SmPositionType::Sell, price);
	}
	else if (cell_pos.second == DarkHorse::OrderGridHeader::BUY_STOP) {
		const int price = FindValue(cell_pos.first);
		PutStopOrder(SmPositionType::Buy, price);
	}
	else if (cell_pos.second == DarkHorse::OrderGridHeader::SELL_ORDER) {
		const int price = FindValue(cell_pos.first);
		PutOrder(SmPositionType::Sell, price);
	}
	else if (cell_pos.second == DarkHorse::OrderGridHeader::BUY_ORDER) {
		const int price = FindValue(cell_pos.first);
		PutOrder(SmPositionType::Buy, price);
	}

	CBCGPStatic::OnLButtonDblClk(nFlags, point);
}

LRESULT SymbolOrderView::OnWmSymbolMasterReceived(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = static_cast<int>(wParam);
	if (!_Symbol || _Symbol->Id() != symbol_id) return 0;

	SetCenterValues(false);

	return 1;
}

LRESULT SymbolOrderView::OnWmQuoteReceived(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = static_cast<int>(wParam);
	if (!_Symbol || _Symbol->Id() != symbol_id) return 0;

	update_quote();

	return 1;
}

LRESULT SymbolOrderView::OnWmHogaReceived(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = static_cast<int>(wParam);
	if (!_Symbol || _Symbol->Id() != symbol_id) return 0;

	update_hoga();

	return 1;
}

LRESULT SymbolOrderView::OnUmOrderChanged(WPARAM wParam, LPARAM lParam)
{
	UpdateOrder(_Symbol->SymbolCode());
	Invalidate(FALSE);
	return 1;
}



void SymbolOrderView::OnTimer(UINT_PTR nIDEvent)
{
	bool needDraw = false;
	if (_EnableQuoteShow && _Symbol) {
		ClearOldQuote();
		update_quote();
		needDraw = true;
		_EnableQuoteShow = false;
	}
	if (_EnableHogaShow && _Symbol) {
		ClearOldHoga();
		update_hoga();
		needDraw = true;
		_EnableHogaShow = false;
	}
	if (_EnableStopShow) {
		ClearStopOrders();
		SetStopOrder();
		needDraw = true;
		_EnableStopShow = false;
	}
	if (_EnableOrderShow && _Symbol) {
		ClearOrders();
		SetOrder();
		ClearOldQuote();
		update_quote();
		needDraw = true;
		_EnableOrderShow = false;
	}

	if (needDraw) Invalidate(FALSE);

	CBCGPStatic::OnTimer(nIDEvent);
}
