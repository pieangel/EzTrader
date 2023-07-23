/*
#pragma once
class DmFundOrderRightWindow
{
};
*/
#pragma once

//#include "../../SmGrid/SmAssetArea.h"
//#include "../../SmGrid/SmSymbolArea.h"
#include "../../View/AccountAssetView.h"
#include "../../View/SymbolInfoView.h"
#include <memory>

// DmFundOrderRightWindow dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}


class DmFundOrderRightWindow : public CBCGPDialog
{
	DECLARE_DYNAMIC(DmFundOrderRightWindow)

public:
	DmFundOrderRightWindow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DmFundOrderRightWindow();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DM_ACNT_ORDER_RIGHT};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	AccountAssetView asset_view_;
	SymbolInfoView symbol_info_view_;
public:
	void SetFund(const std::shared_ptr<DarkHorse::SmFund>& fund);
	void SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account);
	void SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	void OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


