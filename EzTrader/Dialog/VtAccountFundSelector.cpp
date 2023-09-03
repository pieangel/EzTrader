// VtAccountFundSelector.cpp : implementation file
//

#include "stdafx.h"
#include "VtAccountFundSelector.h"
#include "afxdialogex.h"
#include "../resource.h"

// VtAccountFundSelector dialog

IMPLEMENT_DYNAMIC(VtAccountFundSelector, CBCGPDialog)

VtAccountFundSelector::VtAccountFundSelector(CWnd* pParent)
	: CBCGPDialog(IDD_ACNT_FUND_SELECTOR, pParent)
{
}


VtAccountFundSelector::~VtAccountFundSelector()
{
}

void VtAccountFundSelector::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtAccountFundSelector, CBCGPDialog)
	ON_BN_CLICKED(IDC_RADIO_ACNT, &VtAccountFundSelector::OnBnClickedRadioAcnt)
	ON_BN_CLICKED(IDC_RADIO_FUND, &VtAccountFundSelector::OnBnClickedRadioFund)
END_MESSAGE_MAP()


// VtAccountFundSelector message handlers


BOOL VtAccountFundSelector::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	((CButton*)GetDlgItem(IDC_RADIO_ACNT))->SetCheck(BST_CHECKED);
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_ACNT_FUND_GRID);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	// Create the Windows control and attach it to the Grid object
	account_fund_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, 0x0023);

	account_fund_view_.mode(0);
	account_fund_view_.init_grid();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtAccountFundSelector::OnBnClickedRadioAcnt()
{
	account_fund_view_.mode(0);
	account_fund_view_.init_grid();
}


void VtAccountFundSelector::OnBnClickedRadioFund()
{
	account_fund_view_.mode(1);
	account_fund_view_.init_grid();
}
