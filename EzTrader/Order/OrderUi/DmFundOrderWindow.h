#pragma once
namespace DarkHorse {
/// <summary>
/// DmFundOrderWindow ������ �� OrderWindowControl�� ���� �����Ѵ�.
/// ������ OrderWindowControl�� id�� �Բ� EventHub�� ����Ѵ�. 
/// ���� OrderWindowControl���� �߻��ϴ� ��� �̺�Ʈ�� DmFundOrderWindow�� ���� �Ǹ�
/// DmFundOrderWindow�� �̺�Ʈ�� ���� ȭ���� ������ �ǹ��� ������ �ȴ�. 
/// ��� �����ͺ�ȭ�� �����Ͻ� ������ OrderWindowControl���� å������ �ȴ�. 
/// </summary>
class DmFundOrderWindow
{
public:
	DmFundOrderWindow();
	virtual ~DmFundOrderWindow();
private:
	int _id;
};
}

