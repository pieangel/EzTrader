// HdOptionPage.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "HdOptionPage.h"
#include "afxdialogex.h"
#include "HdSymbolSelecter.h"
#include "../Global/SmTotalManager.h"

// HdOptionPage dialog
//extern TApplicationFont g_Font;

IMPLEMENT_DYNAMIC(HdOptionPage, CBCGPDialog)

HdOptionPage::HdOptionPage(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_OPT_PAGE, pParent)
{
	_SymSelecter = nullptr;
}

HdOptionPage::~HdOptionPage()
{
}

void HdOptionPage::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_OPT_MARKET, _ComboOptionMarket);
	DDX_Control(pDX, IDC_COMBO_OPT_MONTH, _ComboOptionMonth);
	DDX_Control(pDX, IDC_STATIC_OPTION, option_view_);
}


BEGIN_MESSAGE_MAP(HdOptionPage, CBCGPDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_OPT_MARKET, &HdOptionPage::OnCbnSelchangeComboOptMarket)
	ON_CBN_SELCHANGE(IDC_COMBO_OPT_MONTH, &HdOptionPage::OnCbnSelchangeComboOptMonth)
END_MESSAGE_MAP()


// HdOptionPage message handlers


BOOL HdOptionPage::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	//::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());

	//_OptionGrid.OptionPage(this);
	// TODO:  Add extra initialization here
	//_OptionGrid.AttachGrid(this, IDC_STATIC_OPT_GRID);
	//_OptionGrid.SymSelecter(_SymSelecter);
	//if (_SymSelecter && _SymSelecter->SelTab == 1)
	//	_OptionGrid.GetSymbolMasterLately();

	option_view_.SetUp();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void HdOptionPage::OnCbnSelchangeComboOptMarket()
{
	// TODO: Add your control notification handler code here
	//_OptionGrid.SetProductSection();
	//_OptionGrid.SetYearMonth();
	//if (_SymSelecter && _SymSelecter->SelTab == 1)
	//	_OptionGrid.GetSymbolMasterLately();
	//_OptionGrid.InitGrid();
}


void HdOptionPage::OnCbnSelchangeComboOptMonth()
{
	// TODO: Add your control notification handler code here
	//if (_SymSelecter && _SymSelecter->SelTab == 1)
	//	_OptionGrid.GetSymbolMasterLately();
	//_OptionGrid.InitGrid();
}

void HdOptionPage::GetSymbolMaster()
{
	//_OptionGrid.GetSymbolMasterLately();
}

void HdOptionPage::SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg)
{
	//_OptionGrid.SetConfigDlg(ConfigDlg);
}

void HdOptionPage::SetAddConSigDlg(VtAddConnectSignalDlg* conSigDlg)
{
	//_OptionGrid.SetAddConSigDlg(conSigDlg);
}

void HdOptionPage::SetSigConGrid(VtSignalConnectionGrid* sigConGrid)
{
	//_OptionGrid.SetSigConGrid(sigConGrid);
}

void HdOptionPage::SetChartTimeToolBar(VtChartTimeToolBar* timeToolBar)
{
	//_OptionGrid.SetChartTimeToolBar(timeToolBar);
}

void HdOptionPage::SetHftConfig(SmHftConfig* hftConfig)
{
	//_OptionGrid.SetHftConfig(hftConfig);
}
