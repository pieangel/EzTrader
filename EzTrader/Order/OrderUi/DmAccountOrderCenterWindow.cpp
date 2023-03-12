//#include "stdafx.h"
//#include "DmAccountOrderCenterWindow.h"

#include "stdafx.h"
#include "../../DarkHorse.h"
#include "DmAccountOrderCenterWindow.h"
#include "afxdialogex.h"
#include "../../Grid/customcells.h"
#include "../SmOrderGridConst.h"
#include "../../Util/VtStringUtil.h"
#include "../../Global/SmTotalManager.h"
#include "../../Symbol/SmSymbolManager.h"
#include "../../Symbol/SmSymbol.h"
#include "../../Symbol/SmProduct.h"
#include "../../Symbol/SmProductYearMonth.h"
#include "../../Symbol/DomesticMarketDefine.h"
#include "BCGPGridCtrl.h"
#include <format>
#include "../../Log/MyLogger.h"
#include "../../SmGrid/SmGrid.h"
#include "../../SmGrid/SmCell.h"
#include "../../Account/SmAccount.h"
#include "../../Account/SmAccountManager.h"
#include "DmAccountOrderWindow.h"
#include "../../Symbol/SmSymbolTableDialog.h"
#include "../../Task/SmTaskRequestMaker.h"
#include "../../Client/ViStockClient.h"
#include "../OrderWndConst.h"
#include "../../Order/SmSymbolOrderManager.h"
#include "../../Order/SmTotalOrderManager.h"
#include "../../Position/SmTotalPositionManager.h"
#include "../../Position/SmAccountPositionManager.h"
#include "../../Position/SmPosition.h"
#include "../../Order/SmOrderRequest.h"
#include "../../Client/ViStockClient.h"
#include "../../Order/SmOrderRequestManager.h"
#include "../SmOrderSetDialog.h"
#include "../../Global/SmTotalManager.h"
#include "../../Task/SmTaskArg.h"
#include "../../Task/SmTaskRequestManager.h"
#include "../../Util/IdGenerator.h"

// SmOrderWnd dialog
#define BTN_ORDER_AMOUNT 0x00000001
#define BTN_ORDER_SELL 0x00000002
#define BTN_ORDER_BUY 0x00000003
#define BTN_ORDER_CENTER_QUOTE 0x00000004

using namespace DarkHorse;

int DmAccountOrderCenterWindow::DeltaOrderArea = 0;

IMPLEMENT_DYNAMIC(DmAccountOrderCenterWindow, CBCGPDialog)

DmAccountOrderCenterWindow::DmAccountOrderCenterWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_CENTER, pParent)
{

	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	id_ = IdGenerator::get_id();
}

DmAccountOrderCenterWindow::~DmAccountOrderCenterWindow()
{
	//KillTimer(1);
}

void DmAccountOrderCenterWindow::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	account_ = val;
	symbol_order_view_.Account(val);
	symbol_order_view_.Refresh();
	symbol_position_view_.Account(val);
	symbol_position_view_.Refresh();
}

void DmAccountOrderCenterWindow::Selected(bool val)
{
	selected_ = val;
	symbol_order_view_.Selected(val);
}

void DmAccountOrderCenterWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, combo_symbol_);
	DDX_Control(pDX, IDC_STATIC_ORDER, symbol_order_view_);
	DDX_Control(pDX, IDC_STATIC_POSITION, symbol_position_view_);
	DDX_Control(pDX, IDC_STATIC_QUOTE, symbol_tick_view_);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_NAME_KR, static_symbol_name_);
	DDX_Control(pDX, IDC_CHECK_SHOW_REAL_QUOTE, _CheckShowRealTick);
	DDX_Control(pDX, IDC_SPIN_ORDER_AMOUNT, _OrderAmountSpin);
	DDX_Control(pDX, IDC_STATIC_FILLED, _StaticFilledCount);
	DDX_Control(pDX, IDC_CHECK_LOSS, _CheckLoss);
	DDX_Control(pDX, IDC_CHECK_PROFIT, _CheckProfit);
	DDX_Control(pDX, IDC_EDIT_LOSS, _EditLoss);
	DDX_Control(pDX, IDC_EDIT_PROFIT, _EditProfit);
	DDX_Control(pDX, IDC_EDIT_SLIP, _EditSlip);
	DDX_Control(pDX, IDC_EDIT_AMOUNT, _EditAmount);
	DDX_Control(pDX, IDC_SPIN_LCUT, _SpinLossCut);
	DDX_Control(pDX, IDC_SPIN_PCUT, _SpinProfitCut);
	DDX_Control(pDX, IDC_SPIN_SLIP, _SpinSlip);
	DDX_Control(pDX, IDC_RADIO_MARKET, _RadioMarket);
	DDX_Control(pDX, IDC_RADIO_PRICE, _RadioPrice);

	//DDX_Control(pDX, IDC_STATIC_CONFIG, _ConfigGrid);

	DDX_Control(pDX, IDC_BTN_SEARCH, _BtnSearch);
	DDX_Control(pDX, IDC_STATIC_GROUP2, _Group2);
	DDX_Control(pDX, IDC_STATIC_GROUP3, _Group3);
	DDX_Control(pDX, IDC_STATIC_GROUP4, _Group4);
	DDX_Control(pDX, IDC_SPIN_BUY_AVAIL, _BuyAvail);
	DDX_Control(pDX, IDC_SPIN_SELL_AVAIL, _SellAvail);
	DDX_Control(pDX, IDC_STATIC_FILLED_REMAIN, filled_remain_button_);
	DDX_Control(pDX, IDC_STATIC_REMAIN, remain_button_);
}


BEGIN_MESSAGE_MAP(DmAccountOrderCenterWindow, CBCGPDialog)
	ON_WM_TIMER()
	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &DmAccountOrderCenterWindow::OnCbnSelchangeComboSymbol)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_SHOW_REAL_QUOTE, &DmAccountOrderCenterWindow::OnBnClickedCheckShowRealQuote)
	ON_BN_CLICKED(IDC_BTN_REFRESH_ORDER, &DmAccountOrderCenterWindow::OnBnClickedBtnRefreshOrder)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_SYMBOL, &DmAccountOrderCenterWindow::OnBnClickedBtnSymbol)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &DmAccountOrderCenterWindow::OnUmSymbolSelected)
	ON_BN_CLICKED(IDC_BTN_AMOUNT1, &DmAccountOrderCenterWindow::OnBnClickedBtnAmount1)
	ON_BN_CLICKED(IDC_BTN_AMOUNT2, &DmAccountOrderCenterWindow::OnBnClickedBtnAmount2)

	ON_BN_CLICKED(IDC_BTN_AMOUNT5, &DmAccountOrderCenterWindow::OnBnClickedBtnAmount5)
	ON_BN_CLICKED(IDC_BTN_AMOUNT3, &DmAccountOrderCenterWindow::OnBnClickedBtnAmount3)
	ON_BN_CLICKED(IDC_BTN_AMOUNT4, &DmAccountOrderCenterWindow::OnBnClickedBtnAmount4)
	ON_EN_CHANGE(IDC_EDIT_AMOUNT, &DmAccountOrderCenterWindow::OnEnChangeEditAmount)
	ON_BN_CLICKED(IDC_BTN_LIQ_SYMBOL_POSITION, &DmAccountOrderCenterWindow::OnBnClickedBtnLiqSymbolPosition)
	ON_BN_CLICKED(IDC_CHECK_PROFIT, &DmAccountOrderCenterWindow::OnBnClickedCheckProfit)
	ON_BN_CLICKED(IDC_CHECK_LOSS, &DmAccountOrderCenterWindow::OnBnClickedCheckLoss)
	ON_BN_CLICKED(IDC_RADIO_MARKET, &DmAccountOrderCenterWindow::OnBnClickedRadioMarket)
	ON_BN_CLICKED(IDC_RADIO_PRICE, &DmAccountOrderCenterWindow::OnBnClickedRadioPrice)
	ON_EN_CHANGE(IDC_EDIT_PROFIT, &DmAccountOrderCenterWindow::OnEnChangeEditProfit)
	ON_EN_CHANGE(IDC_EDIT_LOSS, &DmAccountOrderCenterWindow::OnEnChangeEditLoss)
	ON_EN_CHANGE(IDC_EDIT_SLIP, &DmAccountOrderCenterWindow::OnEnChangeEditSlip)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &DmAccountOrderCenterWindow::OnBnClickedBtnSearch)
	ON_BN_CLICKED(IDC_BTN_SET, &DmAccountOrderCenterWindow::OnBnClickedBtnSet)
	ON_STN_CLICKED(IDC_STATIC_FILLED_REMAIN, &DmAccountOrderCenterWindow::OnStnClickedStaticFilledRemain)
	ON_BN_CLICKED(IDC_CHECK_FIX_HOGA, &DmAccountOrderCenterWindow::OnBnClickedCheckFixHoga)
