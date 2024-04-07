#include "stdafx.h"
#include "VtAccountAssetGrid.h"
#include "VtAssetPage.h"
#include <numeric>

VtAccountAssetGrid::VtAccountAssetGrid()
{
	_AssetPage = nullptr;
}


VtAccountAssetGrid::~VtAccountAssetGrid()
{
}

void VtAccountAssetGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));




	SetRowTitle();

	InitGrid();

	CRect rcWnd;
	GetWindowRect(rcWnd);
	SetWindowPos(nullptr, 0, 0, std::accumulate(_ColWidVec.begin(), _ColWidVec.end(), 0), rcWnd.Height(), SWP_NOMOVE);
}

void VtAccountAssetGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void VtAccountAssetGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void VtAccountAssetGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	LPCTSTR titleLeft[5] = { "예탁총액", "평가손익", "실현손익", "위탁증거금", "유지증거금" };
	LPCTSTR titleRight[5] = { "평가예탁총액", "수수료", "순손익", "주문가능금액", "추가증거금" };
	//SetColWidth(-1, 60);
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	_ColWidVec.push_back(74);
	_ColWidVec.push_back(80);
	_ColWidVec.push_back(74);
	_ColWidVec.push_back(80);
	
}

void VtAccountAssetGrid::QuickRedrawCell(int col, long row)
{
}

void VtAccountAssetGrid::InitGrid()
{
	try
	{
		CString msg;
		msg.Format("VtAccountAssetGrid::InitGrid\n");
		//TRACE(msg);
		/*
		if (!_AssetPage || !_AssetPage->Account())
			return;
		
		VtAccount* acnt = _AssetPage->Account();

		std::string val = NumberFormatter::format(acnt->Deposit);
		CString strDeposit = XFormatNumber(val.c_str(), -1);
		QuickSetTextColor(1, 0, RGB(0, 0, 0));
		QuickSetText(1, 0, strDeposit);

		val = NumberFormatter::format(acnt->OpenDeposit);
		CString strOpenDeposit = XFormatNumber(val.c_str(), -1);
		QuickSetTextColor(3, 0, RGB(0, 0, 0));
		QuickSetText(3, 0, strOpenDeposit);

		std::string temp = NumberFormatter::format(acnt->OpenPL, 0);
		CString profitLoss = XFormatNumber(temp.c_str(), -1);

		if (acnt->OpenPL > 0) {
			QuickSetTextColor(1, 1, RGB(255, 0, 0));
			QuickSetText(1, 1, profitLoss);
		}
		else if (acnt->OpenPL < 0) {
			QuickSetTextColor(1, 1, RGB(0, 0, 255));
			QuickSetText(1, 1, profitLoss);
		}
		else {
			QuickSetTextColor(1, 1, RGB(0, 0, 0));
			QuickSetNumber(1, 1, 0);
		}

		temp = NumberFormatter::format(acnt->TradePL, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		if (acnt->TradePL > 0) {
			QuickSetTextColor(1, 2, RGB(255, 0, 0));
			QuickSetText(1, 2, profitLoss);
		}
		else if (acnt->TradePL < 0) {
			QuickSetTextColor(1, 2, RGB(0, 0, 255));
			QuickSetText(1, 2, profitLoss);
		}
		else {
			QuickSetTextColor(1, 2, RGB(0, 0, 0));
			QuickSetNumber(1, 2, 0);
		}

		temp = NumberFormatter::format(acnt->Fee, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(3, 1, profitLoss);

		temp = NumberFormatter::format(acnt->TotalPL, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		if (acnt->TotalPL > 0) {
			QuickSetTextColor(3, 2, RGB(255, 0, 0));
			QuickSetText(3, 2, profitLoss);
		}
		else if (acnt->TotalPL < 0) {
			QuickSetTextColor(3, 2, RGB(0, 0, 255));
			QuickSetText(3, 2, profitLoss);
		}
		else {
			QuickSetTextColor(3, 2, RGB(0, 0, 0));
			QuickSetNumber(3, 2, 0);
		}

		temp = NumberFormatter::format(acnt->Trst_mgn, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(1, 3, profitLoss);

		temp = NumberFormatter::format(acnt->OrdableAmt, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(3, 3, profitLoss);

		temp = NumberFormatter::format(acnt->Mnt_mgn, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(1, 4, profitLoss);


		temp = NumberFormatter::format(acnt->Add_mgn, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(3, 4, profitLoss);

		for (int i = 0; i < _ColCount; ++i) {
			for (int j = 0; j < _RowCount; ++j) {
				if (i == 1 || i == 3) {
					QuickSetAlignment(i, j, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
					QuickRedrawCell(i, j);
				}
			}
		}
		*/
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}
