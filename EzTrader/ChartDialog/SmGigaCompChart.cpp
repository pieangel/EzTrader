#include "stdafx.h"
#include "../MainFrm.h"
#include "../DarkHorse.h"
#include "SmGigaCompChart.h"
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

#include "Pegrpapi.h"
#include <tchar.h>


using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

#define ID_NEXT_DATA_POINT	1

#define COMP_REFRESH_TIMER 1
#define COMP_REQUEST_TIMER 2

#define GetRandom( min, max ) ((rand() % (int)(((max)+1) - (min))) + (min))


static const int RSI_PERIOD = 14;
//static const CString RSI_INFO_FORMAT = _T("RSI(%d), Value: %.4f");
//static const CString OHLC_INFO_FORMAT = _T("O: %.4f, H: %.4f, L: %.4f, C: %.4f");

// SmGigaCompChart dialog

IMPLEMENT_DYNAMIC(SmGigaCompChart, CBCGPDialog)

SmGigaCompChart::SmGigaCompChart(CWnd* pParent /*=nullptr*/)
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

	m_hPE = NULL;
}

SmGigaCompChart::~SmGigaCompChart()
{
	KillTimer(COMP_REFRESH_TIMER);
	mainApp.CallbackMgr()->UnsubscribeChartCallback((long)this);
}

void SmGigaCompChart::DoDataExchange(CDataExchange* pDX)
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


BEGIN_MESSAGE_MAP(SmGigaCompChart, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_MON, &SmGigaCompChart::OnBnClickedBtnMon)
	ON_BN_CLICKED(IDC_BTN_WEEK, &SmGigaCompChart::OnBnClickedBtnWeek)
	ON_BN_CLICKED(IDC_BTN_DAY, &SmGigaCompChart::OnBnClickedBtnDay)
	ON_BN_CLICKED(IDC_BTN_MIN, &SmGigaCompChart::OnBnClickedBtnMin)
	ON_BN_CLICKED(IDC_BTN_TICK, &SmGigaCompChart::OnBnClickedBtnTick)
	ON_BN_CLICKED(IDC_BTN_SET, &SmGigaCompChart::OnBnClickedBtnSet)
	ON_BN_CLICKED(IDC_BTN_FIND, &SmGigaCompChart::OnBnClickedBtnFind)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &SmGigaCompChart::OnCbnSelchangeComboSymbol)
	ON_CBN_SELCHANGE(IDC_COMBO_CYCLE, &SmGigaCompChart::OnCbnSelchangeComboCycle)
	ON_CBN_SELCHANGE(IDC_COMBO_TICK, &SmGigaCompChart::OnCbnSelchangeComboTick)
	ON_CBN_SELCHANGE(IDC_COMBO_STYLE, &SmGigaCompChart::OnCbnSelchangeComboStyle)
	ON_REGISTERED_MESSAGE(BCGM_ON_CHART_AXIS_ZOOMED, OnChartAxisZoomed)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &SmGigaCompChart::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_THEME, &SmGigaCompChart::OnCbnSelchangeComboTheme)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// SmGigaCompChart message handlers


BOOL SmGigaCompChart::OnInitDialog()
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


	InitSymbolCombo();

	ChangeTimeButtonColor(_ChartTimeType);

	_CompData.MakeChartData();
	
	InitChart();


	SetTimer(COMP_REQUEST_TIMER, 700, NULL);

	mainApp.CallbackMgr()->SubscribeChartCallback((long)this, std::bind(&SmGigaCompChart::OnChartEvent, this, _1, _2));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmGigaCompChart::OnBnClickedBtnMon()
{
	_ChartTimeType = SmChartType::MON;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmGigaCompChart::OnBnClickedBtnWeek()
{
	_ChartTimeType = SmChartType::WEEK;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmGigaCompChart::OnBnClickedBtnDay()
{
	_ChartTimeType = SmChartType::DAY;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmGigaCompChart::OnBnClickedBtnMin()
{
	_ChartTimeType = SmChartType::MIN;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmGigaCompChart::OnBnClickedBtnTick()
{
	_ChartTimeType = SmChartType::TICK;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmGigaCompChart::OnBnClickedBtnSet()
{
	// TODO: Add your control notification handler code here
}


void SmGigaCompChart::OnBnClickedBtnFind()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	//_SymbolTableDlg->SpreadChart = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}


void SmGigaCompChart::OnCbnSelchangeComboSymbol()
{
	_CurIndex = _ComboSymbol.GetCurSel();
	auto found = _RowToSymbolMap.find(_CurIndex);
	if (found == _RowToSymbolMap.end()) return;

	
}


void SmGigaCompChart::OnCbnSelchangeComboCycle()
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


void SmGigaCompChart::OnCbnSelchangeComboTick()
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


void SmGigaCompChart::OnCbnSelchangeComboStyle()
{
	m_nLineType = _ComboStyle.GetCurSel();
	UpdateChart();
}

void SmGigaCompChart::OnClear()
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

LRESULT SmGigaCompChart::OnChartAxisZoomed(WPARAM wp, LPARAM lp)
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

void SmGigaCompChart::OnTimer(UINT_PTR nIDEvent)
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
			;
		}
	}
}

LRESULT SmGigaCompChart::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
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

	}
	else if (found_index != _CurIndex) {
		_ComboSymbol.SetCurSel(found_index);
		_CurIndex = found_index;

		auto found = _RowToSymbolMap.find(_CurIndex);
		if (found == _RowToSymbolMap.end()) return 1;

		std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindChartData(found->second->SymbolCode(), _ChartTimeType, _Cycle);

	}

	return 1;
}

