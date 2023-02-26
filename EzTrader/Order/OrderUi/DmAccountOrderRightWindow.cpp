#include "stdafx.h"
#include "DmAccountOrderRightWindow.h"
#include "../../DarkHorse.h"
#include "afxdialogex.h"


// DmAccountOrderRightWindow dialog

IMPLEMENT_DYNAMIC(DmAccountOrderRightWindow, CBCGPDialog)

DmAccountOrderRightWindow::DmAccountOrderRightWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_RIGHT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

DmAccountOrderRightWindow::~DmAccountOrderRightWindow()
{
	//KillTimer(1);
}

void DmAccountOrderRightWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, _AssetArea);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_MASTER, _SymbolArea);
}


BEGIN_MESSAGE_MAP(DmAccountOrderRightWindow, CBCGPDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// DmAccountOrderRightWindow message handlers


void DmAccountOrderRightWindow::SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account)
{
	_AssetArea.Account(account);
	_AssetArea.SetAssetInfo();
}

void DmAccountOrderRightWindow::SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol)
{
	_SymbolArea.Symbol(symbol);
}

void DmAccountOrderRightWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void DmAccountOrderRightWindow::OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	_SymbolArea.Symbol(symbol);
}

BOOL DmAccountOrderRightWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	_AssetArea.SetUp();
	_SymbolArea.SetUp();


	//SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void DmAccountOrderRightWindow::OnTimer(UINT_PTR nIDEvent)
{
	_AssetArea.SetAssetInfo();
	_SymbolArea.UpdateSymbolInfo();

	CBCGPDialog::OnTimer(nIDEvent);
}
