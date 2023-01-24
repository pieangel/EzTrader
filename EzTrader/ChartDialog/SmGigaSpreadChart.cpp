// SmGigaSpreadChart.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "../MainFrm.h"
#include "SmGigaSpreadChart.h"
#include "afxdialogex.h"
#include "Pegrpapi.h"
#include <tchar.h>
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
#include "../DataFrame/DataFrame.h"

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

#define ID_NEXT_DATA_POINT	1

// SmGigaSpreadChart dialog
#define GetRandom( min, max ) ((rand() % (int)(((max)+1) - (min))) + (min))
IMPLEMENT_DYNAMIC(SmGigaSpreadChart, CBCGPDialog)

SmGigaSpreadChart::SmGigaSpreadChart(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_GIGA_SPREAD_CHART, pParent)
{
	_UniqueId = CMainFrame::GetId();

	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	SetWhiteBackground(FALSE);

	m_hPE = NULL;

	m_nChartType = 0;
	m_nOverlayLine = 0;
	m_nOverlayBand = 2;
	//}}AFX_DATA_INIT

	m_pOverlaySeriesLine = NULL;
	m_pOverlaySeriesBand = NULL;

	m_nLineType = 0;
	m_bAutoScroll = TRUE;
	m_bDataTable = FALSE;
	//}}AFX_DATA_INIT

	m_dblUpTrendLimit = 0.5;
	m_pRSISeries = NULL;
	m_pRSIInfoObject = NULL;
	m_pOHLCInfo = NULL;

	m_bIsDarkTheme = TRUE;
}

SmGigaSpreadChart::~SmGigaSpreadChart()
{
	KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeChartCallback((long)this);
}

void SmGigaSpreadChart::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_CYCLE, _ComboCycle);
	DDX_Control(pDX, IDC_COMBO_STYLE, _ComboStyle);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbol);
	DDX_Control(pDX, IDC_COMBO_TICK, _ComboTick);
	DDX_Control(pDX, IDC_BTN_DAY, _BtnDay);
	DDX_Control(pDX, IDC_BTN_MIN, _BtnMin);
	DDX_Control(pDX, IDC_BTN_MON, _BtnMon);
	DDX_Control(pDX, IDC_BTN_TICK, _BtnTick);
	DDX_Control(pDX, IDC_BTN_WEEK, _BtnWeek);
	DDX_Control(pDX, IDC_COMBO_THEME, _ComboTheme);
}


BEGIN_MESSAGE_MAP(SmGigaSpreadChart, CBCGPDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()

	ON_BN_CLICKED(IDC_BTN_MON, &SmGigaSpreadChart::OnBnClickedBtnMon)
	ON_BN_CLICKED(IDC_BTN_WEEK, &SmGigaSpreadChart::OnBnClickedBtnWeek)
	ON_BN_CLICKED(IDC_BTN_DAY, &SmGigaSpreadChart::OnBnClickedBtnDay)
	ON_BN_CLICKED(IDC_BTN_MIN, &SmGigaSpreadChart::OnBnClickedBtnMin)
	ON_BN_CLICKED(IDC_BTN_TICK, &SmGigaSpreadChart::OnBnClickedBtnTick)
	ON_BN_CLICKED(IDC_BTN_SET, &SmGigaSpreadChart::OnBnClickedBtnSet)
	ON_BN_CLICKED(IDC_BTN_FIND, &SmGigaSpreadChart::OnBnClickedBtnFind)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &SmGigaSpreadChart::OnCbnSelchangeComboSymbol)
	ON_CBN_SELCHANGE(IDC_COMBO_CYCLE, &SmGigaSpreadChart::OnCbnSelchangeComboCycle)
	ON_CBN_SELCHANGE(IDC_COMBO_TICK, &SmGigaSpreadChart::OnCbnSelchangeComboTick)
	ON_CBN_SELCHANGE(IDC_COMBO_STYLE, &SmGigaSpreadChart::OnCbnSelchangeComboStyle)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &SmGigaSpreadChart::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_THEME, &SmGigaSpreadChart::OnCbnSelchangeComboTheme)
END_MESSAGE_MAP()


// SmGigaSpreadChart message handlers


