#include "stdafx.h"
#include "HdProductRemainGrid.h"
#include "HdAccountPLDlg.h"
#include <numeric>
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Order/SmAccountOrderManager.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccount.h"
#include "../Position/TotalPositionManager.h"
#include "../Position/AccountPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Order/SmOrderRequest.h"
#include "../Client/ViStockClient.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Fund/SmFund.h"

#include "../Event/SmCallbackManager.h"
#include "../Order/OrderUi/DmAccountOrderWindow.h"
#include "../Fund/SmFundOrderDialog.h"
#include "../CompOrder/SmOrderCompMainDialog.h"
#include "../CompOrder/SmFundCompMainDialog.h"
#include "../Controller/AccountPositionControl.h"
#include "../Position/Position.h"
#include "../Util/VtStringUtil.h"
#include "../Log/MyLogger.h"
#include "../Event/EventHub.h"
#include "../Util/IdGenerator.h"
#include "../Symbol/SmSymbolManager.h"

#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

HdProductRemainGrid::HdProductRemainGrid()
{
	_AcntPLDlg = nullptr;
	account_position_control_ = std::make_shared<DarkHorse::AccountPositionControl>();
	account_position_control_->set_single_position_event_handler(std::bind(&HdProductRemainGrid::on_update_single_position, this, _1));
	account_position_control_->set_event_handler(std::bind(&HdProductRemainGrid::on_update_whole_position, this, _1));

}


HdProductRemainGrid::~HdProductRemainGrid()
{
}


void HdProductRemainGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetUGMode(UGMode::Normal);
	SetVScrollMode(UG_SCROLLJOYSTICK);

	SetDoubleBufferMode(TRUE);
	
	SetDefColWidth(80);
	//_RowCount = GetMaxRow();
	SetHS_Height(0);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	QuickSetAlignment(0, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetAlignment(0, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetAlignment(1, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetAlignment(2, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetAlignment(3, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(0);
	SetColTitle();
	InitGrid();
	CRect rcWnd;
	GetWindowRect(rcWnd);
	SetWindowPos(nullptr, 0, 0, std::accumulate(_ColWidthVector.begin(), _ColWidthVector.end(), 0), rcWnd.Height(), SWP_NOMOVE);
}

BEGIN_MESSAGE_MAP(HdProductRemainGrid, VtGrid)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void HdProductRemainGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

int HdProductRemainGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

void HdProductRemainGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[4] = { "코드", "구분", "평균가", "평가손익" };
	_ColWidthVector.push_back(58);
	_ColWidthVector.push_back(22);
	_ColWidthVector.push_back(50);
	_ColWidthVector.push_back(62);

	for (int i = 0; i < _ColCount; i++) {
		SetColWidth(i, _ColWidthVector[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		//cell.SetBackColor(VtGlobal::GridTitleBackColor);
		//cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		QuickSetAlignment(i, -1, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		RedrawCell(i, -1);
	}
}

void HdProductRemainGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void HdProductRemainGrid::InitGrid()
{
	if (!_AcntPLDlg )
		return;

}



void HdProductRemainGrid::ClearValues()
{
	for (int i = 0; i < _RowCount; i++) {
		QuickSetText(0, i, _T(""));
		QuickSetText(1, i, _T(""));
		QuickSetText(2, i, _T(""));
		QuickSetText(3, i, _T(""));
	}
}
/*
void HdProductRemainGrid::ShowPosition(VtPosition* posi, VtAccount* acnt, int index)
{
	if (!posi || !acnt)
		return;
	
	VtCellPos pos;
	for (int i = 0; i < 4; i++) {
		pos.Row = index;
		pos.Col = i;
		_OldPosMap[pos] = pos;
	}

	if (!posi) {
		QuickSetText(0, index, _T(""));
		QuickSetText(1, index, _T(""));
		QuickSetText(2, index, _T(""));
		QuickSetText(3, index, _T(""));
		return;
	}

	if (posi->OpenQty == 0) {
		QuickSetText(0, index, _T(""));
		QuickSetText(1, index, _T(""));
		QuickSetText(2, index, _T(""));
		QuickSetText(3, index, _T(""));
		return;
	}
	
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindHdSymbol(posi->ShortCode);
	if (!sym)
		return;

	QuickSetText(0, index, posi->ShortCode.c_str());
	
	if (posi->Position == VtPositionType::Buy) {
		QuickSetTextColor(1, index, RGB(255, 0, 0));
		QuickSetTextColor(2, index, RGB(255, 0, 0));
		QuickSetTextColor(3, index, RGB(255, 0, 0));
	} else if (posi->Position == VtPositionType::Sell) {
		QuickSetTextColor(1, index, RGB(0, 0, 255));
		QuickSetTextColor(2, index, RGB(0, 0, 255));
		QuickSetTextColor(3, index, RGB(0, 0, 255));
		QuickSetNumber(1, index, std::abs(posi->OpenQty));
	}

	std::string symCode = posi->ShortCode;
	if (symCode.find(_T("201")) != std::string::npos) { // coloring the call 
		QuickSetBackColor(0, index, RGB(252, 226, 228));
		QuickSetBackColor(1, index, RGB(252, 226, 228));
		QuickSetBackColor(2, index, RGB(252, 226, 228));
		QuickSetBackColor(3, index, RGB(252, 226, 228));
	}

	if (symCode.find(_T("301")) != std::string::npos) { // coloring the put 
		QuickSetBackColor(0, index, RGB(218, 226, 245));
		QuickSetBackColor(1, index, RGB(218, 226, 245));
		QuickSetBackColor(2, index, RGB(218, 226, 245));
		QuickSetBackColor(3, index, RGB(218, 226, 245));
	}

	CString strValue;
	strValue.Format(_T("%d"), posi->OpenQty);
	QuickSetText(1, index, strValue);

	std::string temp = NumberFormatter::format(posi->AvgPrice, sym->Decimal);
	CString thVal = XFormatNumber(temp.c_str(), sym->Decimal);
	QuickSetText(2, index, thVal);

	temp = NumberFormatter::format(posi->OpenProfitLoss, 0);
	CString profitLoss = XFormatNumber(temp.c_str(), -1);

	if (posi->OpenProfitLoss > 0) {
		QuickSetTextColor(3, index, RGB(255, 0, 0));
		QuickSetText(3, index, profitLoss);
	} else if (posi->OpenProfitLoss < 0) {
		QuickSetTextColor(3, index, RGB(0, 0, 255));
		QuickSetText(3, index, profitLoss);
	} else {
		QuickSetTextColor(3, index, RGB(0, 0, 0));
		QuickSetNumber(3, index, 0);
	}
	
}
*/

void HdProductRemainGrid::ClearOldValuse(std::map<VtCellPos, VtCellPos>& posMap)
{
	for (auto it = _OldPosMap.begin(); it != _OldPosMap.end(); ++it) {
		VtCellPos pos = it->second;
		QuickSetText(pos.Col, pos.Row, _T(""));
		QuickSetBackColor(pos.Col, pos.Row, RGB(255, 255, 255));
		posMap[pos] = pos;
	}
}

void HdProductRemainGrid::RefreshCells(std::map<VtCellPos, VtCellPos>& posMap)
{
	for (auto it = posMap.begin(); it != posMap.end(); ++it) {
		VtCellPos pos = it->second;
		QuickRedrawCell(pos.Col, pos.Row);
	}
}


void HdProductRemainGrid::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	fund_ = val;

	if (!account_position_control_) return;
	account_position_control_->set_fund(fund_);
	enable_position_show_ = true;
	// 계좌 유형에 따라 표시 내용과 표시 간격을 바꾼다.
	//SetColumnName(3, "평균단가");
	//SetColumnWidth(0, column_widths_vector_[0]);
	//set_column_widths(fund_->fund_type());
	//set_column_names(fund_->fund_type());
	//update_account_position();
}



void HdProductRemainGrid::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	account_ = val;
	if (!account_position_control_) return;
	account_position_control_->set_account(account_);
	enable_position_show_ = true;
	// 계좌 유형에 따라 표시 내용과 표시 간격을 바꾼다.
	//SetColumnName(3, "평균단가");
	//SetColumnWidth(0, column_widths_vector_[0]);
	//set_column_widths(account_->Type());
	//set_column_names(account_->Type());
	//update_account_position();
}



void HdProductRemainGrid::UpdatePositionInfo()
{

}

void HdProductRemainGrid::LiqSelPositions()
{
	_Mode == 0 ? LiqSelPositionsForAccount() : LiqSelPositionsForFund();
	//ClearCheck();
}

void HdProductRemainGrid::LiqAll()
{
	_Mode == 0 ? LiqAllForAccount() : LiqAllForFund();
	//ClearCheck();
}

void HdProductRemainGrid::on_update_single_position(const int position_id)
{
	/*
	auto found = position_to_row_.find(position_id);
	if (found == position_to_row_.end()) return;
	std::string format_type;
	if (account_) format_type = account_->Type();
	else if (fund_) format_type = fund_->fund_type();
	else return;

	CBCGPGridRow* pRow = GetRow(found->second);
	if (!pRow) return;
	auto found2 = row_to_position_.find(found->second);
	if (found2 == row_to_position_.end()) return;
	auto position = found2->second;
	if (format_type == "1")
		update_ab_account_position(pRow, position, format_type);
	else
		update_dm_account_position(pRow, position, format_type);
	Invalidate();
	*/
	LOGINFO(CMyLogger::getInstance(), "on_update_single_position = %d", position_id);
	//update_account_position();
	enable_position_show_ = true;
}

void HdProductRemainGrid::on_update_whole_position(const int result)
{
	// 	if (result == 0) {
	// 		ClearOldContents(0);
	// 	}
	LOGINFO(CMyLogger::getInstance(), "on_update_whole_position = %d", result);
	//update_account_position();
	enable_position_show_ = true;
}

/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl message handlers




void HdProductRemainGrid::LiqSelPositionsForAccount()
{
	if (!account_) return;

// 	for (auto it = row_to_position_.begin(); it != row_to_position_.end(); ++it) {
// 		CBCGPGridRow* pRow = GetRow(it->first);
// 		if (pRow->GetCheck()) {
// 			std::shared_ptr<SmOrderRequest> order_req = nullptr;
// 			if (it->second->open_quantity > 0)
// 				order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account_->No(), account_->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
// 			else if (it->second->open_quantity < 0)
// 				order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account_->No(), account_->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
// 			mainApp.Client()->NewOrder(order_req);
// 		}
// 	}
}

void HdProductRemainGrid::LiqSelPositionsForFund()
{
	if (!fund_) return;

// 	const std::vector<std::shared_ptr<SmAccount>>& account_vec = fund_->GetAccountVector();
// 	for (auto it2 = account_vec.begin(); it2 != account_vec.end(); it2++) {
// 		auto account = *it2;
// 		for (auto it = row_to_position_.begin(); it != row_to_position_.end(); ++it) {
// 			CBCGPGridRow* pRow = GetRow(it->first);
// 			if (pRow->GetCheck()) {
// 				std::shared_ptr<SmOrderRequest> order_req = nullptr;
// 				if (it->second->open_quantity > 0)
// 					order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
// 				else if (it->second->open_quantity < 0)
// 					order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
// 				mainApp.Client()->NewOrder(order_req);
// 			}
// 		}
// 	}
}

void HdProductRemainGrid::LiqAllForAccount()
{
	if (!account_) return;

	for (auto it = row_to_position_.begin(); it != row_to_position_.end(); ++it) {
		std::shared_ptr<SmOrderRequest> order_req = nullptr;
		if (it->second->open_quantity > 0)
			order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account_->No(), account_->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
		else if (it->second->open_quantity < 0)
			order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account_->No(), account_->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
		mainApp.Client()->NewOrder(order_req);
	}
}

void HdProductRemainGrid::LiqAllForFund()
{
	if (!fund_) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = fund_->GetAccountVector();
	for (auto it2 = account_vec.begin(); it2 != account_vec.end(); it2++) {
		auto account = *it2;
		for (auto it = row_to_position_.begin(); it != row_to_position_.end(); ++it) {
			std::shared_ptr<SmOrderRequest> order_req = nullptr;
			if (it->second->open_quantity > 0)
				order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
			else if (it->second->open_quantity < 0)
				order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), it->second->symbol_code, 0, abs(it->second->open_quantity), DarkHorse::SmPriceType::Market);
			mainApp.Client()->NewOrder(order_req);
		}
	}
}

void HdProductRemainGrid::update_account_position()
{
	if (!account_position_control_) return;

	//if (updating_) return;
	//updating_ = true;
	//ClearValues();
	row_to_position_.clear();
	position_to_row_.clear();

	std::string format_type;
	if (account_) format_type = account_->Type();
	else if (fund_) format_type = fund_->fund_type();
	else return;

	const std::map<std::string, position_p>& active_positions = account_position_control_->get_active_position_map();
	int row = 0, index = 0;
	for (auto it = active_positions.begin(); it != active_positions.end(); ++it) {
		const auto& position = it->second;
		if (position->open_quantity == 0) {
			clear_row(index++);
			continue;
		}
		index++;
		if (format_type == "1")
			ShowPosition(row, position, format_type);
		else
			ShowPosition(row, position, format_type);

		_OldContentRowSet.insert(row);
		row_to_position_[row] = position;
		position_to_row_[position->id] = row;
		row++;
	}
	//ClearOldContents(row);
	_OldMaxRow = row;
	//updating_ = false;
	enable_position_show_ = true;
}

void HdProductRemainGrid::clear_row(const int row)
{
	for (int i = 0; i < 4; i++) {
		QuickSetText(i, row, _T(""));
		QuickRedrawCell(i, row);

	}
}
/*
void HdProductRemainGrid::ShowPosition(int index, position_p posi, const std::string& format_type)
{
	if (!posi) {
		clear_row(index);
		return;
	}
	if (updating_) return;
	updating_ = true;

	VtCellPos pos;
	if (!posi) {
		clear_row(index);
		updating_ = false;
		return;
	}
	auto symbol = mainApp.SymMgr()->FindSymbol(posi->symbol_code);
	if (!symbol) {
		updating_ = false;
		clear_row(index);
		return;
	}

	QuickSetText(0, index, posi->symbol_code.c_str());
	pos.Col = 0;
	pos.Row = index;
	_OldPosMap[pos] = pos;
	if (posi->open_quantity > 0) {
		QuickSetTextColor(1, index, RGB(255, 0, 0));
		QuickSetTextColor(2, index, RGB(255, 0, 0));
		QuickSetTextColor(3, index, RGB(255, 0, 0));
		QuickSetNumber(1, index, posi->open_quantity);
	}
	else if (posi->open_quantity < 0) {
		QuickSetTextColor(1, index, RGB(0, 0, 255));
		QuickSetTextColor(2, index, RGB(0, 0, 255));
		QuickSetTextColor(3, index, RGB(0, 0, 255));
		QuickSetNumber(1, index, posi->open_quantity);
	}
	else {
		QuickSetTextColor(1, index, RGB(0, 0, 0));
		QuickSetTextColor(2, index, RGB(0, 0, 0));
		QuickSetTextColor(3, index, RGB(0, 0, 0));
		QuickSetNumber(1, index, posi->open_quantity);
	}
	pos.Col = 1;
	pos.Row = index;
	_OldPosMap[pos] = pos;
	std::string symCode = posi->symbol_code;
	if (symCode.find(_T("201")) != std::string::npos) { // coloring the call 
		QuickSetBackColor(0, index, RGB(252, 226, 228));
		QuickSetBackColor(1, index, RGB(252, 226, 228));
		QuickSetBackColor(2, index, RGB(252, 226, 228));
		QuickSetBackColor(3, index, RGB(252, 226, 228));
	}

	if (symCode.find(_T("301")) != std::string::npos) { // coloring the put 
		QuickSetBackColor(0, index, RGB(218, 226, 245));
		QuickSetBackColor(1, index, RGB(218, 226, 245));
		QuickSetBackColor(2, index, RGB(218, 226, 245));
		QuickSetBackColor(3, index, RGB(218, 226, 245));
	}

	CString strValue;
	strValue.Format(_T("%d"), posi->open_quantity);
	QuickSetText(1, index, strValue);
	pos.Col = 1;
	pos.Row = index;
	_OldPosMap[pos] = pos;
	const int decimal = format_type == "1" ? 2 : 0;


	std::string value_string = VtStringUtil::get_format_value(posi->average_price / pow(10, symbol->decimal()), symbol->decimal(), true);
	pos.Col = 2;
	pos.Row = index;
	_OldPosMap[pos] = pos;
	QuickSetText(2, index, value_string.c_str());

	std::string open_pl = VtStringUtil::get_format_value(posi->open_profit_loss, decimal, true);

	if (posi->open_profit_loss > 0) {
		QuickSetTextColor(3, index, RGB(255, 0, 0));
		QuickSetText(3, index, open_pl.c_str());
	}
	else if (posi->open_profit_loss < 0) {
		QuickSetTextColor(3, index, RGB(0, 0, 255));
		QuickSetText(3, index, open_pl.c_str());
	}
	else {
		QuickSetTextColor(3, index, RGB(0, 0, 0));
		QuickSetText(3, index, "0");
	}
	pos.Col = 3;
	pos.Row = index;
	_OldPosMap[pos] = pos;

	QuickRedrawCell(0, index);
	QuickRedrawCell(1, index);
	QuickRedrawCell(2, index);
	QuickRedrawCell(3, index);

	updating_ = false;
}
*/

void HdProductRemainGrid::ShowPosition(int index, position_p posi, const std::string& format_type)
{
	if (!posi || updating_) {
		clear_row(index);
		return;
	}

	updating_ = true;

	VtCellPos pos;
	auto symbol = mainApp.SymMgr()->FindSymbol(posi->symbol_code);
	if (!symbol) {
		updating_ = false;
		clear_row(index);
		return;
	}

	QuickSetText(0, index, posi->symbol_code.c_str());
	pos.Col = 0;
	pos.Row = index;
	_OldPosMap[pos] = pos;

	int open_quantity = posi->open_quantity;
	COLORREF text_color = RGB(0, 0, 0); // Default text color
	if (open_quantity > 0) {
		text_color = RGB(255, 0, 0); // Red for positive quantity
	}
	else if (open_quantity < 0) {
		text_color = RGB(0, 0, 255); // Blue for negative quantity
	}

	QuickSetTextColor(1, index, text_color);
	QuickSetTextColor(2, index, text_color);
	QuickSetTextColor(3, index, open_quantity != 0 ? text_color : RGB(0, 0, 0)); // Set text color for quantity

	QuickSetNumber(1, index, open_quantity);

	// Set background color based on symbol code
	COLORREF back_color = RGB(255, 255, 255); // Default background color
	std::string symCode = posi->symbol_code;
	if (symCode.find(_T("201")) != std::string::npos) { // Call
		back_color = RGB(252, 226, 228);
	}
	else if (symCode.find(_T("301")) != std::string::npos) { // Put
		back_color = RGB(218, 226, 245);
	}
	QuickSetBackColor(0, index, back_color);
	QuickSetBackColor(1, index, back_color);
	QuickSetBackColor(2, index, back_color);
	QuickSetBackColor(3, index, back_color);

	CString strValue;
	strValue.Format(_T("%d"), open_quantity);
	QuickSetText(1, index, strValue);
	pos.Col = 1;
	pos.Row = index;
	_OldPosMap[pos] = pos;

	const int decimal = format_type == "1" ? 2 : 0;
	std::string value_string = VtStringUtil::get_format_value(posi->average_price / pow(10, symbol->decimal()), symbol->decimal(), true);
	QuickSetText(2, index, value_string.c_str());
	pos.Col = 2;
	pos.Row = index;
	_OldPosMap[pos] = pos;

	std::string open_pl = VtStringUtil::get_format_value(posi->open_profit_loss, decimal, true);
	COLORREF pl_text_color = posi->open_profit_loss > 0 ? RGB(255, 0, 0) : (posi->open_profit_loss < 0 ? RGB(0, 0, 255) : RGB(0, 0, 0));
	QuickSetTextColor(3, index, pl_text_color);
	QuickSetText(3, index, open_pl.c_str());
	pos.Col = 3;
	pos.Row = index;
	_OldPosMap[pos] = pos;

	// Redraw cells
	QuickRedrawCell(0, index);
	QuickRedrawCell(1, index);
	QuickRedrawCell(2, index);
	QuickRedrawCell(3, index);

	updating_ = false;
}


void HdProductRemainGrid::update_dm_account_position(CBCGPGridRow* pRow, position_p position, const std::string& format_type)
{
	pRow->GetItem(0)->SetValue(position->symbol_code.c_str(), TRUE);
	pRow->GetItem(0)->SetBackgroundColor(RGB(255, 255, 255));
	pRow->GetItem(0)->SetTextColor(RGB(0, 0, 0));
	if (position->open_quantity > 0) {
		//pRow->GetItem(0)->SetBackgroundColor(RGB(255, 0, 0));
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(255, 0, 0));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(255, 0, 0));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));

	}
	else if (position->open_quantity < 0) {
		//pRow->GetItem(0)->SetBackgroundColor(RGB(0, 0, 255));
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(0, 0, 255));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(0, 0, 255));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
	}
	else {
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(0, 0, 0));
	}

	std::string open_quantity = VtStringUtil::get_format_value(position->open_quantity, 0, true);
	pRow->GetItem(1)->SetValue(open_quantity.c_str(), TRUE);
	pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
	pRow->GetItem(1)->SetTextColor(RGB(0, 0, 0));
	std::string value_string = VtStringUtil::get_format_value(position->average_price / pow(10, 2), 2, true);
	pRow->GetItem(2)->SetValue(value_string.c_str(), TRUE);
	pRow->GetItem(2)->SetBackgroundColor(RGB(255, 255, 255));
	pRow->GetItem(2)->SetTextColor(RGB(0, 0, 0));
	//const std::string open_pl = std::format("{0:.2f}", position->open_profit_loss);
	const int decimal = format_type == "1" ? 2 : 0;
	std::string open_pl = VtStringUtil::get_format_value(position->open_profit_loss, decimal, true);

	CBCGPGridItem* pItem = pRow->GetItem(3);
	if (pItem) {
		pItem->SetValue(open_pl.c_str(), TRUE);
		if (position->open_profit_loss > 0) {
			pItem->SetBackgroundColor(RGB(255, 255, 255));
			pItem->SetTextColor(RGB(255, 0, 0));
		}
		else if (position->open_profit_loss < 0) {
			pItem->SetBackgroundColor(RGB(255, 255, 255));
			pItem->SetTextColor(RGB(0, 0, 255));
		}
		else {
			pItem->SetBackgroundColor(RGB(255, 255, 255));
			pItem->SetTextColor(RGB(0, 0, 0));
		}
	}

}

