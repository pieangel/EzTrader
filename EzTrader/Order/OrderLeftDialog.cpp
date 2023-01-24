// OrderLeftDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "OrderLeftDialog.h"
#include "afxdialogex.h"
#include "../Symbol/SmSymbolTableDialog.h"

#define WND_ID1 0x00000001
#define WND_ID2 0x00000002
#define WND_ID3 0x00000003


// OrderLeftDialog dialog

IMPLEMENT_DYNAMIC(OrderLeftDialog, CBCGPDialog)

OrderLeftDialog::OrderLeftDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_LEFT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

OrderLeftDialog::~OrderLeftDialog()
{
	//KillTimer(1);
}

void OrderLeftDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_ACCEPTED, _AcceptedArea);
	//DDX_Control(pDX, IDC_STATIC_FILLED, _FilledArea);
	//DDX_Control(pDX, IDC_STATIC_FAVORITE, _FavoriteArea);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, _AccountArea);
	//DDX_Control(pDX, IDC_SCROLLBAR_VER_ACPT, _VScrollBarAcpt);
	//DDX_Control(pDX, IDC_SCROLLBAR_VER_POSI, _VScrollBarPosi);
	//DDX_Control(pDX, IDC_SCROLLBAR_VER_FAV, _VScrollBarFav);
	DDX_Control(pDX, IDC_BTN_ADD_FAV, _BtnAddFav);
	DDX_Control(pDX, IDC_STATIC_FAV, _StaticFav);
}


BEGIN_MESSAGE_MAP(OrderLeftDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_FAV, &OrderLeftDialog::OnBnClickedBtnAddFav)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &OrderLeftDialog::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CANCEL_SEL, &OrderLeftDialog::OnBnClickedBtnCancelSel)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, &OrderLeftDialog::OnBnClickedBtnCancelAll)
	ON_BN_CLICKED(IDC_BTN_LIQ_SEL, &OrderLeftDialog::OnBnClickedBtnLiqSel)
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &OrderLeftDialog::OnBnClickedBtnLiqAll)
END_MESSAGE_MAP()


// OrderLeftDialog message handlers


void OrderLeftDialog::SetMainWnd(SmMainOrderDialog* main_wnd)
{
	_AcceptedGrid.SetMainWnd(main_wnd);
	_PositionGrid.SetMainWnd(main_wnd);
	_FavoriteGrid.SetMainWnd(main_wnd);
}

BOOL OrderLeftDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	//_AcceptedArea.SetUp();
	//_FilledArea.SetUp();
	//_FavoriteArea.SetUp();
	_AccountArea.SetUp();
	



	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_ACCEPTED);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	_AcceptedGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID1);

	pWnd = GetDlgItem(IDC_STATIC_FILLED);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	_PositionGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID2);
	//_PositionGrid.ShowWindow(SW_HIDE);


	pWnd = GetDlgItem(IDC_STATIC_FAVORITE);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	_FavoriteGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID3);


	SetTimer(1, 100, NULL);

	/*
	_VScrollBarAcpt.ShowWindow(SW_SHOW);
	SCROLLINFO  scrinfo;
	scrinfo.cbSize = sizeof(scrinfo);
	scrinfo.fMask = SIF_ALL;
	scrinfo.nMin = 0;          // 최소값
	scrinfo.nMax = 200;      // 최대값
	scrinfo.nPage = _AcceptedArea.GetRowCount();      // 전체 스크롤바에서 스크롤박스가 차지하는 범위 
	scrinfo.nTrackPos = 0;  // 트랙바가 움직일때의 위치값
	scrinfo.nPos = 0;        // 위치
	_VScrollBarAcpt.SetScrollInfo(&scrinfo);


	_VScrollBarPosi.ShowWindow(SW_SHOW);
	
	scrinfo.cbSize = sizeof(scrinfo);
	scrinfo.fMask = SIF_ALL;
	scrinfo.nMin = 0;          // 최소값
	scrinfo.nMax = 200;      // 최대값
	scrinfo.nPage = _FilledArea.GetRowCount();      // 전체 스크롤바에서 스크롤박스가 차지하는 범위 
	scrinfo.nTrackPos = 0;  // 트랙바가 움직일때의 위치값
	scrinfo.nPos = 0;        // 위치
	_VScrollBarPosi.SetScrollInfo(&scrinfo);


	_VScrollBarFav.ShowWindow(SW_SHOW);

	scrinfo.cbSize = sizeof(scrinfo);
	scrinfo.fMask = SIF_ALL;
	scrinfo.nMin = 0;          // 최소값
	scrinfo.nMax = 200;      // 최대값
	scrinfo.nPage = _FavoriteArea.GetRowCount();      // 전체 스크롤바에서 스크롤박스가 차지하는 범위 
	scrinfo.nTrackPos = 0;  // 트랙바가 움직일때의 위치값
	scrinfo.nPos = 0;        // 위치
	_VScrollBarFav.SetScrollInfo(&scrinfo);
	*/


	_AcceptedGrid.UpdateAcceptedOrder();
	_PositionGrid.UpdatePositionInfo();

	//_AcceptedGrid.StartTimer();

	_FavoriteGrid.SetFavorite();

	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();

	//_FavoriteArea.SetFavorite();
	//_AcceptedArea.UpdateAcceptedOrder();
	//_FilledArea.UpdatePositionInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}



void OrderLeftDialog::OnBnClickedBtnAddFav()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	_SymbolTableDlg->OrderLeftWnd = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

void OrderLeftDialog::OnTimer(UINT_PTR nIDEvent)
{
	_AcceptedGrid.Update();
	_PositionGrid.Update();
	_FavoriteGrid.Update();
	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();
	//_AccountArea.UpdateAssetInfo();
	//_FavoriteGrid.SetFavorite();
}

void OrderLeftDialog::SetAccount(std::shared_ptr<DarkHorse::SmAccount> account)
{
	_AcceptedGrid.Account(account);
	_PositionGrid.Account(account);
	_AccountArea.Account(account);
	_AccountArea.UpdateAssetInfo();
	//_FilledArea.Account(account);
	//_AcceptedArea.Account(account);
}

LRESULT OrderLeftDialog::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	//_FavoriteGrid.AddSymbol(static_cast<int>(wParam));
	return 1;
}

void OrderLeftDialog::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();
}


void OrderLeftDialog::OnBnClickedBtnCancelSel()
{
	_AcceptedGrid.CancelSelOrders();
}


void OrderLeftDialog::OnBnClickedBtnCancelAll()
{
	_AcceptedGrid.CancelAll();
}


void OrderLeftDialog::OnBnClickedBtnLiqSel()
{
	_PositionGrid.LiqSelPositions();
}


void OrderLeftDialog::OnBnClickedBtnLiqAll()
{
	_PositionGrid.LiqAll();
}
