#include "stdafx.h"
#include "SmOptionGrid.h"
//#include "../Symbol/VtSymbol.h"
#include "VtOrderConfigManager.h"
#include "VtOrderLeftWndHd.h"
//#include "VtOptionMonthSection.h"
//#include "../Symbol/VtSymbolManager.h"
//#include "../Position/VtPosition.h"
//#include "../Account/VtAccount.h"
//#include "../Order/VtOrder.h"
//#include "../Order/VtOrderManager.h"
//#include "../Order/VtOrderManagerSelector.h"
//#include "../Order/VtProductOrderManagerSelector.h"
//#include "../Order/VtProductOrderManager.h"
//#include "../Fund/VtFund.h"
//#include "../Symbol/VtRealtimeRegisterManager.h"
//#include "../Global/MainBeetle.h"
#include "SmOrderPanel.h"
//#include "../Task/SmCallbackManager.h"
//#include "../Symbol/SmMarketManager.h"
#include <functional>
#include "../Symbol/SmProductYearMonth.h"
#include "../Symbol/SmProduct.h"
//#include "../Format/format.h"
#include "../Global/SmTotalManager.h"
#include "../MessageDefine.h"

#pragma warning(disable:4018)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;
using namespace std::placeholders;

SmOptionGrid::SmOptionGrid()
{
}


SmOptionGrid::~SmOptionGrid()
{
}

void SmOptionGrid::RegisterMasterCallback()
{
	//mainApp.CallbackMgr().SubscribeMasterCallback((long)this, std::bind(&SmOptionGrid::OnMasterEvent, this, _1));
}

void SmOptionGrid::UnregisterAllCallback()
{
	//mainApp.CallbackMgr().UnsubscribeOrderCallback((long)this);
	//mainApp.CallbackMgr().UnsubscribeMasterCallback((long)this);
	//mainApp.CallbackMgr().UnsubscribeQuoteWndCallback(GetSafeHwnd());
}

void SmOptionGrid::RegisterQuoteCallback()
{
	//mainApp.CallbackMgr().SubscribeQuoteWndCallback(GetSafeHwnd());
}

void SmOptionGrid::RegisterOrderCallback()
{
	//mainApp.CallbackMgr().SubscribeOrderCallback((long)this, std::bind(&SmOptionGrid::OnOrderEvent, this, _1));
}

void SmOptionGrid::OnMasterEvent(symbol_p sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 1) {
		ShowCurrent(sym);
	}
}

void SmOptionGrid::OnOrderEvent(VtOrder* order)
{
	try {
		CString msg;
		//msg.Format("OnOrderEvent code = %s\n", order->shortCode.c_str());
		//TRACE(msg);
		if (!_OrderConfigMgr || !order)
			return;
		// �ܰ� ��尡 �ƴϸ� �������� ����
		if (_Mode != 0)
			return;

		// ü�� �̺�Ʈ
		/*
		if (order->state == VtOrderState::Filled) {
			if (_OrderConfigMgr->Type() == 0) {
				VtAccount* acnt = _OrderConfigMgr->Account();
				if (!acnt)
					return;

				VtPosition* posi = acnt->FindPosition(order->shortCode);
				if (posi)
					SetRemain(posi);
			}
			else {
				VtFund* fund = _OrderConfigMgr->Fund();
				if (!fund)
					return;
				int count = 0;
				VtPosition posi = fund->GetPosition(order->shortCode, count);
				SetRemain(&posi);
			}
		}
		else { // ������ �ֹ� �̺�Ʈ 
			symbol_p symbol = mainApp.SymbolMgr().FindSymbol(order->shortCode);
			if (symbol) {
				SetRemain(symbol);
			}
		}
		*/
	} 
	catch (...)
	{
		auto expPtr = std::current_exception();
		try
		{
			if (expPtr) std::rethrow_exception(expPtr);
		}
		catch (const std::exception& e) //it would not work if you pass by value
		{
			std::string msg = e.what();
		}
	}
}

void SmOptionGrid::OnQuoteEvent(symbol_p sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 1) {
		ShowCurrent(sym);
	}
}

BEGIN_MESSAGE_MAP(SmOptionGrid, CGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_QUOTE_CHANGED, OnQuoteChangedMessage)
END_MESSAGE_MAP()