void SmGigaSpreadChart::LoadData(TCHAR* pName)
{
	// Function to load financial data from a comma delimited file //
	USES_CONVERSION;

	// 서브셋 갯수 설정
	PEnset(m_hPE, PEP_nSUBSETS, 11);
	// 전체 데이터 갯수 설정
	PEnset(m_hPE, PEP_nPOINTS, 1000);  // setting to a large value, later we will reduce //
	// single or double precision X axis data is used
	PEnset(m_hPE, PEP_bUSINGXDATAII, TRUE);

	double d;
	TCHAR szFile[48];
	float f1, f2, f3, f4, f5;
	int nCnt = 0;
	int result;
	char szYear[3]; szYear[2] = 0;
	char szMonth[3]; szMonth[2] = 0;
	char szDay[3]; szDay[2] = 0;
	char szLabel[10];
	TM tm;

	tm.nHour = 12;
	tm.nMinute = 0;
	tm.nSecond = 0;
	tm.nMilliSecond = 0;

	lstrcpy(szFile, pName);
	lstrcat(szFile, TEXT(".txt"));

	// Empty Data Before resetting //
	// 데이터 초기화
	f2 = 0;
	PEvsetcell(m_hPE, PEP_faYDATA, -1, &f2);  // -1 special index causes array to resize to value defined by f2
	d = 0;
	PEvsetcell(m_hPE, PEP_faXDATAII, -1, &d);  // -1 special index causes array to resize to value defined by d
	strcpy(szLabel, "0");
	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, -1, szLabel);  // -1 special index causes array to resize to value defined by szLabel

	// Open Data File //
	FILE* pFile = NULL;

	std::string path = "C:\\Project\\DarkHorse\\Debug\\MSFT.txt";

	_tcscpy(szFile, path.c_str());

	pFile = _tfopen(szFile, TEXT("r"));

	std::vector<double> temp_data(1500);

	if (pFile)
	{
		// Count records //
		while (!feof(pFile))
		{
			// Parse out different fields //
			result = fscanf(pFile, "%c%c%c%c%c%c,%f,%f,%f,%f,%f\n", &szYear[0], &szYear[1], &szMonth[0], &szMonth[1], &szDay[0], &szDay[1], &f1, &f2, &f3, &f4, &f5);

			if (result == 11)
			{
				// Create serial date from string data and pass into XDataII //
				tm.nYear = atoi(szYear);
				if (tm.nYear > 90)
					tm.nYear = tm.nYear + 1900;
				else if (tm.nYear < 10)
					tm.nYear = tm.nYear + 2000;
				tm.nYear = tm.nYear - 1900;
				tm.nMonth = atoi(szMonth);
				tm.nDay = atoi(szDay);
				PEcreateserialdate(&d, &tm, 1);
				PEvsetcell(m_hPE, PEP_faXDATAII, nCnt, &d);

				// Pass OHLC Volume Data //
				//PEvsetcellEx(m_hPE, PEP_faYDATA, 0, nCnt, &temp_data[nCnt]); //High
				PEvsetcellEx(m_hPE, PEP_faYDATA, 0, nCnt, &f2); //High
				PEvsetcellEx(m_hPE, PEP_faYDATA, 1, nCnt, &f3); //Low
				PEvsetcellEx(m_hPE, PEP_faYDATA, 2, nCnt, &f1); //Open
				PEvsetcellEx(m_hPE, PEP_faYDATA, 3, nCnt, &f4); //Close
				PEvsetcellEx(m_hPE, PEP_faYDATA, 7, nCnt, &f5); //Volume into 8th subset

				// Set PointLabel data, string representation of date //
				szLabel[0] = 0;
				strcat(szLabel, szMonth);
				strcat(szLabel, "/");
				strcat(szLabel, szDay);
				strcat(szLabel, "/");
				strcat(szLabel, szYear);

				const wchar_t* x = A2W((LPCSTR)szLabel);
				PEvsetcell(m_hPE, PEP_szaPOINTLABELS, nCnt, (LPVOID)x);

				// Store first date as StartTime property.  You must set StartTime property!
				if (nCnt == 0)
					PEvset(m_hPE, PEP_fSTARTTIME, &d, 1);

				nCnt++;
			}
		}
		fclose(pFile);

		// We now have data, let's make some studies //

		int ID = 0;
		float f;
		double Days;
		int pnt, i;
		double Total = 0;
		double Total2 = 0;
		DWORD dw;
		int X = 1;

		double SMAfBB, BBNum;

		float* pYD3;
		pYD3 = new float[nCnt];
		if (!pYD3)
			return;
		// 값을 가져온다.
		for (pnt = 0; pnt < nCnt; pnt++)
			PEvgetcellEx(m_hPE, PEP_faYDATA, 3, pnt, &pYD3[pnt]);

		// Middle Band = SMA
		Days = 20;
		for (pnt = 0; pnt < (nCnt - Days); pnt++)
		{
			Total = 0;
			for (i = 0 + pnt; i < Days + pnt; i++)
				Total = Total + pYD3[i];
			f = Total / Days;
			PEvsetcellEx(m_hPE, PEP_faYDATA, 5, pnt + Days - 1, &f);
		}

		// Upper Band = Middle Band + [Standard Deviation * Square Root{[Sum for x Days((Closing Price - Middle Band)^2)]/Days]
		for (pnt = 0; pnt < (nCnt - Days); pnt++)
		{
			Total = 0;
			for (i = 0 + pnt; i < Days + pnt; i++)
				Total = Total + pYD3[i];
			SMAfBB = Total / Days;
			// Find sum of Closing Price minus SMA over days and square
			BBNum = 0;
			for (i = 0 + pnt; i < Days + pnt; i++)
				BBNum = ((pYD3[i] - SMAfBB) * (pYD3[i] - SMAfBB)) + BBNum;

			f = SMAfBB + 2.0F * sqrt(BBNum / Days);
			PEvsetcellEx(m_hPE, PEP_faYDATA, 4, pnt + Days - 1, &f);  // upper
		}

		// Lower Band = Middle Band - [Standard Deviation * Square Root{[Sum for x Days((Closing Price - Middle Band)^2)]/Days]
		for (pnt = 0; pnt < (nCnt - Days); pnt++)
		{
			Total = 0;
			for (i = 0 + pnt; i < Days + pnt; i++)
				Total = Total + pYD3[i];
			SMAfBB = Total / Days;
			// Find sum of Closing Price minus SMA over days and square
			BBNum = 0;
			for (i = 0 + pnt; i < Days + pnt; i++)
				BBNum = ((pYD3[i] - SMAfBB) * (pYD3[i] - SMAfBB)) + BBNum;

			f = SMAfBB - 2.0F * sqrt(BBNum / Days);
			PEvsetcellEx(m_hPE, PEP_faYDATA, 6, pnt + Days - 1, &f);  // upper
		}

		Total = 0;
		Total2 = 0;
		float RS = 0;
		float RSI = 0;
		X = 1;
		int LowerCount = 0;
		int UpperCount = 0;

		// How many days do you want to check
		Days = 10;

		float* pUpperArray;
		float* pLowerArray;
		pUpperArray = new float[nCnt];
		pLowerArray = new float[nCnt];

		for (i = 1; i <= Days; i++)
		{
			if ((pYD3[i] - pYD3[i - 1]) < 0)
			{
				pLowerArray[LowerCount] = (pYD3[i] - pYD3[i - 1]);
				LowerCount = LowerCount + 1;
			}
			else
			{
				pUpperArray[UpperCount] = (pYD3[i] - pYD3[i - 1]);
				UpperCount = UpperCount + 1;
			}
		}

		// Total the amounts over the period
		Total = 0;
		for (i = 0; i < LowerCount; i++)
			Total = pLowerArray[i] + Total;

		Total2 = 0;
		for (i = 0; i < UpperCount; i++)
			Total2 = pUpperArray[i] + Total2;

		// Divide up by down
		RS = (Total2 / Days) / (fabs(Total) / Days);

		// Use the RSI formula
		RSI = 100.0 - (100.0 / (1.0 + RS));
		PEvsetcellEx(m_hPE, PEP_faYDATA, 8, Days - 1, &RSI);

		for (i = Days; i < nCnt; i++)
		{
			if (pYD3[i] != 0 && pYD3[i - 1] != 0)
			{
				Total = Total * (Days - 1);
				Total2 = Total2 * (Days - 1);

				if ((pYD3[i] - pYD3[i - 1]) < 0)
					Total = (pYD3[i] - pYD3[i - 1]) + Total;
				else
					Total2 = (pYD3[i] - pYD3[i - 1]) + Total2;

				Total = Total / Days;
				Total2 = Total2 / Days;

				RS = (Total2) / (fabs(Total));
				RSI = 100.0 - (100.0 / (1.0 + RS));
				PEvsetcellEx(m_hPE, PEP_faYDATA, 8, Days - 1 + i, &RSI);
			}
		}

		// Add a mid axis line annotation
		d = 50.0;			PEvsetcell(m_hPE, PEP_faHORZLINEANNOTATION, 0, &d);
		i = 2;				PEvsetcell(m_hPE, PEP_naHORZLINEANNOTATIONAXIS, 0, &i);
		i = 5;				PEvsetcell(m_hPE, PEP_naHORZLINEANNOTATIONTYPE, 0, &i);
		dw = PERGB(255, 198, 0, 0);	PEvsetcell(m_hPE, PEP_dwaHORZLINEANNOTATIONCOLOR, 0, &dw);
		PEnset(m_hPE, PEP_bSHOWANNOTATIONS, TRUE);

		// Set the working axis extents
		PEnset(m_hPE, PEP_nWORKINGAXIS, 2);
		PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
		d = 0.0;			PEvset(m_hPE, PEP_fMANUALMINY, &d, 1);
		d = 100.0;			PEvset(m_hPE, PEP_fMANUALMAXY, &d, 1);

		/////////////////////////////////////////////////////////////////////
		// Stochastic Oscillator //
		///////////////////////////

		int SwFac, Dperiod, q;
		float LowMin, HighMax, SlowK, Dfactor, PercentD;
		LowMin = 9999.9F;
		HighMax = 0.0F;

		float* pSlowOC;
		float* pKperiod;
		float* pHigh;
		float* pLow;

		pSlowOC = new float[nCnt];
		pKperiod = new float[nCnt];
		pHigh = new float[nCnt];
		pLow = new float[nCnt];

		for (pnt = 0; pnt < nCnt; pnt++)
		{
			PEvgetcellEx(m_hPE, PEP_faYDATA, 0, pnt, &pHigh[pnt]);
			PEvgetcellEx(m_hPE, PEP_faYDATA, 1, pnt, &pLow[pnt]);
		}

		// How many days do you want to check
		Days = 30;
		SwFac = 1;
		Dperiod = 15;

		for (pnt = 0; pnt < (nCnt - Days); pnt++)
		{
			for (i = pnt; i < Days + pnt; i++)
			{
				if (pHigh[i] != 0)
				{
					if (pHigh[i] > HighMax)
						HighMax = pHigh[i];
				}
			}
			for (i = pnt; i < Days + pnt; i++)
			{
				if (pLow[i] != 0)
				{
					if (pLow[i] < LowMin)
						LowMin = pLow[i];
				}
			}

			if (pYD3[(int)(Days - 1 + pnt)] != 0)
				pKperiod[pnt] = ((pYD3[(int)(Days - 1 + pnt)] - LowMin) / (HighMax - LowMin)) * 100.0F;

			LowMin = 9999.9F;
			HighMax = 0.0F;
		}

		for (pnt = 0; pnt < nCnt - SwFac; pnt++)
		{
			SlowK = 0;
			for (q = pnt; q < SwFac + pnt; q++)
				SlowK = pKperiod[q] + SlowK;

			pSlowOC[pnt] = SlowK / SwFac;
			PEvsetcellEx(m_hPE, PEP_faYDATA, 9, Days + pnt + SwFac, &pSlowOC[pnt]);
		}

		for (pnt = 0; pnt < nCnt - Dperiod; pnt++)
		{
			PercentD = 0;
			for (q = pnt; q < Dperiod + pnt; q++)
				PercentD = pSlowOC[q] + PercentD;
			Dfactor = PercentD / Dperiod;
			PEvsetcellEx(m_hPE, PEP_faYDATA, 10, Days + pnt + Dperiod, &Dfactor);
		}

		// Set the working axis extents
		PEnset(m_hPE, PEP_nWORKINGAXIS, 3);
		PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
		d = 0.0;			PEvset(m_hPE, PEP_fMANUALMINY, &d, 1);
		d = 100.0;			PEvset(m_hPE, PEP_fMANUALMAXY, &d, 1);

		delete pYD3;
		delete pUpperArray;
		delete pLowerArray;
		delete pSlowOC;
		delete pKperiod;
		delete pHigh;
		delete pLow;
	}
	else
	{
		MessageBox(TEXT("Data File not found in program directory."));
		return;
	}

	// Set Points to number of records read, this requires PEP_bSUBSETBYPOINT be set to FALSE //
	PEnset(m_hPE, PEP_nPOINTS, nCnt);  // It's critical that points equals amount of XData passed
}

