#include <boost/format.hpp>
#include "stdhdrs.h"

#include "SyndicateManager.h"
#include "Server.h"
#include "CmdMsg.h"
#include "doFunc.h"
#include "DBManager.h"

#include "../ShareLib/packetType/ptype_syndicate.h"

//////////////////////////////////////////
// �����丮 �α�
SyndicateHistoryManager::SyndicateHistoryManager(CPC* pc)
	:_owner(pc)
{
}

SyndicateHistoryManager::~SyndicateHistoryManager()
{
}

void SyndicateHistoryManager::initDBLoading(int syndicateType, SYNDICATE_HISTORY& historys)
{
	pushHistory(historys);
}

void SyndicateHistoryManager::joinHistory(int syndicateType)
{
	SYNDICATE_HISTORY historyDatas;

	historyDatas.syndicateType	= syndicateType;
	historyDatas.time			= gserver->getNowSecond();
	historyDatas.grade			= 0;
	historyDatas.type			= SYNDICATE::JOIN;
	historyDatas.targetSyndicateType = SYNDICATE::eSYNDICATE_NONE;
	historyDatas.targetName = "";

	pushHistory(historyDatas);
}

void SyndicateHistoryManager::removeHistory(int syndicateType)
{
	SYNDICATE_HISTORY historyDatas;

	historyDatas.syndicateType	= syndicateType;
	historyDatas.time			= gserver->getNowSecond();
	historyDatas.grade			= 0;
	historyDatas.type			= SYNDICATE::REMOVE;
	historyDatas.targetSyndicateType = SYNDICATE::eSYNDICATE_NONE;
	historyDatas.targetName = "";

	pushHistory(historyDatas);
}

void SyndicateHistoryManager::degradeHistory(int syndicateType, int grade)
{
	SYNDICATE_HISTORY historyDatas;

	historyDatas.syndicateType	= syndicateType;
	historyDatas.time			= gserver->getNowSecond();
	historyDatas.grade			= grade;

	historyDatas.type			= SYNDICATE::DEGRADE;
	historyDatas.targetSyndicateType = SYNDICATE::eSYNDICATE_NONE;
	historyDatas.targetName = "";

	pushHistory(historyDatas);
}

void SyndicateHistoryManager::promotionHistory(int syndicateType, int grade)
{
	SYNDICATE_HISTORY historyDatas;

	historyDatas.syndicateType	= syndicateType;
	historyDatas.time			= gserver->getNowSecond();
	historyDatas.grade			= grade;

	historyDatas.type			= SYNDICATE::PROMOTION;
	historyDatas.targetSyndicateType = SYNDICATE::eSYNDICATE_NONE;
	historyDatas.targetName = "";

	pushHistory(historyDatas);
}

void SyndicateHistoryManager::battleWinHistory(int syndicateType, int targetSyndicateType, const char* targetName)
{
	SYNDICATE_HISTORY historyDatas;

	historyDatas.syndicateType	= syndicateType;
	historyDatas.time			= gserver->getNowSecond();
	historyDatas.grade			= 0;

	historyDatas.type			= SYNDICATE::WIN;
	historyDatas.targetSyndicateType = targetSyndicateType;
	historyDatas.targetName = targetName;

	pushHistory(historyDatas);
}

void SyndicateHistoryManager::battleLoseHistory(int syndicateType, int targetSyndicateType, const char* targetName)
{
	SYNDICATE_HISTORY historyDatas;

	historyDatas.syndicateType	= syndicateType;
	historyDatas.time			= gserver->getNowSecond();
	historyDatas.grade			= 0;

	historyDatas.type			= SYNDICATE::LOSE;
	historyDatas.targetSyndicateType = targetSyndicateType;
	historyDatas.targetName = targetName;

	pushHistory(historyDatas);
}

