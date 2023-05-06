#pragma once
#include<string>
#include <map>

enum class DhTaskType
{
	// 초기 상태
	None = 0,
	// 지수 선물 심볼 
	JisuFtSymbol,
	// 지수 옵션 심볼 
	JisuOpSymbol,
	// 상품 선물 심볼 
	CommodityFtSymbol,
	// 미니 코스피 선물 심볼 
	MiniFtSymbol,
	// 미니 코스피 선물 옵션 심볼 
	MiniOpSymbol,
	// EFT 심볼 받음
	EtfSymbol,
	/// <summary>
	/// 위클리 옵션 심볼 받음
	/// </summary>
	WeeklyOpSymbol,
	// 해외 선물 심볼 목록 받음
	AbroadSymbol,
	EbSymbolCode, // 심볼 코드
	EbSymbolMaster, // 심볼 마스터
	EBAccountAsset, // 예탁 잔고
	EBAccountProfitLoss, // 계좌별 손익
	EBProductRemain, // 종목별 잔고와 평가 손익
	EBAccountOrder, // 계좌별 주문 내역
	HdSymbolMaster, // 해외 종목 정보
	HdSymbolSise, // 해외 종목 시세
	HdSymbolHoga, // 해외 종목 호가
	HdSymbolChartData, // 해외 종목 차트 데이터
	HdAccountAsset, // 해외 계좌 
	HdAccountProfitLoss, // 해외 계좌 손익
	HdAccountSymbolRemain, // 해외 종목 잔고
	HdAcceptedOrderList, // 해외 미체결 주문
	HdFilledOrderList, // 해외 체결 주문
	HdFilledDetail // 해외 체결 주문
};

struct DhTaskArg {
	// 요청 아이디
	int ArgId = 0;
	// 요청했는지 여부
	bool Requested = false;
	// 받았는지 여부
	bool Received = false;
	DhTaskType TaskType = DhTaskType::None;
	// 작업 설명
	std::string Desc;
	// 작업 내용
	std::string Detail;
	// 매개변수 맵
	std::map<std::string, std::string> ArgMap;

	void AddArg(std::string key, std::string value) {
		ArgMap[key] = value;
	}

	std::string GetArg(std::string key) {
		std::string value;
		auto it = ArgMap.find(key);
		if (it != ArgMap.end()) {
			value = it->second;
		}

		return value;
	}
};

struct DhTaskInfo {
	DhTaskType TaskType;
	std::string TaskTitle;
	std::string TaskDetail;
	int TotalTaskCount;
	int RemainTaskCount;
	std::map<int, DhTaskArg> argMap;
};
