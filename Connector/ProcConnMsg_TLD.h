void ConnBLogin(CDescriptor* d, CNetMsg::SP& msg);
void ConnBLogout(CDescriptor* d, CNetMsg::SP& msg);
void ConnCashItemBringReq(CDescriptor* d, CNetMsg::SP& msg);
void ConnCashItemBalanceReq(CDescriptor* d, CNetMsg::SP& msg);
void ConnCashItemPurchaseReq(CDescriptor* d, CNetMsg::SP& msg);
void ConnCashItemPurchaselistReq(CDescriptor* d, CNetMsg::SP& msg);
void ConnCashItemPurchaseHistoryReq(CDescriptor* d, CNetMsg::SP& msg);
void ConnCashItemGift(CDescriptor* d, CNetMsg::SP& msg);
void ConnCashItemSendHistory(CDescriptor* d, CNetMsg::SP& msg);
void ConnCashItemRecvHistory(CDescriptor* d, CNetMsg::SP& msg);
void ConnCashItemRecvList(CDescriptor* d, CNetMsg::SP& msg);
void ConnCashItemRecv(CDescriptor* d, CNetMsg::SP& msg);
void ConnCashItemPresentHistory(CDescriptor* d, CNetMsg::SP& msg, bool bSend );
// cashItemRecv, BringReq �Ѵ� ��, �����ΰ� �ƴѰ�.
void ConnCashItemBringProcess(CDescriptor* d, CNetMsg::SP& msg, bool bPresent);