void SyndicateHistoryManager::sendHistory(int syndicateType)
{
	HISTORY_DATAS::iterator it = historys_.find(syndicateType);
	if (it == historys_.end())
		return;

	CNetMsg::SP rmsg(new CNetMsg);
	UpdateClient::SyndicateHistoryData* packet = reinterpret_cast<UpdateClient::SyndicateHistoryData*>(rmsg->m_buf);
	packet->type	= MSG_RVR;
	packet->subType	= MSG_UPDATE_SYNDICATE_HISTORY_LOG;

	HISTORY_DATA_ELE& tempHistorys = it->second;
	packet->count	= tempHistorys.size();
	for (int i = 0; i < packet->count; ++i)
	{
		packet->data[i].syndicateType		= tempHistorys[i].syndicateType;
		packet->data[i].time				= tempHistorys[i].time;
		packet->data[i].grade				= tempHistorys[i].grade;
		packet->data[i].type				= tempHistorys[i].type;
		packet->data[i].targetSyndicateType	= tempHistorys[i].targetSyndicateType;
		strncpy(packet->data[i].targetName, tempHistorys[i].targetName.c_str(), MAX_CHAR_NAME_LENGTH);
		packet->data[i].targetName[MAX_CHAR_NAME_LENGTH] = '\0';
	}
	rmsg->setSize(sizeof(UpdateClient::SyndicateHistoryData) + packet->count * sizeof(UpdateClient::SyndicateHistoryData::historys));
	SEND_Q(rmsg, _owner->m_desc);
}

void SyndicateHistoryManager::save(std::vector<std::string>& vec)
{
	std::string deleteStr = boost::str(boost::format(
										   "DELETE FROM t_syndicate_history WHERE a_char_index = %1%" ) % _owner->m_index);

	vec.push_back(deleteStr);

	HISTORY_DATAS::iterator it = historys_.begin();
	HISTORY_DATAS::iterator endit = historys_.end();

	for (; it != endit; ++it)
	{
		std::string insertStr = "INSERT INTO t_syndicate_history VALUES";
		insertStr.reserve(10240);

		HISTORY_DATA_ELE& historyData = it->second;
		for(int i = 0; i < historyData.size(); ++i)
		{
			insertStr += boost::str(
							 boost::format("(%1%, %2%, %3%, %4%, %5%, '%6%', FROM_UNIXTIME(%7%)),")
							 % _owner->m_index % historyData[i].syndicateType % historyData[i].type % historyData[i].grade
							 % historyData[i].targetSyndicateType % historyData[i].targetName % historyData[i].time);
		}

		int index = insertStr.rfind(",");
		insertStr.erase(index);

		vec.push_back(insertStr);
	}
}

void SyndicateHistoryManager::pushHistory( SYNDICATE_HISTORY& data )
{
	int type = data.syndicateType;

	if(historys_[type].size() >= SYNDICATE_HISTORY_MAX)
	{
		historys_[type].pop_front();
	}

	historys_[type].push_back(data);
}


//////////////////////////////////////////
// Syndicate Manager

SyndicateManager::SyndicateManager(CPC* pc)
	:_owner(pc), historyManager_(pc)
{
}

SyndicateManager::~SyndicateManager()
{
}

void SyndicateManager::initHistoryDBLoading(int syndicateType, SYNDICATE_HISTORY& historys)
{
	if (	syndicateType != SYNDICATE::eSYNDICATE_NONE
			&&	syndicateType != SYNDICATE::eSYNDICATE_FLOR
	   )
	{
		historyManager_.initDBLoading(syndicateType, historys);
	}
}

bool SyndicateManager::isFirstJoin(int syndicateType)
{
	if( syndicateData_.syndicate_join_bit & (1 << syndicateType) )
		return false;

	return true;
}

