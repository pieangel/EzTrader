//#include "stdafx.h"
//#include "DmFundOrderRightWindow.h"

// SmFundOrderRight.cpp : implementation file
//

#include "stdafx.h"
#include "../../DarkHorse.h"
#include "DmFundOrderRightWindow.h"
#include "afxdialogex.h"


// DmFundOrderRightWindow dialog

IMPLEMENT_DYNAMIC(DmFundOrderRightWindow, CBCGPDialog)

DmFundOrderRightWindow::DmFundOrderRightWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_RIGHT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

DmFundOrderRightWindow::~DmFundOrderRightWindow()
{
	//KillTimer(1);
}

void DmFundOrderRightWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, _AssetArea);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_MASTER, _SymbolArea);
}


BEGIN_MESSAGE_MAP(DmFundOrderRightWindow, CBCGPDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// DmFundOrderRightWindow message handlers


void DmFundOrderRightWindow::SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account)
{
	_AssetArea.Account(account);
	_AssetArea.SetAssetInfo();
}

void DmFundOrderRightWindow::SetFund(const std::shared_ptr<DarkHorse::SmFund>& fund)
{
	_AssetArea.Fund(fund);
	_AssetArea.SetAssetInfo();
}

void DmFundOrderRightWindow::SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol)
{
	_SymbolArea.Symbol(symbol);
}

void DmFundOrderRightWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void DmFundOrderRightWindow::OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	_SymbolArea.Symbol(symbol);
}

BOOL DmFundOrderRightWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	_AssetArea.SetUp();
	_SymbolArea.SetUp();
	_AssetArea.Mode(1);

	//SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void DmFundOrderRightWindow::OnTimer(UINT_PTR nIDEvent)
{
	_AssetArea.SetAssetInfo();
	_SymbolArea.UpdateSymbolInfo();

	CBCGPDialog::OnTimer(nIDEvent);
}

