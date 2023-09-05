#pragma once

#include "afxwin.h"
#include <BCGCBProInc.h>
#include <memory>
#include "OutSystemView.h"
#include "ActiveOutSystemView.h"
#include "OutSystemDefView.h"
const int STD_GAP = 8;
const int STD_BUTTON_WIDTH = 90;
const int STD_BUTTON_HEIGHT = 25;
// VtAutoSignalManagerDialog dialog
const int RefTimer = 0x00000010;

namespace DarkHorse
{
	class SmOutSystem;
}
class VtAutoSignalManagerDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(VtAutoSignalManagerDialog)

public:
	VtAutoSignalManagerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtAutoSignalManagerDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYS_AUTO_CONNECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();

private:
	OutSystemView out_system_view_;
	ActiveOutSystemView active_out_system_view_;
	OutSystemDefView out_system_def_view_;
	void RefreshOrder();
	void Resize();
public:
	void add_active_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	void remove_active_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	void add_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	afx_msg void OnBnClickedBtnAddConnect();
	afx_msg void OnBnClickedBtnAddSignal();
	afx_msg void OnBnClickedBtnDelConnect();
	afx_msg void OnBnClickedBtnDelSignal();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnOrderConfig();
	afx_msg void OnBnClickedCheckAll();
	CButton _CheckAll;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};