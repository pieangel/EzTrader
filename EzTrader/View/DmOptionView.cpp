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
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Util/SmUtil.h"
#include "../Global/SmTotalManager.h"
#include "../Task/SmTaskArg.h"
#include "../Task/SmTaskRequestManager.h"
#include "../Util/IdGenerator.h"

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
	: id_(IdGenerator::get_id())
{
	quote_control_ = std::make_shared<DarkHorse::QuoteControl>();
	quote_control_->set_event_handler(std::bind(&DmOptionView::on_update_quote, this));

	mainApp.event_hub()->subscribe_expected_event_handler
	(
		id_,
		std::bind(&DmOptionView::update_expected, this, std::placeholders::_1)
	);
}

DmOptionView::~DmOptionView()
{
	//KillTimer(1);
	mainApp.event_hub()->unsubscribe_expected_event_handler
	(
		id_
	);
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}
void DmOptionView::on_update_quote()
{
	_EnableQuoteShow = true;
}
void DmOptionView::update_quote()
{
	const VmQuote& quote = quote_control_->get_quote();
	CString msg;
	msg.Format("DmOptionView::update_quote :: %s ::  close : %d\n", quote.symbol_code.c_str(), quote.close);
	TRACE(msg);
	update_close(quote);
}

void DmOptionView::update_expected(std::shared_ptr<SmQuote> quote)
{
	const std::string option_code = quote->symbol_code.substr(1, quote->symbol_code.length() - 1);
	auto found = symbol_vector_index_map_.find(option_code);
	if (found == symbol_vector_index_map_.end()) return;
	if (quote->symbol_code.at(0) == '2') {
		DarkHorse::VmOption& option_info = call_symbol_vector_[found->second];
		option_info.expected = quote->expected;
		update_expected_cell(quote->symbol_id, option_info);
	}
	else {
		DarkHorse::VmOption& option_info = put_symbol_vector_[found->second];
		option_info.expected = quote->expected;
		update_expected_cell(quote->symbol_id, option_info);
	}
}

void DmOptionView::update_close(const DarkHorse::VmQuote& quote)
{
	const std::string option_code = quote.symbol_code.substr(1, quote.symbol_code.length() - 1);
	auto found = symbol_vector_index_map_.find(option_code);
	if (found == symbol_vector_index_map_.end()) return;
	if (quote.symbol_code.at(0) == '2') {
		DarkHorse::VmOption& option_info = call_symbol_vector_[found->second];
		option_info.close = quote.close;
		update_close_cell(quote.symbol_id, option_info);
	}
	else {
		DarkHorse::VmOption& option_info = put_symbol_vector_[found->second];
		option_info.close = quote.close;
		update_close_cell(quote.symbol_id, option_info);
	}
}

void DmOptionView::update_close_cell(const int symbol_id, const DarkHorse::VmOption& option_info)
{
	if (view_mode_ != ViewMode::VM_Close) return;
	auto found = row_col_map_.find(symbol_id);
	if (found == row_col_map_.end()) return;
	show_value(found->second.first, found->second.second, option_info);
}

void DmOptionView::update_expected_cell(const int symbol_id, const DarkHorse::VmOption& option_info)
{
	if (view_mode_ != ViewMode::VM_Expected) return;
	auto found = row_col_map_.find(symbol_id);
	if (found == row_col_map_.end()) return;
	show_value(found->second.first, found->second.second, option_info);
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
		return;

	m_pGM->BindDC(pDC, rect);

	if (!m_pGM->BeginDraw())
		return;

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
	set_option_info(option_market_index, year_month_name);
	make_symbol_vec(true);
	make_symbol_vec(false);
	register_symbols(option_market_index);
	init_strike_index();
	set_option_view();
}

