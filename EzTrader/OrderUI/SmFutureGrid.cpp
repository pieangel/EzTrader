#include "stdafx.h"
#include "SmFutureGrid.h"
#include "VtOrderConfigManager.h"
#include "SmOrderPanel.h"
#include <functional>
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmProduct.h"
#include "../Symbol/SmProductYearMonth.h"
#include "../SmGrid/SmCell.h"
#include "../Account/SmAccount.h"

#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Symbol/SmSymbolManager.h"
#include <format>

#include "../Fund/SmFund.h"
#include "../Event/EventHub.h"

#include "../Controller/QuoteControl.h"
#include "../ViewModel/VmQuote.h"
#include "../Util/SmUtil.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
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
#include "../Log/MyLogger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

SmFutureGrid::SmFutureGrid()
	: id_(IdGenerator::get_id())
{
	quote_control_ = std::make_shared<DarkHorse::QuoteControl>();
	quote_control_->symbol_type(SymbolType::Domestic);
	quote_control_->set_direct_event_handler(std::bind(&SmFutureGrid::update_quote_direct, this, _1));

	position_control_ = std::make_shared<DarkHorse::SymbolPositionControl>();
	position_control_->set_future_event_handler(std::bind(&SmFutureGrid::on_update_position_direct, this, _1));

	CString strLog;
	strLog.Format("DmFutureView futur_view id = [%d], position_control_id = [%d]", id_, position_control_->get_id());
	//LOGINFO(CMyLogger::getInstance(), (const char*)strLog);

	mainApp.event_hub()->subscribe_expected_event_handler
	(
		id_,
		std::bind(&SmFutureGrid::update_expected_direct, this, std::placeholders::_1)
	);

	mainApp.event_hub()->subscribe_order_event_handler
	(
		id_,
		std::bind(&SmFutureGrid::update_order, this, std::placeholders::_1, std::placeholders::_2)
	);
}


SmFutureGrid::~SmFutureGrid()
{
	mainApp.event_hub()->unsubscribe_order_event_handler(id_);
	mainApp.event_hub()->unsubscribe_expected_event_handler(id_);
}


void SmFutureGrid::RegisterMasterCallback()
{
	//mainApp.CallbackMgr().SubscribeMasterCallback((long)this, std::bind(&SmFutureGrid::OnMasterEvent, this, _1));
}

void SmFutureGrid::RegisterQuoteCallback()
{
	//mainApp.CallbackMgr().SubscribeQuoteCallback((long)this, std::bind(&SmFutureGrid::OnQuoteEvent, this, _1));
}

void SmFutureGrid::RegisterOrderCallback()
{
	//mainApp.CallbackMgr().SubscribeOrderCallback((long)this, std::bind(&SmFutureGrid::OnOrderEvent, this, _1));
}

void SmFutureGrid::OnMasterEvent(symbol_p sym)
{
	if (!sym)
		return;
	if (_Mode != 1)
		return;

	CUGCell cell;
	/*
	auto it = _FutureCurrentValueRowMap.find(sym->ShortCode);
	if (it != _FutureCurrentValueRowMap.end()) {
		auto pos = _FutureCurrentValueRowMap[sym->ShortCode];
		ShowCurrent(sym, std::get<0>(pos));
	}
	*/
}

void SmFutureGrid::OnOrderEvent(VtOrder* order)
{
	if (!_OrderConfigMgr || !order)
		return;

	if (_OrderConfigMgr->Type() == 0) {
		/*
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;
		SetRemain(order->shortCode);
		*/
		int i = 0;
	}
	else {
		/*
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		SetRemain(order->shortCode);
		*/
		int i = 0;
	}
}

