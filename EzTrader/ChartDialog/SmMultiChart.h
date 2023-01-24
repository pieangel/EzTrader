#pragma once
#include <BCGCBProInc.h>
#include "../Chart/SmChartConsts.h"
#include "../TimeData/SmCompData.h"
#include <memory>
#include <map>
// SmMultiChart dialog
namespace DarkHorse {
	class SmChartData;
	class SmSymbol;
}
class SmMultiChart : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmMultiChart)

public:
	SmMultiChart(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmMultiChart();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:
	void InitCombo();
	HWND m_hPE;
	DarkHorse::SmCompData _CompData;
	int _ToolBarHeight = 26;

	CBCGPComboBox _ComboSymbolComp;
	CBCGPComboBox _ComboSymbolMain;

	// key : row index, value : smsymbol object
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToMainSymbolMap;
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToCompSymbolMap;

	void SetChart();
public:
	virtual BOOL OnInitDialog();
};