void SmOptionGrid::Init()
{
	_EqualIndex = -1;
	_EqualCell.row = -1;
	_EqualCell.col = -1;
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"));

	SetRowCount(_RowCount);
	SetColumnCount(_ColCount);
	SetFixedRowCount(0);
	SetFixedColumnCount(0);
	SetColTitle();
	EnableScrollBars(SB_VERT, FALSE);
	EnableScrollBars(SB_HORZ, FALSE);
	SetFixedColumnSelection(FALSE);
	SetFixedRowSelection(FALSE);
	SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	SetDoubleBuffering(TRUE);
	EnableSelection(FALSE);
	SetEditable(FALSE);
	EnableTitleTips(FALSE);
	SetColumnResize(FALSE);
	SetRowResize(FALSE);
	AllowReorderColumn(false);

	for (int i = 0; i < _RowCount; i++) {
		SetRowHeight(i, _CellHeight);
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				// �ؽ�Ʈ ����
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				// �� ��� �� ����
				pCell->SetBackClr(RGB(255, 255, 255));
				// �� ���ڻ� ����
				pCell->SetTextClr(RGB(0, 0, 0));

				InvalidateCellRect(i, j);
			}
		}
	}

	SetFont(&_defFont);
	RegisterQuoteCallback();
	RegisterMasterCallback();
	RegisterOrderCallback();

	SetColTitle();

	//_RunInfo = mainApp.SymbolMgr().MrktMgr().GetOptionRunVector();

	InitSymbol();
	InitYearMonth();
}

void SmOptionGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	CCellID cell = GetCellFromPt(point);
	CGridCellBase* pCell = GetCell(cell.row, cell.col);
	/*
	symbol_p sym = (symbol_p)pCell->GetData();
	if (sym) {
		sym->GetSymbolMaster();
		if (_OrderConfigMgr->_HdCenterWnd) {
			_OrderConfigMgr->Symbol(sym);
			_OrderConfigMgr->_HdCenterWnd->ChangeSymbol(sym);
		}
	}
	*/
}

void SmOptionGrid::OnRButtonDown(UINT nFlags, CPoint point)
{
	return;
}

BOOL SmOptionGrid::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int distance = (zDelta / 60);
	if (abs(zDelta) > 120)
		distance = zDelta / 120;
	else
		distance = zDelta / 40;
	_ValueStartRow = _ValueStartRow + -1 * (distance);
	if (_ValueStartRow > _MaxIndex - (_MaxRow - 2))
		_ValueStartRow = _MaxIndex - _MaxRow + 2;
	if (_ValueStartRow < 0)
		_ValueStartRow = 0;

	
	// �������� �����Ѵ�.
	_SymbolRowMap.clear();
	// ���ο� ���� �����.
	MakeSymbolRowMap(_ValueStartRow, _ValueMaxRow);

	if (_Mode == 0)
		SetRemain2();
	else if (_Mode == 1)
		SetCurrent2();
	else
		SetExpected2();
	
	//InitGrid();
	return CGridCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

LRESULT SmOptionGrid::OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam)
{
	symbol_p* pData = reinterpret_cast<symbol_p*>(lParam);
	symbol_p symbol = *pData;
	
	if (_Mode == 0) {
		SetRemain(symbol);
	}
	else if (_Mode == 1) {
		ShowCurrent(symbol);
	}

	return 1;
}

LRESULT SmOptionGrid::OnOrderChangedMessage(WPARAM wParam, LPARAM lParam)
{
	return 1;
}

void SmOptionGrid::SetColTitle()
{
	LPCTSTR title[3] = { "Call", "��簡", "Put" };
	int colWidth[9] = { 60, 50, 60 };

	for (int i = 0; i < _ColCount; i++)
	{
		SetColumnWidth(i, colWidth[i]);
		CGridCellBase* pCell = GetCell(0, i);
		pCell->SetText(title[i]);
		if (i == 0) {
			pCell->SetTextClr(RGB(255, 255, 255));
			pCell->SetBackClr(RGB(240, 51, 58));
		}
		else if (i == 2) {
			pCell->SetTextClr(RGB(255, 255, 255));
			pCell->SetBackClr(RGB(19, 137, 255));
		}
		else {
			pCell->SetTextClr(DarkHorse::SmTotalManager::GridTitleBackColor);
			pCell->SetBackClr(DarkHorse::SmTotalManager::GridTitleTextColor);
		}
		InvalidateCellRect(0, i);
	}
}