BOOL SmGigaSpreadChart::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();


	// 테마 초기화 - 차트 설정과 관계가 없다.
	InitTheme();
	// 스타일 초기화
	InitStyle();

	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	
	CRect rcWnd;
	GetDlgItem(IDC_STATIC_CHART)->GetWindowRect(rcWnd);
	ScreenToClient(&rcWnd);
	if (pLayout != NULL)
	{
		pLayout->AddAnchor(IDC_STATIC_CHART, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);

	}

	// 차트 색상 테마 업데이트
	UpdateChartColorTheme(m_nColorTheme, m_bIsDarkTheme);

	for (int i = 0; i < 60; i++)
		_ComboCycle.AddString(std::to_string(i + 1).c_str());
	_ComboCycle.SetCurSel(0);
	_ComboTick.AddString("1");
	_ComboTick.AddString("5");
	_ComboTick.AddString("10");
	_ComboTick.AddString("20");
	_ComboTick.AddString("30");
	_ComboTick.AddString("60");
	_ComboTick.AddString("90");
	_ComboTick.AddString("120");
	_ComboTick.AddString("240");
	_ComboTick.AddString("300");
	_ComboTick.AddString("600");
	_ComboTick.AddString("900");
	_ComboTick.SetCurSel(0);

	InitSymbolCombo();

	ChangeTimeButtonColor(_ChartTimeType);

	
	InitChart();

	SetTimer(1, 10, NULL);

	mainApp.CallbackMgr()->SubscribeChartCallback((long)this, std::bind(&SmGigaSpreadChart::OnChartEvent, this, _1, _2));
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmGigaSpreadChart::OnBnClickedBtnSet()
{
	
}


