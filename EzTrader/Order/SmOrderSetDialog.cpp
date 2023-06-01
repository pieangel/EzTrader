// SmOrderSetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmOrderSetDialog.h"
#include "afxdialogex.h"
#include "AbAccountOrderCenterWindow.h"
#include "../Fund/SmFundOrderCenterWnd.h"
#include "../Util/IdGenerator.h"
#include "../Event/EventHub.h"
#include "../Event/EventHubArg.h"
#include "../Global/SmTotalManager.h"
#include <vector>

// SmOrderSetDialog dialog

IMPLEMENT_DYNAMIC(SmOrderSetDialog, CBCGPDialog)

SmOrderSetDialog::SmOrderSetDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_SET, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	id_ = DarkHorse::IdGenerator::get_id();
}

SmOrderSetDialog::SmOrderSetDialog(CWnd* pParent, const int& window_id_from)
	: CBCGPDialog(IDD_ORDER_SET, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	id_ = DarkHorse::IdGenerator::get_id();
	window_id_from_ = window_id_from;
}

SmOrderSetDialog::~SmOrderSetDialog()
{
}

void SmOrderSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ALIGN_BY_ALT, check_align_by_alt_);
	DDX_Control(pDX, IDC_CHECK_BAR_COLOR, check_show_bar_color_);
	DDX_Control(pDX, IDC_CHECK_CANCEL_BY_RIGHT_CLICK, check_cancel_by_right_click_);
	DDX_Control(pDX, IDC_CHECK_ORDER_BY_SPACE, check_order_by_space_);

	DDX_Control(pDX, IDC_CHECK_STOP_TO_REAL, check_stop_by_real_);
	DDX_Control(pDX, IDC_CHECK_SHOW_ORDER_COL, check_show_order_column_);
	DDX_Control(pDX, IDC_CHECK_SHOW_STOP_COL, check_show_stop_column_);
	DDX_Control(pDX, IDC_CHECK_SHOW_COUNT_COL, check_show_count_column_);

	DDX_Control(pDX, IDC_EDIT_CELL_HEIGHT, edit_row_height_);
	DDX_Control(pDX, IDC_EDIT_STOP_WIDTH, edit_stop_width_);
	DDX_Control(pDX, IDC_EDIT_ORDER_WIDTH, edit_order_width_);
	DDX_Control(pDX, IDC_EDIT_COUNT_WIDTH, edit_count_width_);
	DDX_Control(pDX, IDC_EDIT_QTY_WIDTH, edit_qty_width_);
	DDX_Control(pDX, IDC_EDIT_QUOTE_WIDTH, edit_quote_width_);
}


BEGIN_MESSAGE_MAP(SmOrderSetDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_CHECK_SET_WIDE, &SmOrderSetDialog::OnBnClickedCheckSetWide)
	ON_BN_CLICKED(IDC_CHECK_BAR_COLOR, &SmOrderSetDialog::OnBnClickedCheckBarColor)
	ON_BN_CLICKED(IDC_CHECK_ALIGN_BY_ALT, &SmOrderSetDialog::OnBnClickedCheckAlignByAlt)
	ON_BN_CLICKED(IDC_CHECK_ORDER_BY_SPACE, &SmOrderSetDialog::OnBnClickedCheckOrderBySpace)
	ON_BN_CLICKED(IDC_CHECK_CANCEL_BY_RIGHT_CLICK, &SmOrderSetDialog::OnBnClickedCheckCancelByRightClick)
	ON_BN_CLICKED(IDC_BUTTON1, &SmOrderSetDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK_STOP_TO_REAL, &SmOrderSetDialog::OnBnClickedCheckStopToReal)
END_MESSAGE_MAP()


// SmOrderSetDialog message handlers


void SmOrderSetDialog::OnBnClickedCheckSetWide()
{
	
}


BOOL SmOrderSetDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	check_align_by_alt_.SetCheck(BST_CHECKED);
	check_show_bar_color_.SetCheck(BST_CHECKED);
	check_cancel_by_right_click_.SetCheck(BST_CHECKED);
	check_order_by_space_.SetCheck(BST_CHECKED);

	check_show_order_column_.SetCheck(BST_CHECKED);
	check_show_stop_column_.SetCheck(BST_CHECKED);
	check_show_count_column_.SetCheck(BST_CHECKED);
	check_stop_by_real_.SetCheck(BST_CHECKED);

	edit_row_height_.SetWindowText("18");
	edit_stop_width_.SetWindowText("40");
	edit_order_width_.SetWindowText("55");
	edit_count_width_.SetWindowText("35");
	edit_qty_width_.SetWindowText("35");
	edit_quote_width_.SetWindowText("60");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmOrderSetDialog::OnBnClickedCheckBarColor()
{
	// TODO: Add your control notification handler code here
}


void SmOrderSetDialog::OnBnClickedCheckAlignByAlt()
{
	// TODO: Add your control notification handler code here
}


void SmOrderSetDialog::OnBnClickedCheckOrderBySpace()
{
	// TODO: Add your control notification handler code here
}


void SmOrderSetDialog::OnBnClickedCheckCancelByRightClick()
{
	// TODO: Add your control notification handler code here
}


void SmOrderSetDialog::OnBnClickedButton1()
{
	if (!edit_row_height_.GetSafeHwnd()) return;
	if (!edit_stop_width_.GetSafeHwnd()) return;
	if (!edit_order_width_.GetSafeHwnd()) return;
	if (!edit_count_width_.GetSafeHwnd()) return;
	if (!edit_qty_width_.GetSafeHwnd()) return;
	if (!edit_quote_width_.GetSafeHwnd()) return;
	std::vector<int> height_and_width_vec;
	CString value;
	edit_row_height_.GetWindowText(value);
	height_and_width_vec.push_back(_ttoi(value));
	edit_stop_width_.GetWindowText(value);
	height_and_width_vec.push_back(_ttoi(value));
	edit_order_width_.GetWindowText(value);
	height_and_width_vec.push_back(_ttoi(value));
	edit_count_width_.GetWindowText(value);
	height_and_width_vec.push_back(_ttoi(value));
	edit_qty_width_.GetWindowText(value);
	height_and_width_vec.push_back(_ttoi(value));
	edit_quote_width_.GetWindowText(value);
	height_and_width_vec.push_back(_ttoi(value));

	DarkHorse::OrderSetEvent order_set_event;
	order_set_event.window_id = id_;
	order_set_event.grid_height = height_and_width_vec[0];
	order_set_event.stop_width = height_and_width_vec[1];
	order_set_event.order_width = height_and_width_vec[2];
	order_set_event.count_width = height_and_width_vec[3];
	order_set_event.qty_width = height_and_width_vec[4];
	order_set_event.quote_width = height_and_width_vec[5];
	order_set_event.stop_as_real_order = stop_as_real_order_;

	//mainApp.event_hub()->trigger_order_set_event(window_id_from_, order_set_event, false);


	mainApp.event_hub()->trigger_parameter_event( window_id_from_, order_set_event, "test", true);
}


void SmOrderSetDialog::OnBnClickedCheckStopToReal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (check_stop_by_real_.GetCheck() == BST_CHECKED)
		stop_as_real_order_ = true;
	else
		stop_as_real_order_ = false;
}
