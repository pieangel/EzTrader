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
#include "../Symbol/SmSymbol.h"


#include "../Global/SmTotalManager.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Controller/QuoteControl.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Util/SmUtil.h"
#include "../Controller/SymbolPositionControl.h"
#include "../ViewModel/VmPosition.h"
#include "../Account/SmAccountManager.h"
#include "../Util/VtStringUtil.h"
#include "../Util/SimpleTree.h"
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



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

void VtProductRemainGrid::OnQuoteEvent(const symbol_p symbol)
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

void VtProductRemainGrid::OnQuoteEvent(const std::string& symbol_code)
{

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

void VtProductRemainGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	enable_position_show_ = true;
}

void VtProductRemainGrid::on_update_quote()
{
	enable_quote_show_ = true;
}

void VtProductRemainGrid::on_update_position()
{
	enable_position_show_ = true;
	enable_quote_show_ = true;
}

void VtProductRemainGrid::set_position()
{
	if (!position_control_ || !symbol_) return;
	position_control_->reset_position();
}

void VtProductRemainGrid::update_position()
{
	if (!position_control_ || !symbol_) return;

	const VmPosition& position = position_control_->get_position();

	if (position.open_quantity == 0) {
		ClearPosition();
		return;
	}
	QuickSetText(0, 0, symbol_->SymbolCode().c_str());
	QuickRedrawCell(0, 0);
	if (position.open_quantity > 0) {
		QuickSetText(1, 0, _T("매수"));
		QuickSetTextColor(1, 0, RGB(255, 0, 0));
		QuickSetTextColor(2, 0, RGB(255, 0, 0));
		QuickSetTextColor(3, 0, RGB(255, 0, 0));
	}
	else if (position.open_quantity < 0) {
		QuickSetText(1, 0, _T("매도"));
		QuickSetTextColor(1, 0, RGB(0, 0, 255));
		QuickSetTextColor(2, 0, RGB(0, 0, 255));
		QuickSetTextColor(3, 0, RGB(0, 0, 255));
	}
	else {
		QuickSetText(1, 0, _T(""));
		QuickSetTextColor(1, 0, RGB(255, 255, 255));
		QuickSetTextColor(2, 0, RGB(255, 255, 255));
		QuickSetTextColor(3, 0, RGB(255, 255, 255));
	}
	QuickRedrawCell(1, 0);
	QuickSetNumber(2, 0, std::abs(position.open_quantity));
	QuickRedrawCell(2, 0);

	const int decimal = symbol_type_ == DarkHorse::SymbolType::Abroad ? 2 : 0;
	std::string value_string = VtStringUtil::get_format_value(position.average_price / pow(10, symbol_->decimal()), symbol_->decimal(), true);
	QuickSetText(3, 0, value_string.c_str());
	QuickRedrawCell(3, 0);
	value_string = VtStringUtil::get_format_value(position.open_profit_loss, decimal, true);
	if (position.open_profit_loss > 0) {
		QuickSetTextColor(5, 0, RGB(255, 0, 0));
		QuickSetText(5, 0, value_string.c_str());
	}
	else if (position.open_profit_loss < 0) {
		QuickSetTextColor(5, 0, RGB(0, 0, 255));
		QuickSetText(5, 0, value_string.c_str());
	}
	else {
		QuickSetTextColor(5, 0, RGB(0, 0, 0));
		QuickSetText(5, 0, value_string.c_str());
	}
	QuickRedrawCell(5, 0);
}

VtProductRemainGrid::VtProductRemainGrid()
{
	_CenterWnd = nullptr;
	_OrderConfigMgr = nullptr;

	quote_control_ = std::make_shared<DarkHorse::QuoteControl>();
	quote_control_->set_event_handler(std::bind(&VtProductRemainGrid::on_update_quote, this));
	position_control_ = std::make_shared<DarkHorse::SymbolPositionControl>();
	position_control_->set_event_handler(std::bind(&VtProductRemainGrid::on_update_position, this));
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

void VtProductRemainGrid::ShowPosition(VtPosition* posi, symbol_p sym)
{
	if (!posi || !sym)
		return;
}
/*
void VtProductRemainGrid::SetSymbol(symbol_p sym)
{
	if (!sym || !_CenterWnd || !_OrderConfigMgr)
		return;

	
	account_p acnt = _OrderConfigMgr->Account();
	if (!acnt)
		return;
	VtPosition* posi = acnt->FindPosition(sym->ShortCode);
	if (!posi)
		return;

	QuickSetText(0, 0, sym->ShortCode.c_str());
	QuickRedrawCell(0, 0);
	
}
*/

void VtProductRemainGrid::ShowSinglePosition()
{
	if (!_OrderConfigMgr->Account())
		return;

	symbol_p sym = _CenterWnd->Symbol();
	account_p acnt = _OrderConfigMgr->Account();
	//VtPosition* posi = acnt->FindPosition(sym->ShortCode);
	//ShowPosition(posi, sym);
}

void VtProductRemainGrid::ShowFundPosition()
{
	if (!_OrderConfigMgr->Fund())
		return;

	symbol_p sym = _CenterWnd->Symbol();
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
	set_position();
	update_position();
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

	if (_CenterWnd) _CenterWnd->SetRemain(0);
}

void VtProductRemainGrid::OnReceiveQuote(symbol_p sym)
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

void VtProductRemainGrid::Clear()
{

}

void VtProductRemainGrid::Symbol(symbol_p val)
{
	symbol_ = val;
	auto quote = mainApp.QuoteMgr()->get_quote(symbol_->SymbolCode());
	quote->symbol_id = symbol_->Id();
	quote_control_->set_symbol_id(symbol_->Id());
	quote_control_->update_quote(quote);
	position_control_->set_symbol(symbol_);
	update_quote();
	set_position();
	update_position();
	enable_position_show_ = true;
}


void VtProductRemainGrid::update_quote()
{
	if (!quote_control_ || !symbol_) return;
	//if (!position_control_) return;
	const VmQuote& quote = quote_control_->get_quote();
	//const VmPosition& position = position_control_->get_position();
	std::string value = std::to_string(quote.close);
	SmUtil::insert_decimal(value, symbol_->decimal());
	CUGCell cell;
	GetCell(4, 0, &cell);
	cell.SetText(value.c_str());
	//cell.LongValue(quote.close);
	SetCell(4, 0, &cell);
	QuickRedrawCell(4, 0);
}

void VtProductRemainGrid::Account(account_p val)
{
	account_ = val;
	if (account_->is_subaccount())
		position_type_ = PositionType::SubAccount;
	else
		position_type_ = PositionType::MainAccount;
	position_control_->set_account(account_);
	set_position();
	update_position();
	update_quote();
	enable_position_show_ = true;
}

void VtProductRemainGrid::fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	fund_ = val;
	position_type_ = PositionType::Fund;
	position_control_->set_fund(fund_);
	set_position();
	update_position();
	enable_position_show_ = true;
}

void VtProductRemainGrid::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

LRESULT VtProductRemainGrid::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	symbol_p* pData = reinterpret_cast<symbol_p*>(lParam);
	symbol_p symbol = *pData;
		
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
