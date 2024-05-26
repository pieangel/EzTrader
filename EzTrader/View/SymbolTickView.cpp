//#include "stdafx.h"
//#include "SymbolTickView.h"

#include "stdafx.h"
#include "SymbolTickView.h"
#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../SmGrid/SmCell.h"
#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Controller/SymbolTickControl.h"
#include "../Util/SmUtil.h"
#include "../Util/IdGenerator.h"
#include "../Event/EventHub.h"
#include "../Util/SimpleTree.h"
#include <format>


#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

const int fixed_tick_wnd_width = 173;


BEGIN_MESSAGE_MAP(SymbolTickView, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()

SymbolTickView::SymbolTickView()
	: id_(IdGenerator::get_id())
{
	tick_control_ = std::make_shared<DarkHorse::SymbolTickControl>();
	tick_control_->set_event_handler(std::bind(&SymbolTickView::on_update_tick, this));
}

SymbolTickView::~SymbolTickView()
{
	//KillTimer(1);
	if (m_pGM != NULL) delete m_pGM;
}

int SymbolTickView::GetGridWidth()
{
	int width_sum = 0;
	for (int i = 0; i < 3; i++) {
		width_sum += colWidth[i];
	}
	return width_sum;
}

void SymbolTickView::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void SymbolTickView::MaxRow(int val)
{
	_RowCount = val;
}

int SymbolTickView::MaxRow()
{
	return _RowCount;
}

void SymbolTickView::Symbol(std::shared_ptr<DarkHorse::SmSymbol> val)
{
	symbol_ = val;
	tick_control_->set_symbol_id(val->Id());
	tick_control_->set_symbol_decimal(val->decimal());
	on_update_tick();
	Invalidate();
}

void SymbolTickView::on_update_tick()
{
	_EnableQuoteShow = true;
}

void SymbolTickView::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, _RowCount, 3);
	//int colWidth[3] = { 60, 55, 58 };
	int width_sum = 0;
	for (int i = 0; i < 3; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	//width_sum -= colWidth[2];
	//_Grid->SetColWidth(2, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("시각");
		_HeaderTitles.push_back("체결가");
		_HeaderTitles.push_back("체결");
		_Grid->SetColHeaderTitles(_HeaderTitles);
	}

	SetTimer(1, 40, NULL);
}

void SymbolTickView::SetUp(std::shared_ptr<WinInfo> parent_win_info)
{
	win_info_ = std::make_shared<WinInfo>(parent_win_info, id_, 0, 0, 0, 0);
	win_info_->name_ = "symbol_tick_view";
	if (parent_win_info) parent_win_info->children_.push_back(win_info_);

	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 26, 3);
	//int colWidth[3] = { 60, 55, 58 };
	int width_sum = 0;
	for (int i = 0; i < 3; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	//width_sum -= colWidth[2];
	//_Grid->SetColWidth(2, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("시각");
		_HeaderTitles.push_back("체결가");
		_HeaderTitles.push_back("체결");
		_Grid->SetColHeaderTitles(_HeaderTitles);
	}

	SetTimer(1, 40, NULL);
}

