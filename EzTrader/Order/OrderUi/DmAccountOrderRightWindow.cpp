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
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, asset_veiw_);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_MASTER, symbol_info_view_);
}


BEGIN_MESSAGE_MAP(DmAccountOrderRightWindow, CBCGPDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// DmAccountOrderRightWindow message handlers


void DmAccountOrderRightWindow::SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account)
{
	asset_veiw_.Account(account);
	asset_veiw_.SetAssetInfo();
}

void DmAccountOrderRightWindow::SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol)
{
	symbol_info_view_.Symbol(symbol);
}

void DmAccountOrderRightWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void DmAccountOrderRightWindow::OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	symbol_info_view_.Symbol(symbol);
}

BOOL DmAccountOrderRightWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	asset_veiw_.SetUp();
	symbol_info_view_.SetUp();


	//SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void DmAccountOrderRightWindow::OnTimer(UINT_PTR nIDEvent)
{
	CBCGPDialog::OnTimer(nIDEvent);
}
