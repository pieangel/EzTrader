#include "stdafx.h"
#include "../MainFrm.h"
#include "../DarkHorse.h"
#include "SmCompChart.h"
#include "afxdialogex.h"
#include <string>
#include <vector>

#include "../Chart/SmChartData.h"
#include "../Util/SmUtil.h"
#include "../resource.h"

#include "../Symbol/SmSymbolTableDialog.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Chart/SmChartDataManager.h"
#include "../Event/SmCallbackManager.h"
#include <functional>
#include <format>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

#define ID_NEXT_DATA_POINT	1

#define COMP_REFRESH_TIMER 1
#define COMP_REQUEST_TIMER 2

static const int RSI_PERIOD = 14;
//static const CString RSI_INFO_FORMAT = _T("RSI(%d), Value: %.4f");
//static const CString OHLC_INFO_FORMAT = _T("O: %.4f, H: %.4f, L: %.4f, C: %.4f");

// SmCompChart dialog

IMPLEMENT_DYNAMIC(SmCompChart, CBCGPDialog)

SmCompChart::SmCompChart(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_COMP_CHART, pParent)
{
	_UniqueId = CMainFrame::GetId();

	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	SetWhiteBackground(FALSE);
	m_nChartType = 0;
	m_nOverlayLine = 0;
	m_nOverlayBand = 2;
	//}}AFX_DATA_INIT

	m_pOverlaySeriesLine = NULL;
	m_pOverlaySeriesBand = NULL;

	m_nLineType = 5;
	m_bAutoScroll = TRUE;
	m_bDataTable = FALSE;
	//}}AFX_DATA_INIT

	m_dblUpTrendLimit = 0.5;
	m_pRSISeries = NULL;
	m_pRSIInfoObject = NULL;
	m_pOHLCInfo = NULL;

	m_bIsDarkTheme = TRUE;
}

SmCompChart::~SmCompChart()
{
	KillTimer(COMP_REFRESH_TIMER);
	mainApp.CallbackMgr()->UnsubscribeChartCallback((long)this);
}

void SmCompChart::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CHART, m_wndChart);
	DDX_Control(pDX, IDC_COMBO_THEME, _ComboTheme);
	DDX_Control(pDX, IDC_CHECK_COMP, _CheckComp);
	DDX_Control(pDX, IDC_CHECK_MAIN, _CheckMain);
	DDX_Control(pDX, IDC_CHECK_VIRTUAL, _CheckVirtual);
	DDX_Control(pDX, IDC_COMBO_SYMBOL_COMP, _ComboSymbolComp);
	DDX_Control(pDX, IDC_COMBO_SYMBOL_MAIN, _ComboSymbolMain);
}


BEGIN_MESSAGE_MAP(SmCompChart, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_MON, &SmCompChart::OnBnClickedBtnMon)
	ON_BN_CLICKED(IDC_BTN_WEEK, &SmCompChart::OnBnClickedBtnWeek)
	ON_BN_CLICKED(IDC_BTN_DAY, &SmCompChart::OnBnClickedBtnDay)
	ON_BN_CLICKED(IDC_BTN_MIN, &SmCompChart::OnBnClickedBtnMin)
	ON_BN_CLICKED(IDC_BTN_TICK, &SmCompChart::OnBnClickedBtnTick)
	ON_BN_CLICKED(IDC_BTN_SET, &SmCompChart::OnBnClickedBtnSet)
	ON_BN_CLICKED(IDC_BTN_FIND, &SmCompChart::OnBnClickedBtnFind)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &SmCompChart::OnCbnSelchangeComboSymbol)
	ON_CBN_SELCHANGE(IDC_COMBO_CYCLE, &SmCompChart::OnCbnSelchangeComboCycle)
	ON_CBN_SELCHANGE(IDC_COMBO_TICK, &SmCompChart::OnCbnSelchangeComboTick)
	ON_CBN_SELCHANGE(IDC_COMBO_STYLE, &SmCompChart::OnCbnSelchangeComboStyle)
	ON_REGISTERED_MESSAGE(BCGM_ON_CHART_AXIS_ZOOMED, OnChartAxisZoomed)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &SmCompChart::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_THEME, &SmCompChart::OnCbnSelchangeComboTheme)
END_MESSAGE_MAP()


// SmCompChart message handlers


