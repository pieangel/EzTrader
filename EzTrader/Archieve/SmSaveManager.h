#pragma once
//#include "../Global/TemplateSingleton.h"
#include <vector>
#include <string>
#include <fstream>
//#include <windows.h>
#include <map>
#include <set>
class CMainFrame;
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmSaveManager //: public TemplateSingleton<VtSaveManager>
	{
	public:
		SmSaveManager();
		~SmSaveManager();
		void SaveAccountListToXml();
		void SaveFundListToXml();
		void WriteSettings();
		void ReadSettings();
		void ReadLoginInfo();
		void ReadAccountsAndFunds();
		void ReadWindows();
		std::string GetLastestFile(std::string ext);
		void SaveAccountList();
		void SaveFundList();
		void SaveOrders();
		void SaveOrders(std::string fileName);
		void SaveFundList(std::string fileName);
		void LoadFundList(std::string fileName);
		void SaveAccountList(std::string fileName);
		void LoadAccountList(std::string fileName);
		void SaveOrderWndList(std::string fileName, CMainFrame* mainFrm);
		void LoadOrderWndList(std::string fileName, CMainFrame* mainFrm);
		void SaveDialogList(std::string fileName, CMainFrame* mainFrm);
		void LoadDialogList(std::string fileName, CMainFrame* mainFrm);
		void SaveTotal(std::string fileName, CMainFrame* mainFrm);
		void SaveLoginInfo(std::string fileName, std::string id, std::string pwd, std::string cert, bool save);
		void LoadLoginInfo(std::string fileName, std::string& id, std::string& pwd, std::string& cert, bool& save);
		void SaveSystems(std::string fileName);
		void LoadSystems(std::string fileName);
		void SaveOutSystems(std::string fileName);
		void LoadOutSystems(std::string fileName);
		void SaveOutSignal(std::string fileName);
		void LoadOutSignal(std::string fileName);
		void CreateUserDirectory();
		/// <summary>
		/// 주문과 시스템 이름을 저장한다.
		/// </summary>
		void SaveSystemOrders();
		/// <summary>
		/// 주문과 시스템 이름을 불러온다.
		/// </summary>
		void LoadSystemOrders();
		void SaveSystemPositions();
		void LoadSystemPositions();
		
		bool fexists(const std::string& filename) {
			std::ifstream ifile(filename.c_str());
			return (bool)ifile;
		}
		bool IsAccountFileExist();

		void SaveLoginInfoToXml(std::string id, std::string pwd, std::string cert, bool save);
		int LoadLoginInfoFromXml(std::string& id, std::string& pwd, std::string& cert, bool& save);
		int LoadLoginInfoFromXml();
		void LoadRunInfoFromXml();
		void GetWindowSymbolList(std::set<std::string>& symbol_list);
		void SaveAccountPasswords(const std::map<int, std::shared_ptr<DarkHorse::SmAccount>>& account_map);
		void LoadAccountPasswords();
	private:
		bool ListContents(std::map<std::string, std::string>& dest, std::string dir, std::string filter, bool recursively);
		
		void GetSymbolMasters();
	};
}

