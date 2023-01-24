// SmOrderSetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmOrderSetDialog.h"
#include "afxdialogex.h"
#include "SmOrderWnd.h"
#include "../Fund/SmFundOrderCenterWnd.h"

// SmOrderSetDialog dialog

IMPLEMENT_DYNAMIC(SmOrderSetDialog, CBCGPDialog)

SmOrderSetDialog::SmOrderSetDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_SET, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
}

SmOrderSetDialog::~SmOrderSetDialog()
{
}

void SmOrderSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ALIGN_BY_ALT, _CheckAlignByAlt);
	DDX_Control(pDX, IDC_CHECK_BAR_COLOR, _CheckBarColor);
	DDX_Control(pDX, IDC_CHECK_CANCEL_BY_RIGHT_CLICK, _CheckCancelByRightClick);
	DDX_Control(pDX, IDC_CHECK_ORDER_BY_SPACE, _CheckOrderBySpace);
	DDX_Control(pDX, IDC_CHECK_SET_WIDE, _CheckSetWide);
}


BEGIN_MESSAGE_MAP(SmOrderSetDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_CHECK_SET_WIDE, &SmOrderSetDialog::OnBnClickedCheckSetWide)
	ON_BN_CLICKED(IDC_CHECK_BAR_COLOR, &SmOrderSetDialog::OnBnClickedCheckBarColor)
	ON_BN_CLICKED(IDC_CHECK_ALIGN_BY_ALT, &SmOrderSetDialog::OnBnClickedCheckAlignByAlt)
	ON_BN_CLICKED(IDC_CHECK_ORDER_BY_SPACE, &SmOrderSetDialog::OnBnClickedCheckOrderBySpace)
	ON_BN_CLICKED(IDC_CHECK_CANCEL_BY_RIGHT_CLICK, &SmOrderSetDialog::OnBnClickedCheckCancelByRightClick)
END_MESSAGE_MAP()


// SmOrderSetDialog message handlers


void SmOrderSetDialog::OnBnClickedCheckSetWide()
{
	if (_OrderWnd) {
		if (_CheckSetWide.GetCheck() == BST_CHECKED)
			_OrderWnd->SetRowWide();
		else
			_OrderWnd->SetRowNarrow();
	}

	if (_FundWnd) {
		if (_CheckSetWide.GetCheck() == BST_CHECKED)
			_FundWnd->SetRowWide();
		else
			_FundWnd->SetRowNarrow();
	}
}


BOOL SmOrderSetDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	_CheckAlignByAlt.SetCheck(BST_CHECKED);
	_CheckBarColor.SetCheck(BST_CHECKED);
	_CheckCancelByRightClick.SetCheck(BST_CHECKED);
	_CheckOrderBySpace.SetCheck(BST_CHECKED);
	_CheckSetWide.SetCheck(BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmOrderSetDialog::OnBnClickedCheckBarColor()
{
	// TODO: Add your control notification handler code here
}


void SmOrderSetDialog::OnBnClickedCheckAlignByAlt()
{
	// TODO: Add your control notification handler code here
}


void SmOrderSetDialog::OnBnClickedCheckOrderBySpace()
{
	// TODO: Add your control notification handler code here
}


void SmOrderSetDialog::OnBnClickedCheckCancelByRightClick()
{
	// TODO: Add your control notification handler code here
}