void SmGigaCompChart::UpdateChartColorTheme(int nTheme, BOOL bIsDarkTheme)
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

void SmGigaCompChart::SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& window_id)
{
	if (!chart_data || _UniqueId != window_id) return;

	auto symbol = mainApp.SymMgr()->FindSymbol(chart_data->SymbolCode());
	if (!symbol) return;


}

void SmGigaCompChart::SetChartData()
{
	USES_CONVERSION;

	// 서브셋 갯수 설정
	PEnset(m_hPE, PEP_nSUBSETS, 6);
	// 전체 데이터 갯수 설정
	PEnset(m_hPE, PEP_nPOINTS, _CompData.GetDataFrame().shape().first);  // setting to a large value, later we will reduce //
	// single or double precision X axis data is used
	PEnset(m_hPE, PEP_bUSINGXDATAII, TRUE);
	PEnset(m_hPE, PEP_bUSINGYDATAII, TRUE);

	double d;
	double data_nasdaq, data_kospi200, data_vir1, data_vir2, data_delta1, data_delta2;
	int nCnt = 0;
	char szYear[3]; szYear[2] = 0;
	char szMonth[3]; szMonth[2] = 0;
	char szDay[3]; szDay[2] = 0;
	char szLabel[10];
	//TM tm;
	// Empty Data Before resetting //
	// 데이터 초기화
	data_nasdaq = 0;
	PEvsetcell(m_hPE, PEP_faYDATAII, -1, &data_nasdaq);  // -1 special index causes array to resize to value defined by f2
	d = 0;
	PEvsetcell(m_hPE, PEP_faXDATAII, -1, &d);  // -1 special index causes array to resize to value defined by d
	strcpy(szLabel, "0");
	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, -1, szLabel);  // -1 special index causes array to resize to value defined by szLabel


	const std::vector<hmdf::DateTime>& index = _CompData.GetDataFrame().get_index();
	const std::vector<double>& main = _CompData.GetDataFrame().get_column<double>("main");
	const std::vector<double>& comp = _CompData.GetDataFrame().get_column<double>("comp");
	const std::vector<double>& nk = _CompData.GetDataFrame().get_column<double>("nk");
	const std::vector<double>& hs = _CompData.GetDataFrame().get_column<double>("hs");
	const std::vector<double>& vir1 = _CompData.GetDataFrame().get_column<double>("vir1");
	const std::vector<double>& vir2 = _CompData.GetDataFrame().get_column<double>("vir2");
	const std::vector<double>& price_delta = _CompData.GetDataFrame().get_column<double>("price_delta");
	double main_divedend = 1, comp_divedend = 1;
	for (size_t i = 0; i < index.size(); i++) {
		auto symbol = mainApp.SymMgr()->FindSymbol(_CompData.KospiSymbolCode());
		if (symbol) {
			main_divedend = pow(10, symbol->Decimal());
			data_nasdaq = main[i] / main_divedend, index[i];
		}
		symbol = mainApp.SymMgr()->FindSymbol(_CompData.NqSymbolCode());
		if (symbol) {
			comp_divedend = pow(10, symbol->Decimal());
			data_kospi200 = comp[i] / comp_divedend, index[i];
			data_vir1 = vir1[i] / comp_divedend, index[i];
			data_vir2 = vir2[i] / comp_divedend, index[i];
			data_delta1 = price_delta[i] / comp_divedend, index[i];
			data_delta2 = price_delta[i] / comp_divedend, index[i];
		}
		/*
		tm.nYear = index[i].GetYear() - 1900;
		tm.nMonth = index[i].GetMonth();
		tm.nDay = index[i].GetDay();
		tm.nHour = index[i].GetHour();
		tm.nMinute = index[i].GetMinute();
		tm.nSecond = index[i].GetSecond();
		tm.nMilliSecond = 0;
		PEcreateserialdate(&d, &tm, 1);
		PEvsetcell(m_hPE, PEP_faXDATAII, nCnt, &d);

		// Pass OHLC Volume Data //
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 0, nCnt, &data_nasdaq); // Nasdaq data
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 1, nCnt, &data_kospi200); // Kospi200 data
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 2, nCnt, &data_vir1); // virtual 1
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 3, nCnt, &data_vir2); // virtual 2
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 4, nCnt, &data_delta1); // delta 1
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 5, nCnt, &data_delta2); // delta 2
		

		// Set PointLabel data, string representation of date //
		// 09/16/99 13:51:04

		//std::string dt_fmt = date_time[i].string_format(hmdf::DT_FORMAT::DT_TM2);
		CString fmt_dt = index[i].Format("%c");
		std::string dt_fmt(fmt_dt);
		// 09/16/99 13:51:04
		szLabel[0] = 0;
		strcpy_s(szMonth, sizeof(szMonth), dt_fmt.substr(0, 2).c_str());
		strcat(szLabel, szMonth);
		strcat(szLabel, "/");
		strcpy_s(szDay, sizeof(szDay), dt_fmt.substr(3, 2).c_str());
		strcat(szLabel, szDay);
		strcat(szLabel, "/");
		strcpy_s(szYear, sizeof(szYear), dt_fmt.substr(6, 2).c_str());
		strcat(szLabel, szYear);

		const wchar_t* x = A2W((LPCSTR)szLabel);
		PEvsetcell(m_hPE, PEP_szaPOINTLABELS, nCnt, (LPVOID)x);

		// Store first date as StartTime property.  You must set StartTime property!
		if (tm.nHour == 0)
			PEvset(m_hPE, PEP_fSTARTTIME, &d, 1);
			*/

		nCnt++;
	}

	

	// Set Points to number of records read, this requires PEP_bSUBSETBYPOINT be set to FALSE //
	PEnset(m_hPE, PEP_nPOINTS, nCnt);  // It's critical that points equals amount of XData passed
}

