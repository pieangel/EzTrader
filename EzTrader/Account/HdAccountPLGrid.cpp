#include "stdafx.h"
#include "HdAccountPLGrid.h"
#include "HdAccountPLDlg.h"

#include "../Symbol/SmSymbol.h"
#include "../Account/SmAccount.h"

#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Controller/AccountProfitLossControl.h"
#include <format>

#include <functional>
#include "../Fund/SmFund.h"
#include "../Util/VtStringUtil.h"

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;


HdAccountPLGrid::HdAccountPLGrid()
{
	account_profit_loss_control_ = std::make_shared<DarkHorse::AccountProfitLossControl>();
	account_profit_loss_control_->set_event_handler(std::bind(&HdAccountPLGrid::on_update_account_profit_loss, this));

	_AcntPLDlg = nullptr;
}


HdAccountPLGrid::~HdAccountPLGrid()
{
}


void HdAccountPLGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetUGMode(UGMode::Normal);
	SetVScrollMode(UG_SCROLLJOYSTICK);

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);
	//_RowCount = GetMaxRow();
	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	SetSH_Width(60);
	SetHS_Height(0);
	SetVS_Width(0);
	SetTH_Height(0);

	QuickSetAlignment(0, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	for (int i = 0; i < _RowCount; i++)
	{
		QuickSetAlignment(0, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	}

	SetDefFont(&_defFont);


	SetRowTitle();

	update_account_profit_loss();

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.right = rcWnd.left + 192;
	SetWindowPos(nullptr, 0, 0, rcWnd.Width(), rcWnd.Height(), SWP_NOMOVE);
}

void HdAccountPLGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void HdAccountPLGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void HdAccountPLGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	CUGCell cell;
	LPCTSTR title[4] = { "평가손익", "실현손익", "수수료", "순손익" };
	SetColWidth(-1, 66);
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.right = rcWnd.left + 192;
	SetColWidth(0, rcWnd.Width() - 66);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetText(-1, i, title[i]);
		GetCell(-1, i, &cell);
		//cell.SetBackColor(VtGlobal::GridTitleBackColor);
		//cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(-1, i, &cell);
		QuickSetAlignment(-1, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetFont(-1, i, &_titleFont);
		RedrawCell(-1, i);
	}
}

void HdAccountPLGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void HdAccountPLGrid::Begin()
{

}

void HdAccountPLGrid::End()
{

}

void HdAccountPLGrid::InitGrid()
{
	if (!_AcntPLDlg)
		return;
	ClearValues();
	enable_account_profit_loss_show_ = true;
}

void HdAccountPLGrid::ClearValues()
{
	QuickSetText(0, 0, _T(""));
	QuickSetText(0, 1, _T(""));
	QuickSetText(0, 2, _T(""));
	QuickSetText(0, 3, _T(""));

	QuickRedrawCell(0, 0);
	QuickRedrawCell(0, 1);
	QuickRedrawCell(0, 2);
	QuickRedrawCell(0, 3);
}



void HdAccountPLGrid::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	fund_ = val;
	if (!account_profit_loss_control_) return;
	account_profit_loss_control_->set_fund(fund_);
	//update_fund_profit_loss();
	enable_account_profit_loss_show_ = true;
}

void HdAccountPLGrid::on_update_account_profit_loss()
{
	enable_account_profit_loss_show_ = true;
}

void HdAccountPLGrid::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	account_ = val;

	if (!account_profit_loss_control_) return;
	account_profit_loss_control_->set_account(account_);
	//update_account_profit_loss();
	enable_account_profit_loss_show_ = true;
}

void HdAccountPLGrid::UpdateSymbolInfo()
{
	if (!_Symbol) return;

// 	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
// 	if (cell) cell->Text(_Symbol->SymbolCode());
// 	cell = _Grid->FindCell(1, 4);
// 	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}



void HdAccountPLGrid::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void HdAccountPLGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	enable_account_profit_loss_show_ = true;
}

std::string HdAccountPLGrid::get_format_price(const double& value)
{
	std::string value_string;
	CString format_price;
	if (account_->Type() == "1")
		format_price.Format("%.2f", value);
	else
		format_price.Format("%.0f", value);
	return std::string(CT2CA(format_price));
}

