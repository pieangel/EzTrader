#include "stdafx.h"
#include "DmFundOrderLeftWindow.h"

// OrderLeftDialog.cpp : implementation file
//

//#include "stdafx.h"
#include "../../DarkHorse.h"
//#include "OrderLeftDialog.h"
#include "afxdialogex.h"
#include "../../Symbol/SmSymbolTableDialog.h"
#include "../../Global/SmTotalManager.h"
#include "../../Symbol/SmSymbolManager.h"
#include "../../Symbol/SmProduct.h"
#include "../../Symbol/SmProductYearMonth.h"
#include "../../Symbol/SmSymbol.h"
#include "../../View/AccountAssetView.h"

#define WND_ID1 0x00000001
#define WND_ID2 0x00000002
#define WND_ID3 0x00000003


// DmFundOrderLeftWindow dialog

IMPLEMENT_DYNAMIC(DmFundOrderLeftWindow, CBCGPDialog)

DmFundOrderLeftWindow::DmFundOrderLeftWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_LEFT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

DmFundOrderLeftWindow::~DmFundOrderLeftWindow()
{
	//KillTimer(1);
}

void DmFundOrderLeftWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT_PROFIT_LOSS, account_profit_loss_view_);
	DDX_Control(pDX, IDC_STATIC_ASSET, asset_view_);
	DDX_Control(pDX, IDC_STATIC_FUTURE, future_view_);
	DDX_Control(pDX, IDC_STATIC_OPTION, option_view_);
	DDX_Control(pDX, IDC_COMBO_OPTION_MARKET, combo_option_market_);
	DDX_Control(pDX, IDC_COMBO_OPTION_MONTH, combo_option_month_);
	DDX_Control(pDX, IDC_RADIO_CLOSE, radio_close_);
	DDX_Control(pDX, IDC_RADIO_POSITION, radio_position_);
	DDX_Control(pDX, IDC_RADIO_EXPECTED, ratio_expected_);
}

void DmFundOrderLeftWindow::set_option_view()
{
	const std::string year_month_name = option_yearmonth_index_map[year_month_index];
	option_view_.set_option_view(option_market_index, year_month_name);
}

void DmFundOrderLeftWindow::init_option_market()
{
	std::vector<DarkHorse::DmOption>& option_vec = mainApp.SymMgr()->get_dm_option_vec();
	for (size_t i = 0; i < option_vec.size(); i++) {
		int index = combo_option_market_.AddString(option_vec[i].option_name.c_str());
	}
	option_market_index = 0;
	combo_option_market_.SetCurSel(option_market_index);
	const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map = option_vec[option_market_index].call_product->get_yearmonth_map();
	for (auto it = year_month_map.begin(); it != year_month_map.end(); it++) {
		const std::string& name = it->second->get_name();
		int index = combo_option_month_.AddString(name.c_str());
		option_yearmonth_index_map[index] = name;
	}
	year_month_index = 0;
	combo_option_month_.SetCurSel(year_month_index);
}

BEGIN_MESSAGE_MAP(DmFundOrderLeftWindow, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_FAV, &DmFundOrderLeftWindow::OnBnClickedBtnAddFav)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &DmFundOrderLeftWindow::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CANCEL_SEL, &DmFundOrderLeftWindow::OnBnClickedBtnCancelSel)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, &DmFundOrderLeftWindow::OnBnClickedBtnCancelAll)
	ON_BN_CLICKED(IDC_BTN_LIQ_SEL, &DmFundOrderLeftWindow::OnBnClickedBtnLiqSel)
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &DmFundOrderLeftWindow::OnBnClickedBtnLiqAll)
	ON_CBN_SELCHANGE(IDC_COMBO_OPTION_MARKET, &DmFundOrderLeftWindow::OnCbnSelchangeComboOptionMarket)
	ON_CBN_SELCHANGE(IDC_COMBO_OPTION_MONTH, &DmFundOrderLeftWindow::OnCbnSelchangeComboOptionMonth)
	ON_BN_CLICKED(IDC_RADIO_CLOSE, &DmFundOrderLeftWindow::OnBnClickedRadioClose)
	ON_BN_CLICKED(IDC_RADIO_POSITION, &DmFundOrderLeftWindow::OnBnClickedRadioPosition)
	ON_BN_CLICKED(IDC_RADIO_EXPECTED, &DmFundOrderLeftWindow::OnBnClickedRadioExpected)
