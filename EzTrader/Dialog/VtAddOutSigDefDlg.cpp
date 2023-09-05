// VtAddOutSigDefDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "VtAddOutSigDefDlg.h"
#include "afxdialogex.h"

#include <memory>
#include <algorithm>


// VtAddOutSigDefDlg dialog

IMPLEMENT_DYNAMIC(VtAddOutSigDefDlg, CBCGPDialog)

VtAddOutSigDefDlg::VtAddOutSigDefDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_ADD_SIG_DEF, pParent)
{

}

VtAddOutSigDefDlg::~VtAddOutSigDefDlg()
{
}

void VtAddOutSigDefDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SIGNAL, _EditSignal);
}


BEGIN_MESSAGE_MAP(VtAddOutSigDefDlg, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &VtAddOutSigDefDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// VtAddOutSigDefDlg message handlers


void VtAddOutSigDefDlg::OnBnClickedOk()
{
// 	VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
// 	SharedOutSigDef sig = std::make_shared<VtOutSignalDef>();
// 	CString strDesc, strSymbol, strSignal, strStrategy;
// 	_EditSignal.GetWindowText(strSignal);
// 	sig->SignalName = (LPCTSTR)strSignal;
// 	sig->SymbolCode = (LPCTSTR)strSymbol;
// 	sig->StrategyName = (LPCTSTR)strStrategy;
// 	sig->Desc = (LPCTSTR)strDesc;
// 	outSigDefMgr->AddOutSignalDef(sig);
// 	if (_SigDefGrid) _SigDefGrid->AddOutSigDef(sig);
	CBCGPDialog::OnOK();
}