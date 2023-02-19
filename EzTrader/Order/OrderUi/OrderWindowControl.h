#pragma once
#include <string>
#include <memory>

namespace DarkHorse {
struct Position;
class OrderWindowControl
{
public:
	OrderWindowControl(const int id);
	~OrderWindowControl();
	void remove_order_event(const std::string& order_no);
	void update_position_event(std::shared_ptr<Position> position);
private:
	void subscribe_remove_order_event();
private:
	int _id;
};
}

