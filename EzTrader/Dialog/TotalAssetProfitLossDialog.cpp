// TotalAssetProfitLossDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "TotalAssetProfitLossDialog.h"
#include "afxdialogex.h"
#include <unordered_map>
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccountManager.h"
#include "../MainFrm.h"


// TotalAssetProfitLossDialog dialog

IMPLEMENT_DYNAMIC(TotalAssetProfitLossDialog, CBCGPDialog)

TotalAssetProfitLossDialog::TotalAssetProfitLossDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_TOTAL_ASSET, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

TotalAssetProfitLossDialog::~TotalAssetProfitLossDialog()
{
}

void TotalAssetProfitLossDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ASSET, total_asset_profit_loss_view_);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
}


BEGIN_MESSAGE_MAP(TotalAssetProfitLossDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_GET, &TotalAssetProfitLossDialog::OnBnClickedBtnGet)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &TotalAssetProfitLossDialog::OnCbnSelchangeComboAccount)
END_MESSAGE_MAP()


// TotalAssetProfitLossDialog message handlers


void TotalAssetProfitLossDialog::SetAccount()
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
		total_asset_profit_loss_view_.Account(_ComboAccountMap[_CurrentAccountIndex]);
	}
}

BOOL TotalAssetProfitLossDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	SetAccount();
	total_asset_profit_loss_view_.SetUp();
	total_asset_profit_loss_view_.SetAssetInfo();
	total_asset_profit_loss_view_.Invalidate();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void TotalAssetProfitLossDialog::OnBnClickedBtnGet()
{
	// TODO: Add your control notification handler code here
}


void TotalAssetProfitLossDialog::OnCbnSelchangeComboAccount()
{
	const int cur_sel = _ComboAccount.GetCurSel();
	if (cur_sel < 0) return;

	_CurrentAccountIndex = cur_sel;
	total_asset_profit_loss_view_.Account(_ComboAccountMap[_CurrentAccountIndex]);

	total_asset_profit_loss_view_.SetAssetInfo();
	total_asset_profit_loss_view_.Invalidate();
}


void TotalAssetProfitLossDialog::PostNcDestroy()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->RemoveAssetWnd(GetSafeHwnd());

	CBCGPDialog::PostNcDestroy();
}