void HdProductRemainGrid::update_ab_account_position(CBCGPGridRow* pRow, position_p position, const std::string& format_type)
{
	pRow->GetItem(0)->SetValue(position->symbol_code.c_str(), TRUE);
	pRow->GetItem(0)->SetBackgroundColor(RGB(255, 255, 255));
	pRow->GetItem(0)->SetTextColor(RGB(0, 0, 0));
	if (position->open_quantity > 0) {
		pRow->GetItem(0)->SetBackgroundColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(255, 0, 0));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(255, 0, 0));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(255, 0, 0));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));

		pRow->GetItem(1)->SetValue("매수", TRUE);
	}
	else if (position->open_quantity < 0) {
		pRow->GetItem(1)->SetValue("매도", TRUE);
		pRow->GetItem(0)->SetBackgroundColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetBackgroundColor(RGB(255, 255, 255));
		//pRow->GetItem(2)->SetBackgroundColor(RGB(0, 0, 255));
		//pRow->GetItem(3)->SetBackgroundColor(RGB(0, 0, 255));

		//pRow->GetItem(0)->SetTextColor(RGB(255, 255, 255));
		pRow->GetItem(1)->SetTextColor(RGB(0, 0, 255));
		//pRow->GetItem(2)->SetTextColor(RGB(255, 255, 255));
		//pRow->GetItem(3)->SetTextColor(RGB(255, 255, 255));
	}
	std::string open_quantity = VtStringUtil::get_format_value(position->open_quantity, 0, true);
	pRow->GetItem(3)->SetValue(open_quantity.c_str(), TRUE);
	pRow->GetItem(3)->SetBackgroundColor(RGB(255, 255, 255));
	pRow->GetItem(3)->SetTextColor(RGB(0, 0, 0));
	//const std::string open_pl = std::format("{0:.2f}", position->open_profit_loss);
	const int decimal = format_type == "1" ? 2 : 0;
	std::string open_pl = VtStringUtil::get_format_value(position->open_profit_loss, decimal, true);
	if (position->open_profit_loss > 0) {
		pRow->GetItem(2)->SetBackgroundColor(RGB(255, 255, 255));
		pRow->GetItem(2)->SetTextColor(RGB(255, 0, 0));
	}
	else if (position->open_profit_loss < 0) {
		pRow->GetItem(2)->SetBackgroundColor(RGB(255, 255, 255));
		pRow->GetItem(2)->SetTextColor(RGB(0, 0, 255));
	}
	else {
		pRow->GetItem(2)->SetBackgroundColor(_DefaultBackColor);
		pRow->GetItem(2)->SetTextColor(RGB(0, 0, 0));
	}
	pRow->GetItem(2)->SetValue(open_pl.c_str(), TRUE);
}

void HdProductRemainGrid::refresh()
{
	if (enable_position_show_) {
		update_account_position();
	}
	enable_position_show_ = false;
}

void HdProductRemainGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	
}


void HdProductRemainGrid::OnTimer(UINT_PTR nIDEvent)
{
	VtGrid::OnTimer(nIDEvent);
}
