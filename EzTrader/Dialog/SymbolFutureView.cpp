#include "stdafx.h"
#include "SymbolFutureView.h"
#include <vector>
#include "../Global/SmTotalManager.h"
#include "../Symbol/MarketDefine.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmProductYearMonth.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Symbol/SmProduct.h"
#include "../Util/SmUtil.h"
#include "../Event/EventHub.h"
#include "../OrderUI/VtOrderConfigManager.h"
#include "../OrderUI/SmOrderPanel.h"
#include <format>

using namespace DarkHorse;
IMPLEMENT_DYNAMIC(SymbolFutureView, CBCGPGridCtrl)

BEGIN_MESSAGE_MAP(SymbolFutureView, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()



SymbolFutureView::SymbolFutureView()
{

}

SymbolFutureView::~SymbolFutureView()
{

}

void SymbolFutureView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* pItem;
	HitTest(point, id, pItem);



	CString msg;
	//msg.Format("%d", nColumn);
	//AfxMessageBox(msg);

	auto found = row_to_symbol_.find(id.m_nRow);
	if (found == row_to_symbol_.end()) return;

	if (_OrderConfigMgr) {
		if (_OrderConfigMgr->_HdCenterWnd) {
			_OrderConfigMgr->_HdCenterWnd->ChangeSymbol(found->second);
		}
	}

	Invalidate();

	CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}

void SymbolFutureView::init_symbol(const int market_index)
{
	const std::vector<DmFuture>& future_vec = mainApp.SymMgr()->get_dm_future_vec();
	if (market_index < 0 || market_index >= (int)future_vec.size()) return;
	ClearGrid();
	const std::map<std::string, std::shared_ptr<SmProductYearMonth>>& year_month_map = future_vec[market_index].product->get_yearmonth_map();
	int row = 0;
	for (auto& [key, value] : year_month_map) {

		std::shared_ptr<SmProductYearMonth> year_month = value;
		auto symbol_vector = year_month->get_symbol_vector();

		for (size_t i = 0; i < symbol_vector.size(); ++i)
		{
			CBCGPGridRow* pRow = GetRow(row);
			pRow->GetItem(0)->SetValue(symbol_vector[i]->SymbolCode().c_str());
			pRow->GetItem(1)->SetValue(symbol_vector[i]->SymbolNameKr().c_str());
			std::string value_string;
			auto quote = mainApp.QuoteMgr()->get_quote(symbol_vector[i]->SymbolCode());
			if (!quote) continue;
			value_string = std::format("{0}", quote->close);
			SmUtil::insert_decimal(value_string, symbol_vector[i]->decimal());

			pRow->GetItem(2)->SetValue(value_string.c_str(), TRUE);

			row_to_symbol_[row] = symbol_vector[i];
			row++;
		}

	}
}

int SymbolFutureView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetGraphicsManager()->SetDefaultGraphicsManagerType(CBCGPGraphicsManager::BCGP_GRAPHICS_MANAGER_D2D);

	EnableHeader(TRUE, ~BCGP_GRID_HEADER_SORT);
	EnableRowHeader(TRUE);
	EnableMarkSortedColumn(FALSE);
	SetClearInplaceEditOnEnter(FALSE);

	SetWholeRowSel(FALSE);
	EnableAlternateRows(FALSE);


	SetRowHeaderWidth(1);
	SetVisualManagerColorTheme(TRUE);

	// Insert columns:
	InsertColumn(0, _T("코드"), 80);
	InsertColumn(1, _T("이름"), 100);
	InsertColumn(2, _T("현재가"), 60);



	FreezeColumns(0);

	const int nColumns = GetColumnCount();


	for (int row = 0; row < grid_row_count; row++) {
		// Create new row:
		CBCGPGridRow* pRow = CreateRow(nColumns);
		// Set each column data:
		for (int nColumn = 0; nColumn < nColumns; nColumn++)
		{
			long lValue = (nColumn + 1) * (row + 1);
			//pRow->GetItem(nColumn)->SetValue(lValue);


			//pRow->GetItem(nColumn)->SetValue(lValue);

			pRow->GetItem(nColumn)->AllowEdit(FALSE);
			_DefaultBackColor = pRow->GetItem(0)->GetBackgroundColor();
		}

		// Add row to grid:
		AddRow(pRow, FALSE /* Don't recal. layout */);

		row++;
	}


	SetColumnAlign(0, 2);
	SetColumnAlign(1, 2);
	SetColumnAlign(2, 2);
	SetColumnAlign(3, 2);
	SetHeaderAlign(0, 2);
	SetHeaderAlign(1, 2);
	SetHeaderAlign(2, 2);
	SetHeaderAlign(3, 2);



	AdjustLayout();

	SetHighlightActiveItem(FALSE);
	SetReadOnly(TRUE);


	//start_timer();

	init_ = true;
	return 0;
}

void SymbolFutureView::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void SymbolFutureView::ClearGrid()
{
	if (!init_) return;

	for (int row = 0; row < GetRowCount(); row++) {
		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		for (int col = 0; col < GetColumnCount(); col++) {
			pRow->GetItem(col)->SetValue("");
		}
	}
}
