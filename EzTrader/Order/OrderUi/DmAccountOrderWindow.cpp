#include "stdafx.h"
#include "DmAccountOrderWindow.h"
#include "../../Util/IdGenerator.h"
#include "../../Global/SmTotalManager.h"
#include "../../Task/SmTaskRequestManager.h"
#include "OrderWndConst.h"
#include "../../Log/MyLogger.h"

/*
DmAccountOrderWindow::DmAccountOrderWindow()
{
	_id = DarkHorse::IdGenerator::get_id();
}

DmAccountOrderWindow::~DmAccountOrderWindow()
{

}
*/


#include "stdafx.h"
#include "../../DarkHorse.h"
#include "DmAccountOrderWindow.h"
#include "afxdialogex.h"
#include "DmAccountOrderLeftWindow.h"
#include "DmAccountOrderRightWindow.h"
#include "DmAccountOrderCenterWindow.h"
#include <set>
#include "../../MainFrm.h"
#include "../../Account/SmAccount.h"
#include "../../Global/SmTotalManager.h"
#include "../../Account/SmAccountManager.h"
#include "../../Client/ViStockClient.h"
#include "../../MessageDefine.h"
#include "../../Event/SmCallbackManager.h"

#include "../../Position/SmTotalPositionManager.h"
#include "../../Position/SmAccountPositionManager.h"
#include "../../Position/SmPosition.h"
#include "../../Order/SmOrderRequest.h"
#include "../../Client/ViStockClient.h"
#include "../../Order/SmOrderRequestManager.h"
#include "../../Order/SmTotalOrderManager.h"

#include "../../Symbol/SmSymbol.h"
#include "../../Event/EventHub.h"
#include "../../Util/IdGenerator.h"
#include <functional>
using namespace std::placeholders;

using namespace DarkHorse;

const int CtrlHeight = 32;

int DmAccountOrderWindow::_Id = 0;

void DmAccountOrderWindow::SetAccount()
{
	std::vector<std::shared_ptr<SmAccount>> main_account_vector;
	mainApp.AcntMgr()->get_main_account_vector("9", main_account_vector);
	if (main_account_vector.empty()) return;
	for (auto ita = main_account_vector.begin(); ita != main_account_vector.end(); ++ita) {
		auto main_acnt = *ita;
		std::string account_info;
		account_info.append(main_acnt->Name());
		account_info.append(" : ");
		account_info.append(main_acnt->No());
		const int index = _ComboAccount.AddString(account_info.c_str());
		_ComboAccountMap[index] = main_acnt;
		_AccountComboMap[main_acnt->No()] = index;

		const std::vector<std::shared_ptr<SmAccount>>& sub_account_vector = main_acnt->get_sub_accounts();
		for (auto it = sub_account_vector.begin(); it != sub_account_vector.end(); it++) {
			auto account = *it;
			account_info = "";
			account_info.append(account->Name());
			account_info.append(" : ");
			account_info.append(account->No());
			const int index = _ComboAccount.AddString(account_info.c_str());
			_ComboAccountMap[index] = account;
			_AccountComboMap[account->No()] = index;
		}
	}

	if (!_ComboAccountMap.empty()) {
		// if account_no_ is empty, set the first account as default
		if (account_no_.empty())
			_CurrentAccountIndex = 0;
		else {
			// if account_no_ is not empty, set the account_no_ as default
			auto it = _AccountComboMap.find(account_no_);
			if (it != _AccountComboMap.end()) {
				_CurrentAccountIndex = it->second;
			}
			else {
				_CurrentAccountIndex = 0;
			}	
		}
		_ComboAccount.SetCurSel(_CurrentAccountIndex);
		SetAccountInfo(_ComboAccountMap[_CurrentAccountIndex]);
		_LeftWnd->OnOrderChanged(0, 0);

		
	}
}

// DmAccountOrderWindow dialog

IMPLEMENT_DYNAMIC(DmAccountOrderWindow, CBCGPDialog)

