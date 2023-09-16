#include "stdafx.h"
#include "VtSignalConnectionGrid.h"
#include "VtGlobal.h"
#include "VtOutSignalDefManager.h"
#include "VtAccount.h"
#include "VtFund.h"
#include "VtAccountFundSelector.h"
#include "HdSymbolSelecter.h"
#include "System/VtSystem.h"
#include "VtSymbol.h"
#include "VtOutSignalDef.h"
#include "VtOutSystemOrderManager.h"
#include "VtTotalSignalGrid.h"
#include "VtOutSignalDefManager.h"
#include "VtSymbol.h"
#include "VtOrderLogDlg.h"
#include "XFormatNumber.h"
#include "Poco/NumberFormatter.h"
using Poco::NumberFormatter;

VtSignalConnectionGrid::VtSignalConnectionGrid()
{
}


VtSignalConnectionGrid::~VtSignalConnectionGrid()
{
}

void VtSignalConnectionGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	//Ellipsis Button cell type
	m_nEllipsisIndex = AddCellType(&m_ellipsis);
	//Spin Button cell type
	m_nSpinIndex = AddCellType(&m_spin);
	//Push Button cell type
	m_nButtonIndex = AddCellType(&m_button);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
	CUGCell cell;
	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(GetSystemMetrics(SM_CXVSCROLL));
	SetHS_Height(0);
	SetColTitle();
	EnableScrollBar(SB_VERT);
	SetVScrollMode(UG_SCROLLNORMAL);
	InitGrid();
}

void VtSignalConnectionGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	if (col == 0 && row == 0)
		RedrawAll();
}

void VtSignalConnectionGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_ClickedRow == row)
		return;

	if (_ClickedRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _ClickedRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _ClickedRow);
		}
	}
	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, row, _ClickedColor);
		QuickRedrawCell(i, row);
	}
	_ClickedRow = row;
	if (updn == 1) {
		OnLogButton(col, row);
	}
}

void VtSignalConnectionGrid::OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_ClickedRow == row)
		return;

	if (_ClickedRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _ClickedRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _ClickedRow);
		}
	}
	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, row, _ClickedColor);
		QuickRedrawCell(i, row);
	}
	_ClickedRow = row;
}

void VtSignalConnectionGrid::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed /*= 0*/)
{
	if (_OldSelRow == row)
		return;

	if (_OldSelRow != _ClickedRow && _OldSelRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _OldSelRow);
		}
	}

	if (row != _ClickedRow) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, row, _SelColor);
			QuickRedrawCell(i, row);
		}
	}
	else {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, row, _ClickedColor);
			QuickRedrawCell(i, row);
		}
	}

	_OldSelRow = row;
}

int VtSignalConnectionGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param) {
	CString log;
	log.Format("OnCellTypeNotify col = %d, row = %d, msg = %d, param = %d \n", col, row, msg, param);
	TRACE(log);

	if (ID == UGCT_DROPLIST) {
		return OnDropList(ID, col, row, msg, param);
	}
	if (ID == UGCT_CHECKBOX) {
		return OnCheckbox(ID, col, row, msg, param);
	}
	if (ID == m_nEllipsisIndex) {
		return OnEllipsisButton(ID, col, row, msg, param);
	}
	if (ID == m_nSpinIndex) {
		return OnSpinButton(ID, col, row, msg, param);
	}

	if (ID == m_nButtonIndex) {
		return OnButton(ID, col, row, msg, param);
	}
	
	return TRUE;
}


void VtSignalConnectionGrid::OnMouseLeaveFromMainGrid()
{
	if (_OldSelRow == _ClickedRow)
		return;

	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
		QuickRedrawCell(i, _OldSelRow);
	}

	_OldSelRow = -2;
}

void VtSignalConnectionGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[9] = { "실행", "계좌번호", "종목", "신호차트", "승수", "평가손익", "청산손익", "총손익", "로그"};
	int colWidth[9] = { 25, 98, 90, 80, 58, 100, 100, 100, 85 };


	for (int i = 0; i < _ColCount; i++) {
		SetColWidth(i, colWidth[i]);
		GetCell(i, -1, &cell);
		cell.SetText(title[i]);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetFont(&_titleFont);
		SetCell(i, -1, &cell);
		QuickRedrawCell(i, -1);
	}
}

void VtSignalConnectionGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtSignalConnectionGrid::InitGrid()
{
	try {
	_SystemMap.clear();
	_SystemToRowMap.clear();
	int yIndex = 0;
	CUGCell cell;
	VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
	VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
	SharedSystemVec& outSysMap = outSysMgr->GetSysMap();
	for (auto it = outSysMap.begin(); it != outSysMap.end(); ++it) {
		SharedSystem sys = *it;
		VtPosition posi = sys->GetPosition();
		CString thVal;
		std::string temp;
		temp = NumberFormatter::format(posi.OpenProfitLoss, 0);
		CString profitLoss = XFormatNumber(temp.c_str(), -1);
		for (int xIndex = 0; xIndex < _ColCount; ++xIndex) {
			if (xIndex == 0) {
				GetCell(xIndex, yIndex, &cell);
				sys->Enable() ? cell.SetNumber(1.0) : cell.SetNumber(0.0);
				cell.SetLabelText(_T(""));
				cell.SetCellType(UGCT_CHECKBOX);
				cell.SetCellTypeEx(UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 1) {
				GetCell(xIndex, yIndex, &cell);
				if (sys->SysTargetType() == TargetType::RealAccount || sys->SysTargetType() == TargetType::SubAccount) {
					if (sys->Account()) cell.SetText(sys->Account()->AccountNo.c_str());
				}
				else {
					if (sys->Fund()) cell.SetText(sys->Fund()->Name.c_str());
				}
				cell.SetCellType(m_nEllipsisIndex);
				cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
				cell.SetParam(ELLIPSISBUTTON_CLICK_ACNT);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 2) {
				GetCell(xIndex, yIndex, &cell);
				if (sys->Symbol()) cell.SetText(sys->Symbol()->ShortCode.c_str());
				cell.SetCellType(m_nEllipsisIndex);
				cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
				cell.SetParam(ELLIPSISBUTTON_CLICK_PRDT);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 3) {
				GetCell(xIndex, yIndex, &cell);
				if (sys->OutSignal())  cell.SetText(sys->OutSignal()->SignalName.c_str());
				cell.SetCellType(UGCT_DROPLIST);
				cell.SetCellTypeEx(UGCT_DROPLISTHIDEBUTTON);
				cell.SetReadOnly(FALSE);
				cell.SetLabelText(outSigDefMgr->GetOutSigDefString().c_str());
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 4) {
				GetCell(xIndex, yIndex, &cell);
				cell.SetNumber(sys->SeungSu());
				cell.SetCellType(m_nSpinIndex);
				cell.SetParam(SPIN_TYPE_SEUNGSU);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 5) {
				temp = NumberFormatter::format(posi.OpenProfitLoss, 0);
				CString profitLoss = XFormatNumber(temp.c_str(), -1);
				// 평가손익 표시
				if (posi.OpenProfitLoss > 0) {
					QuickSetTextColor(5, yIndex, RGB(255, 0, 0));
					QuickSetText(5, yIndex, profitLoss);
				}
				else if (posi.OpenProfitLoss < 0) {
					QuickSetTextColor(5, yIndex, RGB(0, 0, 255));
					QuickSetText(5, yIndex, profitLoss);
				}
				else {
					QuickSetTextColor(5, yIndex, RGB(0, 0, 0));
					QuickSetNumber(5, yIndex, 0);
				}
			}
			else if (xIndex == 6) {
				temp = NumberFormatter::format(posi.TradePL, 0);
				profitLoss = XFormatNumber(temp.c_str(), -1);
				// 실현손익 표시
				if (posi.TradePL > 0) {
					QuickSetTextColor(6, yIndex, RGB(255, 0, 0));
					QuickSetText(6, yIndex, profitLoss);
				}
				else if (posi.TradePL < 0) {
					QuickSetTextColor(6, yIndex, RGB(0, 0, 255));
					QuickSetText(6, yIndex, profitLoss);
				}
				else {
					QuickSetTextColor(6, yIndex, RGB(0, 0, 0));
					QuickSetNumber(6, yIndex, 0);
				}
			}
			else if (xIndex == 7) {
				double totalPL = posi.OpenProfitLoss + posi.TradePL;

				temp = NumberFormatter::format(totalPL, 0);
				profitLoss = XFormatNumber(temp.c_str(), -1);

				// 총손익 표시
				if (totalPL > 0) {
					QuickSetTextColor(7, yIndex, RGB(255, 0, 0));
					QuickSetText(7, yIndex, profitLoss);
				}
				else if (totalPL < 0) {
					QuickSetTextColor(7, yIndex, RGB(0, 0, 255));
					QuickSetText(7, yIndex, profitLoss);
				}
				else {
					QuickSetTextColor(7, yIndex, RGB(0, 0, 0));
					QuickSetNumber(7, yIndex, 0);
				}
			}
			else if (xIndex == 8) {
				GetCell(xIndex, yIndex, &cell);
				cell.SetNumber(sys->SeungSu());
				cell.SetCellType(m_nButtonIndex);
				cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
				cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
				cell.SetFont(&_titleFont);
				cell.SetTextColor(RGB(0, 0, 0));
				cell.SetBackColor(RGB(218, 226, 245));
				cell.SetText("로그보기");
				SetCell(xIndex, yIndex, &cell);
			}
			QuickRedrawCell(xIndex, yIndex);			
		}
		_SystemMap[yIndex] = sys;
		_SystemToRowMap[sys->Id()] = yIndex;
		yIndex++;
	}
	_OccupiedRowCount = yIndex;

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::ClearCells()
{
	CUGCell cell;
	for (int i = 0; i < _RowCount; i++) {
		for (int j = 0; j < _ColCount; j++) {
			QuickSetText(j, i, _T(""));
			QuickSetBackColor(j, i, RGB(255, 255, 255));
			QuickRedrawCell(j, i);
			GetCell(j, i, &cell);
			cell.Tag(nullptr);
			cell.SetCellType(UGCT_NORMAL);
			SetCell(j, i, &cell);
		}
	}
}

void VtSignalConnectionGrid::SetTargetAcntOrFund(std::tuple<int, VtAccount*, VtFund*>& selItem)
{
	try {
	CUGCell cell;
	GetCell(1, _ButtonRow, &cell);
	int _Type = std::get<0>(selItem);
	SharedSystem sys = _SystemMap[_ButtonRow];
	cell.LongValue(_Type);
	if (_Type == 0 || _Type == 1) {
		VtAccount* _Account = std::get<1>(selItem);
		cell.SetText(_Account->AccountNo.c_str());
		cell.Tag(_Account);
		if (sys) {
			sys->Account(_Account);
		}
	}
	else {
		VtFund* _Fund = std::get<2>(selItem);
		cell.SetText(_Fund->Name.c_str());
		cell.Tag(_Fund);
		if (sys) {
			sys->Fund(_Fund);
		}
	}
	SetCell(1, _ButtonRow, &cell);
	QuickRedrawCell(1, _ButtonRow);

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::SetSymbol(VtSymbol* sym)
{
	try {
	if (!sym)
		return;
	SharedSystem sys = _SystemMap[_ButtonRow];
	if (sys) {
		CUGCell cell;
		GetCell(2, _ButtonRow, &cell);
		cell.SetText(sym->ShortCode.c_str());
		cell.Tag(sym);
		SetCell(2, _ButtonRow, &cell);
		sys->Symbol(sym);
		QuickRedrawCell(2, _ButtonRow);
	}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::AddSystem(SharedSystem sys)
{
	try {
	VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
	VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
	CUGCell cell;
	int yIndex = outSysMgr->GetSysMap().size();
	for (int xIndex = 0; xIndex < _ColCount; ++xIndex) {
		if (xIndex == 0) {
			GetCell(xIndex, yIndex, &cell);
			sys->Enable() ? cell.SetNumber(1.0) : cell.SetNumber(0.0);
			cell.SetLabelText(_T(""));
			cell.SetCellType(UGCT_CHECKBOX);
			cell.SetCellTypeEx(UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 1) {
			GetCell(xIndex, yIndex, &cell);
			if (sys->SysTargetType() == TargetType::RealAccount || sys->SysTargetType() == TargetType::SubAccount) {
				if (sys->Account()) cell.SetText(sys->Account()->AccountNo.c_str());
			}
			else {
				if (sys->Fund()) cell.SetText(sys->Fund()->Name.c_str());
			}
			cell.SetCellType(m_nEllipsisIndex);
			cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
			cell.SetParam(ELLIPSISBUTTON_CLICK_ACNT);
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 2) {
			GetCell(xIndex, yIndex, &cell);
			if (sys->Symbol()) cell.SetText(sys->Symbol()->ShortCode.c_str());
			cell.SetCellType(m_nEllipsisIndex);
			cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
			cell.SetParam(ELLIPSISBUTTON_CLICK_PRDT);
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 3) {
			GetCell(xIndex, yIndex, &cell);
			if (sys->OutSignal())  cell.SetText(sys->OutSignal()->SignalName.c_str());
			cell.SetCellType(UGCT_DROPLIST);
			cell.SetCellTypeEx(UGCT_DROPLISTHIDEBUTTON);
			cell.SetReadOnly(FALSE);
			cell.SetLabelText(outSigDefMgr->GetOutSigDefString().c_str());
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 4) {
			GetCell(xIndex, yIndex, &cell);
			cell.SetNumber(sys->SeungSu());
			cell.SetCellType(m_nSpinIndex);
			cell.SetParam(SPIN_TYPE_SEUNGSU);
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 8) {
			GetCell(xIndex, yIndex, &cell);
			cell.SetNumber(sys->SeungSu());
			cell.SetCellType(m_nButtonIndex);
			cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
			cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
			cell.SetFont(&_titleFont);
			cell.SetTextColor(RGB(0, 0, 0));
			cell.SetBackColor(RGB(218, 226, 245));
			cell.SetText("로그보기");
			SetCell(xIndex, yIndex, &cell);
		}
		QuickRedrawCell(xIndex, yIndex);
	}
	_SystemMap[yIndex] = sys;
	outSysMgr->AddSystem(sys);
	_OccupiedRowCount = yIndex;

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::RemoveSystem()
{
	try {
	int row = _ClickedRow;
	auto it = _SystemMap.find(row);
	if (it != _SystemMap.end()) {
		SharedSystem sys = it->second;
		// 시스템을 정지 시킨다.
		sys->Enable(false);
		VtOutSystemOrderManager* outSysOrderMgr = VtOutSystemOrderManager::GetInstance();
		// 주문관리자에서 삭제한다.
		outSysOrderMgr->RemoveSystem(sys);
		// 주문상태 목록을 리프레쉬 한다.
		if (_TotalGrid) _TotalGrid->Refresh();

		// 시스템 목록에서 삭제한다.
		VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
		outSysMgr->RemoveSystem(sys->Id());

		// 모든 셀 정보를 초기화 시킨다.
		CUGCell cell;
		for (int i = 0; i < _ColCount; ++i) {
			GetCell(i, row, &cell);
			cell.SetCellType(UGCT_NORMAL);
			cell.SetText(_T(""));
			SetCell(i, row, &cell);
			QuickRedrawCell(i, row);
		}
	}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::Refresh()
{
	ClearCells();
	InitGrid();
}

void VtSignalConnectionGrid::SetCheck(bool flag)
{
	try {
	_TotalGrid->ClearCells();
	int row = 0;
	CUGCell cell;
	VtOutSystemOrderManager* outSysOrderMgr = VtOutSystemOrderManager::GetInstance();
	for (auto it = _SystemMap.begin(); it != _SystemMap.end(); ++it) {
		SharedSystem sys = _SystemMap[row];
		GetCell(0, row, &cell);
		if (flag) {
			cell.SetNumber(1);
			sys->Enable(true);
			outSysOrderMgr->AddSystem(sys);
		}
		else {
			cell.SetNumber(0);
			sys->Enable(false);
			outSysOrderMgr->RemoveSystem(sys);
		}
		SetCell(0, row, &cell);
		QuickRedrawCell(0, row);
		row++;
	}
	if (_TotalGrid) _TotalGrid->Refresh();

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::RefreshOrders()
{
	try {
	if (_OccupiedRowCount == 0)
		return;

	int i = 0;
	for (auto it = _SystemMap.begin(); it != _SystemMap.end(); ++it) {
		SharedSystem sys = it->second;
	
		
		VtPosition posi = sys->GetPosition();

		CString thVal;
		std::string temp;
	
		temp = NumberFormatter::format(posi.OpenProfitLoss, 0);
		CString profitLoss = XFormatNumber(temp.c_str(), -1);
		// 평가손익 표시
		if (posi.OpenProfitLoss > 0) {
			QuickSetTextColor(5, i, RGB(255, 0, 0));
			QuickSetText(5, i, profitLoss);
		}
		else if (posi.OpenProfitLoss < 0) {
			QuickSetTextColor(5, i, RGB(0, 0, 255));
			QuickSetText(5, i, profitLoss);
		}
		else {
			QuickSetTextColor(5, i, RGB(0, 0, 0));
			QuickSetNumber(5, i, 0);
		}

		temp = NumberFormatter::format(posi.TradePL, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);
		// 실현손익 표시
		if (posi.TradePL > 0) {
			QuickSetTextColor(6, i, RGB(255, 0, 0));
			QuickSetText(6, i, profitLoss);
		}
		else if (posi.TradePL < 0) {
			QuickSetTextColor(6, i, RGB(0, 0, 255));
			QuickSetText(6, i, profitLoss);
		}
		else {
			QuickSetTextColor(6, i, RGB(0, 0, 0));
			QuickSetNumber(6, i, 0);
		}

		double totalPL = posi.OpenProfitLoss + posi.TradePL;

		temp = NumberFormatter::format(totalPL, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		// 총손익 표시
		if (totalPL > 0) {
			QuickSetTextColor(7, i, RGB(255, 0, 0));
			QuickSetText(7, i, profitLoss);
		}
		else if (totalPL < 0) {
			QuickSetTextColor(7, i, RGB(0, 0, 255));
			QuickSetText(7, i, profitLoss);
		}
		else {
			QuickSetTextColor(7, i, RGB(0, 0, 0));
			QuickSetNumber(7, i, 0);
		}
		
		
		for (int j = 5; j < 8; ++j) {
			QuickRedrawCell(j, i);
		}
		// 인덱스를 증가 시킨다.
		i++;
	}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::ClearCheck(VtSystem* sys)
{
	if (!sys)
		return;
	auto it = _SystemToRowMap.find(sys->Id());
	if (it != _SystemToRowMap.end()) {
		int row = it->second;
		CUGCell cell;
		GetCell(0, row, &cell);
		cell.SetNumber(0);
		sys->Enable(false);
		SetCell(0, row, &cell);
		QuickRedrawCell(0, row);
	}
}

int VtSignalConnectionGrid::OnDropList(long ID, int col, long row, long msg, long param)
{
	try {
	if (msg == UGCT_DROPLISTSTART) {
		CStringList* pList = (CStringList*)param;
		pList->RemoveAll();
		VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
		OutSigDefVec& sigDefVec = outSigDefMgr->GetSignalDefVec();
		for (size_t loop = 0; loop < sigDefVec.size(); loop++) {
			SharedOutSigDef sig = sigDefVec[loop];
			pList->AddTail(sig->SignalName.c_str());
		}
	}
	if (msg == UGCT_DROPLISTSELECT) {
		CUGCell cell;
		GetCell(col, row, &cell);
		CString oldSigName;
		oldSigName = cell.GetText();
		SharedSystem sys = _SystemMap[row];
		if (!sys)
			return TRUE;

		VtOutSystemOrderManager* outSysOrderMgr = VtOutSystemOrderManager::GetInstance();
		// 먼저 기존 시그널을 없애 준다.
		if (sys->Enable())
			outSysOrderMgr->RemoveSystem(sys);

		// 새로운 시그널을 등록해 준다.
		CString * pString = (CString*)param;
		std::string sigName = *pString;
		VtOutSignalDefManager* outSigMgr = VtOutSignalDefManager::GetInstance();
		SharedOutSigDef sig = outSigMgr->FindOutSigDef(sigName);
		if (sig) {
			sys->OutSignal(sig);
			CUGCell cell;
			GetCell(col, row, &cell);
			cell.SetText(sig->SignalName.c_str());
			QuickRedrawCell(col, row);
			if (sys->Enable())
				outSysOrderMgr->AddSystem(sys);
		}
	}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
	
	return TRUE;
}
// 체크박스에 체크하면 시스템 주문 관리자에 등록해 준다.
int VtSignalConnectionGrid::OnCheckbox(long ID, int col, long row, long msg, long param)
{
	try {
		VtOutSystemOrderManager* outSysOrderMgr = VtOutSystemOrderManager::GetInstance();
		SharedSystem sys = _SystemMap[row];
		if (sys) {
			CUGCell cell;
			GetCell(col, row, &cell);
			double num = cell.GetNumber();
			if (num == 1.0) {
				sys->Enable(true);
				outSysOrderMgr->AddSystem(sys);
			}
			else {
				sys->Enable(false);
				outSysOrderMgr->RemoveSystem(sys);
			}
			if (_TotalGrid) _TotalGrid->Refresh();
		}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}

	return TRUE;
}

int VtSignalConnectionGrid::OnEllipsisButton(long ID, int col, long row, long msg, long param)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	int nCellTypeIndex = cell.GetCellType();
	int nParam = cell.GetParam();

	if (msg == UGCT_ELLIPSISBUTTONCLICK) {
		if (nParam == ELLIPSISBUTTON_CLICK_ACNT) {
			_ButtonRow = row;
			VtAccountFundSelector dlg;
			dlg.SetSignalConnectionGrid(this);
			dlg.DoModal();
		}
		else if (nParam == ELLIPSISBUTTON_CLICK_PRDT) {
			_ButtonRow = row;
			HdSymbolSelecter dlg;
			dlg.SetSigConGrid(this);
			dlg.DoModal();
		}
		else {
			MessageBox("The button was clicked", "Cell Type Notification");
		}
	}

	return TRUE;
}

int VtSignalConnectionGrid::OnSpinButton(long ID, int col, long row, long msg, long param)
{
	long num;
	CUGCell cell;
	GetCell(col, row, &cell);
	cell.GetNumber(&num);

	if (msg == UGCT_SPINBUTTONUP) {
		num++;
	}
	if (msg == UGCT_SPINBUTTONDOWN) {
		num--;
		num == 0 ? num = 1 : num = num;
	}
	CString str;
	str.Format("%ld", num);
	cell.SetText(str);
	SetCell(col, row, &cell);

	SharedSystem sys = _SystemMap[row];
	sys->SeungSu((int)num);

	return TRUE;
}

int VtSignalConnectionGrid::OnLogButton(int col, long row)
{
	if (col != 8)
		return -1;

	
	

	return 1;
}

int VtSignalConnectionGrid::OnButton(long ID, int col, long row, long msg, long param)
{
	if (msg != 1)
		return -1;

	CString log;
	log.Format("OnButton col = %d, row = %d, msg = %d, param = %d \n", col, row, msg, param);
	TRACE(log);
	try {
	if (VtOutSystemOrderManager::GetInstance()->GetLogMapSize() == 0)
		return -1;

	SharedSystem sys = _SystemMap[row];
	if (sys) {
		_LogDlg = new VtOrderLogDlg();
		_LogDlg->Create(IDD_ORDER_LOG, this);
		std::vector<std::pair<std::string, std::string>>& log_vec = VtOutSystemOrderManager::GetInstance()->GetOrderLog(sys);
		_LogDlg->UpdateOrderLog2(log_vec);
		_LogDlg->ShowWindow(SW_SHOW);
	}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}

	return 1;
}
