//#include "stdafx.h"
//#include "SymbolPositionView.h"

#include "stdafx.h"
#include "SymbolPositionView.h"
#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../SmGrid/SmCell.h"
#include "../GridDefine.h"
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
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SymbolPositionView, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(GVN_ENDLABELEDIT, SM_INPLACE_CONTROL, OnEndInPlaceEdit)
	ON_WM_TIMER()
END_MESSAGE_MAP()

SymbolPositionView::SymbolPositionView()
{
	quote_control_ = std::make_shared<DarkHorse::QuoteControl>();
	quote_control_->symbol_position_view(this);
}

void SymbolPositionView::on_update_quote()
{
	_EnableQuoteShow = true;
}

SymbolPositionView::~SymbolPositionView()
{
	quote_control_->symbol_position_view(nullptr);
	//KillTimer(1);
	if (m_pGM != NULL)	delete m_pGM;
}

void SymbolPositionView::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 2, 6);
	int colWidth[6] = { 60, 35, 40, 70, 90, 90 };
	int width_sum = 0;
	for (int i = 0; i < 6; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	width_sum -= colWidth[5];
	_Grid->SetColWidth(5, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("종목");
		_HeaderTitles.push_back("구분");
		_HeaderTitles.push_back("잔고");
		_HeaderTitles.push_back("평균가");
		_HeaderTitles.push_back("현재가");
		_HeaderTitles.push_back("평가손익");

		_Grid->SetColHeaderTitles(_HeaderTitles);
	}

	SetTimer(1, 40, NULL);
}

void SymbolPositionView::OnPaint()
{
	try {
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
		_Grid->DrawCells(m_pGM, rect);
		_Grid->DrawBorder(m_pGM, rect);

		m_pGM->EndDraw();
	}
	catch (const std::exception& e) {
		const std::string error = e.what();

	}
}

void SymbolPositionView::Clear()
{
	for (int i = 0; i < _Grid->ColCount(); i++) {
		auto cell = _Grid->FindCell(1, i);
		if (!cell) continue;
		cell->CellType(SmCellType::CT_NORMAL);
		cell->clear();
	}
	Invalidate();
}

void SymbolPositionView::UpdateSymbolInfo()
{
	if (!quote_control_ || !_Symbol) return;
	const VmQuote quote = quote_control_->get_quote();
	std::string value = std::to_string(quote.close);
	SmUtil::insert_decimal(value, _Symbol->Decimal());
	auto cell = _Grid->FindCell(1, 4);
	cell->Text(value);
}

void SymbolPositionView::Symbol(std::shared_ptr<SmSymbol> val)
{
	_Symbol = val;
	auto quote = mainApp.QuoteMgr()->get_quote(_Symbol->SymbolCode());
	quote->symbol_id = val->Id();
	quote_control_->set_symbol_id(val->Id());
	quote_control_->update_quote(quote);
	UpdateSymbolInfo();
	UpdatePositionInfo();
	Invalidate();
}

void SymbolPositionView::UpdatePositionInfo()
{
	if (!_Account || !_Symbol) return Clear();

	std::shared_ptr<SmPosition> position = mainApp.TotalPosiMgr()->FindAddPosition(_Account->No(), _Symbol->SymbolCode());
	if (position->OpenQty == 0) return Clear();
	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	//position->OpenQty > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);
	cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	std::string value_string = std::format("{0}", _Symbol->Qoute.close);
	if (_Symbol->Decimal() > 0 && value_string.length() > (size_t)_Symbol->Decimal())
		value_string.insert(value_string.length() - _Symbol->Decimal(), 1, '.');
	cell->Text(value_string);
	//position->OpenQty > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);

	const int avg_price = static_cast<int>(position->AvgPrice);
	cell = _Grid->FindCell(1, 1);
	position->Position == SmPositionType::Buy ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);
	if (position->OpenQty > 0)
		cell->Text("매수");
	else if (position->OpenQty < 0)
		cell->Text("매도");
	cell = _Grid->FindCell(1, 2);
	cell->Text(std::to_string(position->OpenQty));
	//position->OpenQty > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);
	cell = _Grid->FindCell(1, 3);
	//position->OpenQty > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);
	value_string = std::format("{0}", avg_price);
	if (_Symbol->Decimal() > 0 && value_string.length() > (size_t)_Symbol->Decimal())
		value_string.insert(value_string.length() - _Symbol->Decimal(), 1, '.');
	cell->Text(value_string);
	cell = _Grid->FindCell(1, 5);
	if (position->OpenPL != 0)
		position->OpenPL > 0 ? cell->CellType(SmCellType::CT_REMAIN_BUY) : cell->CellType(SmCellType::CT_REMAIN_SELL);
	else
		cell->CellType(SmCellType::CT_NORMAL);
	const std::string open_pl = std::format("{0:.2f}", position->OpenPL);
	cell->Text(open_pl);

	//Invalidate();
}