DmAccountOrderWindow::DmAccountOrderWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_DM_ACNT_ORDER_MAIN, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	id_ = IdGenerator::get_id();
	win_info_ = std::make_shared<WinInfo>(nullptr, id_, 0, 0, 0, 0);
	win_info_->name_ = "main";
	mainApp.event_hub()->add_symbol_order_view_event(1, std::bind(&DmAccountOrderWindow::on_symbol_view_clicked, this, std::placeholders::_1, std::placeholders::_2));
	mainApp.event_hub()->subscribe_symbol_order_view_event_handler(id_, std::bind(&DmAccountOrderWindow::on_symbol_view_event, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

DmAccountOrderWindow::DmAccountOrderWindow(CWnd* pParent, const size_t center_window_count, std::string& account_no)
	: CBCGPDialog(IDD_DM_ACNT_ORDER_MAIN, pParent), center_window_count_(center_window_count), account_no_(account_no)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	id_ = IdGenerator::get_id();
	win_info_ = std::make_shared<WinInfo>(nullptr, id_, 0, 0, 0, 0);
	mainApp.event_hub()->add_symbol_order_view_event(1, std::bind(&DmAccountOrderWindow::on_symbol_view_clicked, this, std::placeholders::_1, std::placeholders::_2));
	mainApp.event_hub()->subscribe_symbol_order_view_event_handler(id_, std::bind(&DmAccountOrderWindow::on_symbol_view_event, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

DmAccountOrderWindow::DmAccountOrderWindow(CWnd* pParent, const size_t center_window_count, std::string& account_no, const nlohmann::json center_wnd_prop)
	: CBCGPDialog(IDD_DM_ACNT_ORDER_MAIN, pParent), center_window_count_(center_window_count), account_no_(account_no), center_wnd_prop_(center_wnd_prop)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
	id_ = IdGenerator::get_id();
	win_info_ = std::make_shared<WinInfo>(nullptr, id_, 0, 0, 0, 0);
	mainApp.event_hub()->add_symbol_order_view_event(1, std::bind(&DmAccountOrderWindow::on_symbol_view_clicked, this, std::placeholders::_1, std::placeholders::_2));
	mainApp.event_hub()->subscribe_symbol_order_view_event_handler(id_, std::bind(&DmAccountOrderWindow::on_symbol_view_event, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

DmAccountOrderWindow::~DmAccountOrderWindow()
{
	mainApp.event_hub()->unsubscribe_symbol_order_view_event_handler(id_);
}

void DmAccountOrderWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
	//DDX_Control(pDX, IDC_STATIC_ACCOUNT_NAME, _StaticAccountName);
	DDX_Control(pDX, IDC_STATIC_LINE1, _Line1);
	DDX_Control(pDX, IDC_STATIC_LINE2, _Line2);
	DDX_Control(pDX, IDC_STATIC_LINE3, _Line3);
	DDX_Control(pDX, IDC_STATIC_LINE4, _Line4);
	DDX_Control(pDX, IDC_STATIC_LINE5, _Line5);
	DDX_Control(pDX, IDC_STATIC_LINE6, _Line6);
	DDX_Control(pDX, IDC_STATIC_LINE7, _Line7);
	DDX_Control(pDX, IDC_STATIC_LINE8, _Line8);
	DDX_Control(pDX, IDC_STATIC_LINE9, _Line9);
	DDX_Control(pDX, IDC_STATIC_LINE10, _Line10);
	DDX_Control(pDX, IDC_STATIC_MSG, _StaticMsg);
	DDX_Control(pDX, IDC_BTN_LEFT, _BtnLeft);
	DDX_Control(pDX, IDC_BTN_RIGHT, _BtnRight);
}


BEGIN_MESSAGE_MAP(DmAccountOrderWindow, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD, &DmAccountOrderWindow::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_REMOVE, &DmAccountOrderWindow::OnBnClickedBtnRemove)
	ON_BN_CLICKED(IDC_BTN_LEFT, &DmAccountOrderWindow::OnBnClickedBtnLeft)
	ON_BN_CLICKED(IDC_BTN_RIGHT, &DmAccountOrderWindow::OnBnClickedBtnRight)
	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	//ON_STN_CLICKED(IDC_STATIC_ACCOUNT_NAME, &DmAccountOrderWindow::OnStnClickedStaticAccountName)
	ON_BN_CLICKED(IDC_BUTTON6, &DmAccountOrderWindow::OnBnClickedButton6)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &DmAccountOrderWindow::OnCbnSelchangeComboAccount)
	ON_MESSAGE(WM_ORDER_UPDATE, &DmAccountOrderWindow::OnUmOrderUpdate)
	ON_MESSAGE(WM_SERVER_MSG, &DmAccountOrderWindow::OnUmServerMsg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &DmAccountOrderWindow::OnBnClickedBtnLiqAll)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// DmAccountOrderWindow message handlers


LRESULT DmAccountOrderWindow::OnEnterSizeMove(WPARAM wparam, LPARAM lparam)
{


	GetWindowRect(moveRect);
	// do stuff      
	return (LRESULT)0;
}
LRESULT DmAccountOrderWindow::OnExitSizeMove(WPARAM wparam, LPARAM lparam)
{


	CRect rcWnd;
	GetWindowRect(rcWnd);

	if (moveRect.Width() == rcWnd.Width() && moveRect.Height() == rcWnd.Height()) return 0;

	//RecalcChildren(CM_REFRESH);
	//Invalidate(FALSE);
	// do stuff      
	return (LRESULT)0;
}


BOOL DmAccountOrderWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	CRect rcClient, rcWnd;

	_LineVector.push_back(&_Line1);
	_LineVector.push_back(&_Line2);
	_LineVector.push_back(&_Line3);
	_LineVector.push_back(&_Line4);
	_LineVector.push_back(&_Line5);
	_LineVector.push_back(&_Line6);
	_LineVector.push_back(&_Line7);
	_LineVector.push_back(&_Line8);
	_LineVector.push_back(&_Line9);
	_LineVector.push_back(&_Line10);

	GetWindowRect(rcWnd);


	_LeftWnd = std::make_shared<DmAccountOrderLeftWindow>(this, win_info_);
	_LeftWnd->order_window_id(id_);
	_LeftWnd->Create(IDD_DM_ACNT_ORDER_LEFT, this);
	_LeftWnd->ShowWindow(SW_SHOW);
	_LeftWnd->SetMainWnd(this);

	if (center_window_count_ == 0) {
		std::shared_ptr<DmAccountOrderCenterWindow> center_wnd = std::make_shared<DmAccountOrderCenterWindow>(this, win_info_);
		center_wnd->Create(IDD_DM_ACNT_ORDER_CENTER, this);
		center_wnd->ShowWindow(SW_SHOW);
		center_wnd->SetMainDialog(this);
		center_wnd->Selected(true);
		center_wnd->order_window_id(id_);
		center_window_map_.insert(std::make_pair(center_wnd->ID(), center_wnd));
	}
	else {
		int index = 0;
		for (const auto& centerWindowJson : center_wnd_prop_) {
			std::string symbolCode = centerWindowJson["symbol_code"].get<std::string>();
			int windowId = centerWindowJson["window_id"].get<int>();
			std::string message = centerWindowJson["message"].get<std::string>();

			int grid_height = centerWindowJson["grid_height"].get<int>();
			int stop_width = centerWindowJson["stop_width"].get<int>();
			int order_width = centerWindowJson["order_width"].get<int>();
			int count_width = centerWindowJson["count_width"].get<int>();
			int qty_width = centerWindowJson["qty_width"].get<int>();
			int quote_width = centerWindowJson["quote_width"].get<int>();
			bool stop_as_real_order = centerWindowJson["stop_as_real_order"].get<bool>();

			bool show_symbol_tick = centerWindowJson["show_symbol_tick"].get<bool>();
			bool show_bar_color = centerWindowJson["show_bar_color"].get<bool>();
			bool align_by_alt = centerWindowJson["align_by_alt"].get<bool>();
			bool cancel_by_right_click = centerWindowJson["cancel_by_right_click"].get<bool>();
			bool order_by_space = centerWindowJson["order_by_space"].get<bool>();
			bool show_order_column = centerWindowJson["show_order_column"].get<bool>();
			bool show_stop_column = centerWindowJson["show_stop_column"].get<bool>();
			bool show_count_column = centerWindowJson["show_count_column"].get<bool>();

			DarkHorse::OrderSetEvent order_set_event;
			order_set_event.grid_height = grid_height;
			order_set_event.stop_width = stop_width;
			order_set_event.order_width = order_width;
			order_set_event.count_width = count_width;
			order_set_event.qty_width = qty_width;
			order_set_event.quote_width = quote_width;
			order_set_event.stop_as_real_order = stop_as_real_order;
			order_set_event.show_symbol_tick = show_symbol_tick;
			order_set_event.show_bar_color = show_bar_color;
			order_set_event.align_by_alt = align_by_alt;
			order_set_event.cancel_by_right_click = cancel_by_right_click;
			order_set_event.order_by_space = order_by_space;
			order_set_event.show_order_column = show_order_column;
			order_set_event.show_stop_column = show_stop_column;
			order_set_event.show_count_column = show_count_column;

			// ... Retrieve other properties

			// Create and populate DmAccountOrderCenterWindow object
			std::shared_ptr<DmAccountOrderCenterWindow> center_wnd = std::make_shared<DmAccountOrderCenterWindow>(this, win_info_, symbolCode, order_set_event);
			center_wnd->Create(IDD_DM_ACNT_ORDER_CENTER, this);
			center_wnd->ShowWindow(SW_SHOW);
			center_wnd->SetMainDialog(this);
			center_wnd->order_window_id(id_);
			if (index == 0) {
				center_wnd->Selected(true);
			}
			index++;
			center_window_map_.insert(std::make_pair(center_wnd->ID(), center_wnd));
		}
	}
	

	_RightWnd = std::make_shared<DmAccountOrderRightWindow>(this, win_info_);
	_RightWnd->order_window_id(id_);
	_RightWnd->Create(IDD_DM_ACNT_ORDER_RIGHT, this);
	_RightWnd->ShowWindow(SW_SHOW);


	SetAccount();

	SetAccountForOrderWnd();

	_ComboAccount.SetDroppedWidth(150);

	GetWindowRect(rcWnd);

	rcWnd.bottom = rcWnd.top + 1000;
	MoveWindow(rcWnd);
	recalChildWndPos();
	moveMainWnd();
	moveChildWnd();
	//GetClientRect(rcClient);
	//_StaticAccountName.GetWindowRect(rcClient);

	RECT rcFrame = { 0 };
	//AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW, FALSE, 0);



	//RecalcChildren(CM_REFRESH);



	_Init = true;

	CenterWindow();

	_StaticMsg.SetUp();

	_ComboAccount.SetDroppedWidth(250);

	mainApp.CallbackMgr()->SubscribeOrderUpdateCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->SubscribeServerMsgCallback(GetSafeHwnd());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void DmAccountOrderWindow::OnQuoteAreaShowHide()
{

}

void DmAccountOrderWindow::RecalcChildren(CmdMode mode)
{
  CRect rc_main, rc_left, rc_right, rc_center;
  GetWindowRect(rc_main);
  _LeftWnd->GetWindowRect(rc_left);
  _RightWnd->GetWindowRect(rc_right);
  // First, calculate the height of the child windows.
  const int child_wnd_height = rc_main.bottom - rc_left.top;
  const size_t child_count = win_info_->children_.size();
  int child_wnd_xpos = 0;
  win_info_->children_[0]->wnd = _LeftWnd.get();
  win_info_->children_[0]->rc_new.left = child_wnd_xpos;
  win_info_->children_[0]->rc_new.right = _ShowLeft ? fixed_left_wnd_width : 0;
  win_info_->children_[0]->rc_new.top = fixed_child_wnd_y_pos;
  win_info_->children_[0]->rc_new.bottom = fixed_child_wnd_y_pos + child_wnd_height;
  child_wnd_xpos = win_info_->children_[0]->rc_new.right;
  size_t i = 1;
  for (auto it = center_window_map_.begin(); 
  it != center_window_map_.end(); 
  it++) {
    win_info_->children_[i]->wnd = it->second.get();
    win_info_->children_[i]->rc_new.left = child_wnd_xpos;
    win_info_->children_[i]->rc_new.right = it->second->get_width();
    win_info_->children_[i]->rc_new.top = fixed_child_wnd_y_pos;
    win_info_->children_[i]->rc_new.bottom = fixed_child_wnd_y_pos + child_wnd_height;
    child_wnd_xpos += win_info_->children_[i]->rc_new.right;
	i++;
  }
  
  win_info_->children_[child_count - 1]->wnd = _RightWnd.get();
  win_info_->children_[child_count - 1]->rc_new.left = child_wnd_xpos;
  win_info_->children_[child_count - 1]->rc_new.right = _ShowRight ? fixed_right_wnd_width : 0;
  win_info_->children_[child_count - 1]->rc_new.top = fixed_child_wnd_y_pos;
  win_info_->children_[child_count - 1]->rc_new.bottom = fixed_child_wnd_y_pos + child_wnd_height;
   

	GetWindowRect(rc_main);
	_rcMain = rc_main;
	CRect rcWnd;
	_StaticMsg.GetWindowRect(rcWnd);
	int len = rc_main.right - rcWnd.left - 13;
	rcWnd.right = rcWnd.left + len;
	ScreenToClient(rcWnd);
	//_StaticMsg.MoveWindow(rcWnd);
	_StaticMsg.SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOZORDER | SWP_FRAMECHANGED);
	SendMessage(WM_SERVER_MSG, 0, 0);
}


void DmAccountOrderWindow::RecalcChildren2(CmdMode mode)
{
	// ��ü ���� ���
	const int hor_gap = 2;
	int total_width = 0;
	CRect rcWnd;
	std::vector<CRect> rcVector;
	if (_ShowLeft) {
		_LeftWnd->GetWindowRect(rcWnd);
		rcVector.push_back(rcWnd);
	}

	for (auto it = center_window_map_.begin(); it != center_window_map_.end(); it++) {
		it->second->GetWindowRect(rcWnd);
		rcVector.push_back(rcWnd);
	}

	if (_ShowRight) {
		_RightWnd->GetWindowRect(rcWnd);
		rcVector.push_back(rcWnd);
	}
	for (size_t i = 0; i < rcVector.size(); i++) {
		total_width += rcVector[i].Width();
		total_width += hor_gap;
	}

	total_width += hor_gap;

	GetWindowRect(rcWnd);
	rcWnd.right = rcWnd.left + total_width + 20;

	SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOZORDER | SWP_NOREDRAW);
}

void DmAccountOrderWindow::onResizeEvent(OrderWndResizeEvent event)
{
  switch(event) {
    case CHILD_ADD:
    case CHILD_DELETE:
    case SHOW_LEFT:
    case HIDE_LEFT:
    case SHOW_RIGHT:
    case HIDE_RIGHT:
    case ORDER_CONFIG_CHANGED:
      recalChildWndPos();
	  moveMainWnd();

      // 이 함수는 이벤트가 일어나는 곳에서 직접 호출한다. 
      //moveWndPos();
      moveChildWnd();
      break;
    case RESIZE_MAIN:
		
      recalChildWndPos();
	  moveMainWnd();
      moveChildWnd();
      break;
    default:
      break;
  }

  Invalidate(FALSE);
  //UpdateWindow();
}

void DmAccountOrderWindow::moveWndPos()
{
	if (!win_info_) return;
	win_info_->move_window(true);
}

void DmAccountOrderWindow::recalChildWndPos()
{
  CRect rc_main, rc_left, rc_right, rc_center;
  GetWindowRect(rc_main);
  _LeftWnd->GetWindowRect(rc_left);
  _RightWnd->GetWindowRect(rc_right);
  // First, calculate the height of the child windows.
  const int child_wnd_height = rc_main.bottom - rc_left.top;
  const size_t child_count = win_info_->children_.size();
  int child_wnd_xpos = 0;
  int next_child_wnd_xpos = _ShowLeft ? fixed_left_wnd_width : 0;
  LOGINFO(CMyLogger::getInstance(), "child_wnd_xpos[%d]", child_wnd_xpos);
  win_info_->children_[0]->wnd = _LeftWnd.get();
  win_info_->children_[0]->rc_new.left = child_wnd_xpos;
  win_info_->children_[0]->rc_new.right = next_child_wnd_xpos;
  win_info_->children_[0]->rc_new.top = fixed_child_wnd_y_pos;
  win_info_->children_[0]->rc_new.bottom = fixed_child_wnd_y_pos + child_wnd_height;
  child_wnd_xpos = next_child_wnd_xpos;
  LOGINFO(CMyLogger::getInstance(), "child_wnd_count[%d]", win_info_->children_.size());  
  size_t i = 1;
  for (auto it = center_window_map_.begin(); 
  it != center_window_map_.end(); 
  it++) {
    LOGINFO(CMyLogger::getInstance(), "child_wnd_xpos[%d]", child_wnd_xpos);
    win_info_->children_[i]->wnd = it->second.get();
    win_info_->children_[i]->rc_new.left = child_wnd_xpos;
    win_info_->children_[i]->rc_new.right = child_wnd_xpos + it->second->get_width();
    win_info_->children_[i]->rc_new.top = fixed_child_wnd_y_pos;
    win_info_->children_[i]->rc_new.bottom = fixed_child_wnd_y_pos + child_wnd_height;
    it->second->set_child_wnd_pos(it->second->get_width(), child_wnd_height);
    child_wnd_xpos += it->second->get_width();
    const int x = win_info_->children_[i]->rc_new.left;
    const int y = win_info_->children_[i]->rc_new.top;
    const int w = win_info_->children_[i]->rc_new.right - win_info_->children_[i]->rc_new.left;
    const int h = win_info_->children_[i]->rc_new.bottom - win_info_->children_[i]->rc_new.top;
    LOGINFO(CMyLogger::getInstance(), "x[%d],y[%d],w[%d],h[%d]", x, y, w, h);
        
	i++;
  }
  LOGINFO(CMyLogger::getInstance(), "child_wnd_xpos[%d]", child_wnd_xpos);  
  win_info_->children_[child_count - 1]->wnd = _RightWnd.get();
  win_info_->children_[child_count - 1]->rc_new.left = child_wnd_xpos;
  win_info_->children_[child_count - 1]->rc_new.right = _ShowRight ? child_wnd_xpos + fixed_right_wnd_width : 0;
  win_info_->children_[child_count - 1]->rc_new.top = fixed_child_wnd_y_pos;
  win_info_->children_[child_count - 1]->rc_new.bottom = fixed_child_wnd_y_pos + child_wnd_height;
  
  const int x1 = win_info_->children_[child_count - 1]->rc_new.left;
  const int y1 = win_info_->children_[child_count - 1]->rc_new.top;
  const int w1 = win_info_->children_[child_count - 1]->rc_new.right - win_info_->children_[i]->rc_new.left;
  const int h1 = win_info_->children_[child_count - 1]->rc_new.bottom - win_info_->children_[i]->rc_new.top;
  LOGINFO(CMyLogger::getInstance(), "x1[%d],y1[%d],w1[%d],h1[%d]", x1, y1, w1, h1);
  
  const int main_wnd_width = child_wnd_xpos + fixed_right_wnd_width;
  win_info_->wnd = this;
  win_info_->rc_new.left = rc_main.left;
  win_info_->rc_new.top = rc_main.top;
  win_info_->rc_new.right = rc_main.left + main_wnd_width;
  win_info_->rc_new.bottom = rc_main.bottom;
}

void DmAccountOrderWindow::moveMainWnd()
{
  if (!win_info_) return;
  win_info_->move_window(true);
}

void DmAccountOrderWindow::moveChildWnd()
{
  const size_t child_count = win_info_->children_.size();
  LOGINFO(CMyLogger::getInstance(), "child_count = %d", child_count);
  for(size_t i = 0; i < child_count; i++) {
    const int x = win_info_->children_[i]->rc_new.left;
    const int y = win_info_->children_[i]->rc_new.top;
    const int w = win_info_->children_[i]->rc_new.right - win_info_->children_[i]->rc_new.left;
    const int h = win_info_->children_[i]->rc_new.bottom - win_info_->children_[i]->rc_new.top;
          
    if (!win_info_->children_[i]->is_same_as_old_window()) {
        LOGINFO(CMyLogger::getInstance(), "name[%s]", win_info_->children_[i]->name_.c_str());
        LOGINFO(CMyLogger::getInstance(), "x[%d],y[%d],w[%d],h[%d]", x, y, w, h);
    }
    win_info_->children_[i]->move_window(true);
    
    if (win_info_->children_[i]->get_child_count() > 0) {
      win_info_->children_[i]->move_child_window();
    }
  }
}

void DmAccountOrderWindow::SetAccountForOrderWnd()
{
	if (_ComboAccountMap.size() > 0) {
		const int cur_sel = _ComboAccount.GetCurSel();
		if (cur_sel >= 0) {
			for (auto it = center_window_map_.begin(); it != center_window_map_.end(); ++it) {
				it->second->Account(_ComboAccountMap[cur_sel]);
			}
		}
	}
}


void DmAccountOrderWindow::SetAccountInfo(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account || _CurrentAccountIndex < 0) return;

	std::string account_info;
	account_info.append(account->Name());
	account_info.append(" : ");
	account_info.append(account->No());
	//_StaticAccountName.SetWindowText(account_info.c_str());
	if (!account->is_subaccount())
		mainApp.Client()->RegisterAccount(account->No());
	_Account = account;
	std::shared_ptr<SmAccount> parent_account = nullptr;
	if (_Account->is_subaccount()) {
		parent_account = account->parent_account().lock();
		_LeftWnd->SetAccount(parent_account);
		_RightWnd->SetAccount(parent_account);
	}
	else {
		_LeftWnd->SetAccount(_Account);
		_RightWnd->SetAccount(_Account);
	}
}

std::string DmAccountOrderWindow::get_account_no()
{
	if (_Account) return _Account->No();
	return "";	
}

void DmAccountOrderWindow::on_symbol_view_event(const std::string& account_type, int center_window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!_Account || _Account->Type() != account_type) return;
	if (!symbol) return;
	auto found = center_window_map_.find(center_window_id);
	if (found == center_window_map_.end()) return;
	ChangedSymbol(symbol);
	ChangedCenterWindow(center_window_id);
}

