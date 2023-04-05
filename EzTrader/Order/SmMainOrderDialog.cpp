// SmMainOrderDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmMainOrderDialog.h"
#include "afxdialogex.h"
#include "OrderLeftDialog.h"
#include "OrderRightDialog.h"
#include "SmOrderWnd.h"
#include <set>
#include "../MainFrm.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccountManager.h"
#include "../Client/ViStockClient.h"
#include "../MessageDefine.h"
#include "../Event/SmCallbackManager.h"

#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmAccountPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Order/SmOrderRequest.h"
#include "../Client/ViStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Order/SmTotalOrderManager.h"

#include "../Symbol/SmSymbol.h"

using namespace DarkHorse;

const int CtrlHeight = 32;

int SmMainOrderDialog::_Id = 0;

void SmMainOrderDialog::SetAccount()
{
	const std::unordered_map<std::string, std::shared_ptr<DarkHorse::SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
	for (auto it = account_map.begin(); it != account_map.end(); ++it) {
		auto account = it->second;
		std::string account_info;
		account_info.append(account->Name());
		account_info.append(" : ");
		account_info.append(account->No());
		const int index = _ComboAccount.AddString(account_info.c_str());
		_ComboAccountMap[index] = account;
		
	}

	if (!_ComboAccountMap.empty()) {
		_CurrentAccountIndex = 0;
		_ComboAccount.SetCurSel(_CurrentAccountIndex);
		SetAccountInfo(_ComboAccountMap[_CurrentAccountIndex]);
		_LeftWnd->OnOrderChanged(0, 0);
	}
}

// SmMainOrderDialog dialog

IMPLEMENT_DYNAMIC(SmMainOrderDialog, CBCGPDialog)

SmMainOrderDialog::SmMainOrderDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_MAIN, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmMainOrderDialog::~SmMainOrderDialog()
{
}

void SmMainOrderDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
	//DDX_Control(pDX, IDC_STATIC_ACCOUNT_NAME, _StaticAccountName);
	DDX_Control(pDX, IDC_STATIC_LINE1, _Line1);
	DDX_Control(pDX, IDC_STATIC_LINE2, _Line2);
	DDX_Control(pDX, IDC_STATIC_LINE3, _Line3);
	DDX_Control(pDX, IDC_STATIC_LINE4, _Line4);
	DDX_Control(pDX, IDC_STATIC_LINE5, _Line5);
	DDX_Control(pDX, IDC_STATIC_LINE6, _Line6);
	DDX_Control(pDX, IDC_STATIC_LINE7, _Line7);
	DDX_Control(pDX, IDC_STATIC_LINE8, _Line8);
	DDX_Control(pDX, IDC_STATIC_LINE9, _Line9);
	DDX_Control(pDX, IDC_STATIC_LINE10, _Line10);
	DDX_Control(pDX, IDC_STATIC_MSG, _StaticMsg);
}


BEGIN_MESSAGE_MAP(SmMainOrderDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD, &SmMainOrderDialog::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_REMOVE, &SmMainOrderDialog::OnBnClickedBtnRemove)
	ON_BN_CLICKED(IDC_BTN_LEFT, &SmMainOrderDialog::OnBnClickedBtnLeft)
	ON_BN_CLICKED(IDC_BTN_RIGHT, &SmMainOrderDialog::OnBnClickedBtnRight)
	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	//ON_STN_CLICKED(IDC_STATIC_ACCOUNT_NAME, &SmMainOrderDialog::OnStnClickedStaticAccountName)
	ON_BN_CLICKED(IDC_BUTTON6, &SmMainOrderDialog::OnBnClickedButton6)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &SmMainOrderDialog::OnCbnSelchangeComboAccount)
	ON_MESSAGE(WM_ORDER_UPDATE, &SmMainOrderDialog::OnUmOrderUpdate)
	ON_MESSAGE(WM_SERVER_MSG, &SmMainOrderDialog::OnUmServerMsg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &SmMainOrderDialog::OnBnClickedBtnLiqAll)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// SmMainOrderDialog message handlers


