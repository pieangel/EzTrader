#include "stdafx.h"
#include "DmFundOrderRightWindow.h"
#include "../../DarkHorse.h"
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
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, asset_view_);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_MASTER, symbol_info_view_);
}


BEGIN_MESSAGE_MAP(DmFundOrderRightWindow, CBCGPDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// DmFundOrderRightWindow message handlers


void DmFundOrderRightWindow::SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account)
{
	asset_view_.Account(account);
	asset_view_.SetAssetInfo();
}

void DmFundOrderRightWindow::SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol)
{
	symbol_info_view_.Symbol(symbol);
}

void DmFundOrderRightWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void DmFundOrderRightWindow::OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	symbol_info_view_.Symbol(symbol);
}

BOOL DmFundOrderRightWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	asset_view_.SetUp();
	symbol_info_view_.SetUp();


	//SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void DmFundOrderRightWindow::OnTimer(UINT_PTR nIDEvent)
{
	CBCGPDialog::OnTimer(nIDEvent);
}
