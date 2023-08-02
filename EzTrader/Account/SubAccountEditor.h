#pragma once
#include "afxdialogex.h"
#include <BCGCBProInc.h>
#include "MainAccountGrid.h"
// SubAccountEditor dialog

class SubAccountEditor : public CBCGPScrollDialog
{
	DECLARE_DYNAMIC(SubAccountEditor)

public:
	SubAccountEditor(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SubAccountEditor();
	virtual BOOL OnInitDialog();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUB_ACCOUNT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	MainAccountGrid main_account_grid_;
};