END_MESSAGE_MAP()


// DmAccountOrderCenterWindow message handlers

#define WS_CAPTION          0x00C00000L

LRESULT DmAccountOrderCenterWindow::OnEnterSizeMove(WPARAM wparam, LPARAM lparam)
{
	resizing_ = true;
	// do stuff      
	return (LRESULT)0;
}
LRESULT DmAccountOrderCenterWindow::OnExitSizeMove(WPARAM wparam, LPARAM lparam)
{

	resizing_ = false;
	// do stuff      
	return (LRESULT)0;
}




void DmAccountOrderCenterWindow::SetMainDialog(DmAccountOrderWindow* main_dialog)
{
	symbol_order_view_.SetMainDialog(main_dialog);
}



void DmAccountOrderCenterWindow::SetSelected(const bool& selected)
{
	selected_ = selected;
	symbol_order_view_.Selected(selected);
	symbol_order_view_.Invalidate();
}

void DmAccountOrderCenterWindow::OnSymbolClicked(const std::string& symbol_code)
{
	auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (symbol) set_symbol_from_out(symbol);
}

void DmAccountOrderCenterWindow::SetOrderAmount(const int& count)
{
	CString strValue;
	strValue.Format("%d", count);
	_EditAmount.SetWindowText(strValue);
}

int DmAccountOrderCenterWindow::GetPositionCount()
{
	if (!account_ || !symbol_) return 0;

	std::shared_ptr<SmPosition> position = mainApp.TotalPosiMgr()->FindAddPosition(account_->No(), symbol_->SymbolCode());

	return position->OpenQty;
}

void DmAccountOrderCenterWindow::ArrangeCenterValue()
{
	symbol_order_view_.ArrangeCenterValue();
}

void DmAccountOrderCenterWindow::CreateResource()
{

}

void DmAccountOrderCenterWindow::set_symbol(std::shared_ptr<DarkHorse::SmSymbol>symbol)
{
	symbol_ = symbol;
}

void DmAccountOrderCenterWindow::set_symbol_name(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	static_symbol_name_.SetWindowText(make_symbol_name(symbol).c_str());
}

std::string DmAccountOrderCenterWindow::make_symbol_name(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return "";
	std::string symbol_info(symbol->SymbolNameKr());
	symbol_info.append(" [");
	symbol_info.append(symbol->SymbolCode());
	symbol_info.append("]");
	return symbol_info;
}

int DmAccountOrderCenterWindow::add_to_symbol_combo(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return -1;
	auto found = symbol_to_index_.find(symbol->SymbolCode());
	if (found != symbol_to_index_.end()) return -1;
	const std::string symbol_name = make_symbol_name(symbol);
	if (symbol_name.empty()) return -1;
	const int index = combo_symbol_.AddString(symbol_name.c_str());
	index_to_symbol_[index] = symbol;
	symbol_to_index_[symbol->SymbolCode()] = index;
	return index;
}

