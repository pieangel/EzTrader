#include "stdafx.h"
#include "AccountPositionView.h"

#include <map>
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Order/SmAccountOrderManager.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccount.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmAccountPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Order/SmOrderRequest.h"
#include "../Client/ViStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Fund/SmFund.h"

#include "../Event/SmCallbackManager.h"
#include "../Order/OrderUi/DmAccountOrderWindow.h"
#include "../Fund/SmFundOrderDialog.h"
#include "../CompOrder/SmOrderCompMainDialog.h"
#include "../CompOrder/SmFundCompMainDialog.h"
#include "../Controller/AccountPositionControl.h"
#include "../Position/Position.h"
#include "../Util/VtStringUtil.h"
#include "../Log/MyLogger.h"
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AccountPositionView::start_timer()
{
	SetTimer(1, 40, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl

IMPLEMENT_DYNAMIC(AccountPositionView, CBCGPGridCtrl)

AccountPositionView::AccountPositionView()
{
	m_bExtendedPadding = FALSE;
	account_position_control_ = std::make_shared<DarkHorse::AccountPositionControl>();
	account_position_control_->set_event_handler(std::bind(&AccountPositionView::on_update_account_position, this));
	ab_column_widths_vector_.push_back(70);
	ab_column_widths_vector_.push_back(35);
	ab_column_widths_vector_.push_back(70);
	ab_column_widths_vector_.push_back(40);

	dm_column_widths_vector_.push_back(65);
	dm_column_widths_vector_.push_back(35);
	dm_column_widths_vector_.push_back(50);
	dm_column_widths_vector_.push_back(65);
}

void AccountPositionView::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	_Fund = val;
	UpdateFundPositionInfo();
}

AccountPositionView::~AccountPositionView()
{
	//KillTimer(1);
	//mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);
	//mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
}

void AccountPositionView::OnHeaderCheckBoxClick(int nColumn)
{
	CString msg;
	msg.Format("%d", nColumn);
	//AfxMessageBox(msg);
	if (_HeaderCheck) {
		_HeaderCheck = false;

		CheckAll(FALSE);

	}
	else {
		_HeaderCheck = true;

		CheckAll(TRUE);
	}
	UpdateHeaderCheckbox();
	Invalidate();
}

void AccountPositionView::OnRowCheckBoxClick(CBCGPGridRow* pRow)
{
	CString msg;
	msg.Format("%d", pRow->GetRowId());
	//AfxMessageBox(msg);
	if (pRow->GetCheck() == 1)
		pRow->SetCheck(0);
	else if (pRow->GetCheck() == 0)
		pRow->SetCheck(1);
	Invalidate();
}

void AccountPositionView::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	account_ = val;
	if (!account_position_control_) return;

	account_position_control_->load_position_from_account(account_->No());
	enable_position_show_ = true;
	// 계좌 유형에 따라 표시 내용과 표시 간격을 바꾼다.
	//SetColumnName(3, "평균단가");
	//SetColumnWidth(0, column_widths_vector_[0]);
	set_column_widths(account_->Type());
	set_column_names(account_->Type());
	update_account_position();
}

void AccountPositionView::UpdateAcceptedOrder()
{
	if (!account_) return;
	ClearOldContents();
	auto account_order_mgr = mainApp.TotalOrderMgr()->FindAccountOrderManager(account_->No());
	if (!account_order_mgr) return;

	const std::map<std::string, std::shared_ptr<SmSymbolOrderManager>>& symbol_order_mgr_map = account_order_mgr->GetSymbolOrderMgrMap();
	int row = 0;
	for (auto it = symbol_order_mgr_map.begin(); it != symbol_order_mgr_map.end(); ++it) {
		auto symbol_order_mgr = it->second;
		const std::map<std::string, std::shared_ptr<SmOrder>>& accepted_map = symbol_order_mgr->GetAcceptedOrders();
		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		for (auto it2 = accepted_map.begin(); it2 != accepted_map.end(); it2++) {
			auto order = it2->second;
			pRow->GetItem(0)->SetValue(order->SymbolCode.c_str());
			pRow->GetItem(0)->SetTextColor(RGB(255, 0, 0));
			if (order->PositionType == SmPositionType::Buy) {
				pRow->GetItem(0)->SetTextColor(RGB(255, 0, 0));
				pRow->GetItem(1)->SetTextColor(RGB(255, 0, 0));
				pRow->GetItem(2)->SetTextColor(RGB(255, 0, 0));
				pRow->GetItem(1)->SetValue("매수");
			}
			else {
				pRow->GetItem(1)->SetValue("매도");
				pRow->GetItem(0)->SetTextColor(RGB(0, 0, 255));
				pRow->GetItem(1)->SetTextColor(RGB(0, 0, 255));
				pRow->GetItem(2)->SetTextColor(RGB(0, 0, 255));
			}
			pRow->GetItem(2)->SetValue(std::to_string(order->OrderAmount).c_str());
			pRow->GetItem(0)->Redraw();
			_OldContentRowSet.insert(row);
			row++;
		}
	}
	Invalidate();
}

