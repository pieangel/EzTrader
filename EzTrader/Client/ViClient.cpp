// ViClient.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "../Order/OrderRequest/OrderRequest.h"
#include "ViClient.h"
#include "../Log/MyLogger.h"
#include "../Global/SmTotalManager.h"
#include "../Task/SmServerDataReceiver.h"
#include "ClientConst.h"
#include "../Json/json.hpp"
#include "../Util/VtStringUtil.h"
#include "../Login/SmLoginManager.h"
#include "../MainFrm.h"
#include "../Chart/SmChartConsts.h"
#include "../Chart/SmChartData.h"
#include "../Chart/SmChartDataManager.h"
#include "../Order/SmOrderRequest.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Util/SmUtil.h"
#include "../Account/SmAccountManager.h"
#include "../Account/SmAccount.h"
#include "../Event/SmCallbackManager.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Symbol/SmProduct.h"
#include "../DataFrame/Utils/DateTime.h"
#include "../DataFrame/Utils/FixedSizePriorityQueue.h"
#include "../DataFrame/Utils/FixedSizeString.h"
#include "../Config/SmConfigManager.h"
#include "../Order/SmOrderConst.h"
#include "../Global/SmTotalManager.h"
#include "../Order/OrderProcess/OrderProcessor.h"
#include <chartdir.h>


class CMainFrame;
using namespace DarkHorse;
using namespace nlohmann;

// ViClient dialog
const int AbroadFileDownloadCode = 332;
const int DomesticMasterFileDownloadCode = 331;

BEGIN_MESSAGE_MAP(ViClient, CDialog)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(ViClient, CDialog)
	ON_EVENT(ViClient, (UINT)-1, 3, OnDataRecv, VTS_BSTR VTS_I4)
	ON_EVENT(ViClient, (UINT)-1, 4, OnGetBroadData, VTS_BSTR VTS_I4)
	ON_EVENT(ViClient, (UINT)-1, 5, OnGetMsg, VTS_BSTR VTS_BSTR)
	ON_EVENT(ViClient, (UINT)-1, 6, OnGetMsgWithRqId, VTS_I4 VTS_BSTR VTS_BSTR)

END_EVENTSINK_MAP()


IMPLEMENT_DYNAMIC(ViClient, CDialog)

ViClient::ViClient(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_VI_CLIENT, pParent)
{
	
}

ViClient::~ViClient()
{
	if (m_CommAgent.GetSafeHwnd()) {
		if (m_CommAgent.CommGetConnectState() == 1) {
			//if (m_sUserId != "")
			//	m_CommAgent.CommLogout(m_sUserId);
		}

		m_CommAgent.CommTerminate(TRUE);
		m_CommAgent.DestroyWindow();
	}
}


void ViClient::OnDataRecv(LPCTSTR sTrRcvCode, LONG nRqID)
{
	const CString code = sTrRcvCode;
	const LONG req_id = nRqID;

	LOGINFO(CMyLogger::getInstance(), sTrRcvCode);

	if (code == DefAccountList) {
		OnAccountListReceived(code, req_id);
	}
	else if (code == MasterFile) {
		OnMasterFile(code, req_id);
	}
	else if (code == DefAbSymbolMaster) {
		OnSymbolMaster(code, req_id);
	}
	else if (code == DefSymbolMaster) {
		OnDmSymbolMaster(code, req_id);
	}
	else if (code == DefAbSymbolSise) {
		OnSymbolQuote(code, req_id);
	}
	else if (code == DefAbSymbolHoga) {
		OnSymbolHoga(code, req_id);
	}
	else if (code == DefAbAsset) {
		OnAccountAsset(code, req_id);
	}
	else if (code == DefDmAsset) {
		OnDmAccountAsset(code, req_id);
	}
	else if (code == DefAbAccountProfitLoss) {
		OnAccountProfitLoss(code, req_id);
	}
	else if (code == DefAbSymbolProfitLoss) {
		OnAccountProfitLoss(code, req_id);
	}
	else if (code == DefAbOutstanding) {
		OnSymbolProfitLoss(code, req_id);
	}
	else if (code == DefAbAccepted) {
		OnAcceptedList(code, req_id);
	}
	else if (code == DefAbFilled2) {
		OnFilledList(code, req_id);
	}
	else if (code == DefAbChartData) {
		
		auto it_arg = _ReqMap.find(nRqID);
		if (it_arg != _ReqMap.end())
			OnChartDataShort_Init(code, req_id);
		else
			OnChartDataShort(code, req_id);
	}
	else if (code == DefAbsChartData2) {
		auto it_arg = _ReqMap.find(nRqID);
		if (it_arg != _ReqMap.end()) 
			OnChartDataLong_Init(code, req_id);
		else
			OnChartDataLong(code, req_id);
	}
	else if (code == DefSymbolCode) {
		OnDomesticSymbolCode(code, req_id);
	}
	else if (code == DefChartData) {
		auto it_arg = _ReqMap.find(nRqID);
		if (it_arg != _ReqMap.end()) 
			OnDomesticChartData_Init(code, req_id);
		else
			OnDomesticChartData(code, req_id);
	}
}

void ViClient::OnGetBroadData(LPCTSTR strRecvKey, LONG nRealType)
{
	if (!_Enable) return;

	//LOGINFO(CMyLogger::getInstance(), strRecvKey);
	switch (nRealType)
	{
	//case 196:
 	case 296: // 해외 주문 접수
 		OnOrderAccepted(strRecvKey, nRealType); break;
	//case 186:
 	case 286: // 해외 주문 미체결
 		OnOrderUnfilled(strRecvKey, nRealType);	break;
	//case 189:
 	case 289: // 해외 주문 체결
 		OnOrderFilled(strRecvKey, nRealType); break;
	case 76: // 해외 실시간 호가
		OnRealtimeHoga(strRecvKey, nRealType); break;
	case 82: // 해외 실시간 체결
		OnRealtimeQuote(strRecvKey, nRealType); break;
	case 51: // 국내 선물 호가
	case 75:
		OnRealtimeDomesticHoga(strRecvKey, nRealType); break;
	case 52: // dm option hoga
		on_dm_option_hoga(strRecvKey, nRealType); break;
	case 58: // dm commodity future hoga
		on_dm_commodity_future_hoga(strRecvKey, nRealType); break;
	case 65: // 국내 선물 시세
	case 77:
		OnRealtimeDomesticQuote(strRecvKey, nRealType); break;
	case 66:
		on_dm_option_quote(strRecvKey, nRealType); break;
	case 71:
		on_dm_commodity_future_quote(strRecvKey, nRealType); break;
	case 310:
		on_dm_expected(strRecvKey, nRealType); break;
	case 261: // 국내 주문 접수
		on_dm_order_accepted(strRecvKey, nRealType); break;
	case 262: // 국내 주문 미체결
		on_dm_order_unfilled(strRecvKey, nRealType); break;
	case 265: // 해외 주문 체결
		on_dm_order_filled(strRecvKey, nRealType); break;
	case 183:
		on_dm_order_position(strRecvKey, nRealType); break;
	default:
		break;
	}
}

void ViClient::OnGetMsg(LPCTSTR strCode, LPCTSTR strMsg)
{
	CString strLog;
	strLog.Format("[%s][%s]", strCode, strMsg);
	LOGINFO(CMyLogger::getInstance(), strMsg);
}

void ViClient::OnGetMsgWithRqId(int nRqId, LPCTSTR strCode, LPCTSTR strMsg)
{
	try {
		CString strLog;
		auto found = _ReqMap.find(nRqId);
		if (found != _ReqMap.end()) {
			const auto& argument_map = found->second;
			if (argument_map.contains("tr_code")) {
				const std::string tr_code = std::any_cast<std::string>(found->second["tr_code"]);
				strLog.Format("RQID[%d][%s][%s]", nRqId, tr_code.c_str(), strMsg);
			}
			else {
				strLog.Format("RQID[%d][%s][%s]", nRqId, strCode, strMsg);
			}
		}
		strLog.Format("RQID[%d][%s][%s]", nRqId, strCode, strMsg);
		CString code(strCode);
		if (code.Compare("0332") == 0) {
			const int res_code = std::stoi((const char*)code);
			OnTaskComplete(nRqId);
		}

		auto it = _ChartReqMap.find(nRqId);
		if (it != _ChartReqMap.end()) {
			if (code.Compare("91001") == 0 || code.Compare("-2712") == 0) {
				_ChartReqMap.erase(it);
				AfxMessageBox(strMsg);
			}
		}

	

		if (nRqId == _CheckPwdReqId) {
			auto found = _ReqMap.find(nRqId);
			if (found != _ReqMap.end()) {
				const std::string account_no = std::any_cast<std::string>(found->second["account_no"]);
				const int result = _ttoi(code) == 0 ? 1 : 0;
				auto account = mainApp.AcntMgr()->FindAccount(account_no);
				if (account) {
					const int account_id = account->id();
					account->Confirm(result);
					mainApp.CallbackMgr()->OnPasswordConfirmed(account_id, result);
				}
			}
			_CheckPwdReqId = -1;
		}

		mainApp.TotalOrderMgr()->ServerMsg = strMsg;
		mainApp.CallbackMgr()->OnServerMsg(_ttoi(strCode));
	
		LOGINFO(CMyLogger::getInstance(), (LPCTSTR)strLog);

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void ViClient::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


int ViClient::ConnectToServer()
{
	int nRet = -1000;
	if (!m_CommAgent.GetSafeHwnd()) {
		m_CommAgent.Create("HDF CommAgent", WS_CHILD, CRect(0, 0, 0, 0), this, 2286);
	}
	else return nRet;

	if (m_CommAgent.GetSafeHwnd()) {
		nRet = m_CommAgent.CommInit(1);
	}

	return nRet;
}


int DarkHorse::ViClient::Login(const std::string& id, const std::string& pwd, const std::string& cert)
{
	const char* pLoginSuccess[] = { "로그인 성공"	, "Login Successful" };
	const char* pLoginFail[] = { "로그인 실패"	, "Login Failure" };

	int nRet = m_CommAgent.CommLogin(id.c_str(), pwd.c_str(), cert.c_str());
	if (nRet > 0) {
		//AfxMessageBox(pLoginSuccess[0]);

		//로긴후 반드시 호출...
		// 계좌 정보를 가져온다.
		m_CommAgent.CommAccInfo();
		//LOG_F(INFO, pLoginSuccess[0]);
		LOGINFO(CMyLogger::getInstance(), pLoginSuccess[0]);
	}
	else {
		CString strRet;
		strRet.Format("[%d]", nRet);
		//AfxMessageBox(pLoginFail[0] + strRet);
		//LOG_F(INFO, _T("%s"), strRet);
		LOGINFO(CMyLogger::getInstance(), pLoginFail[0]);
	}

	return nRet;
}

int DarkHorse::ViClient::Login(task_arg&& arg)
{
	int result = -1;
	try {
		const std::string type_name = arg["id"].type().name();
		const std::string id = std::any_cast<const std::string>(arg["id"]);
		const std::string pwd = std::any_cast<const std::string>(arg["pwd"]);
		const std::string cert = std::any_cast<const std::string>(arg["cert"]);

		result = Login(id, pwd, cert);

		if (result > 0) {
			mainApp.LoginMgr()->SaveUserInfo(id, pwd, cert);
			mainApp.LoginMgr()->IsLoggedIn(true);

			LOGINFO(CMyLogger::getInstance(), "로그인 성공 사용자 저장 user id = %s", id.c_str());

			((CMainFrame*)AfxGetMainWnd())->SetAccountInfo();
		}
	}
	catch (const std::exception & e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
	return result;
}

int DarkHorse::ViClient::CheckDmAccountPassword(task_arg&& arg)
{
	int nRqID = -1;
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);
		//const int task_id = std::any_cast<int>(arg["task_id"]);

		std::string reqString;
		std::string temp;
		// 계좌 번호
		temp = VtStringUtil::PadRight(account_no, ' ', 11);
		reqString.append(temp);
		// 비밀번호
		temp = VtStringUtil::PadRight(password, ' ', 8);
		reqString.append(temp);



		const CString sInput = reqString.c_str();
		//LOG_F(INFO, _T("AbGetAsset code = %s, input = %s"), DEF_Ab_Asset, sInput);
		const CString strNextKey = _T("");
		nRqID = m_CommAgent.CommRqData(DefDmAsset, sInput, sInput.GetLength(), strNextKey);
		_ReqMap[nRqID] = arg;
		_CheckPwdReqId = nRqID;
	}
	catch (const std::exception& e) {
		std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), error = "%s", error.c_str());
	}

	return nRqID;
}

int DarkHorse::ViClient::CheckAccountPassword(task_arg&& arg)
{
	int nRqID = -1;
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);
		const std::string type = std::any_cast<std::string>(arg["type"]);

		std::string reqString;
		std::string temp;

		if (type == "1") {
			// 계좌 번호
			temp = VtStringUtil::PadRight(account_no, ' ', 6);
			reqString.append(temp);
			// 비밀번호
			temp = VtStringUtil::PadRight(password, ' ', 8);
			reqString.append(temp);

			const CString sInput = reqString.c_str();
			//LOG_F(INFO, _T("AbGetAsset code = %s, input = %s"), DEF_Ab_Asset, sInput);
			const CString strNextKey = _T("");

			nRqID = m_CommAgent.CommRqData(DefAbAsset, sInput, sInput.GetLength(), strNextKey);
			_ReqMap[nRqID] = arg;
			_CheckPwdReqId = nRqID;
		}
		else if (type == "9") {
			// 계좌 번호
			temp = VtStringUtil::PadRight(account_no, ' ', 11);
			reqString.append(temp);
			// 비밀번호
			temp = VtStringUtil::PadRight(password, ' ', 8);
			reqString.append(temp);



			const CString sInput = reqString.c_str();
			//LOG_F(INFO, _T("AbGetAsset code = %s, input = %s"), DEF_Ab_Asset, sInput);
			const CString strNextKey = _T("");
			nRqID = m_CommAgent.CommRqData(DefDmAsset, sInput, sInput.GetLength(), strNextKey);
			_ReqMap[nRqID] = arg;
			_CheckPwdReqId = nRqID;
		}
	}
	catch (const std::exception& e) {
		std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), error = "%s", error.c_str());
	}

	return nRqID;
}

int DarkHorse::ViClient::CheckDmAccountPassword(const std::string& account_no, const std::string& password)
{
	std::string reqString;
	std::string temp;
	// 계좌 번호
	temp = VtStringUtil::PadRight(account_no, ' ', 11);
	reqString.append(temp);
	// 비밀번호
	temp = VtStringUtil::PadRight(password, ' ', 8);
	reqString.append(temp);



	const CString sInput = reqString.c_str();
	//LOG_F(INFO, _T("AbGetAsset code = %s, input = %s"), DEF_Ab_Asset, sInput);
	const CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(DefDmAsset, sInput, sInput.GetLength(), strNextKey);
	//_ReqMap[nRqID] = arg;
	_CheckPwdReqId = nRqID;
	return nRqID;
}
int DarkHorse::ViClient::CheckAbAccountPassword(const std::string& account_no, const std::string& password)
{
	std::string reqString;
	std::string temp;
	// 계좌 번호
	temp = VtStringUtil::PadRight(account_no, ' ', 6);
	reqString.append(temp);
	// 비밀번호
	temp = VtStringUtil::PadRight(password, ' ', 8);
	reqString.append(temp);



	const CString sInput = reqString.c_str();
	//LOG_F(INFO, _T("AbGetAsset code = %s, input = %s"), DEF_Ab_Asset, sInput);
	const CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(DefAbAsset, sInput, sInput.GetLength(), strNextKey);
	//_ReqMap[nRqID] = arg;
	_CheckPwdReqId = nRqID;
	return nRqID;
}

void DarkHorse::ViClient::RemoveTask(const int& task_id)
{
	for (auto it = _ReqMap.begin(); it != _ReqMap.end(); ++it) {
		const int _task_id = std::any_cast<const int>(it->second["task_id"]);
		if (task_id == _task_id) {
			_ReqMap.erase(it);
			break;
		}
	}
}

int DarkHorse::ViClient::DownloadFile(task_arg&& arg)
{
	try {
		const std::string range = std::any_cast<const char*>(arg["range"]);
		const std::string type_name = arg["task_id"].type().name();
		//const int task_id = std::any_cast<const int>(arg["task_id"]);
		_ReqMap[AbroadFileDownloadCode] = arg;
		m_CommAgent.CommReqMakeCod(range.c_str(), 0);
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return 1;
}

int ViClient::DownloadMasterFile(task_arg&& arg)
{
	try {
		const std::string file_name = std::any_cast<std::string>(arg["file_name"]);
		const std::string type_name = arg["task_id"].type().name();
		//const int task_id = std::any_cast<const int>(arg["task_id"]);

		CString sInput = file_name.c_str();
		const CString strNextKey = _T("");
		int nRqID = m_CommAgent.CommRqData(MasterFile, sInput, sInput.GetLength(), strNextKey);

		_ReqMap[nRqID] = arg;

		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetSymbolMaster(task_arg&& arg)
{
	try {
		const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);



		CString msg;
		msg.Format("GetSymbolMaster = %s", symbol_code.c_str());
		TRACE(msg);

		//const int task_id = std::any_cast<int>(arg["task_id"]);
		std::string temp;
		temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
		const CString sInput = temp.c_str();
		const CString sReqFidInput = "000001002003004005006007008009010011012013014015016017018019020021022023";
		const CString strNextKey = m_CommAgent.CommGetNextKey(0, "");
		const int nRqID = m_CommAgent.CommFIDRqData(DefAbSymbolMaster, sInput, sReqFidInput, sInput.GetLength(), strNextKey);

		_ReqMap[nRqID] = arg;

		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
	
	return -1;
}

int DarkHorse::ViClient::GetSymbolQuote(task_arg&& arg)
{
	try {
		const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);
		//const int task_id = std::any_cast<int>(arg["task_id"]);
		std::string temp;
		temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
		const CString sInput = temp.c_str();
		const CString sReqFidInput = "000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037";
		const CString strNextKey = m_CommAgent.CommGetNextKey(0, "");
		const int nRqID = m_CommAgent.CommFIDRqData(DefAbSymbolSise, sInput, sReqFidInput, sInput.GetLength(), strNextKey);

		CString msg;
		msg.Format("symbol_code = %s\n", symbol_code.c_str());
		//TRACE(msg);

		_ReqMap[nRqID] = arg;

		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetSymbolHoga(task_arg&& arg)
{
	try {
		const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);
		//const int task_id = std::any_cast<int>(arg["task_id"]);
		std::string temp;
		temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
		const CString sInput = temp.c_str();
		const CString sReqFidInput = _T("000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037038039040041042043044045046047048049050051052053054055056057058059060061");
		const CString strNextKey = m_CommAgent.CommGetNextKey(0, "");
		int nRqID = m_CommAgent.CommFIDRqData(DefAbSymbolHoga, sInput, sReqFidInput, sInput.GetLength(), strNextKey);

		_ReqMap[nRqID] = arg;

		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetAccountAsset(task_arg&& arg)
{
	int nRqID = -1;
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);
		//const int task_id = std::any_cast<int>(arg["task_id"]);

		std::string reqString;
		std::string temp;
		// 계좌 번호
		temp = VtStringUtil::PadRight(account_no, ' ', 6);
		reqString.append(temp);
		// 비밀번호
		temp = VtStringUtil::PadRight(password, ' ', 8);
		reqString.append(temp);

		

		const CString sInput = reqString.c_str();
		//LOG_F(INFO, _T("AbGetAsset code = %s, input = %s"), DEF_Ab_Asset, sInput);
		const CString strNextKey = _T("");
		nRqID = m_CommAgent.CommRqData(DefAbAsset, sInput, sInput.GetLength(), strNextKey);
		_ReqMap[nRqID] = arg;
		//_TaskInfoMap[nRqID] = account_no;
	}
	catch (const std::exception& e) {
		std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), error = "%s", error.c_str());
	}

	return nRqID;
}

int DarkHorse::ViClient::GetDeposit(task_arg&& arg)
{
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);
		//const int task_id = std::any_cast<int>(arg["task_id"]);

		std::string reqString;
		std::string temp;
		reqString.append("1");
		temp = VtStringUtil::PadRight(mainApp.LoginMgr()->id(), ' ', 8);
		// 아이디 
		reqString.append(temp);

		temp = VtStringUtil::PadRight(account_no, ' ', 6);
		reqString.append(temp);
		temp = VtStringUtil::PadRight(password, ' ', 8);
		reqString.append(temp);
		// 그룹명 - 공백
		reqString.append("                    ");
		// 통화코드
		reqString.append("USD");


		const CString sInput = reqString.c_str();
		const CString strNextKey = _T("");
		const int nRqID = m_CommAgent.CommRqData(DefAbAccountProfitLoss, sInput, sInput.GetLength(), strNextKey);
		_ReqMap[nRqID] = arg;

		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetAccountProfitLoss(task_arg&& arg)
{
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);
		//const int task_id = std::any_cast<int>(arg["task_id"]);

		std::string reqString;
		std::string temp;
		reqString.append("1");
		temp = VtStringUtil::PadRight(mainApp.LoginMgr()->id(), ' ', 8);
		// 아이디 
		reqString.append(temp);

		temp = VtStringUtil::PadRight(account_no, ' ', 6);
		reqString.append(temp);
		temp = VtStringUtil::PadRight(password, ' ', 8);
		reqString.append(temp);
		// 그룹명 - 공백
		reqString.append("                    ");
		// 통화코드
		reqString.append("USD");


		const CString sInput = reqString.c_str();
		const CString strNextKey = _T("");
		const int nRqID = m_CommAgent.CommRqData(DefAbSymbolProfitLoss, sInput, sInput.GetLength(), strNextKey);
		_ReqMap[nRqID] = arg;

		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetSymbolProfitLoss(task_arg&& arg)
{
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);
		//const int task_id = std::any_cast<int>(arg["task_id"]);
		std::string reqString;
		std::string temp;
		reqString.append("1");
		temp = VtStringUtil::PadRight(mainApp.LoginMgr()->id(), ' ', 8);
		// 아이디 
		reqString.append(temp);

		temp = VtStringUtil::PadRight(account_no, ' ', 6);
		reqString.append(temp);
		temp = VtStringUtil::PadRight(password, ' ', 8);
		reqString.append(temp);
		// 그룹명 - 공백
		reqString.append("                    ");


		const CString sInput = reqString.c_str();
		const CString strNextKey = _T("");
		const int nRqID = m_CommAgent.CommRqData(DefAbOutstanding, sInput, sInput.GetLength(), strNextKey);
		_ReqMap[nRqID] = arg;
		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetAbAcceptedOrderList(task_arg arg)
{
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);
		//const int task_id = std::any_cast<int>(arg["task_id"]);

		std::string reqString;
		std::string temp;
		reqString.append("1");
		temp = VtStringUtil::PadRight(mainApp.LoginMgr()->id(), ' ', 8);
		// 아이디 
		reqString.append(temp);

		temp = VtStringUtil::PadRight(account_no, ' ', 6);
		reqString.append(temp);
		temp = VtStringUtil::PadRight(password, ' ', 8);
		reqString.append(temp);
		// 그룹명 - 공백
		reqString.append("                    ");



		const CString sInput = reqString.c_str();
		const CString strNextKey = _T("");
		const int nRqID = m_CommAgent.CommRqData(DefAbAccepted, sInput, sInput.GetLength(), strNextKey);
		//arg["tr_code"] = std::string(DefAbAccepted);
		_ReqMap[nRqID] = arg;

		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}