BOOL SmCompChart::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	// 테마 초기화 - 차트 설정과 관계가 없다.
	InitTheme();
	// 스타일 초기화
	//InitStyle();

	// 레이아웃 설정
	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	if (pLayout != NULL)
	{
		pLayout->AddAnchor(IDC_STATIC_CHART, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);
	}

	// 차트 색상 테마 업데이트
	UpdateChartColorTheme(m_nColorTheme, m_bIsDarkTheme);

	// 차트 객체를 가져온다.
	CBCGPChartVisualObject* pChart = GetChart();
	if (pChart != NULL)
	{
		// 교차 색상 설정
		pChart->ShowAxisIntervalInterlacing(BCGP_CHART_X_PRIMARY_AXIS);
	}


	//pChart->SetChartType(BCGPChartStock, BCGP_CT_SIMPLE, FALSE);

	// 시리즈 생성
	CCustomStockSeries* pMainSeries = new CCustomStockSeries(pChart, CBCGPBaseChartStockSeries::SST_LINE_CLOSE);
	CBCGPChartSeries* pCloseSeries = pMainSeries->GetChildSeries(CBCGPChartStockSeries::CHART_STOCK_SERIES_CLOSE_IDX);
	// 시리즈 이름 설정
	pMainSeries->m_strSeriesName = _T("main");
	pMainSeries->m_nOpenCloseBarSizePercent = 70;
	//pStockSeries->SetExternalStorage(&_MainStorage);
	// 시리즈 추가
	pChart->AddSeries(pMainSeries);
	_SeriresVector.push_back(pMainSeries);


	CCustomStockSeries* pCompSeries = new CCustomStockSeries(pChart, CBCGPBaseChartStockSeries::SST_LINE_CLOSE);
	// 시리즈 이름 설정
	pCompSeries->m_strSeriesName = _T("comp");
	pCompSeries->m_nOpenCloseBarSizePercent = 70;
	//pStockSeries->SetExternalStorage(&_MainStorage);
	// 시리즈 추가
	pChart->AddSeries(pCompSeries);
	_SeriresVector.push_back(pCompSeries);

	CCustomStockSeries* pVirSeries = new CCustomStockSeries(pChart, CBCGPBaseChartStockSeries::SST_LINE_CLOSE);
	// 시리즈 이름 설정
	pVirSeries->m_strSeriesName = _T("comp");
	pVirSeries->m_nOpenCloseBarSizePercent = 70;
	//pStockSeries->SetExternalStorage(&_MainStorage);
	// 시리즈 추가
	pChart->AddSeries(pVirSeries);
	_SeriresVector.push_back(pVirSeries);


	// 축 설정
	CBCGPChartAxis* pAxisX = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	CBCGPChartAxis* pAxisY = pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
	CBCGPChartAxis* pSecondAxisY = pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);

	pChart->ShowAxis(BCGP_CHART_X_PRIMARY_AXIS, TRUE, TRUE);
	pChart->ShowAxis(BCGP_CHART_Y_PRIMARY_AXIS, TRUE, TRUE);
	pChart->ShowAxis(BCGP_CHART_Y_SECONDARY_AXIS, TRUE, TRUE);
	pChart->ShowAxisGridLines(BCGP_CHART_Y_PRIMARY_AXIS, FALSE);
	pChart->ShowAxisGridLines(BCGP_CHART_Y_SECONDARY_AXIS, FALSE);
	pChart->SetLegendPosition(BCGPChartLayout::LP_TOP);
	// 시리즈와 축 연결
	pMainSeries->SetRelatedAxis(pAxisY, CBCGPChartSeries::AI_Y);
	pCompSeries->SetRelatedAxis(pSecondAxisY, CBCGPChartSeries::AI_Y);
	pVirSeries->SetRelatedAxis(pSecondAxisY, CBCGPChartSeries::AI_Y);
	// 축 세부 설정
	pAxisX->m_bFormatAsDate = TRUE;
	pAxisX->m_strDataFormat = _T("%H:%M:%S");
	pAxisX->ShowScrollBar(TRUE);
	//pAxisX->SetFixedIntervalWidth(33, 2);

	// 축과 갭 설정
	// set offset of last displayed value in auto-scroll mode (in pixels)
	pAxisX->SetRightOffsetInPixels(33);
	// 축 포맷 설정
	pAxisY->m_strDataFormat = _T("%.2f");
	pSecondAxisY->m_strDataFormat = _T("%.2f");


	// 지표 설정
	// Setup last OHLC info
	m_pOHLCInfo = SetupIndicatorObjects(DBL_MAX, pAxisY, _T(" ")); // DBL_MAX means that we don't need overbought line

	// Setup Custom Axes for indicators:
	// RSI
	CBCGPChartAxis* pAxisRSI = (CBCGPChartAxisY*)pAxisY->Split(80, 6, NULL, FALSE);
	pAxisRSI->SetFixedDisplayRange(0, 100, 50);
	pAxisRSI->m_strAxisName = _T("RSI Axis");
	pAxisRSI->EnableZoom(FALSE);
	pAxisRSI->m_crossType = CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE;
	pAxisRSI->m_majorTickMarkType = CBCGPChartAxis::TMT_INSIDE;

	// Create indicators
	// RSI
	static const int nRSIPeriod = 14;
	CBCGPChartRSIFormula rsi(CBCGPChartRSIFormula::RSI_SMOOTHED, nRSIPeriod);
	m_pRSISeries = rsi.Create(pChart, _T("RSI Smooth"), pCloseSeries, pAxisRSI);
	m_pRSISeries->m_bIncludeSeriesToLegend = FALSE;

	// Setup overbought and oversold indicator lines and indicator info
	static double dblRSIOversoldValue = 20;
	static double dblRSIOverboughtValue = 80;

	CString strRSIInfo;
	strRSIInfo.Format(_RsiFormat, RSI_PERIOD, 0.);

	SetupIndicatorObjects(dblRSIOversoldValue, pAxisRSI);
	m_pRSIInfoObject = SetupIndicatorObjects(dblRSIOverboughtValue, pAxisRSI, strRSIInfo);

	// X Axis should be always at the diagram bottom along with its labels
	pAxisX->m_crossType = CBCGPChartAxis::CT_FIXED_DEFAULT_POS;
	pAxisX->m_axisLabelType = CBCGPChartAxis::ALT_LOW;

	// allow interlacing for indicator area as well
	pAxisX->UsePerpendicularAxisForZoning(FALSE);

	pChart->SetZoomScrollConfig(BCGPChartMouseConfig::ZSO_WHEEL_PAN);
	//pChart->SetZoomScrollConfig(BCGPChartMouseConfig::ZSO_WHEEL_PAN);
	m_wndChart.EnableTooltip();

	

	UpdateChart();

	InitSymbolCombo();

	ChangeTimeButtonColor(_ChartTimeType);

	//RequestChartData();

	//SetTimer(1, 10, NULL);

	//SetTimer(COMP_REFRESH_TIMER, 10, NULL);
	SetTimer(COMP_REQUEST_TIMER, 700, NULL);

	mainApp.CallbackMgr()->SubscribeChartCallback((long)this, std::bind(&SmCompChart::OnChartEvent, this, _1, _2));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmCompChart::OnBnClickedBtnMon()
{
	_ChartTimeType = SmChartType::MON;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmCompChart::OnBnClickedBtnWeek()
{
	_ChartTimeType = SmChartType::WEEK;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmCompChart::OnBnClickedBtnDay()
{
	_ChartTimeType = SmChartType::DAY;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmCompChart::OnBnClickedBtnMin()
{
	_ChartTimeType = SmChartType::MIN;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmCompChart::OnBnClickedBtnTick()
{
	_ChartTimeType = SmChartType::TICK;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmCompChart::OnBnClickedBtnSet()
{
	// TODO: Add your control notification handler code here
}


void SmCompChart::OnBnClickedBtnFind()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	//_SymbolTableDlg->SpreadChart = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}


void SmCompChart::OnCbnSelchangeComboSymbol()
{
	_CurIndex = _ComboSymbol.GetCurSel();
	auto found = _RowToSymbolMap.find(_CurIndex);
	if (found == _RowToSymbolMap.end()) return;

	std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindChartData(found->second->SymbolCode(), _ChartTimeType, _Cycle);
	if (!chart_data || !_ChartData) return;

	if (chart_data->Id() == _ChartData->Id()) return;

	mainApp.SymMgr()->RegisterSymbolToServer(found->second->SymbolCode(), true);
	RequestChartData(found->second->SymbolCode());
}


void SmCompChart::OnCbnSelchangeComboCycle()
{
	if (_ChartTimeType == DarkHorse::SmChartType::TICK) {
		_ChartTimeType = DarkHorse::SmChartType::MIN;
		ChangeTimeButtonColor(_ChartTimeType);
	}
	CString strValue;
	const int cur_sel = _ComboCycle.GetCurSel();
	_ComboCycle.GetLBText(cur_sel, strValue);
	_Cycle = _ttoi(strValue);
	RequestChartData();
}


void SmCompChart::OnCbnSelchangeComboTick()
{
	if (_ChartTimeType != DarkHorse::SmChartType::TICK) {
		_ChartTimeType = DarkHorse::SmChartType::TICK;
		ChangeTimeButtonColor(_ChartTimeType);
	}
	CString strValue;
	const int cur_sel = _ComboTick.GetCurSel();
	_ComboTick.GetLBText(cur_sel, strValue);
	_Cycle = _ttoi(strValue);
	RequestChartData();
}


void SmCompChart::OnCbnSelchangeComboStyle()
{
	m_nLineType = _ComboStyle.GetCurSel();
	UpdateChart();
}

void SmCompChart::OnClear()
{
	//if (!_ChartData) return;

	//_ChartData->ExternalStorage.Clear();

	if (m_pRSISeries != NULL)
	{
		m_pRSISeries->RemoveAllDataPoints();
	}

	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	pXAxis->SetScrollRange(); // clear scroll range

	if (m_pOHLCInfo != NULL)
	{
		m_pOHLCInfo->m_strText.Empty();
	}

	if (m_pRSIInfoObject != NULL)
	{
		CString strRSIInfo;
		strRSIInfo.Format(_RsiFormat, RSI_PERIOD, 0.);

		m_pRSIInfoObject->m_strText = strRSIInfo;
	}

	///pChart->SetDirty(TRUE, TRUE);
	//m_wndChart.SetFocus();
}

LRESULT SmCompChart::OnChartAxisZoomed(WPARAM wp, LPARAM lp)
{
	// show "thick" OHLC bars on X zoom in
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartAxis* pXAxis = (CBCGPChartAxis*)lp;

	if (pXAxis == NULL || pXAxis->m_nAxisID != BCGP_CHART_X_PRIMARY_AXIS)
	{
		return 0;
	}

	SetBarLineWidth();

	return 0;
}

void SmCompChart::OnTimer(UINT_PTR nIDEvent)
{

	if (nIDEvent == COMP_REQUEST_TIMER) {
		RequestChartData();
	}
	else {
		bool needRedraw = false;
		if (_NeedUpdate == true) {
			//UpdateChartData();
			_NeedUpdate = false;
			needRedraw = true;
		}
		if (_NeedAdd == true) {
			//AddChartData();
			_NeedAdd = false;
			needRedraw = true;
		}
		if (needRedraw) {
			UpdateData();
			CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
			ASSERT_VALID(pChart);
			int nStorageCount = _ChartData->ExternalStorage.GetCount();

			/*CBCGPChartVisualObject* pChart = GetChart();*/
			CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);

			// scroll range must be updated to show all data points in the storage
			if (m_bAutoScroll && !pChart->IsThumbTrackMode() && !pChart->IsPanMode())
			{
				// always scroll to the last added value if not paused;
				// otherwise scroll the chart while data points are still being added
				// add some number of values returned by GetRightOffsetAsNumberOfValues for extra 
				// offset from the right side (see SetRightOffsetInPixels in OnInitialUpdate)
				pXAxis->SetFixedMaximumDisplayValue(nStorageCount + pXAxis->GetRightOffsetAsNumberOfValues() - 1);
			}

			if (m_pRSISeries != NULL)
			{
				CBCGPChartRSIFormula* pRSIFormula = DYNAMIC_DOWNCAST(CBCGPChartRSIFormula, m_pRSISeries->GetFormula());

				if (pRSIFormula != NULL)
				{
					pRSIFormula->UpdateLastDataPoints();
				}

				if (m_pRSIInfoObject != NULL)
				{
					double dblVal = m_pRSISeries->GetDataPointValue(nStorageCount - 1);
					m_pRSIInfoObject->m_strText.Format(_RsiFormat, RSI_PERIOD, dblVal);
				}
			}
			pChart->SetDirty(TRUE, TRUE);
		}
	}
}

LRESULT SmCompChart::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = (int)wParam;
	std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	int found_index = -1;
	for (auto it = _RowToSymbolMap.begin(); it != _RowToSymbolMap.end(); ++it) {
		if (it->second->Id() == symbol->Id()) {
			found_index = it->first;
			break;
		}
	}

	if (found_index < 0) {

		std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindChartData(symbol->SymbolCode(), _ChartTimeType, _Cycle);

		if (chart_data && chart_data->Id() == _ChartData->Id()) return 1;

		RequestChartData(symbol->SymbolCode());
	}
	else if (found_index != _CurIndex) {
		_ComboSymbol.SetCurSel(found_index);
		_CurIndex = found_index;

		auto found = _RowToSymbolMap.find(_CurIndex);
		if (found == _RowToSymbolMap.end()) return 1;

		std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindChartData(found->second->SymbolCode(), _ChartTimeType, _Cycle);

		if (chart_data && chart_data->Id() == _ChartData->Id()) return 1;

		RequestChartData(symbol->SymbolCode());
	}

	return 1;
}

void SmCompChart::UpdateChartColorTheme(int nTheme, BOOL bIsDarkTheme)
{
	CBCGPChartVisualObject* pChart = GetChart();
	if (pChart == NULL)
	{
		return;
	}

	if (nTheme < 0)
	{
		if (m_bIsDarkBackground)
		{
			m_bIsDarkBackground = FALSE;
			OnChangeBackgroundDarkness();
		}

		pChart->SetColors(GetCustomTheme(nTheme), bIsDarkTheme);

		m_bIsTexturedTheme = nTheme == -1;
		m_bIsFlatTheme = FALSE;
	}
	else
	{
		CBCGPChartTheme::ChartTheme theme = (CBCGPChartTheme::ChartTheme)nTheme;

		BOOL bIsDarkBackground = FALSE;

		switch (theme)
		{
		case CBCGPChartTheme::CT_BLACK_AND_GOLD:
		case CBCGPChartTheme::CT_BLACK_AND_RED:
		case CBCGPChartTheme::CT_BLACK_AND_GREEN:
		case CBCGPChartTheme::CT_BLACK_AND_BLUE:
			bIsDarkBackground = TRUE;
			break;
		}

		if (m_bIsDarkBackground != bIsDarkBackground)
		{
			m_bIsDarkBackground = bIsDarkBackground;
			OnChangeBackgroundDarkness();
		}

		pChart->SetColors((CBCGPChartTheme::ChartTheme)nTheme, bIsDarkTheme);

		m_bIsTexturedTheme = FALSE;

		m_bIsFlatTheme = theme >= CBCGPChartTheme::CT_FLAT_2014_1 && theme <= CBCGPChartTheme::CT_ICE;
	}
}

void SmCompChart::SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& window_id)
{
	if (!chart_data || _UniqueId != window_id) return;

	auto symbol = mainApp.SymMgr()->FindSymbol(chart_data->SymbolCode());
	if (!symbol) return;

	_ChartData = chart_data;

	OnClear();

	CBCGPChartVisualObject* pChart = GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartStockSeries* pStockSeries = DYNAMIC_DOWNCAST(CBCGPChartStockSeries, pChart->GetSeries(0));
	pStockSeries->RemoveAllDataPoints();

	CCustomStockSeries* pCustomSeries = (CCustomStockSeries*)pChart->GetSeries(0);
	pCustomSeries->SetExternalStorage(&_ChartData->ExternalStorage);
	pCustomSeries->m_strSeriesName = _ChartData->SymbolCode().c_str();

	int nStorageCount = _ChartData->ExternalStorage.GetCount();

	// limit the storage to ten millions data points
	if (nStorageCount > 10000000)
	{
		OnClear();
		return;
	}

	/*CBCGPChartVisualObject* pChart = GetChart();*/
	CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);

	// scroll range must be updated to show all data points in the storage
	if (m_bAutoScroll && !pChart->IsThumbTrackMode() && !pChart->IsPanMode())
	{
		// always scroll to the last added value if not paused;
		// otherwise scroll the chart while data points are still being added
		// add some number of values returned by GetRightOffsetAsNumberOfValues for extra 
		// offset from the right side (see SetRightOffsetInPixels in OnInitialUpdate)
		pXAxis->SetFixedMaximumDisplayValue(nStorageCount + pXAxis->GetRightOffsetAsNumberOfValues() - 1);
	}

	if (m_pRSISeries != NULL)
	{
		CBCGPChartRSIFormula* pRSIFormula = DYNAMIC_DOWNCAST(CBCGPChartRSIFormula, m_pRSISeries->GetFormula());

		if (pRSIFormula != NULL)
		{
			pRSIFormula->UpdateLastDataPoints();
		}

		if (m_pRSIInfoObject != NULL)
		{
			double dblVal = m_pRSISeries->GetDataPointValue(nStorageCount - 1);
			m_pRSIInfoObject->m_strText.Format(_RsiFormat, RSI_PERIOD, dblVal);
		}
	}

	if (m_pOHLCInfo != NULL)
	{
		CBCGPChartStockData data_point = pCustomSeries->GetStockDataAt(nStorageCount - 1);
		m_pOHLCInfo->m_strText.Format(_OhlcFormat, data_point.m_dblOpen, data_point.m_dblHigh, data_point.m_dblLow, data_point.m_dblClose);
	}


	CBCGPChartAxis* pAxisY = pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
	CString yAxisFormat;
	yAxisFormat.Format("%%.%df", symbol->decimal());
	pAxisY->m_strDataFormat = yAxisFormat;

	yAxisFormat.Format("O: %% .%df, H : %% .%df, L : %% .%df, C : %% .%df", symbol->decimal(), symbol->decimal(), symbol->decimal(), symbol->decimal());
	_OhlcFormat = yAxisFormat;

	yAxisFormat.Format("RSI(%%d), Value: %%.%df", symbol->decimal());
	_RsiFormat = yAxisFormat;

	int found_index = -1;
	for (auto it = _RowToSymbolMap.begin(); it != _RowToSymbolMap.end(); ++it) {
		if (it->second->Id() == symbol->Id()) {
			found_index = it->first;
			break;
		}
	}

	if (found_index < 0) {
		int index = _ComboSymbol.AddString(symbol->SymbolCode().c_str());
		_RowToSymbolMap[index] = symbol;
		_CurIndex = index;
		_ComboSymbol.SetCurSel(_CurIndex);
	}
	else {
		_CurIndex = found_index;
		_ComboSymbol.SetCurSel(_CurIndex);
	}

	pChart->SetDirty(TRUE, TRUE);
}

void SmCompChart::OnChartEvent(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action)
{
	if (!_ChartData || !chart_data || _ChartData->Id() != chart_data->Id()) return;
	bool needRedraw = false;
	if (action == 1) _NeedUpdate = true;
	if (action == 2) _NeedAdd = true;
}

static void InitSeriesColors(BCGPSeriesColors& sc, const CBCGPColor& color, UINT nTextureResID = 0)
{
	CBCGPColor colorGradient = color;
	colorGradient.MakeLighter(.5);

	if (nTextureResID != 0)
	{
		CBCGPColor colorAlt = color;
		colorAlt.MakeLighter(.2);

		sc.m_brElementFillColor.SetTextureImage(CBCGPImage(nTextureResID), colorAlt);
	}
	else
	{
		sc.m_brElementFillColor.SetColors(color, colorGradient, (CBCGPBrush::BCGP_GRADIENT_TYPE)-1);
	}

	CBCGPColor colorOutline = color;
	colorOutline.MakeDarker(.1);

	sc.m_brElementLineColor.SetColor(colorOutline);
	sc.m_brMarkerFillColor.SetColors(color, colorGradient, CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT);
	sc.m_brMarkerFillColor.MakeLighter(.15);

	sc.m_brMarkerLineColor.SetColor(colorOutline);
	sc.m_brDataLabelLineColor.SetColor(colorOutline);

	sc.m_brDataLabelFillColor.SetColor(color);
	sc.m_brDataLabelFillColor.MakePale();

	CBCGPColor colorText = color;
	colorText.MakeDarker(.2);

	sc.m_brDataLabelTextColor.SetColor(colorText);
}

void SmCompChart::InitStyle()
{
	_ComboStyle.AddString("OHLC");
	_ComboStyle.AddString("CandleStick");
	_ComboStyle.AddString("Line(Open)");
	_ComboStyle.AddString("Line(High)");
	_ComboStyle.AddString("Line(Low)");
	_ComboStyle.AddString("Line(Close)");
	_ComboStyle.AddString("Custom");
	_ComboStyle.AddString("HLC");

	m_nLineType = 0;

	_ComboStyle.SetCurSel(0);
}

void SmCompChart::InitTheme()
{
	// Setup custom color theme:
	CBCGPChartTheme::InitChartColors(m_CustomColorTheme,
		CBCGPColor::White,	// Fill color
		CBCGPColor::Chocolate,	// Outline color
		CBCGPColor::Maroon,		// Text color
		CBCGPColor(),
		CBCGPColor::SeaShell, .08);

	InitSeriesColors(m_CustomColorTheme.m_seriesColors[0], CBCGPColor::Crimson);
	InitSeriesColors(m_CustomColorTheme.m_seriesColors[1], CBCGPColor::BlueViolet);
	InitSeriesColors(m_CustomColorTheme.m_seriesColors[2], CBCGPColor::CornflowerBlue);
	InitSeriesColors(m_CustomColorTheme.m_seriesColors[3], CBCGPColor::DarkGreen);
	InitSeriesColors(m_CustomColorTheme.m_seriesColors[4], CBCGPColor::Coral);

	// Create custom texture theme:
	CBCGPChartTheme::InitChartColors(m_CustomTexturedTheme,
		CBCGPColor::White,	// Fill color
		CBCGPColor::CadetBlue,	// Outline color
		CBCGPColor::CadetBlue,		// Text color
		CBCGPColor(),
		CBCGPColor::SeaShell, .08);

	CBCGPImage imageBackground(IDR_BACKGROUND);

	m_CustomTexturedTheme.m_brChartFillColor.SetTextureImage(imageBackground, CBCGPColor::AliceBlue, TRUE /* Watermark */);
	m_CustomTexturedTheme.m_brPlotFillColor.SetTextureImage(imageBackground, CBCGPColor::AliceBlue, TRUE /* Watermark */);
	m_CustomTexturedTheme.m_brLegendFillColor.SetTextureImage(imageBackground, CBCGPColor::AliceBlue, TRUE /* Watermark */);
	m_CustomTexturedTheme.m_brLegendEntryFillColor.Empty();

	CBCGPImage imageWall(IDR_WALL);
	CBCGPChartTheme::PrepareWallBrushes(m_CustomTexturedTheme, CBCGPBrush(imageWall, CBCGPColor::LightGray, FALSE));

	InitSeriesColors(m_CustomTexturedTheme.m_seriesColors[0], CBCGPColor::LightSlateGray, IDR_TEXTURE1);
	InitSeriesColors(m_CustomTexturedTheme.m_seriesColors[1], CBCGPColor::LimeGreen, IDR_TEXTURE2);
	InitSeriesColors(m_CustomTexturedTheme.m_seriesColors[2], CBCGPColor::RosyBrown, IDR_TEXTURE3);
	InitSeriesColors(m_CustomTexturedTheme.m_seriesColors[3], CBCGPColor::CadetBlue, IDR_TEXTURE4);
	InitSeriesColors(m_CustomTexturedTheme.m_seriesColors[4], CBCGPColor::Goldenrod, IDR_TEXTURE5);


	_ComboTheme.AddString(_T("Default Theme"));
	_ComboTheme.AddString(_T("Pastel Theme"));
	_ComboTheme.AddString(_T("Spring Theme"));
	_ComboTheme.AddString(_T("Forest Green Theme"));
	_ComboTheme.AddString(_T("Sea Blue Theme"));
	_ComboTheme.AddString(_T("Golden Theme"));
	_ComboTheme.AddString(_T("Dark Rose Theme"));
	_ComboTheme.AddString(_T("Black and Gold Theme"));
	_ComboTheme.AddString(_T("Rainbow Theme"));
	_ComboTheme.AddString(_T("Gray Theme"));
	_ComboTheme.AddString(_T("Arctic Theme"));
	_ComboTheme.AddString(_T("Black and Red Theme"));
	_ComboTheme.AddString(_T("Plum Theme"));
	_ComboTheme.AddString(_T("Sunny Theme"));
	_ComboTheme.AddString(_T("Violet Theme"));
	_ComboTheme.AddString(_T("Flower Theme"));
	_ComboTheme.AddString(_T("Steel Theme"));
	_ComboTheme.AddString(_T("Gray and Green Theme"));
	_ComboTheme.AddString(_T("Olive Theme"));
	_ComboTheme.AddString(_T("Autumn Theme"));
	_ComboTheme.AddString(_T("Black and Green Theme"));
	_ComboTheme.AddString(_T("Black and Blue Theme"));
	_ComboTheme.AddString(_T("Flat 2014 1-st Theme"));
	_ComboTheme.AddString(_T("Flat 2014 2-nd Theme"));
	_ComboTheme.AddString(_T("Flat 2014 3-rd Theme"));
	_ComboTheme.AddString(_T("Flat 2014 4-th Theme"));
	_ComboTheme.AddString(_T("Flat 2015 1-st Theme"));
	_ComboTheme.AddString(_T("Flat 2015 2-nd Theme"));
	_ComboTheme.AddString(_T("Flat 2015 3-rd Theme"));
	_ComboTheme.AddString(_T("Flat 2015 4-th Theme"));
	_ComboTheme.AddString(_T("Flat 2015 5-th Theme"));
	_ComboTheme.AddString(_T("Flat 2016 1-st Theme"));
	_ComboTheme.AddString(_T("Flat 2016 2-nd Theme"));
	_ComboTheme.AddString(_T("Flat 2016 3-rd Theme"));
	_ComboTheme.AddString(_T("Flat 2016 4-th Theme"));
	_ComboTheme.AddString(_T("Flat 2016 5-th Theme"));
	_ComboTheme.AddString(_T("SPB Theme"));
	_ComboTheme.AddString(_T("Palace Theme"));
	_ComboTheme.AddString(_T("Ice Theme"));
	_ComboTheme.AddString(_T("Custom Theme (Color)"));
	_ComboTheme.AddString(_T("Custom Theme (Textures)"));

	_ComboTheme.SetCurSel(39);
}

void SmCompChart::InitSymbolCombo()
{
	// 반드시 실시간 등록을 해줄것
	const std::map<int, std::shared_ptr<DarkHorse::SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		int index = _ComboSymbolMain.AddString(it->second->SymbolCode().c_str());
		_RowToMainSymbolMap[index] = it->second;
	}

	if (_RowToMainSymbolMap.size() > 0) {
		_ComboSymbolMain.SetCurSel(0);
		_CompData.SetKospiSymbol(_RowToMainSymbolMap.begin()->second->SymbolCode());

	}

	for (auto it = favorite_map.rbegin(); it != favorite_map.rend(); ++it) {
		int index = _ComboSymbolComp.AddString(it->second->SymbolCode().c_str());
		_RowToCompSymbolMap[index] = it->second;
	}

	if (_RowToCompSymbolMap.size() > 0) {
		_ComboSymbolComp.SetCurSel(0);
		_CompData.SetNqSymbol(_RowToCompSymbolMap.begin()->second->SymbolCode());

	}
}

