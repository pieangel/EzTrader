#include "stdafx.h"
#include "SmSaveManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <filesystem>
#include "../Config/SmConfigManager.h"
#include "../Fund/SmFundManager.h"
#include "../Account/SmAccountManager.h"
//#include "VtOrderDialogManager.h"
#include "../Symbol/SmSymbol.h"
//#include "../Order/VtOrderManagerSelector.h"
//#include "HdWindowManager.h"
#include "../Util/VtStringUtil.h"

#include <exception>
#include <fstream>
//#include "VtSystemGroupManager.h"
//#include "VtStrategyWndManager.h"
//#include "VtOutSystemManager.h"
//#include "../Global/MainBeetle.h"
#include "../Util/cryptor.hpp"
#include "../Login/SmLoginManager.h"
#include "../Log/MyLogger.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Account/SmAccount.h"
//#include "Format/format_string.h"
#include "../Global/SmTotalManager.h"
#include "../Xml/pugixml.hpp"

#include <iostream>
#include <filesystem>
#include <chrono>
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace std;
using namespace DarkHorse;
using namespace pugi;
namespace fs = std::filesystem;


template <typename TP>
time_t to_time_t(TP tp) {
	using namespace chrono;
	auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
	return system_clock::to_time_t(sctp);
}


SmSaveManager::SmSaveManager()
{
}


SmSaveManager::~SmSaveManager()
{
}

