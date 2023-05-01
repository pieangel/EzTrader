// SmOrderWnd.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "AbAccountOrderCenterWindow.h"
#include "afxdialogex.h"
#include "../Grid/customcells.h"
#include "SmOrderGridConst.h"
#include "../Util/VtStringUtil.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "BCGPGridCtrl.h"
#include <format>
#include "../Log/MyLogger.h"
#include "../SmGrid/SmGrid.h"
#include "../SmGrid/SmCell.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "AbAccountOrderWindow.h"
#include "../Symbol/SmSymbolTableDialog.h"
#include "../Task/SmTaskRequestMaker.h"
#include "../Client/ViStockClient.h"
#include "OrderWndConst.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmAccountPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Order/SmOrderRequest.h"
#include "../Client/ViStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "SmOrderSetDialog.h"

// SmOrderWnd dialog
#define BTN_ORDER_AMOUNT 0x00000001
#define BTN_ORDER_SELL 0x00000002
#define BTN_ORDER_BUY 0x00000003
#define BTN_ORDER_CENTER_QUOTE 0x00000004

using namespace DarkHorse;

int AbAccountOrderCenterWindow::DeltaOrderArea = 0;

IMPLEMENT_DYNAMIC(AbAccountOrderCenterWindow, CBCGPDialog)

AbAccountOrderCenterWindow::AbAccountOrderCenterWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_CENTER, pParent)
{
	symbol_order_view_.set_order_request_type(OrderRequestType::Abroad);
	symbol_order_view_.set_fill_condition(SmFilledCondition::Day);
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

AbAccountOrderCenterWindow::~AbAccountOrderCenterWindow()
{
	//KillTimer(1);
}

void AbAccountOrderCenterWindow::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	_Account = val;
	symbol_order_view_.Account(val);
	symbol_order_view_.Refresh();
	symbol_position_view_.Account(val);
	symbol_position_view_.Refresh();
}

void AbAccountOrderCenterWindow::Selected(bool val)
{
	_Selected = val;
	symbol_order_view_.Selected(val);
}

void AbAccountOrderCenterWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbol);
	DDX_Control(pDX, IDC_STATIC_ORDER, symbol_order_view_);
	DDX_Control(pDX, IDC_STATIC_POSITION, symbol_position_view_);
	DDX_Control(pDX, IDC_STATIC_QUOTE, symbol_tick_view_);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_NAME_KR, _StaticSymbolName);
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
	DDX_Control(pDX, IDC_STATIC_FILLED_REMAIN, _FilledRemainButton);
	DDX_Control(pDX, IDC_STATIC_REMAIN, _RemainButton);
}


