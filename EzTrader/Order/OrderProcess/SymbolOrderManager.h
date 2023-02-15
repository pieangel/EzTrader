#pragma once
#include <string>
#include <map>
#include <memory>
#include "../../Json/json.hpp"
namespace DarkHorse {
using order_event = nlohmann::json;
class SymbolOrderManager
{
public:
    void on_order_event(const order_event& order_info);
};
}