int DarkHorse::ViClient::GetDmAcceptedOrderList(task_arg arg)
{
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);

		std::string reqString;
		std::string temp;
		temp = VtStringUtil::PadRight(account_no, ' ', 11);
		reqString.append(temp);
		reqString.append(_T("001"));
		temp = VtStringUtil::PadRight(password, ' ', 8);
		reqString.append(temp);


		CString sTrCode = "g11002.DQ0104&";
		CString sInput = reqString.c_str();
		CString strNextKey = _T("");
		int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
		//arg["tr_code"] = std::string(sTrCode);
		_ReqMap[nRqID] = arg;

		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetAcceptedOrderList(task_arg&& arg)
{
	try {
		const std::string type = std::any_cast<std::string>(arg["type"]);
		if (type == "1") GetAbAcceptedOrderList(arg);
		else if (type == "9") GetDmAcceptedOrderList(arg);
		return 1;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetDomesticSymbolCode(task_arg&& arg)
{
	try {
		const std::string product_code = std::any_cast<std::string>(arg["product_code"]);
		
		CString sTrCode = DefSymbolCode;
		CString sInput = product_code.c_str();
		CString strNextKey = _T("");
		
		const int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
		_ReqMap[nRqID] = arg;

		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());

		return -1;
	}
}

int DarkHorse::ViClient::GetFilledOrderList(const std::string& account_no, const std::string& password)
{
	try {
		

		std::string reqString;
		std::string temp;
		reqString.append("1");
		temp = VtStringUtil::PadRight(mainApp.LoginMgr()->id(), ' ', 8);
		// 아이디 
		reqString.append(temp);

		//const std::string cur_date = VtStringUtil::getCurentDate();
		CString m_strBusinessDay = m_CommAgent.CommGetBusinessDay(1);
		reqString.append(m_strBusinessDay);



		temp = VtStringUtil::PadRight(account_no, ' ', 6);
		reqString.append(temp);
		temp = VtStringUtil::PadRight(password, ' ', 8);
		reqString.append(temp);
		// 그룹명 - 공백
		reqString.append("                    ");



		const CString sInput = reqString.c_str();
		const CString strNextKey = _T("");
		const int nRqID = m_CommAgent.CommRqData(DefAbFilled2, sInput, sInput.GetLength(), strNextKey);
		//_ReqMap[nRqID] = arg;

		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetFilledOrderList(task_arg&& arg)
{
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);
		//const int task_id = std::any_cast<int>(arg["task_id"]);

		std::string reqString;
		std::string temp;
		reqString.append("1");
		temp = VtStringUtil::PadRight(mainApp.LoginMgr()->id(), ' ', 8);
		// 아이디 
		reqString.append(temp);

		//const std::string cur_date = VtStringUtil::getCurentDate();
		CString m_strBusinessDay = m_CommAgent.CommGetBusinessDay(1);
		reqString.append(m_strBusinessDay);



		temp = VtStringUtil::PadRight(account_no, ' ', 6);
		reqString.append(temp);
		temp = VtStringUtil::PadRight(password, ' ', 8);
		reqString.append(temp);
		// 그룹명 - 공백
		reqString.append("                    ");



		const CString sInput = reqString.c_str();
		const CString strNextKey = _T("");
		const int nRqID = m_CommAgent.CommRqData(DefAbFilled2, sInput, sInput.GetLength(), strNextKey);
		_ReqMap[nRqID] = arg;

		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetChartData(task_arg&& arg)
{
	try {
		const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);

		if (std::isdigit(symbol_code.at(2))) {
			if (symbol_code.length() < 8)
				return -1;
			return GetChartDataDomestic(std::move(arg));
		}
		else {
			const int chart_type = std::any_cast<int>(arg["chart_type"]);
			const int cycle = std::any_cast<int>(arg["cycle"]);
			if (symbol_code.length() < 4)
				return -1;
			if (chart_type == SmChartType::TICK)
				return GetChartDataAbroadLong(std::move(arg));
			else if (chart_type == SmChartType::MIN) {
				if (cycle <= 60)
					return GetChartDataAbroadShort(std::move(arg));
				else
					return GetChartDataAbroadLong(std::move(arg));
			}
			else if (chart_type == SmChartType::DAY)
				return GetChartDataAbroadLong(std::move(arg));
			else if (chart_type == SmChartType::WEEK)
				return GetChartDataAbroadLong(std::move(arg));
			else if (chart_type == SmChartType::MON)
				return GetChartDataAbroadLong(std::move(arg));
			else
				return GetChartDataAbroadShort(std::move(arg));
		}

		return 1;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}
int ViClient::GetDmSymbolMaster(SmTaskArg&& arg)
{
	if (arg.TaskType != SmTaskType::DmSymbolMaster) return -1;
	try {
		auto req = std::any_cast<DarkHorse::DmSymbolMasterReq>(arg.Param);

		CString sInput;
		sInput = req.symbol_code.c_str();
		sInput.Append(_T("40001"));
		CString sReqFidInput = _T("000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037038039040041042043044045046047048049050051052053054055056057058059060061062063064065066067068069070071072073074075076077078079080081082083084085086087088089090091092093094095096097098099100101102103104105106107108109110111112113114115116117118119120121122123124125126127128129130131132133134135136137138139140141142143144145146147148149150151152153154155156157158159160161162163164165166167168169170171172173174175176177178179180181182183184185186187188189190191192193194195196197198199200201202203204205206207208209210211212213214215216217218219220221222223224225226227228229230231232");
		CString strNextKey = _T("");
		int nRqID = m_CommAgent.CommFIDRqData(DefSymbolMaster, sInput, sReqFidInput, sInput.GetLength(), strNextKey);
		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		return -1;
	}
	return 1;
}
int DarkHorse::ViClient::GetChartData(SmTaskArg&& arg)
{
	if (arg.TaskType != SmTaskType::ChartData) return -1;

	try {
		auto req = std::any_cast<SmChartDataReq>(arg.Param);

		if (std::isdigit(req.SymbolCode.at(2))) {
			if (req.SymbolCode.length() < 8)
				return -1;
			return GetChartDataDomestic(std::move(arg));
		}
		else {
			if (req.SymbolCode.length() < 4)
				return -1;
			if (req.ChartType == SmChartType::TICK)
				return GetChartDataAbroadLong(std::move(arg));
			else if (req.ChartType == SmChartType::MIN) {
				if (req.Cycle <= 60)
					return GetChartDataAbroadShort(std::move(arg));
				else
					return GetChartDataAbroadLong(std::move(arg));
			}
			else if (req.ChartType == SmChartType::DAY)
				return GetChartDataAbroadLong(std::move(arg));
			else if (req.ChartType == SmChartType::WEEK)
				return GetChartDataAbroadLong(std::move(arg));
			else if (req.ChartType == SmChartType::MON)
				return GetChartDataAbroadLong(std::move(arg));
			else
				return GetChartDataAbroadShort(std::move(arg));
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		return -1;
	}

	return 1;
}



void DarkHorse::ViClient::NewOrder(task_arg&& arg)
{
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);
		const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);
		const int position_type = std::any_cast<int>(arg["position_type"]);
		const int price_type = std::any_cast<int>(arg["price_type"]);
		const int filled_condition = std::any_cast<int>(arg["filled_condition"]);
		const int order_price = std::any_cast<int>(arg["order_price"]);
		const int order_amount = std::any_cast<int>(arg["order_amount"]);
		const int request_id = std::any_cast<int>(arg["request_id"]);

		std::string orderString;
		std::string temp;
		// 계좌 번호
		temp = VtStringUtil::PadRight(account_no, ' ', 6);
		orderString.append(temp);
		// 비밀번호
		temp = VtStringUtil::PadRight(password, ' ', 8);
		orderString.append(temp);
		// 종목 코드
		temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
		orderString.append(temp);

		// 매매구분
		if (static_cast<SmPositionType>(position_type) == SmPositionType::Buy)
			orderString.append(_T("1"));
		else if (static_cast<SmPositionType>(position_type) == SmPositionType::Sell)
			orderString.append(_T("2"));

		// 가격 조건
		if (static_cast<SmPriceType>(price_type) == SmPriceType::Price)
			orderString.append(_T("1"));
		else if (static_cast<SmPriceType>(price_type) == SmPriceType::Market)
			orderString.append(_T("2"));
		// 체결 조건
		if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Fas)
			orderString.append(_T("1"));
		else if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Fok)
			orderString.append(_T("2"));
		else if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Fak)
			orderString.append(_T("3"));
		else if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Day)
			orderString.append(_T("0"));

		// 주문 가격
		if (static_cast<SmPriceType>(price_type) == SmPriceType::Price)
			temp = VtStringUtil::PadRight(order_price, ' ', 15);
		else if (static_cast<SmPriceType>(price_type) == SmPriceType::Market)
			temp = VtStringUtil::PadRight(0, ' ', 15);
		orderString.append(temp);

		// 주문 수량
		temp = VtStringUtil::PadRight(order_amount, ' ', 10);
		orderString.append(temp);
		// 기타 설정
		temp = VtStringUtil::PadRight(1, ' ', 35);
		orderString.append(temp);

		std::string userDefined;
		std::string req_id = std::to_string(request_id);
		req_id = VtStringUtil::PadLeft(req_id, '0', 10);
		userDefined.append(req_id);
		userDefined.append("k");

		std::string mac_address;
		mac_address = SmUtil::GetMacAddress();
		userDefined.append(mac_address);
		userDefined.append("m");

		temp = VtStringUtil::PadRight(userDefined, '0', 60);

		orderString.append(temp);

		CString sTrCode = "g12003.AO0401%";
		CString sInput = orderString.c_str();
		int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
		_ReqMap[nRqID] = arg;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}


void ViClient::ChangeOrder(order_request_p order_req)
{
	if (!order_req || order_req->request_type == OrderRequestType::None) return;

	if (order_req->request_type == OrderRequestType::Abroad)
		ab_change_order(order_req);
	else
		dm_change_order(order_req);
}

void DarkHorse::ViClient::NewOrder(const std::shared_ptr<SmOrderRequest>& order_req)
{
	const std::string account_no = order_req->AccountNo;
	const std::string password = order_req->Password;
	const std::string symbol_code = order_req->SymbolCode;
	const SmPositionType position_type = order_req->PositionType;
	const SmPriceType price_type = order_req->PriceType;
	const SmFilledCondition filled_condition = order_req->FilledCond;
	const int order_price = order_req->OrderPrice;
	const int order_amount = order_req->OrderAmount;
	const int request_id = order_req->RequestId;

	LOGINFO(CMyLogger::getInstance(), "NewOrder::  account_no = %s, symbol_code = %s, order_amount = %d", account_no.c_str(), symbol_code.c_str(), order_amount);


	std::string orderString;
	std::string temp;
	// 계좌 번호
	temp = VtStringUtil::PadRight(account_no, ' ', 6);
	orderString.append(temp);
	// 비밀번호
	temp = VtStringUtil::PadRight(password, ' ', 8);
	orderString.append(temp);
	// 종목 코드
	temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
	orderString.append(temp);

	// 매매구분
	if ((position_type) == SmPositionType::Buy)
		orderString.append(_T("1"));
	else if ((position_type) == SmPositionType::Sell)
		orderString.append(_T("2"));

	// 가격 조건
	if ((price_type) == SmPriceType::Price)
		orderString.append(_T("1"));
	else if ((price_type) == SmPriceType::Market)
		orderString.append(_T("2"));
	// 체결 조건
	if ((filled_condition) == SmFilledCondition::Fas)
		orderString.append(_T("1"));
	else if ((filled_condition) == SmFilledCondition::Fok)
		orderString.append(_T("2"));
	else if ((filled_condition) == SmFilledCondition::Fak)
		orderString.append(_T("3"));
	else if ((filled_condition) == SmFilledCondition::Day)
		orderString.append(_T("0"));

	// 주문 가격
	if ((price_type) == SmPriceType::Price)
		temp = VtStringUtil::PadRight(order_price, ' ', 15);
	else if ((price_type) == SmPriceType::Market)
		temp = VtStringUtil::PadRight(0, ' ', 15);
	orderString.append(temp);

	// 주문 수량
	temp = VtStringUtil::PadRight(order_amount, ' ', 10);
	orderString.append(temp);
	// 기타 설정
	temp = VtStringUtil::PadRight(1, ' ', 35);
	orderString.append(temp);

	std::string userDefined;
	std::string req_id = std::to_string(request_id);
	req_id = VtStringUtil::PadLeft(req_id, '0', 10);
	userDefined.append(req_id);
	userDefined.append("k");

	std::string mac_address;
	mac_address = SmUtil::GetMacAddress();
	userDefined.append(mac_address);
	userDefined.append("m");
	
	temp = VtStringUtil::PadRight(userDefined, '0', 60);

	orderString.append(temp);

	CString sTrCode = "g12003.AO0401%";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
	mainApp.OrderReqMgr()->AddOrderRequest(order_req);
	_OrderReqMap[nRqID] = order_req;
}


void DarkHorse::ViClient::ChangeOrder(task_arg&& arg)
{
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);
		const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);
		const std::string ori_order_no = std::any_cast<std::string>(arg["ori_order_no"]);
		const int position_type = std::any_cast<int>(arg["position_type"]);
		const int price_type = std::any_cast<int>(arg["price_type"]);
		const int filled_condition = std::any_cast<int>(arg["filled_condition"]);
		const int order_price = std::any_cast<int>(arg["order_price"]);
		const int order_amount = std::any_cast<int>(arg["order_amount"]);
		const int request_id = std::any_cast<int>(arg["request_id"]);

		std::string orderString;
		std::string temp;
		// 계좌 번호
		temp = VtStringUtil::PadRight(account_no, ' ', 6);
		orderString.append(temp);
		// 비밀번호
		temp = VtStringUtil::PadRight(password, ' ', 8);
		orderString.append(temp);
		// 종목 코드
		temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
		orderString.append(temp);

		// 가격 조건
		if (static_cast<SmPriceType>(price_type) == SmPriceType::Price)
			orderString.append(_T("1"));
		else if (static_cast<SmPriceType>(price_type) == SmPriceType::Market)
			orderString.append(_T("2"));
		// 체결 조건
		if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Fas)
			orderString.append(_T("1"));
		else if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Fok)
			orderString.append(_T("2"));
		else if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Fak)
			orderString.append(_T("3"));
		else if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Day)
			orderString.append(_T("0"));

		// 주문 가격
		if (static_cast<SmPriceType>(price_type) == SmPriceType::Price)
			temp = VtStringUtil::PadRight(order_price, ' ', 15);
		else if (static_cast<SmPriceType>(price_type) == SmPriceType::Market)
			temp = VtStringUtil::PadRight(0, ' ', 15);
		orderString.append(temp);

		// 정정 수량
		temp = VtStringUtil::PadRight(order_amount, ' ', 10);
		orderString.append(temp);
		// 정정이나 취소시 원주문 번호
		temp = VtStringUtil::PadRight(ori_order_no, ' ', 10);
		orderString.append(temp);
		// 기타설정
		temp = VtStringUtil::PadRight(1, ' ', 26);
		orderString.append(temp);

		std::string userDefined;
		std::string req_id = std::to_string(request_id);
		req_id = VtStringUtil::PadLeft(req_id, '0', 10);
		userDefined.append(req_id);
		userDefined.append("k");

		std::string mac_address;
		mac_address = SmUtil::GetMacAddress();
		userDefined.append(mac_address);
		userDefined.append("m");

		temp = VtStringUtil::PadRight(userDefined, '0', 60);

		orderString.append(temp);

		CString sTrCode = "g12003.AO0402%";
		CString sInput = orderString.c_str();
		int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
		
		_ReqMap[nRqID] = arg;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

}


void ViClient::CancelOrder(order_request_p order_req)
{
	if (!order_req || order_req->request_type == OrderRequestType::None) return;
	if (order_req->request_type == OrderRequestType::Abroad)
		ab_cancel_order(order_req);
	else
		dm_cancel_order(order_req);
}

void DarkHorse::ViClient::ChangeOrder(const std::shared_ptr<SmOrderRequest>& order_req)
{
	if (!order_req) return;

	const std::string account_no = order_req->AccountNo;
	const std::string password = order_req->Password;
	const std::string symbol_code = order_req->SymbolCode;
	const std::string ori_order_no = order_req->OriOrderNo;
	const SmPositionType position_type = order_req->PositionType;
	const SmPriceType price_type = order_req->PriceType;
	const SmFilledCondition filled_condition = order_req->FilledCond;
	const int order_price = order_req->OrderPrice;
	const int order_amount = order_req->OrderAmount;
	const int request_id = order_req->RequestId;

	std::string orderString;
	std::string temp;
	// 계좌 번호
	temp = VtStringUtil::PadRight(account_no, ' ', 6);
	orderString.append(temp);
	// 비밀번호
	temp = VtStringUtil::PadRight(password, ' ', 8);
	orderString.append(temp);
	// 종목 코드
	temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
	orderString.append(temp);

	// 가격 조건
	if ((price_type) == SmPriceType::Price)
		orderString.append(_T("1"));
	else if ((price_type) == SmPriceType::Market)
		orderString.append(_T("2"));
	// 체결 조건
	if ((filled_condition) == SmFilledCondition::Fas)
		orderString.append(_T("1"));
	else if ((filled_condition) == SmFilledCondition::Fok)
		orderString.append(_T("2"));
	else if ((filled_condition) == SmFilledCondition::Fak)
		orderString.append(_T("3"));
	else if ((filled_condition) == SmFilledCondition::Day)
		orderString.append(_T("0"));

	// 주문 가격
	if (static_cast<SmPriceType>(price_type) == SmPriceType::Price)
		temp = VtStringUtil::PadRight(order_price, ' ', 15);
	else if (static_cast<SmPriceType>(price_type) == SmPriceType::Market)
		temp = VtStringUtil::PadRight(0, ' ', 15);
	orderString.append(temp);

	// 정정 수량
	temp = VtStringUtil::PadRight(order_amount, ' ', 10);
	orderString.append(temp);
	// 정정이나 취소시 원주문 번호
	temp = VtStringUtil::PadRight(ori_order_no, ' ', 10);
	orderString.append(temp);
	// 기타설정
	temp = VtStringUtil::PadRight(1, ' ', 26);
	orderString.append(temp);

	std::string userDefined;
	std::string req_id = std::to_string(request_id);
	req_id = VtStringUtil::PadLeft(req_id, '0', 10);
	userDefined.append(req_id);
	userDefined.append("k");

	std::string mac_address;
	mac_address = SmUtil::GetMacAddress();
	userDefined.append(mac_address);
	userDefined.append("m");

	temp = VtStringUtil::PadRight(userDefined, '0', 60);

	orderString.append(temp);

	CString sTrCode = "g12003.AO0402%";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
	mainApp.OrderReqMgr()->AddOrderRequest(order_req);
	_OrderReqMap[nRqID] = order_req;
}

void DarkHorse::ViClient::CancelOrder(task_arg&& arg)
{
	try {
		const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
		const std::string password = std::any_cast<std::string>(arg["password"]);
		const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);
		const std::string ori_order_no = std::any_cast<std::string>(arg["ori_order_no"]);
		const int position_type = std::any_cast<int>(arg["position_type"]);
		const int price_type = std::any_cast<int>(arg["price_type"]);
		const int filled_condition = std::any_cast<int>(arg["filled_condition"]);
		const int order_price = std::any_cast<int>(arg["order_price"]);
		const int order_amount = std::any_cast<int>(arg["order_amount"]);
		const int request_id = std::any_cast<int>(arg["request_id"]);

		std::string orderString;
		std::string temp;
		// 계좌 번호
		temp = VtStringUtil::PadRight(account_no, ' ', 6);
		orderString.append(temp);
		// 비밀번호
		temp = VtStringUtil::PadRight(password, ' ', 8);
		orderString.append(temp);
		// 종목 코드
		temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
		orderString.append(temp);

		// 가격 조건
		if (static_cast<SmPriceType>(price_type) == SmPriceType::Price)
			orderString.append(_T("1"));
		else if (static_cast<SmPriceType>(price_type) == SmPriceType::Market)
			orderString.append(_T("2"));
		// 체결 조건
		if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Fas)
			orderString.append(_T("1"));
		else if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Fok)
			orderString.append(_T("2"));
		else if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Fak)
			orderString.append(_T("3"));
		else if (static_cast<SmFilledCondition>(filled_condition) == SmFilledCondition::Day)
			orderString.append(_T("0"));

		// 주문 가격 15
		temp = "               ";
		orderString.append(temp);

		// 정정 수량 10
		temp = "          ";
		orderString.append(temp);
		// 정정이나 취소시 원주문 번호
		temp = VtStringUtil::PadRight(ori_order_no, ' ', 10);
		orderString.append(temp);
		// 기타설정 26
		temp = "                          ";
		orderString.append(temp);

		std::string userDefined;
		std::string req_id = std::to_string(request_id);
		req_id = VtStringUtil::PadLeft(req_id, '0', 10);
		userDefined.append(req_id);
		userDefined.append("k");

		std::string mac_address;
		mac_address = SmUtil::GetMacAddress();
		userDefined.append(mac_address);
		userDefined.append("m");

		temp = VtStringUtil::PadRight(userDefined, '0', 60);

		orderString.append(temp);

		CString sTrCode = "g12003.AO0403%";
		CString sInput = orderString.c_str();
		int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);

		_ReqMap[nRqID] = arg;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

}

