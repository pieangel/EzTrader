#pragma once
#include <BCGPDialog.h>

// SmOrderSetDialog dialog
class SmOrderWnd;
class SmFundOrderCenterWnd;
class SmOrderSetDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmOrderSetDialog)

public:
	SmOrderSetDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmOrderSetDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_SET };
#endif

	void OrderWnd(SmOrderWnd* val) { _OrderWnd = val; }
	void FundWnd(SmFundOrderCenterWnd* val) { _FundWnd = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckSetWide();
private:
	SmOrderWnd* _OrderWnd = nullptr;
	SmFundOrderCenterWnd* _FundWnd = nullptr;
public:
	CBCGPButton _CheckAlignByAlt;
	CBCGPButton _CheckBarColor;
	CBCGPButton _CheckCancelByRightClick;
	CBCGPButton _CheckOrderBySpace;
	CBCGPButton _CheckSetWide;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckBarColor();
	afx_msg void OnBnClickedCheckAlignByAlt();
	afx_msg void OnBnClickedCheckOrderBySpace();
	afx_msg void OnBnClickedCheckCancelByRightClick();
};
