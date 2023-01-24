// SmMultiChart.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmMultiChart.h"
#include "afxdialogex.h"
#include "Pegrpapi.h"
#include <memory>
#include <map>
#include <windows.h>


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
// SmMultiChart dialog

#define GetRandom( min, max ) ((rand() % (int)(((max)+1) - (min))) + (min))

IMPLEMENT_DYNAMIC(SmMultiChart, CBCGPDialog)

SmMultiChart::SmMultiChart(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_DIALOG4, pParent)
{

}

SmMultiChart::~SmMultiChart()
{
}

void SmMultiChart::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SYMBOL_COMP, _ComboSymbolComp);
	DDX_Control(pDX, IDC_COMBO_SYMBOL_MAIN, _ComboSymbolMain);
}


BEGIN_MESSAGE_MAP(SmMultiChart, CBCGPDialog)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// SmMultiChart message handlers


int SmMultiChart::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//! This examples shows Date/Time handling vith the Graph Object.
   //! The Graph is suited to show discontinuous date/time scales.
   //! Discontinuous for example, 8am to 5pm, Monday to Friday.

   //! Here's a tip, when debugging your data-passing logic, use the
   //! built-in text export feature to verify that data is as
   //! expected.

	



	return 0;
}


BOOL SmMultiChart::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void SmMultiChart::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (m_hPE)
	{
		CRect r; GetClientRect(&r);
		::MoveWindow(m_hPE, 0, 0, r.Width(), r.Height(), FALSE);
	}

}


void SmMultiChart::OnDestroy()
{
	CBCGPDialog::OnDestroy();

	// TODO: Add your message handler code here
}


BOOL SmMultiChart::OnCommand(WPARAM wParam, LPARAM lParam)
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
			sprintf_s(buffer, (LPTSTR)("DataPoint %d value %.2f"), hsd.w2, yvalue);
			::MessageBox(this->m_hWnd, buffer, (LPTSTR)("Hello World"), 0);
		}
		break;
	}


	return CBCGPDialog::OnCommand(wParam, lParam);
}


void SmMultiChart::InitCombo()
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

