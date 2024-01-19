#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <windows.h>
#include "../Log/MyLogger.h"
// User-defined structure
struct WinInfo {
	std::string name_;
	HANDLE wh_{nullptr};
	int old_x_{0};
	int old_y_{0};
	int old_w_{0};
	int old_h_{0};
	int x_{ 0 };
	int y_{ 0 };
	int w_{ 0 };
	int h_{ 0 };
	int id_{ 0 };
	CRect rc_new;
	CRect rc_old;
	CWnd* wnd {nullptr};
	bool show_{ false };

	std::vector<std::shared_ptr<WinInfo>> children_{};
	
	std::shared_ptr<WinInfo> parent_{ nullptr };

	WinInfo(std::shared_ptr<WinInfo> parent, int id, int x, int y, int h, int w) : parent_{ parent }, id_(id), x_(x), y_(y), h_(h), w_(w) {}

  size_t get_child_count() {
    return children_.size();
  }
	// Custom equality operator for WinInfo
	bool operator==(const WinInfo& other) const {
		return x_ == other.x_ && y_ == other.y_ && h_ == other.h_ && w_ == other.w_;
	}
	// Custom equality operator for WinInfo
	bool is_samme(const WinInfo& other) {
		return x_ == old_x_ && y_ == old_y_ && h_ == old_h_ && w_ == old_w_;
	}
	bool is_same_as_old_window() 
	{
	  return rc_new.left == rc_old.left &&
	         rc_new.right == rc_old.right && 
	         rc_new.top == rc_old.top &&
	         rc_new.bottom == rc_old.bottom;
	}
	void move_window(bool redraw)
	{
	  if (!wnd || is_same_as_old_window()) 
	  {
	      //if (wnd && wnd->GetSafeHwnd()) wnd->Invalidate(TRUE);
	      return;
	  }
	  LOGINFO(CMyLogger::getInstance(), "move_window[%s]", name_.c_str());  
	  wnd->MoveWindow(rc_new, redraw ? TRUE : FALSE);
	  rc_old.left = rc_new.left;
	  rc_old.top = rc_new.top;
	  rc_old.right = rc_new.right;
	  rc_old.bottom = rc_new.bottom;
	}
	void move_child_window()
	{
	  const size_t child_count = children_.size();
	  for(size_t i = 0; i < child_count; i++) {
	    children_[i]->move_window(true);
	  }
	}
};
