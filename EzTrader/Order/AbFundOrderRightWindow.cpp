// OrderRightDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "AbFundOrderRightWindow.h"
#include "afxdialogex.h"


// OrderRightDialog dialog

IMPLEMENT_DYNAMIC(AbFundOrderRightWindow, CBCGPDialog)

AbFundOrderRightWindow::AbFundOrderRightWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_RIGHT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

AbFundOrderRightWindow::~AbFundOrderRightWindow()
{
	//KillTimer(1);
}

void AbFundOrderRightWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, asset_view_);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_MASTER, symbol_info_view_);
}


BEGIN_MESSAGE_MAP(AbFundOrderRightWindow, CBCGPDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// OrderRightDialog message handlers


void AbFundOrderRightWindow::SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account)
{
	asset_view_.Account(account);
	asset_view_.SetAssetInfo();
}

void AbFundOrderRightWindow::SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol)
{
	symbol_info_view_.Symbol(symbol);
}

void AbFundOrderRightWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void AbFundOrderRightWindow::OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	symbol_info_view_.Symbol(symbol);
}

BOOL AbFundOrderRightWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	asset_view_.SetUp();
	symbol_info_view_.SetUp();
	

	//SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void AbFundOrderRightWindow::OnTimer(UINT_PTR nIDEvent)
{
	CBCGPDialog::OnTimer(nIDEvent);
}