BEGIN_MESSAGE_MAP(AbAccountOrderCenterWindow, CBCGPDialog)
	ON_WM_TIMER()
	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &AbAccountOrderCenterWindow::OnCbnSelchangeComboSymbol)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_SHOW_REAL_QUOTE, &AbAccountOrderCenterWindow::OnBnClickedCheckShowRealQuote)
	ON_BN_CLICKED(IDC_BTN_REFRESH_ORDER, &AbAccountOrderCenterWindow::OnBnClickedBtnRefreshOrder)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_SYMBOL, &AbAccountOrderCenterWindow::OnBnClickedBtnSymbol)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &AbAccountOrderCenterWindow::OnUmSymbolSelected)
	ON_BN_CLICKED(IDC_BTN_AMOUNT1, &AbAccountOrderCenterWindow::OnBnClickedBtnAmount1)
	ON_BN_CLICKED(IDC_BTN_AMOUNT2, &AbAccountOrderCenterWindow::OnBnClickedBtnAmount2)

	ON_BN_CLICKED(IDC_BTN_AMOUNT5, &AbAccountOrderCenterWindow::OnBnClickedBtnAmount5)
	ON_BN_CLICKED(IDC_BTN_AMOUNT3, &AbAccountOrderCenterWindow::OnBnClickedBtnAmount3)
	ON_BN_CLICKED(IDC_BTN_AMOUNT4, &AbAccountOrderCenterWindow::OnBnClickedBtnAmount4)
	ON_EN_CHANGE(IDC_EDIT_AMOUNT, &AbAccountOrderCenterWindow::OnEnChangeEditAmount)
	ON_BN_CLICKED(IDC_BTN_LIQ_SYMBOL_POSITION, &AbAccountOrderCenterWindow::OnBnClickedBtnLiqSymbolPosition)
	ON_BN_CLICKED(IDC_CHECK_PROFIT, &AbAccountOrderCenterWindow::OnBnClickedCheckProfit)
	ON_BN_CLICKED(IDC_CHECK_LOSS, &AbAccountOrderCenterWindow::OnBnClickedCheckLoss)
	ON_BN_CLICKED(IDC_RADIO_MARKET, &AbAccountOrderCenterWindow::OnBnClickedRadioMarket)
	ON_BN_CLICKED(IDC_RADIO_PRICE, &AbAccountOrderCenterWindow::OnBnClickedRadioPrice)
	ON_EN_CHANGE(IDC_EDIT_PROFIT, &AbAccountOrderCenterWindow::OnEnChangeEditProfit)
	ON_EN_CHANGE(IDC_EDIT_LOSS, &AbAccountOrderCenterWindow::OnEnChangeEditLoss)
	ON_EN_CHANGE(IDC_EDIT_SLIP, &AbAccountOrderCenterWindow::OnEnChangeEditSlip)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &AbAccountOrderCenterWindow::OnBnClickedBtnSearch)
	ON_BN_CLICKED(IDC_BTN_SET, &AbAccountOrderCenterWindow::OnBnClickedBtnSet)
	ON_STN_CLICKED(IDC_STATIC_FILLED_REMAIN, &AbAccountOrderCenterWindow::OnStnClickedStaticFilledRemain)
	ON_BN_CLICKED(IDC_CHECK_FIX_HOGA, &AbAccountOrderCenterWindow::OnBnClickedCheckFixHoga)
END_MESSAGE_MAP()


// SmOrderWnd message handlers

#define WS_CAPTION          0x00C00000L

LRESULT AbAccountOrderCenterWindow::OnEnterSizeMove(WPARAM wparam, LPARAM lparam)
{
	_Resizing = true;
	// do stuff      
	return (LRESULT)0;
}
LRESULT AbAccountOrderCenterWindow::OnExitSizeMove(WPARAM wparam, LPARAM lparam)
{
	
	_Resizing = false;
	// do stuff      
	return (LRESULT)0;
}




void AbAccountOrderCenterWindow::SetMainDialog(AbAccountOrderWindow* main_dialog)
{
	//_OrderArea.SetMainDialog(main_dialog);
}

void AbAccountOrderCenterWindow::SetFundDialog(SmFundOrderDialog* fund_dialog)
{

}

void AbAccountOrderCenterWindow::SetSelected(const bool& selected)
{
	_Selected = selected;
	symbol_order_view_.Selected(selected);
	symbol_order_view_.Invalidate();
}

void AbAccountOrderCenterWindow::OnSymbolClicked(const std::string& symbol_code)
{
	auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (symbol) SetSymbolInfo(symbol);
}

void AbAccountOrderCenterWindow::SetOrderAmount(const int& count)
{
	CString strValue;
	strValue.Format("%d", count);
	_EditAmount.SetWindowText(strValue);
}

int AbAccountOrderCenterWindow::GetPositionCount()
{
	if (!_Account || !_Symbol) return 0;

	std::shared_ptr<SmPosition> position = mainApp.TotalPosiMgr()->FindAddPosition(_Account->No(), _Symbol->SymbolCode());
	
	return position->OpenQty;
}

void AbAccountOrderCenterWindow::ArrangeCenterValue()
{
	symbol_order_view_.ArrangeCenterValue();
}

void AbAccountOrderCenterWindow::CreateResource()
{
	
}