void SmFutureGrid::OnQuoteEvent(symbol_p sym)
{
	if (!sym)
		return;

	if (_Mode == 0) {
		ShowRemain(sym);
	}
	else if (_Mode == 1) {
		CUGCell cell;
		/*
		auto it = _FutureCurrentValueRowMap.find(sym->ShortCode);
		if (it != _FutureCurrentValueRowMap.end()) {
			auto pos = _FutureCurrentValueRowMap[sym->ShortCode];
			ShowCurrent(sym, std::get<0>(pos));
		}
		*/
	}
}

void SmFutureGrid::UnregisterAllCallback()
{
	//mainApp.CallbackMgr().UnsubscribeOrderCallback((long)this);
	//mainApp.CallbackMgr().UnsubscribeMasterCallback((long)this);
	//mainApp.CallbackMgr().UnsubscribeQuoteCallback((long)this);
}

BEGIN_MESSAGE_MAP(SmFutureGrid, CGridCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void SmFutureGrid::Init()
{
	_CellHeight = 20;
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
				if (j == 2)
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				else // 텍스트 정렬
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

	SetColTitle();

	RegisterOrderCallback();
	RegisterMasterCallback();
	RegisterQuoteCallback();
}

void SmFutureGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	CCellID cell = GetCellFromPt(point);
	CGridCellBase* pCell = GetCell(cell.row, cell.col);


	auto cell_pos = std::make_pair(cell.row, cell.col);
	if (cell_pos.first < 0 || cell_pos.first > 4) return;
	if (_OrderConfigMgr) {
		if (_OrderConfigMgr->_HdCenterWnd) {
			_OrderConfigMgr->_HdCenterWnd->ChangeSymbol(symbol_vec_[cell_pos.first].symbol_p);
		}
	}
}

LRESULT SmFutureGrid::OnOrderChangedMessage(WPARAM wParam, LPARAM lParam)
{
	return 1;
}

void SmFutureGrid::SetColTitle()
{
	int colWidth[3] = { 65, 50, 55 };

	for (int i = 0; i < _ColCount; i++) {
		SetColumnWidth(i, colWidth[i]);
	}

	LPCTSTR title[5] = { "지수선물", "미니선물", "코닥선물", "국채선물", "달러선물" };
	for (int j = 0; j < _RowCount; ++j) {
		QuickSetBackColor(j, 0, DarkHorse::SmTotalManager::GridTitleBackColor);
		QuickSetText(j, 0, title[j]);
		InvalidateCellRect(j, 0);
		if (j + 1 == 5)
			break;
	}
}


void SmFutureGrid::InitGrid()
{
	if (!_OrderConfigMgr)
		return;
	/*
	std::vector<SmRunInfo> run_info = mainApp.SymbolMgr().MrktMgr().GetFutureRunVector();
	int i = 0;
	for (auto it = run_info.begin(); it != run_info.end(); ++it) {
		SmRunInfo item = *it;
		SmProduct* product = mainApp.SymbolMgr().MrktMgr().FindProduct(item.Code.substr(0, 3));
		if (!product) continue;
		SmProductYearMonth* year_month = product->GetRecentYearMonth();
		if (!year_month) continue;

		symbol_p sym = year_month->GetFirstSymbol();
		if (sym) {
			GetSymbolMaster(sym);
			sym->RecentMonth(true);
			sym->RequestedSymbolMaster(true);
			if (_Mode == 0) {
				ShowRemain(sym);
			}
			else if (_Mode == 1) {
				ShowCurrent(sym, i);
			}
			else {
				QuickSetText(i, 2, _T("0"));
				QuickSetBackColor(i, 2, RGB(255, 255, 255));
			}

			CGridCellBase* pCell = GetCell(i, 0);
			pCell->SetBackClr(MainBeetle::GridTitleBackColor);
			pCell->SetTextClr(MainBeetle::GridTitleTextColor);
			pCell->SetText(item.Name.c_str());
			// Set the symbol object for the current cell.
			pCell->SetData((LPARAM)sym);
			InvalidateCellRect(i, 0);

			QuickSetText(i, 1, item.Code.c_str());
			pCell = GetCell(i, 1);
			pCell->SetData((LPARAM)sym);

			pCell = GetCell(i, 2);
			pCell->SetData((LPARAM)sym);

			_FutureCurrentValueRowMap[sym->ShortCode] = std::make_tuple(i, 2, sym);
			i++;
		}
	}
	*/
}

