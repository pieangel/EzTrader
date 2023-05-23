#pragma once
#include <BCGCBProInc.h>
#include "../SmGrid/SmJangoAccountArea.h"
#include "../SmGrid/SmJangoPositionArea.h"
#include "../View/AccountProfitLossView.h"
#include "../View/AccountPositionView.h"
#include <map>
// MiniJangoDialog dialog

class MiniJangoDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(MiniJangoDialog)

public:
	MiniJangoDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~MiniJangoDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JANGO };
#endif

	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CBCGPComboBox _ComboAccount;
	CBCGPStatic _StaticCombo;
	AccountProfitLossView account_profit_loss_view_;
	AccountPositionView account_position_view_;
	SmJangoAccountArea _StaticAccount;
	SmJangoPositionArea _StaticPosition;
	virtual BOOL OnInitDialog();

private:
	void SetAccount();
	void SetFund();
	// key : combo index, value : account object.
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _ComboAccountMap;
	std::map<int, std::shared_ptr<DarkHorse::SmFund>> _ComboFundMap;
	int _CurrentAccountIndex{ 0 };
	// 0 : account, 1 : fund
	int _Mode = 0;
public:
	afx_msg void OnCbnSelchangeComboAccount();
	virtual void PostNcDestroy();
};