BOOL AbAccountOrderCenterWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	

	//SetTimer(1, 40, NULL);
	_ComboSymbol.SetDroppedWidth(250);
	//_ComboAccount.SetDroppedWidth(200);

	
	CRect rect;
	//CWnd* pWnd = GetDlgItem(IDC_STATIC_SET);
	//pWnd->GetWindowRect(&rect);
	//ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	//_OrderSetGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, IDC_STATIC_SET);
	


	symbol_order_view_.SetUp();
	symbol_tick_view_.SetUp();
	symbol_position_view_.SetUp();

	_CheckShowRealTick.SetCheck(BST_CHECKED);
	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	if (pLayout != NULL)
	{
		pLayout->AddAnchor(IDC_STATIC_POSITION, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeNone);
		pLayout->AddAnchor(IDC_STATIC_ORDER, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeVert);
		pLayout->AddAnchor(IDC_STATIC_QUOTE, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeNone);
	}

	

	_StaticAccountName.m_clrText = RGB(255, 255, 255);
	_StaticSymbolName.m_clrText = RGB(255, 255, 255);

	CRect rcWnd;
	symbol_order_view_.GetWindowRect(&rcWnd);
	ScreenToClient(rcWnd);

	rcWnd.bottom = rcWnd.top + rcWnd.Height() + AbAccountOrderCenterWindow::DeltaOrderArea;

	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("1");

	((CEdit*)GetDlgItem(IDC_EDIT_PROFIT))->SetWindowText("2");
	((CEdit*)GetDlgItem(IDC_EDIT_LOSS))->SetWindowText("2");
	((CEdit*)GetDlgItem(IDC_EDIT_SLIP))->SetWindowText("2");

	_Init = true;

	_ID = AbAccountOrderWindow::GetId();

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

	_FilledRemainButton.SetUp();
	_FilledRemainButton.Text("√º∞·µ» ¿‹∞Ì");
	_FilledRemainButton.ShowWindow(SW_HIDE);

	_RemainButton.SetUp();
	_RemainButton.Text("¿‹∞Ì");
	_RemainButton.OrderCenterWnd(this);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void AbAccountOrderCenterWindow::SetQuote(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	

	const int& close = symbol->Qoute.close;
	const int int_tick_size = static_cast<int>(symbol->TickSize() * std::pow(10, symbol->Decimal()));
	const int start_value = close + (_CloseRow - _ValueStartRow) * int_tick_size;
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

void AbAccountOrderCenterWindow::SetHoga(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{

}

void AbAccountOrderCenterWindow::OnClickSymbol(const std::string& symbol_info)
{
	
}

void AbAccountOrderCenterWindow::OnTimer(UINT_PTR nIDEvent)
{
	//if (!_Resizing) _OrderArea.Refresh();

	//_PositionArea.UpdatePositionInfo();
	//_PositionArea.UpdateSymbolInfo();
	//_QuoteArea.UpdateSymbolInfo();

	CTime t1;
	t1 = CTime::GetCurrentTime();
	//m_bar.SetPaneText(1, t1.Format("%H:%M:%S"));
	if (_Account && _Symbol) {
		auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());
		int filled_count = symbol_order_mgr->GetUnsettledCount();
		CString strCount;
		strCount.Format("%d", filled_count);
		_StaticFilledCount.SetWindowText(strCount);
		//_StaticFilledCount.Invalidate();
	}
}


void AbAccountOrderCenterWindow::SetSymbolInfo(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	_Symbol = symbol;

	std::string symbol_info(symbol->SymbolNameKr());
	symbol_info.append(" [");
	symbol_info.append(symbol->SymbolCode());
	symbol_info.append("]");

	int result = _ComboSymbol.SelectString(-1, symbol_info.c_str());
	if (result == CB_ERR) {
		_CurrentIndex = _ComboSymbol.AddString(symbol_info.c_str());
		_IndexToSymbolMap[_CurrentIndex] = _Symbol;
		_ComboSymbol.SetCurSel(_CurrentIndex);
		mainApp.SymMgr()->RegisterSymbolToServer(_Symbol->SymbolCode(), true);
	}
	else {
		_CurrentIndex = result;

		auto found = _IndexToSymbolMap.find(result);

		if (found == _IndexToSymbolMap.end()) return;

		_Symbol = _IndexToSymbolMap[_CurrentIndex];
	}

	_StaticSymbolName.SetWindowText(symbol_info.c_str());

	SetInfo(_Symbol);
}

void AbAccountOrderCenterWindow::UpdateOrderSettings()
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

void AbAccountOrderCenterWindow::SetCutMode()
{
	_CutMode = 0;
	if (_CheckLoss.GetCheck() == BST_CHECKED) {
		_CutMode = 2;
		if (_CheckProfit.GetCheck() == BST_CHECKED) {
			_CutMode = 3;
		}
	}
	else if (_CheckProfit.GetCheck() == BST_CHECKED) {
		_CutMode = 1;
		if (_CheckLoss.GetCheck() == BST_CHECKED) {
			_CutMode = 3;
		}
	}
	symbol_order_view_.CutMode(_CutMode);
}

void AbAccountOrderCenterWindow::SetInfo(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	symbol_tick_view_.Clear();
	symbol_position_view_.Clear();
	symbol_order_view_.Clear();
	symbol_tick_view_.Symbol(symbol);
	//_OrderArea.SetCenterValues(symbol);
	symbol_order_view_.Symbol(symbol);
	//_OrderArea.SetQuote(symbol);
	//_OrderArea.SetHoga(symbol);
	symbol_position_view_.Symbol(symbol);
	//_OrderArea.UpdateOrder(_Symbol->SymbolCode());
	//_OrderArea.Refresh();
}

void AbAccountOrderCenterWindow::SetRowWide()
{
	symbol_order_view_.SetAllRowHeight(WideRowHeight);
	RecalcOrderAreaHeight(this);
	symbol_order_view_.Invalidate();
}

void AbAccountOrderCenterWindow::SetRowNarrow()
{
	symbol_order_view_.SetAllRowHeight(DefaultRowHeight);
	RecalcOrderAreaHeight(this);
	symbol_order_view_.Invalidate();
}

void AbAccountOrderCenterWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	symbol_order_view_.OnOrderChanged(account_id, symbol_id);
	symbol_position_view_.OnOrderChanged(account_id, symbol_id);
}

