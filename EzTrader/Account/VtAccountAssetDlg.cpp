// VtAccountAssetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "VtAccountAssetDlg.h"
#include "afxdialogex.h"
#include <numeric>
#include <functional>
#include "../Account/SmAccount.h"
using namespace std;
using namespace std::placeholders;


// VtAccountAssetDlg dialog

IMPLEMENT_DYNAMIC(VtAccountAssetDlg, CDialog)

VtAccountAssetDlg::VtAccountAssetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_ACCOUNT_ASSET, pParent)
{
	
}

VtAccountAssetDlg::VtAccountAssetDlg(CWnd* pParent, const std::string& type, const std::string& account_no)
	: CDialog(IDD_ACCOUNT_ASSET, pParent)
{
	_AssetPage.type(type);
	_AssetPage.account_no(account_no);
}

VtAccountAssetDlg::~VtAccountAssetDlg()
{
// 	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
// 	_WindowEvent -= delegate(wndMgr, &HdWindowManager::OnWindowEvent);
// 	SmCallbackManager::GetInstance()->UnsubscribeQuoteWndCallback(GetSafeHwnd());
// 	SmCallbackManager::GetInstance()->UnsubscribeOrderWndCallback(GetSafeHwnd());
// 	SmCallbackManager::GetInstance()->UnsubscribeAccountCallback((long)this);
// 	SmCallbackManager::GetInstance()->UnsubscribeAccountWndCallback(GetSafeHwnd());
}

void VtAccountAssetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_ACCOUNT_ASSET, _TabCtrl);
}


BEGIN_MESSAGE_MAP(VtAccountAssetDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_ACCOUNT_ASSET, &VtAccountAssetDlg::OnTcnSelchangeTabAccountAsset)
	ON_WM_CLOSE()
// 	ON_MESSAGE(WM_QUOTE_CHANGED, OnQuoteChangedMessage)
// 	ON_MESSAGE(WM_ORDER_CHANGED, OnOrderChangedMessage)
// 	ON_MESSAGE(WM_ACCOUNT_CHANGED, OnAccountChangedMessage)
END_MESSAGE_MAP()


// VtAccountAssetDlg message handlers


std::string VtAccountAssetDlg::account_no()
{
	return _AssetPage.account_no();
}

void VtAccountAssetDlg::account_no(std::string val)
{
	_AssetPage.account_no(val);
}

std::string VtAccountAssetDlg::type()
{
	return _AssetPage.type();
}

void VtAccountAssetDlg::type(std::string val)
{
	_AssetPage.type(val);
}

void VtAccountAssetDlg::SaveToXml(pugi::xml_node& window_node)
{
	CRect rcWnd;
	GetWindowRect(rcWnd);
	pugi::xml_node window_child = window_node.append_child("window_pos");
	window_child.append_attribute("left") = rcWnd.left;
	window_child.append_attribute("top") = rcWnd.top;
	window_child.append_attribute("right") = rcWnd.right;
	window_child.append_attribute("bottom") = rcWnd.bottom;
	/*
	if (_AssetPage.GetSafeHwnd() && _AssetPage.Account()) {
		window_child = window_node.append_child("account_no");
		window_child.append_child(pugi::node_pcdata).set_value(_AssetPage.Account()->AccountNo.c_str());
	}
	*/
}

void VtAccountAssetDlg::LoadFromXml(pugi::xml_node& window_node)
{
	pugi::xml_node window_pos_node = window_node.child("window_pos");
	CRect rcWnd;
	rcWnd.left = window_pos_node.attribute("left").as_int();
	rcWnd.top = window_pos_node.attribute("top").as_int();
	rcWnd.right = window_pos_node.attribute("right").as_int();
	rcWnd.bottom = window_pos_node.attribute("bottom").as_int();
	pugi::xml_node account_no_node = window_node.child("account_no");
	if (account_no_node) {
		std::string account_no = window_node.child_value("account_no");
		SetAccount(account_no);
	}
	MoveWindow(rcWnd);
	ShowWindow(SW_SHOW);
}

