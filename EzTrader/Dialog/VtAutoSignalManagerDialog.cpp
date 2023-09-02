// VtAutoSignalManagerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "VtAutoSignalManagerDialog.h"
#include "afxdialogex.h"
#include "VtAddConnectSignalDlg.h"
#include "VtAddOutSigDefDlg.h"

#define WND_ID20 0x00000020
#define WND_ID21 0x00000021
#define WND_ID22 0x00000022
// VtAutoSignalManagerDialog dialog

IMPLEMENT_DYNAMIC(VtAutoSignalManagerDialog, CBCGPDialog)

VtAutoSignalManagerDialog::VtAutoSignalManagerDialog(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_SYS_AUTO_CONNECT, pParent)
{

}

VtAutoSignalManagerDialog::~VtAutoSignalManagerDialog()
{
}

void VtAutoSignalManagerDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ALL, _CheckAll);
	//DDX_Control(pDX, IDC_STATIC_ACTIVE_OUT_SYSTEM, active_out_system_view_);
	//DDX_Control(pDX, IDC_STATIC_OUT_SYSTEM, out_system_view_);
	//DDX_Control(pDX, IDC_STATIC_OUT_SYSTEM_DEF, out_system_def_view_);
}


void VtAutoSignalManagerDialog::PostNcDestroy()
{
	//delete this;
	CBCGPDialog::PostNcDestroy();
}

BEGIN_MESSAGE_MAP(VtAutoSignalManagerDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_CONNECT, &VtAutoSignalManagerDialog::OnBnClickedBtnAddConnect)
	ON_BN_CLICKED(IDC_BTN_ADD_SIGNAL, &VtAutoSignalManagerDialog::OnBnClickedBtnAddSignal)
	ON_BN_CLICKED(IDC_BTN_DEL_CONNECT, &VtAutoSignalManagerDialog::OnBnClickedBtnDelConnect)
	ON_BN_CLICKED(IDC_BTN_DEL_SIGNAL, &VtAutoSignalManagerDialog::OnBnClickedBtnDelSignal)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_ORDER_CONFIG, &VtAutoSignalManagerDialog::OnBnClickedBtnOrderConfig)
	ON_BN_CLICKED(IDC_CHECK_ALL, &VtAutoSignalManagerDialog::OnBnClickedCheckAll)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// VtAutoSignalManagerDialog message handlers


BOOL VtAutoSignalManagerDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_OUT_SYSTEM);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	// Create the Windows control and attach it to the Grid object
	out_system_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID20);

	pWnd = GetDlgItem(IDC_STATIC_ACTIVE_OUT_SYSTEM);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	// Create the Windows control and attach it to the Grid object
	active_out_system_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID21);

	pWnd = GetDlgItem(IDC_STATIC_OUT_SYSTEM_DEF);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	// Create the Windows control and attach it to the Grid object
	out_system_def_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID22);

	SetTimer(RefTimer, 100, NULL);
	Resize();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtAutoSignalManagerDialog::RefreshOrder()
{
// 	_TotalSigGrid.RefreshOrders();
// 	_ConnectGrid.RefreshOrders();
}