void SymbolTickView::OnPaint()
{
	/*
	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rect;
	GetClientRect(rect);

	if (m_pGM == NULL) return;

	m_pGM->BindDC(pDC, rect);

	if (!m_pGM->BeginDraw()) return;

	m_pGM->FillRectangle(rect, _Resource.GridNormalBrush);
	rect.right -= 1;
	rect.bottom -= 1;

	_Grid->DrawGrid(m_pGM, rect);
	_Grid->draw_cells(m_pGM, rect, true, false);
	_Grid->DrawBorder(m_pGM, rect);

	m_pGM->EndDraw();
	*/
	if (!_InitResource) {
		g = new CGraphics(this);
		_InitResource = true;
	}

	CPaintDC dc(this); // device context for painting

	CRect rect;
	::GetClientRect(GetSafeHwnd(), &rect);
	/*
	g->DrawText(_T("Hello, World!"), RGB(255, 0, 0), 200, 20);

	CFont font;
	font.CreatePointFont(140, _T("Segoe UI"));
	g->DrawText(_T("Hello, World!"), RGB(200, 100, 0), 200, 40, DT_SINGLELINE | DT_LEFT, &font);
	g->DrawText(_T("Simple Graphics Class"), RGB(0, 200, 0), 200, 330, DT_SINGLELINE | DT_LEFT, &font);

	g->DrawLine(RGB(255, 0, 0), 10, 10, 100, 100);
	g->DrawLine(RGB(255, 0, 0), 40, 5, 100, 70, 5);

	g->DrawEllipse(RGB(0, 0, 0), 140, 110, 210, 210);
	g->DrawEllipse(RGB(0, 0, 0), 30, 110, 100, 210, 5);
	g->DrawFillEllipse(RGB(0, 0, 0), RGB(0, 200, 0), 30, 230, 100, 330, 5);

	g->DrawGradientFill(RGB(0, 255, 0), RGB(0, 200, 0), 230, 80, 100, 100, false);
	g->DrawGradientFill(RGB(0, 0, 233), RGB(0, 0, 103), 230, 190, 100, 100, true);

	g->DrawFillRectangle(RGB(255, 0, 0), 400, 80, 500, 200);
	g->DrawRectangle(RGB(0, 100, 100), 400, 210, 500, 300);
	*/
	g->DrawFillRectangle(RGB(255, 255, 255), rect);
	rect.right -= 1;
	rect.bottom -= 1;

	_Grid->DrawGrid(g, rect);
	_Grid->draw_cells(g, rect, true, false);
	_Grid->DrawBorder(g, rect);
	// Render what you have done!
	g->Render(&dc, rect.Width(), rect.Height());
}

void SymbolTickView::Init()
{
}

void SymbolTickView::Clear()
{
	for (int row = 1; row < _Grid->RowCount(); row++)
		for (int col = 0; col < _Grid->ColCount(); col++) {
			auto cell = _Grid->FindCell(row, col);
			if (cell) cell->Text("");
		}
	Invalidate();
}

void SymbolTickView::draw_tick(const int row, const int col, const std::string& value, const int up_down)
{
	std::shared_ptr<SmCell> cell = _Grid->FindCell(row, col);
	if (!cell) return;

	cell->Text(value);
	up_down == 1 ?
		cell->CellType(SmCellType::CT_TICK_BUY) :
		cell->CellType(SmCellType::CT_TICK_SELL);
}

void SymbolTickView::update_tick()
{
	if (!tick_control_) return;

	const std::vector<SmTick>& tick_vec = tick_control_->get_tick_vec();
	const int symbol_decimal = tick_control_->get_symbol_decimal();
	for (size_t i = 0; i < tick_vec.size(); i++) {
		const int row = i + 1;
		std::string tick_time = tick_vec[i].time;
		const int up_down = tick_vec[i].updown;
		if (!tick_time.empty()) {
			tick_time.insert(tick_time.length() - 2, ":");
			tick_time.insert(tick_time.length() - 5, ":");
		}
		else continue;
		
		draw_tick(row, 0, tick_time, up_down);
		std::string	value_string = std::format("{0}", tick_vec[i].close);
		SmUtil::insert_decimal(value_string, symbol_decimal);
		draw_tick(row, 1, value_string, up_down);
		draw_tick(row, 2, std::to_string(tick_vec[i].qty), up_down);
	}
}

void SymbolTickView::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SymbolTickView::CreateResource()
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
}

void SymbolTickView::InitHeader()
{
	_HeaderTitles.push_back("시각");
	_HeaderTitles.push_back("체결가");
	_HeaderTitles.push_back("체결");
}


void SymbolTickView::OnTimer(UINT_PTR nIDEvent)
{
	bool needDraw = false;
	if (_EnableQuoteShow) {
		update_tick();
		_EnableQuoteShow = false;
		needDraw = true;
	}

	if (needDraw) Invalidate();

	CBCGPStatic::OnTimer(nIDEvent);
}

void SymbolTickView::OnLButtonDown(UINT nFlags, CPoint point)
{
	//mainApp.event_hub()->trigger_symbol_order_view_event(1, center_window_id_, symbol_);
}


void SymbolTickView::OnSize(UINT nType, int cx, int cy)
{
	CBCGPStatic::OnSize(nType, cx, cy);

	if (g && _InitResource) {
		g->Resize(this);
	}
}
