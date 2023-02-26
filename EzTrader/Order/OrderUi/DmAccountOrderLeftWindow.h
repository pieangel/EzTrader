//#pragma once
/*
class DmAccountOrderLeftWindow
{
};
*/

#pragma once
#include "BCGCBPro.h"
#include "../../SmGrid/SmAcceptedArea.h"
#include "../../SmGrid/SmFilledArea.h"
#include "../../SmGrid/SmFavoriteArea.h"
#include "../../SmGrid/SmAccountArea.h"
#include "../SmCheckGrid.h"
#include "../SmAcceptedGrid.h"
#include "../SmPositionGrid.h"
#include "../SmFavoriteGrid.h"
#include "../../View/AccountOrderView.h"
#include "../../View/AccountPositionView.h"
#include "../../View/AccountProfitLossView.h"
#include "../../View/OptionView.h"
#include "../../View/FutureView.h"

// DmAccountOrderLeftWindow dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
}
class SmSymbolTableDialog;
class DmAccountOrderWindow;
class DmAccountOrderLeftWindow : public CBCGPDialog
{
	DECLARE_DYNAMIC(DmAccountOrderLeftWindow)

public:
	DmAccountOrderLeftWindow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DmAccountOrderLeftWindow();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DM_ACNT_ORDER_LEFT};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	AccountProfitLossView _AccountArea;
	AccountOrderView _AcceptedGrid;
	AccountPositionView _PositionGrid;
	SmFavoriteGrid _FavoriteGrid;
	OptionView option_view_;
	FutureView future_view_;


	//SmFilledArea _FilledArea;
	//SmFavoriteArea _FavoriteArea;
	//SmAcceptedArea _AcceptedArea;
	//CBCGPScrollBar _VScrollBarAcpt;
	//CBCGPScrollBar _VScrollBarPosi;
	//CBCGPScrollBar _VScrollBarFav;
public:
	void SetMainWnd(DmAccountOrderWindow* main_wnd);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnAddFav();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SetAccount(std::shared_ptr<DarkHorse::SmAccount> account);
	afx_msg LRESULT OnUmSymbolSelected(WPARAM wParam, LPARAM lParam);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	afx_msg void OnBnClickedBtnCancelSel();
	afx_msg void OnBnClickedBtnCancelAll();
	afx_msg void OnBnClickedBtnLiqSel();
	afx_msg void OnBnClickedBtnLiqAll();
	CBCGPButton _BtnAddFav;
	CBCGPStatic _StaticFav;
};


