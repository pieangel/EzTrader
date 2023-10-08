// YaClient.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "afxdialogex.h"
#include "YaClient.h"
#include "MessageDef.h"
#include "magic_enum/magic_enum.hpp"

class CMainFrame;
using namespace DarkHorse;
using namespace nlohmann;
// YaClient dialog

IMPLEMENT_DYNAMIC(YaClient, CDialog)

// YaClient::YaClient(CWnd* pParent /*=nullptr*/)
// 	: CDialog(IDD_YA_CLIENT, pParent)
// {
// 
// }

YaClient::YaClient(CWnd* pParent, YaStockClient& ya_stock_client)
	: CDialog(IDD_YA_CLIENT, pParent), ya_stock_client_(ya_stock_client)
{

}

YaClient::~YaClient()
{
}

void YaClient::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(YaClient, CDialog)
	ON_MESSAGE(WMU_RECEIVE_ERROR, OnReceiveError)
	ON_MESSAGE(WMU_RECEIVE_DATA, OnReceiveData)
	ON_MESSAGE(WMU_RECEIVE_REAL_DATA, OnReceiveRealData)
	ON_MESSAGE(WMU_RECEIVE_SYSTEM_MESSAGE, OnReceiveSystemMessage)
	ON_MESSAGE(WMU_LOGIN, OnLoginComplete)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// YaClient message handlers
LRESULT YaClient::OnReceiveError(WPARAM wParam, LPARAM lParam)
{
	int nReqID = wParam;

	CString strMsg;
	CString strTrName;

// 	if (nReqID == m_nReqID_300001)
// 	{
// 		strTrName = _T("[300001]�ֽ����簡");
// 	}
// 
// 	switch (lParam)
// 	{
// 	case ERROR_TIMEOUT_DATA:			// ������ �ð� �ȿ� �����κ��� ������ ���� ���, Ÿ�Ӿƿ��� �߻��մϴ�. (�⺻ 10��)
// 	{
// 		strMsg.Format(_T("Timeout %s ��û�� ������ �����ϴ�."), strTrName);
// 		LogMessage(strMsg);
// 	}
// 	break;
// 	case ERROR_REQUEST_FAIL:			// �������� ��ȸTR(DSO) ó���� ������ �ִ� ��� �߻��մϴ�.
// 	{
// 		TCHAR msg[2048] = { 0, };
// 		g_iYuantaAPI.YOA_GetErrorMessage(nReqID, msg, sizeof(msg));	// ó�� �� ������ ���� �޽����� ���� �� �ֽ��ϴ�.
// 		strMsg.Format(_T("%s\n%s"), strTrName, msg);
// 		strMsg.TrimRight();
// 		LogMessage(strMsg);
// 	}
// 	break;
// 	}

	return 0;
}

LRESULT YaClient::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	//CString strTRID( (LPCTSTR)(LPCSTR)lParam );
	LPRECV_DATA pRecvData = (LPRECV_DATA)lParam;

	CString strTrName = _T("");

// 	if (wParam == m_nReqID_300001)
// 	{
// 		strTrName = _T("[300001]�ֽ����簡");
// 
// 		TCHAR data[1024] = { 0, };
// 
// 		memset(data, 0x00, sizeof(data));
// 		g_iYuantaAPI.YOA_GetTRFieldString(_T("300001"), _T("OutBlock1"), _T("jongname"), data, sizeof(data));
// 		GetDlgItem(IDC_BT_JONGNAME)->SetWindowText(data);
// 
// 		g_iYuantaAPI.YOA_GetTRFieldString(_T("300001"), _T("OutBlock1"), _T("curjuka"), data, sizeof(data));
// 		GetDlgItem(IDC_BT_CURJUKA)->SetWindowText(data);
// 
// 		g_iYuantaAPI.YOA_SetTRInfo(_T("300001"), _T("OutBlock1"));
// 		g_iYuantaAPI.YOA_GetFieldString(_T("debi"), data, sizeof(data));
// 		GetDlgItem(IDC_BT_DEBI)->SetWindowText(data);
// 
// 		g_iYuantaAPI.YOA_GetFieldString(_T("debirate"), data, sizeof(data));
// 		GetDlgItem(IDC_BT_DEBIRATE)->SetWindowText(data);
// 
// 		g_iYuantaAPI.YOA_GetFieldString(_T("volume"), data, sizeof(data));
// 		GetDlgItem(IDC_BT_VOLUME)->SetWindowText(data);
// 
// 		LogMessage(_T("[300001]�ֽ����簡 ������ �����Ͽ����ϴ�."));
// 	}
// 	else if (wParam == m_nReqID_160001)
// 	{
// 		TCHAR data[1024] = { 0, };
// 
// 		memset(data, 0x00, sizeof(data));
// 		g_iYuantaAPI.YOA_GetTRFieldString(_T("160001"), _T("OutBlock1"), _T("org_no"), data, sizeof(data));
// 	}

	return 0;
}

