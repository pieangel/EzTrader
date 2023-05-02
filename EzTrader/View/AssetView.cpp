//#include "stdafx.h"
//#include "AssetView.h"

#include "stdafx.h"
#include "AssetView.h"

#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../SmGrid/SmCell.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccountManager.h"

#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Fund/SmFund.h"
#include "../Controller/AssetControl.h"
#include "../Controller/AccountProfitLossControl.h"
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(AssetView, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

AssetView::AssetView()
{
	account_profit_loss_control_ = std::make_shared<DarkHorse::AccountProfitLossControl>();
	account_profit_loss_control_->set_event_handler(std::bind(&AssetView::on_update_account_profit_loss, this));

	asset_control_ = std::make_shared<DarkHorse::AssetControl>();
	asset_control_->set_event_handler(std::bind(&AssetView::on_update_account_profit_loss, this));
}

AssetView::~AssetView()
{
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void AssetView::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 6, 2);

	int colWidth[2] = { 80, 103 };

	int width_sum = 0;
	for (int i = 0; i < 2; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	width_sum -= colWidth[1];
	_Grid->SetColWidth(1, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("예탁금잔액");
		_HeaderTitles.push_back("평가손익");
		_HeaderTitles.push_back("청산손익");
		_HeaderTitles.push_back("수수료");
		_HeaderTitles.push_back("주문가능금액");
		_HeaderTitles.push_back("통화코드");

		_Grid->SetRowHeaderTitles(_HeaderTitles);
	}

	SetTimer(1, 40, NULL);
}

void AssetView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rect;
	GetClientRect(rect);

	if (m_pGM == NULL)
	{
		return;
	}

	m_pGM->BindDC(pDC, rect);

	if (!m_pGM->BeginDraw())
	{
		return;
	}

	m_pGM->FillRectangle(rect, _Resource.GridNormalBrush);
	rect.right -= 1;
	rect.bottom -= 1;

	_Grid->DrawGrid(m_pGM, rect);
	_Grid->DrawCells(m_pGM, rect, false, true);
	//_Grid->DrawVerticalHeader(m_pGM, _HeaderTitles, 0);
	_Grid->DrawBorder(m_pGM, rect);

	m_pGM->EndDraw();
}

void AssetView::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	account_ = val;

	if (!account_profit_loss_control_) return;

	account_profit_loss_control_->load_position_from_account(account_->No());
	account_profit_loss_control_->set_account_id(account_->id());
	asset_control_->load_position_from_account(account_->No());
	enable_account_profit_loss_show_ = true;
}

void AssetView::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	if (cell) cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}

void AssetView::SetAssetInfo()
{
	_Mode == 0 ? SetAccountAssetInfo() : SetFundAssetInfo();
}

void AssetView::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void AssetView::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	enable_account_profit_loss_show_ = true;
}

void AssetView::on_update_account_profit_loss()
{
	enable_account_profit_loss_show_ = true;
}

void AssetView::update_account_profit_loss()
{
	if (!asset_control_ || !account_profit_loss_control_) return;

	const VmAsset& asset = asset_control_->get_asset();
	const VmAccountProfitLoss account_profit_loss = account_profit_loss_control_->get_account_profit_loss();
	auto cell = _Grid->FindCell(0, 1);
	std::string value;
	value = std::format("{0:.2f}", asset.balance);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(1, 1);
	value = std::format("{0:.2f}", account_profit_loss.open_profit_loss);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(2, 1);
	value = std::format("{0:.2f}", account_profit_loss.trade_profit_loss);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(3, 1);
	value = std::format("{0:.2f}", account_profit_loss.trade_fee);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(4, 1);
	value = std::format("{0:.2f}", asset.order_margin);
	if (cell) cell->Text(value);
}

void AssetView::SetAccountAssetInfo()
{
	if (!account_) return;

	auto cell = _Grid->FindCell(0, 1);
	std::string value;
	value = std::format("{0:.2f}", account_->Asset.Balance);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(1, 1);
	value = std::format("{0:.2f}", account_->Asset.OpenProfitLoss);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(2, 1);
	value = std::format("{0:.2f}", account_->Asset.TradeProfitLoss);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(3, 1);
	value = std::format("{0:.2f}", account_->Asset.Fee);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(4, 1);
	value = std::format("{0:.2f}", account_->Asset.OrderMargin);
	if (cell) cell->Text(value);
}

void AssetView::SetFundAssetInfo()
{
	if (!_Fund) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();

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
}

void AssetView::CreateResource()
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
}

void AssetView::InitHeader()
{
	_HeaderTitles.push_back("예탁금잔액");
	_HeaderTitles.push_back("평가손익");
	_HeaderTitles.push_back("청산손익");
	_HeaderTitles.push_back("수수료");
	_HeaderTitles.push_back("주문가능금액");
	_HeaderTitles.push_back("통화코드");
}



void AssetView::OnTimer(UINT_PTR nIDEvent)
{
	bool needDraw = false;
	if (enable_account_profit_loss_show_) {
		update_account_profit_loss();
		enable_account_profit_loss_show_ = false;
		needDraw = true;
	}

	if (needDraw) Invalidate();

	CBCGPStatic::OnTimer(nIDEvent);
}