int AbAccountOrderCenterWindow::RecalcOrderAreaHeight(CWnd* wnd, bool bottom_up)
{
	CRect rcTopMost;
	CRect rcOrderArea;
	CRect rcOrderWnd;
	symbol_order_view_.GetWindowRect(rcOrderArea);

	GetWindowRect(rcOrderWnd);

	wnd->GetWindowRect(rcTopMost);

	CRect rcSymbol;
	_StaticSymbolName.GetWindowRect(rcSymbol);

	int y_del = 0, delta_height = 0;
	y_del = rcTopMost.bottom - rcOrderArea.top;
	y_del -= 8;

	delta_height = symbol_order_view_.RecalRowCount(y_del);

	if (delta_height > 0) {
		CRect rcParent;
		CRect rect;
		symbol_order_view_.GetWindowRect(rect);

		rect.right -= 2;
		rect.bottom -= delta_height;
		//_OrderArea.SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);

		GetWindowRect(rect);
		//ScreenToClient(rect);
		rect.bottom -= delta_height;
		//MoveWindow(rect);
		//SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);
	}
	

	return delta_height;
}

void AbAccountOrderCenterWindow::OnCbnSelchangeComboSymbol()
{
	const int cur_sel = _ComboSymbol.GetCurSel();
	if (cur_sel < 0) return;

	auto found = _IndexToSymbolMap.find(cur_sel);

	if (found == _IndexToSymbolMap.end()) return;

	_CurrentIndex = cur_sel;

	_Symbol = _IndexToSymbolMap[_CurrentIndex];


	std::string symbol_info(_Symbol->SymbolNameKr());
	symbol_info.append(" [");
	symbol_info.append(_Symbol->SymbolCode());
	symbol_info.append("]");

	_StaticSymbolName.SetWindowText(symbol_info.c_str());

	SetInfo(_Symbol);
}


void AbAccountOrderCenterWindow::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);
	
	if (_Init && symbol_order_view_.GetSafeHwnd()) {
		CRect rcWnd;
		GetWindowRect(rcWnd);
		//_OrderArea.RecalRowCount(rcWnd.Height());
		symbol_order_view_.Invalidate();
	}
	
}


void AbAccountOrderCenterWindow::OnBnClickedCheckShowRealQuote()
{
	_ShowQuoteArea ? _ShowQuoteArea = false : _ShowQuoteArea = true;
	CWnd* parent = GetParent();
	CRect rcQuote;
	symbol_tick_view_.GetWindowRect(rcQuote);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	parent->ScreenToClient(rcWnd);
	if (_ShowQuoteArea) {
		rcWnd.right = rcWnd.left + rcWnd.Width() + rcQuote.Width();
		symbol_tick_view_.ShowWindow(SW_SHOW);
	}
	else {
		rcWnd.right = rcWnd.left + rcWnd.Width() - rcQuote.Width();
		symbol_tick_view_.ShowWindow(SW_HIDE);
	}

	MoveWindow(rcWnd);

	((AbAccountOrderWindow*)parent)->RecalcChildren(_ShowQuoteArea ? CM_SHOW_TICK : CM_HIDE_TICK);
}