void ViClient::ab_new_order(order_request_p order_req)
{
	std::string orderString;
	std::string temp;
	// 계좌 번호
	temp = VtStringUtil::PadRight(order_req->account_no, ' ', 6);
	orderString.append(temp);
	// 비밀번호
	temp = VtStringUtil::PadRight(order_req->password, ' ', 8);
	orderString.append(temp);
	// 종목 코드
	temp = VtStringUtil::PadRight(order_req->symbol_code, ' ', 32);
	orderString.append(temp);

	// 매매구분
	if (order_req->position_type == SmPositionType::Buy)
		orderString.append("1");
	else if (order_req->position_type == SmPositionType::Sell)
		orderString.append("2");

	// 가격 조건
	if (order_req->price_type == SmPriceType::Price)
		orderString.append("1");
	else if (order_req->price_type == SmPriceType::Market)
		orderString.append("2");
	// 체결 조건
	if (order_req->fill_condition == SmFilledCondition::Fas)
		orderString.append("1");
	else if (order_req->fill_condition == SmFilledCondition::Fok)
		orderString.append("2");
	else if (order_req->fill_condition == SmFilledCondition::Fak)
		orderString.append("3");
	else if (order_req->fill_condition == SmFilledCondition::Day)
		orderString.append("0");

	// 주문 가격
	if (order_req->price_type == SmPriceType::Price)
		temp = VtStringUtil::PadRight(order_req->order_price, ' ', 15);
	else if (order_req->price_type == SmPriceType::Market)
		temp = VtStringUtil::PadRight(0, ' ', 15);
	orderString.append(temp);

	// 주문 수량
	temp = VtStringUtil::PadRight(order_req->order_amount, ' ', 10);
	orderString.append(temp);
	// 기타 설정
	temp = VtStringUtil::PadRight(1, ' ', 35);
	orderString.append(temp);

	std::string userDefined;
	make_custom_order_info(order_req, userDefined);
	temp = VtStringUtil::PadRight(userDefined, '0', 60);
	orderString.append(temp);

	CString sTrCode = "g12003.AO0401%";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
	order_request_map[nRqID] = order_req;
}

void ViClient::ab_change_order(order_request_p order_req)
{
	std::string orderString;
	std::string temp;
	// 계좌 번호
	temp = VtStringUtil::PadRight(order_req->account_no, ' ', 6);
	orderString.append(temp);
	// 비밀번호
	temp = VtStringUtil::PadRight(order_req->password, ' ', 8);
	orderString.append(temp);
	// 종목 코드
	temp = VtStringUtil::PadRight(order_req->symbol_code, ' ', 32);
	orderString.append(temp);

	// 가격 조건
	if (order_req->price_type == SmPriceType::Price)
		orderString.append(_T("1"));
	else if (order_req->price_type == SmPriceType::Market)
		orderString.append(_T("2"));
	// 체결 조건
	if (order_req->fill_condition == SmFilledCondition::Fas)
		orderString.append(_T("1"));
	else if (order_req->fill_condition == SmFilledCondition::Fok)
		orderString.append(_T("2"));
	else if (order_req->fill_condition == SmFilledCondition::Fak)
		orderString.append(_T("3"));
	else if (order_req->fill_condition == SmFilledCondition::Day)
		orderString.append(_T("0"));

	// 주문 가격
	if (order_req->price_type == SmPriceType::Price)
		temp = VtStringUtil::PadRight(order_req->order_price, ' ', 15);
	else if (order_req->price_type == SmPriceType::Market)
		temp = VtStringUtil::PadRight(0, ' ', 15);
	orderString.append(temp);

	// 정정 수량
	temp = VtStringUtil::PadRight(order_req->order_amount, ' ', 10);
	orderString.append(temp);
	// 정정이나 취소시 원주문 번호
	temp = VtStringUtil::PadRight(order_req->original_order_no, ' ', 10);
	orderString.append(temp);
	// 기타설정
	temp = VtStringUtil::PadRight(1, ' ', 26);
	orderString.append(temp);

	std::string userDefined;
	make_custom_order_info(order_req, userDefined);
	temp = VtStringUtil::PadRight(userDefined, '0', 60);
	orderString.append(temp);

	CString sTrCode = "g12003.AO0402%";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
	order_request_map[nRqID] = order_req;
}

void ViClient::ab_cancel_order(order_request_p order_req)
{
	std::string orderString;
	std::string temp;
	// 계좌 번호
	temp = VtStringUtil::PadRight(order_req->account_no, ' ', 6);
	orderString.append(temp);
	// 비밀번호
	temp = VtStringUtil::PadRight(order_req->password, ' ', 8);
	orderString.append(temp);
	// 종목 코드
	temp = VtStringUtil::PadRight(order_req->symbol_code, ' ', 32);
	orderString.append(temp);

	// 가격 조건
	if (order_req->price_type == SmPriceType::Price)
		orderString.append(_T("1"));
	else if (order_req->price_type == SmPriceType::Market)
		orderString.append(_T("2"));
	// 체결 조건
	if (order_req->fill_condition == SmFilledCondition::Fas)
		orderString.append(_T("1"));
	else if (order_req->fill_condition == SmFilledCondition::Fok)
		orderString.append(_T("2"));
	else if (order_req->fill_condition == SmFilledCondition::Fak)
		orderString.append(_T("3"));
	else if (order_req->fill_condition == SmFilledCondition::Day)
		orderString.append(_T("0"));

	// 주문 가격 15
	temp = "               ";
	orderString.append(temp);

	// 정정 수량 10
	temp = "          ";
	orderString.append(temp);
	// 정정이나 취소시 원주문 번호
	temp = VtStringUtil::PadRight(order_req->original_order_no, ' ', 10);
	orderString.append(temp);

	// 기타설정 26
	temp = "                          ";
	orderString.append(temp);

	std::string userDefined;
	make_custom_order_info(order_req, userDefined);
	temp = VtStringUtil::PadRight(userDefined, '0', 60);
	orderString.append(temp);

	CString sTrCode = "g12003.AO0403%";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);

	order_request_map[nRqID] = order_req;
}

void ViClient::make_custom_order_info(const order_request_p& order_req, std::string& custom_order_info)
{
	std::string req_id = std::to_string(order_req->request_id);
	req_id = VtStringUtil::PadLeft(req_id, '0', 10);
	custom_order_info.append(req_id);
	custom_order_info.append("k");

	std::string mac_address;
	mac_address = SmUtil::GetMacAddress();
	custom_order_info.append(mac_address);
	custom_order_info.append("m");
}

void ViClient::dm_cancel_order(order_request_p order_req)
{
	std::string orderString;
	std::string temp;
	temp = VtStringUtil::PadRight(order_req->account_no, ' ', 11);
	orderString.append(temp);
	temp = VtStringUtil::PadRight(order_req->password, ' ', 8);
	orderString.append(temp);
	temp = VtStringUtil::PadRight(order_req->symbol_code, ' ', 32);
	orderString.append(temp);

	if (order_req->position_type == SmPositionType::Buy)
		orderString.append(_T("1"));
	else if (order_req->position_type == SmPositionType::Sell)
		orderString.append(_T("2"));

	if (order_req->price_type == SmPriceType::Price)
		orderString.append(_T("1"));
	else if (order_req->price_type == SmPriceType::Market)
		orderString.append(_T("2"));

	if (order_req->fill_condition == SmFilledCondition::Fas)
		orderString.append(_T("1"));
	else if (order_req->fill_condition == SmFilledCondition::Fok)
		orderString.append(_T("2"));
	else if (order_req->fill_condition == SmFilledCondition::Fak)
		orderString.append(_T("3"));

	if (order_req->price_type == SmPriceType::Price)
		temp = VtStringUtil::PadRight(order_req->order_price, ' ', 13);
	else if (order_req->price_type == SmPriceType::Market)
		temp = VtStringUtil::PadRight(0, ' ', 13);
	orderString.append(temp);

	temp = VtStringUtil::PadRight(order_req->order_amount, ' ', 5);
	orderString.append(temp);

	temp = VtStringUtil::PadLeft(order_req->original_order_no, '0', 7);
	orderString.append(temp);

	std::string userDefined;
	make_custom_order_info(order_req, userDefined);
	temp = VtStringUtil::PadRight(userDefined, '0', 60);
	orderString.append(temp);

	CString sTrCode = "g12001.DO1701&";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
	order_request_map[nRqID] = order_req;
}

void ViClient::dm_change_order(order_request_p order_req)
{
	std::string orderString;
	std::string temp;
	temp = VtStringUtil::PadRight(order_req->account_no, ' ', 11);
	orderString.append(temp);
	temp = VtStringUtil::PadRight(order_req->password, ' ', 8);
	orderString.append(temp);
	temp = VtStringUtil::PadRight(order_req->symbol_code, ' ', 32);
	orderString.append(temp);

	if (order_req->position_type == SmPositionType::Buy)
		orderString.append(_T("1"));
	else if (order_req->position_type == SmPositionType::Sell)
		orderString.append(_T("2"));

	if (order_req->price_type == SmPriceType::Price)
		orderString.append(_T("1"));
	else if (order_req->price_type == SmPriceType::Market)
		orderString.append(_T("2"));

	if (order_req->fill_condition == SmFilledCondition::Fas)
		orderString.append(_T("1"));
	else if (order_req->fill_condition == SmFilledCondition::Fok)
		orderString.append(_T("2"));
	else if (order_req->fill_condition == SmFilledCondition::Fak)
		orderString.append(_T("3"));

	if (order_req->price_type == SmPriceType::Price)
		temp = VtStringUtil::PadRight(order_req->order_price, ' ', 13);
	else if (order_req->price_type == SmPriceType::Market)
		temp = VtStringUtil::PadRight(0, ' ', 13);
	orderString.append(temp);

	temp = VtStringUtil::PadRight(order_req->order_amount, ' ', 5);
	orderString.append(temp);

	temp = VtStringUtil::PadLeft(order_req->original_order_no, '0', 7);
	orderString.append(temp);

	std::string userDefined;
	make_custom_order_info(order_req, userDefined);
	temp = VtStringUtil::PadRight(userDefined, '0', 60);
	orderString.append(temp);
	
	CString sTrCode = "g12001.DO1901&";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
	order_request_map[nRqID] = order_req;
}

void ViClient::dm_new_order(order_request_p order_req)
{
	std::string orderString;
	std::string temp;
	temp = VtStringUtil::PadRight(order_req->account_no, ' ', 11);
	orderString.append(temp);
	temp = VtStringUtil::PadRight(order_req->password, ' ', 8);
	orderString.append(temp);
	temp = VtStringUtil::PadRight(order_req->symbol_code, ' ', 32);
	orderString.append(temp);

	if (order_req->position_type == SmPositionType::Buy)
		orderString.append(_T("1"));
	else if (order_req->position_type == SmPositionType::Sell)
		orderString.append(_T("2"));

	if (order_req->price_type == SmPriceType::Price)
		orderString.append(_T("1"));
	else if (order_req->price_type == SmPriceType::Market)
		orderString.append(_T("2"));

	if (order_req->fill_condition == SmFilledCondition::Fas)
		orderString.append(_T("1"));
	else if (order_req->fill_condition == SmFilledCondition::Fok)
		orderString.append(_T("2"));
	else if (order_req->fill_condition == SmFilledCondition::Fak)
		orderString.append(_T("3"));

	if (order_req->price_type == SmPriceType::Price)
		temp = VtStringUtil::PadRight(order_req->order_price, ' ', 13);
	else if (order_req->price_type == SmPriceType::Market)
		temp = VtStringUtil::PadRight(0, ' ', 13);
	orderString.append(temp);

	temp = VtStringUtil::PadRight(order_req->order_amount, ' ', 5);
	orderString.append(temp);

	std::string userDefined;
	make_custom_order_info(order_req, userDefined);
	temp = VtStringUtil::PadRight(userDefined, '0', 60);
	orderString.append(temp);

	CString sTrCode = "g12001.DO1601&";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
	order_request_map[nRqID] = order_req;
}

void ViClient::NewOrder(order_request_p order_req)
{
	if (!order_req || order_req->request_type == OrderRequestType::None) return;

	if (order_req->request_type == OrderRequestType::Abroad)
		ab_new_order(order_req);
	else
		dm_new_order(order_req);
}

int DarkHorse::ViClient::ConvertToInt(CString& strSymbolCode, CString& strValue)
{
	strSymbolCode.Trim();
	strValue.Trim();
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol((LPCTSTR)strSymbolCode);
	if (!symbol) return -1;
	double converted_value = _ttof(strValue);
	converted_value = converted_value * pow(10, symbol->Decimal());
	return static_cast<int>(converted_value);
}

void DarkHorse::ViClient::UnregAllSymbol()
{
	for (auto it = _RegSymbolSet.begin(); it != _RegSymbolSet.end(); it++)
		UnregisterSymbol(*it);
}

void DarkHorse::ViClient::UnregAllAccount()
{
	for (auto it = _RegAccountSet.begin(); it != _RegAccountSet.end(); ++it)
		UnregisterAccount(*it);
}

void DarkHorse::ViClient::CancelOrder(const std::shared_ptr<SmOrderRequest>& order_req)
{
	const std::string account_no = order_req->AccountNo;
	const std::string password = order_req->Password;
	const std::string symbol_code = order_req->SymbolCode;
	const std::string ori_order_no = order_req->OriOrderNo;
	const SmPositionType position_type = order_req->PositionType;
	const SmPriceType price_type = order_req->PriceType;
	const SmFilledCondition filled_condition = order_req->FilledCond;
	const int order_price = order_req->OrderPrice;
	const int order_amount = order_req->OrderAmount;
	const int request_id = order_req->RequestId;

	std::string orderString;
	std::string temp;
	// 계좌 번호
	temp = VtStringUtil::PadRight(account_no, ' ', 6);
	orderString.append(temp);
	// 비밀번호
	temp = VtStringUtil::PadRight(password, ' ', 8);
	orderString.append(temp);
	// 종목 코드
	temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
	orderString.append(temp);

	// 가격 조건
	if ((price_type) == SmPriceType::Price)
		orderString.append(_T("1"));
	else if ((price_type) == SmPriceType::Market)
		orderString.append(_T("2"));
	// 체결 조건
	if ((filled_condition) == SmFilledCondition::Fas)
		orderString.append(_T("1"));
	else if ((filled_condition) == SmFilledCondition::Fok)
		orderString.append(_T("2"));
	else if ((filled_condition) == SmFilledCondition::Fak)
		orderString.append(_T("3"));
	else if ((filled_condition) == SmFilledCondition::Day)
		orderString.append(_T("0"));

	// 주문 가격 15
	temp = "               ";
	orderString.append(temp);

	// 정정 수량 10
	temp = "          ";
	orderString.append(temp);
	// 정정이나 취소시 원주문 번호
	temp = VtStringUtil::PadRight(ori_order_no, ' ', 10);
	orderString.append(temp);
	// 기타설정 26
	temp = "                          ";
	orderString.append(temp);

	std::string userDefined;
	std::string req_id = std::to_string(request_id);
	req_id = VtStringUtil::PadLeft(req_id, '0', 10);


	std::string mac_address;
	//mac_address = MainBeetle::GetMacAddress();
	userDefined.append(mac_address);
	userDefined.append("m");

	temp = VtStringUtil::PadRight(userDefined, '0', 60);

	orderString.append(temp);

	CString sTrCode = "g12003.AO0403%";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);

	_OrderReqMap[nRqID] = order_req;
}

void DarkHorse::ViClient::RegisterSymbol(task_arg&& arg)
{
	const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);
	CString strSymbolCode = symbol_code.c_str();
	if (isdigit(strSymbolCode.GetAt(2))) {
		int nRealType = 0;
		int nResult = 0;
		CString strKey = strSymbolCode;
		TCHAR first = strSymbolCode.GetAt(0);
		CString prefix = strSymbolCode.Left(3);
		if (first == '1' || first == '4') {
			if (prefix.Compare(_T("167")) == 0 || prefix.Compare(_T("175")) == 0) {
				nRealType = 58;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
				nRealType = 71;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			}
			else {
				nRealType = 51;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
				nRealType = 65;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			}
		}
		else if (first == '2' || first == '3') {
			nRealType = 52;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			nRealType = 66;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
		}
		else {
			nRealType = 82;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
		}
	}
	else {
		std::string code = static_cast<const char*>(strSymbolCode);
		std::string key = VtStringUtil::PadRight(code, ' ', 32);
		int nRealType = 76; // 시세
		m_CommAgent.CommSetBroad(key.c_str(), nRealType);
		nRealType = 82; // 호가
		m_CommAgent.CommSetBroad(key.c_str(), nRealType);
	}
}


void DarkHorse::ViClient::RegisterAccount(const std::string& account_no)
{
	const std::string user_id = mainApp.LoginMgr()->id();
	_RegAccountSet.insert(account_no);
	int nResult = m_CommAgent.CommSetJumunChe(user_id.c_str(), account_no.c_str());
}

void DarkHorse::ViClient::UnregisterSymbol(const std::string& symbol_code)
{
	CString strSymbolCode = symbol_code.c_str();
	if (isdigit(strSymbolCode.GetAt(2))) {
		int nRealType = 0;
		int nResult = 0;
		CString strKey = strSymbolCode;
		TCHAR first = strSymbolCode.GetAt(0);
		CString prefix = strSymbolCode.Left(3);
		if (first == '1' || first == '4') {
			if (prefix.Compare(_T("167")) == 0 || prefix.Compare(_T("175")) == 0) {
				nRealType = 58;
				nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
				nRealType = 71;
				nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
			}
			else {
				nRealType = 51;
				nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
				nRealType = 65;
				nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
			}
		}
		else if (first == '2' || first == '3') {
			nRealType = 52;
			nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
			nRealType = 66;
			nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
		}
		else {
			nRealType = 82;
			nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
		}
	}
	else {
		std::string code = static_cast<const char*>(strSymbolCode);
		std::string key = VtStringUtil::PadRight(code, ' ', 32);
		int nRealType = 76; // 시세
		m_CommAgent.CommRemoveBroad(key.c_str(), nRealType);
		nRealType = 82; // 호가
		m_CommAgent.CommRemoveBroad(key.c_str(), nRealType);
	}
}

void DarkHorse::ViClient::UnregisterAccount(const std::string& account_no)
{
	const std::string user_id = mainApp.LoginMgr()->id();
	int nResult = m_CommAgent.CommRemoveJumunChe(user_id.c_str(), account_no.c_str());
}

void ViClient::RegisterSymbol(SmTaskArg&& arg)
{
	if (arg.TaskType != SmTaskType::RegisterSymbol) return;

	auto req = std::any_cast<DarkHorse::DmRegisterReq>(arg.Param);

	CString sInput;
	const std::string symbol_code = req.symbol_code;
	_RegSymbolSet.insert(symbol_code);
	CString strSymbolCode = symbol_code.c_str();
	if (isdigit(strSymbolCode.GetAt(2))) {
		int nRealType = 0;
		int nResult = 0;
		CString strKey = strSymbolCode;
		TCHAR first = strSymbolCode.GetAt(0);
		CString prefix = strSymbolCode.Left(3);
		if (first == '1' || first == '4') {
			if (prefix.Compare(_T("167")) == 0 || prefix.Compare(_T("175")) == 0) {
				nRealType = 58;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
				nRealType = 71;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			}
			else {
				nRealType = 51;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
				nRealType = 65;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			}
		}
		else if (first == '2' || first == '3') {
			nRealType = 52;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			nRealType = 66;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
		}
		else {
			nRealType = 82;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
		}
	}
	else {
		std::string code = static_cast<const char*>(strSymbolCode);
		std::string key = VtStringUtil::PadRight(code, ' ', 32);
		int nRealType = 76; // 시세
		m_CommAgent.CommSetBroad(key.c_str(), nRealType);
		nRealType = 82; // 호가
		m_CommAgent.CommSetBroad(key.c_str(), nRealType);
	}
}

void DarkHorse::ViClient::UnregisterSymbol(task_arg&& arg)
{
	int nRealType = 0;
	int nResult = 0;
	const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);
	CString strKey = symbol_code.c_str();
	TCHAR first = strKey.GetAt(0);
	if (first == '1' || first == '4')
	{
		nRealType = 51;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
		nRealType = 65;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
	}
	else if (first == '2' || first == '3')
	{
		nRealType = 52;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
		nRealType = 66;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
	}
}

void DarkHorse::ViClient::RegisterAccount(task_arg&& arg)
{
	const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
	const std::string user_id = std::any_cast<std::string>(arg["user_id"]);
	int nResult = m_CommAgent.CommSetJumunChe(user_id.c_str(), account_no.c_str());
}

void DarkHorse::ViClient::UnregisterAccount(task_arg&& arg)
{
	const std::string account_no = std::any_cast<std::string>(arg["account_no"]);
	const std::string user_id = std::any_cast<std::string>(arg["user_id"]);
	int nResult = m_CommAgent.CommRemoveJumunChe(user_id.c_str(), account_no.c_str());
}

void DarkHorse::ViClient::RegisterSymbol(const std::string& symbol_code)
{
	CString strSymbolCode = symbol_code.c_str();
	if (isdigit(strSymbolCode.GetAt(2))) {
		int nRealType = 0;
		int nResult = 0;
		CString strKey = strSymbolCode;
		TCHAR first = strSymbolCode.GetAt(0);
		CString prefix = strSymbolCode.Left(3);
		if (first == '1' || first == '4') {
			if (prefix.Compare(_T("167")) == 0 || prefix.Compare(_T("175")) == 0) {
				nRealType = 58;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
				nRealType = 71;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			}
			else {
				nRealType = 51;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
				nRealType = 65;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);

				nRealType = 77;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
				nRealType = 75;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			}
		}
		else if (first == '2' || first == '3') {
			nRealType = 52;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			nRealType = 66;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);

			nRealType = 79;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			nRealType = 78;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
		}
		else {
			nRealType = 82;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
		}
		// Expected.
		nRealType = 310;
		nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
	}
	else {
		std::string code = static_cast<const char*>(strSymbolCode);
		std::string key = VtStringUtil::PadRight(code, ' ', 32);
		int nRealType = 76; // 시세
		m_CommAgent.CommSetBroad(key.c_str(), nRealType);
		nRealType = 82; // 호가
		m_CommAgent.CommSetBroad(key.c_str(), nRealType);
	}
}