void SyndicateManager::joinSyndicate(int syndicateType)
{
	if (_owner == NULL)
		return;

	if(_owner->IsParty())
	{
		CNetMsg::SP rmsg(new CNetMsg);
		PartyMsg(rmsg, MSG_PARTY_QUIT);
		do_Party(_owner, rmsg);
	}

	if(_owner->IsExped())
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExpedQuitReqMsg(rmsg, 0);
		do_Expedition(_owner, rmsg);
	}

	LONGLONG point = 0;
	bool bFirstJoin = false;
	int grade = 0;

	_owner->m_syndicateManager.setSyndicateType(syndicateType);

	if (isFirstJoin(syndicateType))
	{
		int increasePoint = getContributionPointByJoin(syndicateType);

		if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
		{
			syndicateData_.syndicate_point_d += increasePoint;
			syndicateData_.syndicate_grade_d = SYNDICATE::getSyndicateGrade(syndicateType, syndicateData_.syndicate_point_d);
			point = syndicateData_.syndicate_point_d;
			grade = syndicateData_.syndicate_grade_d;
			syndicateData_.syndicate_join_bit |= (1 << SYNDICATE::eSYNDICATE_DEALERMOON);
		}
		else if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
		{
			syndicateData_.syndicate_point_k += increasePoint;
			syndicateData_.syndicate_grade_k = SYNDICATE::getSyndicateGrade(syndicateType, syndicateData_.syndicate_point_k);
			point = syndicateData_.syndicate_point_k;
			grade = syndicateData_.syndicate_grade_k;
			syndicateData_.syndicate_join_bit |= (1 << SYNDICATE::eSYNDICATE_KAILUX);
		}
		bFirstJoin = true;
	}
	else
	{
		if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
		{
			point = syndicateData_.syndicate_point_d;
		}
		else if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
		{
			point = syndicateData_.syndicate_point_k;
		}
		grade = SYNDICATE::getSyndicateGrade(syndicateType, point);
		this->setSyndicateGrade(syndicateType, grade);
	}

	// â���� ������ ���ų� ���ٸ� 10�� �϶�
	// â���� ������ ���ٸ� 10�� ���

	// ���� ���� helper�� ����
	{
		CNetMsg::SP joinMsg(new CNetMsg);
		UpdateServer::HelperServerRVRJoinRequest(joinMsg, syndicateType);
		SEND_Q(joinMsg, gserver->m_helper);
	}

	//  ���� �α�
	historyManager_.joinHistory(syndicateType);

	// �⿩��
	{
		CNetMsg::SP pointMsg(new CNetMsg);
		UpdateClient::makeUpdateSyndicateContributePoint(pointMsg, syndicateType, bFirstJoin, point);
		SEND_Q(pointMsg, _owner->m_desc);
	}
	// ���
	{
		CNetMsg::SP kindMsg(new CNetMsg);
		UpdateClient::makeUpdateSyndicateKind(kindMsg, syndicateType, grade, _owner->m_index);
		SEND_Q(kindMsg, _owner->m_desc);
		_owner->m_pArea->SendToCell(kindMsg, _owner, false);
	}
	// send
	{
		CNetMsg::SP errMsg(new CNetMsg);
		ResponseClient::makeSyndicateErrorMsg(errMsg, ResponseClient::ERROR_SYNDICATE_JOIN_SUCCESS, syndicateType);
		SEND_Q(errMsg, _owner->m_desc);
	}

	// �����丮 ����
	historyManager_.sendHistory(syndicateType);

	if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
	{
		gserver->m_kailuxUser.insert(this->_owner);
	}
	else if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
	{
		gserver->m_dealermoonUser.insert(this->_owner);
	}

	applySyndicateSkill(syndicateType);

	{
		LOG_INFO("JOIN_FACTION name[%s], faction_type[%d], point[%d]", _owner->GetName(), syndicateType, getSyndicatePoint(syndicateType));
	}
}