LRESULT SmMainOrderDialog::OnEnterSizeMove(WPARAM wparam, LPARAM lparam)
{
	

	GetWindowRect(moveRect);
	// do stuff      
	return (LRESULT)0;
}
LRESULT SmMainOrderDialog::OnExitSizeMove(WPARAM wparam, LPARAM lparam)
{
	

	CRect rcWnd;
	GetWindowRect(rcWnd);

	if (moveRect.Width() == rcWnd.Width() && moveRect.Height() == rcWnd.Height()) return 0;

	RecalcChildren(CM_REFRESH);
	//Invalidate(FALSE);
	// do stuff      
	return (LRESULT)0;
}


BOOL SmMainOrderDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	CRect rcClient, rcWnd;

	_LineVector.push_back(&_Line1);
	_LineVector.push_back(&_Line2);
	_LineVector.push_back(&_Line3);
	_LineVector.push_back(&_Line4);
	_LineVector.push_back(&_Line5);
	_LineVector.push_back(&_Line6);
	_LineVector.push_back(&_Line7);
	_LineVector.push_back(&_Line8);
	_LineVector.push_back(&_Line9);
	_LineVector.push_back(&_Line10);

	GetWindowRect(rcWnd);


	_LeftWnd = std::make_shared<OrderLeftDialog>(this);
	_LeftWnd->Create(IDD_ORDER_LEFT, this);
	_LeftWnd->ShowWindow(SW_SHOW);
	_LeftWnd->SetMainWnd(this);


	std::shared_ptr<SmOrderWnd> center_wnd = std::make_shared<SmOrderWnd>(this);
	center_wnd->Create(IDD_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_SHOW);
	center_wnd->SetMainDialog(this);
	center_wnd->Selected(true);
	_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));

	center_wnd->GetWindowRect(rcWnd);
	
	_RightWnd = std::make_shared<OrderRightDialog>(this);
	_RightWnd->Create(IDD_ORDER_RIGHT, this);
	_RightWnd->ShowWindow(SW_SHOW);


	SetAccount();

	SetAccountForOrderWnd();


	
	GetWindowRect(rcWnd);

	rcWnd.bottom = rcWnd.top + 1000;
	MoveWindow(rcWnd);
	//GetClientRect(rcClient);
	//_StaticAccountName.GetWindowRect(rcClient);

	RECT rcFrame = { 0 };
	//AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW, FALSE, 0);

	

	RecalcChildren(CM_REFRESH);

	

	_Init = true;

	CenterWindow();

	_StaticMsg.SetUp();

	mainApp.CallbackMgr()->SubscribeOrderUpdateCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->SubscribeServerMsgCallback(GetSafeHwnd());

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmMainOrderDialog::OnQuoteAreaShowHide()
{

}