END_MESSAGE_MAP()


// DmFundOrderLeftWindow message handlers


void DmFundOrderLeftWindow::SetMainWnd(DmFundOrderWindow* main_wnd)
{
	//_AcceptedGrid.SetMainWnd(main_wnd);
	//_PositionGrid.SetMainWnd(main_wnd);
	//_FavoriteGrid.SetMainWnd(main_wnd);
}

BOOL DmFundOrderLeftWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	account_profit_loss_view_.SetUp();
	asset_view_.SetUp();
	option_view_.SetUp();
	future_view_.SetUp();
	future_view_.init_dm_future();
	init_option_market();
	set_option_view();

	radio_close_.SetCheck(BST_CHECKED);
	SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void DmFundOrderLeftWindow::OnBnClickedBtnAddFav()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	//_SymbolTableDlg->OrderLeftWnd = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

void DmFundOrderLeftWindow::OnTimer(UINT_PTR nIDEvent)
{

}

void DmFundOrderLeftWindow::SetAccount(std::shared_ptr<DarkHorse::SmAccount> account)
{
	option_view_.Account(account);
	future_view_.Account(account);
	asset_view_.Account(account);
	account_profit_loss_view_.Account(account);
	account_profit_loss_view_.UpdateAssetInfo();
}

void DmFundOrderLeftWindow::SetFund(std::shared_ptr<DarkHorse::SmFund> fund)
{
	asset_view_.Fund(fund);
	account_profit_loss_view_.Fund(fund);
	account_profit_loss_view_.UpdateAssetInfo();
}

LRESULT DmFundOrderLeftWindow::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	//_FavoriteGrid.AddSymbol(static_cast<int>(wParam));
	return 1;
}

void DmFundOrderLeftWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();
}


void DmFundOrderLeftWindow::OnBnClickedBtnCancelSel()
{
	//_AcceptedGrid.CancelSelOrders();
}


void DmFundOrderLeftWindow::OnBnClickedBtnCancelAll()
{
	//_AcceptedGrid.CancelAll();
}


void DmFundOrderLeftWindow::OnBnClickedBtnLiqSel()
{
	//_PositionGrid.LiqSelPositions();
}


void DmFundOrderLeftWindow::OnBnClickedBtnLiqAll()
{
	//_PositionGrid.LiqAll();
}


void DmFundOrderLeftWindow::OnCbnSelchangeComboOptionMarket()
{
	option_market_index = combo_option_market_.GetCurSel();
	if (option_market_index < 0) return;

	combo_option_month_.ResetContent();
	option_yearmonth_index_map.clear();
	std::vector<DarkHorse::DmOption>& option_vec = mainApp.SymMgr()->get_dm_option_vec();
	const std::map<std::string, std::shared_ptr<DarkHorse::SmProductYearMonth>>& year_month_map = option_vec[option_market_index].call_product->get_yearmonth_map();
	for (auto it = year_month_map.begin(); it != year_month_map.end(); it++) {
		const std::string& name = it->second->get_name();
		int index = combo_option_month_.AddString(name.c_str());
		option_yearmonth_index_map[index] = name;
	}
	year_month_index = 0;
	combo_option_month_.SetCurSel(year_month_index);
	set_option_view();
}


void DmFundOrderLeftWindow::OnCbnSelchangeComboOptionMonth()
{
	if (option_market_index < 0) return;

	year_month_index = combo_option_month_.GetCurSel();
	set_option_view();
}


void DmFundOrderLeftWindow::OnBnClickedRadioClose()
{
	future_view_.set_view_mode(ViewMode::VM_Close);
	option_view_.set_view_mode(ViewMode::VM_Close);
}


void DmFundOrderLeftWindow::OnBnClickedRadioPosition()
{
	future_view_.set_view_mode(ViewMode::VM_Position);
	option_view_.set_view_mode(ViewMode::VM_Position);
}


void DmFundOrderLeftWindow::OnBnClickedRadioExpected()
{
	future_view_.set_view_mode(ViewMode::VM_Expected);
	option_view_.set_view_mode(ViewMode::VM_Expected);
}
