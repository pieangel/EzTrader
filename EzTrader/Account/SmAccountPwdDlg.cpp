// SmAccountPwdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmAccountPwdDlg.h"
#include "afxdialogex.h"
#include "../Grid/customcells.h"
#include "SmAccount.h"
#include "SmAccountManager.h"
#include "../Global/SmTotalManager.h"
#include <unordered_map>
#include <memory>
#include "../Event/SmCallbackManager.h"
#include <BCGCBProInc.h>
#include "../MessageDefine.h"
#include "../Task/SmTaskRequestMaker.h"
#include "../Client/ViClient.h"
#include "../Archieve/SmSaveManager.h"

using namespace DarkHorse;

// SmAccountPwdDlg dialog

IMPLEMENT_DYNAMIC(SmAccountPwdDlg, CBCGPDialog)

SmAccountPwdDlg::SmAccountPwdDlg(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ACCOUNT_PWD, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);

	
}

SmAccountPwdDlg::~SmAccountPwdDlg()
{
	mainApp.CallbackMgr()->UnsubscribePasswordCallback(GetSafeHwnd());
	//KillTimer(1);
}

void SmAccountPwdDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SmAccountPwdDlg, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_SAVE, &SmAccountPwdDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &SmAccountPwdDlg::OnBnClickedBtnClose)
	ON_MESSAGE(WM_PASSWORD_CONFIRMED, &SmAccountPwdDlg::OnUmPasswordConfirmed)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// SmAccountPwdDlg message handlers


void SmAccountPwdDlg::SavePassword()
{
	for (auto it = _RowToAccountMap.begin(); it != _RowToAccountMap.end(); ++it) {
		const int row = it->first;
		CBCGPGridRow* pRow = m_wndGrid.GetRow(row);
		CString strValue = pRow->GetItem(2)->GetValue();
		it->second->Pwd(static_cast<const char*>(strValue));
	}

	mainApp.SaveMgr()->SaveAccountPasswords(_RowToAccountMap);
}

BOOL SmAccountPwdDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableLayout();
	
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_PWD);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	m_wndGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, IDC_STATIC_PWD);


	m_wndGrid.EnableMarkSortedColumn(FALSE);
	m_wndGrid.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);
	m_wndGrid.EnableRowHeader(TRUE);
	//m_wndGrid.EnableLineNumbers();
	m_wndGrid.SetClearInplaceEditOnEnter(FALSE);
	m_wndGrid.EnableInvertSelOnCtrl();
	m_wndGrid.SetWholeRowSel(FALSE);
	
	m_wndGrid.SetRowHeaderWidth(1);
	m_wndGrid.SetVisualManagerColorTheme(TRUE);
	
	// Insert columns:
	m_wndGrid.InsertColumn(0, _T("??????"), 80);
	m_wndGrid.InsertColumn(1, _T("????????"), 80);
	m_wndGrid.InsertColumn(2, _T("????????"), 80);
	m_wndGrid.InsertColumn(3, _T("????????"), rect.Width() - 246);

	m_wndGrid.FreezeColumns(0);
	
	const int nColumns = m_wndGrid.GetColumnCount();

	const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();

	mainApp.SaveMgr()->LoadAccountPasswords();

	int row = 0;
	for (auto it = account_map.begin(); it != account_map.end(); it++) {
		auto account = it->second;
		// Create new row:
		CBCGPGridRow* pRow = m_wndGrid.CreateRow(nColumns);
		// Set each column data:
		for (int nColumn = 0; nColumn < nColumns; nColumn++)
		{
			
			pRow->GetItem(0)->SetValue(account->Name().c_str(), TRUE);
			pRow->GetItem(1)->SetValue(account->No().c_str(), TRUE);
			
			if (nColumn == 2)
				pRow->ReplaceItem(nColumn, new CPasswordItem(account->Pwd().c_str()));
			if (nColumn == 0 || nColumn == 1)
				pRow->GetItem(nColumn)->AllowEdit(FALSE);

			_RowToAccountMap[row] = account;
		}

		// Add row to grid:
		m_wndGrid.AddRow(pRow, FALSE /* Don't recal. layout */);

		row++;
	}
	
	m_wndGrid.EnableCheckBoxes(TRUE);
	m_wndGrid.SetHeaderAlign(0, 2);
	m_wndGrid.SetHeaderAlign(1, 2);
	m_wndGrid.SetHeaderAlign(2, 2);
	m_wndGrid.SetHeaderAlign(3, 2);
	//m_wndGrid.SetCheckBoxState(BST_CHECKED);

	m_wndGrid.SetColumnAlign(0, 2);
	m_wndGrid.SetColumnAlign(1, 2);
	m_wndGrid.SetColumnAlign(2, 2);
	m_wndGrid.SetColumnAlign(3, 2);
	
	m_wndGrid.CheckAll(TRUE);
	m_wndGrid.UpdateHeaderCheckbox();
	m_wndGrid.AdjustLayout();
	
	mainApp.CallbackMgr()->SubscribePasswordCallback(GetSafeHwnd());


	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