void SmOptionGrid::InitGrid()
{
	CRect rcGrid;
	GetClientRect(rcGrid);
	InitGrid(rcGrid.Height());
}

void SmOptionGrid::InitGrid(int height)
{
	std::pair<int, int> start_max = FindValueStartRow(height);
	_ValueStartRow = start_max.first;
	_ValueMaxRow = start_max.second;
	if (start_max.first == 0 && start_max.second == 0)
		return;
	_RowCount = start_max.second;
	SetRowCount(_RowCount);
	// ���� �ɼ��� �� ǥ���ϰ��� ������ �׸��带 �ٿ��ش�.
	if (_RowCount + 1 > _MaxIndex) {
		int total_cell_height = _CellHeight * (_MaxIndex + 2);
		CRect rcWnd;
		GetWindowRect(&rcWnd);
		SetWindowPos(nullptr, 0, 0, rcWnd.Width(), total_cell_height + 4, SWP_NOMOVE);
	}

	for (int i = 1; i < _RowCount; i++) {
		SetRowHeight(i, _CellHeight);
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				// �ؽ�Ʈ ����
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				InvalidateCellRect(i, j);
			}
		}
	}

	// �������� �����Ѵ�.
	_SymbolRowMap.clear();
	// ��� �ؽ�Ʈ�� �����.
	ClearAllText();
	// ���ο� ���� �����.
	MakeSymbolRowMap(start_max.first, start_max.second);

	
	if (_Mode == 0)
		SetRemain2();
	else if (_Mode == 1)
		SetCurrent2();
	else
		SetExpected2();
}

void SmOptionGrid::InitSymbol()
{
	if (!_LeftWnd)
		return;
	/*
	for (auto it = _RunInfo.begin(); it != _RunInfo.end(); ++it) {
		SmRunInfo& run_info = *it;
		int index = _LeftWnd->_ComboProduct.AddString(run_info.Name.c_str());
	}
	*/

	_LeftWnd->_ComboProduct.SetCurSel(0);
	_SelectedProduct = 0;
}

void SmOptionGrid::InitYearMonth()
{
	if (!_LeftWnd)
		return;
	
	SetYearMonth();
	_LeftWnd->_ComboOptionMonth.SetCurSel(0);
}

void SmOptionGrid::SetYearMonth()
{
	int cur_sel = _LeftWnd->_ComboProduct.GetCurSel();
	if (cur_sel < 0)
		return;
	/*
	SmProduct* product = mainApp.SymbolMgr().MrktMgr().FindProductFromMap(_RunInfo[cur_sel].CallCode);
	if (!product)
		return;
	_LeftWnd->_ComboOptionMonth.ResetContent();

	std::map<std::string, SmProductYearMonth*>& year_month_map = product->GetYearMonthMap();
	for (auto it = year_month_map.begin(); it != year_month_map.end(); ++it) {
		SmProductYearMonth* year_month = it->second;

		std::string code = it->first;

		CString yearMon(code.c_str());
		int index = _LeftWnd->_ComboOptionMonth.AddString(yearMon);
	}
	*/

	_LeftWnd->_ComboOptionMonth.SetCurSel(0);
}

void SmOptionGrid::SetProductSection()
{
	if (!_LeftWnd)
		return;
	_SelectedProduct = _LeftWnd->_ComboProduct.GetCurSel();
}

