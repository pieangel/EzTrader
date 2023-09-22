// MiniJangoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "MiniJangoDialog.h"
#include "afxdialogex.h"
#include <unordered_map>
#include <vector>
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccountManager.h"
#include "../Fund/SmFundManager.h"
#include "../Fund/SmFund.h"
#include "../MainFrm.h"
#include "../Task/SmTaskArg.h"
#include "../Global/SmTotalManager.h"
#include "../Task/SmTaskRequestManager.h"
// MiniJangoDialog dialog
using namespace DarkHorse;
IMPLEMENT_DYNAMIC(MiniJangoDialog, CBCGPDialog)

MiniJangoDialog::MiniJangoDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_JANGO, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

MiniJangoDialog::~MiniJangoDialog()
{
}

void MiniJangoDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, account_profit_loss_view_);
	DDX_Control(pDX, IDC_STATIC_COMBO, _StaticCombo);
	//DDX_Control(pDX, IDC_STATIC_POSITION, _StaticPosition);
}


BEGIN_MESSAGE_MAP(MiniJangoDialog, CBCGPDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &MiniJangoDialog::OnCbnSelchangeComboAccount)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// SmJangGoDialog message handlers


BOOL MiniJangoDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	account_profit_loss_view_.Mode(_Mode);
	account_profit_loss_view_.SetUp();
	//account_position_view_.SetUp();
	account_profit_loss_view_.Mode(_Mode);
	account_position_view_.Mode(_Mode);
	_Mode == 0 ? SetAccount() : SetFund();

	//account_profit_loss_view_.SetUp();




	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_POSITION);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	account_position_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, 0x12345678);

	SetTimer(1, 100, NULL);

	std::vector<int> column_width_vector;
	column_width_vector.push_back(60);
	column_width_vector.push_back(40);
	column_width_vector.push_back(75);
	column_width_vector.push_back(40);
	//account_position_view_.set_column_widths(column_width_vector);

	_ComboAccount.SetDroppedWidth(250);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void MiniJangoDialog::OnTimer(UINT_PTR nIDEvent)
{
	account_position_view_.on_timer();
}

void MiniJangoDialog::SetAccount()
{
	_StaticCombo.SetWindowText("����");
	/*
	const std::unordered_map<std::string, std::shared_ptr<DarkHorse::SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
	for (auto it = account_map.begin(); it != account_map.end(); ++it) {
		auto account = it->second;
		//if (account->is_subaccount()) continue;

		std::string account_info;
		account_info.append(account->Name());
		account_info.append(" : ");
		account_info.append(account->No());
		const int index = _ComboAccount.AddString(account_info.c_str());
		_ComboAccountMap[index] = account->No();

	}
	*/


	std::vector<std::shared_ptr<SmAccount>> main_account_vector;
	mainApp.AcntMgr()->get_main_account_vector(main_account_vector);
	if (main_account_vector.empty()) return;
	for (auto ita = main_account_vector.begin(); ita != main_account_vector.end(); ++ita) {
		auto main_acnt = *ita;
		std::string account_info;
		account_info.append(main_acnt->Name());
		account_info.append(" : ");
		account_info.append(main_acnt->No());
		const int index = _ComboAccount.AddString(account_info.c_str());
		_ComboAccountMap[index] = main_acnt->No();
		//_AccountComboMap[main_acnt->No()] = index;

		const std::vector<std::shared_ptr<SmAccount>>& sub_account_vector = main_acnt->get_sub_accounts();
		for (auto it = sub_account_vector.begin(); it != sub_account_vector.end(); it++) {
			auto account = *it;
			account_info = "";
			account_info.append(account->Name());
			account_info.append(" : ");
			account_info.append(account->No());
			const int index = _ComboAccount.AddString(account_info.c_str());
			_ComboAccountMap[index] = account->No();
			//_AccountComboMap[account->No()] = index;
		}
	}

	if (!_ComboAccountMap.empty()) {
		_CurrentAccountIndex = 0;
		_ComboAccount.SetCurSel(_CurrentAccountIndex);
		const std::string account_no = _ComboAccountMap[_CurrentAccountIndex];
		auto account = mainApp.AcntMgr()->FindAccount(account_no);
		if (account == nullptr) return;
		account_profit_loss_view_.Account(account);
		account_position_view_.Account(account);
	}
}


void MiniJangoDialog::SetFund()
{
	_StaticCombo.SetWindowText("�ݵ�");
	const std::map<std::string, std::shared_ptr<SmFund>>& fund_map = mainApp.FundMgr()->GetFundMap();

	for (auto it = fund_map.begin(); it != fund_map.end(); ++it) {
		auto fund = it->second;
		std::string account_info;
		account_info.append(fund->Name());
		const int index = _ComboAccount.AddString(account_info.c_str());
		_ComboFundMap[index] = fund->Name();

	}

	if (!_ComboFundMap.empty()) {
		_CurrentAccountIndex = 0;
		_ComboAccount.SetCurSel(_CurrentAccountIndex);
		const std::string cur_fund_name = _ComboFundMap[_CurrentAccountIndex];
		auto fund = mainApp.FundMgr()->FindFund(cur_fund_name);
		if (fund == nullptr) return;
		account_profit_loss_view_.Fund(fund);
		account_position_view_.Fund(fund);
	}
}

void MiniJangoDialog::OnCbnSelchangeComboAccount()
{
	const int cur_sel = _ComboAccount.GetCurSel();
	if (cur_sel < 0) return;

	_CurrentAccountIndex = cur_sel;
	if (_Mode == 0) {
		const std::string& account_no = _ComboAccountMap[_CurrentAccountIndex];
		auto account = mainApp.AcntMgr()->FindAccount(account_no);
		if (account == nullptr) return;
		account_profit_loss_view_.Account(account);
		account_position_view_.Account(account);
		account_profit_loss_view_.Invalidate();
		account_position_view_.Invalidate();

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
		account_profit_loss_view_.Fund(fund);
		account_position_view_.Fund(fund);
		//account_profit_loss_view_.UpdateFundAssetInfo();
		//account_position_view_.UpdateFundPositionInfo();
		account_profit_loss_view_.Invalidate();
		account_position_view_.Invalidate();
	}
}


void MiniJangoDialog::PostNcDestroy()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->RemoveJangoWnd(GetSafeHwnd());

	CBCGPDialog::PostNcDestroy();
}