void AccountPositionView::UpdatePositionInfo()
{
	_Mode == 0 ? UpdateAccountPositionInfo() : UpdateFundPositionInfo();
}

void AccountPositionView::LiqSelPositions()
{
	_Mode == 0 ? LiqSelPositionsForAccount() : LiqSelPositionsForFund();
	ClearCheck();
}

void AccountPositionView::LiqAll()
{
	_Mode == 0 ? LiqAllForAccount() : LiqAllForFund();
	ClearCheck();
}

void AccountPositionView::on_update_account_position()
{
	enable_position_show_ = true;
}

BEGIN_MESSAGE_MAP(AccountPositionView, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl message handlers

int AccountPositionView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	InsertColumn(0, _T("종목"), ab_column_widths_vector_[0]);
	InsertColumn(1, _T("구분"), ab_column_widths_vector_[1]);
	InsertColumn(2, _T("평가손익"), ab_column_widths_vector_[2]);
	InsertColumn(3, _T("수량"), ab_column_widths_vector_[3]);

	

	FreezeColumns(0);

	const int nColumns = GetColumnCount();


	for (int row = 0; row < 100; row++) {
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
	EnableCheckBoxes(TRUE);
	SetHeaderAlign(0, 2);
	SetHeaderAlign(1, 2);
	SetHeaderAlign(2, 2);
	SetHeaderAlign(3, 2);




	CheckAll(FALSE);
	UpdateHeaderCheckbox();
	AdjustLayout();

	SetHighlightActiveItem(FALSE);
	SetReadOnly(TRUE);


	//start_timer();
	

	return 0;
}

void AccountPositionView::OnDestroy()
{
	SaveState(_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}

void AccountPositionView::on_timer()
{
	bool needDraw = false;
	if (enable_position_show_) {
		update_account_position();
		needDraw = true;
		enable_position_show_ = false;
	}
	if (needDraw) Invalidate();
}

void AccountPositionView::ClearCheck()
{
	_HeaderCheck = false; CheckAll(FALSE);
	UpdateHeaderCheckbox();	Invalidate();
}

void AccountPositionView::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	enable_position_show_ = true;
}

void AccountPositionView::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void AccountPositionView::set_column_widths(std::vector<int> column_width_vector)
{
	ab_column_widths_vector_ = column_width_vector;
	SetColumnWidth(0, ab_column_widths_vector_[0]);
	SetColumnWidth(1, ab_column_widths_vector_[1]);
	SetColumnWidth(2, ab_column_widths_vector_[2]);
	SetColumnWidth(3, ab_column_widths_vector_[3]);

	EnableCheckBoxes(FALSE);

	AdjustLayout();
}

void AccountPositionView::set_column_widths(const std::string& type)
{
	if (type == "1") {
		SetColumnWidth(0, ab_column_widths_vector_[0]);
		SetColumnWidth(1, ab_column_widths_vector_[1]);
		SetColumnWidth(2, ab_column_widths_vector_[2]);
		SetColumnWidth(3, ab_column_widths_vector_[3]);
		EnableCheckBoxes(FALSE);
	}
	else {
		SetColumnWidth(0, dm_column_widths_vector_[0]);
		SetColumnWidth(1, dm_column_widths_vector_[1]);
		SetColumnWidth(2, dm_column_widths_vector_[2]);
		SetColumnWidth(3, dm_column_widths_vector_[3]);
		EnableCheckBoxes(FALSE);
	}
	AdjustLayout();
}

void AccountPositionView::set_column_names(const std::string& type)
{
	if (type == "1") {
		SetColumnName(0, _T("종목"));
		SetColumnName(1, _T("구분"));
		SetColumnName(2, _T("평가손익"));
		SetColumnName(3, _T("수량"));
	}
	else {
		SetColumnName(0, _T("종목"));
		SetColumnName(1, _T("구분"));
		SetColumnName(2, _T("평균가"));
		SetColumnName(3, _T("평가손익"));
	}
}

void AccountPositionView::LiqSelPositionsForAccount()
{
	if (!account_) return;

	for (auto it = _RowToPositionMap.begin(); it != _RowToPositionMap.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(it->first);
		if (pRow->GetCheck()) {
			std::shared_ptr<SmOrderRequest> order_req = nullptr;
			if (it->second->Position == SmPositionType::Buy)
				order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account_->No(), account_->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
			else
				order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account_->No(), account_->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
			mainApp.Client()->NewOrder(order_req);
		}
	}
}

void AccountPositionView::LiqSelPositionsForFund()
{
	if (!_Fund) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it2 = account_vec.begin(); it2 != account_vec.end(); it2++) {
		auto account = *it2;
		for (auto it = _RowToPositionMap.begin(); it != _RowToPositionMap.end(); ++it) {
			CBCGPGridRow* pRow = GetRow(it->first);
			if (pRow->GetCheck()) {
				std::shared_ptr<SmOrderRequest> order_req = nullptr;
				if (it->second->Position == SmPositionType::Buy)
					order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
				else
					order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
				mainApp.Client()->NewOrder(order_req);
			}
		}
	}
}