int DarkHorse::ViClient::GetChartDataDomestic(task_arg&& arg)
{
	try {
		const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);
		const int chart_type = std::any_cast<int>(arg["chart_type"]);
		const int cycle = std::any_cast<int>(arg["cycle"]);
		const int count = std::any_cast<int>(arg["count"]);
		const int next = std::any_cast<int>(arg["next"]);
		const int prev = 0;

		std::string temp;
		std::string reqString;

		temp = VtStringUtil::PadRight(symbol_code, ' ', 15);
		reqString.append(temp);

		std::string str = VtStringUtil::getCurentDate();
		reqString.append(str);

		reqString.append(_T("999999"));

		temp = VtStringUtil::PadLeft(count, '0', 4);
		reqString.append(temp);

		temp = VtStringUtil::PadLeft(cycle, '0', 3);
		reqString.append(temp);

		if (chart_type == SmChartType::TICK)
			reqString.append("0");
		else if (chart_type == SmChartType::MIN)
			reqString.append("1");
		else if (chart_type == SmChartType::DAY)
			reqString.append("2");
		else if (chart_type == SmChartType::WEEK)
			reqString.append("3");
		else if (chart_type == SmChartType::MON)
			reqString.append("4");
		else
			reqString.append("1");

		if (next == 0)
			reqString.append(_T("0"));
		else
			reqString.append(_T("1"));

		temp = VtStringUtil::PadRight(prev, ' ', 21);
		reqString.append(temp);

		reqString.append(_T("0"));
		reqString.append(_T("0"));
		reqString.append(_T("00"));
		reqString.append(_T("000000"));
		reqString.append(_T(" "));

		if (next == 0)
			reqString.append(_T("0"));
		else
			reqString.append(_T("1"));

		CString sInput = reqString.c_str();
		CString strNextKey = _T("");
		int nRqID = m_CommAgent.CommRqData(DefChartData, sInput, sInput.GetLength(), "");
		_ReqMap[nRqID] = arg;
		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetChartDataDomestic(SmTaskArg&& arg)
{
	if (arg.TaskType != SmTaskType::ChartData) return -1;

	try {
		auto req = std::any_cast<SmChartDataReq>(arg.Param);

		std::string temp;
		std::string reqString;

		temp = VtStringUtil::PadRight(req.SymbolCode, ' ', 15);
		reqString.append(temp);

		std::string str = VtStringUtil::getCurentDate();
		reqString.append(str);

		reqString.append(_T("999999"));

		temp = VtStringUtil::PadLeft(req.Count, '0', 4);
		reqString.append(temp);

		temp = VtStringUtil::PadLeft(req.Cycle, '0', 3);
		reqString.append(temp);

		if (req.ChartType == SmChartType::TICK)
			reqString.append("0");
		else if (req.ChartType == SmChartType::MIN)
			reqString.append("1");
		else if (req.ChartType == SmChartType::DAY)
			reqString.append("2");
		else if (req.ChartType == SmChartType::WEEK)
			reqString.append("3");
		else if (req.ChartType == SmChartType::MON)
			reqString.append("4");
		else
			reqString.append("1");

		if (req.Next == 0)
			reqString.append(_T("0"));
		else
			reqString.append(_T("1"));

		temp = VtStringUtil::PadRight(req.PrevKey, ' ', 21);
		reqString.append(temp);

		reqString.append(_T("0"));
		reqString.append(_T("0"));
		reqString.append(_T("00"));
		reqString.append(_T("000000"));
		reqString.append(_T(" "));

		if (req.Next == 0)
			reqString.append(_T("0"));
		else
			reqString.append(_T("1"));

		CString sInput = reqString.c_str();
		CString strNextKey = _T("");
		int nRqID = m_CommAgent.CommRqData(DefChartData, sInput, sInput.GetLength(), "");
		_ChartReqMap[nRqID] = req;
		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetChartDataAbroadShort(SmTaskArg&& arg)
{
	if (arg.TaskType != SmTaskType::ChartData) return -1;

	try {
		auto req = std::any_cast<SmChartDataReq>(arg.Param);

		std::string temp;
		std::string reqString;
		// 종목 코드 32 자리
		temp = VtStringUtil::PadRight(req.SymbolCode, ' ', 32);
		reqString.append(temp);

		std::string str = VtStringUtil::getCurentDate();
		CString msg;
		//msg.Format("%s \n", str.c_str());
		//TRACE(msg);
		reqString.append(str);
		reqString.append(str);
		reqString.append(_T("9999999999"));

		if (req.Next == 0)
			reqString.append(_T("0"));
		else
			reqString.append(_T("1"));

		if (req.ChartType == SmChartType::TICK)
			reqString.append("1");
		else if (req.ChartType == SmChartType::MIN)
			reqString.append("2");
		else if (req.ChartType == SmChartType::DAY)
			reqString.append("3");
		else if (req.ChartType == SmChartType::WEEK)
			reqString.append("4");
		else if (req.ChartType == SmChartType::MON)
			reqString.append("5");
		else
			reqString.append("2");

		temp = VtStringUtil::PadLeft(req.Cycle, '0', 2);
		reqString.append(temp);

		temp = VtStringUtil::PadLeft(req.Count, '0', 5);
		reqString.append(temp);

		CString sInput = reqString.c_str();
		CString strNextKey = _T("");

		CString sReqFidInput = "000001002003004005006007008009010011012013014015";
		int nRqID = m_CommAgent.CommFIDRqData(DefAbChartData, sInput, sReqFidInput, sInput.GetLength(), strNextKey);
		_ChartReqMap[nRqID] = req;
		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetChartDataAbroadLong(SmTaskArg&& arg)
{
	if (arg.TaskType != SmTaskType::ChartData) return -1;
	try {
		auto req = std::any_cast<SmChartDataReq>(arg.Param);

		std::string temp;
		std::string reqString;
		// 최초 요청시 18자리 공백
		reqString.append("                  ");

		temp = VtStringUtil::PadRight(req.SymbolCode, ' ', 32);
		reqString.append(temp);

		time_t rawtime = 0;
		struct tm timeinfo;
		char buf[100] = { 0 };

		localtime_s(&timeinfo, &rawtime);

		strftime(buf, sizeof(buf), "%Y%m%d-%H:%M:%S", &timeinfo);
		std::string str(buf);
		CString msg;
		msg.Format("%s \n", str.c_str());
		//TRACE(msg);
		//reqString.append(curDate);
		//reqString.append(curDate);
		//reqString.append(_T("99999999"));
		reqString.append(_T("99999999"));
		reqString.append(_T("9999999999"));

		if (req.Next == 0)
			reqString.append(_T("0"));
		else
			reqString.append(_T("1"));

		if (req.ChartType == SmChartType::TICK)
			reqString.append("6");
		else if (req.ChartType == SmChartType::MIN)
			reqString.append("2");
		else if (req.ChartType == SmChartType::DAY)
			reqString.append("3");
		else if (req.ChartType == SmChartType::WEEK)
			reqString.append("4");
		else if (req.ChartType == SmChartType::MON)
			reqString.append("5");
		else
			reqString.append("2");

		temp = VtStringUtil::PadLeft(req.Cycle, '0', 3);
		reqString.append(temp);

		temp = VtStringUtil::PadLeft(req.Count, '0', 5);
		reqString.append(temp);

		reqString.append(_T("1"));
		reqString.append(_T("1"));


		CString sInput = reqString.c_str();
		CString strNextKey = _T("");

		CString sReqFidInput = "000001002003004005006007008009010011012013014015";

		int nRqID = m_CommAgent.CommRqData(DefAbsChartData2, sInput, sInput.GetLength(), strNextKey);
		_ChartReqMap[nRqID] = req;
		return nRqID;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return -1;
}

int DarkHorse::ViClient::GetChartDataAbroadShort(task_arg&& arg)
{
	const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);
	const int chart_type = std::any_cast<int>(arg["chart_type"]);
	const int cycle = std::any_cast<int>(arg["cycle"]);
	const int count = std::any_cast<int>(arg["count"]);
	const int next = std::any_cast<int>(arg["next"]);
	std::string temp;
	std::string reqString;
	// 종목 코드 32 자리
	temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
	reqString.append(temp);

	std::string str = VtStringUtil::getCurentDate();
	CString msg;
	//msg.Format("%s \n", str.c_str());
	//TRACE(msg);
	reqString.append(str);
	reqString.append(str);
	reqString.append(_T("9999999999"));

	if (next == 0)
		reqString.append(_T("0"));
	else
		reqString.append(_T("1"));

	if (chart_type == SmChartType::TICK)
		reqString.append("1");
	else if (chart_type == SmChartType::MIN)
		reqString.append("2");
	else if (chart_type == SmChartType::DAY)
		reqString.append("3");
	else if (chart_type == SmChartType::WEEK)
		reqString.append("4");
	else if (chart_type == SmChartType::MON)
		reqString.append("5");
	else
		reqString.append("2");

	temp = VtStringUtil::PadLeft(cycle, '0', 2);
	reqString.append(temp);

	temp = VtStringUtil::PadLeft(count, '0', 5);
	reqString.append(temp);

	CString sInput = reqString.c_str();
	CString strNextKey = _T("");

	CString sReqFidInput = "000001002003004005006007008009010011012013014015";
	int nRqID = m_CommAgent.CommFIDRqData(DefAbChartData, sInput, sReqFidInput, sInput.GetLength(), strNextKey);
	_ReqMap[nRqID] = arg;
	return nRqID;
}

int DarkHorse::ViClient::GetChartDataAbroadLong(task_arg&& arg)
{
	const std::string symbol_code = std::any_cast<std::string>(arg["symbol_code"]);
	const int chart_type = std::any_cast<int>(arg["chart_type"]);
	const int cycle = std::any_cast<int>(arg["cycle"]);
	const int count = std::any_cast<int>(arg["count"]);
	const int next = std::any_cast<int>(arg["next"]);

	std::string temp;
	std::string reqString;
	// 최초 요청시 18자리 공백
	reqString.append("                  ");

	temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
	reqString.append(temp);

	time_t rawtime = 0;
	struct tm timeinfo;
	char buf[100] = { 0 };

	localtime_s(&timeinfo, &rawtime);

	strftime(buf, sizeof(buf), "%Y%m%d-%H:%M:%S", &timeinfo);
	std::string str(buf);
	CString msg;
	msg.Format("%s \n", str.c_str());
	//TRACE(msg);
	//reqString.append(curDate);
	//reqString.append(curDate);
	//reqString.append(_T("99999999"));
	reqString.append(_T("99999999"));
	reqString.append(_T("9999999999"));

	if (next == 0)
		reqString.append(_T("0"));
	else
		reqString.append(_T("1"));

	if (chart_type == SmChartType::TICK)
		reqString.append("6");
	else if (chart_type == SmChartType::MIN)
		reqString.append("2");
	else if (chart_type == SmChartType::DAY)
		reqString.append("3");
	else if (chart_type == SmChartType::WEEK)
		reqString.append("4");
	else if (chart_type == SmChartType::MON)
		reqString.append("5");
	else
		reqString.append("2");

	temp = VtStringUtil::PadLeft(cycle, '0', 3);
	reqString.append(temp);

	temp = VtStringUtil::PadLeft(count, '0', 5);
	reqString.append(temp);

	reqString.append(_T("1"));
	reqString.append(_T("1"));


	CString sInput = reqString.c_str();
	CString strNextKey = _T("");

	CString sReqFidInput = "000001002003004005006007008009010011012013014015";

	int nRqID = m_CommAgent.CommRqData(DefAbsChartData2, sInput, sInput.GetLength(), strNextKey);
	_ReqMap[nRqID] = arg;
	return nRqID;
}



void DarkHorse::ViClient::OnTaskComplete(const int& nRqId)
{
	auto it = _ReqMap.find(nRqId);
	if (it != _ReqMap.end()) {
		const int task_id = std::any_cast<int>(it->second["task_id"]);
		mainApp.SvrDataRcvr()->OnTaskComplete(task_id);
		_ReqMap.erase(it);
	}
}

void DarkHorse::ViClient::OnChartTaskComplete(const int& nRqId)
{
	auto it = _ChartReqMap.find(nRqId);
	if (it != _ChartReqMap.end()) {
		
		_ChartReqMap.erase(it);
	}
}

void DarkHorse::ViClient::OnDomesticSymbolCode(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++) {
		CString sData = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
		TRACE(sData);
		TRACE(_T("\n"));
		//WriteLog(sData);

		std::string symbol_code = (const char*)sData.Trim();
		std::string product_code = symbol_code.substr(0, 3);

		std::shared_ptr<SmProduct> product = mainApp.SymMgr()->FindProduct(product_code);
		if (!product) continue;

		std::shared_ptr<SmSymbol> symbol = product->AddSymbol(std::move(symbol_code));
		if (symbol) {
			symbol->MarketName(product->MarketName());
			symbol->ProductCode(product_code);
			symbol->Decimal(2);
			symbol->SymbolNameKr("코스피200");
			symbol->SymbolNameEn("Kospi200");
		}
	}

	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnAccountListReceived(const CString& sTrCode, const LONG& nRqID) 
{
	// 계좌 구분 추가. - 20140331 sivas
	typedef	struct
	{
		char 	szAcctNo[11];		// 계좌번호
		char	szAcctNm[30];		// 계좌명
		char	szAcctGb[01];		// 계좌구분  '1': 해외, '2': FX, '9':국내
	}HDF_ACCOUNT_UNIT;

	typedef struct
	{
		char szCount[5];
		HDF_ACCOUNT_UNIT* pHdfAccUnit;
	}HDF_ACCOUNT_INFO;

	HDF_ACCOUNT_INFO* pHdfAccInfo = NULL;
	HDF_ACCOUNT_UNIT* pHdfAccUnit = NULL;
	CString strRcvBuff = m_CommAgent.CommGetAccInfo();

	pHdfAccInfo = (HDF_ACCOUNT_INFO*)strRcvBuff.GetBuffer();
	CString strCount(pHdfAccInfo->szCount, sizeof(pHdfAccInfo->szCount));
	for (int i = 0; i < atoi(strCount); i++) {
		pHdfAccUnit = (HDF_ACCOUNT_UNIT*)(pHdfAccInfo->szCount + sizeof(pHdfAccInfo->szCount) + (sizeof(HDF_ACCOUNT_UNIT) * i));
		CString strAcctNo(pHdfAccUnit->szAcctNo, sizeof(pHdfAccUnit->szAcctNo));
		CString strAcctNm(pHdfAccUnit->szAcctNm, sizeof(pHdfAccUnit->szAcctNm));
		CString strAcctGb(pHdfAccUnit->szAcctGb, sizeof(pHdfAccUnit->szAcctGb));// 계좌 구분 추가. - 20140331 sivas
			
		nlohmann::json account_info;
		account_info["account_no"] = static_cast<const char*>(strAcctNo.Trim());
		account_info["account_name"] = static_cast<const char*>(strAcctNm.Trim());
		account_info["account_type"] = static_cast<const char*>(strAcctGb.Trim());
		if (auto wp = _Client.lock()) {
			wp->OnAccountReceived(std::move(account_info));
		}
	}

	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnDmSymbolMaster(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	CString	strSymbolCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "단축코드");
	CString	strFullCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
	CString	strSymbolNameKr = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "한글종목명");
	CString strDeltaDay = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일대비");
	CString strUpdownRate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "등락률");

	CString	strFilledTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "체결시간");
	CString	strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "현재가");
	CString	strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "시가");
	CString	strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "고가");
	CString	strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "저가");
	CString strPreClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일종가");
	CString strPreHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일고가");
	CString strPreLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일저가");

	nlohmann::json quote;

	quote["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	quote["symbol_name_kr"] = static_cast<const char*>(strSymbolNameKr.Trim());
	quote["delta_day"] = static_cast<const char*>(strDeltaDay.Trim());
	//quote["delta_day_sign"] = static_cast<const char*>(strDeltaDaySign.Trim());
	quote["updown_rate"] = static_cast<const char*>(strUpdownRate.Trim());
	quote["time"] = static_cast<const char*>(strFilledTime.Trim());
	quote["close"] = (_ttoi(strClose.Trim()));
	quote["open"] = (_ttoi(strOpen.Trim()));
	quote["high"] = (_ttoi(strHigh.Trim()));
	quote["low"] = (_ttoi(strLow.Trim()));
	quote["pre_day_close"] = (_ttoi(strPreClose.Trim()));
	quote["cumulative_amount"] = 0;
	quote["volume"] = 0;
	quote["up_down"] = 1;
	quote["preday_volume"] = 0;

	if (auto wp = _Client.lock()) {
		const std::string symbol_code = static_cast<const char*>(strSymbolCode.Trim());
		wp->OnDmSymbolQuote(std::move(quote));
		wp->OnDmSymbolMaster(symbol_code);
	}

	CString	strHogaTime   = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "호가수신시간");
	CString	strSellPrice1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가1");
	CString	strBuyPrice1  = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가1");
	CString	strSellQty1   = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량1");
	CString	strBuyQty1    = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량1");
	CString	strSellCnt1   = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수1");
	CString	strBuyCnt1    = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수1");

	CString	strSellPrice2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가2");
	CString	strBuyPrice2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가2");
	CString	strSellQty2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량2");
	CString	strBuyQty2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량2");
	CString	strSellCnt2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수2");
	CString	strBuyCnt2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수2");

	CString	strSellPrice3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가3");
	CString	strBuyPrice3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가3");
	CString	strSellQty3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량3");
	CString	strBuyQty3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량3");
	CString	strSellCnt3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수3");
	CString	strBuyCnt3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수3");

	CString	strSellPrice4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가4");
	CString	strBuyPrice4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가4");
	CString	strSellQty4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량4");
	CString	strBuyQty4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량4");
	CString	strSellCnt4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수4");
	CString	strBuyCnt4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수4");

	CString	strSellPrice5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가5");
	CString	strBuyPrice5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가5");
	CString	strSellQty5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량5");
	CString	strBuyQty5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량5");
	CString	strSellCnt5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수5");
	CString	strBuyCnt5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수5");

	CString	strTotSellQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가총수량");
	CString	strTotBuyQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가총수량");
	CString	strTotSellCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가총건수");
	CString	strTotBuyCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가총건수");

	

	nlohmann::json hoga;
	hoga["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	hoga["hoga_time"] = static_cast<const char*>(strHogaTime.Trim());
	hoga["tot_buy_qty"] = _ttoi(strTotBuyQty.Trim());
	hoga["tot_sell_qty"] = _ttoi(strTotSellQty.Trim());
	hoga["tot_buy_cnt"] = _ttoi(strTotBuyCnt.Trim());
	hoga["tot_sell_cnt"] = _ttoi(strTotSellCnt.Trim());

	hoga["hoga_items"][0]["sell_price"] = _ttoi(strSellPrice1.Trim());
	hoga["hoga_items"][0]["buy_price"] = _ttoi(strBuyPrice1.Trim());
	hoga["hoga_items"][0]["sell_qty"] = _ttoi(strSellQty1.Trim());
	hoga["hoga_items"][0]["buy_qty"] = _ttoi(strBuyQty1.Trim());
	hoga["hoga_items"][0]["sell_cnt"] = _ttoi(strSellCnt1.Trim());
	hoga["hoga_items"][0]["buy_cnt"] = _ttoi(strBuyCnt1.Trim());

	hoga["hoga_items"][1]["sell_price"] = _ttoi(strSellPrice2.Trim());
	hoga["hoga_items"][1]["buy_price"] = _ttoi(strBuyPrice2.Trim());
	hoga["hoga_items"][1]["sell_qty"] = _ttoi(strSellQty2.Trim());
	hoga["hoga_items"][1]["buy_qty"] = _ttoi(strBuyQty2.Trim());
	hoga["hoga_items"][1]["sell_cnt"] = _ttoi(strSellCnt2.Trim());
	hoga["hoga_items"][1]["buy_cnt"] = _ttoi(strBuyCnt2.Trim());

	hoga["hoga_items"][2]["sell_price"] = _ttoi(strSellPrice3.Trim());
	hoga["hoga_items"][2]["buy_price"] = _ttoi(strBuyPrice3.Trim());
	hoga["hoga_items"][2]["sell_qty"] = _ttoi(strSellQty3.Trim());
	hoga["hoga_items"][2]["buy_qty"] = _ttoi(strBuyQty3.Trim());
	hoga["hoga_items"][2]["sell_cnt"] = _ttoi(strSellCnt3.Trim());
	hoga["hoga_items"][2]["buy_cnt"] = _ttoi(strBuyCnt3.Trim());

	hoga["hoga_items"][3]["sell_price"] = _ttoi(strSellPrice4.Trim());
	hoga["hoga_items"][3]["buy_price"] = _ttoi(strBuyPrice4.Trim());
	hoga["hoga_items"][3]["sell_qty"] = _ttoi(strSellQty4.Trim());
	hoga["hoga_items"][3]["buy_qty"] = _ttoi(strBuyQty4.Trim());
	hoga["hoga_items"][3]["sell_cnt"] = _ttoi(strSellCnt4.Trim());
	hoga["hoga_items"][3]["buy_cnt"] = _ttoi(strBuyCnt4.Trim());

	hoga["hoga_items"][4]["sell_price"] = _ttoi(strSellPrice5.Trim());
	hoga["hoga_items"][4]["buy_price"] = _ttoi(strBuyPrice5.Trim());
	hoga["hoga_items"][4]["sell_qty"] = _ttoi(strSellQty5.Trim());
	hoga["hoga_items"][4]["buy_qty"] = _ttoi(strBuyQty5.Trim());
	hoga["hoga_items"][4]["sell_cnt"] = _ttoi(strSellCnt5.Trim());
	hoga["hoga_items"][4]["buy_cnt"] = _ttoi(strBuyCnt5.Trim());

	if (auto wp = _Client.lock()) {
		wp->OnDmSymbolHoga(std::move(hoga));
	}
}