void DmAccountOrderWindow::on_symbol_view_clicked(const int center_window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;
	auto found = center_window_map_.find(center_window_id);
	if (found == center_window_map_.end()) return;
	ChangedSymbol(symbol);
	ChangedCenterWindow(center_window_id);
}

void DmAccountOrderWindow::OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	for (auto it = center_window_map_.begin(); it != center_window_map_.end(); it++) {
		if (it->second->Selected()) {
			it->second->OnSymbolClicked(symbol->SymbolCode());
			break;
		}
	}
	_RightWnd->OnSymbolClicked(symbol);
}

void DmAccountOrderWindow::OnSymbolClicked(const std::string& symbol_code)
{
	for (auto it = center_window_map_.begin(); it != center_window_map_.end(); it++) {
		if (it->second->Selected()) {
			it->second->OnSymbolClicked(symbol_code);
			break;
		}
	}
}

void DmAccountOrderWindow::OnBnClickedBtnAdd()
{
	auto child_win_info_ = std::make_shared<WinInfo>(win_info_, 0, 0, 0, 0, 0);
	std::shared_ptr<DmAccountOrderCenterWindow> center_wnd = std::make_shared<DmAccountOrderCenterWindow>(this, win_info_, child_win_info_);
	center_wnd->order_window_id(id_);
	center_wnd->Create(IDD_DM_ACNT_ORDER_CENTER, this);
	center_wnd->ShowWindow(SW_SHOW);
	center_wnd->SetMainDialog(this);
	center_wnd->Account(_ComboAccountMap[_CurrentAccountIndex]);
	center_window_map_.insert(std::make_pair(center_wnd->ID(), center_wnd));
	
	child_win_info_->name_ = "center";
	child_win_info_->wnd = center_wnd.get();
	center_wnd->Win_info(child_win_info_);
	win_info_->children_.insert(win_info_->children_.begin() + center_window_map_.size(), child_win_info_);

    onResizeEvent(CHILD_ADD);
	//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	//pFrame->ReconnectOrderEvent();
}


