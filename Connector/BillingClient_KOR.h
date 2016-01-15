#include "BillMessage_KOR.h"
//////////////
// 사용자 타입
#define BILL_USER_FREE			0		// 무료 사용자

////////////
// 지불 형식
#define BILL_PAY_FREE			0		// 무료 이용

#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/rnsocketioservice.h"
#include "../ShareLib/BPacket.h"
#include "../ShareLib/DBCmd.h"

class CBillingClientConnectTimer : public rnSocketIOHandler
{
public:
	CBillingClientConnectTimer() {}
	~CBillingClientConnectTimer() {}

	virtual void operate(rnSocketIOService* service);
	static CBillingClientConnectTimer* instance();
};

//////////////////
// 빌링 클라이언트
class CBillingClient : public rnSocketIOHandler
{
public:
	rnSocketIOService*	service_;

	CBillingClient();

	bool IsRunning()
	{
		return (service_ != NULL);
	}

private:
	void OnCashBalance(CBPacket::SP& msg);
	void OnCanBuy(CBPacket::SP& msg);
	void OnBuy(CBPacket::SP& msg);

public:
	void WriteToOutput(CBPacket::SP msg);

	bool Login(const char* name, const char* pw, const char* ip);
	bool Logout(const char* name);

	virtual void operate(rnSocketIOService* service);
	virtual void onClose(rnSocketIOService* service);

	virtual void onConnect(rnSocketIOService* service);
	virtual void onConnectFail(rnSocketIOService* service);

	void connect();
};