void DmOptionView::set_option_view()
{
	set_strike();
	show_values();
	//register_symbols_to_server();
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

void DmOptionView::set_view_mode(ViewMode view_mode)
{
	view_mode_ = view_mode;
	show_values();
	Invalidate();
}

void DmOptionView::set_strike_start_index(const int distance)
{
	size_t max_symbol_count = call_symbol_vector_.size();
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
	auto found = symbol_map_.find(cell_pos);
	if (found == symbol_map_.end()) return;
	mainApp.event_hub()->process_symbol_event(found->second);

	CBCGPStatic::OnLButtonDown(nFlags, point);
}

void DmOptionView::register_symbols(const int option_market_index)
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

void DmOptionView::register_symbol(const std::string symbol_code)
{
	DmRegisterReq req;
	req.symbol_code = symbol_code;
	SmTaskArg arg;
	arg.TaskType = SmTaskType::RegisterSymbol;
	arg.Param = req;
	mainApp.TaskReqMgr()->AddTask(std::move(arg));
}

void DmOptionView::show_value(const int row, const int col, const DarkHorse::VmOption& option_info)
{
	auto cell = _Grid->FindCell(row, col);
	if (!cell) return;

	std::string value;
	if (view_mode_ == ViewMode::VM_Close) {
		value = std::to_string(option_info.close);
	}
	else if (view_mode_ == ViewMode::VM_Expected) {
		value = std::to_string(option_info.expected);
	}
	else {
		value = std::to_string(option_info.position);
	}
	SmUtil::insert_decimal(value, option_info.decimal);
	cell->Text(value);
	Invalidate();
}

void DmOptionView::show_strike(const int row, const int col, const DarkHorse::VmOption& option_info)
{
	auto cell = _Grid->FindCell(row, col);
	if (!cell) return;

	if (option_info.atm == 1)
		cell->CellType(CT_BUTTON_BUY);
	else
		cell->CellType(CT_NORMAL);
	cell->Text(option_info.strike.c_str());
}

void DmOptionView::show_values()
{
	if (call_symbol_vector_.empty() || put_symbol_vector_.empty()) return;

	for (int i = 1; i < _Grid->RowCount(); i++) {
		int new_strike_index = strike_start_index_ + i - 1;
		const int vec_size = static_cast<int>(call_symbol_vector_.size());
		if (new_strike_index >= vec_size)
			new_strike_index = vec_size - 1;
		if (new_strike_index < 0)
			new_strike_index = 0;
		const VmOption& call_info = call_symbol_vector_[new_strike_index];
		const VmOption& put_info = put_symbol_vector_[new_strike_index];
		show_value(i, 0, call_info);
		show_value(i, 2, put_info);
	}
}

void DmOptionView::set_option_info(const int option_market_index, const std::string& year_month_name)
{
	option_market_index_ = option_market_index;
	year_month_name_ = year_month_name;
}

void DmOptionView::make_symbol_vec(bool call_side)
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
		VmOption option_info;
		option_info.strike = symbol->Strike();
		option_info.atm = symbol->AtmType();
		option_info.decimal = symbol->decimal();
		if (option_info.atm == 1) atm_index_ = i;
		option_info.close = quote->close;
		option_info.expected = quote->expected;
		option_info.ordered = false;
		option_info.position = 0;
		option_info.symbol_id = symbol->Id();
		option_info.symbol_p = symbol;
		const std::string& symbol_code = symbol->SymbolCode();
		const std::string option_code = symbol_code.substr(1, symbol_code.length() - 1);
		symbol_vector_index_map_[option_code] = i;
		if (call_side) call_symbol_vector_.push_back(option_info);
		else put_symbol_vector_.push_back(option_info);
	}
}

void DmOptionView::init_strike_index()
{
	strike_start_index_ = atm_index_ - static_cast<int>(_Grid->RowCount() / 2);
	if (strike_start_index_ < 1) strike_start_index_ = 1;
}

void DmOptionView::register_symbols_to_server()
{
// 	for(auto& symbol : call_symbol_vector_)
// 		mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
// 	for (auto& symbol : put_symbol_vector_)
// 		mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
}

void DmOptionView::set_strike()
{
	CString msg;
	msg.Format("set_strike begin\n");
	TRACE(msg);
	if (call_symbol_vector_.empty() || put_symbol_vector_.empty()) return;

	symbol_map_.clear();
	row_col_map_.clear();
	for (int i = 1; i < _Grid->RowCount(); i++) {
		int new_strike_index = strike_start_index_ + i - 1;
		const int vec_size = static_cast<int>(call_symbol_vector_.size());
		if (new_strike_index >= vec_size)
			new_strike_index = vec_size - 1;
		if (new_strike_index < 0)
			new_strike_index = 0;
		
		show_strike(i, 1, call_symbol_vector_[new_strike_index]);

		auto call_symbol = call_symbol_vector_[new_strike_index].symbol_p;
		auto put_symbol = put_symbol_vector_[new_strike_index].symbol_p;
		symbol_map_[std::make_pair(i, 0)] = call_symbol;
		symbol_map_[std::make_pair(i, 2)] = put_symbol;
		row_col_map_[call_symbol->Id()] = std::make_pair(i, 0);
		row_col_map_[put_symbol->Id()] = std::make_pair(i, 2);
	}
	msg.Format("set_strike end\n");
	TRACE(msg);
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
		update_quote();
		_EnableQuoteShow = false;
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
	Invalidate();
	return TRUE;
}