void SmMainOrderDialog::RecalcChildren(CmdMode mode)
{

	std::set<CWnd*> wnd_set;
	const int top_gap = 2;
	//const int hor_gap = 2;
	int max_height = 0, width_total = 0, start_x = 0;
	CRect rcWnd;
	CRect rcLine;
	CRect rcMain, rcMainWork, rcLeft, rcRight;
	_LeftWnd->GetWindowRect(rcLeft);
	_RightWnd->GetWindowRect(rcRight);
	GetWindowRect(rcMain);
	GetClientRect(rcMainWork);

	// 전체 길이 계산
	const int hor_gap = 5;
	int total_width = 0;
	

	start_x = hor_gap;

	int line_index = 0;
	if (_ShowLeft) {
		_LeftWnd->GetWindowRect(rcWnd);
		_rcLeft = rcWnd;
		_LeftWnd->MoveWindow(start_x, CtrlHeight + top_gap, rcWnd.Width(), rcWnd.Height());
		_LeftWnd->ShowWindow(SW_SHOW);
		max_height = rcWnd.Height();
		start_x += rcWnd.Width();
		width_total += rcWnd.Width();
		wnd_set.insert(_LeftWnd.get());

		rcLine.left = start_x + 1;
		rcLine.right = start_x + _LineGap;
		rcLine.top = CtrlHeight + top_gap + 2;
		rcLine.bottom = 1600;
		_LineVector[line_index]->MoveWindow(rcLine);
		_LineVector[line_index]->ShowWindow(SW_SHOW);
		line_index++;

		start_x += hor_gap;
	}
	else {
		_LeftWnd->ShowWindow(SW_HIDE);
	}
	

	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
		std::shared_ptr<SmOrderWnd> center_wnd = it->second;
		center_wnd->GetWindowRect(rcWnd);
		
		
		//center_wnd->ShowWindow(SW_HIDE);
		center_wnd->MoveWindow(start_x, CtrlHeight + top_gap, rcWnd.Width(), rcWnd.Height());
		
		_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));
		start_x += rcWnd.Width();
		width_total += rcWnd.Width();

		center_wnd->RecalcOrderAreaHeight(this);
		
		wnd_set.insert(center_wnd.get());

		rcLine.left = start_x + 1;
		rcLine.right = start_x + _LineGap;
		rcLine.top = CtrlHeight + top_gap + 2;
		rcLine.bottom = 1600;
		_LineVector[line_index]->MoveWindow(rcLine);
		_LineVector[line_index]->ShowWindow(SW_SHOW);
		line_index++;
		start_x += hor_gap;
	}
	

	if (_ShowRight) {
		_RightWnd->GetWindowRect(rcWnd);
		_rcRight = rcWnd;
		width_total += rcWnd.Width();
		if (rcWnd.Height() > max_height) max_height = rcWnd.Height();
		_RightWnd->MoveWindow(start_x, CtrlHeight + top_gap, rcWnd.Width(), rcWnd.Height());
		_RightWnd->ShowWindow(SW_SHOW);
		wnd_set.insert(_RightWnd.get());
		//start_x += rcWnd.Width();
		
		//start_x++;
	}
	else {
		_RightWnd->ShowWindow(SW_HIDE);
	}



	GetWindowRect(rcWnd);
	//MoveWindow(rcWnd.left, rcWnd.top, width_total + 21, rcWnd.Height());
	
	
	CRect rcClient;
	GetClientRect(&rcClient);

	for (auto it = wnd_set.begin(); it != wnd_set.end(); ++it) {

		(*it)->GetWindowRect(&rcWnd);
		ScreenToClient(rcWnd);
		rcWnd.bottom = rcClient.Height();
		//(*it)->MoveWindow(rcWnd);

		(*it)->SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_FRAMECHANGED);
		//(*it)->ShowWindow(SW_SHOW);
	}

	
	

	if (mode != CM_REFRESH)
		Invalidate();
	
	
	
	
	//SetWindowPos(nullptr, rcMain.left, rcMain.top, width_total + 20, rcMain.Height(), SWP_NOZORDER | SWP_NOREDRAW);

	std::vector<CRect> rcVector;
	if (_ShowLeft) {
		_LeftWnd->GetWindowRect(rcWnd);
		rcVector.push_back(rcWnd);
	}

	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); it++) {
		it->second->GetWindowRect(rcWnd);
		rcVector.push_back(rcWnd);
	}

	if (_ShowRight) {
		_RightWnd->GetWindowRect(rcWnd);
		rcVector.push_back(rcWnd);
	}
	for (size_t i = 0; i < rcVector.size(); i++) {
		total_width += rcVector[i].Width();
		total_width += hor_gap;
	}

	//total_width += hor_gap;

	GetWindowRect(rcWnd);
	rcWnd.right = rcWnd.left + total_width + 20;

	rcLine.left = 0;
	rcLine.right = rcWnd.Width() - 8;
	rcLine.top = CtrlHeight - 2;
	rcLine.bottom = CtrlHeight + _LineGap - 2;

	_LineVector[line_index]->MoveWindow(rcLine);
	_LineVector[line_index]->ShowWindow(SW_SHOW);
	line_index++;

	SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOZORDER | SWP_FRAMECHANGED);
	
	GetWindowRect(rcMain);
	_rcMain = rcMain;

	_StaticMsg.GetWindowRect(rcWnd);
	int len = rcMain.right - rcWnd.left - 13;
	rcWnd.right = rcWnd.left + len;
	ScreenToClient(rcWnd);
	//_StaticMsg.MoveWindow(rcWnd);
	_StaticMsg.SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOZORDER | SWP_FRAMECHANGED);
	SendMessage(WM_SERVER_MSG, 0, 0);
}