void AccountPositionView::LiqAllForAccount()
{
	if (!account_) return;

	for (auto it = _RowToPositionMap.begin(); it != _RowToPositionMap.end(); ++it) {
		std::shared_ptr<SmOrderRequest> order_req = nullptr;
		if (it->second->Position == SmPositionType::Buy)
			order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account_->No(), account_->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
		else
			order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account_->No(), account_->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
		mainApp.Client()->NewOrder(order_req);
	}
}

void AccountPositionView::LiqAllForFund()
{
	if (!_Fund) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it2 = account_vec.begin(); it2 != account_vec.end(); it2++) {
		auto account = *it2;
		for (auto it = _RowToPositionMap.begin(); it != _RowToPositionMap.end(); ++it) {
			std::shared_ptr<SmOrderRequest> order_req = nullptr;
			if (it->second->Position == SmPositionType::Buy)
				order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
			else
				order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
			mainApp.Client()->NewOrder(order_req);
		}
	}
}

void AccountPositionView::update_account_position()
{
	if (!account_ || !account_position_control_) return;

	row_to_position_.clear();
	
	try {
	
	const std::map<std::string, position_p>& account_pos_map = account_position_control_->get_position_map();
	int row = 0;
	for (auto it = account_pos_map.begin(); it != account_pos_map.end(); ++it) {
		const auto position = it->second;
		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		if (position->open_quantity == 0) {
			continue;
		}
		/*
		pRow->GetItem(0)->SetValue(position->symbol_code.c_str(), TRUE);
		//pRow->GetItem(0)->SetTextColor(RGB(255, 0, 0));
		if (position->open_quantity > 0) {
			//pRow->GetItem(0)->SetBackgroundColor(RGB(255, 0, 0));
			pRow->GetItem(1)->SetBackgroundColor(RGB(255, 0, 0));
			//pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
			//pRow->GetItem(3)->SetBackgroundColor(RGB(255, 0, 0));

			//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
			pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
			//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
			//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));

			pRow->GetItem(1)->SetValue("매수", TRUE);
		}
		else if (position->open_quantity < 0){
			pRow->GetItem(1)->SetValue("매도", TRUE);
			//pRow->GetItem(0)->SetBackgroundColor(RGB(0, 0, 255));
			pRow->GetItem(1)->SetBackgroundColor(RGB(0, 0, 255));
			//pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
			//pRow->GetItem(3)->SetBackgroundColor(RGB(0, 0, 255));

			//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
			pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
			//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
			//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
		}

		pRow->GetItem(3)->SetValue(position->open_quantity, TRUE);
		//const std::string open_pl = std::format("{0:.2f}", position->open_profit_loss);
		const int decimal = account_->Type() == "1" ? 2 : 0;
		const std::string open_pl = VtStringUtil::get_format_value(position->open_profit_loss, decimal, true);
		if (position->open_profit_loss > 0) {
			pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
			pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		}
		else if (position->open_profit_loss < 0) {
			pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
			pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		}
		else {
			pRow->GetItem(2)->SetBackgroundColor(_DefaultBackColor);
			pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		}
		pRow->GetItem(2)->SetValue(open_pl.c_str(), TRUE);
		*/
		if (account_->Type() == "1")
			update_ab_account_position(pRow, position);
		else
			update_dm_account_position(pRow, position);
			
		_OldContentRowSet.insert(row);
		row_to_position_[row] = position;
		row++;
	}
	ClearOldContents(row);
	_OldMaxRow = row;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void AccountPositionView::update_dm_account_position(CBCGPGridRow* pRow, position_p position)
{
	pRow->GetItem(0)->SetValue(position->symbol_code.c_str(), TRUE);
	//pRow->GetItem(0)->SetTextColor(RGB(255, 0, 0));
	if (position->open_quantity > 0) {
		//pRow->GetItem(0)->SetBackgroundColor(RGB(255, 0, 0));
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 0, 0));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(255, 0, 0));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));

		pRow->GetItem(1)->SetValue("매수", TRUE);
	}
	else if (position->open_quantity < 0) {
		pRow->GetItem(1)->SetValue("매도", TRUE);
		//pRow->GetItem(0)->SetBackgroundColor(RGB(0, 0, 255));
		pRow->GetItem(1)->SetBackgroundColor(RGB(0, 0, 255));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(0, 0, 255));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
	}
	std::string value_string = VtStringUtil::get_format_value(position->average_price / pow(10, 2), 2, true);
	pRow->GetItem(2)->SetValue(value_string.c_str(), TRUE);
	//const std::string open_pl = std::format("{0:.2f}", position->open_profit_loss);
	const int decimal = account_->Type() == "1" ? 2 : 0;
	std::string open_pl = VtStringUtil::get_format_value(position->open_profit_loss, decimal, true);

	CBCGPGridItem* pItem = pRow->GetItem(3);
	if (pItem) {
		pItem->SetValue(open_pl.c_str(), TRUE);
		if (position->open_profit_loss > 0) {
			pItem->SetBackgroundColor(RGB(255, 0, 0));
			pItem->SetTextColor(RGB(255, 255, 255));
		}
		else if (position->open_profit_loss < 0) {
			pItem->SetBackgroundColor(RGB(0, 0, 255));
			pItem->SetTextColor(RGB(255, 255, 255));
		}
		else {
			pItem->SetBackgroundColor(_DefaultBackColor);
			pItem->SetTextColor(RGB(255, 255, 255));
		}
	}
	
}

