//#include "stdafx.h"
//#include "DmFundOrderLeftWindow.h"

// SmFundOrderLeft.cpp : implementation file
//

#include "stdafx.h"
#include "../../DarkHorse.h"
#include "DmFundOrderLeftWindow.h"
#include "afxdialogex.h"
#include "../../Symbol/SmSymbolTableDialog.h"

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
	KillTimer(1);
}

void DmFundOrderLeftWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_ACCEPTED, _AcceptedArea);
	//DDX_Control(pDX, IDC_STATIC_FILLED, _FilledArea);
	//DDX_Control(pDX, IDC_STATIC_FAVORITE, _FavoriteArea);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, account_profit_loss_view_);
	DDX_Control(pDX, IDC_BTN_ADD_FAV, _BtnAddFav);
	DDX_Control(pDX, IDC_STATIC_FAV, _StaticFav);
}


BEGIN_MESSAGE_MAP(DmFundOrderLeftWindow, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_FAV, &DmFundOrderLeftWindow::OnBnClickedBtnAddFav)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &DmFundOrderLeftWindow::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CANCEL_SEL, &DmFundOrderLeftWindow::OnBnClickedBtnCancelSel)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, &DmFundOrderLeftWindow::OnBnClickedBtnCancelAll)
	ON_BN_CLICKED(IDC_BTN_LIQ_SEL, &DmFundOrderLeftWindow::OnBnClickedBtnLiqSel)
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &DmFundOrderLeftWindow::OnBnClickedBtnLiqAll)
END_MESSAGE_MAP()


// DmFundOrderLeftWindow message handlers


void DmFundOrderLeftWindow::SetFundOrderWnd(SmFundOrderDialog* fund_order_wnd)
{
	//_AcceptedGrid.SetFundOrderWnd(fund_order_wnd);
	//_PositionGrid.SetFundOrderWnd(fund_order_wnd);
	//_FavoriteGrid.SetFundOrderWnd(fund_order_wnd);
}

BOOL DmFundOrderLeftWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	//_AcceptedArea.SetUp();
	//_FilledArea.SetUp();
	//_FavoriteArea.SetUp();
	account_profit_loss_view_.SetUp();
	account_profit_loss_view_.Mode(1);




	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_ACCEPTED);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	SetTimer(1, 100, NULL);

	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();

	//_AcceptedGrid.StartTimer();

	//_FavoriteGrid.SetFavorite();

	//_AcceptedGrid.StartTimer();
	//_PositionGrid.StartTimer();
	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void DmFundOrderLeftWindow::OnBnClickedBtnAddFav()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	//_SymbolTableDlg->FundOrderLeft = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

void DmFundOrderLeftWindow::OnTimer(UINT_PTR nIDEvent)
{
	//_AcceptedGrid.Update();
	//_PositionGrid.Update();
	//_FavoriteGrid.Update();
	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();
	//_AccountArea.UpdateAssetInfo();
	//_FavoriteGrid.SetFavorite();
}

void DmFundOrderLeftWindow::SetAccount(std::shared_ptr<DarkHorse::SmAccount> account)
{
	//_AcceptedGrid.Account(account);
	//_PositionGrid.Account(account);
	account_profit_loss_view_.Account(account);
	account_profit_loss_view_.UpdateAssetInfo();
}

void DmFundOrderLeftWindow::SetFund(std::shared_ptr < DarkHorse::SmFund> fund)
{
	//_AcceptedGrid.Fund(fund);
	//_PositionGrid.Fund(fund);
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