void SmMultiChart::SetChart()
{
	_CompData.MakeChartData();
	USES_CONVERSION;

	// 서브셋 갯수 설정
	PEnset(m_hPE, PEP_nSUBSETS, 6);
	// 전체 데이터 갯수 설정
	PEnset(m_hPE, PEP_nPOINTS, _CompData.GetDataFrame().shape().first);  // setting to a large value, later we will reduce //

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
	TM tm;
	// Empty Data Before resetting //
	// 데이터 초기화
	data_nasdaq = 0;
	PEvsetcell(m_hPE, PEP_faYDATAII, -1, &data_nasdaq);  // -1 special index causes array to resize to value defined by f2
	d = 0;
	PEvsetcell(m_hPE, PEP_faXDATAII, -1, &d);  // -1 special index causes array to resize to value defined by d
	strcpy(szLabel, "0");
	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, -1, szLabel);  // -1 special index causes array to resize to value defined by szLabel

	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 0, (LPTSTR)("Nasdaq"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 1, (LPTSTR)("Kospi200"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 2, (LPTSTR)("Vir1"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 3, (LPTSTR)("Vir2"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 3, (LPTSTR)("Delta1"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 3, (LPTSTR)("Delta2"));


	const std::vector<hmdf::DateTime>& index = _CompData.GetDataFrame().get_index();
	const std::vector<double>& main = _CompData.GetDataFrame().get_column<double>("main");
	const std::vector<double>& comp = _CompData.GetDataFrame().get_column<double>("comp");
	const std::vector<double>& nk = _CompData.GetDataFrame().get_column<double>("nk");
	const std::vector<double>& hs = _CompData.GetDataFrame().get_column<double>("hs");
	const std::vector<double>& vir1 = _CompData.GetDataFrame().get_column<double>("vir1");
	const std::vector<double>& vir2 = _CompData.GetDataFrame().get_column<double>("vir2");
	const std::vector<double>& price_delta = _CompData.GetDataFrame().get_column<double>("price_delta");
	double main_divedend = 1, comp_divedend = 1;
	bool set_start_time = false;
	for (size_t i = 0; i < index.size(); i++) {
		auto symbol = mainApp.SymMgr()->FindSymbol(_CompData.KospiSymbolCode());
		if (symbol) {
			main_divedend = pow(10, symbol->Decimal());
			data_nasdaq = (double)(main[i] / main_divedend);
		}
		symbol = mainApp.SymMgr()->FindSymbol(_CompData.NqSymbolCode());
		if (symbol) {
			comp_divedend = pow(10, symbol->Decimal());
			data_kospi200 = comp[i] / comp_divedend;
			data_vir1 = vir1[i] / comp_divedend;
			data_vir2 = vir2[i] / comp_divedend;
			data_delta1 = price_delta[i] / comp_divedend;
			data_delta2 = price_delta[i] / comp_divedend;
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


		PEdecipherserialdate(&d, &tm, 1);

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
		PEvsetcell(m_hPE, PEP_szaPOINTLABELS, nCnt, (LPVOID)szLabel);

		// Store first date as StartTime property.  You must set StartTime property!
		if (!set_start_time && index[i].GetHour() == 0) {
			PEvset(m_hPE, PEP_fSTARTTIME, &d, 1);
			set_start_time = true;
		}
		*/
		nCnt++;
	}

	// Set Points to number of records read, this requires PEP_bSUBSETBYPOINT be set to FALSE //
	PEnset(m_hPE, PEP_nPOINTS, nCnt);  // It's critical that points equals amount of XData passed

	PEnset(m_hPE, PEP_nWORKINGAXIS, 0);
	//PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
}

BOOL SmMultiChart::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	InitCombo();

	int i = 0;
	//double d, d2;
	RECT rect;
	GetClientRect(&rect);
	int c = 60000;


	PEchangeresources((LPSTR)"PEGRC32D.dll", 129, 1, 1);

	// Construct a Graph Object //
	m_hPE = PEcreate(PECONTROL_GRAPH, WS_VISIBLE, &rect, m_hWnd, 1001);

	if (m_hPE)
	{
		// Enable MouseWheel and Pinch Smoothing //
		PEnset(m_hPE, PEP_nMOUSEWHEELZOOMSMOOTHNESS, 5);
		PEnset(m_hPE, PEP_nPINCHZOOMSMOOTHNESS, 2);
		float fzf = 1.4F; PEvset(m_hPE, PEP_fMOUSEWHEELZOOMFACTOR, &fzf, 1);
		fzf = 1.15F; PEvset(m_hPE, PEP_fPINCHZOOMFACTOR, &fzf, 1);

		// Enable MouseWheel Zooming
		PEnset(m_hPE, PEP_nMOUSEWHEELFUNCTION, PEMWF_HORZ_ZOOM);

		// Enable ZoomWindow Feature //
		PEnset(m_hPE, PEP_bZOOMWINDOW, 1);

		// Enable middle mouse dragging //
		PEnset(m_hPE, PEP_bMOUSEDRAGGINGX, TRUE);
		PEnset(m_hPE, PEP_bMOUSEDRAGGINGY, TRUE);

		// Enable Plotting style gradient and bevel features //
		PEnset(m_hPE, PEP_nAREABEVELSTYLE, PEBS_THIN_SMOOTH);
		PEnset(m_hPE, PEP_nSPLINEBEVELSTYLE, PESBS_THIN_SMOOTH);

		// v7.2 new features //
		PEnset(m_hPE, PEP_nAREABORDER, 1);
		PEnset(m_hPE, PEP_bALLOWSVGEXPORT, 1);

		// Define how much data is in chart //
		//PEnset(m_hPE, PEP_nSUBSETS, 6);
		//PEnset(m_hPE, PEP_nPOINTS, 1500);

		// Set Date Time Handling related properties //
		PEnset(m_hPE, PEP_nDELTAX, 1);  //5 minutes between data points
		PEnset(m_hPE, PEP_nDELTASPERDAY, 1440);  //96 data points in one day
		//double srtime = 0.333333333333334F;
		//PEvset(m_hPE, PEP_fSTARTTIME, &srtime, 1);
		PEnset(m_hPE, PEP_bUSINGXDATAII, TRUE);

		

		SetChart();

		// This empties PointLabels array and invokes virtual point labels //
		// which are the point number.
		//PEvsetcell(m_hPE, PEP_szaPOINTLABELS, -1, nullptr);

		// You can modify date label formats with //
		PEnset(m_hPE, PEP_nYEARMONTHDAYPROMPT, 2);

		// PEP_nTIMELABELTYPE
		// PEP_nDAYLABELTYPE	
		// PEP_nMONTHLABELTYPE
		// PEP_nYEARLABELTYPE 

		// Enable DateTimeMode //
		PEnset(m_hPE, PEP_nDATETIMEMODE, PEDTM_VB);

		// Enable Zooming, zoom the chart and see how structure //
		// of x axis changes as you zoom //
		PEnset(m_hPE, PEP_nALLOWZOOMING, PEAZ_HORIZONTAL);

		// Set various properties //
		PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
		DWORD rgbcol = PERGB(255, 192, 192, 192);
		PEvset(m_hPE, PEP_dwDESKCOLOR, &rgbcol, 1);
		rgbcol = PERGB(48, 0, 215, 0);
		PEvset(m_hPE, PEP_dwGRAPHBACKCOLOR, &rgbcol, 1);
		PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, 0, &rgbcol);
		rgbcol = PERGB(255, 255, 255, 255);
		PEvset(m_hPE, PEP_dwGRAPHFORECOLOR, &rgbcol, 1);
		PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_NONE);
		PEnset(m_hPE, PEP_nGRIDLINECONTROL, PEGLC_BOTH);
		PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
		PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
		PEnset(m_hPE, PEP_nZOOMSTYLE, PEZS_RO2_NOT);

		PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
		PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_LIGHT_LINE);

		PEnset(m_hPE, PEP_nGRADIENTBARS, 8);
		PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
		PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
		PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
		PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
		PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
		PEnset(m_hPE, PEP_nFONTSIZE, PEFS_MEDIUM);
		PEnset(m_hPE, PEP_bSCROLLINGSCALECONTROL, TRUE);
		PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);

		PEnset(m_hPE, PEP_nSOLIDLINEOVERAREA, 1);

		DWORD dwArray[2] = { PERGB(175,198,0,0), PERGB(85,0, 0, 198) };
		PEvsetEx(m_hPE, PEP_dwaSUBSETCOLORS, 0, 2, dwArray, 0);

		int nGradient[2] = { PEPGS_RADIAL_TOP_LEFT, PEPGS_RADIAL_BOTTOM_RIGHT };
		PEvsetW(m_hPE, PEP_naSUBSETGRADIENTSTYLE, nGradient, 2);

		PEnset(m_hPE, PEP_bSHOWLEGEND, FALSE);

		PEszset(m_hPE, PEP_szMAINTITLE, (LPTSTR)("차트 테스트"));
		PEszset(m_hPE, PEP_szSUBTITLE, (LPTSTR)("repeat using popup menu to adjust x axis point labels vertical"));
		PEszset(m_hPE, PEP_szXAXISLABEL, (LPTSTR)("Zoom Gradually inward and study x axis"));

		// Set export defaults //
		PEnset(m_hPE, PEP_nDPIX, 600);
		PEnset(m_hPE, PEP_nDPIY, 600);

		PEnset(m_hPE, PEP_nEXPORTSIZEDEF, PEESD_NO_SIZE_OR_PIXEL);
		PEnset(m_hPE, PEP_nEXPORTTYPEDEF, PEETD_PNG);
		PEnset(m_hPE, PEP_nEXPORTDESTDEF, PEEDD_CLIPBOARD);
		PEszset(m_hPE, PEP_szEXPORTUNITXDEF, (LPTSTR)("1280"));
		PEszset(m_hPE, PEP_szEXPORTUNITYDEF, (LPTSTR)("768"));
		PEnset(m_hPE, PEP_nEXPORTIMAGEDPI, 300);

		// v9 features
		PEnset(m_hPE, PEP_bCURSORPROMPTTRACKING, TRUE);
		PEnset(m_hPE, PEP_nCURSORPROMPTLOCATION, PECPL_TRACKING_TOOLTIP);
		PEnset(m_hPE, PEP_nCURSORPROMPTSTYLE, PECPS_XYVALUES);
		PEnset(m_hPE, PEP_nTRACKINGTOOLTIPMAXWIDTH, 100);

		PEnset(m_hPE, PEP_nRENDERENGINE, PERE_DIRECT2D);

	}


	if (m_hPE)
	{
		CRect r; GetClientRect(&r);
		::MoveWindow(m_hPE, 0, 0, r.Width(), r.Height() , FALSE);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