void SyndicateManager::removeSyndicate(CItem * item)
{
	if (_owner == NULL)
		return;

	int syndicateType = _owner->getSyndicateType();
	
	if(item == NULL)
	{
		const LONGLONG pay_penalty = SYNDICATE_OUT_NEEDNAS;
		if (_owner->m_inventory.getMoney() < pay_penalty)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ResponseClient::makeSyndicateErrorMsg(rmsg, ResponseClient::ERROR_SYNDICATE_REMOVE_FAIL_NOT_ENOUGH_MONEY, syndicateType);
			SEND_Q(rmsg, _owner->m_desc);
			return;
		}
		_owner->m_inventory.decreaseMoney(pay_penalty);

		LOG_INFO("LEAVE_FACTION name[%s], faction_type[%d], point[%d], lostpoint[%d]", _owner->GetName(), syndicateType, 0, getSyndicatePoint(syndicateType));
		resetSyndicatePoint();
	}
	else
	{
		_owner->m_inventory.decreaseItemCount(item, 1);
		LOG_INFO("LEAVE_FACTION name[%s], faction_type[%d], point[%d], lostpoint[%d]", _owner->GetName(), syndicateType, getSyndicatePoint(syndicateType), 0);
	}

	int questIndex[4] = {682, 683, 684, 685};
	for (int i = 0; i < 4; ++i)
	{
		_owner->m_questList.DelQuest(_owner, questIndex[i]);
	}
	
	{
		CNetMsg::SP rmsg(new CNetMsg);
		UpdateServer::HelperServerRVRLeaveRequest(rmsg, syndicateType);
		SEND_Q(rmsg, gserver->m_helper);
	}

	sendLeaveCreateJewel(_owner->getSyndicateType());

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makeSyndicateErrorMsg(rmsg, ResponseClient::ERROR_SYNDICATE_REMOVE_SUCCESS, _owner->getSyndicateType());
		SEND_Q(rmsg, _owner->m_desc);
	}

	if(_owner->getSyndicateType() == SYNDICATE::eSYNDICATE_KAILUX)
		gserver->m_kailuxUser.erase(_owner);
	else if(_owner->getSyndicateType() == SYNDICATE::eSYNDICATE_DEALERMOON)
		gserver->m_dealermoonUser.erase(_owner);

	sendRemove();

	{
		CNetMsg::SP rmsg(new CNetMsg);
		QuestPCListMsg(rmsg, _owner);
		SEND_Q(rmsg, _owner->m_desc);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		QuestCompleteListMsg(rmsg, _owner);
		SEND_Q(rmsg, _owner->m_desc);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		QuestAbandonListMsg(rmsg, _owner);
		SEND_Q(rmsg, _owner->m_desc);
	}

	_owner->m_assist.CureBySkillIndex(1756);
	_owner->m_assist.CureBySkillIndex(1757);

	CZone* pZone = gserver->FindZone(ZONE_RVR);
	if (pZone == NULL)
		return;

	int extra = 0;

	GoZone(_owner,
		ZONE_RVR,
		pZone->m_zonePos[extra][0],
		GetRandom(pZone->m_zonePos[extra][1], pZone->m_zonePos[extra][3]) / 2.0f,
		GetRandom(pZone->m_zonePos[extra][2], pZone->m_zonePos[extra][4]) / 2.0f);
}

void SyndicateManager::setSyndicatePoint(LONGLONG point)
{
	int syndicateType = _owner->getSyndicateType();

	if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
	{
		syndicateData_.syndicate_point_d = point;
	}
	else if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
	{
		syndicateData_.syndicate_point_k = point;
	}

	changeGrade(syndicateType);
}

LONGLONG SyndicateManager::getSyndicatePoint(int syndicateType)
{
	if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
		return syndicateData_.syndicate_point_d;
	else if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
		return syndicateData_.syndicate_point_k;
	else
		return -1;
}

void SyndicateManager::increaseSyndicatePoint(LONGLONG point)
{
	int syndicateType = _owner->getSyndicateType();

	if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
	{
		LONGLONG tmp = (LONGLONG)syndicateData_.syndicate_point_d + point;
		syndicateData_.syndicate_point_d += point;
	}
	else if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
	{
		LONGLONG tmp = (LONGLONG)syndicateData_.syndicate_point_k + point;
		syndicateData_.syndicate_point_k += point;
	}

	changeGrade(syndicateType);

	{
		CNetMsg::SP rmsg(new CNetMsg);
		UpdateClient::makeUpdateSyndicateIncreaseSysMsg(rmsg, syndicateType, point);
		SEND_Q(rmsg, _owner->m_desc);
	}
}

