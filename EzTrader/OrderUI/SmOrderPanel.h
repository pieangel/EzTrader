#pragma once
#include "SmOrderGrid.h"
#include "VtProductRemainGrid.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
#include "../BtnST.h"
#include "VtEditCtrl.h"
#include "VtCutManager.h"
#include "VtConfigGrid.h"
#include "../ShadeButtonST.h"
#include "../Util/GradientStatic.h"
#include <thread>
#include <memory>
#include "SmTickGrid.h"
#include "../Xml/pugixml.hpp"
#include "../VtDefine.h"
#include "../Time/cpptime.h"
#include "../Symbol/SmSymbol.h"
#include "../View/SymbolOrderView.h"
#include "../View/SymbolTickView.h"

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}

using symbol_p = std::shared_ptr<DarkHorse::SmSymbol>;
using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;
// SmOrderPanel dialog
class VtOrderWndHd;
class VtOrderConfigManager;
class VtSymbol;
class VtAccount;
class VtOrderManager;
class VtProductOrderManager;
class VtRealTickWnd;
class VtOrderConfigDlg;
class VtOrderGridConfig;
class VtFund;
class VtCutManager;
class VtLayoutManager;

class VtRefreshManager;

// 국내 선물을 위한 주문창
class SmOrderPanel : public CDialog
{
	DECLARE_DYNAMIC(SmOrderPanel)

public:
	SmOrderPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~SmOrderPanel();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_PANEL };