void SmMainOrderDialog::RecalcChildren2(CmdMode mode)
{
	// 전체 길이 계산
	const int hor_gap = 2;
	int total_width = 0;
	CRect rcWnd;
	std::vector<CRect> rcVector;
	if (_ShowLeft) {
		_LeftWnd->GetWindowRect(rcWnd);
		rcVector.push_back(rcWnd);
	}

	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); it++) {
		it->second->GetWindowRect(rcWnd);
		rcVector.push_back(rcWnd);
	}

	if (_ShowRight) {
		_RightWnd->GetWindowRect(rcWnd);
		rcVector.push_back(rcWnd);
	}
	for (size_t i = 0; i < rcVector.size(); i++) {
		total_width += rcVector[i].Width();
		total_width += hor_gap;
	}

	total_width += hor_gap;

	GetWindowRect(rcWnd);
	rcWnd.right = rcWnd.left + total_width + 20;
	
	SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOZORDER | SWP_NOREDRAW);
}

void SmMainOrderDialog::SetAccountForOrderWnd()
{
	if (_ComboAccountMap.size() > 0) {
		const int cur_sel = _ComboAccount.GetCurSel();
		if (cur_sel >= 0) {
			for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
				it->second->Account(_ComboAccountMap[cur_sel]);
			}
		}
	}
}


void SmMainOrderDialog::SetAccountInfo(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account || _CurrentAccountIndex < 0) return;

	std::string account_info;
	account_info.append(account->Name());
	account_info.append(" : ");
	account_info.append(account->No());
	//_StaticAccountName.SetWindowText(account_info.c_str());
	mainApp.Client()->RegisterAccount(account->No());
	_Account = account;

	_LeftWnd->SetAccount(_ComboAccountMap[_CurrentAccountIndex]);
	_RightWnd->SetAccount(_ComboAccountMap[_CurrentAccountIndex]);
}

void SmMainOrderDialog::OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); it++) {
		if (it->second->Selected()) {
			it->second->OnSymbolClicked(symbol->SymbolCode());
			break;
		}
	}
	_RightWnd->OnSymbolClicked(symbol);
}

void SmMainOrderDialog::OnSymbolClicked(const std::string& symbol_code)
{
	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); it++) {
		if (it->second->Selected()) {
			it->second->OnSymbolClicked(symbol_code);
			break;
		}
	}
}