void SmCompChart::SetBarLineWidth()
{
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	CBCGPChartStockSeries* pStockSeries = DYNAMIC_DOWNCAST(CBCGPChartStockSeries, pChart->GetSeries(0));

	if (pXAxis == NULL || pStockSeries == NULL)
	{
		return;
	}

	CBCGPBaseChartStockSeries::StockSeriesType currStockType = pStockSeries->GetStockSeriesType();

	if (currStockType == CBCGPBaseChartStockSeries::SST_BAR)
	{
		if (pXAxis->GetValuesPerInterval() < 4)
		{
			pStockSeries->SetSeriesLineWidth(2.);
			pStockSeries->m_downBarStyle.m_dblWidth = 2.;
		}
		else
		{
			pStockSeries->SetSeriesLineWidth(1.);
			pStockSeries->m_downBarStyle.m_dblWidth = 1.;
		}
	}
	else if (currStockType == CBCGPBaseChartStockSeries::SST_CANDLE)
	{
		pStockSeries->SetSeriesLineWidth(1.);
		pStockSeries->m_downBarStyle.m_dblWidth = 1.;
	}
	else
	{
		pStockSeries->SetSeriesLineWidth(2.);
		pStockSeries->m_downBarStyle.m_dblWidth = 2.;
	}
}