void SmFutureGrid::ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode)
{
	auto it = _FutureCurrentValueRowMap.find(symCode);
	if (it != _FutureCurrentValueRowMap.end()) {
		auto pos = _FutureCurrentValueRowMap[symCode];
		/*
		if (!posi) {
			if (acptCnt > 0) {
				QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(186, 186, 186));
			}
			else {
				if (init) {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
				else {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
			}
		}
		else {
			if (std::abs(posi->OpenQty) > 0) {
				CString strVal;
				strVal.Format(_T("%d"), posi->OpenQty);
				QuickSetText(std::get<0>(pos), std::get<1>(pos), strVal);
				if (acptCnt > 0)
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(186, 186, 186));
				else
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
			}
			else {
				if (acptCnt > 0) {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(186, 186, 186));
				}
				else {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
			}
		}

		QuickSetTextColor(std::get<0>(pos), std::get<1>(pos), RGB(0, 0, 0));
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));
		*/
	}
}

void SmFutureGrid::GetSymbolMaster(symbol_p sym)
{
	/*
	if (!sym)
		return;
	if (sym->RecentMonth() && sym->RequestedSymbolMaster())
		return;
	sym->GetSymbolMaster();
	*/
	//VtRealtimeRegisterManager* realTimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	//realTimeRegiMgr->RegisterProduct(sym->ShortCode);
}

void SmFutureGrid::QuickSetBackColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetBackClr(color);
	}
}

void SmFutureGrid::QuickSetText(int row, int col, LPCTSTR text)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetText(text);
		InvalidateCellRect(row, col);
	}
}

void SmFutureGrid::QuickSetNumber(int row, int col, int number)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		CString text;
		text.Format("%d", number);
		pCell->SetText(text);
	}
}

void SmFutureGrid::QuickSetTextColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetTextClr(color);
	}
}

void SmFutureGrid::OnReceiveQuote(symbol_p sym)
{
	if (!sym)
		return;

	if (_Mode == 0) {
		ShowRemain(sym);
	}
	else if (_Mode == 1) {
		CUGCell cell;
		/*
		auto it = _FutureCurrentValueRowMap.find(sym->ShortCode);
		if (it != _FutureCurrentValueRowMap.end()) {
			auto pos = _FutureCurrentValueRowMap[sym->ShortCode];
			ShowCurrent(sym, std::get<0>(pos));
		}
		*/
	}
}

void SmFutureGrid::OnSymbolMaster(symbol_p sym)
{
	if (!sym)
		return;
	if (_Mode != 1)
		return;

	CUGCell cell;
	/*
	auto it = _FutureCurrentValueRowMap.find(sym->ShortCode);
	if (it != _FutureCurrentValueRowMap.end()) {
		auto pos = _FutureCurrentValueRowMap[sym->ShortCode];
		ShowCurrent(sym, std::get<0>(pos));
	}
	*/
}

void SmFutureGrid::SetRemain(VtPosition* posi)
{
	if (!posi)
		return;

	if (_Mode != 0)
		return;
	/*
	symbol_p sym = mainApp.SymbolMgr().FindSymbol(posi->ShortCode);
	if (sym) {
		ShowRemain(sym);
	}
	*/
}

