#include <boost/format.hpp>
#include "stdhdrs.h"

#include "DBManager.h"
#include "Descriptor.h"
#include "TimerItemManager.h"

//////////////////////////////////////////////////////////////////////////
TimerItemManager::set_t		TimerItemManager::static_set_;
//////////////////////////////////////////////////////////////////////////

TimerItemManager::TimerItemManager()
{
	owner_ = NULL;

	for (int i = 0; i < TIMERITEM_MAX; ++i)
	{
		flag_[i] = false;
	}
}

TimerItemManager::~TimerItemManager(void)
{
	static_set_.erase(this);
}

// �ʱ� ĳ���� �����͸� �о���϶��� �����
bool TimerItemManager::insert( int charIndex, int type, unsigned int time )
{
	if (type >= TIMERITEM_MAX)
	{
		LOG_ERROR("invalid type : %d. charIndex : %d", type, charIndex);
		return false;
	}

	bool b = map_.insert(map_t::value_type(type, time)).second;
	if (b == false)
	{
		LOG_ERROR("TimerItem : ERROR : duplication type : %d. charIndex : %d", type, charIndex);
		return false;
	}

	flag_[type] = true;

	return true;
}

unsigned int TimerItemManager::appendByDay( int type, int day )
{
	int seconds = day * 60 * 60 * 24;
	return appendBySeccond(type, seconds);
}

unsigned int TimerItemManager::appendBySeccond(int type, int seconds)
{
	int ret = 0;

	map_t::iterator it = map_.find(type);
	if (it != map_.end())
	{
		// ������ ���� ��� �ð��� ������Ų��.
		it->second += seconds;
		ret = it->second;
	}
	else
	{
		// ���� ��� ���� �ð� + ��¥�� ����Ͽ� ����ð��� ���Ѵ�.
		seconds += time(0);
		map_.insert(map_t::value_type(type, seconds));
		ret = seconds;

		// ���ο� �Ⱓ�� �������̹Ƿ� ��� �����Ѵ�.
		std::string qry = boost::str(boost::format(
										 "INSERT INTO t_timer_item VALUES(%1%,%2%,FROM_UNIXTIME(%3%))")
									 % owner_->m_index % type % seconds);
		DBManager::instance()->pushQuery(owner_->m_desc->m_index, qry);
	}

	flag_[type] = true;
	static_set_.insert(this);

	CNetMsg::SP rmsg(new CNetMsg);
	ResponseClient::makeTimerItemModify(rmsg, type, ret);
	SEND_Q(rmsg, owner_->m_desc);

	return ret;
}

void TimerItemManager::deleteDataInDB( int type )
{
	flag_[type] = false;

	CNetMsg::SP rmsg(new CNetMsg);
	ResponseClient::makeTiemrItemDelete(rmsg, type);
	SEND_Q(rmsg, owner_->m_desc);

	// db manger�� ������ DB table���� �����͸� �����Ѵ�.
	std::string query = boost::str(boost::format(
									   "DELETE FROM t_timer_item WHERE a_char_index=%1% AND a_type_index=%2%")
								   % owner_->m_index % type);
	DBManager::instance()->pushQuery(owner_->m_desc->m_index, query);

	switch (type)
	{
	case TIMERITEM_TYPE_PET_STASH:
		owner_->m_petStashManager.ExpireTime();
		break;
	}
}

unsigned int TimerItemManager::getTime( int type )
{
	map_t::iterator it = map_.find(type);
	return (it != map_.end()) ? it->second : 0;
}

void TimerItemManager::sendListToClient()
{
	CNetMsg::SP rmsg(new CNetMsg);
	ResponseClient::listTimerItem* p = reinterpret_cast<ResponseClient::listTimerItem*>(rmsg->m_buf);
	p->type = MSG_TIMER_ITEM;
	p->subType = MSG_SUB_TIMER_ITEM_LIST;
	p->count = map_.size();
	if (p->count > 0)
	{
		map_t::iterator it = map_.begin();
		map_t::iterator endit = map_.end();
		for (int i = 0; it != endit; ++it, ++i)
		{
			p->list[i].type = it->first;
			p->list[i].expire_time = it->second;
		}
	}
	int bodySize = sizeof(ResponseClient::listTimerItem);
	int listSize = p->count * sizeof(ResponseClient::listTimerItem::_list);
	rmsg->setSize(bodySize + listSize);
	SEND_Q(rmsg, owner_->m_desc);
}

void TimerItemManager::setOwner( CPC* owner )
{
	owner_ = owner;

	if (map_.empty() == false)
	{
		static_set_.insert(this);
	}
}

void TimerItemManager::getDataToQuery( std::vector<std::string>& vec )
{
	map_t::iterator it = map_.begin();
	map_t::iterator endit = map_.end();
	for (; it != endit; ++it)
	{
		std::string str = boost::str(boost::format(
										 "UPDATE t_timer_item SET a_expire_time=FROM_UNIXTIME(%1%) WHERE a_char_index=%2% AND a_type_index=%3%")
									 % it->second % owner_->m_index % it->first);
		vec.push_back(str);
	}
}

void TimerItemManager::checkExpireTime( unsigned int nowtime )
{
	map_t::iterator it = map_.begin();
	for ( ; it != map_.end(); )
	{
		if (it->second <= nowtime)
		{
			this->deleteDataInDB(it->first);
			map_.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void TimerItemManager::AllcheckExpireTime( unsigned int nowtime )
{
	set_t::iterator it = static_set_.begin();
	for (; it != static_set_.end(); )
	{
		if ((*it)->isRun() == false)
			continue;

		(*it)->checkExpireTime(nowtime);
		if ((*it)->empty())
		{
			static_set_.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

// GM��ɾ���� ���
// nowtime + sec���� �ð��� ����
void TimerItemManager::setTime( int type, int sec )
{
	if (type <= TIMERITEM_NON || type >= TIMERITEM_MAX)
	{
		LOG_ERROR("invalid type[%d]. charIndex[%d]", type, owner_->m_index);
		return;
	}

	map_t::iterator it = map_.find(type);
	if (it != map_.end())
	{
		// ������ ���� ��쿡�� ���� �ð��� ���ο� �ð����� �����Ѵ�.
		it->second = time(0) + sec;

		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makeTimerItemModify(rmsg, type, it->second);
		SEND_Q(rmsg, owner_->m_desc);
	}
	else
	{
		// ���� ��쿡�� ���� ������ش�.
		this->appendBySeccond(type, sec);
	}
}

bool TimerItemManager::isRemoteExpressSystem()
{
#ifdef PREMIUM_CHAR
	//XX - �����̾�ĳ���� : ���� LCE
	return (owner_->m_premiumChar.isActive() || flag_[TIMETITEM_TYPE_REMOTE_EXPRESS_SYSTEM]);
#else
	return flag_[TIMETITEM_TYPE_REMOTE_EXPRESS_SYSTEM];
#endif
}

bool TimerItemManager::isCashInventory_1()
{
#ifdef PREMIUM_CHAR
	//XX - �����̾�ĳ���� : ù��° ĳ�� �κ��丮
	return (owner_->m_premiumChar.isActive() || flag_[TIMERITEM_TYPE_CASH_INVENTORY_1]);
#else
	return flag_[TIMERITEM_TYPE_CASH_INVENTORY_1];
#endif
}