void SmCompChart::AddChartData()
{
	if (!_ChartData || !_ChartData->Received()) return;

	auto symbol = mainApp.SymMgr()->FindSymbol(_ChartData->SymbolCode());
	if (!symbol) return;

	SmStockData data;
	_ChartData->GetLastData(data);

	std::vector<int> ymd = SmUtil::IntToDate(data.date_time.date());
	COleDateTime ole_date_time;
	ole_date_time.SetDateTime(ymd[0], ymd[1], ymd[2], data.date_time.hour(), data.date_time.minute(), data.date_time.sec());
	double divedend = pow(10, symbol->decimal());
	CBCGPChartStockData stockData;
	stockData.m_dateTime = ole_date_time;
	stockData.m_dblHigh = data.high / divedend;
	stockData.m_dblLow = data.low / divedend;
	stockData.m_dblOpen = data.open / divedend;
	stockData.m_dblClose = data.close / divedend;

	//_MainStorage.ShiftData(stockData);
	//_MainStorage.AddData(stockData.m_dblOpen, stockData.m_dblHigh, stockData.m_dblLow, stockData.m_dblClose, stockData.m_dateTime);
}

void SmCompChart::UpdateChartData()
{
	if (!_ChartData || !_ChartData->Received()) return;

	auto symbol = mainApp.SymMgr()->FindSymbol(_ChartData->SymbolCode());
	if (!symbol) return;

	SmStockData data;
	_ChartData->GetLastData(data);
	double divedend = pow(10, symbol->decimal());
	//_MainStorage.UpdateData(data.open / divedend, data.high / divedend, data.low / divedend, data.close / divedend);
}