LRESULT YaClient::OnReceiveRealData(WPARAM wParam, LPARAM lParam)
{
	LPRECV_DATA pRecvData = (LPRECV_DATA)lParam;
	CString strAutoID(pRecvData->szTrCode);

	if (0 == strAutoID.Compare(_T("11")))
	{
		long nData = 0;
		TCHAR data[1024] = { 0, };

		//g_iYuantaAPI.YOA_GetTRFieldLong( _T("11"), _T("OutBlock1"), _T("curjuka"), &nData );
		g_iYuantaAPI.YOA_GetTRFieldString(_T("11"), _T("OutBlock1"), _T("curjuka"), data, sizeof(data));
		//GetDlgItem(IDC_BT_CURJUKA)->SetWindowText(data);

		g_iYuantaAPI.YOA_GetTRFieldString(_T("11"), _T("OutBlock1"), _T("debi"), data, sizeof(data));
		//GetDlgItem(IDC_BT_DEBI)->SetWindowText(data);

		g_iYuantaAPI.YOA_GetTRFieldString(_T("11"), _T("OutBlock1"), _T("debirate"), data, sizeof(data));
		//GetDlgItem(IDC_BT_DEBIRATE)->SetWindowText(data);

		g_iYuantaAPI.YOA_GetTRFieldString(_T("11"), _T("OutBlock1"), _T("volume"), data, sizeof(data));
		//GetDlgItem(IDC_BT_VOLUME)->SetWindowText(data);
	}
	else if (0 == strAutoID.Compare(_T("71")))
	{
		byte gubun48 = 0;
		g_iYuantaAPI.YOA_GetTRFieldByte(_T("71"), _T("OutBlock1"), _T("gubun48"), &gubun48);

		TCHAR data[1024] = { 0, };
		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetTRFieldString(_T("71"), _T("OutBlock1"), _T("custseq"), data, sizeof(data));

		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetTRFieldString(_T("71"), _T("OutBlock1"), _T("origno"), data, sizeof(data));

		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetTRFieldString(_T("71"), _T("OutBlock1"), _T("ordno"), data, sizeof(data));

		memset(data, 0x00, sizeof(data));
		g_iYuantaAPI.YOA_GetTRFieldString(_T("71"), _T("OutBlock1"), _T("jumunno"), data, sizeof(data));

		__int64 cheprcise = 0;
		int nResult = g_iYuantaAPI.YOA_GetTRFieldLong64(_T("71"), _T("OutBlock1"), _T("jumunuv"), &cheprcise);

		if (ERROR_MAX_CODE < nResult)
		{

		}
		else	// ��ȸ ��û ���� ��, ���� ó��
		{
			TCHAR msg[2048] = { 0, };
			g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));

			//LogMessage(msg);
		}
	}

	return 0;
}

LRESULT YaClient::OnReceiveSystemMessage(WPARAM wParam, LPARAM lParam)
{
	return 1;
}

LRESULT YaClient::OnLoginComplete(WPARAM wParam, LPARAM lParam)
{
	return 1;
}

void YaClient::init()
{
// 	m_btnInitial.EnableWindow(FALSE);
// 
// 	int nCurSel = m_cbServer.GetCurSel();
// 	CString strServer = _T("simul.tradar.api.com");
// 	if (1 == nCurSel)
// 		strServer = _T("real.tradar.api.com");
// 	else if (2 == nCurSel)
// 		strServer = _T("simul.tradarglobal.api.com");
// 	else if (3 == nCurSel)
// 		strServer = _T("real.tradarglobal.api.com");
// 
// 	//if ( RESULT_SUCCESS == g_iYuantaAPI.YOA_Initial( strServer, GetSafeHwnd(), NULL, WMU_STARTMSGID ) )
// 	if (RESULT_SUCCESS == g_iYuantaAPI.YOA_Initial(strServer, GetSafeHwnd(), _T("C:\\YuantaAPI"), WMU_STARTMSGID))
// 	{
// 		if (0 == nCurSel || 2 == nCurSel)
// 			AfxMessageBox(_T("�������ڷ� �����մϴ�.\n���������� ���º�й�ȣ�� 0000�Դϴ�."));
// 
// 		LogMessage(_T("����Ÿ Open API�� �ʱ�ȭ�Ǿ����ϴ�."));
// 
// 		g_bInitial = TRUE;
// 		m_btnLogin.EnableWindow(TRUE);
// 		GetDlgItem(IDC_EDIT_LOGIN_ID)->SetFocus();
// 	}
// 	else
// 	{
// 		LogMessage(_T("����Ÿ Open API�� �ʱ�ȭ�� �����Ͽ����ϴ�."), 1);
// 		m_btnInitial.EnableWindow(TRUE);
// 
// 		return;
// 	}
}

void YaClient::log_in()
{
// 	if (CheckInit())
// 	{
// 		m_btnLogin.EnableWindow(FALSE);
// 
// 		UpdateData();
// 
// 		long nResult = g_iYuantaAPI.YOA_Login(GetSafeHwnd(), m_strLoginID, m_strLoginPwd, m_strCertPwd);
// 		if (RESULT_SUCCESS == nResult)
// 		{
// 			LogMessage(_T("�α��� ��û�� �Ǿ����ϴ�."));
// 		}
// 		else
// 		{
// 			LogMessage(_T("�α��� ��û�� �����Ͽ����ϴ�."), 1);
// 			TCHAR msg[2048] = { 0, };
// 
// 			g_iYuantaAPI.YOA_GetErrorMessage(nResult, msg, sizeof(msg));
// 			LogMessage(msg, 1, FALSE);
// 
// 			m_btnLogin.EnableWindow(TRUE);
// 		}
// 	}
}


void YaClient::OnDestroy()
{
	CDialog::OnDestroy();

	g_iYuantaAPI.YOA_UnInitial();
}