void SmFutureGrid::SetRemain(VtOrder* order)
{
	if (!order || _Mode != 0 || !_OrderConfigMgr)
		return;
	/*
	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtOrderManager* orderMgr = mainApp.TotalOrderMgr().FindAddAccountOrderManger(acnt->AccountNo);
		VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(order->shortCode);
		VtPosition* posi = acnt->FindPosition(order->shortCode);
		if (prdtOrderMgr)
			ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, order->shortCode);
		else
			ShowPosition(false, 0, nullptr, order->shortCode);
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

void SmFutureGrid::SetRemain(std::string symbol_code)
{
	/*
	symbol_p sym = mainApp.SymbolMgr().FindSymbol(symbol_code);
	if (!sym)
		return;

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
	*/
}

void SmFutureGrid::ShowRemain(symbol_p sym)
{
	if (!sym)
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
	*/
}

void SmFutureGrid::ShowCurrent(symbol_p sym, int row)
{
	if (!sym)
		return;
	/*
	std::string val = fmt::format("{:.{}f}", sym->Quote.intClose / std::pow(10, sym->Decimal), sym->Decimal);
	CString thVal = XFormatNumber(val.c_str(), sym->Decimal);
	QuickSetText(row, 2, thVal);
	if (sym->Quote.intOpen == 0) {
		QuickSetTextColor(row, 2, RGB(0, 0, 0));
	}
	else {
		if (sym->Quote.intClose > sym->Quote.intOpen) {
			QuickSetTextColor(row, 2, RGB(255, 0, 0));
		}
		else if (sym->Quote.intClose < sym->Quote.intOpen) {
			QuickSetTextColor(row, 2, RGB(0, 0, 255));
		}
		else {
			QuickSetTextColor(row, 2, RGB(0, 0, 0));
		}
	}
	QuickSetBackColor(row, 2, RGB(255, 255, 255));
	InvalidateCellRect(row, 2);
	*/
}

void SmFutureGrid::OnOrderFilled(VtOrder* order)
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

void SmFutureGrid::OnExpected(symbol_p sym)
{
	if (!sym)
		return;
	/*
	if (_Mode == 2) {
		CUGCell cell;
		auto it = _FutureCurrentValueRowMap.find(sym->ShortCode);
		if (it != _FutureCurrentValueRowMap.end()) {
			auto pos = _FutureCurrentValueRowMap[sym->ShortCode];
			ShowExpected(sym, std::get<0>(pos));
		}
	}
	*/
}

void SmFutureGrid::ShowExpected(symbol_p sym, int row)
{
	if (!sym)
		return;
	/*
	std::string val = fmt::format("{:.{}f}", sym->Quote.intExpected / std::pow(10, sym->Decimal), sym->Decimal);
	CString thVal = XFormatNumber(val.c_str(), sym->Decimal);
	QuickSetText(row, 2, thVal);
	QuickSetTextColor(row, 2, RGB(0, 0, 0));
	QuickSetBackColor(row, 2, RGB(255, 255, 255));
	InvalidateCellRect(row, 2);
	*/
}

