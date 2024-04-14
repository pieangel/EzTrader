#pragma once
#include "HdAccountPLGrid.h"
#include "HdProductRemainGrid.h"
#include "afxwin.h"
#include "../Xml/pugixml.hpp"
#include <string>
#include <unordered_map>
#include <memory>
// HdAccountPLDlg dialog
namespace DarkHorse {
	;
}

class HdAccountPLDlg : public CDialog
{
	DECLARE_DYNAMIC(HdAccountPLDlg)

public:
	HdAccountPLDlg(CWnd* pParent = NULL);   // standard constructor
	HdAccountPLDlg(CWnd* pParent, std::string type);   // standard constructor
	HdAccountPLDlg(CWnd* pParent, const std::string& type, const int mode);
	HdAccountPLDlg(CWnd* pParent, const std::string& type, const int mode, const std::string& target);
	virtual ~HdAccountPLDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MINI_JANGO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	std::string account_no() const { return account_no_; }
	void account_no(std::string val) { account_no_ = val; }
	std::string fund_name() const { return fund_name_; }
	void fund_name(std::string val) { fund_name_ = val; }
	std::string Type() const { return type_; }
	void Type(std::string val) { type_ = val; }

private:
	HdAccountPLGrid _AccountGrid;
	HdProductRemainGrid _ProductGrid;
	void SetAccount();
	void SetFund();
	std::string account_no_;
	std::string fund_name_;
	// key : combo index, value : account object.
	std::unordered_map<int, std::string> _ComboAccountMap;
	std::unordered_map<int, std::string> _ComboFundMap;
	int _CurrentAccountIndex{ 0 };
	// 0 : account, 1 : fund
	int _Mode = 0;
	// "1" : 해외, "9" : 국내 
	std::string type_;

public:
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);


	std::string _DefaultAccount;
	CComboBox _ComboAccount;
	void InitAccount();
	void SetDefaultAccount();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
// 	void OnReceiveQuote(VtSymbol* sym);
// 	void OnOrderFilledHd(VtOrder* order);
// 	void OnSymbolMaster(VtSymbol* sym);
	void OnReceiveAccountInfo();
	afx_msg void OnCbnSelchangeComboAccount();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnDropdownComboAccount();

	// 시세가 변했을 때 전달되는 메시지
	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);
	// 주문이 변했을 때 전달되는 메시지
	afx_msg LRESULT OnOrderChangedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAccountChangedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