void VtAutoSignalManagerDialog::Resize()
{
	CRect rcDlg;
	this->GetClientRect(rcDlg);

	int horizontal_position = rcDlg.Height() / 2;
	int vertical_position = rcDlg.Width() * 77 / 96;
	int controlHeight = 32;

	// 위쪽 타이틀
	CRect rcCtrl;
	CWnd* pWnd = this->GetDlgItem(IDC_STATIC_RUN_LIST);
	if (!pWnd->GetSafeHwnd())
		return;

	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP + 7;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 주문설정 버튼
	pWnd = this->GetDlgItem(IDC_BTN_ORDER_CONFIG);

	int left = rcDlg.right - STD_GAP - 100;
	rcCtrl.left = left;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 포지션 청산 버튼
	pWnd = this->GetDlgItem(IDC_BUTTON_LIQ_POSITION);

	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 신호 합계 버튼
	pWnd = this->GetDlgItem(IDC_BUTTON_SUM_SIGNAL);


	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 상세표시 체크 버튼
	pWnd = this->GetDlgItem(IDC_CHECK_DETAIL);


	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP + 2;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 자동주문 체크 표시 버튼
	pWnd = this->GetDlgItem(IDC_CHECK_AUTO_ORDER);

	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP + 2;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 실행 목록 그리드
	pWnd = this->GetDlgItem(IDC_STATIC_ACTIVE_OUT_SYSTEM);


	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = rcDlg.right - STD_GAP;
	rcCtrl.top = rcDlg.top + STD_GAP + controlHeight;
	rcCtrl.bottom = horizontal_position - STD_GAP;
	pWnd->MoveWindow(rcCtrl, TRUE);
	active_out_system_view_.MoveWindow(rcCtrl, TRUE);

	// 분할 왼쪽 타이틀
	rcCtrl;
	pWnd = this->GetDlgItem(IDC_STATIC_CONNECT_SIGNAL_CHART);

	int top = horizontal_position + STD_GAP;
	rcCtrl.top = top + 7;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.right;

	// 모두 선택 버튼 
	pWnd = this->GetDlgItem(IDC_CHECK_ALL);
	rcCtrl.top = top + 2;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = left + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 왼쪽 삭제 버튼 
	pWnd = this->GetDlgItem(IDC_BTN_DEL_CONNECT);
	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;

	left = vertical_position - STD_GAP - 60;

	rcCtrl.left = left;
	rcCtrl.right = rcCtrl.left + 60;
	pWnd->MoveWindow(rcCtrl, TRUE);


	// 왼쪽 추가 버튼
	pWnd = this->GetDlgItem(IDC_BTN_ADD_CONNECT);

	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = left - STD_GAP - 60;
	rcCtrl.right = rcCtrl.left + 60;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 왼쪽 그리드 목록
	pWnd = this->GetDlgItem(IDC_STATIC_OUT_SYSTEM);


	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = vertical_position - STD_GAP;
	rcCtrl.top = horizontal_position + STD_GAP + controlHeight;
	rcCtrl.bottom = rcDlg.bottom - STD_GAP;
	pWnd->MoveWindow(rcCtrl, TRUE);
	out_system_view_.MoveWindow(rcCtrl, TRUE);




	// 오른쪽 타이틀
	pWnd = this->GetDlgItem(IDC_STATIC_SIGNAL_CHART);

	rcCtrl.top = top + 7;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = vertical_position + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 오른쪽 삭제 버튼
	pWnd = this->GetDlgItem(IDC_BTN_DEL_SIGNAL);

	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = rcDlg.right - STD_GAP - 60;
	rcCtrl.right = rcCtrl.left + 60;
	pWnd->MoveWindow(rcCtrl, TRUE);

	left = rcDlg.right - STD_GAP - 60;

	// 오른쪽 추가 버튼
	pWnd = this->GetDlgItem(IDC_BTN_ADD_SIGNAL);

	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = left - STD_GAP - 60;
	rcCtrl.right = rcCtrl.left + 60;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 오른쪽 그리드 목록
	pWnd = this->GetDlgItem(IDC_STATIC_OUT_SYSTEM_DEF);


	rcCtrl.left = vertical_position + STD_GAP;
	rcCtrl.right = rcDlg.right - STD_GAP;
	rcCtrl.top = horizontal_position + STD_GAP + controlHeight;
	rcCtrl.bottom = rcDlg.bottom - STD_GAP;
	pWnd->MoveWindow(rcCtrl, TRUE);
	out_system_def_view_.MoveWindow(rcCtrl, TRUE);

// 	_TotalSigGrid.GetScrollBarCtrl(SB_VERT)->Invalidate(TRUE);
// 	_ConnectGrid.GetScrollBarCtrl(SB_VERT)->Invalidate(TRUE);
// 	_DefineGrid.GetScrollBarCtrl(SB_VERT)->Invalidate(TRUE);

	Invalidate(TRUE);
}

void VtAutoSignalManagerDialog::OnBnClickedBtnAddConnect()
{
	VtAddConnectSignalDlg dlg;
	//dlg.SigConGrid(&_ConnectGrid);
	dlg.DoModal();
}


void VtAutoSignalManagerDialog::OnBnClickedBtnAddSignal()
{
	VtAddOutSigDefDlg dlg;
	//dlg.SigDefGrid(&_DefineGrid);
	dlg.DoModal();
}


void VtAutoSignalManagerDialog::OnBnClickedBtnDelConnect()
{
	//_ConnectGrid.RemoveSystem();
	//_ConnectGrid.Refresh();
}


void VtAutoSignalManagerDialog::OnBnClickedBtnDelSignal()
{
	// TODO: Add your control notification handler code here
}


void VtAutoSignalManagerDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == RefTimer) {
		RefreshOrder();
	}
	CBCGPDialog::OnTimer(nIDEvent);
}


void VtAutoSignalManagerDialog::OnClose()
{
	KillTimer(RefTimer);
	CBCGPDialog::OnClose();
}

void VtAutoSignalManagerDialog::OnBnClickedBtnOrderConfig()
{
// 	VtSystemOrderConfig orderCfgDlg;
// 	orderCfgDlg._PriceType = VtOutSystemManager::PriceType;
// 	orderCfgDlg._OrderTick = VtOutSystemManager::OrderTick;
// 	int result = orderCfgDlg.DoModal();
// 	if (result == IDOK) {
// 		VtOutSystemManager::PriceType = orderCfgDlg._PriceType;
// 		VtOutSystemManager::OrderTick = orderCfgDlg._OrderTick;
// 
// 		VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
// 		SharedSystemVec& outSysMap = outSysMgr->GetSysMap();
// 		for (auto it = outSysMap.begin(); it != outSysMap.end(); ++it) {
// 			SharedSystem sys = *it;
// 			sys->PriceType(orderCfgDlg._PriceType);
// 			sys->OrderTick(orderCfgDlg._OrderTick);
// 		}
// 	}
}


void VtAutoSignalManagerDialog::OnBnClickedCheckAll()
{
// 	if (_CheckAll.GetCheck() == BST_CHECKED) {
// 		_ConnectGrid.SetCheck(true);
// 	}
// 	else {
// 		_ConnectGrid.SetCheck(false);
// 	}
}


void VtAutoSignalManagerDialog::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	this->Resize();
}