void SmCompChart::CreateChart()
{

}

void SmCompChart::UpdateChart()
{
	UpdateData();

	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartStockSeries* pStockSeries = DYNAMIC_DOWNCAST(CBCGPChartStockSeries, pChart->GetSeries(0));

	if (pStockSeries != NULL)
	{
		pStockSeries->SetStockSeriesType((CBCGPBaseChartStockSeries::StockSeriesType)m_nLineType, FALSE);
	}

	SetBarLineWidth();

	BCGPChartFormatDataTable formatDataTable;
	formatDataTable.m_bShowLegendKeys = TRUE;
	formatDataTable.m_bDrawVerticalGridLines = FALSE;
	formatDataTable.m_bDrawOutline = FALSE;
	formatDataTable.SetContentPadding(CBCGPSize(1., 5.));

	pChart->ShowDataTable(m_bDataTable, &formatDataTable);

	pChart->SetDirty(TRUE, TRUE);
	m_wndChart.SetFocus();
}

CBCGPChartObject* SmCompChart::SetupIndicatorObjects(double dblValue, CBCGPChartAxis* pIndicatorAxis, const CString& strIndicatorName /*= _T("")*/)
{
	CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	ASSERT_VALID(pXAxis);

	// set overbought/oversold line and its axis mark
	if (dblValue != DBL_MAX)
	{
		const CBCGPChartTheme& colors = pChart->GetColors();

		CArray<float, float> dashes;
		dashes.Add(4.0);
		dashes.Add(4.0);

		CBCGPStrokeStyle style(dashes);

		CBCGPChartLineObject* pLine =
			pChart->AddChartLineObject(dblValue, TRUE, colors.m_brAxisMajorGridLineColor, 1.0, &style);
		pLine->SetRelatedAxes(pXAxis, pIndicatorAxis);
		pLine->SetForeground(FALSE);

		CString strText;
		strText.Format(_T("%.0f"), dblValue);

		CBCGPChartAxisMarkObject* pMark = pChart->AddChartAxisMarkObject(dblValue, strText, TRUE, FALSE);
		pMark->SetRelatedAxes(pXAxis, pIndicatorAxis);
	}

	// set indicator name; the name will be used to display last value
	if (!strIndicatorName.IsEmpty())
	{
		CBCGPRect rectLabel(5, -3, CBCGPChartObject::_EmptyCoordinate, CBCGPChartObject::_EmptyCoordinate);
		CBCGPChartObject* pTextObject = pChart->AddChartTextObject(rectLabel, strIndicatorName);

		pTextObject->SetRelatedAxes(pXAxis, pIndicatorAxis);
		return pTextObject;
	}

	return NULL;
}