void SmGigaCompChart::OnChartEvent(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action)
{
	
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

void SmGigaCompChart::InitStyle()
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

void SmGigaCompChart::InitTheme()
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

void SmGigaCompChart::InitSymbolCombo()
{
	// 반드시 실시간 등록을 해줄것
	const std::map<int, std::shared_ptr<DarkHorse::SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		int index = _ComboSymbolMain.AddString(it->second->SymbolNameKr().c_str());
		_RowToMainSymbolMap[index] = it->second;
	}

	if (_RowToMainSymbolMap.size() > 0) {
		_ComboSymbolMain.SetCurSel(2);
		_CompData.SetKospiSymbol(_RowToMainSymbolMap[2]->SymbolCode());
		_CompData.NikkeiCode(_RowToMainSymbolMap[1]->SymbolCode());
		_CompData.HangSengCode(_RowToMainSymbolMap[5]->SymbolCode());
	}

	for (auto it = favorite_map.rbegin(); it != favorite_map.rend(); ++it) {
		int index = _ComboSymbolComp.AddString(it->second->SymbolNameKr().c_str());
		_RowToCompSymbolMap[index] = it->second;
	}

	if (_RowToCompSymbolMap.size() > 0) {
		_ComboSymbolComp.SetCurSel(0);
		_CompData.SetNqSymbol(_RowToCompSymbolMap.begin()->second->SymbolCode());

	}

	_ComboSymbolMain.SetDroppedWidth(300);
	_ComboSymbolComp.SetDroppedWidth(300);
}

void SmGigaCompChart::SetBarLineWidth()
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

void SmGigaCompChart::AddChartData()
{
	
}

void SmGigaCompChart::UpdateChartData()
{
	
}

void SmGigaCompChart::CreateChart()
{

}

void SmGigaCompChart::UpdateChart()
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

CBCGPChartObject* SmGigaCompChart::SetupIndicatorObjects(double dblValue, CBCGPChartAxis* pIndicatorAxis, const CString& strIndicatorName /*= _T("")*/)
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

void SmGigaCompChart::ChangeTimeButtonColor(const DarkHorse::SmChartType& chart_type)
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

void SmGigaCompChart::RequestChartData()
{
	
}

void SmGigaCompChart::RequestChartData(const std::string symbol_code)
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

void SmGigaCompChart::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (m_hPE)
	{
		CRect rcWnd;

		GetDlgItem(IDC_STATIC_CHART)->GetWindowRect(rcWnd);
		ScreenToClient(&rcWnd);

		//RECT r; GetClientRect(&r);
		::MoveWindow(m_hPE, rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, FALSE);
	}
}


void SmGigaCompChart::OnDestroy()
{
	CBCGPDialog::OnDestroy();

	if (m_hPE) { PEdestroy(m_hPE); m_hPE = 0; }
}


BOOL SmGigaCompChart::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CBCGPDialog::OnEraseBkgnd(pDC);
}


