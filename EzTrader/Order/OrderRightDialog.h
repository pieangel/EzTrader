#pragma once

#include "../SmGrid/SmAssetArea.h"
#include "../SmGrid/SmSymbolArea.h"
#include "../View/AssetView.h"
#include "../View/SymbolInfoView.h"
#include <memory>

// OrderRightDialog dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
}


class OrderRightDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(OrderRightDialog)

public:
	OrderRightDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~OrderRightDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_RIGHT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	AssetView _AssetArea;
	SymbolInfoView _SymbolArea;
public:
	void SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account);
	void SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	void OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