void DmAccountOrderCenterWindow::init_control()
{
	combo_symbol_.SetDroppedWidth(250);

	_CheckShowRealTick.SetCheck(BST_CHECKED);
	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	if (pLayout != NULL)
	{
		pLayout->AddAnchor(IDC_STATIC_POSITION, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeNone);
		pLayout->AddAnchor(IDC_STATIC_ORDER, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeVert);
		pLayout->AddAnchor(IDC_STATIC_QUOTE, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeNone);
	}

	_StaticAccountName.m_clrText = RGB(255, 255, 255);
	static_symbol_name_.m_clrText = RGB(255, 255, 255);

	CRect rcWnd;
	symbol_order_view_.GetWindowRect(&rcWnd);
	ScreenToClient(rcWnd);

	rcWnd.bottom = rcWnd.top + rcWnd.Height() + DmAccountOrderCenterWindow::DeltaOrderArea;

	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("1");

	((CEdit*)GetDlgItem(IDC_EDIT_PROFIT))->SetWindowText("2");
	((CEdit*)GetDlgItem(IDC_EDIT_LOSS))->SetWindowText("2");
	((CEdit*)GetDlgItem(IDC_EDIT_SLIP))->SetWindowText("2");

	_OrderAmountSpin.SetRange32(0, 100);
	_SpinLossCut.SetRange32(0, 100);
	_SpinProfitCut.SetRange32(0, 100);
	_SpinSlip.SetRange32(0, 100);

	_BuyAvail.SetRange32(0, 100);
	_SellAvail.SetRange32(0, 100);

	((CButton*)GetDlgItem(IDC_RADIO_PRICE))->SetCheck(BST_CHECKED);

	_BtnSearch.SetImage(IDB_BITMAP_SEARCH, IDB_BITMAP_SEARCH, 0, IDB_BITMAP_SEARCH);
	_BtnSearch.m_bRighImage = FALSE;
	_BtnSearch.m_bTopImage = FALSE;
	//_BtnSearch.SizeToContent();
	_BtnSearch.RedrawWindow();

	//_ConfigGrid.AttachGrid(this, IDC_STATIC_CONFIG);

	filled_remain_button_.SetUp();
	filled_remain_button_.Text("√º∞·µ» ¿‹∞Ì");
	filled_remain_button_.ShowWindow(SW_HIDE);

	remain_button_.SetUp();
	remain_button_.Text("¿‹∞Ì");
	//_RemainButton.OrderCenterWnd(this);
}

void DmAccountOrderCenterWindow::init_views()
{
	symbol_order_view_.SetUp();
	symbol_position_view_.SetUp();
	symbol_tick_view_.SetUp();
}

void DmAccountOrderCenterWindow::init_dm_symbol()
{
	const std::vector<DarkHorse::DmFuture>& future_vec = mainApp.SymMgr()->get_dm_future_vec();
	for (int i = static_cast<int>(future_vec.size() - 1); i >= 0; --i) {
		std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map = future_vec[i].product->get_yearmonth_map();
		if (year_month_map.size() == 0) continue;
		std::shared_ptr<DarkHorse::SmSymbol> symbol = year_month_map.begin()->second->get_first_symbol();
		if (!symbol) continue;
		add_to_symbol_combo(symbol);
	}
	if (index_to_symbol_.empty()) return;
	current_combo_index_ = 0;
	const auto symbol = index_to_symbol_[current_combo_index_];
	set_symbol_name(symbol);
	combo_symbol_.SetCurSel(current_combo_index_);
	set_symbol_info(symbol);
	set_symbol(symbol);
}