void SmMainOrderDialog::OnBnClickedBtnAdd()
{
	// 먼저 현재 창의 크기를 구한다.
	CRect rcWnd, rcCenter, rcRight, rcClient;
	GetWindowRect(rcWnd);
	GetClientRect(rcClient);

	std::shared_ptr<SmOrderWnd> center_wnd = std::make_shared<SmOrderWnd>();
	center_wnd->Create(IDD_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_HIDE);
	center_wnd->SetMainDialog(this);
	center_wnd->Account(_ComboAccountMap[_CurrentAccountIndex]);
	_CenterWndMap.insert(std::make_pair(center_wnd->ID(), center_wnd));
	center_wnd->GetWindowRect(rcCenter);
	ScreenToClient(rcCenter);
	if (_ShowRight) {
		_RightWnd->GetWindowRect(rcRight);
		ScreenToClient(rcRight);
	}

	CRect newRcRight, newRcCenter, newRcRect;
	newRcRight.left = _LineGap + rcClient.right + rcCenter.Width() - rcRight.Width();
	newRcRight.top = rcRight.top;
	newRcRight.bottom = rcClient.Height();
	newRcRight.right = _LineGap + rcClient.right + rcCenter.Width();

	//_RightWnd->SetWindowPos(nullptr, newRcRight.left, newRcRight.top, newRcRight.Width(), newRcRight.Height(), SWP_NOZORDER | SWP_NOREDRAW);
	_RightWnd->MoveWindow(newRcRight);

	newRcCenter.left = _LineGap + rcClient.right - rcRight.Width();
	newRcCenter.top = rcRight.top;
	newRcCenter.right = newRcCenter.left + rcCenter.Width();
	
	//auto it = std::prev(_CenterWndMap.end());
	//it->second->GetWindowRect(rcCenter);
	newRcCenter.bottom = rcClient.Height();

	//center_wnd->SetWindowPos(nullptr, newRcCenter.left, newRcCenter.top, newRcCenter.Width(), newRcCenter.Height(), SWP_NOZORDER | SWP_NOREDRAW);
	center_wnd->MoveWindow(newRcCenter);
	int dif = center_wnd->RecalcOrderAreaHeight(this);
	center_wnd->ShowWindow(SW_SHOW);

	newRcRect.left = rcWnd.left;
	newRcRect.right = rcWnd.right + rcCenter.Width();
	newRcRect.top = rcWnd.top;
	newRcRect.bottom = rcWnd.bottom;

	//SetWindowPos(nullptr, newRcRect.left, newRcRect.top, newRcRect.Width(), newRcRect.Height(), SWP_NOZORDER | SWP_NOREDRAW);
	MoveWindow(newRcRect);

// 	for (int i = 1; i <= rcCenter.Width(); i++) {
// 		newRcRect.left = rcWnd.left;
// 		newRcRect.right = rcWnd.right + i;
// 		newRcRect.top = rcWnd.top;
// 		newRcRect.bottom = rcWnd.bottom;
// 		MoveWindow(newRcRect);
// 	}

	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	pFrame->ReconnectOrderEvent();
}


void SmMainOrderDialog::OnBnClickedBtnRemove()
{
	if (_CenterWndMap.size() == 1) return;

	//LockWindowUpdate();
	auto it = std::prev(_CenterWndMap.end());
	_CenterWndMap.erase(it);
	RecalcChildren(CM_DEL_CENTER);
	//UnlockWindowUpdate();
	Invalidate(FALSE);
}


void SmMainOrderDialog::OnBnClickedBtnLeft()
{
	_ShowLeft ? _ShowLeft = false : _ShowLeft = true;
	//LockWindowUpdate();
	RecalcChildren(_ShowLeft ? CM_SHOW_LEFT : CM_HIDE_LEFT);
	//UnlockWindowUpdate();
	Invalidate(FALSE);
}


void SmMainOrderDialog::OnBnClickedBtnRight()
{
	_ShowRight ? _ShowRight = false : _ShowRight = true;
	//LockWindowUpdate();
	RecalcChildren(_ShowRight ? CM_SHOW_RIGHT : CM_HIDE_RIGHT);
	//UnlockWindowUpdate();
	Invalidate(FALSE);
}


void SmMainOrderDialog::OnStnClickedStaticAccountName()
{
	
}