void SyndicateManager::decreaseSyndicatePoint(LONGLONG point)
{
	int syndicateType = _owner->getSyndicateType();

	if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
	{
		syndicateData_.syndicate_point_d -= point;
		if(syndicateData_.syndicate_point_d <= 0)
			syndicateData_.syndicate_point_d = 0;
	}
	else if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
	{
		syndicateData_.syndicate_point_k -= point;
		if(syndicateData_.syndicate_point_k <= 0)
			syndicateData_.syndicate_point_k = 0;
	}

	changeGrade(syndicateType);
}

void SyndicateManager::resetSyndicatePoint()
{
	int syndicateType = _owner->getSyndicateType();

	if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
		syndicateData_.syndicate_point_d = 0;

	else if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
		syndicateData_.syndicate_point_k = 0;

	changeGrade(syndicateType);
}

int SyndicateManager::getSyndicateGrade(int syndicateType)
{
	if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
		return syndicateData_.syndicate_grade_d;

	else if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
		return syndicateData_.syndicate_grade_k;

	return 0;
}

void SyndicateManager::syndicateAddJewel(int vItemIndex)
{
	CItem* pItem = _owner->m_inventory.FindByVirtualIndex(vItemIndex);
	if (pItem == NULL)
	{
		LOG_ERROR("HACKING : not found item. charIndex[%d]", _owner->m_index);
		_owner->m_desc->Close("not found item");
		return;
	}

	// �������� ��� �������� �´����� ���� üũ �ʿ�
	if (pItem->m_itemProto->isRvrJewel() == false)
	{
		LOG_ERROR("HACKING : not rvr_jewel item. charIndex[%d]", _owner->m_index);
		_owner->m_desc->Close("not rvr_jewel item");
		return;
	}

	int syndicateType = _owner->getSyndicateType();

	// â���� ������ ���� �⿩�� ����
	_owner->m_syndicateManager.increaseSyndicatePoint((LONGLONG)pItem->m_itemProto->getItemNum0());

	CNetMsg::SP rmsg(new CNetMsg);
	ResponseClient::makeSyndicateErrorMsg(rmsg, ResponseClient::ERROR_SYNICATE_GIVE_SUCCESS, syndicateType);
	SEND_Q(rmsg, _owner->m_desc);

	int count = pItem->m_itemProto->getItemNum0();

	// â���� ���� ���� -> helper���� ó��
	CNetMsg::SP sendHelpeMsg(new CNetMsg);
	UpdateServer::HelperServerRVRIncreaseCreateJewel(sendHelpeMsg, _owner->getSyndicateType(), count);
	SEND_Q(sendHelpeMsg, gserver->m_helper);

	LOG_INFO("FACTION_CONTRIBUTE > name[%s], itemIndex[%d], AddPoint[%d], TotalPoint[%lld]", _owner->GetName(), pItem->getDBIndex(), count, _owner->m_syndicateManager.getSyndicatePoint(_owner->getSyndicateType()) + count);

	// ������ ����
	_owner->m_inventory.deleteItemByItem(pItem);
}

void SyndicateManager::syndicateDelJewel( int count )
{
	// â���� ���� ���� -> helper���� ó��
	CNetMsg::SP sendHelpeMsg(new CNetMsg);
	UpdateServer::HelperServerRVRDecreaseCreateJewel(sendHelpeMsg, _owner->getSyndicateType(), count);
	SEND_Q(sendHelpeMsg, gserver->m_helper);
}


