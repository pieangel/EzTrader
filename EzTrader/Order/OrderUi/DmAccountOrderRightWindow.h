/*
#pragma once
class DmAccountOrderRightWindow
{
};
*/
#pragma once

//#include "../../SmGrid/SmAssetArea.h"
//#include "../../SmGrid/SmSymbolArea.h"
#include "../../View/AccountAssetView.h"
#include "../../View/SymbolInfoView.h"
#include <memory>

// DmAccountOrderRightWindow dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}

struct WinInfo;
class DmAccountOrderRightWindow : public CBCGPDialog
{
	DECLARE_DYNAMIC(DmAccountOrderRightWindow)

public:
	DmAccountOrderRightWindow(CWnd* pParent = nullptr);   // standard constructor
	DmAccountOrderRightWindow(CWnd* pParent, std::shared_ptr<WinInfo> parent_win_info);
	virtual ~DmAccountOrderRightWindow();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DM_ACNT_ORDER_RIGHT};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	std::shared_ptr<WinInfo> win_info_{ nullptr };
	AccountAssetView asset_view_;
	SymbolInfoView symbol_info_view_;
	int order_window_id_{ 0 };
public:
	std::shared_ptr<WinInfo> Win_info() const { return win_info_; }
	void Win_info(std::shared_ptr<WinInfo> val) { win_info_ = val; }

	int order_window_id() const { return order_window_id_; }
	void order_window_id(int val) { order_window_id_ = val; }
	void SetFund(std::shared_ptr<DarkHorse::SmFund> fund);
	void SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account);
	void SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	void OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


