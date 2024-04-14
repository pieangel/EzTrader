// HdAccountPLDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "HdAccountPLDlg.h"
#include "afxdialogex.h"
#include "HdAccountPLGrid.h"
#include "HdProductRemainGrid.h"

#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Fund/SmFundManager.h"
#include "../Fund/SmFund.h"
#include "../MainFrm.h"
#include "../Task/SmTaskArg.h"
#include "../Global/SmTotalManager.h"
#include "../Task/SmTaskRequestManager.h"
#include "../Log/MyLogger.h"

// HdAccountPLDlg dialog

IMPLEMENT_DYNAMIC(HdAccountPLDlg, CDialog)

HdAccountPLDlg::HdAccountPLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MINI_JANGO, pParent)
{
	
}

HdAccountPLDlg::HdAccountPLDlg(CWnd* pParent, std::string type)
	: CDialog(IDD_MINI_JANGO, pParent), type_(type)
{

}

HdAccountPLDlg::HdAccountPLDlg(CWnd* pParent, const std::string& type, const int mode)
	: CDialog(IDD_MINI_JANGO, pParent), type_(type), _Mode(mode)
{
	
}

HdAccountPLDlg::HdAccountPLDlg(CWnd* pParent, const std::string& type, const int mode, const std::string& target)
	: CDialog(IDD_MINI_JANGO, pParent), type_(type), _Mode(mode)
{
	if (mode == 0)
	{
		account_no_ = target;
	}
	else {
		fund_name_ = target;
	}
}

HdAccountPLDlg::~HdAccountPLDlg()
{
// 	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
// 	_WindowEvent -= delegate(wndMgr, &HdWindowManager::OnWindowEvent);
// 	SmCallbackManager::GetInstance()->UnsubscribeQuoteWndCallback(GetSafeHwnd());
// 	SmCallbackManager::GetInstance()->UnsubscribeOrderWndCallback(GetSafeHwnd());
// 	SmCallbackManager::GetInstance()->UnsubscribeAccountWndCallback(GetSafeHwnd());
}

void HdAccountPLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
}


BEGIN_MESSAGE_MAP(HdAccountPLDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &HdAccountPLDlg::OnCbnSelchangeComboAccount)
	ON_WM_SIZE()
	ON_CBN_DROPDOWN(IDC_COMBO_ACCOUNT, &HdAccountPLDlg::OnCbnDropdownComboAccount)
// 	ON_MESSAGE(WM_QUOTE_CHANGED, OnQuoteChangedMessage)
// 	ON_MESSAGE(WM_ORDER_CHANGED, OnOrderChangedMessage)
// 	ON_MESSAGE(WM_ACCOUNT_CHANGED, OnAccountChangedMessage)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// HdAccountPLDlg message handlers
void HdAccountPLDlg::OnTimer(UINT_PTR nIDEvent)
{
	//LOGINFO(DarkHorse::CMyLogger::getInstance(), "OnTimer = %d", nIDEvent);
	_AccountGrid.refresh();
	_ProductGrid.refresh();
	CDialog::OnTimer(nIDEvent);
}

BOOL HdAccountPLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	// TODO:  Add extra initialization here
	
	_ProductGrid.AcntPLDlg(this);
	_AccountGrid.AcntPLDlg(this);
	_AccountGrid.AttachGrid(this, IDC_STATIC_MINI_TOP);
	_ProductGrid.AttachGrid(this, IDC_STATIC_MINI_SUB);
	((CButton*)GetDlgItem(IDC_CHECK_THOUSAND))->SetCheck(BST_CHECKED);

	
	CRect rcWnd;
	GetWindowRect(rcWnd);
	SetWindowPos(nullptr, 0, 0, 210, rcWnd.Height(), SWP_NOMOVE);
	_ComboAccount.SetDroppedWidth(250);
	_Mode == 0 ? SetAccount() : SetFund();
	SetTimer(1, 100, NULL);
