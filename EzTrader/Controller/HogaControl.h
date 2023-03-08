#pragma once
#include "../ViewModel/VmHoga.h"
#include "../Util/IdGenerator.h"

#include <memory>
class SymbolOrderView;
namespace DarkHorse {
	struct SmHoga;
class HogaControl
{
public:
	HogaControl();
	~HogaControl() {};
	void update_hoga(std::shared_ptr<SmHoga> hoga);
	const VmHoga& get_hoga()
	{
		return hoga_;
	}
	const int get_id()
	{
		return id_;
	}
	void symbol_order_view(SymbolOrderView* symbol_order_view_p) {
		symbol_order_view_ = symbol_order_view_p;
	}
private:
	void subscribe_hoga_control();
	VmHoga hoga_;
	int id_;
	SymbolOrderView* symbol_order_view_{ nullptr };
};
}