void SmCompChart::ChangeTimeButtonColor(const DarkHorse::SmChartType& chart_type)
{
	if (chart_type == DarkHorse::SmChartType::MIN) {
		_BtnMon.SetFaceColor(RGB(127, 127, 127));
		_BtnWeek.SetFaceColor(RGB(127, 127, 127));
		_BtnDay.SetFaceColor(RGB(127, 127, 127));
		_BtnMin.SetFaceColor(RGB(255, 0, 0));
		_BtnTick.SetFaceColor(RGB(127, 127, 127));
	}
	else if (chart_type == DarkHorse::SmChartType::DAY) {
		_BtnMon.SetFaceColor(RGB(127, 127, 127));
		_BtnWeek.SetFaceColor(RGB(127, 127, 127));
		_BtnDay.SetFaceColor(RGB(255, 0, 0));
		_BtnMin.SetFaceColor(RGB(127, 127, 127));
		_BtnTick.SetFaceColor(RGB(127, 127, 127));
	}
	else if (chart_type == DarkHorse::SmChartType::WEEK) {
		_BtnMon.SetFaceColor(RGB(127, 127, 127));
		_BtnWeek.SetFaceColor(RGB(255, 0, 0));
		_BtnDay.SetFaceColor(RGB(127, 127, 127));
		_BtnMin.SetFaceColor(RGB(127, 127, 127));
		_BtnTick.SetFaceColor(RGB(127, 127, 127));
	}
	else if (chart_type == DarkHorse::SmChartType::MON) {
		_BtnMon.SetFaceColor(RGB(255, 0, 0));
		_BtnWeek.SetFaceColor(RGB(127, 127, 127));
		_BtnDay.SetFaceColor(RGB(127, 127, 127));
		_BtnMin.SetFaceColor(RGB(127, 127, 127));
		_BtnTick.SetFaceColor(RGB(127, 127, 127));
	}
	else if (chart_type == DarkHorse::SmChartType::TICK) {
		_BtnMon.SetFaceColor(RGB(127, 127, 127));
		_BtnWeek.SetFaceColor(RGB(127, 127, 127));
		_BtnDay.SetFaceColor(RGB(127, 127, 127));
		_BtnMin.SetFaceColor(RGB(127, 127, 127));
		_BtnTick.SetFaceColor(RGB(255, 0, 0));
	}
}

