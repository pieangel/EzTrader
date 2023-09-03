#pragma once
#include "AccountFundView.h"

// VtAccountFundSelector dialog
class VtUsdStrategyConfigDlg;
class VtSignalConnectionGrid;
class SmHftConfig;
class VtAccountFundSelector : public CBCGPDialog
{
	DECLARE_DYNAMIC(VtAccountFundSelector)

public:
	VtAccountFundSelector(CWnd* pParent = nullptr);   // standard constructor
	virtual ~VtAccountFundSelector();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACNT_FUND_SELECTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	AccountFundView account_fund_view_;

public:
	void set_mode(int mode) {
		account_fund_view_.mode(mode);
	}
	CString m_SelectedCode;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioAcnt();
	afx_msg void OnBnClickedRadioFund();
};
