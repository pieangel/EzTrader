#pragma once
#include <iostream>
#include <vector>
#include <memory>
// User-defined structure
struct WinInfo {
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
		return id_ == other.id_ && x_ == other.x_ && y_ == other.y_ && h_ == other.h_ && w_ == other.w_;
	}
};