void DmAccountOrderWindow::OnBnClickedBtnRemove()
{
	if (center_window_map_.size() == 1) return;

	//LockWindowUpdate();
	auto it = std::prev(center_window_map_.end());
	center_window_map_.erase(it);
	//RecalcChildren(CM_DEL_CENTER);
	//UnlockWindowUpdate();
	//Invalidate(FALSE);
	win_info_->children_.erase(win_info_->children_.begin() + center_window_map_.size());
	
	onResizeEvent(CHILD_DELETE);
		
}


void DmAccountOrderWindow::OnBnClickedBtnLeft()
{
	_ShowLeft ? _ShowLeft = false : _ShowLeft = true;
	if (_ShowLeft) _BtnLeft.SetWindowText(">>");
	else _BtnLeft.SetWindowText("<<");	
	//LockWindowUpdate();
	//RecalcChildren(_ShowLeft ? CM_SHOW_LEFT : CM_HIDE_LEFT);
	//UnlockWindowUpdate();
	//Invalidate(FALSE);
	onResizeEvent(SHOW_LEFT);
}


void DmAccountOrderWindow::OnBnClickedBtnRight()
{
	_ShowRight ? _ShowRight = false : _ShowRight = true;
	if (_ShowRight) _BtnRight.SetWindowText("<<");
	else _BtnRight.SetWindowText(">>");
	//LockWindowUpdate();
	//RecalcChildren(_ShowRight ? CM_SHOW_RIGHT : CM_HIDE_RIGHT);
	//UnlockWindowUpdate();
	//Invalidate(FALSE);
	onResizeEvent(SHOW_RIGHT);
}


