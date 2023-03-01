#include "stdafx.h"
#include "DmAccountOrderLeftWindow.h"

// OrderLeftDialog.cpp : implementation file
//

//#include "stdafx.h"
#include "../../DarkHorse.h"
//#include "OrderLeftDialog.h"
#include "afxdialogex.h"
#include "../../Symbol/SmSymbolTableDialog.h"

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


BEGIN_MESSAGE_MAP(DmAccountOrderLeftWindow, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_FAV, &DmAccountOrderLeftWindow::OnBnClickedBtnAddFav)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &DmAccountOrderLeftWindow::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CANCEL_SEL, &DmAccountOrderLeftWindow::OnBnClickedBtnCancelSel)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, &DmAccountOrderLeftWindow::OnBnClickedBtnCancelAll)
	ON_BN_CLICKED(IDC_BTN_LIQ_SEL, &DmAccountOrderLeftWindow::OnBnClickedBtnLiqSel)
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &DmAccountOrderLeftWindow::OnBnClickedBtnLiqAll)
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