// 	SmCallbackManager::GetInstance()->SubscribeQuoteWndCallback(GetSafeHwnd());
// 	SmCallbackManager::GetInstance()->SubscribeOrderWndCallback(GetSafeHwnd());
// 	SmCallbackManager::GetInstance()->SubscribeAccountWndCallback(GetSafeHwnd());
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void HdAccountPLDlg::SetAccount()
{
	//_StaticCombo.SetWindowText("°èÁÂ");
	_ComboAccount.ResetContent();
	std::vector<std::shared_ptr<DarkHorse::SmAccount>> main_account_vector;
	mainApp.AcntMgr()->get_main_account_vector(type_, main_account_vector);
	if (main_account_vector.empty()) return;
	int selected_index = 0;
	for (auto ita = main_account_vector.begin(); ita != main_account_vector.end(); ++ita) {
		auto main_acnt = *ita;
		std::string account_info;
		account_info.append(main_acnt->Name());
		account_info.append(" : ");
		account_info.append(main_acnt->No());
		const int index = _ComboAccount.AddString(account_info.c_str());
		_ComboAccountMap[index] = main_acnt->No();
		//_AccountComboMap[main_acnt->No()] = index;
		if (account_no_ == main_acnt->No())
			selected_index = index;

		const std::vector<std::shared_ptr<DarkHorse::SmAccount>>& sub_account_vector = main_acnt->get_sub_accounts();
		for (auto it = sub_account_vector.begin(); it != sub_account_vector.end(); it++) {
			auto account = *it;
			account_info = "";
			account_info.append(account->Name());
			account_info.append(" : ");
			account_info.append(account->No());
			const int index = _ComboAccount.AddString(account_info.c_str());
			_ComboAccountMap[index] = account->No();
			//_AccountComboMap[account->No()] = index;
			if (account_no_ == account->No())
				selected_index = index;
		}
	}

	if (!_ComboAccountMap.empty()) {
		_CurrentAccountIndex = selected_index;
		_ComboAccount.SetCurSel(_CurrentAccountIndex);
		const std::string account_no = _ComboAccountMap[_CurrentAccountIndex];
		auto account = mainApp.AcntMgr()->FindAccount(account_no);
		if (account == nullptr) return;
		_AccountGrid.Account(account);
		_ProductGrid.Account(account);
		account_no_ = account_no;
	}
}


void HdAccountPLDlg::SetFund()
{
	//_StaticCombo.SetWindowText("ÆÝµå");
	_ComboAccount.ResetContent();
	const std::map<std::string, std::shared_ptr<DarkHorse::SmFund>>& fund_map = mainApp.FundMgr()->GetFundMap();
	int selected_index = 0;
	for (auto it = fund_map.begin(); it != fund_map.end(); ++it) {
		auto fund = it->second;
		std::string account_info;
		account_info.append(fund->Name());
		const int index = _ComboAccount.AddString(account_info.c_str());
		_ComboFundMap[index] = fund->Name();
		//_AccountComboMap[fund->Name()] = index;
		if (fund_name_ == fund->Name())
			selected_index = index;
	}

	if (!_ComboFundMap.empty()) {
		_CurrentAccountIndex = selected_index;
		_ComboAccount.SetCurSel(_CurrentAccountIndex);
		const std::string cur_fund_name = _ComboFundMap[_CurrentAccountIndex];
		auto fund = mainApp.FundMgr()->FindFund(cur_fund_name);
		if (fund == nullptr) return;
		_AccountGrid.Fund(fund);
		_ProductGrid.Fund(fund);
		fund_name_ = cur_fund_name;
	}
}


void HdAccountPLDlg::SaveToXml(pugi::xml_node& window_node)
{
	CRect rcWnd;
	GetWindowRect(rcWnd);
	pugi::xml_node window_child = window_node.append_child("window_pos");
	window_child.append_attribute("left") = rcWnd.left;
	window_child.append_attribute("top") = rcWnd.top;
	window_child.append_attribute("right") = rcWnd.right;
	window_child.append_attribute("bottom") = rcWnd.bottom;
// 	if (_Account) {
// 		window_child = window_node.append_child("account_no");
// 		window_child.append_child(pugi::node_pcdata).set_value(_Account->AccountNo.c_str());
// 	}
}