void VtAccountAssetDlg::InitTabCtrl()
{
	_TabCtrl.InsertItem(0, _T("예탁잔고 및 증거금"));
	_TabCtrl.InsertItem(1, _T("주문가능수량"));

	CRect rect;
	_TabCtrl.GetClientRect(rect);

	_AssetPage.Create(IDD_ASSET, &_TabCtrl);
	//_AssetPage.OrderConfigMgr(_OrderConfigMgr);
	_AssetPage.SetWindowPos(nullptr, 0, 25, rect.Width(), rect.Height() - 30, SWP_NOZORDER);
	_CurrentPage = &_AssetPage;

	_OrderPage.Create(IDD_ORDER_AVAILABLE, &_TabCtrl);
	//_OrderPage.OrderConfigMgr(_OrderConfigMgr);
	_OrderPage.SetWindowPos(nullptr, 0, 25, rect.Width(), rect.Height() - 30, SWP_NOZORDER);
	_OrderPage.ShowWindow(SW_HIDE);

	_CurrentPage->ShowWindow(SW_SHOW);
}

BOOL VtAccountAssetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	InitTabCtrl();
	// TODO:  Add extra initialization here
	CRect rect;
	GetWindowRect(rect);
	SetWindowPos(nullptr, 0, 0, 328, rect.Height(), SWP_NOMOVE);

// 	SmCallbackManager::GetInstance()->SubscribeQuoteWndCallback(GetSafeHwnd());
// 	SmCallbackManager::GetInstance()->SubscribeOrderWndCallback(GetSafeHwnd());
// 	SmCallbackManager::GetInstance()->SubscribeAccountCallback((long)this, std::bind(&VtAccountAssetDlg::OnAccountEvent, this, _1));
// 	SmCallbackManager::GetInstance()->SubscribeAccountWndCallback(GetSafeHwnd());

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtAccountAssetDlg::OnTcnSelchangeTabAccountAsset(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	if (_CurrentPage != nullptr)
	{
		_CurrentPage->ShowWindow(SW_HIDE);
		_CurrentPage = nullptr;
	}

	int curIndex = _TabCtrl.GetCurSel();
	switch (curIndex)
	{
	case 0:
		_AssetPage.ShowWindow(SW_SHOW);
		_CurrentPage = &_AssetPage;
		break;
	case 1:
		_OrderPage.ShowWindow(SW_SHOW);
		_CurrentPage = &_OrderPage;
		break;
	}
	*pResult = 0;
}


void VtAccountAssetDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnClose();

// 	HdWindowEventArgs arg;
// 	arg.pWnd = this;
// 	arg.wndType = HdWindowType::AssetWindow;
// 	arg.eventType = HdWindowEventType::Closed;
// 	FireWindowEvent(std::move(arg));
}

void VtAccountAssetDlg::OnReceiveAccountInfo()
{
	if (_AssetPage.GetSafeHwnd())
	{
		_AssetPage.OnReceiveAccountInfo();
	}
}

void VtAccountAssetDlg::SetAccount(std::string acntNo)
{
	if (_AssetPage.GetSafeHwnd()) {
		_AssetPage.SetAccount(acntNo);
	}
}

LRESULT VtAccountAssetDlg::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	if (_AssetPage.GetSafeHwnd())
	{
		_AssetPage.OnReceiveAccountInfo();
	}
	return 1;
}

LRESULT VtAccountAssetDlg::OnOrderChangedMessage(WPARAM wParam, LPARAM lParam)
{
	return 1;
}

void VtAccountAssetDlg::RegisterAccountCallback()
{

}

void VtAccountAssetDlg::UnregisterAccountCallback()
{

}

// void VtAccountAssetDlg::OnAccountEvent(VtAccount* account)
// {
// 	if (_AssetPage.GetSafeHwnd())
// 	{
// 		_AssetPage.OnReceiveAccountInfo();
// 	}
// }

LRESULT VtAccountAssetDlg::OnAccountChangedMessage(WPARAM wParam, LPARAM lParam)
{
	if (_AssetPage.GetSafeHwnd())
	{
		_AssetPage.OnReceiveAccountInfo();
	}
	return 1;
}
