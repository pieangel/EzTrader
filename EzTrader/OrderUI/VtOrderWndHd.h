#pragma once
#include <vector>
#include <memory>
#include "VtOrderLeftWndHd.h"
#include "VtOrderRightWndHd.h"
#include "afxwin.h"
#include "../ShadeButtonST.h"
#include "SmOrderPanel.h"
#include "../Xml/pugixml.hpp"
#include "../Fund/SmFund.h"
#include "../Account/SmAccount.h"

namespace DarkHorse {
	class SmFund;
	class SmSymbol;
	class SmAccount;
	struct SmPosition;

}

class VtOrderConfigManager;
class VtSymbol;
struct VtOrder;
struct VtPosition;
class VtAccount;
class VtFund;
class VtLayoutManager;
// 주문창 컨트롤 위치 정하기
// 제일 먼저 전체 크기를 정한다.
// 전체 크기로 늘린다.
// 왼쪽창과 오른쪽창, 중앙창의 위치와 크기를 정한다.
// 왼쪽창과 오른쪽창을 만든다.
// 컨트롤들을 이동시킨다.
// 중앙창을 개수만큼 만든다.
// 중앙창의 컨트롤들을 정렬시킨다.
// VtOrderWndHd dialog

using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;	
class VtOrderWndHd : public CDialog
{
	DECLARE_DYNAMIC(VtOrderWndHd)

public:
	VtOrderWndHd(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtOrderWndHd();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_WND_HD };
#endif