void SyndicateManager::send()
{
	int syndicateType = _owner->getSyndicateType();
	int point = 0;
	int grade = 0;

	if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
	{
		point = syndicateData_.syndicate_point_d;
		grade = syndicateData_.syndicate_grade_d;
	}
	else if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
	{
		point = syndicateData_.syndicate_point_k;
		grade = syndicateData_.syndicate_grade_k;
	}

	// �⿩�� ����
	CNetMsg::SP pointMsg(new CNetMsg);
	UpdateClient::makeUpdateSyndicateContributePoint(pointMsg, syndicateType, false, point);
	SEND_Q(pointMsg, _owner->m_desc);

	// ���
	CNetMsg::SP gradeMsg(new CNetMsg);
	UpdateClient::makeUpdateSyndicateKind(gradeMsg, syndicateType, grade, _owner->m_index);
	SEND_Q(gradeMsg, _owner->m_desc);
	_owner->m_pArea->SendToCell(gradeMsg, _owner, false);

	historyManager_.sendHistory(syndicateType);
}

void SyndicateManager::save(std::vector<std::string>& vec)
{
	std::string updateStr = boost::str(boost::format(
										   "UPDATE t_characters set a_syndicate_type = %d, a_syndicate_point_k = %lld, a_syndicate_point_d = %lld, a_syndicate_join_bit = %d where a_index = %d")
									   % syndicateData_.syndicateType % syndicateData_.syndicate_point_k % syndicateData_.syndicate_point_d % syndicateData_.syndicate_join_bit % _owner->m_index);

	vec.push_back(updateStr);

	// history DB ����
	historyManager_.save(vec);
}

void SyndicateManager::changeGrade(int syndicateType)
{
	int oldGrade = 0;
	int newGrade = 0;
	LONGLONG point = 0;

	oldGrade = getSyndicateGrade(syndicateType);
	point = getSyndicatePoint(syndicateType);
	newGrade = SYNDICATE::getSyndicateGrade(syndicateType, point);

	if ( (oldGrade != newGrade) && (isKing() == false) )
	{
		if (oldGrade < newGrade)
		{
			// �±� �α�
			historyManager_.promotionHistory(syndicateType, newGrade);

			//�������� �±��ߴٸ� ������ ���۰��� ����Ʈ�� �߰�
			if(	(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON && newGrade == SYNDICATE::MAGUS) ||
					(syndicateType == SYNDICATE::eSYNDICATE_KAILUX && newGrade == SYNDICATE::DUKA)	)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				UpdateServer::HelperServerRVRAdd2ndUser(rmsg, syndicateType, _owner->m_index, point, _owner->GetName());
				SEND_Q(rmsg, gserver->m_helper);
			}
		}
		else
		{
			// ���� �α�
			historyManager_.degradeHistory(syndicateType, newGrade);

			//���ۿ��� ������Ѵٸ� ������ ���۰��� ����Ʈ���� ����
			if(	(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON && oldGrade == SYNDICATE::MAGUS) ||
					(syndicateType == SYNDICATE::eSYNDICATE_KAILUX && oldGrade == SYNDICATE::DUKA)	)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				UpdateServer::HelperServerRVRDel2ndUser(rmsg, syndicateType, _owner->m_index);
				SEND_Q(rmsg, gserver->m_helper);
			}
		}

		setSyndicateGrade(syndicateType, newGrade);
		_owner->m_syndicateManager.send();

		applySyndicateSkill(syndicateType);
	}
	else
	{
		//����� ���� �̻��̶�� ������ ���۷� �⿩�� ��������
		if(	(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON && oldGrade >= SYNDICATE::MAGUS) ||
				(syndicateType == SYNDICATE::eSYNDICATE_KAILUX && oldGrade >= SYNDICATE::DUKA)	)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			UpdateServer::HelperServerRVRUpdate2ndUser(rmsg, syndicateType, _owner->m_index, point);
			SEND_Q(rmsg, gserver->m_helper);
		}
	}
}

int SyndicateManager::getContributionPointByJoin(int syndicateType)
{
	// ���� ���� �ο��� ���� ��
	// �������� 1000
	int joinKailuxCount		= SyndicateInfoDataManager::instance()->getJoinCount(SYNDICATE::eSYNDICATE_KAILUX);
	int joinDealerMoonCount	= SyndicateInfoDataManager::instance()->getJoinCount(SYNDICATE::eSYNDICATE_DEALERMOON);
	int increasePoint = 0;

	switch(syndicateType)
	{
	case SYNDICATE::eSYNDICATE_KAILUX:
		{
			if (joinKailuxCount < joinDealerMoonCount)
				return 1000;
			else
				return 100;
		}
		break;
	case SYNDICATE::eSYNDICATE_DEALERMOON:
		{
			if (joinKailuxCount < joinDealerMoonCount)
				return 100;
			else
				return 1000;
		}
		break;
	}
	return 0;
}