void SmOptionGrid::GetSymbolMaster()
{
	if (_EqualIndex == -1)
		return;

	int selMon = _LeftWnd->_ComboOptionMonth.GetCurSel();
	/*
	std::pair<SmProduct*, SmProduct*> product_pair = mainApp.SymbolMgr().MrktMgr().GetProductPair(_RunInfo[_SelectedProduct]);
	if (!product_pair.first || !product_pair.second)
		return;

	CString curYearMon;
	_LeftWnd->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	SmProductYearMonth* call_year_month = product_pair.first->GetYearMonth((LPCTSTR)curYearMon);
	SmProductYearMonth* put_year_month = product_pair.second->GetYearMonth((LPCTSTR)curYearMon);
	if (!call_year_month || !put_year_month)
		return;
	
	if (call_year_month && put_year_month) {
		std::vector<symbol_p> call_symbol_list = call_year_month->SymbolList();
		std::vector<symbol_p> put_symbol_list = put_year_month->SymbolList();
		size_t startIndex = 0;
		size_t endIndex = call_symbol_list.size() - 1;
		int addNum = 0;
		size_t curIndex = _EqualIndex;
		bool upRange = false, downRange = false;
		while (1) {
			if (addNum % 2 == 0)
				curIndex = curIndex + addNum;
			else
				curIndex = curIndex - addNum;

			if (curIndex < startIndex)
				upRange = true;
			if (curIndex > endIndex)
				downRange = true;

			if (curIndex >= startIndex && curIndex <= endIndex) {
				symbol_p sym =nullptr;
				if (curIndex < call_symbol_list.size()) {
					sym = call_symbol_list[curIndex];
					if (sym->Quote.intClose == 0) {
						sym->GetSymbolMaster();
						//VtRealtimeRegisterManager::GetInstance()->RegisterProduct(sym->ShortCode);
					}
				}
		
				if (curIndex < put_symbol_list.size()) {
					sym = put_symbol_list[curIndex];
					if (sym->Quote.intClose == 0) {
						sym->GetSymbolMaster();
						//VtRealtimeRegisterManager::GetInstance()->RegisterProduct(sym->ShortCode);
					}
				}
			}

			if (upRange && downRange)
				break;

			addNum++;
		}
	}
	*/
}

void SmOptionGrid::RefreshMode()
{
	ResetRemainCells();
	if (_Mode == 0)
		SetRemain2();
	else if (_Mode == 1)
		SetCurrent2();
	else
		SetExpected2();
}

std::pair<int, int> SmOptionGrid::FindValueStartRow(int height)
{
	int selMon = _LeftWnd->_ComboOptionMonth.GetCurSel();
	if (selMon == -1)
		return std::make_pair(0, 0);

	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;
	int intCenter = 0;
	int eCenter = 0;
	// ���� ǥ���ϴ� ���� �ε���
	int start_index = 0;
	int max_row = height / 21;// DefaultCellHeight;
	_MaxRow = max_row;
	CString curYearMon;
	_LeftWnd->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	/*
	SmProduct* product = mainApp.SymbolMgr().MrktMgr().FindProductFromMap(_RunInfo[_SelectedProduct].CallCode);
	if (!product)
		return std::make_pair(0, 0);
	
	std::map<std::string, SmProductYearMonth*>& year_month_map = product->GetYearMonthMap();

	SmProductYearMonth* year_month = product->GetYearMonth((LPCTSTR)curYearMon);
	if (year_month) {
		std::vector<symbol_p> symbol_list = year_month->SymbolList();
		for (size_t i = 0; i < symbol_list.size(); ++i) {
			symbol_p sym = symbol_list[i];
			// �ڽ����� �������� ǥ�ð��� �ٸ���.
			if (product->Code().compare("206") == 0) {
				if (sym->Name.length() > 0) {
					std::string centerVal = sym->Name.substr(18, 6);
					centerVal.erase(std::remove(centerVal.begin(), centerVal.end(), ','), centerVal.end());
					intCenter = std::stoi(centerVal) * (int)(std::pow(10, 2));
					delta = std::abs(mainApp.SymbolMgr().Kosdaq150Current - intCenter);
					if (delta < minVal) {
						minVal = delta;
						eCenter = intCenter;
						eIndex = i;
					}
				}
			}
			else {
				std::string centerVal = sym->ShortCode.substr(5, 3);
				char centerTip = sym->ShortCode.at(7);
				intCenter = std::stoi(centerVal) * static_cast<int>(std::pow(10, sym->Decimal));
				// ��簡�� 2�� 7�� ������ .5�� �ٴ´�. �׷��� �ڸ����� �����Ͽ� ������ ȯ���ؼ� 50�� ���� �ش�.
				if (centerTip == '2' || centerTip == '7')
					intCenter += 50;
				delta = std::abs(mainApp.SymbolMgr().Kospi200Current - intCenter);
				if (delta < minVal) {
					minVal = delta;
					eCenter = intCenter;
					eIndex = i;
				}
			}
		}
		int center_index = (int)(symbol_list.size() / 2);
		_MaxIndex = symbol_list.size() - 1;
		// � �ε����� �����Ѵ�.
		_EqualIndex = eIndex;
		// � �ɺ��� �����Ѵ�.
		_EqualSymbol = symbol_list[eIndex];
		int half = (int)(max_row / 2);
		// � �ε����� ���� �߾Ӻ��� Ŭ�� 
		if (eIndex > center_index) {
			if (eIndex + half > _MaxIndex) {
				// ���� ������ �����̹Ƿ� 1�� ���� �ش�.
				// �ε����� ���� 0���� �����ϴµ� �츮�� 1���� �����ϹǷ� 1�� �ٽ��ѹ� �����ش�.
				start_index = _MaxIndex - max_row + 1 + 1;
			}
			else {
				start_index = eIndex - half;
			}
			if (start_index < 0)
				start_index = 0;
		}
		else { // � �ε����� ���� �߾Ӻ��� ���� ��
			if (eIndex - half < 0) {
				start_index = 0;
			}
			else {
				start_index = eIndex - half;
			}
		}
	}
	*/

	return std::make_pair(start_index, max_row);
}

