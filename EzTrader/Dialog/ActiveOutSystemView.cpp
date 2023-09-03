#include "stdafx.h"
#include "ActiveOutSystemView.h"

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
#include "../OutSystem/SmOutSystem.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Fund/SmFund.h"
#include "../Fund/SmFundManager.h"
#include <format>

using namespace DarkHorse;
IMPLEMENT_DYNAMIC(ActiveOutSystemView, CBCGPGridCtrl)

BEGIN_MESSAGE_MAP(ActiveOutSystemView, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()



ActiveOutSystemView::ActiveOutSystemView()
{

}

ActiveOutSystemView::~ActiveOutSystemView()
{

}

void ActiveOutSystemView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* pItem;
	HitTest(point, id, pItem);



	CString msg;
	//msg.Format("%d", nColumn);
	//AfxMessageBox(msg);

	auto found = row_to_out_system_.find(id.m_nRow);
	if (found == row_to_out_system_.end()) return;

	//auto symbol = mainApp.SymMgr()->FindSymbol(found->second->symbol_code);
	//if (!symbol) return;
	//mainApp.event_hub()->trigger_ab_symbol_event(1, symbol);
	Invalidate();

}



void ActiveOutSystemView::add_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system)
{
	if (!out_system) return;
	out_systems_.push_back(out_system);
	//init_grid();
	int row_count = GetRowCount();
	int index = (int)out_systems_.size() - 1;
	CBCGPGridRow* pRow = nullptr;
	if (index <= row_count) {
		pRow = GetRow(index);
	}
	else {
		pRow = CreateRow(GetColumnCount());
	}
	if (out_system->order_type() == DarkHorse::OrderType::Fund)
		pRow->GetItem(0)->SetValue(out_system->fund()->Name().c_str());
	else
		pRow->GetItem(0)->SetValue(out_system->account()->No().c_str());
	pRow->GetItem(1)->SetValue(out_system->symbol()->SymbolCode().c_str());

	row_to_out_system_[out_system->id()] = index;
	Invalidate();
}

void ActiveOutSystemView::remove_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system)
{
	if (!out_system) return;

	auto it = row_to_out_system_.find(out_system->id());
	if (it == row_to_out_system_.end()) return;

	std::erase_if(out_systems_, [&](const std::shared_ptr<SmOutSystem>& in_out_system) {
		return in_out_system->id() == out_system->id(); });
	//ClearGrid();
	//init_grid();
	RemoveRow(it->second);
	row_to_out_system_.erase(it);
	remap_row_to_out_system();
	Invalidate();
}

int ActiveOutSystemView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	std::vector<std::string> title = { "계좌번호", "종목", "포지션", "평균단가", "현재가", "평가손익", "매도주문", "매수주문", "신호차트", "청산" };
	std::vector<int> col_width = { 100, 100, 80, 100, 100, 100, 100, 100, 124 - GetSystemMetrics(SM_CXVSCROLL), 80 };


	// Insert columns:
	for (size_t i = 0; i < title.size(); i++) {
		InsertColumn(i, title[i].c_str(), col_width[i]);
	}


	FreezeColumns(0);

	const int nColumns = GetColumnCount();

	for (int row = 0; row < active_out_system_row; row++) {
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
		AddRow(pRow, FALSE );

		row++;
	}

	for (int i = 0; i < GetColumnCount(); i++)
	{
		SetColumnAlign(i, 2);
		SetHeaderAlign(i, 2);
	}

	AdjustLayout();

	SetHighlightActiveItem(FALSE);
	SetReadOnly(TRUE);

	init_grid();
	//start_timer();

	init_ = true;
	return 0;
}

void ActiveOutSystemView::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void ActiveOutSystemView::init_grid()
{
	if (out_systems_.empty()) return;

	for (size_t i = 0; i < out_systems_.size(); i++)
	{
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		if (out_systems_[i]->order_type() == DarkHorse::OrderType::Fund)
			pRow->GetItem(0)->SetValue(out_systems_[i]->fund()->Name().c_str());
		else
			pRow->GetItem(0)->SetValue(out_systems_[i]->account()->No().c_str());
		pRow->GetItem(1)->SetValue(out_systems_[i]->symbol()->SymbolCode().c_str());

		row_to_out_system_[out_systems_[i]->id()] = i;
	}

}

void ActiveOutSystemView::remap_row_to_out_system()
{
	for (size_t i = 0; i < out_systems_.size(); i++)
	{
		CBCGPGridRow* pRow = GetRow(i);
		if (!pRow) continue;
		row_to_out_system_[out_systems_[i]->id()] = i;
	}
}

void ActiveOutSystemView::ClearGrid()
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