BOOL SmGigaCompChart::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);
	switch (wmEvent)
	{
	case PEWN_CLICKED:
		HOTSPOTDATA hsd; TCHAR buffer[128]; float yvalue;
		PEvget(m_hPE, PEP_structHOTSPOTDATA, &hsd);
		if (hsd.nHotSpotType == PEHS_DATAPOINT)
		{
			PEvgetcellEx(m_hPE, PEP_faYDATA, hsd.w1, hsd.w2, &yvalue);
			sprintf_s(buffer, TEXT("DataPoint %d value %.2f"), hsd.w2, yvalue);
			::MessageBox(this->m_hWnd, buffer, TEXT("Hello World"), 0);
		}
		///////////////////////////////////
		// Mouse Move Handler, OnCommand //
		///////////////////////////////////

		else if ((HIWORD(wParam) == PEWN_MOUSEMOVE))
		{
			int nX;
			int nY;
			double fX;
			double fY;
			double fI;
			POINT pt;
			int nA;
			TCHAR buffer[128];
			TCHAR szDate[48];
			TCHAR szF[48];
			float fHigh, fLow, fOpen, fClose, fVolume;

			// get last mouse location within control //
			PEvget(m_hPE, PEP_ptLASTMOUSEMOVE, &pt);

			nA = 0;      //Initialize axis, non-zero only if using MultiAxesSubsets
			nX = pt.x;   //Initialize nX and nY with mouse location
			nY = pt.y;
			PEconvpixeltograph(m_hPE, &nA, &nX, &nY, &fX, &fY, 0, 0, 0);

			// We now know data coordinates for mouse location //

			// Code to pick closest point index //
			fY = modf(fX, &fI);
			if (fY > .5)
				nX = (int)floor(fX);
			else
				nX = (int)floor(fX) - 1;

			if (fX < 1 || fX > 731)  // 731 points per subset in data file
			{
				lstrcpy(buffer, TEXT("        "));
				PEnset(m_hPE, PEP_nWORKINGTABLE, 0);
				PEvsetcellEx(m_hPE, PEP_szaTATEXT, 11, 0, buffer);
				PEvsetcellEx(m_hPE, PEP_szaTATEXT, 12, 0, buffer);
				PEvsetcellEx(m_hPE, PEP_szaTATEXT, 13, 0, buffer);
				PEvsetcellEx(m_hPE, PEP_szaTATEXT, 14, 0, buffer);
				PEvsetcellEx(m_hPE, PEP_szaTATEXT, 15, 0, buffer);
				PEvsetcellEx(m_hPE, PEP_szaTATEXT, 16, 0, buffer);
				PEdrawtable(m_hPE, 0, NULL);
				return TRUE;
			}

			fX -= 1.0F;

			// Get Data at closest point //
			PEvgetcellEx(m_hPE, PEP_faYDATA, 0, nX, &fHigh);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 1, nX, &fLow);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 2, nX, &fOpen);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 3, nX, &fClose);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 7, nX, &fVolume);
			PEvgetcell(m_hPE, PEP_szaPOINTLABELS, nX, szDate);

			// Place text in table annotation //
			// Get numeric precision //
			nX = PEnget(m_hPE, PEP_nDATAPRECISION);

			TCHAR szPrecision[10];
			_itot(nX, szPrecision, 10);

			// Place text in table annotation //
			PEnset(m_hPE, PEP_nWORKINGTABLE, 0);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 11, 0, szDate);

			lstrcpy(szF, TEXT("H:%."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f "));
			_stprintf(buffer, szF, fHigh);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 12, 0, buffer);

			lstrcpy(szF, TEXT("L:%."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f "));
			_stprintf(buffer, szF, fLow);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 13, 0, buffer);

			lstrcpy(szF, TEXT("O:%."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f "));
			_stprintf(buffer, szF, fOpen);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 14, 0, buffer);

			lstrcpy(szF, TEXT("C:%."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f "));
			_stprintf(buffer, szF, fClose);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 15, 0, buffer);

			_stprintf(buffer, TEXT("V:%.0f "), fVolume);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 16, 0, buffer);

			PEdrawtable(m_hPE, 0, NULL);

			return TRUE;
		}

		///////////////////////////////////////
		// Table Hot Spot Handler, OnCommand //
		///////////////////////////////////////

		else if ((HIWORD(wParam) == PEWN_CLICKED))
		{
			// Look for table hot spot and change financial data //

			HOTSPOTDATA hsd;

			// now look at HotSpotData structure //
			PEvget(m_hPE, PEP_structHOTSPOTDATA, &hsd);

			if (hsd.nHotSpotType == PEHS_TABLEANNOTATION + 0) // zero represents first table annotation
			{
				PEnset(m_hPE, PEP_bZOOMMODE, FALSE);
				PEnset(m_hPE, PEP_nPOINTSTOGRAPH, 0);

				// Change color of select table item //
				DWORD dwColor = PERGB(255, 142, 142, 142);
				for (int i = 0; i < 10; i++)
					PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, i, 0, &dwColor);
				dwColor = PERGB(255, 198, 0, 0);
				PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, hsd.w1, hsd.w2, &dwColor);

				TCHAR szSym[32];
				PEvgetcellEx(m_hPE, PEP_szaTATEXT, hsd.w1, hsd.w2, szSym);

				// OK, Change Data //
				//LoadData(&szSym[1]);  // [1] because there is a lead space character

				PEreinitialize(m_hPE);
				PEresetimage(m_hPE, 0, 0);
				::InvalidateRect(m_hPE, 0, 0);
			}

			// Look to see if a data hot spot was clicked //

			if (hsd.nHotSpotType == PEHS_DATAPOINT)
			{
				PEnset(m_hPE, PEP_nCURSORMODE, PECM_POINT); // Enable Vertical Cursor Mode.
				PEnset(m_hPE, PEP_nCURSORPOINT, hsd.w2);    // Set Cursor's focus selected point.
				PEresetimage(m_hPE, 0, 0);
			}
		}

		////////////////////////////////////
		// CustomTrackingDataText Handler //
		////////////////////////////////////

		else if ((HIWORD(wParam) == PEWN_CUSTOMTRACKINGDATATEXT))  // v9 feature
		{
			int nX, nY, nA;
			double fX, fY, fI;
			POINT pt;
			TCHAR buffer[256];
			TCHAR buffer2[256];
			TCHAR szDate[48];
			TCHAR szF[48];
			float fHigh, fLow, fOpen, fClose, fVolume;

			// get last mouse location within control //
			PEvget(m_hPE, PEP_ptLASTMOUSEMOVE, &pt);

			nA = 0;      //Initialize axis, non-zero only if using MultiAxesSubsets
			nX = pt.x;   //Initialize nX and nY with mouse location
			nY = pt.y;
			PEconvpixeltograph(m_hPE, &nA, &nX, &nY, &fX, &fY, 0, 0, 0);

			// We now know data coordinates for mouse location //

			// Code to pick closest point index //
			fY = modf(fX, &fI);
			if (fY > .5)
				nX = (int)floor(fX);
			else
				nX = (int)floor(fX) - 1;

			if (fX < 1 || fX > 731)  // 731 points per subset in data file
			{
				lstrcpy(buffer, TEXT("        "));
				PEszset(m_hPE, PEP_szTRACKINGTEXT, buffer);
				return TRUE;
			}
			fX -= 1.0F;

			if (!PEnget(m_hPE, PEP_nTRACKINGPROMPTTRIGGER) == PETPT_MOUSEMOVE)
			{
				// Trigger not mouse, must be a cursormove event, get cursor location...
				nX = PEnget(m_hPE, PEP_nCURSORPOINT);
			}

			PEvgetcellEx(m_hPE, PEP_faYDATA, 0, nX, &fHigh);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 1, nX, &fLow);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 2, nX, &fOpen);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 3, nX, &fClose);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 7, nX, &fVolume);
			PEvgetcell(m_hPE, PEP_szaPOINTLABELS, nX, szDate);

			// Place text in table annotation //
			// Get numeric precision //
			nX = PEnget(m_hPE, PEP_nDATAPRECISION);

			TCHAR szPrecision[10];
			_itot(nX, szPrecision, 10);

			lstrcpy(buffer, szDate);
			lstrcat(buffer, TEXT("      \n"));

			lstrcpy(szF, TEXT("H: %."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f \n"));
			_stprintf(buffer2, szF, fHigh);
			lstrcat(buffer, buffer2);

			lstrcpy(szF, TEXT("L: %."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f \n"));
			_stprintf(buffer2, szF, fLow);
			lstrcat(buffer, buffer2);

			lstrcpy(szF, TEXT("O: %."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f \n"));
			_stprintf(buffer2, szF, fOpen);
			lstrcat(buffer, buffer2);

			lstrcpy(szF, TEXT("C: %."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f \n"));
			_stprintf(buffer2, szF, fClose);
			lstrcat(buffer, buffer2);

			_stprintf(buffer2, TEXT("V: %.0f "), fVolume);
			lstrcat(buffer, buffer2);

			PEszset(m_hPE, PEP_szTRACKINGTEXT, buffer);

			return TRUE;
		}

		////////////////////////////////////
		// Cursor Move Handler, OnCommand //
		////////////////////////////////////

		else if ((HIWORD(wParam) == PEWN_CURSORMOVE))
		{
			int nX;
			TCHAR buffer[128];
			TCHAR szDate[24];
			TCHAR szF[24];

			nX = PEnget(m_hPE, PEP_nCURSORPOINT);

			float fHigh, fLow, fOpen, fClose, fVolume;

			// Get Data at closest point //
			PEvgetcellEx(m_hPE, PEP_faYDATA, 0, nX, &fHigh);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 1, nX, &fLow);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 2, nX, &fOpen);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 3, nX, &fClose);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 7, nX, &fVolume);
			PEvgetcell(m_hPE, PEP_szaPOINTLABELS, nX, szDate);

			// Get numeric precision //
			nX = PEnget(m_hPE, PEP_nDATAPRECISION);

			TCHAR szPrecision[10];
			_itot(nX, szPrecision, 10);

			// Place text in table annotation //
			PEnset(m_hPE, PEP_nWORKINGTABLE, 0);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 11, 0, szDate);

			lstrcpy(szF, TEXT("H:%."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f "));
			_stprintf(buffer, szF, fHigh);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 12, 0, buffer);

			lstrcpy(szF, TEXT("L:%."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f "));
			_stprintf(buffer, szF, fLow);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 13, 0, buffer);

			lstrcpy(szF, TEXT("O:%."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f "));
			_stprintf(buffer, szF, fOpen);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 14, 0, buffer);

			lstrcpy(szF, TEXT("C:%."));  lstrcat(szF, szPrecision); lstrcat(szF, TEXT("f "));
			_stprintf(buffer, szF, fClose);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 15, 0, buffer);

			_stprintf(buffer, TEXT("V:%.0f "), fVolume);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 16, 0, buffer);

			PEdrawtable(m_hPE, 0, NULL);
		}
		break;
	}

	return CBCGPDialog::OnCommand(wParam, lParam);
}

void SmGigaCompChart::InitChart()
{
	CRect rcWnd;
	GetDlgItem(IDC_STATIC_CHART)->GetWindowRect(rcWnd);
	ScreenToClient(&rcWnd);
	m_hPE = PEcreate(PECONTROL_GRAPH, WS_VISIBLE, &rcWnd, m_hWnd, 2001);

	// v9 features
	// 마우스 밑에 관련된 값을 표시해 준다.
	PEnset(m_hPE, PEP_bCURSORPROMPTTRACKING, TRUE);
	// 마우스 위치, 툴팁 표시
	PEnset(m_hPE, PEP_nCURSORPROMPTLOCATION, PECPL_TRACKING_TOOLTIP);
	// 마우스 커서 프롬프트 문자열 표시 방법 설정
	PEnset(m_hPE, PEP_nCURSORPROMPTSTYLE, PECPS_XYVALUES);
	// 툴팁 최대 길이
	PEnset(m_hPE, PEP_nTRACKINGTOOLTIPMAXWIDTH, 100);
	// 사용자 정의 텍스트 트래킹 여부 설정
	PEnset(m_hPE, PEP_bTRACKINGCUSTOMDATATEXT, TRUE);
	// 사용자가 데이터 포인트를 제어할지 결정
	PEnset(m_hPE, PEP_bMOUSECURSORCONTROL, TRUE);

	// Enable ZoomWindow //
	PEnset(m_hPE, PEP_bZOOMWINDOW, TRUE);
	// 마우스 휠로 줌을 할 때 한번에 얼마나 줌을 할지 결정. 1.01보다 커야 함.
	float fzf = 1.2F; PEvset(m_hPE, PEP_fMOUSEWHEELZOOMFACTOR, &fzf, 1);
	fzf = 1.1F; PEvset(m_hPE, PEP_fPINCHZOOMFACTOR, &fzf, 1);

	// Enable MouseWheel Zooming
	PEnset(m_hPE, PEP_nMOUSEWHEELFUNCTION, PEMWF_HORZ_ZOOM);
	PEnset(m_hPE, PEP_bMOUSEDRAGGINGX, TRUE);  // note that pan gestures require MouseDragging to be enabled 

	// Enable MouseWheel Zoom Smoothness
	PEnset(m_hPE, PEP_nMOUSEWHEELZOOMSMOOTHNESS, 4);
	PEnset(m_hPE, PEP_nPINCHZOOMSMOOTHNESS, 2);

	// Enable Bar Glass Effect //
	PEnset(m_hPE, PEP_bBARGLASSEFFECT, TRUE);

	// Enable Plotting style gradient and bevel features //
	PEnset(m_hPE, PEP_nAREAGRADIENTSTYLE, PEPGS_RADIAL_BOTTOM_RIGHT);
	PEnset(m_hPE, PEP_nAREABEVELSTYLE, PEBS_MEDIUM_SMOOTH);
	PEnset(m_hPE, PEP_nSPLINEGRADIENTSTYLE, PEPGS_RADIAL_BOTTOM_RIGHT);
	PEnset(m_hPE, PEP_nSPLINEBEVELSTYLE, PESBS_MEDIUM_SMOOTH);

	// No Flicker //
	PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
	PEnset(m_hPE, PEP_bCACHEBMP, TRUE);

	// Fixed Font Sizes //
	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);

	// YAxis Changes Range as you pan horizontally or zoom //
	PEnset(m_hPE, PEP_bSCROLLINGSCALECONTROL, TRUE);


	// Load Data and Add Studies, Total of 11 subsets when finished //
	// SubsetByPoint lets us add/remove points  //
	PEnset(m_hPE, PEP_bSUBSETBYPOINT, FALSE);
	//LoadData((LPSTR)("MSFT"));


	SetChartData();


	//! Set properties related to date/time mode //
	//PEnset(m_hPE, PEP_nDELTASPERDAY, 1);		// 1 data point per day
	//PEnset(m_hPE, PEP_nDELTAX, -1);				// -1 special code for daily data //


	PEnset(m_hPE, PEP_nDELTAX, 1);  //5 minutes between data points
	PEnset(m_hPE, PEP_nDELTASPERDAY, 1440);  //96 data points in one day
	//double srtime = 0.333333333333334F;
	//PEvset(m_hPE, PEP_fSTARTTIME, &srtime, 1);
	PEnset(m_hPE, PEP_bUSINGXDATAII, TRUE);

	PEnset(m_hPE, PEP_nDATETIMEMODE, PEDTM_VB); // How to interpret serial dates //
	PEnset(m_hPE, PEP_nYEARMONTHDAYPROMPT, PEDP_INSIDE_TOP);
	//PEnset(m_hPE, PEP_nDAYLABELTYPE, PEDLT_1_CHAR);
	//PEnset(m_hPE, PEP_nMONTHLABELTYPE, PEDLT_3_CHAR);

	// 각각의 축에 대하여 서브셋 갯수를 분할해서 각각 설정해준다.
	// Split up subsets among different axes //
	int nMAS[] = { 4, 1, 1 };
	PEvset(m_hPE, PEP_naMULTIAXESSUBSETS, nMAS, 3);
	// 각각의 축이 차지하는 비율을 정해준다. 모두 합해서 100%가 되어야 한다.
	float fMAP[] = { .80F, .10F, .10F };
	PEvset(m_hPE, PEP_faMULTIAXESPROPORTIONS, fMAP, 3);



	// Set up per axis properties //
	PEnset(m_hPE, PEP_nWORKINGAXIS, 0);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	// PEP_nCOMPARISONSUBSETS - 같은 축에 비교할 때
	// PEP_nRYAXISCOMPARISONSUBSETS - 다른 축에 비교할 때 
	PEnset(m_hPE, PEP_nRYAXISCOMPARISONSUBSETS, 3);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	PEnset(m_hPE, PEP_nWORKINGAXIS, 1);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	PEnset(m_hPE, PEP_nWORKINGAXIS, 2);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	PEnset(m_hPE, PEP_nWORKINGAXIS, 0);

	// Set Various Other Properties ///
	PEnset(m_hPE, PEP_nDATAPRECISION, 2);
	// 여러개의 축이 사용될때 분리시킬 것인지 같이 사용할 것인지 결정
	PEnset(m_hPE, PEP_nMULTIAXISSTYLE, PEMAS_GROUP_ALL_AXES);
	PEnset(m_hPE, PEP_nMULTIAXESSIZING, TRUE);
	PEnset(m_hPE, PEP_bYAXISONRIGHT, TRUE);
	//PEnset(m_hPE, PEP_bSPECIFICPLOTMODECOLOR, TRUE); // Enables mult-colored candle stick fills
	PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_NONE);
	PEnset(m_hPE, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);
	PEnset(m_hPE, PEP_nZOOMSTYLE, PEZS_RO2_NOT);

	PEnset(m_hPE, PEP_bALLOWDATAHOTSPOTS, TRUE);
	PEnset(m_hPE, PEP_nHOTSPOTSIZE, PEHSS_LARGE);

	PEnset(m_hPE, PEP_nSHOWXAXIS, PESA_GRIDNUMBERS);
	PEszset(m_hPE, PEP_szMAINTITLE, (LPSTR)(""));
	PEszset(m_hPE, PEP_szSUBTITLE, (LPSTR)(""));
	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_MEDIUM);
	PEnset(m_hPE, PEP_nPOINTSIZE, PEPS_SMALL);
	PEnset(m_hPE, PEP_bALLOWMAXIMIZATION, FALSE);
	PEnset(m_hPE, PEP_nGRIDLINECONTROL, PEGLC_BOTH);
	PEnset(m_hPE, PEP_bSIMPLELINELEGEND, TRUE);
	PEnset(m_hPE, PEP_bSIMPLEPOINTLEGEND, TRUE);
	PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE_TOP_OF_AXIS);
	PEnset(m_hPE, PEP_nAUTOMINMAXPADDING, 1);
	PEnset(m_hPE, PEP_nOHLCMINWIDTH, 12); // Controls minium candle-stick width //

	PEnset(m_hPE, PEP_nGRAPHPLUSTABLEMENU, PEMC_HIDE);
	PEnset(m_hPE, PEP_nTABLEWHATMENU, PEMC_HIDE);
	PEnset(m_hPE, PEP_nMULTIAXISSTYLEMENU, PEMC_SHOW);
	PEnset(m_hPE, PEP_nLEGENDLOCATIONMENU, PEMC_SHOW);
	PEnset(m_hPE, PEP_nSHOWTABLEANNOTATIONSMENU, PEMC_SHOW);
	PEnset(m_hPE, PEP_bALLOWANNOTATIONCONTROL, TRUE);

	


	// Set Subset Colors //
	DWORD dwArray[6] = { PERGB(255,160,160,160), PERGB(255,160, 160, 160),	PERGB(255,128,0,0), PERGB(255,0,128,0),
							 PERGB(80,235, 0, 0), PERGB(80,235, 235, 0) };
	PEvset(m_hPE, PEP_dwaSUBSETCOLORS, dwArray, 6);

	int nSLT = PELT_THINSOLID;
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 0, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 1, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 2, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 3, &nSLT);
	nSLT = PELT_MEDIUMTHINSOLID;
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 4, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 5, &nSLT);


	// Set Subset Labels //
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 0, (LPSTR)("Main"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 1, (LPSTR)("Comp"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 2, (LPSTR)("Vir1"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 3, (LPSTR)("Vir2"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 4, (LPSTR)("Delta1"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 5, (LPSTR)("Delta2"));


	PEnset(m_hPE, PEP_nGRADIENTBARS, 14);
	PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
	PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
	PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
	PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
	PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_MEDIUM);
	PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_SHADOWS);

	PEnset(m_hPE, PEP_nIMAGEADJUSTTOP, 75);
	PEnset(m_hPE, PEP_nIMAGEADJUSTLEFT, 75);
	PEnset(m_hPE, PEP_nIMAGEADJUSTRIGHT, 75);

	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_NO_BORDER);

	// Set some random annotations //       
	PEnset(m_hPE, PEP_bALLOWANNOTATIONCONTROL, TRUE);
	PEnset(m_hPE, PEP_nSHOWANNOTATIONTEXTMENU, PEMC_SHOW);

	double d;  	int t;   float f;
	int aCnt = 0;
	TCHAR szAnnotText[128];
	for (int annot = 0; annot < 15; annot++)
	{
		int nRndRow = (int)(GetRandom(1, 4));
		int nRndCol = (int)(GetRandom(1, PEnget(m_hPE, PEP_nPOINTS)));
		d = nRndCol + 1;   PEvsetcell(m_hPE, PEP_faGRAPHANNOTATIONX, aCnt, &d);
		PEvgetcellEx(m_hPE, PEP_faYDATA, nRndRow, nRndCol, &f);  d = (double)f;  PEvsetcell(m_hPE, PEP_faGRAPHANNOTATIONY, aCnt, &d);
		t = PEGAT_SMALLDOWNTRIANGLESOLID; PEvsetcell(m_hPE, PEP_naGRAPHANNOTATIONTYPE, aCnt, &t);
		t = PERGB(255, 255, 255, 255);  PEvsetcell(m_hPE, PEP_dwaGRAPHANNOTATIONCOLOR, aCnt, &t);
		PEvsetcell(m_hPE, PEP_szaGRAPHANNOTATIONTEXT, aCnt, (LPSTR)(""));
		t = PEAZ_GRAPH_AND_ZOOMWINDOW; PEvsetcell(m_hPE, PEP_naGRAPHANNOTATIONZOOM, aCnt, &t);
		aCnt++;
		d = nRndCol + 1;   PEvsetcell(m_hPE, PEP_faGRAPHANNOTATIONX, aCnt, &d);
		PEvgetcellEx(m_hPE, PEP_faYDATA, nRndRow, nRndCol, &f);  d = (double)f;  PEvsetcell(m_hPE, PEP_faGRAPHANNOTATIONY, aCnt, &d);
		t = PEGAT_POINTER; PEvsetcell(m_hPE, PEP_naGRAPHANNOTATIONTYPE, aCnt, &t);
		t = PERGB(255, 255, 255, 255);  PEvsetcell(m_hPE, PEP_dwaGRAPHANNOTATIONCOLOR, aCnt, &t);
		_stprintf(szAnnotText, (LPSTR)("Annotation %d"), annot);
		PEvsetcell(m_hPE, PEP_szaGRAPHANNOTATIONTEXT, aCnt, szAnnotText);
		aCnt++;
	}

	PEnset(m_hPE, PEP_bSHOWANNOTATIONS, TRUE);
	PEnset(m_hPE, PEP_nGRAPHANNOTATIONTEXTSIZE, 115);
	PEnset(m_hPE, PEP_nMAXIMUMSYMBOLSIZE, PEMPS_LARGE);
	PEnset(m_hPE, PEP_nGRAPHANNOTMINSYMBOLSIZE, PEMPS_LARGE);


	// v7.2 new features //
	PEnset(m_hPE, PEP_nPOINTGRADIENTSTYLE, PEPGS_VERTICAL_ASCENT_INVERSE);
	PEnset(m_hPE, PEP_dwPOINTBORDERCOLOR, PERGB(100, 0, 0, 0));
	PEnset(m_hPE, PEP_nLINESYMBOLTHICKNESS, 3);
	PEnset(m_hPE, PEP_nAREABORDER, 0);
	PEnset(m_hPE, PEP_nSOLIDLINEOVERAREA, 1);
	PEnset(m_hPE, PEP_bALLOWSVGEXPORT, TRUE);

	// Set export defaults //
	PEnset(m_hPE, PEP_nDPIX, 600);
	PEnset(m_hPE, PEP_nDPIY, 600);

	PEnset(m_hPE, PEP_nEXPORTSIZEDEF, PEESD_NO_SIZE_OR_PIXEL);
	PEnset(m_hPE, PEP_nEXPORTTYPEDEF, PEETD_PNG);
	PEnset(m_hPE, PEP_nEXPORTDESTDEF, PEEDD_CLIPBOARD);
	PEszset(m_hPE, PEP_szEXPORTUNITXDEF, (LPSTR)("1280"));
	PEszset(m_hPE, PEP_szEXPORTUNITYDEF, (LPSTR)("768"));
	PEnset(m_hPE, PEP_nEXPORTIMAGEDPI, 300);

	PEnset(m_hPE, PEP_nRENDERENGINE, PERE_DIRECT2D);

	PEreinitialize(m_hPE);
	PEresetimage(m_hPE, 0, 0);
}

void SmGigaCompChart::OnCbnSelchangeComboTheme()
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



int SmGigaCompChart::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}