void AbAccountOrderCenterWindow::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
}


void AbAccountOrderCenterWindow::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
}


void AbAccountOrderCenterWindow::OnBnClickedBtnRefreshOrder()
{
	//_OrderArea.UpdateOrder(_Symbol->SymbolCode());
	//_OrderArea.Invalidate(FALSE);
	mainApp.Client()->GetFilledOrderList(_Account->No(), _Account->Pwd());
}


BOOL AbAccountOrderCenterWindow::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return FALSE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void AbAccountOrderCenterWindow::OnBnClickedBtnSymbol()
{
	
}

LRESULT AbAccountOrderCenterWindow::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = (int)wParam;
	std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	if (symbol) SetSymbolInfo(symbol);
	return 1;
}


BOOL AbAccountOrderCenterWindow::PreTranslateMessage(MSG* pMsg)
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


void AbAccountOrderCenterWindow::OnBnClickedBtnAmount1()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("1");
}


void AbAccountOrderCenterWindow::OnBnClickedBtnAmount2()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("2");
}


void AbAccountOrderCenterWindow::OnBnClickedBtnAmount5()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("5");
}


void AbAccountOrderCenterWindow::OnBnClickedBtnAmount3()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("3");
}


void AbAccountOrderCenterWindow::OnBnClickedBtnAmount4()
{
	((CEdit*)GetDlgItem(IDC_EDIT_AMOUNT))->SetWindowText("4");
}


void AbAccountOrderCenterWindow::OnEnChangeEditAmount()
{
	CEdit* amount = (CEdit*)GetDlgItem(IDC_EDIT_AMOUNT);
	if (!amount) return;

	CString strValue;
	amount->GetWindowText(strValue);
	symbol_order_view_.OrderAmount(_ttoi(strValue));
}





void AbAccountOrderCenterWindow::OnBnClickedBtnLiqSymbolPosition()
{
	if (!_Account || !_Symbol) return;

	auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(_Account->No());
	const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();
	auto found = account_pos_map.find(_Symbol->SymbolCode());
	if (found == account_pos_map.end()) return;

	std::shared_ptr<SmOrderRequest> order_req = nullptr;
	if (found->second->Position == SmPositionType::Buy)
		order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(_Account->No(), _Account->Pwd(), _Symbol->SymbolCode(), 0, abs(found->second->OpenQty), DarkHorse::SmPriceType::Market);
	else
		order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(_Account->No(), _Account->Pwd(), _Symbol->SymbolCode(), 0, abs(found->second->OpenQty), DarkHorse::SmPriceType::Market);
	mainApp.Client()->NewOrder(order_req);
}


void AbAccountOrderCenterWindow::OnBnClickedCheckProfit()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnBnClickedCheckLoss()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnBnClickedRadioMarket()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnBnClickedRadioPrice()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnEnChangeEditProfit()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnEnChangeEditLoss()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnEnChangeEditSlip()
{
	UpdateOrderSettings();
}


void AbAccountOrderCenterWindow::OnBnClickedBtnSearch()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	_SymbolTableDlg->OrderWnd = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}


void AbAccountOrderCenterWindow::OnBnClickedBtnSet()
{
	_OrderSetDlg = std::make_shared<SmOrderSetDialog>();
	_OrderSetDlg->Create(IDD_ORDER_SET, this);
	_OrderSetDlg->OrderWnd(this);
	_OrderSetDlg->ShowWindow(SW_SHOW);
}


void AbAccountOrderCenterWindow::OnStnClickedStaticFilledRemain()
{
	// TODO: Add your control notification handler code here
}


void AbAccountOrderCenterWindow::OnBnClickedCheckFixHoga()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_FIX_HOGA))->GetCheck() == BST_CHECKED)
		symbol_order_view_.FixedMode(true);
	else
		symbol_order_view_.FixedMode(false);
}