BOOL DmAccountOrderCenterWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	init_views();
	init_control();
	init_dm_symbol();
	init_dialog_ = true;
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void DmAccountOrderCenterWindow::SetQuote(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;



	const int& close = symbol->Qoute.close;
	const int int_tick_size = static_cast<int>(symbol->TickSize() * std::pow(10, symbol->Decimal()));
	const int start_value = close + (close_row_ - value_start_row_) * int_tick_size;
	try {
		std::shared_ptr<SmGrid> grid = symbol_order_view_.Grid();
		int value = start_value;
		for (int i = 1; i < grid->RowCount(); i++) {
			grid->SetCellText(i, DarkHorse::OrderGridHeader::QUOTE, std::to_string(value));
			value += int_tick_size;
		}
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void DmAccountOrderCenterWindow::SetHoga(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{

}

void DmAccountOrderCenterWindow::OnClickSymbol(const std::string& symbol_info)
{

}

void DmAccountOrderCenterWindow::OnTimer(UINT_PTR nIDEvent)
{
	//if (!_Resizing) _OrderArea.Refresh();

	//_PositionArea.UpdatePositionInfo();
	//_PositionArea.UpdateSymbolInfo();
	//_QuoteArea.UpdateSymbolInfo();

	CTime t1;
	t1 = CTime::GetCurrentTime();
	//m_bar.SetPaneText(1, t1.Format("%H:%M:%S"));
	if (account_ && symbol_) {
		auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account_->No(), symbol_->SymbolCode());
		int filled_count = symbol_order_mgr->GetUnsettledCount();
		CString strCount;
		strCount.Format("%d", filled_count);
		_StaticFilledCount.SetWindowText(strCount);
		//_StaticFilledCount.Invalidate();
	}
}


void DmAccountOrderCenterWindow::set_symbol_from_out(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	add_to_symbol_combo(symbol);
	set_symbol_info(symbol);
	set_symbol_name(symbol);
	set_symbol(symbol);
}

void DmAccountOrderCenterWindow::UpdateOrderSettings()
{
	if (!_EditLoss.GetSafeHwnd()) return;
	if (!_EditProfit.GetSafeHwnd()) return;
	if (!_EditSlip.GetSafeHwnd()) return;

	DarkHorse::SmOrderSettings settings;
	if (_CheckProfit.GetCheck() == BST_CHECKED)
		settings.ProfitCut = true;
	else
		settings.ProfitCut = false;

	if (_CheckLoss.GetCheck() == BST_CHECKED)
		settings.LossCut = true;
	else
		settings.LossCut = false;

	if (_RadioMarket.GetCheck() == BST_CHECKED)
		settings.PriceType = SmPriceType::Market;
	else
		settings.PriceType = SmPriceType::Price;

	CString strValue;
	_EditProfit.GetWindowText(strValue);
	settings.ProfitCutTick = _ttoi(strValue);
	_EditLoss.GetWindowText(strValue);
	settings.LossCutTick = _ttoi(strValue);
	_EditSlip.GetWindowText(strValue);
	settings.SlipTick = _ttoi(strValue);

	symbol_order_view_.UpdateOrderSettings(settings);
}

void DmAccountOrderCenterWindow::SetCutMode()
{
	profit_loss_cut_mode_ = 0;
	if (_CheckLoss.GetCheck() == BST_CHECKED) {
		profit_loss_cut_mode_ = 2;
		if (_CheckProfit.GetCheck() == BST_CHECKED) {
			profit_loss_cut_mode_ = 3;
		}
	}
	else if (_CheckProfit.GetCheck() == BST_CHECKED) {
		profit_loss_cut_mode_ = 1;
		if (_CheckLoss.GetCheck() == BST_CHECKED) {
			profit_loss_cut_mode_ = 3;
		}
	}
	symbol_order_view_.CutMode(profit_loss_cut_mode_);
}

void DmAccountOrderCenterWindow::set_symbol_info(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;
	symbol_position_view_.Clear();
	symbol_tick_view_.Clear();
	symbol_order_view_.Clear();
	symbol_position_view_.Symbol(symbol);
	symbol_order_view_.Symbol(symbol);
	symbol_tick_view_.Symbol(symbol);
	request_dm_symbol_master(symbol->SymbolCode());
	mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
}

void DmAccountOrderCenterWindow::request_dm_symbol_master(const std::string symbol_code)
{
	DmSymbolMasterReq req;
	req.symbol_code = symbol_code;
	SmTaskArg arg;
	arg.TaskType = SmTaskType::DmSymbolMaster;
	arg.Param = req;
	mainApp.TaskReqMgr()->AddTask(std::move(arg));
}

void DmAccountOrderCenterWindow::SetRowWide()
{
	symbol_order_view_.SetAllRowHeight(WideRowHeight);
	RecalcOrderAreaHeight(this);
	symbol_order_view_.Invalidate();
}

void DmAccountOrderCenterWindow::SetRowNarrow()
{
	symbol_order_view_.SetAllRowHeight(DefaultRowHeight);
	RecalcOrderAreaHeight(this);
	symbol_order_view_.Invalidate();
}

void DmAccountOrderCenterWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	symbol_order_view_.OnOrderChanged(account_id, symbol_id);
	symbol_position_view_.OnOrderChanged(account_id, symbol_id);
}

int DmAccountOrderCenterWindow::RecalcOrderAreaHeight(CWnd* wnd, bool bottom_up)
{
	CRect rcTopMost;
	CRect rcOrderArea;
	CRect rcOrderWnd;
	symbol_order_view_.GetWindowRect(rcOrderArea);

	GetWindowRect(rcOrderWnd);

	wnd->GetWindowRect(rcTopMost);

	CRect rcSymbol;
	static_symbol_name_.GetWindowRect(rcSymbol);

	int y_del = 0, extra_height = 0;
	y_del = rcTopMost.bottom - rcOrderArea.top;
	y_del -= 8;

	extra_height = symbol_order_view_.RecalRowCount(y_del);

	if (extra_height > 0) {
		CRect rcParent;
		CRect rect;
		symbol_order_view_.GetWindowRect(rect);

		rect.right -= 2;
		rect.bottom -= extra_height;
		//_OrderArea.SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);

		GetWindowRect(rect);
		//ScreenToClient(rect);
		rect.bottom -= extra_height;
		//MoveWindow(rect);
		//SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);
	}


	return extra_height;
}

void DmAccountOrderCenterWindow::OnCbnSelchangeComboSymbol()
{
	const int cur_sel = combo_symbol_.GetCurSel();
	if (cur_sel < 0) return;

	auto found = index_to_symbol_.find(cur_sel);

	if (found == index_to_symbol_.end()) return;

	current_combo_index_ = cur_sel;

	symbol_ = index_to_symbol_[current_combo_index_];

	set_symbol_info(symbol_);
	set_symbol_name(symbol_);
}


void DmAccountOrderCenterWindow::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (init_dialog_ && symbol_order_view_.GetSafeHwnd()) {
		CRect rcWnd;
		GetWindowRect(rcWnd);
		//_OrderArea.RecalRowCount(rcWnd.Height());
		symbol_order_view_.Invalidate();
	}

}


