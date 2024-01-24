// DarkHorse.h : main header file for the DarkHorse application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
BOOL __stdcall SetChildFont(HWND hwnd, LPARAM lparam);

class TApplicationFont : public CObject {
private:
	CString m_strFaceName;
	int m_nFontSize;
	int m_nPointSize;
	CFont m_Font;

private:
	void createFont(void);

public:
	TApplicationFont(LPCTSTR szFaceName);
	virtual ~TApplicationFont();

	CFont* GetFont(void);
	CString& GetFaceName(void);
	int GetFontSize(void);
	int GetPointSize(void);

	void SetFaceName(LPCTSTR szFaceName);
};



// CDarkHorseApp:
// See DarkHorse.cpp for the implementation of this class
//

class CDarkHorseApp : public CBCGPWinApp
{
public:
	CDarkHorseApp();

	// Override from CBCGPWorkspace
	virtual void PreLoadState();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	virtual BOOL SaveAllModified();


// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	void InitLog();
	int RegisterOCX();
	void CreateCrashHandler();
	bool CheckExpire();
};

extern CDarkHorseApp theApp;