void SmCompChart::RequestChartData()
{
	if (!_CompData.RequestChartData(_UniqueId)) {
		KillTimer(COMP_REQUEST_TIMER);

		UpdateData();

		_CompData.MakeChartData();


		
		UpdateChart();

		//_bReady = true;

		CBCGPChartVisualObject* pChart = m_wndChart.GetChart();
		ASSERT_VALID(pChart);



		//CBCGPChartStockSeries* pStockSeries = DYNAMIC_DOWNCAST(CBCGPChartStockSeries, pChart->GetSeries(0));
		//pStockSeries->RemoveAllDataPoints();

		CCustomStockSeries* pMainSeries = (CCustomStockSeries*)pChart->GetSeries(0);
		pMainSeries->RemoveAllDataPoints();
		pMainSeries->SetExternalStorage(&_CompData.MainStorage);
		pMainSeries->m_strSeriesName = _CompData.KospiSymbolCode().c_str();

		int nStorageCount = _CompData.MainStorage.GetCount();



		/*CBCGPChartVisualObject* pChart = GetChart();*/
		CBCGPChartAxis* pXAxis = pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);

		// scroll range must be updated to show all data points in the storage
		if (m_bAutoScroll && !pChart->IsThumbTrackMode() && !pChart->IsPanMode())
		{
			// always scroll to the last added value if not paused;
			// otherwise scroll the chart while data points are still being added
			// add some number of values returned by GetRightOffsetAsNumberOfValues for extra 
			// offset from the right side (see SetRightOffsetInPixels in OnInitialUpdate)
			pXAxis->SetFixedMaximumDisplayValue(nStorageCount + pXAxis->GetRightOffsetAsNumberOfValues() - 1);
		}


		CCustomStockSeries* pCompSeries = (CCustomStockSeries*)pChart->GetSeries(1);
		pCompSeries->RemoveAllDataPoints();
		pCompSeries->SetExternalStorage(&_CompData.CompStorage);
		pCompSeries->m_strSeriesName = _CompData.NqSymbolCode().c_str();

		CCustomStockSeries* pVirSeries = (CCustomStockSeries*)pChart->GetSeries(2);
		pVirSeries->RemoveAllDataPoints();
		pVirSeries->SetExternalStorage(&_CompData.VirtualStorage);
		pVirSeries->m_strSeriesName = _CompData.NqSymbolCode().c_str();

		pChart->RecalcMinMaxValues();
		pChart->SetDirty();
		pChart->Redraw();
		m_wndChart.SetFocus();
	}
}

void SmCompChart::RequestChartData(const std::string symbol_code)
{
	std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindChartData(symbol_code, _ChartTimeType, _Cycle);

	if (chart_data) {
		SetChartData(chart_data, _UniqueId);
	}
	else {

		mainApp.ChartDataMgr()->RequestChartData(symbol_code, _ChartTimeType, _Cycle, _UniqueId, 0);

		mainApp.SymMgr()->RegisterSymbolToServer(symbol_code, true);
	}
}


void SmCompChart::OnCbnSelchangeComboTheme()
{
	m_nColorTheme = _ComboTheme.GetCurSel();

	if (m_nColorTheme == _ComboTheme.GetCount() - 2)
	{
		m_nColorTheme = -2;	// Custom color theme
	}
	else if (m_nColorTheme == _ComboTheme.GetCount() - 1)
	{
		m_nColorTheme = -1;	// Custom textures theme
	}

	UpdateChartColorTheme(m_nColorTheme, m_bIsDarkTheme);
}