void SmOptionGrid::ClearAllText()
{
	for (int i = 1; i < _RowCount; ++i) {
		for (int j = 0; j < _ColCount; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			pCell->SetText("");
			InvalidateCellRect(i, j);
		}
	}
}

void SmOptionGrid::ResetRemainCells()
{
	for (auto it = _RemainPos.begin(); it != _RemainPos.end(); ++it) {
		std::pair<int, int> pos = *it;
		if (pos.second == 0)
			QuickSetBackColor(pos.first, pos.second, RGB(252, 226, 228));
		else
			QuickSetBackColor(pos.first, pos.second, RGB(218, 226, 245));

		QuickSetText(pos.first, pos.second, _T(""));
	}
}

void SmOptionGrid::ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode)
{
	CUGCell cell;
	auto it = _SymbolRowMap.find(symCode);
	if (it != _SymbolRowMap.end()) {
		auto pos = _SymbolRowMap[symCode];
		int col = std::get<1>(pos);
		int row = std::get<0>(pos);
		if (col == 0) { // ���� ���
			if (!posi) {
				if (acptCnt > 0) { // ������ ���� ����Ȯ�� �ֹ��� ���� �� - ȸ�� ��濡 0
					QuickSetText(row, col, _T("0"));
					QuickSetBackColor(row, col, RGB(212, 186, 188));
				}
				else { // ���� Ȯ�� �ֹ��� ���� ��
					if (init) { // �ѹ��̶� �ֹ��� �������� �����濡 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
					else { // �ѹ��� �ֹ��� �� �������� �ż� ����
						QuickSetText(row, col, _T(""));
						QuickSetBackColor(row, col, RGB(252, 226, 228));
					}
				}
			}
			else { // �������� �ִ� ��� - �ּ� �ѹ��̶� �ֹ��� ���������� �����Ѵ�. - ������ ����� 
				/*
				if (std::abs(posi->OpenQty) > 0) { // �ܰ��� �ִ� ���
					QuickSetNumber(row, col, posi->OpenQty);
					if (acptCnt > 0) // ���� Ȯ�� �ֹ��� �ִ� ���  ȸ����濡 �ܰ� ����
						QuickSetBackColor(row, col, RGB(212, 186, 188));
					else // ���� Ȯ�� �ֹ��� ���� ���� ��� ��濡 �ܰ� ����
						QuickSetBackColor(row, col, RGB(255, 255, 255));
				}
				else { // �ܰ��� ���� ���
					if (acptCnt > 0) { // �ѹ��̶� �ֹ��� ������� ȸ����濡 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(212, 186, 188));
					}
					else { // �ܰ�, ����Ȯ�� �� �� ���� �� ��� ��濡 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
				}
				*/
				int i = 0;
			}
		}
		else { // ǲ�� ���
			/*
			if (!posi) { // �������� ���� ��� - �ֹ��� �������� ���� ���
				if (acptCnt > 0) { // ����Ȯ�ε� �ֹ��� ���� ��� - 0�� ȸ�� ���
					QuickSetText(row, col, _T("0"));
					QuickSetBackColor(row, col, RGB(178, 186, 205));
				}
				else {
					if (init) { // �ܰ� ���� �ѹ��̶� �ֹ��� ������� - 0�� ��� ���
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
					else { // �ܰ��� ���� �ѹ��� �ֹ��� ������ ���� ��� - ���ھ��� �ŵ� ����
						QuickSetText(row, col, _T(""));
						QuickSetBackColor(row, col, RGB(218, 226, 245));
					}
				}
			}
			else { // �������� �ִ� ��� - �ֹ��� �������� �ִ� ��� - �ѹ��̶� �ֹ��� ������
				if (std::abs(posi->OpenQty) > 0) { // �ܰ��� �ִ� ���
					QuickSetNumber(row, col, posi->OpenQty);
					if (acptCnt > 0) // ����Ȯ���� �ִ� ��� ȸ�� ���
						QuickSetBackColor(row, col, RGB(178, 186, 205));
					else // �ܰ��� ���� ���� ��� ���
						QuickSetBackColor(row, col, RGB(255, 255, 255));
				}
				else { // �ܰ��� ���� ���
					if (acptCnt > 0) { // ����Ȯ���� �ִ� ��� - ȸ�� ��濡 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(178, 186, 205));
					}
					else { // �ܰ�, ����Ȯ�� ���� �� ��� ��濡 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
				}
			}
			*/
		}
		InvalidateCellRect(row, col);
		_RemainPos.insert(std::make_pair(row, col));
	}
}

void SmOptionGrid::ShowExpected(symbol_p sym)
{
	if (!sym)
		return;
	CUGCell cell;
	/*
	auto it = _SymbolRowMap.find(sym->ShortCode);
	if (it != _SymbolRowMap.end()) {
		auto pos = _SymbolRowMap[sym->ShortCode];
		ShowExpected(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
	}
	*/
}

void SmOptionGrid::ShowExpected(int row, int col, symbol_p sym)
{
	CGridCellBase* pCell = GetCell(row, col);
	/*
	if (pCell) {
		int curValue = sym->Quote.intExpected;

		std::string strVal = fmt::format("{:.{}f}", curValue / std::pow(10, sym->Decimal), sym->Decimal);

		pCell->SetText(strVal.c_str());
		InvalidateCellRect(row, col);
	}
	*/
}

void SmOptionGrid::ShowCurrent(symbol_p sym)
{
	if (!sym)
		return;
	CUGCell cell;
	/*
	auto it = _SymbolRowMap.find(sym->ShortCode);
	if (it != _SymbolRowMap.end()) {
		auto pos = _SymbolRowMap[sym->ShortCode];
		ShowCurrent(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
	}
	*/
}

void SmOptionGrid::ShowCurrent(int row, int col, symbol_p sym)
{
	CGridCellBase* pCell = GetCell(row, col);
	/*
	if (pCell) {
		int curValue = sym->Quote.intClose;

		std::string strVal = fmt::format("{:.{}f}", curValue / std::pow(10, sym->Decimal), sym->Decimal);

		pCell->SetText(strVal.c_str());
		InvalidateCellRect(row, col);
	}
	*/
}

void SmOptionGrid::SetRemain(VtPosition* posi)
{
	if (!posi)
		return;

	if (_Mode != 0)
		return;

	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;
	/*
	auto it = _SymbolRowMap.find(posi->ShortCode);
	if (it != _SymbolRowMap.end())
	{
		auto pos = _SymbolRowMap[posi->ShortCode];
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (std::abs(posi->OpenQty) > 0) {
			QuickSetNumber(std::get<0>(pos), std::get<1>(pos), posi->OpenQty);
			if (acnt->GetAcceptedOrderCount(posi->ShortCode) > 0)
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(212, 186, 188));
			else
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
		}
		else {
			QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
			QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
		}
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));

		CString msg;
		msg.Format("SetRemain row = %d, col = %d\n", std::get<0>(pos), std::get<1>(pos));
		TRACE(msg);
	}
	*/
}

void SmOptionGrid::SetRemain(VtOrder* order)
{
	if (!order || _Mode != 0 || !_OrderConfigMgr)
		return;
	/*
	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtOrderManager* orderMgr = mainApp.TotalOrderMgr().FindAddAccountOrderManger(acnt->AccountNo);
		if (!orderMgr)
			return;
		
		VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(order->shortCode);
		if (!prdtOrderMgr)
			return;
		VtPosition* posi = acnt->FindPosition(order->shortCode);
		ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, order->shortCode);

	}
	else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;

		std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(order->shortCode);
		int count = 0;
		VtPosition posi = fund->GetPosition(order->shortCode, count);
		if (count != 0)
			ShowPosition(std::get<0>(result), std::get<1>(result), &posi, order->shortCode);
		else
			ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, order->shortCode);
	}
	*/
}

void SmOptionGrid::SetRemain(symbol_p sym)
{
	if (!sym || !_OrderConfigMgr)
		return;
	/*
	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtOrderManager* orderMgr = mainApp.TotalOrderMgr().FindAddAccountOrderManger(acnt->AccountNo);
		if (!orderMgr)
			return;
		VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(sym->ShortCode);
		if (!prdtOrderMgr)
			return;
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);
		ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, sym->ShortCode);
	}
	else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;

		std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(sym->ShortCode);
		int count = 0;
		VtPosition posi = fund->GetPosition(sym->ShortCode, count);
		if (count != 0)
			ShowPosition(std::get<0>(result), std::get<1>(result), &posi, sym->ShortCode);
		else
			ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, sym->ShortCode);
	}
	*/
}

void SmOptionGrid::SetRemain2()
{
	if (_Mode != 0 || !_OrderConfigMgr)
		return;
	/*
	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it) {
		auto pos = it->second;
		symbol_p sym = std::get<2>(pos);

		if (_OrderConfigMgr->Type() == 0) {
			VtAccount* acnt = _OrderConfigMgr->Account();
			if (!acnt)
				return;

			VtOrderManager* orderMgr = mainApp.TotalOrderMgr().FindAddAccountOrderManger(acnt->AccountNo);
			if (!orderMgr)
				return;
			VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(sym->ShortCode);
			VtPosition* posi = acnt->FindPosition(sym->ShortCode);
			if (prdtOrderMgr)
				ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, sym->ShortCode);
			else
				ShowPosition(false, 0, nullptr, sym->ShortCode);
		}
		else {
			VtFund* fund = _OrderConfigMgr->Fund();
			if (!fund)
				return;

			std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(sym->ShortCode);
			int count = 0;
			VtPosition posi = fund->GetPosition(sym->ShortCode, count);
			if (count != 0)
				ShowPosition(std::get<0>(result), std::get<1>(result), &posi, sym->ShortCode);
			else
				ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, sym->ShortCode);
		}
	}
	*/
}

void SmOptionGrid::SetCurrent2()
{
	if (_Mode != 1)
		return;

	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it) {
		auto pos = it->second;
		symbol_p sym = std::get<2>(pos);
		ShowCurrent(sym);
	}
}

