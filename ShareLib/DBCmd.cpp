#include <boost/format.hpp>
#include "bnf.h"
#include "logsystem.h"
#include "DBCmd.h"
#include "PrintExcuteInfo.h"

CDBCmd::CDBCmd()
	: m_dbconn(NULL)
	, m_nrecords(0)
	, m_res(NULL)
	, m_row(NULL)
	, m_fieldinfo(NULL)
	, m_nfields(0)
{
}

CDBCmd::CDBCmd(MYSQL* mysql)
	: m_dbconn(mysql)
	, m_nrecords(0)
	, m_res(NULL)
	, m_row(NULL)
	, m_fieldinfo(NULL)
	, m_nfields(0)
{
}

CDBCmd::~CDBCmd()
{
	if (m_res)
	{
		mysql_free_result(m_res);
		m_res = NULL;
	}
}

bool CDBCmd::Open()
{
	Close();

	if (mysql_real_query(m_dbconn, m_sql.c_str(), m_sql.size()) != 0)
	{
		LOG_ERROR("query error (mysql_real_query) : error[%s] query[%s]", mysql_error(m_dbconn), m_sql.c_str());
		return false;
	}

	m_res = mysql_store_result(m_dbconn);
	if (m_res == NULL)
	{
		LOG_ERROR("query error (mysql_store_result) : error[%s] query[%s]", mysql_error(m_dbconn), m_sql.c_str());
		return false;
	}

	m_nrecords = (int)mysql_num_rows(m_res);
	m_nfields = (int)mysql_num_fields(m_res);
	m_fieldinfo = mysql_fetch_fields(m_res);

	map_.clear();
	for (int i = 0; i < m_nfields; ++i)
	{
		map_.insert(map_t::value_type(m_fieldinfo[i].name, i));
	}

	return true;
}

bool CDBCmd::MoveFirst()
{
	if (m_res == NULL)
		return false;

	mysql_data_seek(m_res, 0);

	m_row = mysql_fetch_row(m_res);
	if (m_row == NULL)
		return false;

	return true;
}

bool CDBCmd::Seek(int nOffset)
{
	if (m_res == NULL)
		return false;

	mysql_data_seek(m_res, nOffset);

	m_row = mysql_fetch_row(m_res);
	if (m_row == NULL)
		return false;

	return true;
}

bool CDBCmd::MoveNext()
{
	if (m_res == NULL)
		return false;

	m_row = mysql_fetch_row(m_res);
	if (m_row == NULL)
		return false;

	return true;
}

bool CDBCmd::Update()
{
	Close();

	if (mysql_real_query(m_dbconn, m_sql.c_str(), m_sql.size()) != 0)
	{
		m_nrecords = 0;
		LOG_ERROR("query error (mysql_real_query) : error[%s] query[%s]", mysql_error(m_dbconn), m_sql.c_str());
		return false;
	}

	m_nrecords = static_cast<int>(mysql_affected_rows(m_dbconn));
	return true;
}

bool CDBCmd::GetRec(int fieldno, int& n)
{
	char* ret = GetRec(fieldno);
	if (ret == NULL)
		return false;

	n = atoi(ret);
	return true;
}

bool CDBCmd::GetRec(int fieldno, unsigned int& n)
{
	int t;
	if (!GetRec(fieldno, t))
		return false;

	n = (unsigned int)t;
	return true;
}

bool CDBCmd::GetRec(int fieldno, short& n)
{
	int t;
	if (!GetRec(fieldno, t))
		return false;

	n = (short)t;
	return true;
}

bool CDBCmd::GetRec(int fieldno, unsigned short& n)
{
	int t;
	if (!GetRec(fieldno, t))
		return false;

	n = (unsigned short)t;
	return true;
}

bool CDBCmd::GetRec(int fieldno, char& ch)
{
	int t;
	if (!GetRec(fieldno, t))
		return false;

	ch = (char)t;
	return true;
}

bool CDBCmd::GetRec(int fieldno, unsigned char& ch)
{
	int t;
	if (!GetRec(fieldno, t))
		return false;

	ch = (unsigned char)t;
	return true;
}

bool CDBCmd::GetRec(int fieldno, CLCString& str, bool bTrim)
{
	// ������ �б�
	char* ret = GetRec(fieldno);

	// NULL�̸� ����
	if (ret == NULL)
		return false;

	char* p = ret;

	// left trim ó��
	if (bTrim)
	{
		while (*p && CLCString::IsSpace(*p))
			p++;
	}

	// ���� ����Ʈ ����
	char* pStart = p;

	// ���� ����Ʈ ���� ���̰� 1���� ������
	if (strlen(pStart) < 1)
	{
		str = "";
		return true;
	}

	// �� ����Ʈ ����(NULL ���ڸ�)
	char* pEnd = pStart + strlen(pStart) - 1;

	// right trim ó��
	if (bTrim)
	{
		while (pEnd >= pStart)
		{
			if (CLCString::IsSpace(*pEnd))
				pEnd--;
			else
				break;
		}
	}

	// trim�� ���� ���̰� 0�� �Ǹ� ����
	if (pEnd < pStart)
	{
		str = "";
		return true;
	}

	// ���⿡ ���� pStart���� pEnd���� ���ڸ� �����ϸ� ��
	int len = pEnd - pStart + 2;	// ��� ���ڿ� ���� (NULL ����)

	// ��� ������ ũ��� �� ���� ���� ����
	if (len > str.BufferSize())
		len = str.BufferSize();

	char* tmp = new char[len];
	memcpy(tmp, pStart, len - 1);
	tmp[len - 1] = '\0';
	str = tmp;
	delete [] tmp;

	return true;
}

bool CDBCmd::GetRec(int fieldno, float& f)
{
	char* ret = GetRec(fieldno);
	if (ret == NULL)
		return false;

	f = (float)atof(ret);
	return true;
}

bool CDBCmd::GetRec(int fieldno, LONGLONG& ll)
{
	char* ret = GetRec(fieldno);
	if (ret == NULL)
		return false;

	ll = ATOLL(ret);
	return true;
}

bool CDBCmd::GetRec( int fieldno, std::string& str )
{
	str = GetRec(fieldno);
	return true;
}

int CDBCmd::FindField( const char* fieldname )
{
	map_t::iterator it = map_.find(fieldname);
	if (it != map_.end())
	{
		return it->second;
	}

	std::string title = boost::str(boost::format("query[%s] / not found mysql field names ('%1%') : ") % m_sql % fieldname);
	std::string str = title;
	map_t::iterator eit = map_.begin();
	map_t::iterator eendit = map_.end();
	for (; eit != eendit; ++eit)
	{
		str += eit->first + " | ";
	}

	LOG_ERROR("\n\n\n\n\n");
	LOG_ERROR(str.c_str());
	LOG_ERROR("\n\n\n\n\n");

	PrintExcuteInfo::Instance()->SetStopMessage(title);

	bnf::instance()->Stop();

	return -1;
}