void DarkHorse::ViClient::OnSymbolMaster(const CString& sTrCode, const LONG& nRqID)
{
	CString m_strBusinessDay = m_CommAgent.CommGetBusinessDay(1);
	

	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strSymbolCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");

		CString msg;

		

		CString strLastTradeDay = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "최종거래일");
		CString strExchange = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "거래소");
		CString strExpireDay = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "만기일");
		CString strPriceTag = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "가격표시");
		CString strStartTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "장시작시간(CME)");
		CString strEndTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "장종료시간(CME)");
		CString strLocalStartTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "장시작시간(한국)");
		CString strLocalEndTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "장종료시간(한국)");
		CString strCurrency = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "거래통화");
		CString strProduct = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "상품구분");
		CString strTickSize = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "ticksize");
		CString strTickValue = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "tickvalue");
		CString strNeedMoney = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "증거금");
		CString strContractUnit = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계약단위");
		CString strRemainMoney = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "유지증거금");

		msg.Format("SymbolMaster strSymbolCode = %s, strLocalStartTime = %s, %s\n", strSymbolCode, strLocalStartTime, strLocalEndTime);
		TRACE(msg);

	
		nlohmann::json symbol_master;
		symbol_master["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
		symbol_master["last_trade_day"] = static_cast<const char*>(strLastTradeDay.Trim());
		symbol_master["exchange"] = static_cast<const char*>(strExchange.Trim());
		symbol_master["expire_day"] = static_cast<const char*>(strExpireDay.Trim());
		symbol_master["price_tag"] = static_cast<const char*>(strPriceTag.Trim());
		symbol_master["start_time"] = static_cast<const char*>(strStartTime.Trim());
		symbol_master["end_time"] = static_cast<const char*>(strEndTime.Trim());
		symbol_master["local_start_time"] = static_cast<const char*>(strLocalStartTime.Trim());
		symbol_master["local_end_time"] = static_cast<const char*>(strLocalEndTime.Trim());
		symbol_master["currency"] = static_cast<const char*>(strCurrency.Trim());
		symbol_master["product_code"] = static_cast<const char*>(strProduct.Trim());
		symbol_master["tick_size"] = static_cast<const char*>(strTickSize.Trim());
		symbol_master["tick_value"] = static_cast<const char*>(strTickValue.Trim());
		symbol_master["deposit"] = static_cast<const char*>(strNeedMoney.Trim());
		symbol_master["contract_unit"] = static_cast<const char*>(strContractUnit.Trim());
		symbol_master["maintenance_margin"] = static_cast<const char*>(strRemainMoney.Trim());
		if (auto wp = _Client.lock()) {
			wp->OnSymbolMaster(std::move(symbol_master));
		}
	}

	OnTaskComplete(nRqID);
}

void ViClient::OnMasterFile(const CString& sTrCode, const LONG& nRqID)
{
	auto it = _ReqMap.find(nRqID);
	if (it == _ReqMap.end()) return;
	const std::string file_name = std::any_cast<string>(it->second["file_name"]);

	long nFileSize = atol(m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "파일크기"));
	CString strFileNm = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "파일명");
	CString strProcCd = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "응답코드");

	if (strProcCd == "REOK")
	{
		CString strBuff = m_CommAgent.CommGetDataDirect(sTrCode, -1, 128 + 4 + 8, nFileSize, 0, "A");

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(_T("mst"));
		appPath.append(_T("\\"));
		//TRACE(file_name.c_str());
		std::string file_path = appPath + file_name;

		CString strCommonFileName = file_path.c_str();

		CFile commonfile;
		if (!commonfile.Open(strCommonFileName, CFile::modeWrite /*| CFile::typeBinary*/))
		{
			if (commonfile.Open(strCommonFileName, CFile::modeCreate | CFile::modeWrite /*| CFile::typeBinary*/) == FALSE)
			{
				CString strMsg;
				strMsg.Format("%s화일 생성에 실패하였습니다. ", strCommonFileName);
				return;
			}
		}

		commonfile.Write(strBuff, nFileSize);
		commonfile.Close();
	}
}

void DarkHorse::ViClient::OnSymbolQuote(const CString& sTrCode, const LONG& nRqID)
{
	CString	strSymbolCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
	CString	strSymbolNameKr = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "한글종목명");
	CString strDeltaDay = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일대비");
	CString strDeltaDaySign = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일대비구분");
	CString strUpdownRate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일대비등락율");
	CString	strFilledTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "체결시간");
	CString	strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "현재가");
	CString	strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "시가");
	CString	strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "고가");
	CString	strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "저가");
	CString	strCumulativeAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "누적거래량");
	CString strPreDayVolume = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일거래량");

	CString msg;

	msg.Format("OnSymbolQuote strSymbolCode = %s, quote = %s, preday_volume = %s\n", strSymbolCode, strClose, strPreDayVolume);
	TRACE(msg);

	nlohmann::json quote;

	quote["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	quote["symbol_name_kr"] = static_cast<const char*>(strSymbolNameKr.Trim());
	quote["delta_day"] = static_cast<const char*>(strDeltaDay.Trim());
	quote["delta_day_sign"] = static_cast<const char*>(strDeltaDaySign.Trim());
	quote["updown_rate"] = static_cast<const char*>(strUpdownRate.Trim());
	quote["time"] = static_cast<const char*>(strFilledTime.Trim());
	quote["close"] = (_ttoi(strClose.Trim()));
	quote["open"] = (_ttoi(strOpen.Trim()));
	quote["high"] = (_ttoi(strHigh.Trim()));
	quote["low"] = (_ttoi(strLow.Trim()));
	quote["cumulative_amount"] = (_ttoi(strCumulativeAmount.Trim()));
	quote["volume"] = 0;
	quote["up_down"] = 1;
	quote["preday_volume"] = _ttoi(strPreDayVolume.Trim());

	if (auto wp = _Client.lock()) {
		wp->OnSymbolQuote(std::move(quote));
	}
	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnSymbolHoga(const CString& sTrCode, const LONG& nRqID)
{
	CString	strSymbolCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");


	CString msg;

	msg.Format("OnSymbolHoga strSymbolCode = %s\n", strSymbolCode);
	TRACE(msg);

	CString	strHogaTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "호가시간");

	CString	strSellPrice1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가1");
	CString	strBuyPrice1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가1");
	CString	strSellQty1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가잔량1");
	CString	strBuyQty1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가잔량1");
	CString	strSellCnt1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수1");
	CString	strBuyCnt1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수1");

	CString	strSellPrice2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가2");
	CString	strBuyPrice2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가2");
	CString	strSellQty2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가잔량2");
	CString	strBuyQty2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가잔량2");
	CString	strSellCnt2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수2");
	CString	strBuyCnt2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수2");

	CString	strSellPrice3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가3");
	CString	strBuyPrice3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가3");
	CString	strSellQty3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가잔량3");
	CString	strBuyQty3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가잔량3");
	CString	strSellCnt3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수3");
	CString	strBuyCnt3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수3");

	CString	strSellPrice4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가4");
	CString	strBuyPrice4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가4");
	CString	strSellQty4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가잔량4");
	CString	strBuyQty4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가잔량4");
	CString	strSellCnt4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수4");
	CString	strBuyCnt4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수4");

	CString	strSellPrice5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가5");
	CString	strBuyPrice5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가5");
	CString	strSellQty5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가잔량5");
	CString	strBuyQty5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가잔량5");
	CString	strSellCnt5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수5");
	CString	strBuyCnt5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수5");
	

	CString	strTotSellQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가총잔량");
	CString	strTotBuyQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가총잔량");
	CString	strTotSellCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가총건수");
	CString	strTotBuyCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가총건수");

	nlohmann::json hoga;
	hoga["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	hoga["hoga_time"] = static_cast<const char*>(strHogaTime.Trim());
	hoga["tot_buy_qty"] = _ttoi(strTotBuyQty.Trim());
	hoga["tot_sell_qty"] = _ttoi(strTotSellQty.Trim());
	hoga["tot_buy_cnt"] = _ttoi(strTotBuyCnt.Trim());
	hoga["tot_sell_cnt"] = _ttoi(strTotSellCnt.Trim());

	hoga["hoga_items"][0]["sell_price"] = _ttoi(strSellPrice1.Trim());
	hoga["hoga_items"][0]["buy_price"] = _ttoi(strBuyPrice1.Trim());
	hoga["hoga_items"][0]["sell_qty"] = _ttoi(strSellQty1.Trim());
	hoga["hoga_items"][0]["buy_qty"] = _ttoi(strBuyQty1.Trim());
	hoga["hoga_items"][0]["sell_cnt"] = _ttoi(strSellCnt1.Trim());
	hoga["hoga_items"][0]["buy_cnt"] = _ttoi(strBuyCnt1.Trim());

	hoga["hoga_items"][1]["sell_price"] = _ttoi(strSellPrice2.Trim());
	hoga["hoga_items"][1]["buy_price"] = _ttoi(strBuyPrice2.Trim());
	hoga["hoga_items"][1]["sell_qty"] = _ttoi(strSellQty2.Trim());
	hoga["hoga_items"][1]["buy_qty"] = _ttoi(strBuyQty2.Trim());
	hoga["hoga_items"][1]["sell_cnt"] = _ttoi(strSellCnt2.Trim());
	hoga["hoga_items"][1]["buy_cnt"] = _ttoi(strBuyCnt2.Trim());

	hoga["hoga_items"][2]["sell_price"] = _ttoi(strSellPrice3.Trim());
	hoga["hoga_items"][2]["buy_price"] = _ttoi(strBuyPrice3.Trim());
	hoga["hoga_items"][2]["sell_qty"] = _ttoi(strSellQty3.Trim());
	hoga["hoga_items"][2]["buy_qty"] = _ttoi(strBuyQty3.Trim());
	hoga["hoga_items"][2]["sell_cnt"] = _ttoi(strSellCnt3.Trim());
	hoga["hoga_items"][2]["buy_cnt"] = _ttoi(strBuyCnt3.Trim());

	hoga["hoga_items"][3]["sell_price"] = _ttoi(strSellPrice4.Trim());
	hoga["hoga_items"][3]["buy_price"] = _ttoi(strBuyPrice4.Trim());
	hoga["hoga_items"][3]["sell_qty"] = _ttoi(strSellQty4.Trim());
	hoga["hoga_items"][3]["buy_qty"] = _ttoi(strBuyQty4.Trim());
	hoga["hoga_items"][3]["sell_cnt"] = _ttoi(strSellCnt4.Trim());
	hoga["hoga_items"][3]["buy_cnt"] = _ttoi(strBuyCnt4.Trim());

	hoga["hoga_items"][4]["sell_price"] = _ttoi(strSellPrice5.Trim());
	hoga["hoga_items"][4]["buy_price"] = _ttoi(strBuyPrice5.Trim());
	hoga["hoga_items"][4]["sell_qty"] = _ttoi(strSellQty5.Trim());
	hoga["hoga_items"][4]["buy_qty"] = _ttoi(strBuyQty5.Trim());
	hoga["hoga_items"][4]["sell_cnt"] = _ttoi(strSellCnt5.Trim());
	hoga["hoga_items"][4]["buy_cnt"] = _ttoi(strBuyCnt5.Trim());

	if (strSymbolCode.Compare("HSIQ21") == 0) {
		int i = 0;
		i = i + 1;
	}

	if (auto wp = _Client.lock()) {
		wp->OnSymbolHoga(std::move(hoga));
	}

	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnAccountAsset(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strCurrency = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "통화구분");
		CString strEntrustTotal = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁금총액");
		strEntrustTotal.Trim();
		if (strEntrustTotal.Compare("0") == 0) {
			continue;
		}

		CString strBalance = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁금잔액");
		CString strUnsettledMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "미결제증거금");
		CString strOrderMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문증거금");
		CString strEntrustMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "위탁증거금");
		CString strMaintenaceMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "유지증거금");
		CString strSettledProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "청산손익");
		CString strFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "선물옵션수수료");
		CString strOpenProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가손익");
		CString strOpenTrustTotal = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가예탁총액");
		CString strAdditionalMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "추가증거금");
		CString strOrderableAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가능금액");

		std::string account_no;
		auto found = _ReqMap.find(nRqID);
		if (found != _ReqMap.end()) {
			account_no = std::any_cast<std::string>(found->second["account_no"]);
		}


		CString msg;

		msg.Format("OnAccountAsset strAccountNo = %s\n", account_no.c_str());
		//TRACE(msg);

		nlohmann::json account_asset;
		account_asset["account_no"] = account_no;
		account_asset["currency"] = static_cast<const char*>(strCurrency.Trim());
		account_asset["entrust_total"] = _ttof(strEntrustTotal.Trim());
		account_asset["balance"] = _ttof(strBalance.Trim());
		account_asset["outstanding_deposit"] = _ttof(strUnsettledMargin.Trim());
		account_asset["order_deposit"] = _ttof(strOrderMargin.Trim());
		account_asset["entrust_deposit"] = _ttof(strEntrustMargin.Trim());
		account_asset["maintenance_margin"] = _ttof(strMaintenaceMargin.Trim());
		account_asset["settled_profit_loss"] = _ttof(strSettledProfitLoss.Trim());
		account_asset["fee"] = _ttof(strFee.Trim());
		account_asset["open_profit_loss"] = _ttof(strOpenProfitLoss.Trim());
		account_asset["open_trust_total"] = _ttof(strOpenTrustTotal.Trim());
		account_asset["additional_margin"] = _ttof(strAdditionalMargin.Trim());
		account_asset["order_margin"] = _ttof(strOrderableAmount.Trim());

		if (auto wp = _Client.lock()) {
			wp->OnAccountAsset(std::move(account_asset));
		}
	}

	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnDmAccountAsset(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strEntrustTotal = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁총액");
		strEntrustTotal.Trim();
		if (strEntrustTotal.Compare("0") == 0) {
			continue;
		}
		CString strAccountNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
		CString strOrderMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문증거금");
		CString strEntrustMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "위탁증거금");
		CString strFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "장중선물옵션수수료");
		CString strOpenProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가손익");
		CString strOpenTrustTotal = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가예탁총액");
		CString strOrderableAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가능금액");

		

		CString msg;
		nlohmann::json account_asset;
		account_asset["account_no"] = std::string(strAccountNo.Trim());
		account_asset["entrust_total"] = _ttof(strEntrustTotal.Trim());
		account_asset["order_deposit"] = _ttof(strOrderMargin.Trim());
		account_asset["entrust_deposit"] = _ttof(strEntrustMargin.Trim());
		account_asset["fee"] = _ttof(strFee.Trim());
		account_asset["open_profit_loss"] = _ttof(strOpenProfitLoss.Trim());
		account_asset["open_trust_total"] = _ttof(strOpenTrustTotal.Trim());
		account_asset["order_margin"] = _ttof(strOrderableAmount.Trim());

		if (auto wp = _Client.lock()) {
			wp->OnDmAccountAsset(std::move(account_asset));
		}
	}

	OnTaskComplete(nRqID);
}


