#pragma once
#include<string>
#include <map>

enum class DhTaskType
{
	// �ʱ� ����
	None = 0,
	// ���� ���� �ɺ� 
	JisuFtSymbol,
	// ���� �ɼ� �ɺ� 
	JisuOpSymbol,
	// ��ǰ ���� �ɺ� 
	CommodityFtSymbol,
	// �̴� �ڽ��� ���� �ɺ� 
	MiniFtSymbol,
	// �̴� �ڽ��� ���� �ɼ� �ɺ� 
	MiniOpSymbol,
	// EFT �ɺ� ����
	EtfSymbol,
	/// <summary>
	/// ��Ŭ�� �ɼ� �ɺ� ����
	/// </summary>
	WeeklyOpSymbol,
	// �ؿ� ���� �ɺ� ��� ����
	AbroadSymbol,
	EbSymbolCode, // �ɺ� �ڵ�
	EbSymbolMaster, // �ɺ� ������
	EBAccountAsset, // ��Ź �ܰ�
	EBAccountProfitLoss, // ���º� ����
	EBProductRemain, // ���� �ܰ�� �� ����
	EBAccountOrder, // ���º� �ֹ� ����
	HdSymbolMaster, // �ؿ� ���� ����
	HdSymbolSise, // �ؿ� ���� �ü�
	HdSymbolHoga, // �ؿ� ���� ȣ��
	HdSymbolChartData, // �ؿ� ���� ��Ʈ ������
	HdAccountAsset, // �ؿ� ���� 
	HdAccountProfitLoss, // �ؿ� ���� ����
	HdAccountSymbolRemain, // �ؿ� ���� �ܰ�
	HdAcceptedOrderList, // �ؿ� ��ü�� �ֹ�
	HdFilledOrderList, // �ؿ� ü�� �ֹ�
	HdFilledDetail // �ؿ� ü�� �ֹ�
};

struct DhTaskArg {
	// ��û ���̵�
	int ArgId = 0;
	// ��û�ߴ��� ����
	bool Requested = false;
	// �޾Ҵ��� ����
	bool Received = false;
	DhTaskType TaskType = DhTaskType::None;
	// �۾� ����
	std::string Desc;
	// �۾� ����
	std::string Detail;
	// �Ű����� ��
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