void SmMainOrderDialog::OnBnClickedButton6()
{
	int max_delta_height = 0;
	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
		std::shared_ptr<SmOrderWnd> center_wnd = it->second;
		int delta_height = center_wnd->RecalcOrderAreaHeight(this);
		if (delta_height > max_delta_height) max_delta_height = delta_height;
	}

	
	if (max_delta_height > 2) {
		CRect rcWnd;
		GetWindowRect(rcWnd);
		rcWnd.bottom += max_delta_height;
		SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOMOVE);
	}

	Invalidate(FALSE);
}


void SmMainOrderDialog::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (!_Init) return;

	//LockWindowUpdate();
	RecalcChildren(CM_REFRESH);
	//UnlockWindowUpdate();
	//Invalidate(FALSE);
}


BOOL SmMainOrderDialog::OnEraseBkgnd(CDC* pDC)
{
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void SmMainOrderDialog::OnCbnSelchangeComboAccount()
{
	_CurrentAccountIndex = _ComboAccount.GetCurSel();
	if (_CurrentAccountIndex < 0) return;

	SetAccountInfo(_ComboAccountMap[_CurrentAccountIndex]);

	SetAccountForOrderWnd();
}

LRESULT SmMainOrderDialog::OnUmOrderUpdate(WPARAM wParam, LPARAM lParam)
{
	const int account_id = (int)wParam;
	const int symbol_id = (int)lParam;

	_LeftWnd->OnOrderChanged(account_id, symbol_id);
	_RightWnd->OnOrderChanged(account_id, symbol_id);

	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); it++) {
		it->second->OnOrderChanged(account_id, symbol_id);
	}

	return 1;
}

void SmMainOrderDialog::ChangedSymbol(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	_RightWnd->SetSymbol(symbol);
}

void SmMainOrderDialog::ChangedCenterWindow(const int& center_wnd_id)
{
	for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
		it->second->ID() == center_wnd_id ? it->second->SetSelected(true) : it->second->SetSelected(false);
	}
}


void SmMainOrderDialog::OnClose()
{
	mainApp.CallbackMgr()->UnsubscribeOrderUpdateCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->UnsubscribeServerMsgCallback(GetSafeHwnd());

	

	CBCGPDialog::OnClose();
}


void SmMainOrderDialog::OnBnClickedBtnLiqAll()
{
	if (!_Account) return;

	auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(_Account->No());
	const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();
	for (auto it = account_pos_map.begin(); it != account_pos_map.end(); ++it) {
		std::shared_ptr<SmOrderRequest> order_req = nullptr;
		if (it->second->Position == SmPositionType::Buy)
			order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(_Account->No(), _Account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
		else
			order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(_Account->No(), _Account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
		mainApp.Client()->NewOrder(order_req);
	}
}

LRESULT SmMainOrderDialog::OnUmServerMsg(WPARAM wParam, LPARAM lParam)
{
	_StaticMsg.Text(mainApp.TotalOrderMgr()->ServerMsg.c_str());
	_StaticMsg.Invalidate();
	return 1;
}


void SmMainOrderDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0x0000FFF0) == SC_MINIMIZE)
	{
		// Minimizing, post to main dialogue also.
		//AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
	}
	else if ((nID & 0x0000FFF0) == SC_RESTORE && IsIconic())
	{
		// Restoring, post to main dialogue also.
		//AfxGetMainWnd()->ShowWindow(SW_RESTORE);
		Invalidate(TRUE);
	}

	if ((nID & 0x0000FFF0) == SC_KEYMENU) {
		for (auto it = _CenterWndMap.begin(); it != _CenterWndMap.end(); ++it) {
			std::shared_ptr<SmOrderWnd> center_wnd = it->second;
			center_wnd->ArrangeCenterValue();
		}
		return;
	}

	CBCGPDialog::OnSysCommand(nID, lParam);
}


void SmMainOrderDialog::OnDestroy()
{
	CBCGPDialog::OnDestroy();

	
}


void SmMainOrderDialog::PostNcDestroy()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->RemoveOrderWnd(GetSafeHwnd());
	delete this;
	CBCGPDialog::PostNcDestroy();
}