void DmAccountOrderCenterWindow::OnBnClickedCheckShowRealQuote()
{
	show_symbol_tick_view_ ? show_symbol_tick_view_ = false : show_symbol_tick_view_ = true;
	CWnd* parent = GetParent();
	CRect rcQuote;
	symbol_tick_view_.GetWindowRect(rcQuote);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	parent->ScreenToClient(rcWnd);
	if (show_symbol_tick_view_) {
		rcWnd.right = rcWnd.left + rcWnd.Width() + rcQuote.Width();
		symbol_tick_view_.ShowWindow(SW_SHOW);
	}
	else {
		rcWnd.right = rcWnd.left + rcWnd.Width() - rcQuote.Width();
		symbol_tick_view_.ShowWindow(SW_HIDE);
	}

	MoveWindow(rcWnd);

	((DmAccountOrderWindow*)parent)->RecalcChildren(show_symbol_tick_view_ ? CM_SHOW_TICK : CM_HIDE_TICK);
}


void DmAccountOrderCenterWindow::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
}


void DmAccountOrderCenterWindow::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
}


void DmAccountOrderCenterWindow::OnBnClickedBtnRefreshOrder()
{
	//_OrderArea.UpdateOrder(_Symbol->SymbolCode());
	//_OrderArea.Invalidate(FALSE);
	mainApp.Client()->GetFilledOrderList(account_->No(), account_->Pwd());
}


