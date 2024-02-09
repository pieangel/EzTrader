#include "stdafx.h"
#include "SmPLGrid.h"
#include "VtOrderConfigManager.h"
#include "../Global/SmTotalManager.h"
#include "../MessageDefine.h"
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SmPLGrid::SmPLGrid()
{
	account_profit_loss_control_ = std::make_shared<DarkHorse::AccountProfitLossControl>();
	account_profit_loss_control_->set_event_handler(std::bind(&SmPLGrid::on_update_account_profit_loss, this));
}


SmPLGrid::~SmPLGrid()
{
	//mainApp.CallbackMgr().UnsubscribeQuoteWndCallback(GetSafeHwnd());
}


BEGIN_MESSAGE_MAP(SmPLGrid, CGridCtrl)
	ON_MESSAGE(WM_QUOTE_CHANGED, OnQuoteChangedMessage)
END_MESSAGE_MAP()

void SmPLGrid::Init()
{
	_CellHeight = 20;
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetRowCount(_RowCount);
	SetColumnCount(_ColCount);
	SetFixedRowCount(0);
	SetFixedColumnCount(0);
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
		for (int j = 0; j < _ColCount; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				if (j == 0)
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				else // 텍스트 정렬
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				// 셀 배경 색 설정
				pCell->SetBackClr(RGB(255, 255, 255));
				// 셀 글자색 설정
				pCell->SetTextClr(RGB(0, 0, 0));

				InvalidateCellRect(i, j);
			}
		}
	}

	SetFont(&_defFont);

	SetRowTitle();

	//mainApp.CallbackMgr().SubscribeQuoteWndCallback(GetSafeHwnd());
}

void SmPLGrid::SetRowTitle()
{
	LPCTSTR title[3] = { "평가손익", "실현손익", "총손익" };
	SetColumnWidth(0, 60);
	SetColumnWidth(1, 110);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetText(i, 0, title[i]);
		QuickSetBackColor(i, 0, DarkHorse::SmTotalManager::GridTitleBackColor);
		QuickSetTextColor(i, 0, DarkHorse::SmTotalManager::GridTitleTextColor);
		InvalidateCellRect(i, 0);
	}
}


void SmPLGrid::QuickSetBackColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetBackClr(color);
	}
}

void SmPLGrid::QuickSetText(int row, int col, LPCTSTR text)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetText(text);
	}
}

void SmPLGrid::QuickSetNumber(int row, int col, int number)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		CString text;
		text.Format("%d", number);
		pCell->SetText(text);
	}
}

void SmPLGrid::QuickSetTextColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetTextClr(color);
	}
}


void SmPLGrid::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	fund_ = val;
	if (!account_profit_loss_control_) return;
	account_profit_loss_control_->set_fund(fund_);
	enable_account_profit_loss_show_ = true;
}

void SmPLGrid::on_update_account_profit_loss()
{
	enable_account_profit_loss_show_ = true;
}

void SmPLGrid::Account(account_p val)
{
	account_ = val;

	if (!account_profit_loss_control_) return;
	account_profit_loss_control_->set_account(account_);
	enable_account_profit_loss_show_ = true;
}

void SmPLGrid::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SmPLGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	enable_account_profit_loss_show_ = true;
}

void SmPLGrid::update_account_profit_loss()
{
	if (!account_profit_loss_control_) return;
	std::string format_type("0");
	if (account_) format_type = account_->Type();
	if (fund_) format_type = fund_->fund_type();

	const VmAccountProfitLoss& account_profit_loss = account_profit_loss_control_->get_account_profit_loss();
	const int decimal = format_type == "1" ? 2 : 0;
	std::string value;
	value = VtStringUtil::get_format_value(account_profit_loss.open_profit_loss, decimal, true);
	if (account_profit_loss.open_profit_loss > 0) {
		QuickSetTextColor(0, 1, RGB(255, 0, 0));
		QuickSetText(0, 1, value.c_str());
	}
	else if (account_profit_loss.open_profit_loss < 0) {
		QuickSetTextColor(0, 1, RGB(0, 0, 255));
		QuickSetText(0, 1, value.c_str());
	}
	else {
		QuickSetTextColor(0, 1, RGB(0, 0, 0));
		QuickSetNumber(0, 1, 0);
	}

	value = VtStringUtil::get_format_value(account_profit_loss.trade_profit_loss, decimal, true);
	if (account_profit_loss.trade_profit_loss > 0) {
		QuickSetTextColor(1, 1, RGB(255, 0, 0));
		QuickSetText(1, 1, value.c_str());
	}
	else if (account_profit_loss.trade_profit_loss < 0) {
		QuickSetTextColor(1, 1, RGB(0, 0, 255));
		QuickSetText(1, 1, value.c_str());
	}
	else {
		QuickSetTextColor(1, 1, RGB(0, 0, 0));
		QuickSetNumber(1, 1, 0);
	}

	//value = VtStringUtil::get_format_value(account_profit_loss.trade_fee, decimal, true);
	const double pure_profit = account_profit_loss.open_profit_loss + account_profit_loss.trade_profit_loss - abs(account_profit_loss.trade_fee);
	value = VtStringUtil::get_format_value(pure_profit, decimal, true);
	if (pure_profit > 0) {
		QuickSetTextColor(2, 1, RGB(255, 0, 0));
		QuickSetText(2, 1, value.c_str());
	}
	else if (pure_profit < 0) {
		QuickSetTextColor(2, 1, RGB(0, 0, 255));
		QuickSetText(2, 1, value.c_str());
	}
	else {
		QuickSetTextColor(2, 1, RGB(0, 0, 0));
		QuickSetNumber(2, 1, 0);
	}
	InvalidateCellRect(0, 1);
	InvalidateCellRect(1, 1);
	InvalidateCellRect(2, 1);
	enable_account_profit_loss_show_ = true;
}

