

#pragma once

#include <BCGCBProInc.h>
#include "SmChartStorage.h"
#include <memory>
#include <map>
#include "../Chart/SmChartConsts.h"
#include "../TimeData/SmCompData.h"
// SmSpreadChart dialog
namespace DarkHorse {
	class SmChartData;
	class SmSymbol;
}
class SmSymbolTableDialog;
class SmGigaCompChart : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmGigaCompChart)

public:
	SmGigaCompChart(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmGigaCompChart();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_COMP_CHART
};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);


	afx_msg void OnBnClickedBtnMon();
	afx_msg void OnBnClickedBtnWeek();
	afx_msg void OnBnClickedBtnDay();
	afx_msg void OnBnClickedBtnMin();
	afx_msg void OnBnClickedBtnTick();
	afx_msg void OnBnClickedBtnSet();
	afx_msg void OnBnClickedBtnFind();
	afx_msg void OnCbnSelchangeComboSymbol();
	afx_msg void OnCbnSelchangeComboCycle();
	afx_msg void OnCbnSelchangeComboTick();
	afx_msg void OnCbnSelchangeComboStyle();
	afx_msg void OnClear();
	afx_msg LRESULT OnChartAxisZoomed(WPARAM wp, LPARAM lp);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnUmSymbolSelected(WPARAM wParam, LPARAM lParam);
	void UpdateChartColorTheme(int nTheme, BOOL bIsDarkTheme);
	void SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& window_id);
	// action :: 1 : update, 2 : add
	void OnChartEvent(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action);
private:
	HWND m_hPE;

	std::vector< CCustomStockSeries*> _SeriresVector;
	DarkHorse::SmCompData _CompData;
	// key : row index, value : smsymbol object
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToMainSymbolMap;
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToCompSymbolMap;

	CString _OhlcFormat;
	CString _RsiFormat;
	void InitStyle();
	const CBCGPChartTheme& GetCustomTheme(int nTheme)
	{
		return nTheme == -2 ? m_CustomColorTheme : m_CustomTexturedTheme;
	}

	CBCGPChartTheme		m_CustomColorTheme;
	CBCGPChartTheme		m_CustomTexturedTheme;
	CBCGPToolBarImages	m_Thumbnails;

	BOOL m_bIsDarkTheme;
	int m_nColorTheme = -2;
	void InitTheme();
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	bool _NeedUpdate = false;
	bool _NeedAdd = false;
	DarkHorse::SmChartType _ChartTimeType = DarkHorse::SmChartType::MIN;
	int _Cycle = 1;
	int _UniqueId = -1;
	void InitSymbolCombo();
	// key : row index, value : smsymbol object
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToSymbolMap;
	int _CurIndex = -1;
	virtual void OnChangeBackgroundDarkness() {}
	BOOL					m_bIsDarkBackground;
	BOOL					m_bIsTexturedTheme;
	BOOL					m_bIsFlatTheme;
	UINT					m_nThumbnailFlags;

	CBCGPChartSeries* m_pOverlaySeriesLine;
	CBCGPChartSeries* m_pOverlaySeriesBand;

	void SetBarLineWidth();
	int		m_nChartType;
	int		m_nLineType;
	int		m_nOverlayLine;
	int		m_nOverlayBand;

	BOOL	m_bAutoScroll;
	BOOL	m_bDataTable;
	void AddChartData();
	void UpdateChartData();
	void CreateChart();
	void UpdateChart();
	CBCGPChartObject* SetupIndicatorObjects(double dblValue, CBCGPChartAxis* pIndicatorAxis,
		const CString& strIndicatorName = _T(""));

	double				m_dblUpTrendLimit;
	CBCGPChartSeries* m_pRSISeries;
	CBCGPChartObject* m_pRSIInfoObject;
	CBCGPChartObject* m_pOHLCInfo;

	//CExternalStorage _MainStorage;
	CBCGPChartCtrl	m_wndChart;

	CBCGPChartVisualObject* GetChart()
	{
		return m_wndChart.GetChart();
	}

	CBCGPBaseVisualCtrl* GetCtrl()
	{
		return &m_wndChart;
	}

	void ChangeTimeButtonColor(const DarkHorse::SmChartType& chart_type);
	void RequestChartData();
	void RequestChartData(const std::string symbol_code);
	void InitChart();
	void SetChartData();
public:
	CBCGPComboBox _ComboCycle;
	CBCGPComboBox _ComboStyle;
	CBCGPComboBox _ComboSymbol;
	CBCGPComboBox _ComboTick;
	CBCGPButton _BtnDay;
	CBCGPButton _BtnMin;
	CBCGPButton _BtnMon;
	CBCGPButton _BtnTick;
	CBCGPButton _BtnWeek;
	CBCGPComboBox _ComboTheme;

	CBCGPButton _CheckComp;
	CBCGPButton _CheckMain;
	CBCGPButton _CheckVirtual;
	CBCGPComboBox _ComboSymbolComp;
	CBCGPComboBox _ComboSymbolMain;

	afx_msg void OnCbnSelchangeComboTheme();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