#endif

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val);
	VtFilledCondition FillCondition() const { return _FillCondition; }
	void FillCondition(VtFilledCondition val) { _FillCondition = val; }
	VtPriceType PriceType() const { return _PriceType; }
	void PriceType(VtPriceType val) { _PriceType = val; }
	symbol_p Symbol() const { return _Symbol; }
	void Symbol(symbol_p val) { _Symbol = val; }
	int StopVal() const { return _StopVal; }
	void StopVal(int val) { _StopVal = val; }
	int TickWndPos() const { return _TickWndPos; }
	void TickWndPos(int val) { _TickWndPos = val; }
	bool ShowTickWnd() const { return _ShowTickWnd; }
	void ShowTickWnd(bool val) { _ShowTickWnd = val; }
	bool ShowRemainConfig() const { return _ShowRemainConfig; }
	void ShowRemainConfig(bool val) { _ShowRemainConfig = val; }
	bool UseHogaSiseFilter() const { return _UseHogaSiseFilter; }
	void UseHogaSiseFilter(bool val) { _UseHogaSiseFilter = val; }
	void BlockEvent();
	int OrderAmount() const { return _OrderAmount; }
	void OrderAmount(int val) { _OrderAmount = val; }
	bool FixedCenter() const { return _FixedCenter; }
	void FixedCenter(bool val);
	bool OrderByRemain() const { return _OrderByRemain; }
	void OrderByRemain(bool val) { _OrderByRemain = val; }
	bool ShowOrderCountArea() const { return _ShowOrderCountArea; }
	void ShowOrderCountArea(bool val) { _ShowOrderCountArea = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void OnEntered();
	void OnSymbolMaster(symbol_p sym);
	void InitSymbol();
	void SetSymbol(symbol_p sym);
	void AddSymbolToCombo(symbol_p symbol);
	void SetProductName(symbol_p symbol);
	void InitPosition();
	void RegisterRealtimeSymbol();
	void UnregisterRealtimeSymbol();
	void RegisterRealtimeAccount();
	void UnregisterRealtimeAccount();
	virtual BOOL OnInitDialog();
	SymbolOrderView m_Grid;
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	void set_symbol_from_out(const int order_window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol);
	//void OnTimer2(Timer& timer);
	void InitGridInfo();
	void SetSymbol();
	void ClearPosition();
	void SetTickCount(int count);
	int GetTickCount();
	int GetOrderCellWidth()
	{
		return m_Grid.OrderWidth();
	}

	int GetCellHeight()
	{
		return m_Grid.CellHeight();
	}
	const SymbolOrderView& GetOrderPanelGrid()
	{
		return m_Grid;
	}
	std::vector<bool>& GetGridColOptions() {
		return m_Grid.OrderGridColOption();
	}
	void SetOrderArea(int height, int width);
	void ShowOrderCountInGrid(bool flag);
	void SetShowPLConfigWnd(bool flag);
	void InitAll();
	void Activated(bool flag);
	void ResetByCenterRow();
	void ChangeFocus();
	int GetWindowWidth();
	void RefreshLayout(bool flag);
	VtOrderWndHd* ParentDlg() const { return _ParentDlg; }
	void ParentDlg(VtOrderWndHd* val) { _ParentDlg = val; }
	int x, y, width, height;
	int GetMaxWidth();
	void AdjustControlForMode();
	void RepositionControl(int height);
	void RepositionControl();
	void ResizeOrderGrid(int maxHeight);
	void ResizeOrderGrid();
	int GetCountOrderGridEnabledCol();
	bool ShowTickWnd();
	void ChangeAccount(account_p acnt);
	void ChangeFund(fund_p fund);
	void ChangeSymbol(symbol_p symbol);
	void ResetRemainFund();
public:
	void set_default_symbol();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CComboBox _ComboSymbol;
	CEdit _EditOrderAmount;
	CSpinButtonCtrl _ScOrderAmount;
	CSpinButtonCtrl _ScStopVal;
	CEdit _EditStopVal;
	CButton _BtnAvail;
	CShadeButtonST _BtnRemain;
	CShadeButtonST _BtnSearch;
	CStatic _StaticMsg;
	CGradientStatic _StaticProductName;
	CGradientStatic _StaticProduct;
	CShadeButtonST _BtnAmt1;
	CShadeButtonST _BtnAmt2;
	CShadeButtonST _BtnAmt3;
	CShadeButtonST _BtnAmt4;
	CShadeButtonST _BtnAmt5;
	CShadeButtonST _BtnAmt6;
	CShadeButtonST _BtnSettings;
	VtEditCtrl _EditAmt;
	CShadeButtonST* _CurBtn = nullptr;
	CStatic _StaticStop;
	CButton _CheckFixedCenter;
	CButtonST _BtnRemainFund;


	afx_msg void OnBnClickedButtonSetting();
	afx_msg void OnDestroy();
	afx_msg void OnDeltaposSpinExpand(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStnClickedStaticMsg();
	afx_msg void OnBnClickedBtnShowRealtick();
	afx_msg void OnBnClickedStaticFillPl();
	afx_msg void OnCbnSelchangeComboProductHd();
	afx_msg void OnBnClickedCkFixedCenter();
	afx_msg void OnBnClickedBtnOrderAmt1();
	afx_msg void OnBnClickedBtnOrderAmt2();
	afx_msg void OnBnClickedBtnOrderAmt3();
	afx_msg void OnBnClickedBtnOrderAmt4();
	afx_msg void OnBnClickedBtnOrderAmt5();
	afx_msg void OnBnClickedBtnOrderAmt6();
	afx_msg void OnDeltaposSpinOrderAmount(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditOrderAmount();
	afx_msg void OnEnChangeEditStopval();
	afx_msg void OnBnClickedBtnLiq();
	afx_msg void OnBnClickedBtnRemain();
	afx_msg void OnBnClickedBtnSelSymbol();
	afx_msg void OnBnClickedButtonProfitLoss();
	afx_msg void OnStnClickedStaticOrderPanel();
	afx_msg void OnRClicked(NMHDR * pNotifyStruct, LRESULT * result);
	afx_msg void OnStnClickedStaticRealTick();
	afx_msg void OnBnClickedBtnRemainFund();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
private:
	void AddItemToComboBox(CComboBox& comboBox, symbol_p item)
	{
		// Add the std::shared_ptr to the vector
		combo_symbol_vector.push_back(item);

		// Add item to the combo box and store the index in the combo box
		int index = comboBox.AddString(item->SymbolCode().c_str()); // Replace with appropriate text
		comboBox.SetItemDataPtr(index, reinterpret_cast<void*>(combo_symbol_vector.size() - 1));
	}

	symbol_p GetSelectedItemDataPtr(const CComboBox& comboBox)
	{
		int index = comboBox.GetCurSel();
		if (index != CB_ERR)
		{
			// Retrieve the index from the combo box and get the std::shared_ptr
			size_t ptrIndex = reinterpret_cast<size_t>(comboBox.GetItemDataPtr(index));
			return combo_symbol_vector[ptrIndex];
		}
		return nullptr;
	}
	std::vector<symbol_p> combo_symbol_vector;
	//Timer* timer = nullptr;
	bool _BlockEvent = true;
	VtOrderWndHd* _ParentDlg = nullptr;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	std::vector<VtOrderGridConfig*> _ConfigDlgVector;
	SymbolTickView _TickGrid;
	VtProductRemainGrid _ProductRemainGrid;
	VtConfigGrid _ConfigGrid;
	bool _FixedCenter;
	bool _Init = false;
	bool _ShowRemainConfig = true;
	bool _ShowOrderCountArea = true;
	bool _UseHogaSiseFilter = false;

	symbol_p  _Symbol;
	int _OrderAmount;
	VtFilledCondition _FillCondition;
	VtPriceType _PriceType;

	int _StopVal;
	bool _Activated = false;
	int _TickWndPos = 0;
	int _GridGap;

	bool _ShowOrderArea = true;
	bool _ShowStopArea = true;
	bool _ShowTickWnd = false;

	VtRefreshManager* _RefreshManager = nullptr;
	bool _Unregistered = false;
	//VtCutManager* _CutMgr;
	// 남은 잔고 만큼 주문을 낸다.
	bool _OrderByRemain;
	VtLayoutManager* _LayoutMgr;
	//std::vector<bool> _OrderGridColOption;

	// key : combobox index, value : symbol object
	std::map<int, symbol_p> index_to_symbol_;
	// key : symbol code, value : combobox index.
	std::map<std::string, int> symbol_to_index_;


	int _DefaultWidth = 482;
	int _DefaultHeight = 750;
	symbol_p _DefaultSymbol = nullptr;
	VtOrderConfigDlg* _ConfigDlg = nullptr;
	int id_{ 0 };
private:
	void CreateChildWindow(VtOrderConfigDlg* centerWnd, UINT id, CWnd* parent);
	CRect GetClientArea(int resourceID);
public:
	void SetRemain(int remain);
	void RefreshOrderPositon();
	void ClearConfigDlg();
	void SaveControlPos();
	void CalcLayout();
	int GetParentHeight();
	int GetParentTitleBarHeight();
	int GetGridWidth();
	int GetRemainWidth(int parentWidth);
	int GetRemainWidth();
	void RepositionConfigWnd();
	void RepositionTickGrid();
	void ShowHideCtrl();
	void RepositionProductGrid();

	bool EnableCutProfit() const;

	void EnableCutProfit(bool val);

	bool EnableCutLoss() const;

	void EnableCutLoss(bool val);

	int CutProfit() const;

	void CutProfit(int val);

	int CutLoss() const;

	void CutLoss(int val);

	int OrderType() const;

	void OrderType(int val);

	int OrderTypeSlip() const;

	void OrderTypeSlip(int val);

// 	VtCutManager* GetCutManager() {
// 		return m_Grid.CutMgr();
// 	}

	void ApplyProfitLossForPosition();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void ShowOrderAreaInGrid(bool flag);
	void ShowStopAreaInGrid(bool flag);
	void SetTickWndPos(int pos);
public:
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
};
