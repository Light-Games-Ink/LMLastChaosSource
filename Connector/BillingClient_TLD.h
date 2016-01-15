#include "BillMessage_TLD.h"
//////////////
// ����� Ÿ��
#define BILL_USER_FREE			0		// ���� �����

////////////
// ���� ����
#define BILL_PAY_FREE			0		// ���� �̿�

////////////
// ���� ��ġ
#define BILL_LOCATION_HOME		0		// �Ϲ�

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
// ���� Ŭ���̾�Ʈ
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
	void OnCashBalance(CNetMsg::SP& msg);
	void OnCanBuy(CNetMsg::SP& msg);
	void OnBuy(CNetMsg::SP& msg);

public:
	void WriteToOutput(CNetMsg::SP& msg);

	bool Login(const char* name, const char* pw, const char* ip);
	bool Logout(const char* name);

	virtual void operate(rnSocketIOService* service);
	virtual void onClose(rnSocketIOService* service);

	virtual void onConnect(rnSocketIOService* service);
	virtual void onConnectFail(rnSocketIOService* service);

	void connect();
};