void SyndicateManager::sendJoinCreateJewel(int syndicateType)
{
	// â���� ������ ���ų� ���ٸ� 10�� �϶�
	// â���� ������ ���ٸ� 10�� ���
	// helper�� ����
	int jewelKailuxPoint		= SyndicateInfoDataManager::instance()->getJewelPoint(SYNDICATE::eSYNDICATE_KAILUX);
	int jewelDealerMoonPoint	= SyndicateInfoDataManager::instance()->getJewelPoint(SYNDICATE::eSYNDICATE_DEALERMOON);
	if (jewelKailuxPoint == 0 && jewelDealerMoonPoint == 0)
	{
		return;
	}
	int penalty_point = 0;
	bool bIncrease = false;
	switch(syndicateType)
	{
	case SYNDICATE::eSYNDICATE_KAILUX:
		{
			if (jewelKailuxPoint < jewelDealerMoonPoint)
			{
				// ����
				penalty_point = 10;
				bIncrease = true;
			}
			else
			{
				// ����
				penalty_point = 10;
			}
		}
		break;
	case SYNDICATE::eSYNDICATE_DEALERMOON:
		{
			if (jewelKailuxPoint < jewelDealerMoonPoint)
			{
				// ����
				penalty_point = 10;
			}
			else
			{
				// ����
				penalty_point = 10;
				bIncrease = true;
			}
		}
		break;
	}

	CNetMsg::SP rmsg(new CNetMsg);
	if (bIncrease)
	{
		UpdateServer::HelperServerRVRIncreaseCreateJewel(rmsg, syndicateType, penalty_point);
	}
	else
	{
		UpdateServer::HelperServerRVRDecreaseCreateJewel(rmsg, syndicateType, penalty_point);
	}
	SEND_Q(rmsg, gserver->m_helper);
}

void SyndicateManager::sendLeaveCreateJewel(int syndicateType)
{
	// â���� ������ ���ų� ���ٸ� 10�� �϶�
	// â���� ������ ���ٸ� 10�� ���
	// helper�� ����
	LONGLONG jewelKailuxPoint		= SyndicateInfoDataManager::instance()->getJewelPoint(SYNDICATE::eSYNDICATE_KAILUX);
	LONGLONG jewelDealerMoonPoint	= SyndicateInfoDataManager::instance()->getJewelPoint(SYNDICATE::eSYNDICATE_DEALERMOON);
	if (jewelKailuxPoint == 0 && jewelDealerMoonPoint == 0)
	{
		return;
	}
	int penalty_point = 0;
	bool bIncrease = false;
	switch(syndicateType)
	{
	case SYNDICATE::eSYNDICATE_KAILUX:
		{
			if (jewelKailuxPoint < jewelDealerMoonPoint)
			{
				// ����
				penalty_point = 10;
				bIncrease = true;
			}
			else
			{
				// ����
				penalty_point = 10;
			}
		}
		break;
	case SYNDICATE::eSYNDICATE_DEALERMOON:
		{
			if (jewelKailuxPoint < jewelDealerMoonPoint)
			{
				// ����
				penalty_point = 10;
			}
			else
			{
				// ����
				penalty_point = 10;
				bIncrease = true;
			}
		}
		break;
	}

	CNetMsg::SP rmsg(new CNetMsg);
	if (bIncrease)
	{
		UpdateServer::HelperServerRVRIncreaseCreateJewel(rmsg, syndicateType, penalty_point);
	}
	else
	{
		UpdateServer::HelperServerRVRDecreaseCreateJewel(rmsg, syndicateType, penalty_point);
	}
	SEND_Q(rmsg, gserver->m_helper);
}

