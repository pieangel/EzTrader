#pragma once
#include <BCGCBProInc.h>
#include <map>
#include "../SmGrid/SmTotalAssetArea.h"
#include "../View/TotalAssetProfitLossView.h"
// SmTotalAssetDialog dialog
class TotalAssetProfitLossDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(TotalAssetProfitLossDialog)

public:
	TotalAssetProfitLossDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~TotalAssetProfitLossDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOTAL_ASSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	// key : combo index, value : account object.
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _ComboAccountMap;
	int _CurrentAccountIndex{ 0 };
	TotalAssetProfitLossView total_asset_profit_loss_view_;
public:
	void SetAccount();
	virtual BOOL OnInitDialog();
	CBCGPComboBox _ComboAccount;
	afx_msg void OnBnClickedBtnGet();
	afx_msg void OnCbnSelchangeComboAccount();
	virtual void PostNcDestroy();
};
