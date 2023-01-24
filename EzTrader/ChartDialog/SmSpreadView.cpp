// SmSpreadView.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmSpreadView.h"
#include "Pegrpapi.h"
#include <tchar.h>
#include <string>
#include <vector>
// SmSpreadView

#define GetRandom( min, max ) ((rand() % (int)(((max)+1) - (min))) + (min))

IMPLEMENT_DYNCREATE(SmSpreadView, CView)

SmSpreadView::SmSpreadView()
{
	m_hPE = NULL;
}

SmSpreadView::~SmSpreadView()
{
}

BEGIN_MESSAGE_MAP(SmSpreadView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


// SmSpreadView drawing

void SmSpreadView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// SmSpreadView diagnostics

#ifdef _DEBUG
void SmSpreadView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void SmSpreadView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// SmSpreadView message handlers


int SmSpreadView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	/*
	RECT r;
	int s, p;
	int dwColor;
	float f[] = { 10, 30, 20, 40, 30, 50, 15, 63, 74, 54, 25, 34 };

	GetClientRect(&r);
	hPE = PEcreate(PECONTROL_GRAPH, 0, &r, this->m_hWnd, 1000);
	PEszset(hPE, PEP_szMAINTITLE, (LPSTR)("Hello World"));
	PEszset(hPE, PEP_szSUBTITLE, (LPSTR)(""));
	PEnset(hPE, PEP_nSUBSETS, 2);
	PEnset(hPE, PEP_nPOINTS, 6);
	for (s = 0; s < 2; s++)
	{
		for (p = 0; p < 6; p++)
		{ // (s*6)+p or (SubsetIndex * NumberPoints) + PointIndex
			PEvsetcellEx(hPE, PEP_faYDATA, s, p, &f[(s * 6) + p]);
		}
	}
	// or Pass data in one call is much faster -> PEvset(hPE, PEP_faYDATA, f, 12);
	PEvsetcell(hPE, PEP_szaPOINTLABELS, 0, (LPSTR)("Jan"));
	PEvsetcell(hPE, PEP_szaPOINTLABELS, 1, (LPSTR)("Feb"));
	PEvsetcell(hPE, PEP_szaPOINTLABELS, 2, (LPSTR)("Mar"));
	PEvsetcell(hPE, PEP_szaPOINTLABELS, 3, (LPSTR)("Apr"));
	PEvsetcell(hPE, PEP_szaPOINTLABELS, 4, (LPSTR)("May"));
	PEvsetcell(hPE, PEP_szaPOINTLABELS, 5, (LPSTR)("June"));
	PEvsetcell(hPE, PEP_szaSUBSETLABELS, 0, (LPSTR)("For .Net Framework"));
	PEvsetcell(hPE, PEP_szaSUBSETLABELS, 1, (LPSTR)("or MFC, ActiveX, VCL"));
	PEszset(hPE, PEP_szYAXISLABEL, (LPSTR)("Simple Quality Rendering"));
	PEszset(hPE, PEP_szXAXISLABEL, (LPSTR)(""));
	dwColor = PERGB(60, 0, 180, 0); PEvsetcell(hPE, PEP_dwaSUBSETCOLORS, 0, &dwColor);
	dwColor = PERGB(180, 0, 0, 130); PEvsetcell(hPE, PEP_dwaSUBSETCOLORS, 1, &dwColor);
	PEnset(hPE, PEP_nGRAPHPLUSTABLE, PEGPT_BOTH);
	PEnset(hPE, PEP_nDATAPRECISION, 0);
	PEnset(hPE, PEP_bLABELBOLD, TRUE);
	PEnset(hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
	PEnset(hPE, PEP_nGRADIENTBARS, 8);
	PEnset(hPE, PEP_bBARGLASSEFFECT, TRUE);
	PEnset(hPE, PEP_nLEGENDLOCATION, PELL_LEFT);
	PEnset(hPE, PEP_nDATASHADOWS, PEDS_3D);
	PEnset(hPE, PEP_nFONTSIZE, PEFS_LARGE);
	PEnset(hPE, PEP_bPREPAREIMAGES, TRUE);
	PEnset(hPE, PEP_bCACHEBMP, TRUE);
	PEnset(hPE, PEP_nRENDERENGINE, PERE_DIRECT2D);
	PEnset(hPE, PEP_bANTIALIASGRAPHICS, TRUE);
	PEnset(hPE, PEP_bANTIALIASTEXT, TRUE);
	PEnset(hPE, PEP_bALLOWDATAHOTSPOTS, TRUE);
	PEnset(hPE, PEP_bBITMAPGRADIENTMODE, FALSE);
	PEnset(hPE, PEP_nQUICKSTYLE, PEQS_LIGHT_SHADOW);
	PEnset(hPE, PEP_bFIXEDFONTS, TRUE);
	PEreinitialize(hPE);
	PEresetimage(hPE, 0, 0);

	return 0;
	*/


	//! Please read the RED text within this code window and others. 
	//!
	//! Some things to try...
	//! 1) Right click the chart and use the popup menu to adjust viewing style and other items.
	//! 2) Left click and drag to zoom, use popup menu to undo zoom. Note how time scale changes.
	//! 3) Left click the Portfolio table to select different stocks.
	//! 4) Change Point Label Orientation via the popup menu and note how x axis scale changes.
	//! 5) The Data Cursor can be moved with arrow keys.  Click a data point and then use arrow keys.
	//! 6) Zoom chart and watch the Volume bar plotting method structure for different time ranges.
	//!    There's new drawing logic that first recongnizes all bars are connected, then as range is shrunk,
	//!    bar's get drawn 1 pixel apart, and finally after zooming more, bars are spaced.

	// Note, the entire code to produce this example is shown below.
	// The immediate secion controls chart construction, latter sections show.
	// how to respond to mouse-move and table annotation hot spot events.
	// The final section shows how the data is loaded from a comma delimited.
	// file and studies calculated.

	// This example shows a collection of features.
	// Table Annotation Hot Spots can turn table annotations into user interface objects.
	// Also shown is real-time table annotation updating and date/time handling.

	RECT rect;
	GetClientRect(&rect);
	m_hPE = PEcreate(PECONTROL_GRAPH, WS_VISIBLE, &rect, m_hWnd, 1001);

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

	//CMDIFrameWnd* pWnd = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
	//pWnd->SendMessage(WM_CHANGE_DEMO_RENDERENGINE, RENDER_2DX);
	return 0;

}


void SmSpreadView::OnDestroy()
{
	CView::OnDestroy();

	if (m_hPE) { PEdestroy(m_hPE); m_hPE = 0; }
}


BOOL SmSpreadView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CView::OnEraseBkgnd(pDC);
}


void SmSpreadView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_hPE)
	{
		RECT r; GetClientRect(&r);
		::MoveWindow(m_hPE, 0, 0, r.right, r.bottom, FALSE);
	}

}


BOOL SmSpreadView::OnCommand(WPARAM wp, LPARAM lParam)
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

		else if ((m_nLastSelection == 30) && (HIWORD(wp) == PEWN_MOUSEMOVE))
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

		else if ((m_nLastSelection == 30) && (HIWORD(wp) == PEWN_CLICKED))
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

		else if ((m_nLastSelection == 30) && (HIWORD(wp) == PEWN_CUSTOMTRACKINGDATATEXT))  // v9 feature
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

		else if ((m_nLastSelection == 30) && (HIWORD(wp) == PEWN_CURSORMOVE))
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


	return CView::OnCommand(wp, lParam);
}


void SmSpreadView::LoadData(TCHAR* pName)
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

int SmSpreadView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	return MA_ACTIVATE;
	//return CView::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