void SyndicateManager::sendUserInfo()
{
	CNetMsg::SP rmsg(new CNetMsg);
	ResponseClient::RVRSyndicateInfoOfUser* packet = reinterpret_cast<ResponseClient::RVRSyndicateInfoOfUser*>(rmsg->m_buf);

	packet->type = MSG_RVR;
	packet->subType = MSG_RVR_SYNDICATE_INFO_USER_REP;
	packet->syndicateType = _owner->getSyndicateType();
	packet->grade = _owner->m_syndicateManager.getSyndicateGrade(_owner->getSyndicateType());
	packet->accumulateContributionPoint = _owner->m_syndicateManager.getSyndicatePoint(_owner->getSyndicateType());
	packet->maxPoint = SYNDICATE::getSyndicateGradeMaxPoint(_owner->getSyndicateType(), _owner->m_syndicateManager.getSyndicateGrade(_owner->getSyndicateType()));
	rmsg->setSize(sizeof(ResponseClient::RVRSyndicateInfoOfUser));
	SEND_Q(rmsg, _owner->m_desc);
}

void SyndicateManager::sendRemove()
{
	int syndicateType = _owner->getSyndicateType();
	int point;

	if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
	{
		point = syndicateData_.syndicate_point_d;
	}
	else if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
	{
		point = syndicateData_.syndicate_point_k;
	}

	_owner->m_syndicateManager.setSyndicateType(SYNDICATE::eSYNDICATE_NONE);

	// �⿩�� ����
	{
		CNetMsg::SP pointMsg(new CNetMsg);
		UpdateClient::makeUpdateSyndicateContributePoint(pointMsg, syndicateType, false, point);
		SEND_Q(pointMsg, _owner->m_desc);

	}
	// ���
	{
		CNetMsg::SP gradeMsg(new CNetMsg);
		UpdateClient::makeUpdateSyndicateKind(gradeMsg, _owner->getSyndicateType(), 0, _owner->m_index);
		SEND_Q(gradeMsg, _owner->m_desc);
		_owner->m_pArea->SendToCell(gradeMsg, _owner, false);

	}
	// �α� �߰� �� �����丮 ����
	{
		historyManager_.removeHistory(syndicateType);
		historyManager_.sendHistory(syndicateType);
	}
}

bool SyndicateManager::isKing()
{
	char* kingName = SyndicateInfoDataManager::instance()->getKingName(_owner->getSyndicateType());
	if(strcmp(kingName, _owner->GetName()) == 0)
	{
		return true;
	}
	return false;
}

void SyndicateManager::setSyndicateType( int syndicateType )
{
	_owner->setSyndicateType(syndicateType);
	this->syndicateData_.syndicateType = syndicateType;
}

void SyndicateManager::setSyndicateGrade( int syndicateType, int syndicateGrade )
{
	if(syndicateType == SYNDICATE::eSYNDICATE_DEALERMOON)
		syndicateData_.syndicate_grade_d = syndicateGrade;

	else if(syndicateType == SYNDICATE::eSYNDICATE_KAILUX)
		syndicateData_.syndicate_grade_k = syndicateGrade;

	else
		return;
}

void SyndicateManager::applySyndicateSkill( int syndicateType )
{
	switch(syndicateType)
	{
	case SYNDICATE::eSYNDICATE_KAILUX:
		{
			CSkill* skill;
			skill = gserver->m_skillProtoList.Create(1756, _owner->m_syndicateManager.getSyndicateGrade(_owner->getSyndicateType()));
			bool bApply = false;
			ApplySkill(_owner, _owner, skill, -1, bApply);
		}
		break;
	case SYNDICATE::eSYNDICATE_DEALERMOON:
		{
			CSkill* skill;
			skill = gserver->m_skillProtoList.Create(1757, _owner->m_syndicateManager.getSyndicateGrade(_owner->getSyndicateType()));
			bool bApply = false;
			ApplySkill(_owner, _owner, skill, -1, bApply);
		}
		break;
	}
}
