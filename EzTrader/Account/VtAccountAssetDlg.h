#pragma once
#include "afxcmn.h"
#include "VtAssetPage.h"
#include "VtOrderAvailablePage.h"
#include "../Xml/pugixml.hpp"

// VtAccountAssetDlg dialog

class VtAccountAssetDlg : public CDialog
{
	DECLARE_DYNAMIC(VtAccountAssetDlg)

public:
	VtAccountAssetDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtAccountAssetDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACCOUNT_ASSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);

	VtAssetPage _AssetPage;
	VtOrderAvailablePage _OrderPage;
	CWnd* _CurrentPage = nullptr;
	void InitTabCtrl();
	CTabCtrl _TabCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTabAccountAsset(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	void OnReceiveAccountInfo();
	void SetAccount(std::string acntNo);

	// �ü��� ������ �� ���޵Ǵ� �޽���
	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);
	// �ֹ��� ������ �� ���޵Ǵ� �޽���
	afx_msg LRESULT OnOrderChangedMessage(WPARAM wParam, LPARAM lParam);

	void RegisterAccountCallback();
	void UnregisterAccountCallback();
	void OnAccountEvent(VtAccount* account);

	afx_msg LRESULT OnAccountChangedMessage(WPARAM wParam, LPARAM lParam);
};
