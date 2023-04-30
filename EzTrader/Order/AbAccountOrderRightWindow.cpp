// OrderRightDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "AbAccountOrderRightWindow.h"
#include "afxdialogex.h"


// OrderRightDialog dialog

IMPLEMENT_DYNAMIC(AbAccountOrderRightWindow, CBCGPDialog)

AbAccountOrderRightWindow::AbAccountOrderRightWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_RIGHT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

AbAccountOrderRightWindow::~AbAccountOrderRightWindow()
{
	//KillTimer(1);
}

void AbAccountOrderRightWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, _AssetArea);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_MASTER, _SymbolArea);
}


BEGIN_MESSAGE_MAP(AbAccountOrderRightWindow, CBCGPDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// OrderRightDialog message handlers


void AbAccountOrderRightWindow::SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account)
{
	_AssetArea.Account(account);
	_AssetArea.SetAssetInfo();
}

void AbAccountOrderRightWindow::SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol)
{
	_SymbolArea.Symbol(symbol);
}

void AbAccountOrderRightWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void AbAccountOrderRightWindow::OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	_SymbolArea.Symbol(symbol);
}

BOOL AbAccountOrderRightWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	_AssetArea.SetUp();
	_SymbolArea.SetUp();
	

	//SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void AbAccountOrderRightWindow::OnTimer(UINT_PTR nIDEvent)
{
	CBCGPDialog::OnTimer(nIDEvent);
}
