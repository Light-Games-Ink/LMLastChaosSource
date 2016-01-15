#ifndef __LCSTRING_H__
#define __LCSTRING_H__

// �����ڿ� ũ�⸦ �������� �ƴ��ϰ�, �迭�� ������ ��츦 ����� ���� ���Խ��� ã�ƺ���
// CLCString\:b+\:i\[

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#ifndef CIRCLE_WINDOWS
#define VSNPRINTF		vsnprintf
#else
#define VSNPRINTF		_vsnprintf
#endif // CIRCLE_WINDOWS

class CLCString
{
private:
	char*		m_strBuf;
	int			m_nSize;

	char* Search(char* target, char* find);

public:

	// constructor
	CLCString();
	CLCString(int size);
	CLCString(const CLCString& src);
	CLCString(const char* src);

	// destructor
	~CLCString();

	// format
	int Format(const char* fmt, ...);

	// length of string
	int Length() const;

	// size of buffer
	int BufferSize() const;

	// TRIM
	void Trim(bool bOnlySpace = true);

	// Copy From src
	// len : length for copy, include null
	void CopyFrom(const char* src, int len);

	void Replace(char* oldstr, char* newstr);

	// assign operator
	CLCString& operator=(const char* str);
	CLCString& operator=(const CLCString& str);

	// array operator
	char operator[](int idx) const;

	// cast operator
	operator const char*() const;

	// append string
	CLCString& operator+=(const char* str);
	CLCString& operator+=(const CLCString& str);

	static bool IsSpace(char ch);

	char* getBuffer();

protected:

	// Initialize
	void Init(int size);
};

////////////
// functions
inline void CLCString::Init(int size)
{
	if (size <= 0)
		size = 1024;

	m_nSize = size;
	m_strBuf = new char[m_nSize];
	memset(m_strBuf, 0, sizeof(char) * m_nSize);
}

inline CLCString::CLCString(int size)
{
	Init(size);
}

inline CLCString::CLCString(const CLCString& src)
{
	Init(src.m_nSize);
	strcpy(m_strBuf, src.m_strBuf);
}

inline CLCString::CLCString(const char* src)
{
	if (src == NULL)
	{
		Init(1);
		return ;
	}
	Init(strlen(src) + 1);
	strcpy(m_strBuf, src);
}

inline CLCString::CLCString()
{
	Init(50 + 1);
}

inline CLCString::~CLCString()
{
	if (m_strBuf)
		delete [] m_strBuf;
	m_strBuf = NULL;
	m_nSize = 0;
}

inline CLCString& CLCString::operator=(const char* str)
{
	if (str == NULL)
	{
		m_strBuf[0] = '\0';
		return *this;
	}
	int nSize = strlen(str) + 1;
	if (nSize > m_nSize)
		nSize = m_nSize;
	memcpy(m_strBuf, str, nSize - 1);
	m_strBuf[nSize - 1] = '\0';
	return *this;
}

inline CLCString& CLCString::operator=(const CLCString& str)
{
	*this = str.m_strBuf;
	return *this;
}

inline CLCString::operator const char*() const
{
	return (const char*)m_strBuf;
}

inline int CLCString::Format(const char* fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	int ret;
	ret = VSNPRINTF(m_strBuf, m_nSize - 1, fmt, vl);
	m_strBuf[m_nSize - 1] = '\0';
	va_end(vl);
	return ret;
}

inline int CLCString::Length() const
{
	if(m_strBuf == NULL) return 0;

	return strlen(m_strBuf);
}

inline int CLCString::BufferSize() const
{
	return m_nSize;
}

inline void CLCString::Trim(bool bOnlySpace)
{
	int len = Length();
	int index = 0, i;

	// ������ �ƴ� ó�� ���� ����
	while ((bOnlySpace && m_strBuf[index] == ' ') || (!bOnlySpace && CLCString::IsSpace(m_strBuf[index])))
		index++;

	// ���鸸ŭ ����
	if (index > 0)
	{
		int count = len - index;
		for (i = 0; i <= count; i++)
			m_strBuf[i] = m_strBuf[i + index];

		m_strBuf[i] = '\0';
	}

	// ���� ���� ����
	for (i = len - 1; i > 0; i--)
	{
		if ((bOnlySpace && m_strBuf[i] != ' ') || (!bOnlySpace && !CLCString::IsSpace(m_strBuf[i])))
			break;
	}

	m_strBuf[i + 1] = '\0';
}

inline char CLCString::operator[](int idx) const
{
	if (idx < 0 || idx >= Length())
		return 0;
	else
		return m_strBuf[idx];
}

inline void CLCString::CopyFrom(const char* src, int len)
{
	if (src == NULL || len < 1)
	{
		m_strBuf[0] = '\0';
		return ;
	}

	if (len < 2)
	{
		m_strBuf[0] = src[0];
		m_strBuf[1] = '\0';
	}
	else
	{
		if (len > m_nSize)
			len = m_nSize;
		memcpy(m_strBuf, src, len - 1);
		m_strBuf[len - 1] = '\0';
	}
}

inline void CLCString::Replace(char* oldstr, char* newstr)
{
	char *p;
	char tmp[1024];

	p = Search(m_strBuf, oldstr);
	while(p != NULL)
	{
		*p = '\0';
		strcpy(tmp, p + strlen(oldstr));
		strcat(m_strBuf, newstr);
		strcat(m_strBuf, tmp);

		p = Search(p+ strlen(newstr), oldstr);
	}
}

inline char* CLCString::Search(char* target, char* find)
{
	int s1, s2;
	char* p;

	s1 = strlen(target);
	s2 = strlen(find);

	for(p=target; p<=target+s1-s2; p++)
	{
		if(strncmp(p, find, s2) == 0)
			return p;
	}
	return NULL;
}

inline CLCString& CLCString::operator+=(const char* str)
{
	if (str == NULL)
		return *this;

	int curLength = Length();
	int strLength = strlen(str);
	if (curLength + strLength + 1 > m_nSize)
		strLength = m_nSize - curLength - 1;
	// ���� ���ڿ� curLength ���Ŀ� str���� strLength ��ŭ �����ϰ� NULL�� ��´�
	if (strLength > 0)
	{
		memcpy(m_strBuf + curLength, str, strLength);
		*(m_strBuf + curLength + strLength) = '\0';
	}
	return *this;
}

inline CLCString& CLCString::operator+=(const CLCString& str)
{
	*this += str.m_strBuf;
	return *this;
}

inline bool CLCString::IsSpace(char ch)
{
	if (0x09 <= ch && ch <= 0x0d)
		return true;
	return (ch == 0x20);
}

inline char* CLCString::getBuffer()
{
	return m_strBuf;
}


#endif // __LCSTRING_H__
//