void SmGigaSpreadChart::OnDestroy()
{
	CBCGPDialog::OnDestroy();

	if (m_hPE) { PEdestroy(m_hPE); m_hPE = 0; }
}


void SmGigaSpreadChart::OnSize(UINT nType, int cx, int cy)
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


BOOL SmGigaSpreadChart::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CBCGPDialog::OnEraseBkgnd(pDC);
}


BOOL SmGigaSpreadChart::OnCommand(WPARAM wp, LPARAM lParam)
{
	int wmId = LOWORD(wp);
	int wmEvent = HIWORD(wp);
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

		else if ((HIWORD(wp) == PEWN_MOUSEMOVE))
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

		else if ((HIWORD(wp) == PEWN_CLICKED))
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

		else if ((HIWORD(wp) == PEWN_CUSTOMTRACKINGDATATEXT))  // v9 feature
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

		else if ( (HIWORD(wp) == PEWN_CURSORMOVE))
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

	return CBCGPDialog::OnCommand(wp, lParam);
}



void SmGigaSpreadChart::OnBnClickedBtnMon()
{
	_ChartTimeType = SmChartType::MON;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmGigaSpreadChart::OnBnClickedBtnWeek()
{
	_ChartTimeType = SmChartType::WEEK;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmGigaSpreadChart::OnBnClickedBtnDay()
{
	_ChartTimeType = SmChartType::DAY;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmGigaSpreadChart::OnBnClickedBtnMin()
{
	_ChartTimeType = SmChartType::MIN;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}


void SmGigaSpreadChart::OnBnClickedBtnTick()
{
	_ChartTimeType = SmChartType::TICK;
	ChangeTimeButtonColor(_ChartTimeType);
	RequestChartData();
}



void SmGigaSpreadChart::OnBnClickedBtnFind()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	//_SymbolTableDlg->SpreadChart = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}


void SmGigaSpreadChart::OnCbnSelchangeComboSymbol()
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


void SmGigaSpreadChart::OnCbnSelchangeComboCycle()
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


void SmGigaSpreadChart::OnCbnSelchangeComboTick()
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


void SmGigaSpreadChart::OnCbnSelchangeComboStyle()
{
	m_nLineType = _ComboStyle.GetCurSel();
	UpdateChart();
}

void SmGigaSpreadChart::OnClear()
{
	
}

void SmGigaSpreadChart::OnTimer(UINT_PTR nIDEvent)
{
	if (!_ChartData) return;

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
	
}

LRESULT SmGigaSpreadChart::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
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

void SmGigaSpreadChart::UpdateChartColorTheme(int nTheme, BOOL bIsDarkTheme)
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

void SmGigaSpreadChart::SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& window_id)
{
	if (!chart_data || _UniqueId != window_id) return;


	_ChartData = chart_data;

	// Function to load financial data from a comma delimited file //
	USES_CONVERSION;

	hmdf::StdDataFrame<hmdf::DateTime>& df = _ChartData->GetDataFrame();
	// 서브셋 갯수 설정
	PEnset(m_hPE, PEP_nSUBSETS, 11);
	// 전체 데이터 갯수 설정
	PEnset(m_hPE, PEP_nPOINTS, df.shape().first);  // setting to a large value, later we will reduce //
	// single or double precision X axis data is used
	PEnset(m_hPE, PEP_bUSINGXDATAII, TRUE);
	PEnset(m_hPE, PEP_bUSINGYDATAII, TRUE);

	double d;
	TCHAR szFile[48];
	double h, l, o, c, v;
	int nCnt = 0;
	int result;
	char szYear[3]; szYear[2] = 0;
	char szMonth[3]; szMonth[2] = 0;
	char szDay[3]; szDay[2] = 0;
	char szLabel[10];
	TM tm;

	tm.nHour = 12;
	tm.nMinute = 0;
	tm.nSecond = 0;
	tm.nMilliSecond = 0;

	//lstrcpy(szFile, pName);
	//lstrcat(szFile, TEXT(".txt"));

	// Empty Data Before resetting //
	// 데이터 초기화
	h = 0;
	PEvsetcell(m_hPE, PEP_faYDATAII, -1, &h);  // -1 special index causes array to resize to value defined by f2
	d = 0;
	PEvsetcell(m_hPE, PEP_faXDATAII, -1, &d);  // -1 special index causes array to resize to value defined by d
	strcpy(szLabel, "0");
	PEvsetcell(m_hPE, PEP_szaPOINTLABELS, -1, szLabel);  // -1 special index causes array to resize to value defined by szLabel

	/*
	// Open Data File //
	FILE* pFile = NULL;

	std::string path = "C:\\Project\\DarkHorse\\Debug\\MSFT.txt";

	_tcscpy(szFile, path.c_str());

	pFile = _tfopen(szFile, TEXT("r"));

	std::vector<double> temp_data(1500);

	if (pFile)
	{
		// Count records //
		while (!feof(pFile))
		{
			// Parse out different fields //
			result = fscanf(pFile, "%c%c%c%c%c%c,%f,%f,%f,%f,%f\n", &szYear[0], &szYear[1], &szMonth[0], &szMonth[1], &szDay[0], &szDay[1], &f1, &f2, &f3, &f4, &f5);

			if (result == 11)
			{
				// Create serial date from string data and pass into XDataII //
				tm.nYear = atoi(szYear);
				if (tm.nYear > 90)
					tm.nYear = tm.nYear + 1900;
				else if (tm.nYear < 10)
					tm.nYear = tm.nYear + 2000;
				tm.nYear = tm.nYear - 1900;
				tm.nMonth = atoi(szMonth);
				tm.nDay = atoi(szDay);
				PEcreateserialdate(&d, &tm, 1);
				PEvsetcell(m_hPE, PEP_faXDATAII, nCnt, &d);

				// Pass OHLC Volume Data //
				//PEvsetcellEx(m_hPE, PEP_faYDATA, 0, nCnt, &temp_data[nCnt]); //High
				PEvsetcellEx(m_hPE, PEP_faYDATA, 0, nCnt, &f2); //High
				PEvsetcellEx(m_hPE, PEP_faYDATA, 1, nCnt, &f3); //Low
				PEvsetcellEx(m_hPE, PEP_faYDATA, 2, nCnt, &f1); //Open
				PEvsetcellEx(m_hPE, PEP_faYDATA, 3, nCnt, &f4); //Close
				PEvsetcellEx(m_hPE, PEP_faYDATA, 7, nCnt, &f5); //Volume into 8th subset

				// Set PointLabel data, string representation of date //
				szLabel[0] = 0;
				strcat(szLabel, szMonth);
				strcat(szLabel, "/");
				strcat(szLabel, szDay);
				strcat(szLabel, "/");
				strcat(szLabel, szYear);

				const wchar_t* x = A2W((LPCSTR)szLabel);
				PEvsetcell(m_hPE, PEP_szaPOINTLABELS, nCnt, (LPVOID)x);

				// Store first date as StartTime property.  You must set StartTime property!
				if (nCnt == 0)
					PEvset(m_hPE, PEP_fSTARTTIME, &d, 1);

				nCnt++;
			}
		}
		fclose(pFile);
		*/
		// We now have data, let's make some studies //

	const auto& date_time = df.get_index();
	auto& high = df.get_column<double>("high");
	auto& low = df.get_column<double>("low");
	auto& open = df.get_column<double>("open");
	auto& close = df.get_column<double>("close");
	auto& volume = df.get_column<double>("volume");

	for (size_t i = 0; i < df.shape().first; i++) {
		tm.nYear = date_time[i].year() - 1900;
	
		tm.nMonth = static_cast<int>(date_time[i].month());
		tm.nDay = date_time[i].dmonth();
		tm.nHour = date_time[i].hour();
		tm.nMinute = date_time[i].minute();
		tm.nSecond = date_time[i].sec();
		tm.nMilliSecond = date_time[i].msec();
		PEcreateserialdate(&d, &tm, 1);
		PEvsetcell(m_hPE, PEP_faXDATAII, nCnt, &d);

		h = high[i]; l = low[i]; o = open[i]; c = close[i]; v = volume[i];
		// Pass OHLC Volume Data //
		//PEvsetcellEx(m_hPE, PEP_faYDATA, 0, nCnt, &temp_data[nCnt]); //High
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 0, nCnt, &h); //High
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 1, nCnt, &l); //Low
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 2, nCnt, &o); //Open
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 3, nCnt, &c); //Close
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 7, nCnt, &v); //Volume into 8th subset

		// Set PointLabel data, string representation of date //
		// 09/16/1999 13:51:04.256
		std::string dt_fmt = date_time[i].string_format(hmdf::DT_FORMAT::DT_TM2);

		szLabel[0] = 0;
		strcpy_s(szMonth, sizeof(szMonth), dt_fmt.substr(0, 2).c_str());
		strcat(szLabel, szMonth);
		strcat(szLabel, "/");
		strcpy_s(szDay, sizeof(szDay), dt_fmt.substr(3, 2).c_str());
		strcat(szLabel, szDay);
		strcat(szLabel, "/");
		strcpy_s(szYear, sizeof(szYear), dt_fmt.substr(8, 2).c_str());
		strcat(szLabel, szYear);

		const wchar_t* x = A2W((LPCSTR)szLabel);
		PEvsetcell(m_hPE, PEP_szaPOINTLABELS, nCnt, (LPVOID)x);

		// Store first date as StartTime property.  You must set StartTime property!
		if (tm.nHour == 0)
			PEvset(m_hPE, PEP_fSTARTTIME, &d, 1);

		nCnt++;
	}

	int ID = 0;
	double f;
	double Days;
	int pnt, i;
	double Total = 0;
	double Total2 = 0;
	DWORD dw;
	int X = 1;

	double SMAfBB, BBNum;

	double* pYD3;
	pYD3 = new double[nCnt];
	if (!pYD3)
		return;
	// 값을 가져온다.
	for (pnt = 0; pnt < nCnt; pnt++)
		PEvgetcellEx(m_hPE, PEP_faYDATAII, 3, pnt, &pYD3[pnt]);

	// Middle Band = SMA
	Days = 20;
	for (pnt = 0; pnt < (nCnt - Days); pnt++)
	{
		Total = 0;
		for (i = 0 + pnt; i < Days + pnt; i++)
			Total = Total + pYD3[i];
		f = Total / Days;
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 5, pnt + Days - 1, &f);
	}

	// Upper Band = Middle Band + [Standard Deviation * Square Root{[Sum for x Days((Closing Price - Middle Band)^2)]/Days]
	for (pnt = 0; pnt < (nCnt - Days); pnt++)
	{
		Total = 0;
		for (i = 0 + pnt; i < Days + pnt; i++)
			Total = Total + pYD3[i];
		SMAfBB = Total / Days;
		// Find sum of Closing Price minus SMA over days and square
		BBNum = 0;
		for (i = 0 + pnt; i < Days + pnt; i++)
			BBNum = ((pYD3[i] - SMAfBB) * (pYD3[i] - SMAfBB)) + BBNum;

		f = SMAfBB + 2.0F * sqrt(BBNum / Days);
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 4, pnt + Days - 1, &f);  // upper
	}

	// Lower Band = Middle Band - [Standard Deviation * Square Root{[Sum for x Days((Closing Price - Middle Band)^2)]/Days]
	for (pnt = 0; pnt < (nCnt - Days); pnt++)
	{
		Total = 0;
		for (i = 0 + pnt; i < Days + pnt; i++)
			Total = Total + pYD3[i];
		SMAfBB = Total / Days;
		// Find sum of Closing Price minus SMA over days and square
		BBNum = 0;
		for (i = 0 + pnt; i < Days + pnt; i++)
			BBNum = ((pYD3[i] - SMAfBB) * (pYD3[i] - SMAfBB)) + BBNum;

		f = SMAfBB - 2.0F * sqrt(BBNum / Days);
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 6, pnt + Days - 1, &f);  // upper
	}

	Total = 0;
	Total2 = 0;
	double RS = 0;
	double RSI = 0;
	X = 1;
	int LowerCount = 0;
	int UpperCount = 0;

	// How many days do you want to check
	Days = 10;

	double* pUpperArray;
	double* pLowerArray;
	pUpperArray = new double[nCnt];
	pLowerArray = new double[nCnt];

	for (i = 1; i <= Days; i++)
	{
		if ((pYD3[i] - pYD3[i - 1]) < 0)
		{
			pLowerArray[LowerCount] = (pYD3[i] - pYD3[i - 1]);
			LowerCount = LowerCount + 1;
		}
		else
		{
			pUpperArray[UpperCount] = (pYD3[i] - pYD3[i - 1]);
			UpperCount = UpperCount + 1;
		}
	}

	// Total the amounts over the period
	Total = 0;
	for (i = 0; i < LowerCount; i++)
		Total = pLowerArray[i] + Total;

	Total2 = 0;
	for (i = 0; i < UpperCount; i++)
		Total2 = pUpperArray[i] + Total2;

	// Divide up by down
	RS = (Total2 / Days) / (fabs(Total) / Days);

	// Use the RSI formula
	RSI = 100.0 - (100.0 / (1.0 + RS));
	PEvsetcellEx(m_hPE, PEP_faYDATAII, 8, Days - 1, &RSI);

	for (i = Days; i < nCnt; i++)
	{
		if (pYD3[i] != 0 && pYD3[i - 1] != 0)
		{
			Total = Total * (Days - 1);
			Total2 = Total2 * (Days - 1);

			if ((pYD3[i] - pYD3[i - 1]) < 0)
				Total = (pYD3[i] - pYD3[i - 1]) + Total;
			else
				Total2 = (pYD3[i] - pYD3[i - 1]) + Total2;

			Total = Total / Days;
			Total2 = Total2 / Days;

			RS = (Total2) / (fabs(Total));
			RSI = 100.0 - (100.0 / (1.0 + RS));
			PEvsetcellEx(m_hPE, PEP_faYDATAII, 8, Days - 1 + i, &RSI);
		}
	}

	// Add a mid axis line annotation
	d = 50.0;			PEvsetcell(m_hPE, PEP_faHORZLINEANNOTATION, 0, &d);
	i = 2;				PEvsetcell(m_hPE, PEP_naHORZLINEANNOTATIONAXIS, 0, &i);
	i = 5;				PEvsetcell(m_hPE, PEP_naHORZLINEANNOTATIONTYPE, 0, &i);
	dw = PERGB(255, 198, 0, 0);	PEvsetcell(m_hPE, PEP_dwaHORZLINEANNOTATIONCOLOR, 0, &dw);
	PEnset(m_hPE, PEP_bSHOWANNOTATIONS, TRUE);

	// Set the working axis extents
	PEnset(m_hPE, PEP_nWORKINGAXIS, 2);
	PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
	d = 0.0;			PEvset(m_hPE, PEP_fMANUALMINY, &d, 1);
	d = 100.0;			PEvset(m_hPE, PEP_fMANUALMAXY, &d, 1);

	/////////////////////////////////////////////////////////////////////
	// Stochastic Oscillator //
	///////////////////////////

	int SwFac, Dperiod, q;
	double LowMin, HighMax, SlowK, Dfactor, PercentD;
	LowMin = 9999.9F;
	HighMax = 0.0F;

	double* pSlowOC;
	double* pKperiod;
	double* pHigh;
	double* pLow;

	pSlowOC = new double[nCnt];
	pKperiod = new double[nCnt];
	pHigh = new double[nCnt];
	pLow = new double[nCnt];

	for (pnt = 0; pnt < nCnt; pnt++)
	{
		PEvgetcellEx(m_hPE, PEP_faYDATAII, 0, pnt, &pHigh[pnt]);
		PEvgetcellEx(m_hPE, PEP_faYDATAII, 1, pnt, &pLow[pnt]);
	}

	// How many days do you want to check
	Days = 30;
	SwFac = 1;
	Dperiod = 15;

	for (pnt = 0; pnt < (nCnt - Days); pnt++)
	{
		for (i = pnt; i < Days + pnt; i++)
		{
			if (pHigh[i] != 0)
			{
				if (pHigh[i] > HighMax)
					HighMax = pHigh[i];
			}
		}
		for (i = pnt; i < Days + pnt; i++)
		{
			if (pLow[i] != 0)
			{
				if (pLow[i] < LowMin)
					LowMin = pLow[i];
			}
		}

		if (pYD3[(int)(Days - 1 + pnt)] != 0)
			pKperiod[pnt] = ((pYD3[(int)(Days - 1 + pnt)] - LowMin) / (HighMax - LowMin)) * 100.0F;

		LowMin = 9999.9F;
		HighMax = 0.0F;
	}

	for (pnt = 0; pnt < nCnt - SwFac; pnt++)
	{
		SlowK = 0;
		for (q = pnt; q < SwFac + pnt; q++)
			SlowK = pKperiod[q] + SlowK;

		pSlowOC[pnt] = SlowK / SwFac;
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 9, Days + pnt + SwFac, &pSlowOC[pnt]);
	}

	for (pnt = 0; pnt < nCnt - Dperiod; pnt++)
	{
		PercentD = 0;
		for (q = pnt; q < Dperiod + pnt; q++)
			PercentD = pSlowOC[q] + PercentD;
		Dfactor = PercentD / Dperiod;
		PEvsetcellEx(m_hPE, PEP_faYDATAII, 10, Days + pnt + Dperiod, &Dfactor);
	}

	// Set the working axis extents
	PEnset(m_hPE, PEP_nWORKINGAXIS, 3);
	PEnset(m_hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
	d = 0.0;			PEvset(m_hPE, PEP_fMANUALMINY, &d, 1);
	d = 100.0;			PEvset(m_hPE, PEP_fMANUALMAXY, &d, 1);

	delete pYD3;
	delete pUpperArray;
	delete pLowerArray;
	delete pSlowOC;
	delete pKperiod;
	delete pHigh;
	delete pLow;
	

	// Set Points to number of records read, this requires PEP_bSUBSETBYPOINT be set to FALSE //
	PEnset(m_hPE, PEP_nPOINTS, nCnt);  // It's critical that points equals amount of XData passed

}