void DmAccountOrderWindow::OnStnClickedStaticAccountName()
{

}


void DmAccountOrderWindow::OnBnClickedButton6()
{
	int max_delta_height = 0;
	for (auto it = center_window_map_.begin(); it != center_window_map_.end(); ++it) {
		std::shared_ptr<DmAccountOrderCenterWindow> center_wnd = it->second;
		int delta_height = center_wnd->RecalcOrderAreaHeight(this);
		if (delta_height > max_delta_height) max_delta_height = delta_height;
	}


	if (max_delta_height > 2) {
		CRect rcWnd;
		GetWindowRect(rcWnd);
		rcWnd.bottom += max_delta_height;
		SetWindowPos(nullptr, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOMOVE);
	}

	Invalidate(FALSE);
}


void DmAccountOrderWindow::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (!_Init) return;

	onResizeEvent(RESIZE_MAIN);
}


BOOL DmAccountOrderWindow::OnEraseBkgnd(CDC* pDC)
{
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void DmAccountOrderWindow::OnCbnSelchangeComboAccount()
{
	_CurrentAccountIndex = _ComboAccount.GetCurSel();
	if (_CurrentAccountIndex < 0) return;

	SetAccountInfo(_ComboAccountMap[_CurrentAccountIndex]);

	SetAccountForOrderWnd();
	if (!_Account) return;
	if (_Account->is_subaccount()) return;

	DhTaskArg arg;
	arg.detail_task_description = _Account->No();
	arg.task_type = DhTaskType::AccountProfitLoss;
	arg.parameter_map["account_no"] = _Account->No();
	arg.parameter_map["password"] = _Account->Pwd();
	arg.parameter_map["account_type"] = _Account->Type();

	mainApp.TaskReqMgr()->AddTask(std::move(arg));
}

LRESULT DmAccountOrderWindow::OnUmOrderUpdate(WPARAM wParam, LPARAM lParam)
{
	const int account_id = (int)wParam;
	const int symbol_id = (int)lParam;

	_LeftWnd->OnOrderChanged(account_id, symbol_id);
	_RightWnd->OnOrderChanged(account_id, symbol_id);

	for (auto it = center_window_map_.begin(); it != center_window_map_.end(); it++) {
		it->second->OnOrderChanged(account_id, symbol_id);
	}

	return 1;
}

void DmAccountOrderWindow::ChangedSymbol(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	_RightWnd->SetSymbol(symbol);
}

void DmAccountOrderWindow::ChangedCenterWindow(const int& center_wnd_id)
{
	for (auto it = center_window_map_.begin(); it != center_window_map_.end(); ++it) {
		it->second->ID() == center_wnd_id ? it->second->SetSelected(true) : it->second->SetSelected(false);
	}
}


void DmAccountOrderWindow::OnClose()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->remove_dm_account_order_window(GetSafeHwnd());
	//mainApp.CallbackMgr()->UnsubscribeOrderUpdateCallback(GetSafeHwnd());
	//mainApp.CallbackMgr()->UnsubscribeServerMsgCallback(GetSafeHwnd());

	if (!destroyed_) CBCGPDialog::OnClose();
}