void SmFutureGrid::update_expected_direct(std::shared_ptr<DarkHorse::SmQuote> quote)
{
	if (view_mode_ != ViewMode::VM_Expected) return;

	auto found = symbol_row_index_map_.find(quote->symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.expected = quote->expected;
	show_value(found->second, 2, future_info);
	enable_show_ = true;
}

void SmFutureGrid::update_quote_direct(std::shared_ptr<DarkHorse::SmQuote> quote)
{
	if (view_mode_ != ViewMode::VM_Close) return;

	auto found = symbol_row_index_map_.find(quote->symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.close = quote->close;
	show_value(found->second, 2, future_info);
	enable_show_ = true;
}

void SmFutureGrid::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	_Fund = val;
	order_type_ = OrderType::Fund;
	position_control_->set_fund(_Fund);
	LOGINFO(CMyLogger::getInstance(), "fund[%s]", _Fund->Name().c_str());
	for (auto& future_info : symbol_vec_) {
		get_future_info(future_info);
		if (!future_info.symbol_p) continue;
		auto found = symbol_row_index_map_.find(future_info.symbol_p->SymbolCode());
		if (found == symbol_row_index_map_.end()) continue;
		show_value(found->second, 2, future_info);
	}

	enable_show_ = true;
}

void SmFutureGrid::init_dm_future()
{
	const std::vector<DmFuture>& future_vec = mainApp.SymMgr()->get_dm_future_vec();
	for (size_t i = 0; i < future_vec.size(); i++) {
		const std::map<std::string, std::shared_ptr<SmProductYearMonth>>& year_month_map = future_vec[i].product->get_yearmonth_map();
		std::shared_ptr<SmProductYearMonth> year_month = year_month_map.begin()->second;
		auto symbol = year_month->get_first_symbol();
		if (symbol) {
			auto quote = mainApp.QuoteMgr()->get_quote(symbol->SymbolCode());
			VmFuture future_info;
			future_info.decimal = symbol->decimal();
			future_info.close = quote->close;
			future_info.expected = quote->expected;
			future_info.ordered = false;
			future_info.position = 0;
			future_info.symbol_id = symbol->Id();
			future_info.symbol_p = symbol;
			future_info.symbol_code = symbol->SymbolCode();
			get_future_info(future_info);
			symbol_vec_.push_back(future_info);
			symbol_row_index_map_[symbol->SymbolCode()] = i;
		}
		std::string value = future_vec[i].future_name;
		QuickSetText(i, 0, value.c_str());
		value = future_vec[i].product_code;
		QuickSetText(i, 1, value.c_str());
	}
	register_symbols();
	show_values();
	Invalidate();
}

void SmFutureGrid::update_quote()
{
	const VmQuote quote = quote_control_->get_quote();
	CString msg;
	msg.Format("DmFutureView::update_quote ::  close : %d\n", quote.close);
	//TRACE(msg);
	if (view_mode_ != ViewMode::VM_Close) return;
	update_close(quote);
}

void SmFutureGrid::set_view_mode(ViewMode view_mode)
{
	if (view_mode == view_mode_) return;
	view_mode_ = view_mode;
	show_values();
	Invalidate();
}

void SmFutureGrid::on_update_position_vm_future(const VmPosition& position)
{
	if (!position_control_) return;
	if (position_control_ && position_control_->Position_type() != position.position_type) return;
	auto found = symbol_row_index_map_.find(position.symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.position = position.open_quantity;
	//show_value(found->second, 2, future_info);
}

void SmFutureGrid::refresh_values()
{
	bool needDraw = false;
	if (enable_show_) {
		if (view_mode_ == ViewMode::VM_Close) {
			update_quote();
		}
		else
			update_position();

		enable_show_ = false;
		needDraw = true;
	}
}

void SmFutureGrid::update_order(order_p order, DarkHorse::OrderEvent order_event)
{
	auto found = symbol_row_index_map_.find(order->symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	get_init_accepted_order_count(future_info);
	show_value(found->second, 2, future_info);
	enable_show_ = true;
}

void SmFutureGrid::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	_Account = val;
	position_control_->set_account(_Account);

	LOGINFO(CMyLogger::getInstance(), "account[%s]", _Account->No().c_str());

	if (_Account->is_subaccount())
		order_type_ = OrderType::SubAccount;
	else
		order_type_ = OrderType::MainAccount;

	for (auto& future_info : symbol_vec_) {
		get_future_info(future_info);
		if (!future_info.symbol_p) continue;
		auto found = symbol_row_index_map_.find(future_info.symbol_p->SymbolCode());
		if (found == symbol_row_index_map_.end()) continue;
		show_value(found->second, 2, future_info);
		LOGINFO(CMyLogger::getInstance(), "symbol_code[%s], remain=[%d]", future_info.symbol_p->SymbolCode().c_str(), future_info.position);
	}

	enable_show_ = true;
}

std::string SmFutureGrid::get_position_text(const DarkHorse::VmFuture& future_info)
{
	if (future_info.position != 0)
		return std::to_string(future_info.position);
	else if (future_info.accepted_count > 0 ||
		future_info.ordered)
		return "0";
	else return "";
}

void SmFutureGrid::get_future_info(DarkHorse::VmFuture& future_info)
{
	get_init_accepted_order_count(future_info);
	if (order_type_ == OrderType::None) return;
	if (order_type_ == OrderType::MainAccount) {
		VmPosition position;
		mainApp.total_position_manager()->get_position_from_parent_account(future_info.account_no, future_info.symbol_code, position);
		future_info.position = position.open_quantity;
	}
	else if (order_type_ == OrderType::SubAccount) {
		VmPosition position;
		mainApp.total_position_manager()->get_position_from_account(future_info.account_no, future_info.symbol_code, position);
		future_info.position = position.open_quantity;
	}
	else if (order_type_ == OrderType::Fund) {
		VmPosition position;
		mainApp.total_position_manager()->get_position_from_fund(future_info.fund_name, future_info.symbol_code, position);
		future_info.position = position.open_quantity;
	}
}

void SmFutureGrid::get_init_accepted_order_count(DarkHorse::VmFuture& future_info)
{
	if (order_type_ == OrderType::None) return;
	if (order_type_ == OrderType::MainAccount) {
		if (!_Account) return;
		future_info.account_no = _Account->No();
		future_info.fund_name = "";
		auto init_and_count = mainApp.total_order_manager()->get_init_and_acpt_order_count_from_parent_account(future_info.account_no, future_info.symbol_code);
		future_info.ordered = init_and_count.first;
		future_info.accepted_count = init_and_count.second;
	}
	else if (order_type_ == OrderType::SubAccount) {
		if (!_Account) return;
		future_info.account_no = _Account->No();
		future_info.fund_name = "";
		auto init_and_count = mainApp.total_order_manager()->get_init_and_acpt_order_count_from_account(future_info.account_no, future_info.symbol_code);
		future_info.ordered = init_and_count.first;
		future_info.accepted_count = init_and_count.second;
	}
	else if (order_type_ == OrderType::Fund) {
		if (!_Fund) return;
		future_info.fund_name = _Fund->Name();
		future_info.account_no = "";
		auto init_and_count = mainApp.total_order_manager()->get_init_and_acpt_order_count_from_fund(future_info.fund_name, future_info.symbol_code);
		future_info.ordered = init_and_count.first;
		future_info.accepted_count = init_and_count.second;
	}
}

void SmFutureGrid::set_position(DarkHorse::VmFuture& future_info)
{
	if (!_Account || !future_info.symbol_p) return;

	auto account_position_manager = mainApp.total_position_manager()->get_account_position_manager(_Account->No());

	auto position = account_position_manager->find_position(future_info.symbol_p->SymbolCode());
	if (!position) return;
	future_info.position = position->open_quantity;
}

void SmFutureGrid::update_position()
{
	if (!position_control_) return;

	const VmPosition& position = position_control_->get_position();

	LOGINFO(CMyLogger::getInstance(), "position control :: 계좌[%s],펀드이름[%s],종목[%s]", 
		position_control_->Account() ? position_control_->Account()->No().c_str() : "",
		position_control_->Fund() ? position_control_->Fund()->Name().c_str() : "",
		position_control_->Symbol() ? position_control_->Symbol()->SymbolNameKr().c_str() : "");

	LOGINFO(CMyLogger::getInstance(), "_OrderConfigMgr :: Type[%d], 계좌[%s],펀드이름[%s]", _OrderConfigMgr->Type(), 
		_OrderConfigMgr->Account() ? _OrderConfigMgr->Account()->No().c_str() : "",
		_OrderConfigMgr->Fund() ? _OrderConfigMgr->Fund()->Name().c_str() : "");

	auto found = symbol_row_index_map_.find(position.symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.position = position.open_quantity;
	show_value(found->second, 2, future_info);
}

void SmFutureGrid::on_update_position()
{
	if (!position_control_) return;

	const VmPosition& position = position_control_->get_position();

	LOGINFO(CMyLogger::getInstance(), "_OrderConfigMgr :: Type[%d], 계좌[%s],펀드이름[%s]", _OrderConfigMgr->Type(),
		_OrderConfigMgr->Account() ? _OrderConfigMgr->Account()->No().c_str() : "",
		_OrderConfigMgr->Fund() ? _OrderConfigMgr->Fund()->Name().c_str() : "");

	auto found = symbol_row_index_map_.find(position.symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.position = position.open_quantity;
	//show_value(found->second, 2, future_info);

	enable_show_ = true;
}

void SmFutureGrid::on_update_position_direct(position_p position)
{

// 	LOGINFO(CMyLogger::getInstance(), "_OrderConfigMgr :: Type[%d], 계좌[%s],펀드이름[%s]", _OrderConfigMgr->Type(),
// 		_OrderConfigMgr->Account() ? _OrderConfigMgr->Account()->No().c_str() : "",
// 		_OrderConfigMgr->Fund() ? _OrderConfigMgr->Fund()->Name().c_str() : "");

	if (!_OrderConfigMgr) return;
	if (_OrderConfigMgr->Type() == 0 && _Account) {
		if (position->account_no != _Account->No()) return;
	}
	else if (_OrderConfigMgr->Type() == 1 && _Fund) {
		if (position->fund_name != _Fund->Name()) return;
	}

	auto found = symbol_row_index_map_.find(position->symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.position = position->open_quantity;
	show_value(found->second, 2, future_info);
}

void SmFutureGrid::on_update_quote()
{
	enable_show_ = true;
}

void SmFutureGrid::update_close(const DarkHorse::VmQuote& quote)
{
	auto found = symbol_row_index_map_.find(quote.symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.close = quote.close;
	show_value(found->second, 2, future_info);
}

void SmFutureGrid::show_values()
{
	for (int i = 0; i < 5; i++) {
		const VmFuture& future_info = symbol_vec_[i];
		show_value(i, 2, future_info);
	}
}

void SmFutureGrid::show_value(const int row, const int col, const DarkHorse::VmFuture& future_info)
{
	std::string value;
	if (view_mode_ == ViewMode::VM_Close) {
		value = std::to_string(future_info.close);
		SmUtil::insert_decimal(value, future_info.decimal);
	}
	else if (view_mode_ == ViewMode::VM_Expected) {
		value = std::to_string(future_info.expected);
		SmUtil::insert_decimal(value, future_info.decimal);
	}
	else {
		value = get_position_text(future_info);
	}
	set_background_color(row, col, future_info);
	QuickSetText(row, col, value.c_str());
}

void SmFutureGrid::set_background_color(const int row, const int col, const DarkHorse::VmFuture& future_info)
{
	if (future_info.accepted_count > 0)
		QuickSetBackColor(row, col, RGB(212, 186, 188));
	else if (future_info.position != 0) {
		if (future_info.accepted_count > 0)
			QuickSetBackColor(row, col, RGB(212, 186, 188));
		else
			QuickSetBackColor(row, col, RGB(255, 255, 255));
	}
	else if (future_info.ordered)
		QuickSetBackColor(row, col, RGB(255, 255, 255));
// 	else if (future_info.call_put == 1)
// 		QuickSetBackColor(row, col, RGB(252, 226, 228));
	else
		QuickSetBackColor(row, col, RGB(218, 226, 245));
}

void SmFutureGrid::register_symbol_to_server(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;
	mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
}

void SmFutureGrid::register_symbols()
{
	if (registered_) return;

	for (auto& symbol : symbol_vec_) {
		register_symbol_to_server(symbol.symbol_p);
	}
	registered_ = true;
}