void SmGigaSpreadChart::SetChartData()
{
	if (_CurIndex < 0) return;
	auto found = _RowToSymbolMap.find(_CurIndex);
	if (found == _RowToSymbolMap.end()) return;

	std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindChartData(found->second->SymbolCode(), _ChartTimeType, _Cycle);

	if (chart_data) {
		SetChartData(chart_data, _UniqueId);
	}
}

void SmGigaSpreadChart::InitChart()
{
	CRect rcWnd;
	GetDlgItem(IDC_STATIC_CHART)->GetWindowRect(rcWnd);
	ScreenToClient(&rcWnd);
	m_hPE = PEcreate(PECONTROL_GRAPH, WS_VISIBLE, &rcWnd, m_hWnd, 1001);

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

	// Construct a simple table annotation //
	PEnset(m_hPE, PEP_nWORKINGTABLE, 0);
	PEnset(m_hPE, PEP_nTAROWS, 17);
	PEnset(m_hPE, PEP_nTACOLUMNS, 1);

	// Pass the table text //
	DWORD dwColor;
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 0, (LPSTR)(" MSFT"));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 1, 0, (LPSTR)(" SUNW"));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 2, 0, (LPSTR)(" ORCL"));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 3, 0, (LPSTR)(" IBM"));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 4, 0, (LPSTR)(" INTC"));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 5, 0, (LPSTR)(" NSM"));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 6, 0, (LPSTR)(" DELL"));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 7, 0, (LPSTR)(" WMT"));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 8, 0, (LPSTR)(" CVX"));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 9, 0, (LPSTR)(" PBG"));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 10, 0, (LPSTR)("-----------------"));

	int nHS = 1;
	PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, 0, 0, &nHS);
	PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, 1, 0, &nHS);
	PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, 2, 0, &nHS);
	PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, 3, 0, &nHS);
	PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, 4, 0, &nHS);
	PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, 5, 0, &nHS);
	PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, 6, 0, &nHS);
	PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, 7, 0, &nHS);
	PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, 8, 0, &nHS);
	PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, 9, 0, &nHS);

	dwColor = PERGB(255, 198, 0, 0);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 0, 0, &dwColor);
	dwColor = PERGB(255, 142, 142, 142);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 1, 0, &dwColor);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 2, 0, &dwColor);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 3, 0, &dwColor);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 4, 0, &dwColor);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 5, 0, &dwColor);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 6, 0, &dwColor);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 7, 0, &dwColor);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 8, 0, &dwColor);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 9, 0, &dwColor);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 10, 0, &dwColor);
	dwColor = PERGB(255, 150, 150, 150);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 11, 0, &dwColor);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 12, 0, &dwColor);
	dwColor = PERGB(255, 142, 142, 142);
	PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 13, 0, &dwColor);
	dwColor = PERGB(255, 0, 170, 0);  PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 14, 0, &dwColor);
	dwColor = PERGB(255, 198, 0, 0);  PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 15, 0, &dwColor);
	dwColor = PERGB(255, 160, 160, 160);  PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 16, 0, &dwColor);

	int nTACW = 8;
	PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 0, &nTACW);

	// Set Table Location //
	PEnset(m_hPE, PEP_nTALOCATION, PETAL_LEFT_CENTER);

	// Other Table Related Properties ///
	PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, TRUE);
	PEnset(m_hPE, PEP_nTABORDER, PETAB_THIN_LINE);
	PEnset(m_hPE, PEP_dwTABACKCOLOR, PERGB(255, 0, 0, 0));
	PEnset(m_hPE, PEP_dwTABLEFORECOLOR, PERGB(255, 190, 190, 190));
	PEnset(m_hPE, PEP_nTATEXTSIZE, 100);

	// Done setting Table Annotations //

	// Load Data and Add Studies, Total of 11 subsets when finished //
	// SubsetByPoint lets us add/remove points  //
	PEnset(m_hPE, PEP_bSUBSETBYPOINT, FALSE);
	//LoadData((LPSTR)("MSFT"));


	SetChartData();


	//! Set properties related to date/time mode //
	PEnset(m_hPE, PEP_nDELTASPERDAY, 1);		// 1 data point per day
	PEnset(m_hPE, PEP_nDELTAX, -1);				// -1 special code for daily data //
	PEnset(m_hPE, PEP_nDATETIMEMODE, PEDTM_VB); // How to interpret serial dates //
	PEnset(m_hPE, PEP_nYEARMONTHDAYPROMPT, PEDP_INSIDE_TOP);
	PEnset(m_hPE, PEP_nDAYLABELTYPE, PEDLT_1_CHAR);
	PEnset(m_hPE, PEP_nMONTHLABELTYPE, PEDLT_3_CHAR);

	// 각각의 축에 대하여 서브셋 갯수를 분할해서 각각 설정해준다.
	// Split up subsets among different axes //
	int nMAS[] = { 7, 1, 1, 2 };
	PEvset(m_hPE, PEP_naMULTIAXESSUBSETS, nMAS, 4);
	// 각각의 축이 차지하는 비율을 정해준다. 모두 합해서 100%가 되어야 한다.
	float fMAP[] = { .55F, .15F, .15F, .15F };
	PEvset(m_hPE, PEP_faMULTIAXESPROPORTIONS, fMAP, 4);

	PEnset(m_hPE, PEP_nWORKINGAXIS, 0);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	// PEP_nCOMPARISONSUBSETS - 같은 축에 비교할 때
	// PEP_nRYAXISCOMPARISONSUBSETS - 다른 축에 비교할 때 
	PEnset(m_hPE, PEP_nRYAXISCOMPARISONSUBSETS, 3);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	// Set Various Other Properties ///
	PEnset(m_hPE, PEP_nDATAPRECISION, 2);
	// 여러개의 축이 사용될때 분리시킬 것인지 같이 사용할 것인지 결정
	PEnset(m_hPE, PEP_nMULTIAXISSTYLE, PEMAS_GROUP_ALL_AXES);
	PEnset(m_hPE, PEP_nMULTIAXESSIZING, TRUE);
	PEnset(m_hPE, PEP_bYAXISONRIGHT, TRUE);
	PEnset(m_hPE, PEP_bSPECIFICPLOTMODECOLOR, TRUE); // Enables mult-colored candle stick fills
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

	// Set up per axis properties //
	PEnset(m_hPE, PEP_nWORKINGAXIS, 0);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_SPECIFICPLOTMODE);
	PEnset(m_hPE, PEP_nSPECIFICPLOTMODE, PESPM_OPENHIGHLOWCLOSE);
	PEnset(m_hPE, PEP_nCOMPARISONSUBSETS, 3);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	PEnset(m_hPE, PEP_nWORKINGAXIS, 1);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	PEnset(m_hPE, PEP_nWORKINGAXIS, 2);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	PEnset(m_hPE, PEP_nWORKINGAXIS, 3);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_AREA);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	// Set Subset Colors //
	DWORD dwArray[11] = { PERGB(255,160,160,160), PERGB(255,160, 160, 160),	PERGB(255,128,0,0), PERGB(255,0,128,0),
							 PERGB(80,235, 0, 0), PERGB(80,235, 235, 0), PERGB(80,0,235, 235),	PERGB(255,170, 170, 170),
							 PERGB(255,0, 140, 140),PERGB(180,0, 195, 0), PERGB(120,165,165,165) };
	PEvset(m_hPE, PEP_dwaSUBSETCOLORS, dwArray, 11);

	int nSLT = PELT_THINSOLID;
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 0, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 1, &nSLT);
	nSLT = PELT_MEDIUMTHINSOLID;
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 2, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 3, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 4, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 5, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 6, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 7, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 8, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 9, &nSLT);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, 10, &nSLT);

	// Set Subset Labels //
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 0, (LPSTR)("High"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 1, (LPSTR)("Low"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 2, (LPSTR)("Open"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 3, (LPSTR)("Close"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 4, (LPSTR)("Bollinger Upper"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 5, (LPSTR)("SMA 20"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 6, (LPSTR)("Bollinger Lower"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 7, (LPSTR)("Volume"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 8, (LPSTR)("Relative Strength Index - 10"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 9, (LPSTR)("Fast %K"));
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 10, (LPSTR)("Slow %D"));

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

void SmGigaSpreadChart::SetChartData2(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& window_id)
{
	if (!chart_data || _UniqueId != window_id) return;



	CBCGPChartVisualObject* pChart = GetChart();
	ASSERT_VALID(pChart);

	CBCGPChartStockSeries* pStockSeries = DYNAMIC_DOWNCAST(CBCGPChartStockSeries, pChart->GetSeries(1));
	pStockSeries->RemoveAllDataPoints();

	CCustomStockSeries* pCustomSeries = (CCustomStockSeries*)pChart->GetSeries(1);
	pCustomSeries->SetExternalStorage(&chart_data->ExternalStorage);
	pCustomSeries->m_strSeriesName = chart_data->SymbolCode().c_str();



	auto symbol = mainApp.SymMgr()->FindSymbol(chart_data->SymbolCode());
	if (!symbol) return;

	CBCGPChartAxis* pAxisY = pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);
	CString yAxisFormat;
	yAxisFormat.Format("%%.%df", symbol->Decimal());
	pAxisY->m_strDataFormat = yAxisFormat;

	pChart->RecalcMinMaxValues();
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

void SmGigaSpreadChart::OnChartEvent(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action)
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

void SmGigaSpreadChart::InitStyle()
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

void SmGigaSpreadChart::InitTheme()
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

void SmGigaSpreadChart::InitSymbolCombo()
{
	// 반드시 실시간 등록을 해줄것
	const std::map<int, std::shared_ptr<DarkHorse::SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		int index = _ComboSymbol.AddString(it->second->SymbolCode().c_str());
		_RowToSymbolMap[index] = it->second;
	}

	if (_RowToSymbolMap.size() > 0) {
		_CurIndex = 0;
		_ComboSymbol.SetCurSel(_CurIndex);
	}
}

void SmGigaSpreadChart::SetBarLineWidth()
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

void SmGigaSpreadChart::AddChartData()
{
	if (!_ChartData || !_ChartData->Received()) return;

	auto symbol = mainApp.SymMgr()->FindSymbol(_ChartData->SymbolCode());
	if (!symbol) return;

	SmStockData data;
	_ChartData->GetLastData(data);

	std::vector<int> ymd = SmUtil::IntToDate(data.date_time.date());
	COleDateTime ole_date_time;
	ole_date_time.SetDateTime(ymd[0], ymd[1], ymd[2], data.date_time.hour(), data.date_time.minute(), data.date_time.sec());
	double divedend = pow(10, symbol->Decimal());
	CBCGPChartStockData stockData;
	stockData.m_dateTime = ole_date_time;
	stockData.m_dblHigh = data.high / divedend;
	stockData.m_dblLow = data.low / divedend;
	stockData.m_dblOpen = data.open / divedend;
	stockData.m_dblClose = data.close / divedend;

	//_MainStorage.ShiftData(stockData);
	//_MainStorage.AddData(stockData.m_dblOpen, stockData.m_dblHigh, stockData.m_dblLow, stockData.m_dblClose, stockData.m_dateTime);
}

void SmGigaSpreadChart::UpdateChartData()
{
	if (!_ChartData || !_ChartData->Received()) return;

	auto symbol = mainApp.SymMgr()->FindSymbol(_ChartData->SymbolCode());
	if (!symbol) return;

	SmStockData data;
	_ChartData->GetLastData(data);
	double divedend = pow(10, symbol->Decimal());
	//_MainStorage.UpdateData(data.open / divedend, data.high / divedend, data.low / divedend, data.close / divedend);
}

void SmGigaSpreadChart::CreateChart()
{

}

void SmGigaSpreadChart::UpdateChart()
{
	
}

CBCGPChartObject* SmGigaSpreadChart::SetupIndicatorObjects(double dblValue, CBCGPChartAxis* pIndicatorAxis, const CString& strIndicatorName /*= _T("")*/)
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

void SmGigaSpreadChart::ChangeTimeButtonColor(const DarkHorse::SmChartType& chart_type)
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

void SmGigaSpreadChart::RequestChartData()
{
	if (_CurIndex < 0) return;
	auto found = _RowToSymbolMap.find(_CurIndex);
	if (found == _RowToSymbolMap.end()) return;

	std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindChartData(found->second->SymbolCode(), _ChartTimeType, _Cycle);

	if (chart_data) {
		SetChartData(chart_data, _UniqueId);
	}
	else {

		mainApp.ChartDataMgr()->RequestChartData(found->second->SymbolCode(), _ChartTimeType, _Cycle, _UniqueId, 0);

		mainApp.SymMgr()->RegisterSymbolToServer(found->second->SymbolCode(), true);
	}
}

void SmGigaSpreadChart::RequestChartData(const std::string symbol_code)
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


void SmGigaSpreadChart::OnCbnSelchangeComboTheme()
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