void SmSaveManager::SaveAccountListToXml()
{
	try {
		
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.

		appPath.append(_T("\\"));
		appPath.append(VtStringUtil::getTimeStr());
		appPath.append(_T(".xml"));
		if (fs::exists(appPath)) {
			return;
		}

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result) {
			return;
		}

		pugi::xml_node application = doc.child("application");

		// 	application.remove_child("login_info");
		// 	pugi::xml_node login_info = application.append_child("login_info");
		// 	VtLoginManager::GetInstance()->SaveToXml(login_info);

		application.remove_child("account_list");
		pugi::xml_node account_list = application.append_child("account_list");
		//mainApp.AcntMgr().SaveToXml(account_list);

		doc.save_file(appPath.c_str());

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmSaveManager::SaveFundListToXml()
{
	try {
		
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.

		appPath.append(_T("\\"));
		appPath.append(VtStringUtil::getTimeStr());
		appPath.append(_T(".xml"));
		filesystem::path path1(appPath);
		if (fs::exists(appPath)) {
			return;
		}

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result) {
			return;
		}

		pugi::xml_node application = doc.child("application");

		application.remove_child("fund_list");
		pugi::xml_node fund_list = application.append_child("fund_list");
		//mainApp.FundMgr().SaveToXml(fund_list);

		doc.save_file(appPath.c_str());

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmSaveManager::WriteSettings()
{
	try {
		
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append("user");
		appPath.append(_T("\\"));
		appPath.append(id);
		// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.
		if (!fs::exists(appPath)) {
			fs::create_directory(appPath);
		}
		appPath.append(_T("\\"));
		appPath.append(VtStringUtil::getTimeStr());
		appPath.append(_T(".xml"));
		if (!fs::exists(appPath)) {
			std::ofstream ofs(appPath);
			ofs << "this is dark horse config file.\n";
			ofs.close();
			/// [create new xml file]
			// Generate new XML document within memory
			pugi::xml_document doc;
			// Alternatively store as shared pointer if tree shall be used for longer
			// time or multiple client calls:
			// std::shared_ptr<pugi::xml_document> spDoc = std::make_shared<pugi::xml_document>();
			// Generate XML declaration
			auto declarationNode = doc.append_child(pugi::node_declaration);
			declarationNode.append_attribute("version") = "1.0";
			declarationNode.append_attribute("encoding") = "euc-kr";
			declarationNode.append_attribute("standalone") = "yes";
			// A valid XML doc must contain a single root node of any name
			auto root = doc.append_child("application");
			// Save XML tree to file.
			// Remark: second optional param is indent string to be used;
			// default indentation is tab character.
			bool saveSucceeded = doc.save_file(appPath.c_str(), PUGIXML_TEXT("  "));
#ifdef DEBUG
			assert(saveSucceeded);
#endif
		}

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
		}

		pugi::xml_node application = doc.child("application");

		application.remove_child("login_info");
		pugi::xml_node login_info = application.append_child("login_info");
		mainApp.LoginMgr()->SaveToXml(login_info);

		application.remove_child("account_list");
		pugi::xml_node account_list = application.append_child("account_list");
		//mainApp.AcntMgr().SaveToXml(account_list);

		application.remove_child("fund_list");
		pugi::xml_node fund_list = application.append_child("fund_list");
		mainApp.FundMgr()->SaveToXml(fund_list);

		/*
		application.remove_child("external_system_list");
		pugi::xml_node external_system_list = application.append_child("external_system_list");
		VtOutSystemManager::GetInstance()->SaveToXml(external_system_list);

		application.remove_child("system_group_list");
		pugi::xml_node system_group_list = application.append_child("system_group_list");
		VtSystemGroupManager::GetInstance()->SaveToXml(system_group_list);

		application.remove_child("order_window_list");
		pugi::xml_node window_list = application.append_child("order_window_list");
		VtOrderDialogManager::GetInstance()->SaveToXml(window_list);

		application.remove_child("stratege_window_list");
		pugi::xml_node stratege_window_list = application.append_child("stratege_window_list");
		VtStrategyWndManager::GetInstance()->SaveToXml(stratege_window_list);

		application.remove_child("etc_window_list");
		pugi::xml_node etc_window_list = application.append_child("etc_window_list");
		HdWindowManager::GetInstance()->SaveToXml(etc_window_list);
		*/

		doc.save_file(appPath.c_str());

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmSaveManager::ReadAccountsAndFunds()
{
	
}

bool SmSaveManager::ListContents(std::map<std::string, std::string>& dest, std::string dir, std::string filter, bool recursively)
{
	try {
		WIN32_FIND_DATAA ffd;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwError = 0;

		// Prepare string
		//if (dir.back() != '\\') dir += "\\";

		// Safety check
		if (dir.length() >= MAX_PATH) {
			LOGINFO(CMyLogger::getInstance(), "Cannot open folder %s: path too long", dir.c_str());
			return false;
		}

		// First entry in directory
		hFind = FindFirstFileA((dir + filter).c_str(), &ffd);

		if (hFind == INVALID_HANDLE_VALUE) {
			LOGINFO(CMyLogger::getInstance(), "Cannot open folder in folder %s: error accessing first entry.", dir.c_str());
			return false;
		}

		// List files in directory
		do {
			// Ignore . and .. folders, they cause stack overflow
			if (strcmp(ffd.cFileName, ".") == 0) continue;
			if (strcmp(ffd.cFileName, "..") == 0) continue;


			// Is directory?
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// Go inside recursively
				//if (recursively)
				//	ListContents(dest, dir + ffd.cFileName, filter, recursively, content_type);
				continue;
			}
			// Add file to our list
			else {

				SYSTEMTIME stLocal;

				// Convert the last-write time to local time.
				FileTimeToSystemTime(&ffd.ftLastWriteTime, &stLocal);
				std::string local_time;
				local_time += std::to_string(stLocal.wYear);
				local_time += std::to_string(stLocal.wMonth);
				local_time += std::to_string(stLocal.wDay);
				CString local_file_time;
				local_file_time.Format("%04d%02d%02d", stLocal.wYear, stLocal.wMonth, stLocal.wDay);
				dest[(LPCTSTR)local_file_time] = dir + ffd.cFileName;
			}

		} while (FindNextFileA(hFind, &ffd));

		// Get last error
		dwError = GetLastError();
		if (dwError != ERROR_NO_MORE_FILES) {
			LOGINFO(CMyLogger::getInstance(), "Error reading file list in folder %s.", dir.c_str());
			return false;
		}

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return true;
}

void SmSaveManager::ReadWindows()
{
	
}

std::string SmSaveManager::GetLastestFile(std::string file_name)
{
	FILETIME bestDate = { 0, 0 };
	FILETIME curDate;
	string name = "";
	CFileFind finder;

	finder.FindFile(file_name.c_str());
	while (finder.FindNextFile())
	{
		finder.GetCreationTime(&curDate);

		if (CompareFileTime(&curDate, &bestDate) > 0)
		{
			bestDate = curDate;
			name = finder.GetFileName().GetString();
			AfxMessageBox(name.c_str());
		}
	}
	return name;
}

void SmSaveManager::SaveOrders()
{
	SaveOrders(_T("orderlist.dat"));
}

void SmSaveManager::SaveOrders(std::string fileName)
{
	
}

void SmSaveManager::SaveFundList(std::string fileName)
{

}

void SmSaveManager::SaveFundList()
{
	SaveFundList(_T("fundlist.dat"));
}

void SmSaveManager::LoadFundList(std::string fileName)
{
	
}

void SmSaveManager::SaveAccountList(std::string fileName)
{
	
}

void SmSaveManager::SaveAccountList()
{
	SaveAccountList(_T("accountlist.dat"));
}

void SmSaveManager::LoadAccountList(std::string fileName)
{
	
}

void SmSaveManager::SaveOrderWndList(std::string fileName, CMainFrame* mainFrm)
{
	
}

void SmSaveManager::LoadOrderWndList(std::string fileName, CMainFrame* mainFrm)
{
	
}

void SmSaveManager::SaveDialogList(std::string fileName, CMainFrame* mainFrm)
{
	
}

void SmSaveManager::LoadDialogList(std::string fileName, CMainFrame* mainFrm)
{
	
}

void SmSaveManager::SaveTotal(std::string fileName, CMainFrame* mainFrm)
{
	
}

void SmSaveManager::SaveLoginInfo(std::string fileName, std::string id, std::string pwd, std::string cert, bool save)
{
	
}

void SmSaveManager::LoadLoginInfo(std::string fileName, std::string& id, std::string& pwd, std::string& cert, bool& save)
{
	
}

void SmSaveManager::SaveSystems(std::string fileName)
{
	
}

void SmSaveManager::LoadSystems(std::string fileName)
{
	
}

void SmSaveManager::SaveOutSystems(std::string fileName)
{
	
}

void SmSaveManager::LoadOutSystems(std::string fileName)
{
	
}

void SmSaveManager::SaveOutSignal(std::string fileName)
{

}

void SmSaveManager::LoadOutSignal(std::string fileName)
{

}

void SmSaveManager::CreateUserDirectory()
{
	std::string appPath;
	appPath = SmConfigManager::GetApplicationPath();
	appPath.append(_T("\\"));
	appPath.append(mainApp.LoginMgr()->id());
	
	if (fs::exists(appPath)) { // 디렉토리가 존재하지 않을 경우
		fs::create_directory(appPath);
	}
}

bool SmSaveManager::IsAccountFileExist()
{
	return false;
}

void SmSaveManager::SaveLoginInfoToXml(std::string id, std::string pwd, std::string cert, bool save)
{
	
}

int SmSaveManager::LoadLoginInfoFromXml(std::string& id, std::string& pwd, std::string& cert, bool& save)
{
	

	return -1;
}

int SmSaveManager::LoadLoginInfoFromXml()
{
	
	return -1;
}

void SmSaveManager::LoadRunInfoFromXml()
{
	
}

void SmSaveManager::GetWindowSymbolList(std::set<std::string>& symbol_list)
{
	
}

void SmSaveManager::SaveAccountPasswords(const std::map<int, std::shared_ptr<DarkHorse::SmAccount>>& account_map)
{
	if (account_map.empty()) return;

	try {
		

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(_T("acnt_pwd.xml"));
		
		if (!fs::exists(appPath)) {
			/// [create new xml file]
			// Generate new XML document within memory
			pugi::xml_document doc;
			// Alternatively store as shared pointer if tree shall be used for longer
			// time or multiple client calls:
			// std::shared_ptr<pugi::xml_document> spDoc = std::make_shared<pugi::xml_document>();
			// Generate XML declaration
			auto declarationNode = doc.append_child(pugi::node_declaration);
			declarationNode.append_attribute("version") = "1.0";
			declarationNode.append_attribute("encoding") = "euc-kr";
			declarationNode.append_attribute("standalone") = "yes";
			// A valid XML doc must contain a single root node of any name
			auto root = doc.append_child("application");
			// Save XML tree to file.
			// Remark: second optional param is indent string to be used;
			// default indentation is tab character.
			bool saveSucceeded = doc.save_file(appPath.c_str(), PUGIXML_TEXT("  "));
			assert(saveSucceeded);
		}

		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
		}

		pugi::xml_node application = doc.child("application");

		application.remove_child("account_list");
		pugi::xml_node account_list = application.append_child("account_list");


		for (auto it = account_map.begin(); it != account_map.end(); ++it) {
			std::shared_ptr<DarkHorse::SmAccount> account = it->second;
			auto enc_AccountNo = cryptor::encrypt(account->No());
			auto enc_Password = cryptor::encrypt(account->Pwd());

			pugi::xml_node account_node = account_list.append_child("account");

			pugi::xml_node child_node = account_node.append_child("account_no");
			child_node.append_child(pugi::node_pcdata).set_value(enc_AccountNo.c_str());
			child_node = account_node.append_child("account_pwd");
			child_node.append_child(pugi::node_pcdata).set_value(enc_Password.c_str());
		}


		doc.save_file(appPath.c_str());

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}


void SmSaveManager::LoadAccountPasswords()
{
	try {
		

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));

		appPath.append(_T("acnt_pwd.xml"));


		std::string file_name = appPath;
		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
		}

		pugi::xml_node application = doc.child("application");
		pugi::xml_node account_list_node = application.child("account_list");
		if (account_list_node) {
			for (pugi::xml_node account_node = account_list_node.child("account"); account_node; account_node = account_node.next_sibling("account")) {
				std::string dec_AccountNo, dec_Password;
				dec_AccountNo = account_node.child_value("account_no");
				dec_Password = account_node.child_value("account_pwd");

				std::string account_no = cryptor::decrypt(dec_AccountNo);
				std::string pwd = cryptor::decrypt(dec_Password);

				mainApp.AcntMgr()->SetPassword(account_no, pwd);
			}
		}

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmSaveManager::GetSymbolMasters()
{
	
}

void SmSaveManager::SaveSystemOrders()
{
	
}

void SmSaveManager::LoadSystemOrders()
{
	
}

void SmSaveManager::SaveSystemPositions()
{
	
}

void SmSaveManager::LoadSystemPositions()
{
	
}

void DarkHorse::SmSaveManager::ReadSettings()
{
	try {
		
		std::string id = mainApp.LoginMgr()->id();
		// 아이디가 없으면 그냥 반환한다.
		if (id.length() == 0)
			return;

		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append(id);
		appPath.append(_T("\\"));
		std::string config_path = appPath;
		appPath.append(_T("*.*"));
		std::map<std::string, std::string> file_list;
		ListContents(file_list, config_path, "*.xml", false);

		if (file_list.size() == 0)
			return;

		std::string file_name = file_list.rbegin()->second;
		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
		}

		pugi::xml_node application = doc.child("application");
		pugi::xml_node login_info = application.child("login_info");
		if (login_info) {
			mainApp.LoginMgr()->LoadFromXml(login_info);
		}
		//pugi::xml_node account_list = application.child("account_list");
		//if (account_list) {
		//	VtAccountManager::GetInstance()->LoadFromXml(account_list);
		//}

		pugi::xml_node fund_list = application.child("fund_list");
		if (fund_list) {
			mainApp.FundMgr()->LoadFromXml(fund_list);
		}

		/*
		pugi::xml_node order_window_list = application.child("order_window_list");
		if (order_window_list) {
		VtOrderDialogManager::GetInstance()->LoadFromXml(order_window_list);
		}

		pugi::xml_node stratege_window_list = application.child("stratege_window_list");
		if (stratege_window_list) {
		VtStrategyWndManager::GetInstance()->LoadFromXml(stratege_window_list);
		}

		pugi::xml_node etc_window_list = application.child("etc_window_list");
		if (etc_window_list) {
		HdWindowManager::GetInstance()->LoadFromXml(etc_window_list);
		}
		*/
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void DarkHorse::SmSaveManager::ReadLoginInfo()
{
	try {
		std::string appPath;
		appPath = SmConfigManager::GetApplicationPath();
		appPath.append(_T("\\"));
		appPath.append("user");
		//string path_name = "/bin";
		std::map<time_t, fs::directory_entry> sort_by_time;

		//--- sort the files in the map by time
		for (auto& entry : fs::directory_iterator(appPath))
			if (entry.is_directory()) {
				auto time = to_time_t(entry.last_write_time());
				sort_by_time[time] = entry;
			}

		//--- print the files sorted by time
		for (auto const& [time, entry] : sort_by_time) {
			string timestamp = asctime(std::localtime(&time));
			timestamp.pop_back(); // remove automatic linebreak
			//cout << timestamp << "\t " << entry.path().c_str() << endl;
		}

		if (sort_by_time.empty()) return;
		
		//std::string appPath;
		
		std::string config_path = std::prev(sort_by_time.end())->second.path().string();
		config_path.append("\\");
		appPath.append(_T("*.*"));
		std::map<std::string, std::string> file_list;
		ListContents(file_list, config_path, "*.xml", false);

		if (file_list.size() == 0)
			return;

		std::string file_name = file_list.rbegin()->second;
		/// [load xml file]
		// Create empty XML document within memory
		pugi::xml_document doc;
		// Load XML file into memory
		// Remark: to fully read declaration entries you have to specify
		// "pugi::parse_declaration"
		pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
			pugi::parse_default | pugi::parse_declaration);
		if (!result)
		{
			std::cout << "Parse error: " << result.description()
				<< ", character pos= " << result.offset;
		}

		pugi::xml_node application = doc.child("application");
		pugi::xml_node login_info = application.child("login_info");
		if (login_info) {
			mainApp.LoginMgr()->LoadFromXml(login_info);
		}
		

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}