void DmAccountOrderWindow::OnBnClickedBtnLiqAll()
{
	if (!_Account) return;

	auto account_pos_mgr = mainApp.TotalPosiMgr()->FindAddAccountPositionManager(_Account->No());
	const std::map<std::string, std::shared_ptr<SmPosition>>& account_pos_map = account_pos_mgr->GetPositionMap();
	for (auto it = account_pos_map.begin(); it != account_pos_map.end(); ++it) {
		std::shared_ptr<SmOrderRequest> order_req = nullptr;
		if (it->second->Position == SmPositionType::Buy)
			order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(_Account->No(), _Account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
		else
			order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(_Account->No(), _Account->Pwd(), it->second->SymbolCode, 0, abs(it->second->OpenQty), DarkHorse::SmPriceType::Market);
		mainApp.Client()->NewOrder(order_req);
	}
}

LRESULT DmAccountOrderWindow::OnUmServerMsg(WPARAM wParam, LPARAM lParam)
{
	_StaticMsg.Text(mainApp.TotalOrderMgr()->ServerMsg.c_str());
	_StaticMsg.Invalidate();
	return 1;
}


void DmAccountOrderWindow::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0x0000FFF0) == SC_MINIMIZE)
	{
		// Minimizing, post to main dialogue also.
		//AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
	}
	else if ((nID & 0x0000FFF0) == SC_RESTORE && IsIconic())
	{
		// Restoring, post to main dialogue also.
		//AfxGetMainWnd()->ShowWindow(SW_RESTORE);
		Invalidate(TRUE);
	}

	if ((nID & 0x0000FFF0) == SC_KEYMENU) {
		for (auto it = center_window_map_.begin(); it != center_window_map_.end(); ++it) {
			std::shared_ptr<DmAccountOrderCenterWindow> center_wnd = it->second;
			center_wnd->ArrangeCenterValue();
		}
		return;
	}

	CBCGPDialog::OnSysCommand(nID, lParam);
}


