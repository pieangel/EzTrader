#pragma once
#include "VtAccountAssetGrid.h"
#include "afxwin.h"
#include "../ShadeButtonST.h"
#include <string>
#include <map>
#include <memory>
// VtAssetPage dialog
namespace DarkHorse {
	class SmAccount;
}
class VtAssetPage : public CDialog
{
	DECLARE_DYNAMIC(VtAssetPage)

public:
	VtAssetPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtAssetPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ASSET };
#endif
	void SetAccount(std::string acntNo);
private:
	bool Init = false;
	VtAccountAssetGrid _AssetGrid;

	// key : combo index, value : account object.
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _ComboAccountMap;
	int _CurrentAccountIndex{ 0 };
	std::string account_no_;
	std::string type_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	std::string account_no() const { return account_no_; }
	void account_no(std::string val);
	std::string type() const { return type_; }
	void type(std::string val) { type_ = val; }
	void InitAccount();
	virtual BOOL OnInitDialog();
	CComboBox _ComboAccount;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnSearch();
	afx_msg void OnCbnSelchangeComboAccount();
	void OnReceiveAccountInfo();
	afx_msg void OnBnClickedBtnCommit();
	CShadeButtonST _BtnCommit;
	CShadeButtonST _BtnSearch;
	CEdit _EditPwd;
};
