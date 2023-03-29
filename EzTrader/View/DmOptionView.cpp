//#include "stdafx.h"
//#include "OptionView.h"

#include "stdafx.h"
#include "DmOptionView.h"

#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../SmGrid/SmCell.h"
#include "../Account/SmAccount.h"

#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Global/SmTotalManager.h"
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

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(DmOptionView, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

DmOptionView::DmOptionView()
{
	quote_control_ = std::make_shared<DarkHorse::QuoteControl>();
	quote_control_->dm_option_view(this);
}

DmOptionView::~DmOptionView()
{
	KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);

	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void DmOptionView::update_quote()
{
	const VmQuote quote = quote_control_->get_quote();
	CString msg;
	msg.Format("DmOptionView::update_quote ::  close : %d\n", quote.close);
	TRACE(msg);
}

void DmOptionView::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 90, 3);
	int colWidth[3] = { 73, 50, 73 };
	int width_sum = 0;
	for (int i = 0; i < 3; i++) {
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
		_HeaderTitles.push_back("CALL");
		_HeaderTitles.push_back("행사가");
		_HeaderTitles.push_back("PUT");
		_Grid->SetColHeaderTitles(_HeaderTitles);
	}

	mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&DmOptionView::OnQuoteEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&DmOptionView::OnOrderEvent, this, _1, _2));


	SetTimer(1, 40, NULL);
}

void DmOptionView::OnPaint()
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

void DmOptionView::set_option_view(
	const int option_market_index, 
	const std::string& year_month_name)
{
	option_market_index_ = option_market_index;
	year_month_name_ = year_month_name;
	strike_start_index_ = 0;
	
	std::vector<DarkHorse::DmOption>& option_vec = mainApp.SymMgr()->get_dm_option_vec();
	if (option_market_index_ < 0 || option_market_index_ >= static_cast<const int>(option_vec.size())) return;
	const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& call_year_month_map = option_vec[option_market_index_].call_product->get_yearmonth_map();
	auto it = call_year_month_map.find(year_month_name_);
	if (it == call_year_month_map.end()) return;

	const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& call_symbol_vec = it->second->get_symbol_vector();
	call_symbol_vector_.clear();
	std::copy(call_symbol_vec.begin(), call_symbol_vec.end(), std::back_inserter(call_symbol_vector_));

	//std::reverse_copy(call_symbol_vec.begin(), call_symbol_vec.end(), std::back_inserter(call_symbol_vector_));


	const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& put_year_month_map = option_vec[option_market_index_].put_product->get_yearmonth_map();
	it = put_year_month_map.find(year_month_name_);
	if (it == put_year_month_map.end()) return;

	const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& put_symbol_vec = it->second->get_symbol_vector();
	put_symbol_vector_.clear();
	std::copy(put_symbol_vec.begin(), put_symbol_vec.end(), std::back_inserter(put_symbol_vector_));
	//std::reverse_copy(put_symbol_vec.begin(), put_symbol_vec.end(), std::back_inserter(put_symbol_vector_));

	const int atm_index = get_atm_index(call_symbol_vector_);
	strike_start_index_ = get_atm_index(call_symbol_vector_) - static_cast<int>(_Grid->RowCount() / 2);
	strike_start_index_--;
	if (strike_start_index_ < 1) strike_start_index_ = 1;

	set_option_view();
}

void DmOptionView::set_option_view()
{
	set_strike(call_symbol_vector_, put_symbol_vector_);
	register_symbols_to_server();
	Invalidate();
}

void DmOptionView::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	if (cell) cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}

void DmOptionView::UpdateAssetInfo()
{
	_Mode == 0 ? UpdateAccountAssetInfo() : UpdateFundAssetInfo();
}

void DmOptionView::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void DmOptionView::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

int DmOptionView::get_atm_index(const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec)
{
	if (symbol_vec.size() == 0) return -1;
	for (size_t i = 0; i < symbol_vec.size(); i++) {
		if (symbol_vec[i]->AtmType() == 1) return i;
	}
	return -1;
}

void DmOptionView::set_strike_start_index(const int distance)
{
	strike_start_index_ += distance;
	if (strike_start_index_ < 1)
		strike_start_index_ = 1;
	if (max_symbol_count <= static_cast<size_t>(_Grid->RowCount()))
		strike_start_index_ = 1;
	const size_t diff = max_symbol_count - _Grid->RowCount();
	if (strike_start_index_ >= static_cast<int>(diff))
		strike_start_index_ = diff - 2;
}

