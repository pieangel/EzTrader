#pragma once
const int fixed_left_wnd_width = 200;
const int fixed_right_wnd_width = 181;
const int fixed_child_wnd_y_pos = 34;
const int fixed_order_panel_y_pos = 167;
const int fixed_tick_wnd_width = 173;
const int fixed_tick_wnd_height = 400;
const int hor_gap = 3;
const int ver_gap = 3;
const int default_child_wnd_height = 700;

enum OrderWndResizeEvent {
	CHILD_ADD = 0,
	CHILD_DELETE,
	SHOW_LEFT,
	HIDE_LEFT,
	SHOW_RIGHT,
	HIDE_RIGHT,
	ORDER_CONFIG_CHANGED,
	RESIZE_MAIN
};