void SmOptionGrid::SetExpected2()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;

	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it) {
		auto pos = it->second;
		QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));
	}
}

void SmOptionGrid::QuickSetBackColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetBackClr(color);
	}
}

void SmOptionGrid::QuickSetText(int row, int col, LPCTSTR text)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetText(text);
	}
}

void SmOptionGrid::QuickSetNumber(int row, int col, int number)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		CString text;
		text.Format("%d", number);
		pCell->SetText(text);
	}
}

void SmOptionGrid::QuickSetTextColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetTextClr(color);
	}
}

void SmOptionGrid::SetEqualRow(int equalRow)
{
	int maxRow = GetMaxRow();

	int centerRow = maxRow / 2;
}

int SmOptionGrid::GetMaxRow()
{
	int rowHeight = 21; // DefaultCellHeight;

	if (_OrderConfigMgr)
		rowHeight = _OrderConfigMgr->OrderCellHeight;


	RECT rect;
	GetClientRect(&rect);

	int count = (rect.bottom - rect.top) / rowHeight;

	return count;
}

void SmOptionGrid::MakeSymbolRowMap(int start_index, int max_row)
{
	int selMon = _LeftWnd->_ComboOptionMonth.GetCurSel();
	if (selMon == -1)
		return;
	/*
	std::pair<SmProduct*, SmProduct*> product_pair = mainApp.SymbolMgr().MrktMgr().GetProductPair(_RunInfo[_SelectedProduct]);
	if (!product_pair.first || !product_pair.second)
		return;

	CString curYearMon;
	_LeftWnd->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	SmProductYearMonth* call_year_month = product_pair.first->GetYearMonth((LPCTSTR)curYearMon);	
	SmProductYearMonth* put_year_month = product_pair.second->GetYearMonth((LPCTSTR)curYearMon);
	if (!call_year_month || !put_year_month)
		return;

	// ���� � ���� �����.
	if (_EqualCell.IsValid()) {
		CGridCellBase* pCell = GetCell(_EqualCell.row, _EqualCell.col);
		if (pCell) {
			pCell->SetBackClr(RGB(255, 255, 255));
			InvalidateCellRect(_EqualCell.row, _EqualCell.col);
		}
	}

	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;
	if (call_year_month) {
		std::vector<symbol_p> call_symbol_list = call_year_month->SymbolList();
		// �ݰ� ǲ�� �ɺ��� ��ȸ�Ѵ�.
		for (size_t i = start_index, j = 1; i < call_symbol_list.size(); ++i, ++j) {
			// ���� ���� �����ϸ� Ż���Ѵ�.
			if (i < 0 || j >= _MaxRow)
				break;
			symbol_p call_sym = call_symbol_list[i];
			_SymbolRowMap[call_sym->ShortCode] = std::make_tuple(j, 0, call_sym);
			// ���⼭ ���� �ǽð� �ü� ����� ���ش�.
			mainApp.RealtimeRegisterMgr().RegisterProduct(call_sym->ShortCode, 1);
			std::string strVal = "";
			// �ڽ����϶� 
			if (product_pair.first->Code().compare("206") == 0) {
				std::string centerVal = call_sym->Name.substr(18, 6);
				centerVal.erase(std::remove(centerVal.begin(), centerVal.end(), ' '), centerVal.end());
				// �߾Ӱ��� �����Ѵ�.
				strVal = centerVal;
			}
			else { // �ڽ��� �϶�
				std::string centerVal = call_sym->ShortCode.substr(5, 3);
				char centerTip = call_sym->ShortCode.at(7);
				int intCenter = std::stoi(centerVal) * static_cast<int>(std::pow(10, call_sym->Decimal));
				// ��簡�� 2�� 7�� ������ .5�� �ٱ� ������ 50�� �����ش�.
				// ������ ��簡�� 0���� ������ ������ ���� ���� �ʾƵ� �ȴ�.
				if (centerTip == '2' || centerTip == '7')
					intCenter += 50;

				// �߾Ӱ��� �����Ѵ�.
				strVal = fmt::format("{:.{}f}", intCenter / std::pow(10, call_sym->Decimal), call_sym->Decimal);
			}

			
			CGridCellBase* pCell = GetCell(j, 1);
			if (pCell) {
				// ���ο� ��� �����Ѵ�.
				if (call_sym == _EqualSymbol) {
					pCell->SetBackClr(RGB(255, 255, 0));
					_EqualCell.row = j;
					_EqualCell.col = 1;
				}
				pCell->SetText(strVal.c_str());
				InvalidateCellRect(j, 1);
			}
			pCell = GetCell(j, 0);
			if (pCell) {
				pCell->SetText("");
				pCell->SetData((LPARAM)call_sym);
				pCell->SetBackClr(RGB(252, 226, 228));
				InvalidateCellRect(j, 0);
			}
		}
	}

	if (put_year_month) {
		std::vector<symbol_p> put_symbol_list = put_year_month->SymbolList();
		// �ݰ� ǲ�� �ɺ��� ��ȸ�Ѵ�.
		for (size_t i = start_index, j = 1; i < put_symbol_list.size(); ++i, ++j) {
			// ���� ���� �����ϸ� Ż���Ѵ�.
			if (i < 0 || j >= _MaxRow)
				break;
			symbol_p put_sym = put_symbol_list[i];
			_SymbolRowMap[put_sym->ShortCode] = std::make_tuple(j, 2, put_sym);
			// ���⼭ ���� �ǽð� �ü� ����� ���ش�.
			mainApp.RealtimeRegisterMgr().RegisterProduct(put_sym->ShortCode, 1);

			CGridCellBase* pCell = GetCell(j, 2);
			if (pCell) {
				pCell->SetText("");
				pCell->SetData((LPARAM)put_sym);
				pCell->SetBackClr(RGB(218, 226, 245));
				InvalidateCellRect(j, 2);
			}
		}
	}
	*/
}

void SmOptionGrid::OnSymbolMaster(symbol_p sym)
{
	ShowCurrent(sym);
}

void SmOptionGrid::OnOrderFilled(VtOrder* order)
{
	if (!_OrderConfigMgr || !order)
		return;
	/*
	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtPosition* posi = acnt->FindPosition(order->shortCode);
		if (posi)
			SetRemain(posi);
	}
	else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		int count = 0;
		VtPosition posi = fund->GetPosition(order->shortCode, count);
		SetRemain(&posi);
	}
	*/
}

void SmOptionGrid::OnReceiveQuote(symbol_p sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 1) {
		ShowCurrent(sym);
	}
}

void SmOptionGrid::OnExpected(symbol_p sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 2) {
		ShowExpected(sym);
	}
}