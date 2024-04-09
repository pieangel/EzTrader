#include "stdafx.h"
#include "VtAccountAssetGrid.h"
#include "VtAssetPage.h"
#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../SmGrid/SmCell.h"
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
#include <numeric>
#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

VtAccountAssetGrid::VtAccountAssetGrid()
{
	_AssetPage = nullptr;

	account_profit_loss_control_ = std::make_shared<DarkHorse::AccountProfitLossControl>();
	account_profit_loss_control_->set_event_handler(std::bind(&VtAccountAssetGrid::on_update_account_profit_loss, this));

	asset_control_ = std::make_shared<DarkHorse::AccountAssetControl>();
	asset_control_->set_event_handler(std::bind(&VtAccountAssetGrid::on_update_account_profit_loss, this));
}


VtAccountAssetGrid::~VtAccountAssetGrid()
{
}

void VtAccountAssetGrid::OnSetup()
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

	SetSH_Width(0);
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

	InitGrid();

	CRect rcWnd;
	GetWindowRect(rcWnd);
	SetWindowPos(nullptr, 0, 0, std::accumulate(_ColWidVec.begin(), _ColWidVec.end(), 0), rcWnd.Height(), SWP_NOMOVE);
}

void VtAccountAssetGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void VtAccountAssetGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void VtAccountAssetGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	CUGCell cell;
	LPCTSTR titleLeft[5] = { "예탁총액", "평가손익", "실현손익", "위탁증거금", "유지증거금" };
	LPCTSTR titleRight[5] = { "평가예탁총액", "수수료", "순손익", "주문가능금액", "추가증거금" };
	//SetColWidth(-1, 60);
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	_ColWidVec.push_back(74);
	_ColWidVec.push_back(80);
	_ColWidVec.push_back(74);
	_ColWidVec.push_back(80);
	SetColWidth(0, _ColWidVec[0]);
	SetColWidth(1, _ColWidVec[1]);
	SetColWidth(2, _ColWidVec[2]);
	SetColWidth(3, _ColWidVec[3]);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetText(0, i, titleLeft[i]);
		GetCell(0, i, &cell);
		cell.SetBackColor(RGB(70, 130, 180));
		cell.SetTextColor(RGB(255, 255, 255));
		SetCell(0, i, &cell);
		QuickSetAlignment(0, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetFont(0, i, &_titleFont);
		RedrawCell(0, i);

		QuickSetText(2, i, titleRight[i]);
		GetCell(2, i, &cell);
		cell.SetBackColor(RGB(70, 130, 180));
		cell.SetTextColor(RGB(255, 255, 255));
		SetCell(2, i, &cell);
		QuickSetAlignment(2, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetFont(2, i, &_titleFont);
		RedrawCell(2, i);
	}
}

void VtAccountAssetGrid::QuickRedrawCell(int col, long row)
{
}

void VtAccountAssetGrid::InitGrid()
{
	enable_account_profit_loss_show_ = true;
}


void VtAccountAssetGrid::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	fund_ = val;

	if (!account_profit_loss_control_) return;
	account_profit_loss_control_->set_fund(fund_);
	asset_control_->set_fund(fund_);
	//update_account_profit_loss();
	enable_account_profit_loss_show_ = true;
}

void VtAccountAssetGrid::refresh()
{
	if (enable_account_profit_loss_show_) {
		update_account_profit_loss();
		Invalidate(FALSE);
		enable_account_profit_loss_show_ = false;
	}
	enable_account_profit_loss_show_ = true;
}

void VtAccountAssetGrid::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	account_ = val;

	if (!account_profit_loss_control_) return;
	account_profit_loss_control_->set_account(account_);
	asset_control_->set_account(account_);
	//update_account_profit_loss();
	enable_account_profit_loss_show_ = true;
}

void VtAccountAssetGrid::UpdateSymbolInfo()
{
	if (!_Symbol) return;
	/*
	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	if (cell) cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));
	*/
	Invalidate();
}

void VtAccountAssetGrid::SetAssetInfo()
{
	_Mode == 0 ? SetAccountAssetInfo() : SetFundAssetInfo();
}

void VtAccountAssetGrid::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void VtAccountAssetGrid::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void VtAccountAssetGrid::on_update_account_profit_loss()
{

	enable_account_profit_loss_show_ = true;
}