void HdAccountPLDlg::LoadFromXml(pugi::xml_node& window_node)
{
	pugi::xml_node window_pos_node = window_node.child("window_pos");
	CRect rcWnd;
	rcWnd.left = window_pos_node.attribute("left").as_int();
	rcWnd.top = window_pos_node.attribute("top").as_int();
	rcWnd.right = window_pos_node.attribute("right").as_int();
	rcWnd.bottom = window_pos_node.attribute("bottom").as_int();
	pugi::xml_node account_no_node = window_node.child("account_no");
	if (account_no_node) {
		_DefaultAccount = window_node.child_value("account_no");
		SetDefaultAccount();
	}
	MoveWindow(rcWnd);
	ShowWindow(SW_SHOW);
}

void HdAccountPLDlg::InitAccount()
{
	//_ComboAccount.ResetContent();
}



void HdAccountPLDlg::SetDefaultAccount()
{
	InitAccount();
	_AccountGrid.ClearValues();
	_ProductGrid.ClearValues();

	_AccountGrid.InitGrid();
	_ProductGrid.InitGrid();
}

int HdAccountPLDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


void HdAccountPLDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(1);
	CDialog::OnClose();
}

void HdAccountPLDlg::OnReceiveAccountInfo()
{
	_AccountGrid.InitGrid();
	_ProductGrid.InitGrid();
}

void HdAccountPLDlg::OnCbnSelchangeComboAccount()
{
	const int cur_sel = _ComboAccount.GetCurSel();
	if (cur_sel < 0) return;
	_AccountGrid.ClearValues();
	_ProductGrid.ClearValues();

	_CurrentAccountIndex = cur_sel;
	if (_Mode == 0) {
		const std::string& account_no = _ComboAccountMap[_CurrentAccountIndex];
		auto account = mainApp.AcntMgr()->FindAccount(account_no);
		if (account == nullptr) return;
		account_no_ = account_no;
		_AccountGrid.Account(account);
		_ProductGrid.Account(account);

		_AccountGrid.InitGrid();
		_ProductGrid.InitGrid();


		_AccountGrid.Invalidate();
		_ProductGrid.Invalidate();

		DhTaskArg arg;
		arg.detail_task_description = account->No();
		arg.task_type = DhTaskType::AccountProfitLoss;
		arg.parameter_map["account_no"] = account->No();
		arg.parameter_map["password"] = account->Pwd();
		arg.parameter_map["account_type"] = account->Type();

		mainApp.TaskReqMgr()->AddTask(std::move(arg));


	}
	else {
		const std::string cur_fund_name = _ComboFundMap[_CurrentAccountIndex];
		auto fund = mainApp.FundMgr()->FindFund(cur_fund_name);
		if (fund == nullptr) return;
		fund_name_ = cur_fund_name;
		_AccountGrid.Fund(fund);
		_ProductGrid.Fund(fund);

		_AccountGrid.InitGrid();
		_ProductGrid.InitGrid();

		_AccountGrid.Invalidate();
		_ProductGrid.Invalidate();
	}
}


void HdAccountPLDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (_ProductGrid.GetSafeHwnd())
		_ProductGrid.MoveWindow(0, 112, cx, cy - 20);
}


void HdAccountPLDlg::OnCbnDropdownComboAccount()
{
	//_Mode == 0 ? SetAccount() : SetFund();
	//_AccountGrid.InitGrid();
	//_ProductGrid.InitGrid();
}

LRESULT HdAccountPLDlg::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	_AccountGrid.InitGrid();
	_ProductGrid.InitGrid();
	return 1;
}

LRESULT HdAccountPLDlg::OnOrderChangedMessage(WPARAM wParam, LPARAM lParam)
{
	_AccountGrid.InitGrid();
	_ProductGrid.InitGrid();
	return 1;
}

LRESULT HdAccountPLDlg::OnAccountChangedMessage(WPARAM wParam, LPARAM lParam)
{
	_AccountGrid.InitGrid();
	_ProductGrid.InitGrid();
	return 1;
}
