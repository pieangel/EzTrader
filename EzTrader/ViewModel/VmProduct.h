#pragma once
#include <string>
namespace DarkHorse {
struct VmProduct {
	// 소수점
	int decimal;
	// 정수 틱크기
	int int_tick_size;
	// 틱크기 
	double tick_size;
	// 틱가치 - 정수
	int tick_value;
	// 승수
	int seung_su;
	// 호가 단위 정수
	int hoga_unit;
};
}