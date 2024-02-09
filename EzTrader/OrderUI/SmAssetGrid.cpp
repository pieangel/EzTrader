#include "stdafx.h"
#include "SmAssetGrid.h"
#include "VtOrderConfigManager.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccountManager.h"

#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Fund/SmFund.h"
#include "../Controller/AccountAssetControl.h"
#include "../Controller/AccountProfitLossControl.h"
#include "../Util/VtStringUtil.h"
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SmAssetGrid::SmAssetGrid()
{
	_OrderConfigMgr = nullptr;
	account_profit_loss_control_ = std::make_shared<DarkHorse::AccountProfitLossControl>();
	account_profit_loss_control_->set_event_handler(std::bind(&SmAssetGrid::on_update_account_profit_loss, this));

	asset_control_ = std::make_shared<DarkHorse::AccountAssetControl>();
	asset_control_->set_event_handler(std::bind(&SmAssetGrid::on_update_account_profit_loss, this));
}


SmAssetGrid::~SmAssetGrid()
{
}

void SmAssetGrid::Init()
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
}

void SmAssetGrid::SetRowTitle()
{
	LPCTSTR title[2] = { "평가예탁", "주문가능" };
	SetColumnWidth(0, 60);
	SetColumnWidth(1, 110);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetText(i, 0, title[i]);
		QuickSetBackColor(i, 0, DarkHorse::SmTotalManager::GridTitleBackColor);
		QuickSetTextColor(i, 0, DarkHorse::SmTotalManager::GridTitleTextColor);
		InvalidateCellRect(i, 0);
	}
}

void SmAssetGrid::InitGrid()
{
	if (_OrderConfigMgr->Type() == 0) {
		account_p acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;
		Account(acnt);
		SetAccountAssetInfo();
	}
	else {
		fund_p fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		Fund(fund);
		SetFundAssetInfo();
	}
}

void SmAssetGrid::OnReceiveAccountInfo()
{
	InitGrid();
}


void SmAssetGrid::QuickSetBackColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetBackClr(color);
	}
}

void SmAssetGrid::QuickSetText(int row, int col, LPCTSTR text)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetText(text);
	}
}

void SmAssetGrid::QuickSetNumber(int row, int col, int number)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		CString text;
		text.Format("%d", number);
		pCell->SetText(text);
	}
}

void SmAssetGrid::QuickSetTextColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetTextClr(color);
	}
}

void SmAssetGrid::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	fund_ = val;
	_Mode = 1;

	if (!account_profit_loss_control_) return;

	account_profit_loss_control_->set_fund(fund_);
	asset_control_->set_fund(fund_);
	enable_account_profit_loss_show_ = true;
	SetFundAssetInfo();
}

void SmAssetGrid::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	account_ = val;
	_Mode = 0;

	if (!account_profit_loss_control_) return;

	account_profit_loss_control_->set_account(account_);
	asset_control_->set_account(account_);
	enable_account_profit_loss_show_ = true;
	SetAccountAssetInfo();
}

void SmAssetGrid::SetAssetInfo()
{
	_Mode == 0 ? SetAccountAssetInfo() : SetFundAssetInfo();
}

void SmAssetGrid::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SmAssetGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	enable_account_profit_loss_show_ = true;
}

void SmAssetGrid::on_update_account_profit_loss()
{
	enable_account_profit_loss_show_ = true;
}

void SmAssetGrid::update_account_profit_loss()
{

}

void SmAssetGrid::SetAccountAssetInfo()
{
	if (!account_) return;

	std::string format_type("0");
	if (account_) format_type = account_->Type();
	const int decimal = format_type == "1" ? 2 : 0;
	std::string value;
	value = std::format("{0:.2f}", account_->Asset.Balance);
	value = VtStringUtil::get_format_value(account_->Asset.Balance, decimal, true);
	QuickSetText(0, 1, value.c_str());
	value = std::format("{0:.2f}", account_->Asset.OpenProfitLoss);
	value = std::format("{0:.2f}", account_->Asset.TradeProfitLoss);
	value = std::format("{0:.2f}", account_->Asset.Fee);
	value = std::format("{0:.2f}", account_->Asset.OrderMargin);
	value = VtStringUtil::get_format_value(account_->Asset.OrderMargin, decimal, true);
	QuickSetText(1, 1, value.c_str());

	InvalidateCellRect(0, 1);
	InvalidateCellRect(1, 1);
}

void SmAssetGrid::SetFundAssetInfo()
{
	if (!fund_) return;
	std::string format_type("0");
	if (fund_) format_type = fund_->fund_type();
	const int decimal = format_type == "1" ? 2 : 0;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = fund_->GetAccountVector();

	double balance = 0.0, order_margin = 0.0, open_pl = 0.0, settled_pl = 0.0, fee = 0.0, pure_pl = 0.0;
	for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
		open_pl += (*it)->Asset.OpenProfitLoss;
		settled_pl += (*it)->Asset.TradeProfitLoss;
		fee += (*it)->Asset.Fee;
		pure_pl = open_pl + settled_pl - abs(fee);
		balance += (*it)->Asset.Balance;
		order_margin += (*it)->Asset.OrderMargin;
	}

	std::string value;
	value = std::format("{0:.2f}", balance);
	value = VtStringUtil::get_format_value(balance, decimal, true);
	QuickSetText(0, 1, value.c_str());
	value = std::format("{0:.2f}", open_pl);
	value = std::format("{0:.2f}", settled_pl);
	value = std::format("{0:.2f}", fee);
	value = std::format("{0:.2f}", order_margin);
	value = VtStringUtil::get_format_value(order_margin, decimal, true);
	QuickSetText(1, 1, value.c_str());
	InvalidateCellRect(0, 1);
	InvalidateCellRect(1, 1);
}
