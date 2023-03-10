/*
#pragma once
class DmFundOrderLeftWindow
{
};
*/



#pragma once
#include "BCGCBPro.h"
#include "../../SmGrid/SmAcceptedArea.h"
#include "../../SmGrid/SmFilledArea.h"
#include "../../SmGrid/SmFavoriteArea.h"
#include "../../SmGrid/SmAccountArea.h"
#include "../../Order/SmCheckGrid.h"
#include "../../Order/SmAcceptedGrid.h"
#include "../../Order/SmPositionGrid.h"
#include "../../Order/SmFavoriteGrid.h"

#include "../../View/AccountOrderView.h"
#include "../../View/AccountPositionView.h"
#include "../../View/AccountProfitLossView.h"
#include "../../View/DmOptionView.h"
#include "../../View/DmFutureView.h"
#include "../../View/AssetView.h"

// OrderLeftDialog dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}
class SmSymbolTableDialog;
class SmFundOrderDialog;
class DmFundOrderLeftWindow : public CBCGPDialog
{
	DECLARE_DYNAMIC(DmFundOrderLeftWindow)

public:
	DmFundOrderLeftWindow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DmFundOrderLeftWindow();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DM_FUND_ORDER_LEFT};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	
	AccountProfitLossView account_profit_loss_view_;
	AssetView asset_view_;
	//AccountOrderView _AcceptedGrid;
	//AccountPositionView _PositionGrid;
	//SmFavoriteGrid _FavoriteGrid;
	DmOptionView option_view_;
	DmFutureView future_view_;


public:
	CBCGPButton _BtnAddFav;
	CBCGPStatic _StaticFav;
	void SetFundOrderWnd(SmFundOrderDialog* fund_order_wnd);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnAddFav();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SetAccount(std::shared_ptr<DarkHorse::SmAccount> account);
	void SetFund(std::shared_ptr < DarkHorse::SmFund> fund);
	afx_msg LRESULT OnUmSymbolSelected(WPARAM wParam, LPARAM lParam);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	afx_msg void OnBnClickedBtnCancelSel();
	afx_msg void OnBnClickedBtnCancelAll();
	afx_msg void OnBnClickedBtnLiqSel();
	afx_msg void OnBnClickedBtnLiqAll();
};


