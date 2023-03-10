#include "stdafx.h"
#include "DmAccountOrderLeftWindow.h"

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

#define WND_ID1 0x00000001
#define WND_ID2 0x00000002
#define WND_ID3 0x00000003


// DmAccountOrderLeftWindow dialog

IMPLEMENT_DYNAMIC(DmAccountOrderLeftWindow, CBCGPDialog)

DmAccountOrderLeftWindow::DmAccountOrderLeftWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_LEFT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

DmAccountOrderLeftWindow::~DmAccountOrderLeftWindow()
{
	//KillTimer(1);
}

void DmAccountOrderLeftWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT_PROFIT_LOSS, account_profit_loss_view_);
	DDX_Control(pDX, IDC_STATIC_ASSET, asset_view_);
	DDX_Control(pDX, IDC_STATIC_FUTURE, future_view_);
	DDX_Control(pDX, IDC_STATIC_OPTION, option_view_);
	DDX_Control(pDX, IDC_COMBO_OPTION_MARKET, combo_option_market_);
	DDX_Control(pDX, IDC_COMBO_OPTION_MONTH, combo_option_month_);
}

void DmAccountOrderLeftWindow::set_option_view()
{
	const std::string year_month_name = option_yearmonth_index_map[year_month_index];
	option_view_.set_option_view(option_market_index, year_month_name);
}

void DmAccountOrderLeftWindow::init_option_market()
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

BEGIN_MESSAGE_MAP(DmAccountOrderLeftWindow, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_FAV, &DmAccountOrderLeftWindow::OnBnClickedBtnAddFav)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &DmAccountOrderLeftWindow::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CANCEL_SEL, &DmAccountOrderLeftWindow::OnBnClickedBtnCancelSel)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, &DmAccountOrderLeftWindow::OnBnClickedBtnCancelAll)
	ON_BN_CLICKED(IDC_BTN_LIQ_SEL, &DmAccountOrderLeftWindow::OnBnClickedBtnLiqSel)
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &DmAccountOrderLeftWindow::OnBnClickedBtnLiqAll)
	ON_CBN_SELCHANGE(IDC_COMBO_OPTION_MARKET, &DmAccountOrderLeftWindow::OnCbnSelchangeComboOptionMarket)
	ON_CBN_SELCHANGE(IDC_COMBO_OPTION_MONTH, &DmAccountOrderLeftWindow::OnCbnSelchangeComboOptionMonth)
END_MESSAGE_MAP()


// DmAccountOrderLeftWindow message handlers


void DmAccountOrderLeftWindow::SetMainWnd(DmAccountOrderWindow* main_wnd)
{
	//_AcceptedGrid.SetMainWnd(main_wnd);
	//_PositionGrid.SetMainWnd(main_wnd);
	//_FavoriteGrid.SetMainWnd(main_wnd);
}

BOOL DmAccountOrderLeftWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	account_profit_loss_view_.SetUp();
	asset_view_.SetUp();
	option_view_.SetUp();
	future_view_.SetUp();
	future_view_.init_dm_future();
	init_option_market();
	set_option_view();

	SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void DmAccountOrderLeftWindow::OnBnClickedBtnAddFav()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	//_SymbolTableDlg->OrderLeftWnd = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

void DmAccountOrderLeftWindow::OnTimer(UINT_PTR nIDEvent)
{
	//_AcceptedGrid.Update();
	//_PositionGrid.Update();
	//_FavoriteGrid.Update();
	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();
	//_AccountArea.UpdateAssetInfo();
	//_FavoriteGrid.SetFavorite();
}

void DmAccountOrderLeftWindow::SetAccount(std::shared_ptr<DarkHorse::SmAccount> account)
{
	//_AcceptedGrid.Account(account);
	//_PositionGrid.Account(account);
	account_profit_loss_view_.Account(account);
	account_profit_loss_view_.UpdateAssetInfo();
	//_FilledArea.Account(account);
	//_AcceptedArea.Account(account);
}

LRESULT DmAccountOrderLeftWindow::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	//_FavoriteGrid.AddSymbol(static_cast<int>(wParam));
	return 1;
}

void DmAccountOrderLeftWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();
}


void DmAccountOrderLeftWindow::OnBnClickedBtnCancelSel()
{
	//_AcceptedGrid.CancelSelOrders();
}


void DmAccountOrderLeftWindow::OnBnClickedBtnCancelAll()
{
	//_AcceptedGrid.CancelAll();
}


void DmAccountOrderLeftWindow::OnBnClickedBtnLiqSel()
{
	//_PositionGrid.LiqSelPositions();
}


void DmAccountOrderLeftWindow::OnBnClickedBtnLiqAll()
{
	//_PositionGrid.LiqAll();
}


void DmAccountOrderLeftWindow::OnCbnSelchangeComboOptionMarket()
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


void DmAccountOrderLeftWindow::OnCbnSelchangeComboOptionMonth()
{
	if (option_market_index < 0) return;

	year_month_index = combo_option_month_.GetCurSel();
	set_option_view();
}