void DarkHorse::ViClient::OnAccountProfitLoss(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++) {
		CString strAccountNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");


		CString msg;

		msg.Format("OnAccountAsset strAccountNo = %s\n", strAccountNo);
		TRACE(msg);

		CString strAccountName = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌명");
		CString strCurrency = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "통화코드");
		CString strSymbolCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목");
		CString strSymbolSettledProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "청산손익");
		CString strSymbolSettledPureProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "청산순손익");
		CString strSymbolFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "청산수수료");
		CString strSymbolOpenProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평가손익");
		CString strSymbolUnsettledFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "미결제수수료");
		CString strSymbolUnsettledPureProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "미결제순손익");

	
		nlohmann::json account_profit_loss;
		account_profit_loss["account_no"] = static_cast<const char*>(strAccountNo.Trim());
		account_profit_loss["account_name"] = static_cast<const char*>(strAccountName.Trim());
		account_profit_loss["currency"] = static_cast<const char*>(strCurrency.Trim());
		account_profit_loss["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
		account_profit_loss["trade_profit_loss"] = _ttof(strSymbolSettledProfitLoss.Trim());
		account_profit_loss["pure_trade_profit_loss"] = _ttof(strSymbolSettledPureProfitLoss.Trim());
		account_profit_loss["trade_fee"] = _ttof(strSymbolFee.Trim());
		account_profit_loss["open_profit_loss"] = _ttof(strSymbolOpenProfitLoss.Trim());
		account_profit_loss["unsettled_fee"] = _ttof(strSymbolUnsettledFee.Trim());
		account_profit_loss["pure_unsettled_profit_loss"] = _ttof(strSymbolUnsettledPureProfitLoss.Trim());
		

		if (auto wp = _Client.lock()) {
			wp->OnAccountProfitLoss(std::move(account_profit_loss));
		}
	}

	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnSymbolProfitLoss(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strAccountNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");

		CString msg;

		msg.Format("OnAccountAsset strAccountNo = %s\n", strAccountNo);
		TRACE(msg);

		CString strAccountName = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌명");
		CString strSymbolCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목");
		CString strSymbolPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매매구분");
		CString strSymbolPreOpenQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "전일미결제수량");
		CString strSymbolOpenQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "당일순 미결제수량");
		CString strSymbolAvgPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평균단가");
		CString strSymbolUnitPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평균단가(소수점반영)");
		CString strSymbolOpenProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평가손익");

		const int avg_price = ConvertToInt(strSymbolCode, strSymbolUnitPrice);
		const int unit_price = ConvertToInt(strSymbolCode, strSymbolUnitPrice);
		if (avg_price < 0) continue;

		int position = _ttoi(strSymbolPosition.Trim());

		nlohmann::json symbol_profit_loss;
		symbol_profit_loss["account_no"] = static_cast<const char*>(strAccountNo.Trim());
		symbol_profit_loss["account_name"] = static_cast<const char*>(strAccountName.Trim());
		symbol_profit_loss["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
		symbol_profit_loss["symbol_position"] = position == 1 ? 1 : -1;
		symbol_profit_loss["symbol_pre_open_qty"] = _ttoi(strSymbolPreOpenQty.Trim());
		symbol_profit_loss["symbol_open_qty"] = _ttoi(strSymbolOpenQty.Trim());
		symbol_profit_loss["symbol_avg_price"] = avg_price;
		symbol_profit_loss["symbol_unit_price"] = unit_price;
		symbol_profit_loss["symbol_open_profit_loss"] = _ttof(strSymbolOpenProfitLoss.Trim());

		if (auto wp = _Client.lock()) {
			wp->OnSymbolProfitLoss(std::move(symbol_profit_loss));
		}
	}

	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnAcceptedList(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strAccountNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");

		CString msg;

		msg.Format("OnAccountAsset strAccountNo = %s\n", strAccountNo);
		TRACE(msg);

		CString strOrderNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문번호");
		CString strSymbolCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
		CString strOrderPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문가격");
		CString strOrderAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문수량");
		CString strOrderPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매매구분");
		CString strPriceType = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "가격조건");
		CString strOriOrderNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "원주문번호");
		CString strFirstOrderNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "최초원주문번호");

		CString strOrderDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문일자");
		CString strOrderTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문시간");

		strOrderPrice.Trim();
		strSymbolCode.Trim();
		std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol((LPCTSTR)strSymbolCode);
		if (!symbol) continue;

		int whole_len = strOrderPrice.GetLength();
		int pos = strOrderPrice.Find(".");
		if (pos == -1) {
			strOrderPrice.Append(".");
			for (int i = 0; i < symbol->Decimal(); i++)
				strOrderPrice.Append("0");
		}
		else {
			int delta = (pos + symbol->Decimal() + 1) - whole_len;
			for (int i = 0; i < delta; i++)
				strOrderPrice.Append("0");
		}

		strOrderPrice.Remove('.');
		
		nlohmann::json order_info;
		order_info["account_no"] = static_cast<const char*>(strAccountNo.Trim());
		order_info["order_no"] = static_cast<const char*>(strOrderNo.Trim());
		order_info["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
		order_info["order_price"] = _ttoi(strOrderPrice.Trim());
		order_info["order_amount"] = _ttoi(strOrderAmount.Trim());
		order_info["order_position"] = static_cast<const char*>(strOrderPosition.Trim());
		order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
		order_info["ori_order_no"] = static_cast<const char*>(strOriOrderNo.Trim());
		order_info["first_order_no"] = static_cast<const char*>(strFirstOrderNo.Trim());
		order_info["order_date"] = static_cast<const char*>(strOrderDate.Trim());
		order_info["order_time"] = static_cast<const char*>(strOrderTime.Trim());
		order_info["order_type"] = "1"; // new 
		order_info["custom"] = "";

		if (auto wp = _Client.lock()) {
			wp->OnAcceptedList(std::move(order_info));
		}
	}

	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnFilledList(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = nRepeatCnt - 1; i >= 0; --i)
	{
		CString strAccountNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");


		CString msg;

		

		CString strOrderNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문번호");
		CString strSymbolCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목명");
		CString strOrderPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문가격");
		CString strOrderAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문수량");
		CString strOrderPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매매구분");
		CString strPriceType = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "가격조건");
		CString strOriOrderNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "원주문번호");
		CString strFirstOrderNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "최초원주문번호");

		CString strOrderDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문일자");
		CString strOrderTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문시간");

		CString strFilledPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "체결가격");
		CString strFilledAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "체결수량");

		CString strFilledDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "체결일자");
		CString strFilledTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "체결시간");

		strOrderPrice.Trim();
		strFilledPrice.Trim();
		strSymbolCode.Trim();
		strAccountNo.Trim();
		strOrderNo.Trim();
		strOrderAmount.Trim();
		strFilledAmount.Trim();

		msg.Format("OnAccountAsset:: strAccountNo = [%s] symbolcode = [%s], orderno = [%s], filldate = [%s], filltime = [%s], filledcount = [%s], orderprice = [%s], filledprice = [%s]\n", strAccountNo, strSymbolCode, strOrderNo, strFilledDate, strFilledTime, strFilledAmount, strOrderPrice,  strFilledPrice);
		//TRACE(msg);


		const int order_price = ConvertToInt(strSymbolCode, strOrderPrice);
		const int filled_price = ConvertToInt(strSymbolCode, strFilledPrice);
		if (filled_price < 0) continue;



		nlohmann::json order_info;
		order_info["account_no"] = static_cast<const char*>(strAccountNo.Trim());
		order_info["order_no"] = static_cast<const char*>(strOrderNo.Trim());
		order_info["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
		order_info["order_price"] = order_price;
		order_info["order_amount"] = _ttoi(strOrderAmount.Trim());
		order_info["order_position"] = static_cast<const char*>(strOrderPosition.Trim());
		order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
		order_info["ori_order_no"] = static_cast<const char*>(strOriOrderNo.Trim());
		order_info["first_order_no"] = static_cast<const char*>(strFirstOrderNo.Trim());
		order_info["order_date"] = static_cast<const char*>(strOrderDate.Trim());
		order_info["order_time"] = static_cast<const char*>(strOrderTime.Trim());

		order_info["filled_price"] = filled_price;
		order_info["filled_count"] = _ttoi(strFilledAmount.Trim());

		order_info["filled_date"] = static_cast<const char*>(strFilledDate.Trim());
		order_info["filled_time"] = static_cast<const char*>(strFilledTime.Trim());
		order_info["custom"] = "";

		if (auto wp = _Client.lock()) {
			wp->OnFilledList(std::move(order_info));
		}
	}

	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnChartDataLong(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");

	CString msg;
	auto it = _ChartReqMap.find(nRqID);
	if (it == _ChartReqMap.end()) {
		OnTaskComplete(nRqID);
		return;
	}
	const std::string symbol_code = it->second.SymbolCode;
	const int chart_type = it->second.ChartType;
	const int cycle = it->second.Cycle;
	std::vector<double> open, high, low, close, volume, chart_dt;
	std::vector<hmdf::DateTime> date_time;
	int milisecond_time = 0;
	int cur_time = 0;
	std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindAddChartData(symbol_code, static_cast<SmChartType>(chart_type), cycle);
	// 가장 최근것이 가장 먼저 온다. 따라서 가장 과거의 데이터를 먼저 가져온다.
	// Received the chart data first.
	for (int i = nRepeatCnt - 1; i >= 0; --i) {
		CString strCurDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "일자");
		CString strCurTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "시간");
		CString strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "국내일자");
		CString strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "국내시간");
		CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "시가");
		CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "고가");
		CString strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "저가");
		CString strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "종가");
		CString strVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "체결량");
		CString strCumulVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "누적거래량");

		msg.Format("OnChartData = index = %d, close = %d, %s, %s, %s\n", i, _ttoi(strClose), symbol_code.c_str(), strDate, strTime);
		TRACE(msg);

		open.push_back(_ttoi(strOpen));
		high.push_back(_ttoi(strHigh));
		low.push_back(_ttoi(strLow));
		close.push_back(_ttoi(strClose));
		volume.push_back(_ttoi(strVol));

		
		if (cur_time == _ttoi(strTime)) {
			milisecond_time++;
		}
		else milisecond_time = 0;


		std::vector<int> ymd = SmUtil::IntToTime(_ttoi(strDate));
		std::vector<int> hms = SmUtil::IntToTime(_ttoi(strTime));
		hmdf::DateTime    chart_time(_ttoi(strDate), hms[0], hms[1], hms[2], milisecond_time);
		date_time.push_back(chart_time);

		double cv_dt = Chart::chartTime(ymd[0], ymd[1], ymd[2], hms[0], hms[1], hms[2]);
		chart_dt.push_back(cv_dt);

		cur_time = _ttoi(strTime);
	}

	chart_data->SetChartData(std::move(chart_dt), std::move(high), std::move(low), std::move(open), std::move(close), std::move(volume), std::move(date_time));
	chart_data->Received(true);
	chart_data->ProcessDataPerBar();

	((CMainFrame*)AfxGetMainWnd())->SetChartData(chart_data, it->second.WindowId, it->second.SeriesIndex);


	OnChartTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnChartDataShort(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	CString msg;

	auto it = _ChartReqMap.find(nRqID);
	if (it == _ChartReqMap.end()) {
		OnTaskComplete(nRqID);
		return;
	}
	const std::string symbol_code = it->second.SymbolCode;
	const int chart_type = it->second.ChartType;
	const int cycle = it->second.Cycle;
	std::vector<double> open, high, low, close, volume, chart_dt;
	std::vector<hmdf::DateTime> date_time;

	std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindAddChartData(symbol_code, static_cast<SmChartType>(chart_type), cycle);
	int milisecond_time = 0;
	int cur_time = 0;
	// 가장 최근것이 가장 먼저 온다. 따라서 가장 과거의 데이터를 먼저 가져온다.
	// Received the chart data first.
	for (int i = nRepeatCnt - 1; i >= 0; --i) {
		CString strCurDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "일자");
		CString strCurTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "시간");
		CString strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내일자");
		CString strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내시간");
		CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "시가");
		CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "고가");
		CString strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "저가");
		CString strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종가");
		CString strVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "체결량");

		msg.Format("OnChartData = index = %d, close = %d, %s, %s, %s\n", i, _ttoi(strClose), symbol_code.c_str(), strDate, strTime);
		TRACE(msg);

		/*
		int o, h, l, c, v;
		o = _ttoi(strOpen);
		h = _ttoi(strHigh);
		l = _ttoi(strLow);
		c = _ttoi(strClose);
		v = _ttoi(strVol);
		
		chart_data->AppendChartData(h, l, o, c, v, _ttoi(strDate), _ttoi(strTime));
		*/
		open.push_back(_ttoi(strOpen));
		high.push_back(_ttoi(strHigh));
		low.push_back(_ttoi(strLow));
		close.push_back(_ttoi(strClose));
		volume.push_back(_ttoi(strVol));

		//std::vector<int> hms = SmUtil::IntToTime(_ttoi(strTime));
		if (cur_time == _ttoi(strTime)) {
			milisecond_time++;
		}
		else milisecond_time = 0;

		std::vector<int> ymd = SmUtil::IntToTime(_ttoi(strDate));
		std::vector<int> hms = SmUtil::IntToTime(_ttoi(strTime));
		hmdf::DateTime    chart_time(_ttoi(strDate), hms[0], hms[1], hms[2], milisecond_time);
		date_time.push_back(chart_time);

		double cv_dt = Chart::chartTime(ymd[0], ymd[1], ymd[2], hms[0], hms[1], hms[2]);
		chart_dt.push_back(cv_dt);

		cur_time = _ttoi(strTime);
	}
	chart_data->SetChartData(std::move(chart_dt), std::move(high), std::move(low), std::move(open), std::move(close), std::move(volume), std::move(date_time));
	chart_data->Received(true);
	chart_data->ProcessDataPerBar();

	((CMainFrame*)AfxGetMainWnd())->SetChartData(chart_data, it->second.WindowId, it->second.SeriesIndex);

	OnChartTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnDomesticChartData(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");
	CString msg;

	auto it = _ChartReqMap.find(nRqID);
	if (it == _ChartReqMap.end()) {
		OnTaskComplete(nRqID);
		return;
	}
	const std::string symbol_code = it->second.SymbolCode;
	const int chart_type = it->second.ChartType;
	const int cycle = it->second.Cycle;
	std::vector<double> open, high, low, close, volume, chart_dt;
	std::vector<hmdf::DateTime> date_time;

	int milisecond_time = 0;
	int cur_time = 0;

	std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindAddChartData(symbol_code, static_cast<SmChartType>(chart_type), cycle);
	// 가장 최근것이 가장 먼저 온다. 따라서 가장 과거의 데이터를 먼저 가져온다.
	// Received the chart data first.
	for (int i = nRepeatCnt - 1; i >= 0; --i) {
		CString strCurDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "날짜시간");
		
		if (chart_type == SmChartType::MIN)
			strCurDate.Append(_T("00"));
		else
			strCurDate.Append(_T("000000"));
		
		CString strTime = strCurDate.Right(6);
		CString strDate = strCurDate.Left(8);

		CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "시가");
		CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "고가");
		CString strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "저가");
		CString strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "종가");
		CString strVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "거래량");

		msg.Format("OnChartData = index = %d, close = %d, %s, %s\n", i, _ttoi(strClose), symbol_code.c_str(), strCurDate);
		TRACE(msg);
		/*
		int o, h, l, c, v;
		o = _ttoi(strOpen);
		h = _ttoi(strHigh);
		l = _ttoi(strLow);
		c = _ttoi(strClose);
		v = _ttoi(strVol);

		chart_data->AppendChartData(h, l, o, c, v, _ttoi(strDate), _ttoi(strTime));
		*/
		open.push_back(_ttoi(strOpen));
		high.push_back(_ttoi(strHigh));
		low.push_back(_ttoi(strLow));
		close.push_back(_ttoi(strClose));
		volume.push_back(_ttoi(strVol));

		//std::vector<int> hms = SmUtil::IntToDate(_ttoi(strTime));
		//hmdf::DateTime    chart_time(_ttoi(strDate), hms[0], hms[1], hms[2], 0);
		//date_time.push_back(chart_time);

		//std::vector<int> hms = SmUtil::IntToTime(_ttoi(strTime));
		if (cur_time == _ttoi(strTime)) {
			milisecond_time++;
		}
		else milisecond_time = 0;

		std::vector<int> ymd = SmUtil::IntToTime(_ttoi(strDate));
		std::vector<int> hms = SmUtil::IntToTime(_ttoi(strTime));
		hmdf::DateTime    chart_time(_ttoi(strDate), hms[0], hms[1], hms[2], milisecond_time);
		date_time.push_back(chart_time);

		double cv_dt = Chart::chartTime(ymd[0], ymd[1], ymd[2], hms[0], hms[1], hms[2]);
		chart_dt.push_back(cv_dt);

		cur_time = _ttoi(strTime);
	}
	chart_data->SetChartData(std::move(chart_dt), std::move(high), std::move(low), std::move(open), std::move(close), std::move(volume), std::move(date_time));
	chart_data->Received(true);
	chart_data->ProcessDataPerBar();

	((CMainFrame*)AfxGetMainWnd())->SetChartData(chart_data, it->second.WindowId, it->second.SeriesIndex);

	OnChartTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnChartDataLong_Init(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");

	CString msg;
	auto req = _ReqMap.find(nRqID);
	if (req == _ReqMap.end()) {
		OnTaskComplete(nRqID);
		return;
	}

	const std::string symbol_code = std::any_cast<std::string>(req->second["symbol_code"]);
	const int chart_type = std::any_cast<int>(req->second["chart_type"]);
	const int cycle = std::any_cast<int>(req->second["cycle"]);
	std::vector<double> open, high, low, close, volume, chart_dt;
	std::vector<hmdf::DateTime> date_time;
	int milisecond_time = 0;
	int cur_time = 0;
	std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindAddChartData(symbol_code, static_cast<SmChartType>(chart_type), cycle);
	// 가장 최근것이 가장 먼저 온다. 따라서 가장 과거의 데이터를 먼저 가져온다.
	// Received the chart data first.
	for (int i = nRepeatCnt - 1; i >= 0; --i) {
		CString strCurDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "일자");
		CString strCurTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "시간");
		CString strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "국내일자");
		CString strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "국내시간");
		CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "시가");
		CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "고가");
		CString strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "저가");
		CString strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "종가");
		CString strVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "체결량");
		CString strCumulVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "누적거래량");

		msg.Format("OnChartData = index = %d, close = %d, %s, %s, %s\n", i, _ttoi(strClose), symbol_code.c_str(), strDate, strTime);
		TRACE(msg);
		/*
		int o, h, l, c, v;
		o = _ttoi(strOpen);
		h = _ttoi(strHigh);
		l = _ttoi(strLow);
		c = _ttoi(strClose);
		v = _ttoi(strVol);

		chart_data->AppendChartData(h, l, o, c, v, _ttoi(strDate), _ttoi(strTime));
		*/

		open.push_back(_ttoi(strOpen));
		high.push_back(_ttoi(strHigh));
		low.push_back(_ttoi(strLow));
		close.push_back(_ttoi(strClose));
		volume.push_back(_ttoi(strVol));


		if (cur_time == _ttoi(strTime)) {
			milisecond_time++;
		}
		else milisecond_time = 0;


		//std::vector<int> hms = SmUtil::IntToDate(_ttoi(strTime));
		//hmdf::DateTime    chart_time(_ttoi(strDate), hms[0], hms[1], hms[2], milisecond_time);
		//date_time.push_back(chart_time);

		//cur_time = _ttoi(strTime);
		std::vector<int> ymd = SmUtil::IntToTime(_ttoi(strDate));
		std::vector<int> hms = SmUtil::IntToTime(_ttoi(strTime));
		hmdf::DateTime    chart_time(_ttoi(strDate), hms[0], hms[1], hms[2], milisecond_time);
		date_time.push_back(chart_time);

		double cv_dt = Chart::chartTime(ymd[0], ymd[1], ymd[2], hms[0], hms[1], hms[2]);
		chart_dt.push_back(cv_dt);

		cur_time = _ttoi(strTime);
	}

	chart_data->SetChartData(std::move(chart_dt), std::move(high), std::move(low), std::move(open), std::move(close), std::move(volume), std::move(date_time));
	chart_data->Received(true);

	chart_data->ProcessDataPerBar();

	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnChartDataShort_Init(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	CString msg;

	auto req = _ReqMap.find(nRqID);
	if (req == _ReqMap.end()) {
		OnTaskComplete(nRqID);
		return;
	}

	const std::string symbol_code = std::any_cast<std::string>(req->second["symbol_code"]);
	const int chart_type = std::any_cast<int>(req->second["chart_type"]);
	const int cycle = std::any_cast<int>(req->second["cycle"]);

	std::vector<double> open, high, low, close, volume, chart_dt;
	std::vector<hmdf::DateTime> date_time;

	std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindAddChartData(symbol_code, static_cast<SmChartType>(chart_type), cycle);
	int milisecond_time = 0;
	int cur_time = 0;
	// 가장 최근것이 가장 먼저 온다. 따라서 가장 과거의 데이터를 먼저 가져온다.
	// Received the chart data first.
	for (int i = nRepeatCnt - 1; i >= 0; --i) {
		CString strCurDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "일자");
		CString strCurTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "시간");
		CString strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내일자");
		CString strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내시간");
		CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "시가");
		CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "고가");
		CString strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "저가");
		CString strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종가");
		CString strVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "체결량");

		msg.Format("OnChartData = index = %d, close = %d, %s, %s, %s\n", i, _ttoi(strClose), symbol_code.c_str(), strDate, strTime);
		TRACE(msg);

		/*
		int o, h, l, c, v;
		o = _ttoi(strOpen);
		h = _ttoi(strHigh);
		l = _ttoi(strLow);
		c = _ttoi(strClose);
		v = _ttoi(strVol);

		chart_data->AppendChartData(h, l, o, c, v, _ttoi(strDate), _ttoi(strTime));
		*/
		open.push_back(_ttoi(strOpen));
		high.push_back(_ttoi(strHigh));
		low.push_back(_ttoi(strLow));
		close.push_back(_ttoi(strClose));
		volume.push_back(_ttoi(strVol));

		//std::vector<int> hms = SmUtil::IntToDate(_ttoi(strTime));
		if (cur_time == _ttoi(strTime)) {
			milisecond_time++;
		}
		else milisecond_time = 0;

		//hmdf::DateTime    chart_time(_ttoi(strDate), hms[0], hms[1], hms[2], milisecond_time);
		//date_time.push_back(chart_time);
		//cur_time = _ttoi(strTime);
		std::vector<int> ymd = SmUtil::IntToTime(_ttoi(strDate));
		std::vector<int> hms = SmUtil::IntToTime(_ttoi(strTime));
		hmdf::DateTime    chart_time(_ttoi(strDate), hms[0], hms[1], hms[2], milisecond_time);
		date_time.push_back(chart_time);

		double cv_dt = Chart::chartTime(ymd[0], ymd[1], ymd[2], hms[0], hms[1], hms[2]);
		chart_dt.push_back(cv_dt);

		cur_time = _ttoi(strTime);
	}
	chart_data->SetChartData(std::move(chart_dt), std::move(high), std::move(low), std::move(open), std::move(close), std::move(volume), std::move(date_time));
	chart_data->Received(true);

	chart_data->ProcessDataPerBar();


	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::on_dm_expected(const CString& strKey, const LONG& nRealType)
{
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strExpected = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "예상체결가격");
	nlohmann::json quote;

	quote["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	quote["expected"] = _ttoi(strExpected);

	if (auto wp = _Client.lock()) {
		wp->on_dm_expected(std::move(quote));
	}
}

void DarkHorse::ViClient::on_dm_commodity_future_quote(const CString& strKey, const LONG& nRealType)
{
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결구분");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결량");


	CString	strClose = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "현재가");
	CString	strOpen = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "시가");
	CString	strHigh = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "고가");
	CString	strLow = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "저가");

	nlohmann::json quote;

	quote["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	//quote["symbol_name_kr"] = static_cast<const char*>(strSymbolNameKr.Trim());
	//quote["delta_day"] = static_cast<const char*>(strDeltaDay.Trim());
	//quote["delta_day_sign"] = static_cast<const char*>(strDeltaDaySign.Trim());
	//quote["updown_rate"] = static_cast<const char*>(strUpdownRate.Trim());
	//quote["filled_time"] = static_cast<const char*>(strFilledTime.Trim());
	quote["close"] = (_ttoi(strClose.Trim()));
	quote["open"] = (_ttoi(strOpen.Trim()));
	quote["high"] = (_ttoi(strHigh.Trim()));
	quote["low"] = (_ttoi(strLow.Trim()));
	quote["cumulative_amount"] = 0;
	quote["time"] = static_cast<const char*>(strTime.Trim());
	quote["volume"] = _ttoi(strVolume.Trim());
	quote["updown_rate"] = 0;
	quote["up_down"] = strUpdown == "+" ? 1 : -1;

	if (auto wp = _Client.lock()) {
		wp->on_dm_commodity_future_quote(std::move(quote));
	}
}
void DarkHorse::ViClient::on_dm_commodity_future_hoga(const CString& strKey, const LONG& nRealType)
{
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "kfutcode");



	CString	strSellPrice1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho1");
	CString	strBuyPrice1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho1");
	CString	strSellQty1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem1");
	CString	strBuyQty1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem1");
	CString	strSellCnt1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt1");
	CString	strBuyCnt1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt1");



	CString	strSellPrice2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho2");
	CString	strBuyPrice2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho2");
	CString	strSellQty2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem2");
	CString	strBuyQty2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem2");
	CString	strSellCnt2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt2");
	CString	strBuyCnt2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt2");


	CString	strSellPrice3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho3");
	CString	strBuyPrice3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho3");
	CString	strSellQty3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem3");
	CString	strBuyQty3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem3");
	CString	strSellCnt3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt3");
	CString	strBuyCnt3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt3");


	CString	strSellPrice4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho4");
	CString	strBuyPrice4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho4");
	CString	strSellQty4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem4");
	CString	strBuyQty4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem4");
	CString	strSellCnt4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt4");
	CString	strBuyCnt4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt4");


	CString	strSellPrice5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho5");
	CString	strBuyPrice5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho5");
	CString	strSellQty5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem5");
	CString	strBuyQty5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem5");
	CString	strSellCnt5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt5");
	CString	strBuyCnt5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt5");

	CString strHogaTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "hotime");

	CString	strTotSellQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totofferrem");
	CString	strTotBuyQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totbidrem");
	CString	strTotSellCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totoffercnt");
	CString	strTotBuyCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totbidcnt");

	nlohmann::json hoga;
	hoga["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	hoga["hoga_time"] = static_cast<const char*>(strHogaTime.Trim());
	hoga["tot_buy_qty"] = _ttoi(strTotBuyQty.Trim());
	hoga["tot_sell_qty"] = _ttoi(strTotSellQty.Trim());
	hoga["tot_buy_cnt"] = _ttoi(strTotBuyCnt.Trim());
	hoga["tot_sell_cnt"] = _ttoi(strTotSellCnt.Trim());

	hoga["hoga_items"][0]["sell_price"] = _ttoi(strSellPrice1.Trim());
	hoga["hoga_items"][0]["buy_price"] = _ttoi(strBuyPrice1.Trim());
	hoga["hoga_items"][0]["sell_qty"] = _ttoi(strSellQty1.Trim());
	hoga["hoga_items"][0]["buy_qty"] = _ttoi(strBuyQty1.Trim());
	hoga["hoga_items"][0]["sell_cnt"] = _ttoi(strSellCnt1.Trim());
	hoga["hoga_items"][0]["buy_cnt"] = _ttoi(strBuyCnt1.Trim());

	hoga["hoga_items"][1]["sell_price"] = _ttoi(strSellPrice2.Trim());
	hoga["hoga_items"][1]["buy_price"] = _ttoi(strBuyPrice2.Trim());
	hoga["hoga_items"][1]["sell_qty"] = _ttoi(strSellQty2.Trim());
	hoga["hoga_items"][1]["buy_qty"] = _ttoi(strBuyQty2.Trim());
	hoga["hoga_items"][1]["sell_cnt"] = _ttoi(strSellCnt2.Trim());
	hoga["hoga_items"][1]["buy_cnt"] = _ttoi(strBuyCnt2.Trim());

	hoga["hoga_items"][2]["sell_price"] = _ttoi(strSellPrice3.Trim());
	hoga["hoga_items"][2]["buy_price"] = _ttoi(strBuyPrice3.Trim());
	hoga["hoga_items"][2]["sell_qty"] = _ttoi(strSellQty3.Trim());
	hoga["hoga_items"][2]["buy_qty"] = _ttoi(strBuyQty3.Trim());
	hoga["hoga_items"][2]["sell_cnt"] = _ttoi(strSellCnt3.Trim());
	hoga["hoga_items"][2]["buy_cnt"] = _ttoi(strBuyCnt3.Trim());

	hoga["hoga_items"][3]["sell_price"] = _ttoi(strSellPrice4.Trim());
	hoga["hoga_items"][3]["buy_price"] = _ttoi(strBuyPrice4.Trim());
	hoga["hoga_items"][3]["sell_qty"] = _ttoi(strSellQty4.Trim());
	hoga["hoga_items"][3]["buy_qty"] = _ttoi(strBuyQty4.Trim());
	hoga["hoga_items"][3]["sell_cnt"] = _ttoi(strSellCnt4.Trim());
	hoga["hoga_items"][3]["buy_cnt"] = _ttoi(strBuyCnt4.Trim());

	hoga["hoga_items"][4]["sell_price"] = _ttoi(strSellPrice5.Trim());
	hoga["hoga_items"][4]["buy_price"] = _ttoi(strBuyPrice5.Trim());
	hoga["hoga_items"][4]["sell_qty"] = _ttoi(strSellQty5.Trim());
	hoga["hoga_items"][4]["buy_qty"] = _ttoi(strBuyQty5.Trim());
	hoga["hoga_items"][4]["sell_cnt"] = _ttoi(strSellCnt5.Trim());
	hoga["hoga_items"][4]["buy_cnt"] = _ttoi(strBuyCnt5.Trim());

	if (auto wp = _Client.lock()) {
		wp->on_dm_commodity_future_hoga(std::move(hoga));
	}
}