void AccountPositionView::update_ab_account_position(CBCGPGridRow* pRow, position_p position)
{
	pRow->GetItem(0)->SetValue(position->symbol_code.c_str(), TRUE);
	//pRow->GetItem(0)->SetTextColor(RGB(255, 0, 0));
	if (position->open_quantity > 0) {
		//pRow->GetItem(0)->SetBackgroundColor(RGB(255, 0, 0));
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 0, 0));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(255, 0, 0));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));

		pRow->GetItem(1)->SetValue("매수", TRUE);
	}
	else if (position->open_quantity < 0) {
		pRow->GetItem(1)->SetValue("매도", TRUE);
		//pRow->GetItem(0)->SetBackgroundColor(RGB(0, 0, 255));
		pRow->GetItem(1)->SetBackgroundColor(RGB(0, 0, 255));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(0, 0, 255));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
	}
	std::string open_quantity = VtStringUtil::get_format_value(position->open_quantity, 0, true);
	pRow->GetItem(3)->SetValue(open_quantity.c_str(), TRUE);
	//const std::string open_pl = std::format("{0:.2f}", position->open_profit_loss);
	const int decimal = account_->Type() == "1" ? 2 : 0;
	std::string open_pl = VtStringUtil::get_format_value(position->open_profit_loss, decimal, true);
	if (position->open_profit_loss > 0) {
		pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
		pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
	}
	else if (position->open_profit_loss < 0) {
		pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
		pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
	}
	else {
		pRow->GetItem(2)->SetBackgroundColor(_DefaultBackColor);
		pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
	}
	pRow->GetItem(2)->SetValue(open_pl.c_str(), TRUE);
}