BOOL DmAccountOrderCenterWindow::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return FALSE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void DmAccountOrderCenterWindow::OnBnClickedBtnSymbol()
{

}

LRESULT DmAccountOrderCenterWindow::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = (int)wParam;
	std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	if (symbol) set_symbol_from_out(symbol);
	return 1;
}


BOOL DmAccountOrderCenterWindow::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
			//OnEntered();
			return TRUE;
		}
		else if (pMsg->wParam == VK_SPACE) {
			symbol_order_view_.PutOrderBySpaceBar();
			return TRUE;
		}
		else if (pMsg->wParam == VK_DOWN) {
			symbol_order_view_.ChangeOrderByKey(-1);
		}
		else if (pMsg->wParam == VK_UP) {
			symbol_order_view_.ChangeOrderByKey(1);
		}
	}

	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void DmAccountOrderCenterWindow::OnBnClickedBtnAmount1()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("1");
}


void DmAccountOrderCenterWindow::OnBnClickedBtnAmount2()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("2");
}


void DmAccountOrderCenterWindow::OnBnClickedBtnAmount5()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("5");
}


void DmAccountOrderCenterWindow::OnBnClickedBtnAmount3()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("3");
}


void DmAccountOrderCenterWindow::OnBnClickedBtnAmount4()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("4");
}


void DmAccountOrderCenterWindow::OnEnChangeEditAmount()
{
	CEdit* amount = (CEdit*)GetDlgItem(IDC_EDIT_AMOUNT);
	if (!amount) return;

	CString strValue;
	amount->GetWindowText(strValue);
	symbol_order_view_.OrderAmount(_ttoi(strValue));
}





void DmAccountOrderCenterWindow::OnBnClickedBtnLiqSymbolPosition()
{
	if (!account_ || !symbol_) return;

	auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(account_->No());
	const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();
	auto found = account_pos_map.find(symbol_->SymbolCode());
	if (found == account_pos_map.end()) return;

	std::shared_ptr<SmOrderRequest> order_req = nullptr;
	if (found->second->Position == SmPositionType::Buy)
		order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account_->No(), account_->Pwd(), symbol_->SymbolCode(), 0, abs(found->second->OpenQty), DarkHorse::SmPriceType::Market);
	else
		order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account_->No(), account_->Pwd(), symbol_->SymbolCode(), 0, abs(found->second->OpenQty), DarkHorse::SmPriceType::Market);
	mainApp.Client()->NewOrder(order_req);
}


void DmAccountOrderCenterWindow::OnBnClickedCheckProfit()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnBnClickedCheckLoss()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnBnClickedRadioMarket()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnBnClickedRadioPrice()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnEnChangeEditProfit()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnEnChangeEditLoss()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnEnChangeEditSlip()
{
	UpdateOrderSettings();
}


void DmAccountOrderCenterWindow::OnBnClickedBtnSearch()
{
	symbol_table_dialog_ = std::make_shared<SmSymbolTableDialog>(this);
	symbol_table_dialog_->Create(IDD_SYMBOL_TABLE, this);
	//_SymbolTableDlg->OrderWnd = this;
	symbol_table_dialog_->ShowWindow(SW_SHOW);
}


void DmAccountOrderCenterWindow::OnBnClickedBtnSet()
{
	order_set_dialog_ = std::make_shared<SmOrderSetDialog>();
	order_set_dialog_->Create(IDD_ORDER_SET, this);
	//_OrderSetDlg->OrderWnd(this);
	order_set_dialog_->ShowWindow(SW_SHOW);
}


void DmAccountOrderCenterWindow::OnStnClickedStaticFilledRemain()
{
	// TODO: Add your control notification handler code here
}


void DmAccountOrderCenterWindow::OnBnClickedCheckFixHoga()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_FIX_HOGA))->GetCheck() == BST_CHECKED)
		symbol_order_view_.FixedMode(true);
	else
		symbol_order_view_.FixedMode(false);
}
