#pragma once
#include "SmTaskTypeDef.h"
#include <any>
#include <string>
#include "../Chart/SmChartConsts.h"

namespace DarkHorse {
	struct SmChartDataReq {
		int TaskId = 0;
		std::string SymbolCode;
		SmChartType ChartType = SmChartType::MIN;
		int Cycle = 0;
		int Count = 0;
		int Next = 0;
		int PrevKey = 0;
		bool Requested = false;
		int WindowId = -1;
		int SeriesIndex = 0;
	};
	struct SmTaskArg {
		SmTaskType TaskType = SmTaskType::None;
		std::any Param;
	};
}