void SmPLGrid::update_fund_profit_loss()
{
	if (!fund_) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = fund_->GetAccountVector();

	double open_profit_loss = 0.0, trade_profit_loss = 0.0, fee = 0.0, pure_profit = 0.0;
	for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
		open_profit_loss += (*it)->Asset.OpenProfitLoss;
		trade_profit_loss += (*it)->Asset.TradeProfitLoss;
		fee += (*it)->Asset.Fee;
		pure_profit = open_profit_loss + trade_profit_loss - abs(fee);
	}

	std::string value;
	value = std::format("{0:.2f}", open_profit_loss);
	if (open_profit_loss > 0) {
		QuickSetTextColor(0, 1, RGB(255, 0, 0));
		QuickSetText(0, 1, value.c_str());
	}
	else if (open_profit_loss < 0) {
		QuickSetTextColor(0, 1, RGB(0, 0, 255));
		QuickSetText(0, 1, value.c_str());
	}
	else {
		QuickSetTextColor(0, 1, RGB(0, 0, 0));
		QuickSetNumber(0, 1, 0);
	}

	value = std::format("{0:.2f}", trade_profit_loss);
	if (trade_profit_loss > 0) {
		QuickSetTextColor(1, 1, RGB(255, 0, 0));
		QuickSetText(1, 1, value.c_str());
	}
	else if (trade_profit_loss < 0) {
		QuickSetTextColor(1, 1, RGB(0, 0, 255));
		QuickSetText(1, 1, value.c_str());
	}
	else {
		QuickSetTextColor(1, 1, RGB(0, 0, 0));
		QuickSetNumber(1, 1, 0);
	}
	//value = std::format("{0:.2f}", fee);
	value = std::format("{0:.2f}", pure_profit);
	if (pure_profit > 0) {
		QuickSetTextColor(2, 1, RGB(255, 0, 0));
		QuickSetText(2, 1, value.c_str());
	}
	else if (pure_profit < 0) {
		QuickSetTextColor(2, 1, RGB(0, 0, 255));
		QuickSetText(2, 1, value.c_str());
	}
	else {
		QuickSetTextColor(2, 1, RGB(0, 0, 0));
		QuickSetNumber(2, 1, 0);
	}
	InvalidateCellRect(0, 1);
	InvalidateCellRect(1, 1);
	InvalidateCellRect(2, 1);
	enable_account_profit_loss_show_ = true;
}

std::string SmPLGrid::get_format_price(const double& value)
{
	std::string value_string;
	CString format_price;
	if (account_->Type() == "1")
		format_price.Format("%.2f", value);
	else
		format_price.Format("%.0f", value);
	return std::string(CT2CA(format_price));
}

void SmPLGrid::InitGrid()
{
	if (!_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0)
		ShowAccountProfitLoss();
	else
		ShowFundProfitLoss();
}

void SmPLGrid::OnOutstanding()
{
	InitGrid();
}

void SmPLGrid::OnReceiveQuote(symbol_p sym)
{
	if (!sym)
		return;

	InitGrid();
}

void SmPLGrid::OnReceiveAccountInfo()
{
	InitGrid();
}

LRESULT SmPLGrid::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	InitGrid();

	return 1;
}

void SmPLGrid::ShowAccountProfitLoss()
{
	account_p acnt = _OrderConfigMgr->Account();

	if (!acnt)
		return;

	Account(acnt);
	update_account_profit_loss();
}

void SmPLGrid::ShowFundProfitLoss()
{
	fund_p fund = _OrderConfigMgr->Fund();
	if (!fund)
		return;
	Fund(fund);
	update_fund_profit_loss();
}