void VtAccountAssetGrid::update_account_profit_loss()
{
	if (!asset_control_ || !account_profit_loss_control_ || !account_) return;
	if (updating_) return;
	updating_ = true;
	const VmAsset& asset = asset_control_->get_asset();
	const VmAccountProfitLoss& account_profit_loss = account_profit_loss_control_->get_account_profit_loss();
	const int decimal = account_->Type() == "1" ? 2 : 0;
	std::string value;
	value = VtStringUtil::get_format_value(asset.entrust_total, decimal, true);
	QuickSetTextColor(1, 0, RGB(0, 0, 0));
	QuickSetText(1, 0, value.c_str());
	value = VtStringUtil::get_format_value(account_profit_loss.open_profit_loss, decimal, true);
	QuickSetTextColor(3, 0, RGB(0, 0, 0));
	QuickSetText(3, 0, value.c_str());
	if (account_profit_loss.open_profit_loss > 0) {
		QuickSetTextColor(1, 1, RGB(255, 0, 0));
		QuickSetText(1, 1, value.c_str());
	}
	else if (account_profit_loss.open_profit_loss < 0) {
		QuickSetTextColor(1, 1, RGB(0, 0, 255));
		QuickSetText(1, 1, value.c_str());
	}
	else {
		QuickSetTextColor(1, 1, RGB(0, 0, 0));
		QuickSetNumber(1, 1, 0);
	}
	value = VtStringUtil::get_format_value(account_profit_loss.trade_profit_loss, decimal, true);

	if (account_profit_loss.trade_profit_loss > 0) {
		QuickSetTextColor(1, 2, RGB(255, 0, 0));
		QuickSetText(1, 2, value.c_str());
	}
	else if (account_profit_loss.trade_profit_loss < 0) {
		QuickSetTextColor(1, 2, RGB(0, 0, 255));
		QuickSetText(1, 2, value.c_str());
	}
	else {
		QuickSetTextColor(1, 2, RGB(0, 0, 0));
		QuickSetNumber(1, 2, 0);
	}
	
	value = VtStringUtil::get_format_value(asset.entrust_deposit, decimal, true);
	QuickSetTextColor(3, 0, RGB(0, 0, 0));
	QuickSetText(3, 0, value.c_str());
	value = VtStringUtil::get_format_value(asset.maintenance_margin, decimal, true);
	QuickSetText(1, 4, value.c_str());


	value = VtStringUtil::get_format_value(asset.open_trust_toal, decimal, true);
	QuickSetText(1, 3, value.c_str());

	value = VtStringUtil::get_format_value(account_profit_loss.trade_fee, decimal, true);
	QuickSetText(3, 1, value.c_str());

	double pure_profit = account_profit_loss.open_profit_loss + account_profit_loss.trade_profit_loss - abs(account_profit_loss.trade_fee);
	value = VtStringUtil::get_format_value(pure_profit, decimal, true);
	if (pure_profit > 0) {
		QuickSetTextColor(3, 2, RGB(255, 0, 0));
		QuickSetText(3, 2, value.c_str());
	}
	else if (pure_profit < 0) {
		QuickSetTextColor(3, 2, RGB(0, 0, 255));
		QuickSetText(3, 2, value.c_str());
	}
	else {
		QuickSetTextColor(3, 2, RGB(0, 0, 0));
		QuickSetNumber(3, 2, 0);
	}
	value = VtStringUtil::get_format_value(asset.order_margin, decimal, true);
	QuickSetText(3, 3, value.c_str());
	value = VtStringUtil::get_format_value(asset.additional_margin, decimal, true);
	QuickSetText(3, 4, value.c_str());

	for (int i = 0; i < _ColCount; ++i) {
		for (int j = 0; j < _RowCount; ++j) {
			if (i == 1 || i == 3) {
				QuickSetAlignment(i, j, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
			}
			QuickRedrawCell(i, j);
		}
	}

	updating_ = false;
}

void VtAccountAssetGrid::SetAccountAssetInfo()
{
	if (!account_) return;
	/*
	auto cell = _Grid->FindCell(0, 1);
	std::string value;
	value = std::format("{0:.2f}", account_->Asset.Balance);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(1, 1);
	value = std::format("{0:.2f}", account_->Asset.OpenProfitLoss);
	if (account_->Asset.OpenProfitLoss > 0) {
		cell->CellType(SmCellType::CT_SP_PROFIT);
	}
	else if (account_->Asset.OpenProfitLoss < 0) {
		cell->CellType(SmCellType::CT_SP_LOSS);
	}
	else {
		cell->Text("0");
		cell->CellType(SmCellType::CT_DEFAULT);
	}

	if (cell) cell->Text(value);
	cell = _Grid->FindCell(2, 1);
	value = std::format("{0:.2f}", account_->Asset.TradeProfitLoss);
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(3, 1);
	value = std::format("{0:.2f}", 0.0);
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(4, 1);
	value = std::format("{0:.2f}", 0.0);
	if (cell) cell->Text(value);


	cell = _Grid->FindCell(0, 3);
	value = std::format("{0:.2f}", account_->Asset.TradeProfitLoss);
	if (cell) cell->Text(value);


	cell = _Grid->FindCell(1, 3);
	value = std::format("{0:.2f}", account_->Asset.Fee);
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(2, 3);
	double pure_profit = account_->Asset.OpenProfitLoss + account_->Asset.TradeProfitLoss - abs(account_->Asset.Fee);
	value = std::format("{0:.2f}", pure_profit);

	if (pure_profit > 0) {
		cell->CellType(SmCellType::CT_SP_PROFIT);
	}
	else if (pure_profit < 0) {
		cell->CellType(SmCellType::CT_SP_LOSS);
	}
	else {
		cell->Text("0");
		cell->CellType(SmCellType::CT_DEFAULT);
	}

	if (cell) cell->Text(value);

	cell = _Grid->FindCell(3, 3);
	value = std::format("{0:.2f}", account_->Asset.OrderMargin);
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(4, 3);
	value = std::format("{0:.2f}", account_->Asset.AdditionalMargin);
	if (cell) cell->Text(value);
	*/
}

void VtAccountAssetGrid::SetFundAssetInfo()
{
	if (!fund_) return;
	/*
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

	auto cell = _Grid->FindCell(0, 1);
	std::string value;
	value = std::format("{0:.2f}", balance);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(1, 1);
	value = std::format("{0:.2f}", open_pl);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(2, 1);
	value = std::format("{0:.2f}", settled_pl);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(3, 1);
	value = std::format("{0:.2f}", fee);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(4, 1);
	value = std::format("{0:.2f}", order_margin);
	if (cell) cell->Text(value);
	*/
}

void VtAccountAssetGrid::OnTimer(UINT_PTR nIDEvent)
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