void AccountPositionView::UpdateAccountPositionInfo()
{
	if (!account_) return;

	//ClearOldContents();
	_RowToPositionMap.clear();
	auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(account_->No());
	const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();
	int row = 0;
	for (auto it = account_pos_map.begin(); it != account_pos_map.end(); ++it) {
		const auto position = it->second;
		CBCGPGridRow* pRow = GetRow(row);
		if (!pRow) continue;
		if (position->OpenQty == 0) {
			continue;
		}

		pRow->GetItem(0)->SetValue(position->SymbolCode.c_str(), TRUE);
		//pRow->GetItem(0)->SetTextColor(RGB(255, 0, 0));
		if (position->Position == SmPositionType::Buy) {
			//pRow->GetItem(0)->SetBackgroundColor(RGB(255, 0, 0));
			pRow->GetItem(1)->SetBackgroundColor(RGB(255, 0, 0));
			//pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
			//pRow->GetItem(3)->SetBackgroundColor(RGB(255, 0, 0));

			//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
			pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
			//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
			//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));

			pRow->GetItem(1)->SetValue("매수", TRUE);
		}
		else {
			pRow->GetItem(1)->SetValue("매도", TRUE);
			//pRow->GetItem(0)->SetBackgroundColor(RGB(0, 0, 255));
			pRow->GetItem(1)->SetBackgroundColor(RGB(0, 0, 255));
			//pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
			//pRow->GetItem(3)->SetBackgroundColor(RGB(0, 0, 255));

			//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
			pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
			//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
			//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
		}

		pRow->GetItem(3)->SetValue(position->OpenQty, TRUE);
		const std::string open_pl = std::format("{0:.2f}", position->OpenPL);
		if (position->OpenPL > 0) {
			pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
			pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		}
		else if (position->OpenPL < 0) {
			pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
			pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		}
		else {
			pRow->GetItem(2)->SetBackgroundColor(_DefaultBackColor);
			pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		}
		pRow->GetItem(2)->SetValue(open_pl.c_str(), TRUE);


		_OldContentRowSet.insert(row);
		_RowToPositionMap[row] = position;
		row++;
	}
	ClearOldContents(row);
	_OldMaxRow = row;
}

void AccountPositionView::UpdateFundPositionInfo()
{
	if (!_Fund) return;

	//ClearOldContents();
	_RowToPositionMap.clear();

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	int row = 0, total_pos_count = 0;
	for (size_t i = 0; i < account_vec.size(); ++i) {
		auto account = account_vec[i];

		auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(account->No());
		const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();

		for (auto it = account_pos_map.begin(); it != account_pos_map.end(); ++it) {
			const auto position = it->second;
			total_pos_count += abs(position->OpenQty);
			CBCGPGridRow* pRow = GetRow(row);
			if (!pRow) continue;
			if (position->OpenQty == 0) {

				continue;
			}

			pRow->GetItem(0)->SetValue(position->SymbolCode.c_str(), TRUE);
			//pRow->GetItem(0)->SetTextColor(RGB(255, 0, 0));
			if (position->Position == SmPositionType::Buy) {
				//pRow->GetItem(0)->SetBackgroundColor(RGB(255, 0, 0));
				pRow->GetItem(1)->SetBackgroundColor(RGB(255, 0, 0));
				//pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
				//pRow->GetItem(3)->SetBackgroundColor(RGB(255, 0, 0));

				//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
				pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
				//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
				//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));

				pRow->GetItem(1)->SetValue("매수", TRUE);
			}
			else {
				pRow->GetItem(1)->SetValue("매도", TRUE);
				//pRow->GetItem(0)->SetBackgroundColor(RGB(0, 0, 255));
				pRow->GetItem(1)->SetBackgroundColor(RGB(0, 0, 255));
				//pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
				//pRow->GetItem(3)->SetBackgroundColor(RGB(0, 0, 255));

				//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
				pRow->GetItem(1)->SetTextColor(RGB(255, 255, 255));
				//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
				//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
			}

			pRow->GetItem(3)->SetValue(position->OpenQty, TRUE);
			const std::string open_pl = std::format("{0:.2f}", position->OpenPL);
			pRow->GetItem(2)->SetValue(open_pl.c_str(), TRUE);

			if (position->OpenPL > 0) {
				pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
				pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
			}
			else if (position->OpenPL < 0) {
				pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
				pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
			}
			else {
				pRow->GetItem(2)->SetBackgroundColor(_DefaultBackColor);
				pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
			}

			_OldContentRowSet.insert(row);
			_RowToPositionMap[row] = position;
			row++;
		}
	}
	if (total_pos_count == 0) row = 0;
	ClearOldContents(row);
	_OldMaxRow = row;
}

