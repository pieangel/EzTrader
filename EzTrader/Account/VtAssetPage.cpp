// VtAssetPage.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "VtAssetPage.h"
#include "afxdialogex.h"
#include "../Account/SmAccountManager.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../MainFrm.h"
#include "../Task/SmTaskArg.h"
#include "../Task/SmTaskRequestManager.h"

// VtAssetPage dialog

IMPLEMENT_DYNAMIC(VtAssetPage, CDialog)

VtAssetPage::VtAssetPage(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_ASSET, pParent)
{
}

VtAssetPage::~VtAssetPage()
{
}

void VtAssetPage::SetAccount(std::string acntNo)
{
	
}

void VtAssetPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
	DDX_Control(pDX, IDC_BTN_COMMIT, _BtnCommit);
	DDX_Control(pDX, IDC_BTN_SEARCH, _BtnSearch);
	DDX_Control(pDX, IDC_EDIT_PWD, _EditPwd);
}


BEGIN_MESSAGE_MAP(VtAssetPage, CDialog)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &VtAssetPage::OnBnClickedBtnSearch)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &VtAssetPage::OnCbnSelchangeComboAccount)
	ON_BN_CLICKED(IDC_BTN_COMMIT, &VtAssetPage::OnBnClickedBtnCommit)
END_MESSAGE_MAP()


// VtAssetPage message handlers


void VtAssetPage::InitAccount()
{
	std::vector<std::shared_ptr<DarkHorse::SmAccount>> account_vector;
	mainApp.AcntMgr()->get_main_account_vector(account_vector);
	int selected_index = 0;
	for (auto it = account_vector.begin(); it != account_vector.end(); ++it) {
		auto account = *it;
		if (account->is_subaccount()) continue;
		std::string account_info;
		if (account->Type() == "9")
			account_info.append("[국내]");
		else
			account_info.append("[해외]");
		account_info.append(account->Name());
		account_info.append(" : ");
		account_info.append(account->No());
		const int index = _ComboAccount.AddString(account_info.c_str());
		_ComboAccountMap[index] = account;
		if (account_no_ == account->No()) {
			selected_index = index;
		}
	}

	if (!_ComboAccountMap.empty()) {
		_CurrentAccountIndex = selected_index;
		_ComboAccount.SetCurSel(_CurrentAccountIndex);
		//total_asset_profit_loss_view_.Account(_ComboAccountMap[_CurrentAccountIndex]);
	}
}

BOOL VtAssetPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	_BtnCommit.SetShade(SHS_HSHADE, 8, 20, 5, RGB(55, 55, 255));
	_BtnCommit.DrawFlatFocus(TRUE);

	_BtnSearch.SetShade(SHS_HSHADE, 8, 20, 5, RGB(55, 55, 255));
	_BtnSearch.DrawFlatFocus(TRUE);

	InitAccount();
	_AssetGrid.AssetPage(this);
	// TODO:  Add extra initialization here
	//_AssetGrid.AttachGrid(this, IDC_STATIC_ASSET);

	Init = true;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtAssetPage::OnBnClickedBtnSearch()
{
	if (_CurrentAccountIndex < 0) return;
	auto account = _ComboAccountMap[_CurrentAccountIndex];

	DhTaskArg arg;
	arg.detail_task_description = account->No();
	arg.task_type = DhTaskType::AccountProfitLoss;
	arg.parameter_map["account_no"] = account->No();
	arg.parameter_map["password"] = account->Pwd();
	arg.parameter_map["account_type"] = account->Type();

	mainApp.TaskReqMgr()->AddTask(std::move(arg));
}


void VtAssetPage::OnCbnSelchangeComboAccount()
{
	const int cur_sel = _ComboAccount.GetCurSel();
	if (cur_sel < 0) return;

	_CurrentAccountIndex = cur_sel;
	auto account = _ComboAccountMap[_CurrentAccountIndex];
	if (!account) return;
	account_no_ = account->No();
	type_ = account->Type();
	//total_asset_profit_loss_view_.Account(account);

	//total_asset_profit_loss_view_.SetAssetInfo();
	//total_asset_profit_loss_view_.Invalidate();
	DhTaskArg arg;
	arg.detail_task_description = account->No();
	arg.task_type = DhTaskType::AccountProfitLoss;
	arg.parameter_map["account_no"] = account->No();
	arg.parameter_map["password"] = account->Pwd();
	arg.parameter_map["account_type"] = account->Type();

	mainApp.TaskReqMgr()->AddTask(std::move(arg));
}

void VtAssetPage::OnReceiveAccountInfo()
{
	_AssetGrid.InitGrid();
}


void VtAssetPage::OnBnClickedBtnCommit()
{
// 	if (_Account && _Account->Enable() && _Account->hasValidPassword())
// 		_Account->GetAsset();
}