void DarkHorse::ViClient::on_dm_option_quote(const CString& strKey, const LONG& nRealType)
{
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결량");
	CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결구분");

	

	CString	strClose = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "현재가");
	CString	strOpen = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "시가");
	CString	strHigh = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "고가");
	CString	strLow = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "저가");

	nlohmann::json quote;

	quote["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	//quote["symbol_name_kr"] = static_cast<const char*>(strSymbolNameKr.Trim());
	//quote["delta_day"] = static_cast<const char*>(strDeltaDay.Trim());
	//quote["delta_day_sign"] = static_cast<const char*>(strDeltaDaySign.Trim());
	//quote["updown_rate"] = static_cast<const char*>(strUpdownRate.Trim());
	//quote["filled_time"] = static_cast<const char*>(strFilledTime.Trim());
	quote["close"] = (_ttoi(strClose.Trim()));
	quote["open"] = (_ttoi(strOpen.Trim()));
	quote["high"] = (_ttoi(strHigh.Trim()));
	quote["low"] = (_ttoi(strLow.Trim()));
	quote["cumulative_amount"] = 0;
	quote["time"] = static_cast<const char*>(strTime.Trim());
	quote["volume"] = _ttoi(strVolume.Trim());
	quote["updown_rate"] = 0;
	quote["up_down"] = strUpdown == "+" ? 1 : -1;
	if (auto wp = _Client.lock()) {
		wp->on_dm_option_quote(std::move(quote));
	}
}
void DarkHorse::ViClient::on_dm_option_hoga(const CString& strKey, const LONG& nRealType)
{
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");



	CString	strSellPrice1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가1");
	CString	strBuyPrice1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가1");
	CString	strSellQty1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량1");
	CString	strBuyQty1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량1");
	CString	strSellCnt1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수1");
	CString	strBuyCnt1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수1");



	CString	strSellPrice2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가2");
	CString	strBuyPrice2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가2");
	CString	strSellQty2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량2");
	CString	strBuyQty2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량2");
	CString	strSellCnt2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수2");
	CString	strBuyCnt2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수2");


	CString	strSellPrice3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가3");
	CString	strBuyPrice3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가3");
	CString	strSellQty3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량3");
	CString	strBuyQty3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량3");
	CString	strSellCnt3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수3");
	CString	strBuyCnt3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수3");


	CString	strSellPrice4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가4");
	CString	strBuyPrice4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가4");
	CString	strSellQty4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량4");
	CString	strBuyQty4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량4");
	CString	strSellCnt4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수4");
	CString	strBuyCnt4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수4");


	CString	strSellPrice5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가5");
	CString	strBuyPrice5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가5");
	CString	strSellQty5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량5");
	CString	strBuyQty5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량5");
	CString	strSellCnt5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수5");
	CString	strBuyCnt5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수5");

	CString strHogaTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");

	CString	strTotSellQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총수량");
	CString	strTotBuyQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총수량");
	CString	strTotSellCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총건수");
	CString	strTotBuyCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총건수");

	nlohmann::json hoga;
	hoga["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	hoga["hoga_time"] = static_cast<const char*>(strHogaTime.Trim());
	hoga["tot_buy_qty"] = _ttoi(strTotBuyQty.Trim());
	hoga["tot_sell_qty"] = _ttoi(strTotSellQty.Trim());
	hoga["tot_buy_cnt"] = _ttoi(strTotBuyCnt.Trim());
	hoga["tot_sell_cnt"] = _ttoi(strTotSellCnt.Trim());

	hoga["hoga_items"][0]["sell_price"] = _ttoi(strSellPrice1.Trim());
	hoga["hoga_items"][0]["buy_price"] = _ttoi(strBuyPrice1.Trim());
	hoga["hoga_items"][0]["sell_qty"] = _ttoi(strSellQty1.Trim());
	hoga["hoga_items"][0]["buy_qty"] = _ttoi(strBuyQty1.Trim());
	hoga["hoga_items"][0]["sell_cnt"] = _ttoi(strSellCnt1.Trim());
	hoga["hoga_items"][0]["buy_cnt"] = _ttoi(strBuyCnt1.Trim());

	hoga["hoga_items"][1]["sell_price"] = _ttoi(strSellPrice2.Trim());
	hoga["hoga_items"][1]["buy_price"] = _ttoi(strBuyPrice2.Trim());
	hoga["hoga_items"][1]["sell_qty"] = _ttoi(strSellQty2.Trim());
	hoga["hoga_items"][1]["buy_qty"] = _ttoi(strBuyQty2.Trim());
	hoga["hoga_items"][1]["sell_cnt"] = _ttoi(strSellCnt2.Trim());
	hoga["hoga_items"][1]["buy_cnt"] = _ttoi(strBuyCnt2.Trim());

	hoga["hoga_items"][2]["sell_price"] = _ttoi(strSellPrice3.Trim());
	hoga["hoga_items"][2]["buy_price"] = _ttoi(strBuyPrice3.Trim());
	hoga["hoga_items"][2]["sell_qty"] = _ttoi(strSellQty3.Trim());
	hoga["hoga_items"][2]["buy_qty"] = _ttoi(strBuyQty3.Trim());
	hoga["hoga_items"][2]["sell_cnt"] = _ttoi(strSellCnt3.Trim());
	hoga["hoga_items"][2]["buy_cnt"] = _ttoi(strBuyCnt3.Trim());

	hoga["hoga_items"][3]["sell_price"] = _ttoi(strSellPrice4.Trim());
	hoga["hoga_items"][3]["buy_price"] = _ttoi(strBuyPrice4.Trim());
	hoga["hoga_items"][3]["sell_qty"] = _ttoi(strSellQty4.Trim());
	hoga["hoga_items"][3]["buy_qty"] = _ttoi(strBuyQty4.Trim());
	hoga["hoga_items"][3]["sell_cnt"] = _ttoi(strSellCnt4.Trim());
	hoga["hoga_items"][3]["buy_cnt"] = _ttoi(strBuyCnt4.Trim());

	hoga["hoga_items"][4]["sell_price"] = _ttoi(strSellPrice5.Trim());
	hoga["hoga_items"][4]["buy_price"] = _ttoi(strBuyPrice5.Trim());
	hoga["hoga_items"][4]["sell_qty"] = _ttoi(strSellQty5.Trim());
	hoga["hoga_items"][4]["buy_qty"] = _ttoi(strBuyQty5.Trim());
	hoga["hoga_items"][4]["sell_cnt"] = _ttoi(strSellCnt5.Trim());
	hoga["hoga_items"][4]["buy_cnt"] = _ttoi(strBuyCnt5.Trim());

	if (auto wp = _Client.lock()) {
		wp->on_dm_option_hoga(std::move(hoga));
	}
}

void DarkHorse::ViClient::on_dm_order_position(const CString& strKey, const LONG& nRealType)
{

}

void DarkHorse::ViClient::OnDomesticChartData_Init(const CString& sTrCode, const LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");
	CString msg;

	auto req = _ReqMap.find(nRqID);
	if (req == _ReqMap.end()) {
		OnTaskComplete(nRqID);
		return;
	}

	const std::string symbol_code = std::any_cast<std::string>(req->second["symbol_code"]);
	const int chart_type = std::any_cast<int>(req->second["chart_type"]);
	const int cycle = std::any_cast<int>(req->second["cycle"]);
	std::vector<double> open, high, low, close, volume, chart_dt;
	std::vector<hmdf::DateTime> date_time;

	std::shared_ptr<SmChartData> chart_data = mainApp.ChartDataMgr()->FindAddChartData(symbol_code, static_cast<SmChartType>(chart_type), cycle);
	int milisecond_time = 0;
	int cur_time = 0;
	// 가장 최근것이 가장 먼저 온다. 따라서 가장 과거의 데이터를 먼저 가져온다.
	// Received the chart data first.
	for (int i = nRepeatCnt - 1; i >= 0; --i) {
		CString strCurDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "날짜시간");

		if (chart_type == SmChartType::MIN)
			strCurDate.Append(_T("00"));
		else
			strCurDate.Append(_T("000000"));

		CString strTime = strCurDate.Right(6);
		CString strDate = strCurDate.Left(8);

		CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "시가");
		CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "고가");
		CString strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "저가");
		CString strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "종가");
		CString strVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "거래량");

		msg.Format("OnChartData = index = %d, close = %d, %s, %s\n", i, _ttoi(strClose), symbol_code.c_str(), strCurDate);
		TRACE(msg);
		/*
		int o, h, l, c, v;
		o = _ttoi(strOpen);
		h = _ttoi(strHigh);
		l = _ttoi(strLow);
		c = _ttoi(strClose);
		v = _ttoi(strVol);

		chart_data->AppendChartData(h, l, o, c, v, _ttoi(strDate), _ttoi(strTime));
		*/
		if (cur_time == _ttoi(strTime)) {
			milisecond_time++;
		}
		else milisecond_time = 0;

		open.push_back(_ttoi(strOpen));
		high.push_back(_ttoi(strHigh));
		low.push_back(_ttoi(strLow));
		close.push_back(_ttoi(strClose));
		volume.push_back(_ttoi(strVol));

		//std::vector<int> hms = SmUtil::IntToDate(_ttoi(strTime));
		//hmdf::DateTime    chart_time(_ttoi(strDate), hms[0], hms[1], hms[2], 0);
		//date_time.push_back(chart_time);

		std::vector<int> ymd = SmUtil::IntToTime(_ttoi(strDate));
		std::vector<int> hms = SmUtil::IntToTime(_ttoi(strTime));
		hmdf::DateTime    chart_time(_ttoi(strDate), hms[0], hms[1], hms[2], milisecond_time);
		date_time.push_back(chart_time);

		double cv_dt = Chart::chartTime(ymd[0], ymd[1], ymd[2], hms[0], hms[1], hms[2]);
		chart_dt.push_back(cv_dt);

		cur_time = _ttoi(strTime);
	}
	chart_data->SetChartData(std::move(chart_dt), std::move(high), std::move(low), std::move(open), std::move(close), std::move(volume), std::move(date_time));
	chart_data->Received(true);

	chart_data->ProcessDataPerBar();

	OnTaskComplete(nRqID);
}

void DarkHorse::ViClient::OnOrderAccepted(const CString& strKey, const LONG& nRealType)
{
	CString strAccountNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strOrderPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strOrderAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");
	CString strOrderPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
	//CString strPriceType = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "가격구분");
	CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문구분");
	CString strOriOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "원주문번호");
	CString strFirstOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "최초원주문번호");
	CString strOrderDate = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문일자");
	CString strOrderTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문시간");
	

	//LOG_F(INFO, _T(" OnOrderAcceptedHd Custoem = %s"), strCustom);

	CString strMsg;
	strMsg.Format("OnOrderAcceptedHd 종목[%s]주문번호[%s][원주문번호[%s], 최초 원주문 번호[%s], 주문구분[%s], 주문수량[%s]\n", strSymbolCode, strOrderNo, strOriOrderNo, strFirstOrderNo, strMan, strOrderAmount);
	
	LOGINFO(CMyLogger::getInstance(), "OnOrderAcceptedHd 종목[%s]주문번호[%s][원주문번호[%s], 최초 원주문 번호[%s], 주문구분[%s], 주문수량[%s]\n", strSymbolCode, strOrderNo, strOriOrderNo, strFirstOrderNo, strMan, strOrderAmount);

	//TRACE(strMsg);
	strCustom.Trim();
	strAccountNo.TrimRight(); // 계좌 번호
	strOrderNo.TrimLeft('0'); // 주문 번호
	strSymbolCode.TrimRight(); // 심볼 코드
	strOrderPrice = strOrderPrice.Trim(); // 주문 가격 트림
	
	const int order_price = ConvertToInt(strSymbolCode, strOrderPrice);
	if (order_price < 0) return;
	// 주문 수량 트림
	strOrderAmount.TrimRight();

	nlohmann::json order_info;
	order_info["order_event"] = OrderEvent::AB_Accepted;
	order_info["account_no"] = static_cast<const char*>(strAccountNo.Trim());
	order_info["order_no"] = static_cast<const char*>(strOrderNo.Trim());
	order_info["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	order_info["order_price"] = order_price;
	order_info["order_amount"] = _ttoi(strOrderAmount.Trim());
	order_info["position_type"] = static_cast<const char*>(strOrderPosition.Trim());
	//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
	order_info["original_order_no"] = static_cast<const char*>(strOriOrderNo.Trim());
	order_info["first_order_no"] = static_cast<const char*>(strFirstOrderNo.Trim());
	order_info["order_date"] = static_cast<const char*>(strOrderDate.Trim());
	order_info["order_time"] = static_cast<const char*>(strOrderTime.Trim());
	order_info["order_type"] = static_cast<const char*>(strMan.Trim());
	//order_info["filled_price"] = static_cast<const char*>(strFilledPrice.Trim());
	//order_info["filled_amount"] = static_cast<const char*>(strFilledAmount.Trim());

	//order_info["filled_date"] = static_cast<const char*>(strFilledDate.Trim());
	//order_info["filled_time"] = static_cast<const char*>(strFilledTime.Trim());

	order_info["custom_info"] = static_cast<const char*>(strCustom.Trim());

	mainApp.order_processor()->add_order_event(std::move(order_info));
}

void DarkHorse::ViClient::OnOrderUnfilled(const CString& strKey, const LONG& nRealType)
{
	CString strAccountNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목");
	CString strOrderPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
	CString strOrderPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strOrderAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");
	CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "조작구분");
	CString strCancelCnt = strOrderAmount;
	CString strModyCnt = strOrderAmount;
	CString strFilledCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결수량");
	CString strRemain = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "잔량");

	CString strOriOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "원주문번호");
	CString strFirstOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "최초원주문번호");
	CString strOrderSeq = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문순서");

	CString strMsg;
	strMsg.Format("OnOrderUnfilled 종목[%s]주문번호[%s][원주문번호[%s], 최초 원주문 번호[%s] ,주문순서[%s], 주문수량[%s], 잔량[%s], 체결수량[%s]\n", strSymbolCode, strOrderNo, strOriOrderNo, strFirstOrderNo, strOrderSeq, strOrderAmount, strRemain, strFilledCnt);
	LOGINFO(CMyLogger::getInstance(), "OnOrderUnfilled 종목[%s] 주문번호[%s][원주문번호[%s], 최초 원주문 번호[%s], 주문구분[%s], 주문수량[%s], 체결수량[%s], 잔량[%s]\n", strSymbolCode, strOrderNo, strOriOrderNo, strFirstOrderNo, strMan, strOrderAmount, strFilledCnt, strRemain);

	//TRACE(strMsg);

	strCustom.Trim();
	// 주문 가격
	strOrderPrice.Trim();
	

	const int order_price = ConvertToInt(strSymbolCode, strOrderPrice);
	if (order_price < 0) return;
	// 계좌 번호 트림
	strAccountNo.TrimRight();
	// 주문 번호 트림
	strOrderNo.TrimLeft('0');
	// 원주문 번호 트림
	strOriOrderNo.TrimLeft('0');
	// 첫주문 번호 트림
	strFirstOrderNo.TrimLeft('0');
	// 심볼 코드 트림
	strSymbolCode.TrimRight();
	// 주문 수량 트림
	strOrderAmount.TrimRight();
	// 정정이나 취소시 처리할 수량 트림
	strRemain.TrimRight();
	// 정정이 이루어진 수량
	strModyCnt.TrimRight();
	// 체결된 수량
	strFilledCnt.TrimRight();
	// 취소된 수량
	strCancelCnt.TrimRight();

	nlohmann::json order_info;
	order_info["order_event"] = OrderEvent::AB_Unfilled;
	order_info["account_no"] = static_cast<const char*>(strAccountNo.Trim());
	order_info["order_no"] = static_cast<const char*>(strOrderNo.Trim());
	order_info["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	order_info["order_price"] = order_price;
	order_info["order_amount"] = _ttoi(strOrderAmount.Trim());
	order_info["position_type"] = static_cast<const char*>(strOrderPosition.Trim());
	//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
	order_info["original_order_no"] = static_cast<const char*>(strOriOrderNo.Trim());
	order_info["first_order_no"] = static_cast<const char*>(strFirstOrderNo.Trim());
	//order_info["order_type"] = static_cast<const char*>(strMan.Trim());
	order_info["remain_count"] = _ttoi(strRemain.Trim());
	order_info["cancelled_count"] = _ttoi(strCancelCnt.Trim());
	order_info["modified_count"] = _ttoi(strModyCnt.Trim());
	order_info["filled_count"] = _ttoi(strFilledCnt.Trim());
	order_info["order_sequence"] = _ttoi(strOrderSeq.Trim());
	//order_info["order_date"] = static_cast<const char*>(strOrderDate.Trim());
	//order_info["order_time"] = static_cast<const char*>(strOrderTime.Trim());

	//order_info["filled_price"] = static_cast<const char*>(strFilledPrice.Trim());
	//order_info["filled_amount"] = static_cast<const char*>(strFilledAmount.Trim());

	//order_info["filled_date"] = static_cast<const char*>(strFilledDate.Trim());
	//order_info["filled_time"] = static_cast<const char*>(strFilledTime.Trim());

	order_info["custom_info"] = static_cast<const char*>(strCustom.Trim());

	mainApp.order_processor()->add_order_event(std::move(order_info));
}

void DarkHorse::ViClient::OnOrderFilled(const CString& strKey, const LONG& nRealType)
{
	CString strAccountNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목");
	CString strOrderPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");

	CString strOrderPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strOrderAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");


	CString strFilledPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결가격");
	CString strFilledAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결수량");
	CString strFilledDate = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결일자");
	CString strFilledTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");

	//CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "조작구분");

	//CString strFee = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "수수료");

	CString strMsg;
	strMsg.Format("OnOrderAcceptedHd 종목[%s]주문번호[%s]\n", strSymbolCode, strOrderNo);
	//strMsg.Format(_T("OnOrderFilledHd 수수료 = %s\n"), strFee);
	//TRACE(strMsg);

	//LOG_F(INFO, _T(" OnOrderFilledHd Custoem = %s"), strCustom);

	// 심볼 코드
	strSymbolCode.Trim();

	strCustom.Trim();
	
	const int order_price = ConvertToInt(strSymbolCode, strOrderPrice);
	if (order_price < 0) return;
	const int filled_price = ConvertToInt(strSymbolCode, strFilledPrice);
	if (filled_price < 0) return;
	// 계좌 번호 트림
	strAccountNo.TrimRight();
	// 주문 번호 트림
	strOrderNo.TrimLeft('0');
	
	
	// 체결 수량
	strFilledAmount.TrimLeft();
	// 체결된 시각
	strFilledTime.TrimRight();

	nlohmann::json order_info;
	order_info["order_event"] = OrderEvent::AB_Filled;
	order_info["account_no"] = static_cast<const char*>(strAccountNo.Trim());
	order_info["order_no"] = static_cast<const char*>(strOrderNo.Trim());
	order_info["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	order_info["order_price"] = order_price;
	order_info["order_amount"] = _ttoi(strOrderAmount.Trim());
	order_info["position_type"] = static_cast<const char*>(strOrderPosition.Trim());
	//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
	//order_info["ori_order_no"] = static_cast<const char*>(strOriOrderNo.Trim());
	//order_info["first_order_no"] = static_cast<const char*>(strFirstOrderNo.Trim());
	//order_info["order_date"] = static_cast<const char*>(strOrderDate.Trim());
	//order_info["order_time"] = static_cast<const char*>(strOrderTime.Trim());

	order_info["filled_price"] = filled_price;
	order_info["filled_count"] = _ttoi(strFilledAmount.Trim());

	LOGINFO(CMyLogger::getInstance(), "OnOrderFilled order_no = %s, account_no = %s, symbol_code = %s, filled_amount = %s", strOrderNo, strAccountNo, strSymbolCode, strFilledAmount);

	order_info["filled_date"] = static_cast<const char*>(strFilledDate.Trim());
	order_info["filled_time"] = static_cast<const char*>(strFilledTime.Trim());

	order_info["custom_info"] = static_cast<const char*>(strCustom.Trim());

	mainApp.order_processor()->add_order_event(std::move(order_info));
}

void DarkHorse::ViClient::on_dm_order_accepted(const CString& strKey, const LONG& nRealType)
{
	CString strAccountNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strOrderPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strOrderAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");
	CString strOrderPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
	//CString strPriceType = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "가격구분");
	CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "조작구분");
	//CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문구분");
	CString strOriOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "원주문번호");
	CString strFirstOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "최초원주문번호");
	CString strOrderTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "접수시간");


	//LOG_F(INFO, _T(" OnOrderAcceptedHd Custoem = %s"), strCustom);

	CString strMsg;
	strMsg.Format("OnOrderAcceptedHd 종목[%s]주문번호[%s][원주문번호[%s], 최초 원주문 번호[%s], 주문구분[%s], 주문수량[%s]\n", strSymbolCode, strOrderNo, strOriOrderNo, strFirstOrderNo, strMan, strOrderAmount);

	//TRACE(strMsg);
	strCustom.Trim();
	strAccountNo.TrimRight(); // 계좌 번호
	strOrderNo.TrimLeft('0'); // 주문 번호
	strSymbolCode.TrimRight(); // 심볼 코드
	strOrderPrice = strOrderPrice.Trim(); // 주문 가격 트림

	const int order_price = ConvertToInt(strSymbolCode, strOrderPrice);
	if (order_price < 0) return;
	// 주문 수량 트림
	strOrderAmount.TrimRight();

	nlohmann::json order_info;
	order_info["order_event"] = OrderEvent::DM_Accepted;
	order_info["account_no"] = static_cast<const char*>(strAccountNo.Trim());
	order_info["order_no"] = static_cast<const char*>(strOrderNo.Trim());
	order_info["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	order_info["order_price"] = order_price;
	order_info["order_amount"] = _ttoi(strOrderAmount.Trim());
	order_info["position_type"] = static_cast<const char*>(strOrderPosition.Trim());
	//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
	order_info["original_order_no"] = static_cast<const char*>(strOriOrderNo.Trim());
	order_info["first_order_no"] = static_cast<const char*>(strFirstOrderNo.Trim());
	//order_info["order_date"] = static_cast<const char*>(strOrderDate.Trim());
	order_info["order_time"] = static_cast<const char*>(strOrderTime.Trim());
	order_info["order_type"] = static_cast<const char*>(strMan.Trim());
	//order_info["filled_price"] = static_cast<const char*>(strFilledPrice.Trim());
	//order_info["filled_amount"] = static_cast<const char*>(strFilledAmount.Trim());

	//order_info["filled_date"] = static_cast<const char*>(strFilledDate.Trim());
	//order_info["filled_time"] = static_cast<const char*>(strFilledTime.Trim());

	order_info["custom_info"] = static_cast<const char*>(strCustom.Trim());

	mainApp.order_processor()->add_order_event(std::move(order_info));
}