void SymbolPositionView::OnEndEditCell(int nRow, int nCol, CString str)
{
	auto cell = _Grid->FindCell(nRow, nCol);
	if (!cell) return;
	_Grid->SetCellText(nRow, nCol, std::string(str));
	_Grid->OnEndEdit();
	_Editing = false;
}

void SymbolPositionView::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void SymbolPositionView::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SymbolPositionView::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void SymbolPositionView::CreateResource()
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

void SymbolPositionView::InitHeader()
{
	_HeaderTitles.push_back("종목");
	_HeaderTitles.push_back("구분");
	_HeaderTitles.push_back("잔고");
	_HeaderTitles.push_back("평균가");
	_HeaderTitles.push_back("현재가");
	_HeaderTitles.push_back("평가손익");
}


void SymbolPositionView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!_Grid) return;

	// 	auto cell = _Grid->FindCellByPos(point.x, point.y);
	// 	if (cell) {
	// 		CRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	// 		_Grid->Edit(this, cell->Row(), cell->Col(), rect, SM_INPLACE_CONTROL, VK_LBUTTON);
	// 		_Editing = true;
	// 	}

	CBCGPStatic::OnLButtonDblClk(nFlags, point);
}


void SymbolPositionView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!_Grid) return;

	// 	auto cell = _Grid->FindCellByPos(point.x, point.y);
	// 	if (cell) {
	// 		CRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	// 		_Grid->Edit(this, cell->Row(), cell->Col(), rect, SM_INPLACE_CONTROL, VK_LBUTTON);
	// 		_Editing = true;
	// 	}

	CBCGPStatic::OnLButtonDown(nFlags, point);
}

void SymbolPositionView::OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	GV_DISPINFO* pgvDispInfo = (GV_DISPINFO*)pNMHDR;
	GV_ITEM* pgvItem = &pgvDispInfo->item;

	// In case OnEndInPlaceEdit called as window is being destroyed
	if (!IsWindow(GetSafeHwnd()))
		return;

	OnEndEditCell(pgvItem->row, pgvItem->col, pgvItem->strText);
	//InvalidateCellRect(CCellID(pgvItem->row, pgvItem->col));

	switch (pgvItem->lParam)
	{
	case VK_TAB:
	case VK_DOWN:
	case VK_UP:
	case VK_RIGHT:
	case VK_LEFT:
	case VK_NEXT:
	case VK_PRIOR:
	case VK_HOME:
	case VK_END:
		OnKeyDown((UINT)pgvItem->lParam, 0, 0);
		//OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint(-1, -1), (UINT)pgvItem->lParam);
	}

	*pResult = 0;
}


void SymbolPositionView::OnTimer(UINT_PTR nIDEvent)
{
	if (!_Symbol) return;
	bool needDraw = false;
	if (_EnableQuoteShow) {
		UpdateSymbolInfo();
		_EnableQuoteShow = false;
		needDraw = true;
	}
	if (_EnableOrderShow) {
		UpdatePositionInfo();
		_EnableOrderShow = false;
		needDraw = true;
	}

	if (needDraw) Invalidate();

	CBCGPStatic::OnTimer(nIDEvent);
}


void SymbolPositionView::PreSubclassWindow()
{
	/*
	UINT uiHHeight = GetSystemMetrics(SM_CYHSCROLL);
	UINT uiVWidth = GetSystemMetrics(SM_CXVSCROLL);
	CRect rectClient, rectH, rectV;
	GetClientRect(rectClient);
	rectH = rectClient;
	rectH.top = rectH.bottom - uiHHeight;
	rectH.right -= uiVWidth;

	rectV = rectClient;
	rectV.left = rectV.right - uiVWidth;
	rectV.bottom -= uiHHeight;

	m_HScroll.Create(SBS_HORZ | SBS_BOTTOMALIGN | WS_CHILD | WS_VISIBLE, rectH, this, 1100);
	m_VScroll.Create(SBS_VERT | SBS_RIGHTALIGN | WS_CHILD | WS_VISIBLE, rectV, this, 1101);

	SCROLLINFO sci;
	memset(&sci, 0, sizeof(sci));

	sci.cbSize = sizeof(SCROLLINFO);
	sci.fMask = SIF_ALL;
	sci.nMax = 1000;
	sci.nPage = 200;

	m_HScroll.SetScrollInfo(&sci, TRUE);
	m_VScroll.SetScrollInfo(&sci, TRUE);
	*/

	CBCGPStatic::PreSubclassWindow();
}