void DmAccountOrderWindow::OnDestroy()
{
	
	if (!destroyed_) CBCGPDialog::OnDestroy();
}


void DmAccountOrderWindow::PostNcDestroy()
{
	//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	//pFrame->remove_dm_account_order_window(GetSafeHwnd());

	CBCGPDialog::PostNcDestroy();
	destroyed_ = true;
	delete this;
}


void DmAccountOrderWindow::saveToJson(json& j) const {

	CRect rect;
	GetWindowRect(&rect);

	j = {
		{"x", rect.left},
		{"y", rect.top },
		{"width", rect.right - rect.left},
		{"height", rect.bottom - rect.top},
		{"center_window_count", center_window_map_.size()},
		{"account_no", _Account ? _Account->No() : ""}	
	};

	for (const auto& pair : center_window_map_) {
		json center_window_json;
		pair.second->saveToJson(center_window_json);
		j["center_window_map"][std::to_string(pair.first)] = center_window_json;
	}
}

void DmAccountOrderWindow::loadFromJson(const json& j) {
	const json& center_window_map_json = j["center_window_map"];
	for (const auto& pair : center_window_map_json.items()) {
		int window_id = std::stoi(pair.key());
		const json& center_window_json = pair.value();

		std::shared_ptr<DmAccountOrderCenterWindow> center_window = std::make_shared<DmAccountOrderCenterWindow>();
		center_window->loadFromJson(center_window_json);

		center_window_map_[window_id] = center_window;
	}
}

