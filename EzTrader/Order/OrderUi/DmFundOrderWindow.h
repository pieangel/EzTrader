#pragma once
namespace DarkHorse {
/// <summary>
/// DmFundOrderWindow 성성될 때 OrderWindowControl을 같이 생성한다.
/// 생성한 OrderWindowControl은 id와 함께 EventHub에 등록한다. 
/// 이후 OrderWindowControl에서 발생하는 모든 이벤트는 DmFundOrderWindow에 전달 되며
/// DmFundOrderWindow는 이벤트에 따라 화면을 갱신할 의무를 가지게 된다. 
/// 모든 데이터변화로 비지니스 로직은 OrderWindowControl에서 책임지게 된다. 
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