	// 함수 안에서는 객체를 바꾸지 않는다는 뜻
	//int CenterWndCount() const { return _CenterWndCount; }
	//void CenterWndCount(int val) { _CenterWndCount = val; }
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	account_p Account() const { return _Account; }
	void Account(account_p val) { _Account = val; }
	fund_p Fund() const { return _Fund; }
	void Fund(fund_p val) { _Fund = val; }
	int WindowHeight() const { return _WindowHeight; }
	void WindowHeight(int val) { _WindowHeight = val; }
	int DefaultHeight() const { return _DefaultHeight; }
	void DefaultHeight(int val) { _DefaultHeight = val; }
	bool EnableSizing() const { return _EnableSizing; }
	void EnableSizing(bool val) { _EnableSizing = val; }
	int GetCenterWndCount() {
		return _CenterWndVector.size();
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	static int _MaxWidth;
private:
	bool destroyed_ = false;
	void AddItemToComboBox(const int index, CComboBox& comboBox, account_p item)
	{
		// Add the std::shared_ptr to the vector
		combo_account_vector.push_back(item);
		std::string acntName = item->No();
		acntName.append(_T(":"));
		acntName.append(item->Name());
		// Add item to the combo box and store the index in the combo box
		//int index = comboBox.AddString(acntName.c_str()); // Replace with appropriate text
		comboBox.SetItemDataPtr(index, reinterpret_cast<void*>(combo_account_vector.size() - 1));
	}

	account_p GetSelectedAccountPtr(CComboBox& comboBox)
	{
		int index = comboBox.GetCurSel();
		if (index != CB_ERR)
		{
			// Retrieve the index from the combo box and get the std::shared_ptr
			size_t ptrIndex = reinterpret_cast<size_t>(comboBox.GetItemDataPtr(index));
			return combo_account_vector[ptrIndex];
		}
		return nullptr;
	}
	std::vector<account_p> combo_account_vector;


	void AddItemToComboBox(const int index, CComboBox& comboBox, fund_p item)
	{
		// Add the std::shared_ptr to the vector
		combo_fund_vector.push_back(item);

		// Add item to the combo box and store the index in the combo box
		//int index = comboBox.AddString(item->Name().c_str()); // Replace with appropriate text
		comboBox.SetItemDataPtr(index, reinterpret_cast<void*>(combo_fund_vector.size() - 1));
	}

	fund_p GetSelectedFundPtr(CComboBox& comboBox)
	{
		int index = comboBox.GetCurSel();
		if (index != CB_ERR)
		{
			// Retrieve the index from the combo box and get the std::shared_ptr
			size_t ptrIndex = reinterpret_cast<size_t>(comboBox.GetItemDataPtr(index));
			return combo_fund_vector[ptrIndex];
		}
		return nullptr;
	}

	std::vector<fund_p> combo_fund_vector;

	void RegisterRealtimeAccount(account_p acnt);
	void UnregisterRealtimeAccount(account_p acnt);
	account_p _Account = nullptr;
	fund_p _Fund = nullptr;
	VtOrderLeftWndHd _LeftWnd;
	VtOrderRightWndHd _RightWnd;
	std::vector<SmOrderPanel*> _CenterWndVector;
	//std::map<VtRealTickWnd*, VtRealTickWnd*> _RealTickWndVector;
	void Begin();
	void End();
	virtual BOOL OnInitDialog();
	void SetWindows();
	void InitSettings();
	bool _ShowLeftWnd = true;
	bool _ShowRightWnd = true;
	void RemoveRealTickWnd(VtRealTickWnd* realWnd);
private:
	void requestProfitLoss();
	//int  _CenterWndCount;
	int _WindowID;
	void CreateChildWindows();
	void CreateChildWindows(std::vector<CRect>& rcVec);
	void RemoveLastWindow();
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	void BlockEvent();
	void SetDefaultCenterWnd();
	void ResetByCenterRow();
	int ResizeWindow();
	void RepositionChildWindowsByReverse(int winWidth);
	void ResizeWindowByGridOption();
	void RefreshLayout(bool resize, bool recalGrid, bool forward = true);
	int GetTitleBarHeight();
	afx_msg void OnClose();
	void OnFundAdded();
	void OnFundDeleted(fund_p fund);
	bool InitFund();
	void InitAccount();
	void OnReceiveHoga(symbol_p sym);
	void OnReceiveQuote(symbol_p sym);

	void OnExpected(symbol_p sym);
	void OnReceiveAccountInfo();
	void OnReceiveMsg(CString msg);
	void OnReceiveMsgWithReqId(int id, CString msg);
	void GetOptionSymbolMaster();
	void OnOutstanding();
	void AddWindow();
	SmOrderPanel* AddWindow(CRect& rcWnd);
	void RemoveWindow();
	void RecalcLayout(bool fixed, int maxHeight = 0);
	void ResizeWnd(int maxHeight);
	void ShowLeftWnd();
	void ShowRightWnd();
	bool GetShowLeftWnd()
	{
		return _ShowLeftWnd;
	}

	bool GetShowRightWnd()
	{
		return _ShowRightWnd;
	}

	fund_p GetCurrentFund();
	void ShowHideCtrl();
public:
// 	BasicEvent<VtOrderWndHdEventArgs> _OrderWindowEvent;
// 	void FireOrderWindowEvent(VtOrderWndHdEventArgs&& arg)
// 	{
// 		_OrderWindowEvent(this, arg);
// 	}
	void SetActiveCenterWnd(SmOrderPanel* centerWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

private:
	bool _Sizing = false;
	void CreateChildWindow(SmOrderPanel* centerWnd, UINT id, CWnd* parent);
	void SetControlPositionsOnLeft(int left);
	void SetControlPositionsOnRight(int right);
	CRect GetLastCenterWindowPos();
	VtLayoutManager* _LayoutMgr = nullptr;
	void SaveControlPos();
	CRect GetClientArea(int resourceID);
	void ReposChildWindowsForward();
	void ReposChildWindowsBackward();
	std::vector<std::tuple<CWnd*, bool, CRect>> _ChildWindowInfo;
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	bool ClickedRightExtend = false;
	void OnSymbolMaster(symbol_p sym);
	void OnRemain(VtPosition* posi);
	void OnOrderAccepted(VtOrder* order);
	void OnOrderUnfilled(VtOrder* order);
	void OnOrderFilled(VtOrder* order);

	void ChangeAccount(account_p acnt);
	CShadeButtonST _BtnAddWnd;
	CShadeButtonST _BtnDelWnd;
	CShadeButtonST _BtnGetAcntInfo;
	CShadeButtonST _BtnShowLeft;
	CShadeButtonST _BtnShowRight;
	CComboBox _ComboAcnt;
	CStatic _StaticAcnt;
	CStatic _StaticAcntName;
	// Edit the funds.
	CShadeButtonST _BtnFundEditor;
	afx_msg void OnCbnSelchangeComboAccountHd();
	afx_msg void OnBnClickedBtnShowLeft();
	afx_msg void OnBnClickedBtnGetAcntInfo();
	afx_msg void OnBnClickedBtnDelwnd();
	afx_msg void OnBnClickedBtnAddwnd();
	afx_msg void OnBnClickedBtnShowright();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	void SetAccount(account_p acnt);
	void SetFund(fund_p fund);

	void SetType(int type);
	afx_msg void OnStnClickedStaticAcntName();
	afx_msg void OnStnClickedStaticAcnt();
	afx_msg void OnBnClickedBtnEditFund();

private:
	int _XPos = 0;
	int _YPos = 0;
	int _WindowWidth = 823;
	int _WindowHeight = 0;
	int _DefaultHeight = 1000;
	void CalcLayout();
	int GetTotalCenterWidth();
	void RepositionControl();
	void RecalChildWindows();
	void ShowCtrlByType();
	bool _EnableOnSizeEvent = false;
	bool _OnResizing = false;
	bool _Capturing = false;
	bool _EnableSizing = true;
	std::string _DefaultAccountNo;
	std::string _DefaultFundName;
	int _OldWndWidth = 0;
//public:
//	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	CEdit _EditPwd;
	afx_msg void OnEnChangeEditPwd();
	afx_msg void OnCbnDropdownComboAccountHd();
public:
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
	virtual void PostNcDestroy();
};
