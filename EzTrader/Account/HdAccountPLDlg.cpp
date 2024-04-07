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

// HdAccountPLDlg dialog

IMPLEMENT_DYNAMIC(HdAccountPLDlg, CDialog)

HdAccountPLDlg::HdAccountPLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MINI_JANGO, pParent)
{
	_Account = nullptr;
	
}

HdAccountPLDlg::HdAccountPLDlg(CWnd* pParent, std::string type)
	: CDialog(IDD_MINI_JANGO, pParent), type_(type)
{
	_Account = nullptr;

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
END_MESSAGE_MAP()


// HdAccountPLDlg message handlers


BOOL HdAccountPLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	// TODO:  Add extra initialization here
	_Mode == 0 ? SetAccount() : SetFund();
	_ProductGrid.AcntPLDlg(this);
	_AccountGrid.AcntPLDlg(this);
	_AccountGrid.AttachGrid(this, IDC_STATIC_MINI_TOP);
	_ProductGrid.AttachGrid(this, IDC_STATIC_MINI_SUB);
	((CButton*)GetDlgItem(IDC_CHECK_THOUSAND))->SetCheck(BST_CHECKED);

	
	CRect rcWnd;
	GetWindowRect(rcWnd);
	SetWindowPos(nullptr, 0, 0, 210, rcWnd.Height(), SWP_NOMOVE);
	_ComboAccount.SetDroppedWidth(250);
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
		//account_position_view_.Account(account);
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
		//account_position_view_.Fund(fund);
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
	_ComboAccount.ResetContent();
	/*
	VtGlobal* global = VtGlobal::GetInstance();
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	
	std::string acntName;
	std::map<std::string, int> comboMap;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		acntName = acnt->AccountNo;
		acntName.append(_T(" "));
		acntName.append(acnt->AccountName);
		int index = _ComboAccount.AddString(acntName.c_str());
		_ComboAccount.SetItemDataPtr(index, acnt);
		comboMap[acnt->AccountNo] = index;
		if (acnt->AccountLevel() == 0) {
			std::vector<VtAccount*>& subAcntList = acnt->GetSubAccountList();
			for (auto it = subAcntList.begin(); it != subAcntList.end(); ++it) {
				VtAccount* subAcnt = *it;
				acntName = subAcnt->AccountNo;
				acntName.append(_T(" "));
				acntName.append(subAcnt->AccountName);
				index = _ComboAccount.AddString(acntName.c_str());
				_ComboAccount.SetItemDataPtr(index, subAcnt);
				comboMap[subAcnt->AccountNo] = index;
			}
		}
	}
	
	if (comboMap.size() == 0)
		return;
	
	int selAcnt = 0;
	auto it = comboMap.find(_DefaultAccount);
	it == comboMap.end() ? selAcnt = 0 : selAcnt = it->second;
	_ComboAccount.SetCurSel(selAcnt);
	_Account = (VtAccount*)_ComboAccount.GetItemDataPtr(selAcnt);
	if (_Account->Enable() && _Account->hasValidPassword()) {
		//for(int i = 0; i < 100; i++)
		_Account->GetAccountInfoNFee(1);
	}
	*/
}


void HdAccountPLDlg::Account(VtAccount* val)
{
	_Account = val;
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

	CDialog::OnClose();
}

void HdAccountPLDlg::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;
}


void HdAccountPLDlg::OnOrderFilledHd(VtOrder* order)
{
}

void HdAccountPLDlg::OnSymbolMaster(VtSymbol* sym)
{
	if (!sym)
		return;

	if (!_Account)
		return;

	//_Account->CalcOpenPL(sym);
	//_Account->SumOpenPL();
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
		//_ProductGrid.Account(account);

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
		//_ProductGrid.Fund(fund);

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
	_Mode == 0 ? SetAccount() : SetFund();
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
