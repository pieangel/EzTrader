// VtEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "VtEditCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// VtEditCtrl

IMPLEMENT_DYNAMIC(VtEditCtrl, CEdit)

VtEditCtrl::VtEditCtrl()
{

}

VtEditCtrl::~VtEditCtrl()
{
}


BEGIN_MESSAGE_MAP(VtEditCtrl, CEdit)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// VtEditCtrl message handlers




void VtEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == VK_RETURN)
	{
		AfxMessageBox(_T("enter"));
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
