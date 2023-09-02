#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <BCGCBProInc.h>
//#include "VtOutSignalDefManager.h"

// VtAddConnectSignalDlg dialog
class SmAccount;
class SmFund;
class SmSymbol;
class VtOutSignalDef;
class VtSignalConnectionGrid;
class VtAddConnectSignalDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(VtAddConnectSignalDlg)

public:
	VtAddConnectSignalDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtAddConnectSignalDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_SIG_CONNECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox _ComboAcnt;
	CComboBox _ComboSignal;
	CComboBox _ComboSymbol;
	CComboBox _ComboType;
	CEdit _EditSeungsu;
	CSpinButtonCtrl _SpinSeungsu;
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnCbnSelchangeComboAcnt();
	afx_msg void OnCbnSelchangeComboSymbol();
	afx_msg void OnBnClickedBtnFindSymbol();
	afx_msg void OnCbnSelchangeComboSignal();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnCancel();
	virtual BOOL OnInitDialog();
	int _Mode = 0;
	void InitCombo();
	void InitOutSigDefCombo();
	void SetSymbol(SmSymbol* sym);
	SmAccount* _Acnt = nullptr;
	SmSymbol* _Symbol = nullptr;
	SmFund* _Fund = nullptr;
	//SharedOutSigDef _Signal = nullptr;
	void SigConGrid(VtSignalConnectionGrid* val) { _SigConGrid = val; }
private:
	VtSignalConnectionGrid* _SigConGrid = nullptr;
};