void HdAccountPLGrid::update_account_profit_loss()
{
	if (!account_profit_loss_control_) return;
	if (!account_) return;
	if (updating_) return;
	updating_ = true;
	std::string format_type("0");
	if (account_) format_type = account_->Type();
	if (fund_) format_type = fund_->fund_type();

	const VmAccountProfitLoss& account_profit_loss = account_profit_loss_control_->get_account_profit_loss();
	const int decimal = format_type == "1" ? 2 : 0;
	std::string value;
	value = VtStringUtil::get_format_value(account_profit_loss.open_profit_loss, decimal, true);

	if (account_profit_loss.open_profit_loss > 0) {
		QuickSetTextColor(0, 0, RGB(255, 0, 0));
		QuickSetText(0, 0, value.c_str());
	}
	else if (account_profit_loss.open_profit_loss < 0) {
		QuickSetTextColor(0, 0, RGB(0, 0, 255));
		QuickSetText(0, 0, value.c_str());
	}
	else {
		QuickSetTextColor(0, 0, RGB(0, 0, 0));
		QuickSetNumber(0, 0, 0);
	}

	value = VtStringUtil::get_format_value(account_profit_loss.trade_profit_loss, decimal, true);
	if (account_profit_loss.trade_profit_loss > 0) {
		QuickSetTextColor(0, 1, RGB(255, 0, 0));
		QuickSetText(0, 1, value.c_str());
	}
	else if (account_profit_loss.trade_profit_loss < 0) {
		QuickSetTextColor(0, 1, RGB(0, 0, 255));
		QuickSetText(0, 1, value.c_str());
	}
	else {
		QuickSetTextColor(0, 1, RGB(0, 0, 0));
		QuickSetNumber(0, 1, 0);
	}

	value = VtStringUtil::get_format_value(account_profit_loss.trade_fee, decimal, true);

	QuickSetText(0, 2, value.c_str());

	const double pure_profit = account_profit_loss.open_profit_loss + account_profit_loss.trade_profit_loss - abs(account_profit_loss.trade_fee);
	value = VtStringUtil::get_format_value(pure_profit, decimal, true);

	if (pure_profit > 0) {
		QuickSetTextColor(0, 3, RGB(255, 0, 0));
		QuickSetText(0, 3, value.c_str());
	}
	else if (pure_profit < 0) {
		QuickSetTextColor(0, 3, RGB(0, 0, 255));
		QuickSetText(0, 3, value.c_str());
	}
	else {
		QuickSetTextColor(0, 3, RGB(0, 0, 0));
		QuickSetNumber(0, 3, 0);
	}

	QuickRedrawCell(0, 0);
	QuickRedrawCell(0, 1);
	QuickRedrawCell(0, 2);
	QuickRedrawCell(0, 3);

	updating_ = false;
}

void HdAccountPLGrid::update_fund_profit_loss()
{
	if (!fund_) return;

	if (updating_) return;
	updating_ = true;
	const std::vector<std::shared_ptr<SmAccount>>& account_vec = fund_->GetAccountVector();

	double open_pl = 0.0, settled_pl = 0.0, fee = 0.0, pure_pl = 0.0;
	for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
		open_pl += (*it)->Asset.OpenProfitLoss;
		settled_pl += (*it)->Asset.TradeProfitLoss;
		fee += (*it)->Asset.Fee;
		pure_pl = open_pl + settled_pl - abs(fee);
	}

	std::string value;
	std::string format_type("0");
	if (account_) format_type = account_->Type();
	if (fund_) format_type = fund_->fund_type();
	const int decimal = format_type == "1" ? 2 : 0;

	if (open_pl > 0) {
		QuickSetTextColor(0, 0, RGB(255, 0, 0));
		QuickSetText(0, 0, value.c_str());
	}
	else if (open_pl < 0) {
		QuickSetTextColor(0, 0, RGB(0, 0, 255));
		QuickSetText(0, 0, value.c_str());
	}
	else {
		QuickSetTextColor(0, 0, RGB(0, 0, 0));
		QuickSetNumber(0, 0, 0);
	}

	value = VtStringUtil::get_format_value(settled_pl, decimal, true);
	if (settled_pl > 0) {
		QuickSetTextColor(0, 1, RGB(255, 0, 0));
		QuickSetText(0, 1, value.c_str());
	}
	else if (settled_pl < 0) {
		QuickSetTextColor(0, 1, RGB(0, 0, 255));
		QuickSetText(0, 1, value.c_str());
	}
	else {
		QuickSetTextColor(0, 1, RGB(0, 0, 0));
		QuickSetNumber(0, 1, 0);
	}

	value = VtStringUtil::get_format_value(fee, decimal, true);

	QuickSetText(0, 2, value.c_str());

	value = VtStringUtil::get_format_value(pure_pl, decimal, true);

	if (pure_pl > 0) {
		QuickSetTextColor(0, 3, RGB(255, 0, 0));
		QuickSetText(0, 3, value.c_str());
	}
	else if (pure_pl < 0) {
		QuickSetTextColor(0, 3, RGB(0, 0, 255));
		QuickSetText(0, 3, value.c_str());
	}
	else {
		QuickSetTextColor(0, 3, RGB(0, 0, 0));
		QuickSetNumber(0, 3, 0);
	}

	QuickRedrawCell(0, 0);
	QuickRedrawCell(0, 1);
	QuickRedrawCell(0, 2);
	QuickRedrawCell(0, 3);

	updating_ = false;
}

void HdAccountPLGrid::refresh()
{
	if (enable_account_profit_loss_show_) {
		update_account_profit_loss();
		enable_account_profit_loss_show_ = false;
	}
	enable_account_profit_loss_show_ = false;
}

void HdAccountPLGrid::OnTimer(UINT_PTR nIDEvent)
{
	bool needDraw = false;

	if (enable_account_profit_loss_show_) {
		update_account_profit_loss();
		enable_account_profit_loss_show_ = false;
		needDraw = true;
	}
	if (needDraw) Invalidate();

	VtGrid::OnTimer(nIDEvent);
}

void HdAccountPLGrid::OnLButtonDown(UINT nFlags, CPoint point)
{

}