void DarkHorse::ViClient::on_dm_order_unfilled(const CString& strKey, const LONG& nRealType)
{
	CString strAccountNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strOrderPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
	CString strOrderPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strOrderAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");
	CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "조작구분");
	CString strCancelCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "취소수량");
	CString strModyCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "정정수량");
	CString strFilledCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결수량");
	CString strRemain = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "잔량");

	CString strOriOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "원주문번호");
	CString strFirstOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "최초원주문번호");
	CString strOrderSeq = "0";

	CString strMsg;
	strMsg.Format("OnOrderUnfilled 종목[%s]주문번호[%s][원주문번호[%s], 최초 원주문 번호[%s] ,주문순서[%s], 주문수량[%s], 잔량[%s], 체결수량[%s]\n", strSymbolCode, strOrderNo, strOriOrderNo, strFirstOrderNo, strOrderSeq, strOrderAmount, strRemain, strFilledCnt);

	//TRACE(strMsg);

	strCustom.Trim();
	// 주문 가격
	strOrderPrice.Trim();


	const int order_price = ConvertToInt(strSymbolCode, strOrderPrice);
	if (order_price < 0) return;
	// 계좌 번호 트림
	strAccountNo.TrimRight();
	// 주문 번호 트림
	strOrderNo.TrimLeft('0');
	// 원주문 번호 트림
	strOriOrderNo.TrimLeft('0');
	// 첫주문 번호 트림
	strFirstOrderNo.TrimLeft('0');
	// 심볼 코드 트림
	strSymbolCode.TrimRight();
	// 주문 수량 트림
	strOrderAmount.TrimRight();
	// 정정이나 취소시 처리할 수량 트림
	strRemain.TrimRight();
	// 정정이 이루어진 수량
	strModyCnt.TrimRight();
	// 체결된 수량
	strFilledCnt.TrimRight();
	// 취소된 수량
	strCancelCnt.TrimRight();

	nlohmann::json order_info;
	order_info["order_event"] = OrderEvent::DM_Unfilled;
	order_info["account_no"] = static_cast<const char*>(strAccountNo.Trim());
	order_info["order_no"] = static_cast<const char*>(strOrderNo.Trim());
	order_info["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	order_info["order_price"] = order_price;
	order_info["order_amount"] = _ttoi(strOrderAmount.Trim());
	order_info["position_type"] = static_cast<const char*>(strOrderPosition.Trim());
	//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
	order_info["original_order_no"] = static_cast<const char*>(strOriOrderNo.Trim());
	order_info["first_order_no"] = static_cast<const char*>(strFirstOrderNo.Trim());
	//order_info["order_type"] = static_cast<const char*>(strMan.Trim());
	order_info["remain_count"] = _ttoi(strRemain.Trim());
	order_info["cancelled_count"] = _ttoi(strCancelCnt.Trim());
	order_info["modified_count"] = _ttoi(strModyCnt.Trim());
	order_info["filled_count"] = _ttoi(strFilledCnt.Trim());
	order_info["order_sequence"] = _ttoi(strOrderSeq.Trim());
	//order_info["order_date"] = static_cast<const char*>(strOrderDate.Trim());
	//order_info["order_time"] = static_cast<const char*>(strOrderTime.Trim());

	//order_info["filled_price"] = static_cast<const char*>(strFilledPrice.Trim());
	//order_info["filled_amount"] = static_cast<const char*>(strFilledAmount.Trim());

	//order_info["filled_date"] = static_cast<const char*>(strFilledDate.Trim());
	//order_info["filled_time"] = static_cast<const char*>(strFilledTime.Trim());

	order_info["custom_info"] = static_cast<const char*>(strCustom.Trim());

	mainApp.order_processor()->add_order_event(std::move(order_info));
}


void DarkHorse::ViClient::on_dm_order_filled(const CString& strKey, const LONG& nRealType)
{
	CString strAccountNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strOrderPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");

	CString strOrderPrice = "0";
	CString strOrderAmount = "0";


	CString strFilledPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결가격");
	CString strFilledAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결수량");
	CString strFilledTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");

	//CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "조작구분");

	//CString strFee = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "수수료");

	CString strMsg;
	strMsg.Format("OnOrderAcceptedHd 종목[%s]주문번호[%s]\n", strSymbolCode, strOrderNo);
	//strMsg.Format(_T("OnOrderFilledHd 수수료 = %s\n"), strFee);
	//TRACE(strMsg);

	//LOG_F(INFO, _T(" OnOrderFilledHd Custoem = %s"), strCustom);

	// 심볼 코드
	strSymbolCode.Trim();

	strCustom.Trim();

	const int order_price = ConvertToInt(strSymbolCode, strOrderPrice);
	if (order_price < 0) return;
	const int filled_price = ConvertToInt(strSymbolCode, strFilledPrice);
	if (filled_price < 0) return;
	// 계좌 번호 트림
	strAccountNo.TrimRight();
	// 주문 번호 트림
	strOrderNo.TrimLeft('0');


	// 체결 수량
	strFilledAmount.TrimLeft();
	// 체결된 시각
	strFilledTime.TrimRight();

	nlohmann::json order_info;
	order_info["order_event"] = OrderEvent::DM_Filled;
	order_info["account_no"] = static_cast<const char*>(strAccountNo.Trim());
	order_info["order_no"] = static_cast<const char*>(strOrderNo.Trim());
	order_info["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	order_info["order_price"] = order_price;
	order_info["order_amount"] = _ttoi(strOrderAmount.Trim());
	order_info["position_type"] = static_cast<const char*>(strOrderPosition.Trim());
	//order_info["price_type"] = static_cast<const char*>(strPriceType.Trim());
	//order_info["ori_order_no"] = static_cast<const char*>(strOriOrderNo.Trim());
	//order_info["first_order_no"] = static_cast<const char*>(strFirstOrderNo.Trim());
	//order_info["order_date"] = static_cast<const char*>(strOrderDate.Trim());
	//order_info["order_time"] = static_cast<const char*>(strOrderTime.Trim());

	order_info["filled_price"] = filled_price;
	order_info["filled_count"] = _ttoi(strFilledAmount.Trim());

	LOGINFO(CMyLogger::getInstance(), "order_no = %s, account_no = %s, symbol_code = %s, filled_amount = %s", strOrderNo, strAccountNo, strSymbolCode, strFilledAmount);

	//order_info["filled_date"] = static_cast<const char*>(strFilledDate.Trim());
	order_info["filled_time"] = static_cast<const char*>(strFilledTime.Trim());

	order_info["custom_info"] = static_cast<const char*>(strCustom.Trim());

	mainApp.order_processor()->add_order_event(std::move(order_info));
}

void DarkHorse::ViClient::OnRealtimeQuote(const CString& strKey, const LONG& nRealType)
{
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "기준체결시간");
	CString strClose = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결가");
	CString strOpen = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "시가");
	CString strHigh = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "고가");
	CString strLow = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "저가");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결량");
	CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결구분");
	CString strCumulativeAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "누적거래량");
	CString strPreDayCmp = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비");
	CString strUpRate = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비등락율");
	// 전일대비등락율

	//m_edSeriesO.SetWindowText(strSeries);
	//m_edTimeO.SetWindowText(strTime);
	//CString strFormatPrice = strCloseP;
	//strFormatPrice = m_CommAgent.CommGetHWOrdPrice(strSeries, strCloseP, 0);
	//m_edClosePO.SetWindowText(strFormatPrice);
	//m_edVolumeO.SetWindowText(strVolume);

	nlohmann::json quote;

	quote["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	//quote["symbol_name_kr"] = static_cast<const char*>(strSymbolNameKr.Trim());
	//quote["delta_day"] = static_cast<const char*>(strDeltaDay.Trim());
	//quote["delta_day_sign"] = static_cast<const char*>(strDeltaDaySign.Trim());
	//quote["updown_rate"] = static_cast<const char*>(strUpdownRate.Trim());
	//quote["filled_time"] = static_cast<const char*>(strFilledTime.Trim());
	quote["close"] = (_ttoi(strClose.Trim()));
	quote["open"] = (_ttoi(strOpen.Trim()));
	quote["high"] = (_ttoi(strHigh.Trim()));
	quote["low"] = (_ttoi(strLow.Trim()));
	quote["cumulative_amount"] = (_ttoi(strCumulativeAmount.Trim()));
	quote["time"] = static_cast<const char*>(strTime.Trim());
	quote["volume"] = _ttoi(strVolume.Trim());
	quote["updown_rate"] = static_cast<const char*>(strUpRate.Trim());
	quote["up_down"] = strUpdown == "+" ? 1 : -1;
	if (auto wp = _Client.lock()) {
		wp->OnRealtimeQuote(std::move(quote));
	}
}

void DarkHorse::ViClient::OnRealtimeHoga(const CString& strKey, const LONG& nRealType)
{
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");



	CString	strSellPrice1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가1");
	CString	strBuyPrice1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가1");
	CString	strSellQty1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량1");
	CString	strBuyQty1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량1");
	CString	strSellCnt1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수1");
	CString	strBuyCnt1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수1");



	CString	strSellPrice2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가2");
	CString	strBuyPrice2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가2");
	CString	strSellQty2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량2");
	CString	strBuyQty2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량2");
	CString	strSellCnt2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수2");
	CString	strBuyCnt2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수2");


	CString	strSellPrice3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가3");
	CString	strBuyPrice3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가3");
	CString	strSellQty3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량3");
	CString	strBuyQty3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량3");
	CString	strSellCnt3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수3");
	CString	strBuyCnt3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수3");


	CString	strSellPrice4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가4");
	CString	strBuyPrice4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가4");
	CString	strSellQty4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량4");
	CString	strBuyQty4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량4");
	CString	strSellCnt4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수4");
	CString	strBuyCnt4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수4");


	CString	strSellPrice5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가5");
	CString	strBuyPrice5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가5");
	CString	strSellQty5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량5");
	CString	strBuyQty5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량5");
	CString	strSellCnt5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수5");
	CString	strBuyCnt5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수5");

	CString strHogaTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");

	CString	strTotSellQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총수량");
	CString	strTotBuyQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총수량");
	CString	strTotSellCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총건수");
	CString	strTotBuyCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총건수");

	nlohmann::json hoga;
	hoga["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	hoga["hoga_time"] = static_cast<const char*>(strHogaTime.Trim());
	hoga["tot_buy_qty"] = _ttoi(strTotBuyQty.Trim());
	hoga["tot_sell_qty"] = _ttoi(strTotSellQty.Trim());
	hoga["tot_buy_cnt"] = _ttoi(strTotBuyCnt.Trim());
	hoga["tot_sell_cnt"] = _ttoi(strTotSellCnt.Trim());

	hoga["hoga_items"][0]["sell_price"] = _ttoi(strSellPrice1.Trim());
	hoga["hoga_items"][0]["buy_price"] = _ttoi(strBuyPrice1.Trim());
	hoga["hoga_items"][0]["sell_qty"] = _ttoi(strSellQty1.Trim());
	hoga["hoga_items"][0]["buy_qty"] = _ttoi(strBuyQty1.Trim());
	hoga["hoga_items"][0]["sell_cnt"] = _ttoi(strSellCnt1.Trim());
	hoga["hoga_items"][0]["buy_cnt"] = _ttoi(strBuyCnt1.Trim());

	hoga["hoga_items"][1]["sell_price"] = _ttoi(strSellPrice2.Trim());
	hoga["hoga_items"][1]["buy_price"] = _ttoi(strBuyPrice2.Trim());
	hoga["hoga_items"][1]["sell_qty"] = _ttoi(strSellQty2.Trim());
	hoga["hoga_items"][1]["buy_qty"] = _ttoi(strBuyQty2.Trim());
	hoga["hoga_items"][1]["sell_cnt"] = _ttoi(strSellCnt2.Trim());
	hoga["hoga_items"][1]["buy_cnt"] = _ttoi(strBuyCnt2.Trim());

	hoga["hoga_items"][2]["sell_price"] = _ttoi(strSellPrice3.Trim());
	hoga["hoga_items"][2]["buy_price"] = _ttoi(strBuyPrice3.Trim());
	hoga["hoga_items"][2]["sell_qty"] = _ttoi(strSellQty3.Trim());
	hoga["hoga_items"][2]["buy_qty"] = _ttoi(strBuyQty3.Trim());
	hoga["hoga_items"][2]["sell_cnt"] = _ttoi(strSellCnt3.Trim());
	hoga["hoga_items"][2]["buy_cnt"] = _ttoi(strBuyCnt3.Trim());

	hoga["hoga_items"][3]["sell_price"] = _ttoi(strSellPrice4.Trim());
	hoga["hoga_items"][3]["buy_price"] = _ttoi(strBuyPrice4.Trim());
	hoga["hoga_items"][3]["sell_qty"] = _ttoi(strSellQty4.Trim());
	hoga["hoga_items"][3]["buy_qty"] = _ttoi(strBuyQty4.Trim());
	hoga["hoga_items"][3]["sell_cnt"] = _ttoi(strSellCnt4.Trim());
	hoga["hoga_items"][3]["buy_cnt"] = _ttoi(strBuyCnt4.Trim());

	hoga["hoga_items"][4]["sell_price"] = _ttoi(strSellPrice5.Trim());
	hoga["hoga_items"][4]["buy_price"] = _ttoi(strBuyPrice5.Trim());
	hoga["hoga_items"][4]["sell_qty"] = _ttoi(strSellQty5.Trim());
	hoga["hoga_items"][4]["buy_qty"] = _ttoi(strBuyQty5.Trim());
	hoga["hoga_items"][4]["sell_cnt"] = _ttoi(strSellCnt5.Trim());
	hoga["hoga_items"][4]["buy_cnt"] = _ttoi(strBuyCnt5.Trim());

	if (auto wp = _Client.lock()) {
		wp->OnRealtimeHoga(std::move(hoga));
	}
}

void DarkHorse::ViClient::OnRealtimeDomesticQuote(const CString& strKey, const LONG& nRealType)
{

	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strClose = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "현재가");
	CString strOpen = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "시가");
	CString strHigh = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "고가");
	CString strLow = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "저가");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결량");
	CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결구분");
	CString strCumulativeAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "누적거래량");
	CString strPreDayCmp = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비");
	CString strUpRate = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "등락율");


	nlohmann::json quote;

	quote["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	//quote["symbol_name_kr"] = static_cast<const char*>(strSymbolNameKr.Trim());
	//quote["delta_day"] = static_cast<const char*>(strDeltaDay.Trim());
	//quote["delta_day_sign"] = static_cast<const char*>(strDeltaDaySign.Trim());
	//quote["updown_rate"] = static_cast<const char*>(strUpdownRate.Trim());
	//quote["filled_time"] = static_cast<const char*>(strFilledTime.Trim());
	quote["close"] = (_ttoi(strClose.Trim()));
	quote["open"] = (_ttoi(strOpen.Trim()));
	quote["high"] = (_ttoi(strHigh.Trim()));
	quote["low"] = (_ttoi(strLow.Trim()));
	quote["cumulative_amount"] = (_ttoi(strCumulativeAmount.Trim()));
	quote["time"] = static_cast<const char*>(strTime.Trim());
	quote["volume"] = _ttoi(strVolume.Trim());
	quote["updown_rate"] = static_cast<const char*>(strUpRate.Trim());
	quote["up_down"] = strUpdown == "+" ? 1 : -1;
	if (auto wp = _Client.lock()) {
		wp->OnRealtimeQuote(std::move(quote));
	}
}

void DarkHorse::ViClient::OnRealtimeDomesticHoga(const CString& strKey, const LONG& nRealType)
{
	CString strSymbolCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");



	CString	strSellPrice1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가1");
	CString	strBuyPrice1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가1");
	CString	strSellQty1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량1");
	CString	strBuyQty1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량1");
	CString	strSellCnt1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수1");
	CString	strBuyCnt1 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수1");



	CString	strSellPrice2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가2");
	CString	strBuyPrice2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가2");
	CString	strSellQty2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량2");
	CString	strBuyQty2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량2");
	CString	strSellCnt2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수2");
	CString	strBuyCnt2 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수2");


	CString	strSellPrice3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가3");
	CString	strBuyPrice3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가3");
	CString	strSellQty3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량3");
	CString	strBuyQty3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량3");
	CString	strSellCnt3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수3");
	CString	strBuyCnt3 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수3");


	CString	strSellPrice4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가4");
	CString	strBuyPrice4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가4");
	CString	strSellQty4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량4");
	CString	strBuyQty4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량4");
	CString	strSellCnt4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수4");
	CString	strBuyCnt4 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수4");


	CString	strSellPrice5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가5");
	CString	strBuyPrice5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가5");
	CString	strSellQty5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량5");
	CString	strBuyQty5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량5");
	CString	strSellCnt5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수5");
	CString	strBuyCnt5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수5");

	CString strHogaTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");

	CString	strTotSellQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총수량");
	CString	strTotBuyQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총수량");
	CString	strTotSellCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총건수");
	CString	strTotBuyCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총건수");

	nlohmann::json hoga;
	hoga["symbol_code"] = static_cast<const char*>(strSymbolCode.Trim());
	hoga["hoga_time"] = static_cast<const char*>(strHogaTime.Trim());
	hoga["tot_buy_qty"] = _ttoi(strTotBuyQty.Trim());
	hoga["tot_sell_qty"] = _ttoi(strTotSellQty.Trim());
	hoga["tot_buy_cnt"] = _ttoi(strTotBuyCnt.Trim());
	hoga["tot_sell_cnt"] = _ttoi(strTotSellCnt.Trim());

	hoga["hoga_items"][0]["sell_price"] = _ttoi(strSellPrice1.Trim());
	hoga["hoga_items"][0]["buy_price"] = _ttoi(strBuyPrice1.Trim());
	hoga["hoga_items"][0]["sell_qty"] = _ttoi(strSellQty1.Trim());
	hoga["hoga_items"][0]["buy_qty"] = _ttoi(strBuyQty1.Trim());
	hoga["hoga_items"][0]["sell_cnt"] = _ttoi(strSellCnt1.Trim());
	hoga["hoga_items"][0]["buy_cnt"] = _ttoi(strBuyCnt1.Trim());

	hoga["hoga_items"][1]["sell_price"] = _ttoi(strSellPrice2.Trim());
	hoga["hoga_items"][1]["buy_price"] = _ttoi(strBuyPrice2.Trim());
	hoga["hoga_items"][1]["sell_qty"] = _ttoi(strSellQty2.Trim());
	hoga["hoga_items"][1]["buy_qty"] = _ttoi(strBuyQty2.Trim());
	hoga["hoga_items"][1]["sell_cnt"] = _ttoi(strSellCnt2.Trim());
	hoga["hoga_items"][1]["buy_cnt"] = _ttoi(strBuyCnt2.Trim());

	hoga["hoga_items"][2]["sell_price"] = _ttoi(strSellPrice3.Trim());
	hoga["hoga_items"][2]["buy_price"] = _ttoi(strBuyPrice3.Trim());
	hoga["hoga_items"][2]["sell_qty"] = _ttoi(strSellQty3.Trim());
	hoga["hoga_items"][2]["buy_qty"] = _ttoi(strBuyQty3.Trim());
	hoga["hoga_items"][2]["sell_cnt"] = _ttoi(strSellCnt3.Trim());
	hoga["hoga_items"][2]["buy_cnt"] = _ttoi(strBuyCnt3.Trim());

	hoga["hoga_items"][3]["sell_price"] = _ttoi(strSellPrice4.Trim());
	hoga["hoga_items"][3]["buy_price"] = _ttoi(strBuyPrice4.Trim());
	hoga["hoga_items"][3]["sell_qty"] = _ttoi(strSellQty4.Trim());
	hoga["hoga_items"][3]["buy_qty"] = _ttoi(strBuyQty4.Trim());
	hoga["hoga_items"][3]["sell_cnt"] = _ttoi(strSellCnt4.Trim());
	hoga["hoga_items"][3]["buy_cnt"] = _ttoi(strBuyCnt4.Trim());

	hoga["hoga_items"][4]["sell_price"] = _ttoi(strSellPrice5.Trim());
	hoga["hoga_items"][4]["buy_price"] = _ttoi(strBuyPrice5.Trim());
	hoga["hoga_items"][4]["sell_qty"] = _ttoi(strSellQty5.Trim());
	hoga["hoga_items"][4]["buy_qty"] = _ttoi(strBuyQty5.Trim());
	hoga["hoga_items"][4]["sell_cnt"] = _ttoi(strSellCnt5.Trim());
	hoga["hoga_items"][4]["buy_cnt"] = _ttoi(strBuyCnt5.Trim());

	if (auto wp = _Client.lock()) {
		wp->OnRealtimeHoga(std::move(hoga));
	}
}

// ViClient message handlers