// fe, 2f, 19

void SmAccountPwdDlg::OnBnClickedBtnSave()
{
	for (auto it = _RowToAccountMap.begin(); it != _RowToAccountMap.end(); ++it) {
		const int row = it->first;
		CBCGPGridRow* pRow = m_wndGrid.GetRow(row);
		CString strValue = pRow->GetItem(1)->GetValue();
		
		std::string account_no, pwd;
		account_no = static_cast<const char*>(strValue);
		strValue = pRow->GetItem(2)->GetValue();
		pwd = static_cast<const char*>(strValue);
		_ReqQ.push_back(std::make_pair(account_no, pwd));

		std::shared_ptr<SmAccount> account = mainApp.AcntMgr()->FindAccount(account_no);
		if (!account) continue;
		account->Pwd(pwd);
		//task_arg arg = DarkHorse::SmTaskRequestMaker::MakeAccountAssetRequest(account_no, pwd);
		//mainApp.Client()->CheckAccountPassword(std::move(arg));
	}
	
	//CBCGPDialog::EndDialog(IDOK);
	/*
	mainApp.AcntMgr()->FindAccount(account_no);
	auto account_pwd = _ReqQ.front();
	task_arg arg = DarkHorse::SmTaskRequestMaker::MakeAccountAssetRequest(account_pwd.first, account_pwd.second);
	mainApp.Client()->CheckAccountPassword(std::move(arg));
	_ReqQ.pop_front();
	*/

	SetTimer(1, 700, NULL);
}


void SmAccountPwdDlg::OnBnClickedBtnClose()
{
	for (auto it = _RowToAccountMap.begin(); it != _RowToAccountMap.end(); ++it) {
		const int row = it->first;
		CBCGPGridRow* pRow = m_wndGrid.GetRow(row);
		CString strValue = pRow->GetItem(3)->GetValue();

		if (strValue.GetLength() == 0 || strValue == "X") {
			AfxMessageBox("???? ?????????? ???????? ????????!");
			return;
		}
	}

	SavePassword();
	CBCGPDialog::EndDialog(IDOK);
}

LRESULT SmAccountPwdDlg::OnUmPasswordConfirmed(WPARAM wParam, LPARAM lParam)
{
	const int account_id = static_cast<int>(wParam);
	const int result = static_cast<int>(lParam);
	for (auto it = _RowToAccountMap.begin(); it != _RowToAccountMap.end(); ++it) {
		auto account = it->second;
		if (account->Id() == account_id) {
			CBCGPGridRow* pRow = m_wndGrid.GetRow(it->first);
			if (result == 1)
				pRow->GetItem(3)->SetValue("O");
			else
				pRow->GetItem(3)->SetValue("X");
		}
	}
	return 1;
}


void SmAccountPwdDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (_ReqQ.empty()) { KillTimer(1); }
	else {
		auto account_pwd = _ReqQ.front();

		std::shared_ptr<SmAccount> account = mainApp.AcntMgr()->FindAccount(account_pwd.first);
		if (!account) return;
				
		task_arg arg = DarkHorse::SmTaskRequestMaker::MakeAccountAssetRequest(account_pwd.first, account_pwd.second, account->Type());
		//if (account->Type() == "1")
			mainApp.Client()->CheckAccountPassword(std::move(arg));
		//else if (account->Type() == "9")
		//	mainApp.Client()->CheckDmAccountPassword(std::move(arg));
		_ReqQ.pop_front();
	}
	CBCGPDialog::OnTimer(nIDEvent);
}


void SmAccountPwdDlg::OnDestroy()
{
	CBCGPDialog::OnDestroy();

	KillTimer(1);
}
