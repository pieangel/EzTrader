#pragma once
#include "BCGCBPro.h"
#include "../SmGrid/SmAcceptedArea.h"
#include "../SmGrid/SmFilledArea.h"
#include "../SmGrid/SmFavoriteArea.h"
#include "../SmGrid/SmAccountArea.h"
#include "SmCheckGrid.h"
#include "SmAcceptedGrid.h"
#include "SmPositionGrid.h"
#include "SmFavoriteGrid.h"

// OrderLeftDialog dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
}
class SmSymbolTableDialog;
class SmMainOrderDialog;
class OrderLeftDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(OrderLeftDialog)

public:
	OrderLeftDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~OrderLeftDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_LEFT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	SmAccountArea _AccountArea;

	SmAcceptedGrid _AcceptedGrid;
	SmPositionGrid _PositionGrid;
	

	//SmFilledArea _FilledArea;
	//SmFavoriteArea _FavoriteArea;
	//SmAcceptedArea _AcceptedArea;
	//CBCGPScrollBar _VScrollBarAcpt;
	//CBCGPScrollBar _VScrollBarPosi;
	//CBCGPScrollBar _VScrollBarFav;
public:
	void SetMainWnd(SmMainOrderDialog* main_wnd);
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
	SmFavoriteGrid _FavoriteGrid;
};
