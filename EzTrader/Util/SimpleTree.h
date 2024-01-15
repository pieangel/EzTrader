#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <windows.h>
#include <string>
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
	bool show_{ false };

	std::vector<std::shared_ptr<WinInfo>> children_{};
	std::shared_ptr<WinInfo> parent_{ nullptr };

	WinInfo(std::shared_ptr<WinInfo> parent, int id, int x, int y, int h, int w) : parent_{ parent }, id_(id), x_(x), y_(y), h_(h), w_(w) {}

	// Custom equality operator for WinInfo
	bool operator==(const WinInfo& other) const {
		return x_ == other.x_ && y_ == other.y_ && h_ == other.h_ && w_ == other.w_;
	}
	// Custom equality operator for WinInfo
	bool is_samme(const WinInfo& other) {
		return x_ == old_x_ && y_ == old_y_ && h_ == old_h_ && w_ == old_w_;
	}
};
