// SubAccountEditor.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "afxdialogex.h"
#include "SubAccountEditor.h"

#define WND_ID8 0x00000008
#define WND_ID9 0x00000009
// SubAccountEditor dialog

IMPLEMENT_DYNAMIC(SubAccountEditor, CBCGPScrollDialog)

SubAccountEditor::SubAccountEditor(CWnd* pParent /*=nullptr*/)
	: CBCGPScrollDialog(IDD_SUB_ACCOUNT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SubAccountEditor::~SubAccountEditor()
{
}

BOOL SubAccountEditor::OnInitDialog()
{
	CBCGPScrollDialog::OnInitDialog();


	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_ACCOUNT_LIST);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	main_account_grid_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID8);
	main_account_grid_.StartTimer();
	main_account_grid_.SetAccountList();

	pWnd = GetDlgItem(IDC_SUB_ACCOUNT_LIST);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	//_FundGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID9);
	//_FundGrid.StartTimer();

	//_FundGrid.SetWholeRowSel(TRUE);

	//_FundGrid.SetFundDialog(this);
	main_account_grid_.SetWholeRowSel(TRUE);

	return TRUE;
}

void SubAccountEditor::DoDataExchange(CDataExchange* pDX)
{
	CBCGPScrollDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SubAccountEditor, CBCGPScrollDialog)
END_MESSAGE_MAP()


// SubAccountEditor message handlers