void DmOptionView::OnLButtonDown(UINT nFlags, CPoint point)
{
	auto cell_pos = _Grid->FindRowCol(point.x, point.y);
	if (cell_pos.second == 0) {
		auto it = call_symbol_map_.find(cell_pos);
		if (it == call_symbol_map_.end()) return;

		mainApp.event_hub()->process_symbol_event(it->second);
	}
	else if (cell_pos.second == 2) {
		auto it = put_symbol_map_.find(cell_pos);
		if (it == put_symbol_map_.end()) return;

		mainApp.event_hub()->process_symbol_event(it->second);
	}
	CBCGPStatic::OnLButtonDown(nFlags, point);
}

void DmOptionView::register_symbols_to_server()
{
	for(auto& symbol : call_symbol_vector_)
		mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
	for (auto& symbol : put_symbol_vector_)
		mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
}

void DmOptionView::set_strike(
	const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& call_symbol_vec,
	const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& put_symbol_vec
)
{
	if (call_symbol_vec.empty() || put_symbol_vector_.empty()) return;
	call_symbol_map_.clear();
	put_symbol_map_.clear();
	for (int i = 1; i < _Grid->RowCount(); i++) {
		auto cell = _Grid->FindCell(i, 1);
		int new_strike_index = strike_start_index_ + i - 1;
		if (new_strike_index >= static_cast<int>(call_symbol_vec.size()))
			new_strike_index = call_symbol_vec.size() - 1;
		if (new_strike_index < 0)
			new_strike_index = 0;
		if (cell) {
			if (call_symbol_vec[new_strike_index]->AtmType() == 1)
				cell->CellType(CT_BUTTON_BUY);
			else 
				cell->CellType(CT_NORMAL);
			cell->Text(call_symbol_vec[new_strike_index]->Strike().c_str());

			call_symbol_map_[std::make_pair(i, 0)] = call_symbol_vec[new_strike_index];
			put_symbol_map_[std::make_pair(i, 2)] = put_symbol_vec[new_strike_index];
		}
	}

	max_symbol_count = call_symbol_vec.size();
}

void DmOptionView::UpdateAccountAssetInfo()
{
	if (!_Account) return;

	auto cell = _Grid->FindCell(0, 1);
	std::string value;
	value = std::format("{0:.2f}", _Account->Asset.OpenProfitLoss);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(1, 1);
	value = std::format("{0:.2f}", _Account->Asset.SettledProfitLose);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(2, 1);
	value = std::format("{0:.2f}", _Account->Asset.Fee);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(3, 1);
	const double pure_profit = _Account->Asset.OpenProfitLoss + _Account->Asset.SettledProfitLose - abs(_Account->Asset.Fee);
	value = std::format("{0:.2f}", pure_profit);
	if (cell) cell->Text(value);
}

void DmOptionView::UpdateFundAssetInfo()
{
	if (!_Fund) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();

	double open_pl = 0.0, settled_pl = 0.0, fee = 0.0, pure_pl = 0.0;
	for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
		open_pl += (*it)->Asset.OpenProfitLoss;
		settled_pl += (*it)->Asset.SettledProfitLose;
		fee += (*it)->Asset.Fee;
		pure_pl = open_pl + settled_pl - abs(fee);
	}

	auto cell = _Grid->FindCell(0, 1);
	std::string value;
	value = std::format("{0:.2f}", open_pl);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(1, 1);
	value = std::format("{0:.2f}", settled_pl);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(2, 1);
	value = std::format("{0:.2f}", fee);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(3, 1);
	value = std::format("{0:.2f}", pure_pl);
	if (cell) cell->Text(value);
}

void DmOptionView::CreateResource()
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

void DmOptionView::InitHeader()
{
	_HeaderTitles.push_back("평가손익");
	_HeaderTitles.push_back("실현손익");
	_HeaderTitles.push_back("수수료");
	_HeaderTitles.push_back("총손익");
}



void DmOptionView::OnTimer(UINT_PTR nIDEvent)
{
	bool needDraw = false;
	if (_EnableQuoteShow) {
		UpdateAssetInfo();
		_EnableQuoteShow = false;
		needDraw = true;
	}

	if (_EnableOrderShow) {
		UpdateAssetInfo();
		_EnableOrderShow = false;
		needDraw = true;
	}


	if (needDraw) Invalidate();

	CBCGPStatic::OnTimer(nIDEvent);
}

BOOL DmOptionView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int distance = zDelta / 60;
	if (abs(zDelta) > 120)
		distance = zDelta / 120;
	else
		distance = zDelta / 40;
	set_strike_start_index(distance);
	set_option_view();
	return TRUE;
}