void AccountPositionView::ClearOldContents()
{
	for (auto it = _OldContentRowSet.begin(); it != _OldContentRowSet.end(); ++it) {
		CBCGPGridRow* pRow = GetRow(*it);
		for (int i = 0; i < GetColumnCount(); i++) {
			pRow->GetItem(i)->SetValue("");
			pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
		}
	}
}

void AccountPositionView::ClearOldContents(const int& last_index)
{
	try {
		for (int row = last_index; row <= _OldMaxRow; row++) {
			CBCGPGridRow* pRow = GetRow(row);
			for (int i = 0; i < GetColumnCount(); i++) {
				pRow->GetItem(i)->EmptyValue(FALSE);
				pRow->GetItem(i)->SetBackgroundColor(_DefaultBackColor);
				pRow->GetItem(i)->SetTextColor(RGB(255, 255, 255));
			}
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
	}
}

void AccountPositionView::ToggleExtendedPadding()
{
	m_bExtendedPadding = !m_bExtendedPadding;

	EnableRowHeader(TRUE);	// Need to recalculate the width
	AdjustLayout();
}




void AccountPositionView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridItemID id;
	CBCGPGridItem* pItem;
	HitTest(point, id, pItem);



	CString msg;
	//msg.Format("%d", nColumn);
	//AfxMessageBox(msg);
	/*
	if (id.m_nColumn == -1) {
		if (id.m_nRow == -1) {
			if (_HeaderCheck) {  _HeaderCheck = false; CheckAll(FALSE); }
			else { _HeaderCheck = true; CheckAll(TRUE); }
			UpdateHeaderCheckbox();
		}
		else {
			CBCGPGridRow* pRow = GetRow(id.m_nRow);
			if (pRow->GetCheck())
				pRow->SetCheck(FALSE);
			else
				pRow->SetCheck(TRUE);
		}
	}
	else {
		auto found = _RowToPositionMap.find(id.m_nRow);
		if (found == _RowToPositionMap.end()) return;

		if (_OrderWnd) _OrderWnd->OnSymbolClicked(found->second->SymbolCode);
		if (_FundOrderWnd) _FundOrderWnd->OnSymbolClicked(found->second->SymbolCode);
	}
	Invalidate();
	*/





	if (id.m_nColumn == -1) {
		if (id.m_nRow == -1) {
			if (_HeaderCheck) { _HeaderCheck = false; CheckAll(FALSE); }
			else { _HeaderCheck = true; CheckAll(TRUE); }
			UpdateHeaderCheckbox();
		}
		else {
			CBCGPGridRow* pRow = GetRow(id.m_nRow);
			if (pRow->GetCheck())
				pRow->SetCheck(FALSE);
			else
				pRow->SetCheck(TRUE);
		}
	}
	else {

		auto found = _RowToPositionMap.find(id.m_nRow);
		if (found == _RowToPositionMap.end()) return;

		if (_OrderWnd) _OrderWnd->OnSymbolClicked(found->second->SymbolCode);
		//if (_FundOrderWnd) _FundOrderWnd->OnSymbolClicked(found->second->SymbolCode);
		//if (_CompOrderWnd) _CompOrderWnd->OnSymbolClicked(found->second->SymbolCode);
		//if (_CompFundWnd) _CompFundWnd->OnSymbolClicked(found->second->SymbolCode);
	}
	Invalidate();






	//CBCGPGridCtrl::OnLButtonDown(nFlags, point);
}


void AccountPositionView::OnTimer(UINT_PTR nIDEvent)
{
	if (enable_position_show_) {
		update_account_position();
		//needDraw = true;
		enable_position_show_ = false;
	}

	if (_EnableQuoteShow) {
		UpdatePositionInfo();
		_EnableQuoteShow = false;
	}

	CBCGPGridCtrl::OnTimer(nIDEvent);
}
