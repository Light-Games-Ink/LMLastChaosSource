#include <math.h>
#include "stdhdrs.h"

#include "Log.h"
#include "Server.h"
#include "../ShareLib/DBCmd.h"

static float	a_sin[360];
static float	a_cos[360];
struct timeval	a_NullTime;

/////////////////////////////////////////
// ��ġ �����Ϳ��� ���ڿ��� ��ȯ�ϴ� �Լ�

////////////////////
// Function name	: __Itoa
// Description	    : 32��Ʈ ������ ���ڿ���
// Return type		: char*
//					: ��ȯ �Ϸ�� ���ڿ� ������
// Argument         : int value
//					: ��ȯ �ϰ��� �ϴ� ����
// Argument         : char *string
//					: ��ȯ�� �����͸� ������ ���ڿ� ������
char* __ltoa(int value, char *string)
{
	char p_buf[128];
	int idx, i;

	// �ʱ�ȭ
	*string = '\0';
	idx = 0;
	i = 0;

	// ������ -��ȣ�� ���̰� value�� ����� ������
	if (value < 0)
	{
		strcpy(string, "-");
		value = -value;
		i = 1;
	}
	if (value == 0)
	{
		strcpy(string, "0");
		return string;
	}

	// ���ڸ����� ���ڿ��� ����
	while (value)
	{
		switch (value % 10)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			p_buf[idx++] = (char)((value % 10) + '0');
			break;
		default:
			p_buf[idx++] = '0';
			break;
		}

		value /= 10;
	}

	while (idx)
	{
		string[i++] = p_buf[--idx];
	}
	string[i++] = '\0';

	return string;
}

////////////////////
// Function name	: lltoa
// Description	    : 64��Ʈ ������ ���ڿ��� ��ȯ
// Return type		: char*
//					: ��ȯ�� ���ڿ�
// Argument         : LONGLONG value
//					: 64��Ʈ ����
// Argument         : char *string
//					: ��ȯ�� ����ϴ� ����
char* __ltoa(LONGLONG value, char *string)
{
	char p_buf[128];
	int idx, i;

	// �ʱ�ȭ
	*string = '\0';
	idx = 0;
	i = 0;

	// ������ -��ȣ�� ���̰� value�� ����� ������
	if (value < 0)
	{
		strcpy(string, "-");
		value = -value;
		i = 1;
	}
	if (value == 0)
	{
		strcpy(string, "0");
		return string;
	}

	// ���ڸ����� ���ڿ��� ����
	while (value)
	{
		switch (value % 10)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			p_buf[idx++] = (char)((value % 10) + '0');
			break;
		default:
			p_buf[idx++] = '0';
			break;
		}

		value /= 10;
	}

	while (idx)
	{
		string[i++] = p_buf[--idx];
	}
	string[i++] = '\0';

	return string;
}

////////////////////
// Function name	: __ftoa
// Description	    : float �����͸� ���ڿ��� ��ȯ
// Return type		: char*
//					: ��ȯ�� ���ڿ�
// Argument         : float f
//					: �Է� �Ǽ�
// Argument         : char *buf
//					: ��� ������
// Argument         : int place
//					: �ڸ���
char* __ftoa(float f, char *buf, int place)
{
	char p_buf[128];

	*buf = '\0';

	// �Ҽ��� ���ڸ� ���ڿ���
	// ��� ���� ������
	if (f < 0)
	{
		// ������ ����� �����
		f = -f;
		__ltoa((int)f, p_buf);
		// ���ڿ� �տ� -�� ���̰�
		strcpy(buf, "-");
		strcat(buf, p_buf);
	}
	else
	{
		__ltoa((int)f, buf);
	}

	// �Ҽ��� ���ڸ� ����?
	if (place <= 0)
	{
		return buf;
	}

	// �Ҽ��� 8�ڸ�������
	if (place > 8)
		place = 8;

	// �Ҽ��� �ںκи� �����
	f -= (int)f;

	// ���ϴ� �Ҽ� �ڸ����� ���ؼ�
	while (place > 0)
	{
		f *= 10;
		place--;
	}

	// ���� �κи� �ٽ� ��ȯ
	__ltoa((int)f, p_buf);
	strcat(buf, ".");
	strcat(buf, p_buf);

	return buf;
}

/////////////////////////////////////////////
// ���� ����

////////////////////
// Function name	: MakeMath
// Description	    : �ڻ���, ���� ���� 1�� ������ �̸� ����� ��
// Return type		: void
//					:
void MakeMath()
{
	for (int i = 0; i < 360; i++)
	{
		a_sin[i] = sinf((float) i / 360.0f * PI_2);
		a_cos[i] = cosf((float) i / 360.0f * PI_2);
	}
}

////////////////////
// Function name	: get_sin
// Description	    : �̸� ���ص� ���� ���� ����
// Return type		: float
//					: ���ΰ�
// Argument         : float radian
//					: ����, ������ ��, ������ -180 ~ +180
float get_sin(float radian)
{
	int i = (int)(radian / PI_2 * 360);

	if (i < 0)
	{
		i %= 360;
		i += 360;
	}

	if (i >= 360)
	{
		i %= 360;
	}
	/*while (i >= 360) {
		i -= 360;
	}
	while (i < 0) {
		i += 360;
	}*/

	return a_sin[i];
}

////////////////////
// Function name	: get_cos
// Description	    : �̸� ���ص� �ڻ��� ���� ����
// Return type		: float
//					: ���ΰ�
// Argument         : float radian
//					: ����, ������ ��, ������ -180 ~ +180
float get_cos(float radian)
{
	int i = (int)(radian / PI_2 * 360);

	if (i < 0)
	{
		i %= 360;
		i += 360;
	}

	if (i >= 360)
	{
		i %= 360;
	}
	/*while (i >= 360) {
		i -= 360;
	}
	while (i < 0) {
		i += 360;
	}*/

	return a_cos[i];
}

///////////////////////////////////////////
// �ð� ����

////////////////////
// Function name	: GetTimeofDay
// Description	    : ���� �ð��� ����
// Return type		: void
//					:
// Argument         : struct timeval *t
//					: ���� �ð��� ����
// Argument         : struct timezone *dummy
//					: ������� ����
void GetTimeofDay(struct timeval *t, struct timezone* dummy)
{
#ifdef CIRCLE_WINDOWS
	unsigned long millisec = GetTickCount();
	t->tv_sec = (int) (millisec / 1000);
	t->tv_usec = (millisec % 1000) * 1000;
#else
	gettimeofday (t, dummy);
#endif
}

////////////////////
// Function name	: TimeDiff
// Description	    : �ð� ���̸� ���Ѵ�, a - b = rslt
// Return type		: void
//					:
// Argument         : struct timeval *rslt
//					: ����� ������ ����
// Argument         : struct timeval *a
//					: �ð� ��, ���� ��
// Argument         : struct timeval *b
//					: �ð� ��, ���� ��
void TimeDiff(struct timeval *rslt, struct timeval *a, struct timeval *b)
{
	// a�� ������ 0
	if (a->tv_sec < b->tv_sec)
		*rslt = a_NullTime;

	// a�� b�� �ʴ��� ��
	else if (a->tv_sec == b->tv_sec)
	{
		// �ʴ��� ������
		// a�� ������ 0
		if (a->tv_usec < b->tv_usec)
			*rslt = a_NullTime;
		else
		{
			// ���̸� ����
			rslt->tv_sec = 0;
			rslt->tv_usec = a->tv_usec - b->tv_usec;
		}
	}

	else
	{
		// �ʴ��� �ٸ���
		// �ʴ��� ���� ����
		rslt->tv_sec = a->tv_sec - b->tv_sec;
		// a�� ����ũ���� ������ �۴ٸ�
		if (a->tv_usec < b->tv_usec)
		{
			// ����ũ���� �������� �����ϰ�
			rslt->tv_usec = a->tv_usec + 1000000 - b->tv_usec;
			// �ʴ����� �ڸ��������� ����
			rslt->tv_sec--;
		}
		else
			rslt->tv_usec = a->tv_usec - b->tv_usec;
	}
}

/////////////////////////////////////////
// �Ÿ� ��� �Լ�

////////////////////
// Function name	: GetDistance
// Description	    : �Ÿ� ���
// Return type		: float
//					: �Ÿ� ��ȯ
// Argument         : float x1
//					: ���� x ��ǥ
// Argument         : float x2
//					: �� x ��ǥ
// Argument         : float z1
//					: ���� z ��ǥ
// Argument         : float z2
//					: �� z ��ǥ
// Argument         : float h1
//					: ���� ����
// Argument         : float h2
//					: �� ����
float GetDistance(float x1, float x2, float z1, float z2, float h1, float h2)
{
	float dx = ABS(x1 - x2);
	float dz = ABS(z1 - z2);
	float dh = ABS(h1 - h2);

	float ret = (float)sqrt(dx*dx + dz*dz + dh*dh);
	return (ret > 0.0f) ? ret : 0.0f;
}

/////////////////////////////////
// ���ڿ� ����

////////////////////
// Function name	: SkipSpaces
// Description	    : �պκ� ���� �ѱ�, ������ ���� ����
// Return type		: char*
//					: ó�� ������ ����� ������ ��ġ�� ��ȯ
// Argument         : char* string
//					: ������ �ѱ���� �ϴ� ���ڿ�
const char* SkipSpaces(const char* string)
{
	// �����̸� �������� �ѱ��
	for (; string && *string && is_space_in_util(*string); (string)++)
		;
	// ������ ��ȯ
	return string;
}

const char* SkipSpecialCharacter(const char* string)
{
	for(; string && *string && is_special_in_util(*string); (string)++)
		;
	return string;
}

////////////////////
// Function name	: TrimString
// Description	    : �յ� ������ ����, ������ ������
// Return type		: char*
//					: �Է� ���ڿ�
// Argument         : char* string
//					: ������ �����ϰ��� �ϴ� ���ڿ�
char* TrimString(char* string)
{
	if(string == NULL) return NULL;

	int len = strlen(string);
	int index = 0, i;

	// ������ �ƴ� ó�� ���� ����
	while (string[index] == ' ')
		index ++;

	// ���鸸ŭ ����
	if (index > 0)
	{
		int count = len - index;
		for (i = 0; i <= count; i++)
			string[i] = string[i+index];

		string[i] = '\0';
	}

	// ���� ���� ����
	for (i = strlen(string) - 1; i > 0; i--)
	{
		if (string[i] != ' ')
			break;
	}

	string[i + 1] = '\0';

	return string;
}

////////////////////
// Function name	: AnyOneArg
// Description	    : ���� ������ ���ڿ� ��ūȭ
// Return type		: char*
//					: ���� ��ū ��ġ
// Argument         : char* argument
//					: ���� ���ڿ�
// Argument         : char* first_arg
//					: ��ūȭ�� ���ڿ��� ���� ��
// Argument         : bool toLower
//					: �ҹ��ڷ� ���� ��ȯ���� ����
const char* AnyOneArg(const char* argument, char* first_arg, bool toLower)
{
	argument = SkipSpaces(argument);

	while (argument && *argument && !is_space_in_util(*argument))
	{
		if (toLower)
		{
			*(first_arg++) = LOWER(*argument);
		}
		else
		{
			*(first_arg++) = *argument;
		}
		argument++;
	}

	*first_arg = '\0';

	return (argument);
}
////////////////////
// Function name	: GetToken
// Description	    : Ư������ ������ ���ڿ� ��ūȭ(���ڳ� �����ڸ� ����, �׿� ���ڴ� Ư�����ڷ� �з���.)
// Return type		: char*
//					: ���� ��ū ��ġ
// Argument         : char* arg1
//					: ���� ���ڿ�
// Argument         : char* arg2
//					: ��ūȭ�� ���ڿ��� ���� ��
const char* GetToken(const char* arg1, char* arg2, bool toLower)
{
	arg1 = SkipSpecialCharacter(arg1);

	while(arg1 && *arg1 && !is_special_in_util(*arg1))
	{
		if(toLower)
		{
			*(arg2++) = LOWER(*arg1);
		}
		else
		{
			*(arg2++) = *arg1;
		}
		arg1++;
	}

	*arg2 = '\0';

	return (arg1);
}

////////////////////
// Function name	: IntCat
// Description	    : ���ڿ��� ���� ���̱�
// Return type		: char*
//					: �ٿ��� ���ڿ�
// Argument         : char *dest
//					: ��� ���ڿ�
// Argument         : int i
//					: �ٿ��� ����
char* IntCat(char *dest, int i, bool bInsertSpace)
{
	char i_buf[128];
	if (bInsertSpace)
		strcat(dest, " ");
	strcat(dest, __ltoa(i, i_buf));
	return dest;
}

////////////////////
// Function name	: IntCat
// Description	    : ���ڿ��� ���� ���̱�
// Return type		: char*
//					: �ٿ��� ���ڿ�
// Argument         : char *dest
//					: ��� ���ڿ�
// Argument         : LONGLONG ll
//					: �ٿ��� ����
char* IntCat(char *dest, LONGLONG ll, bool bInsertSpace)
{
	char i_buf[128];
	if (bInsertSpace)
		strcat(dest, " ");
	strcat(dest, __ltoa(ll, i_buf));
	return dest;
}

////////////////////
// Function name	: IntCat0
// Description	    : ������ ���ڿ��� ���̱�, ���� ����ŭ 0���� ä��
// Return type		: char*
//					: ��� ���ڿ�
// Argument         : char* dest
//					: ������ ����
// Argument         : int n
//					: ���� ����
// Argument         : int width
//					: ��
char* IntCat0(char* dest, int n, int width, bool bInsertSpace)
{
	if(dest == NULL) return NULL;

	char tbuf[128];
	int len = strlen(__ltoa(n, tbuf));
	int wasLen = strlen(dest);

	if (bInsertSpace)
		strcat(dest, " ");
	if (width > len)
	{
		width -= len;
		for (int i = wasLen; i < width + wasLen ; i++)
		{
			dest[i] = '0';
			dest[i + 1] = '\0';
		}
	}
	strcat(dest, tbuf);
	return dest;
}

////////////////////
// Function name	: IntCat0
// Description	    : ������ ���ڿ��� ���̱�, ���� ����ŭ 0���� ä��
// Return type		: char*
//					: ��� ���ڿ�
// Argument         : char* dest
//					: ������ ����
// Argument         : LONGLONG n
//					: ���� ����
// Argument         : int width
//					: ��
char* IntCat0(char* dest, LONGLONG n, int width, bool bInsertSpace)
{
	if(dest == NULL) return NULL;

	char tbuf[128];
	int len = strlen(__ltoa(n, tbuf));
	int wasLen = strlen(dest);

	if (bInsertSpace)
		strcat(dest, " ");
	if (width > len)
	{
		width -= len;
		for (int i = wasLen; i < width + wasLen ; i++)
		{
			dest[i] = '0';
			dest[i + 1] = '\0';
		}
	}
	strcat(dest, tbuf);
	return dest;
}

////////////////////
// Function name	: strinc
// Description	    : Ư�� ���ڿ��� �����ϴ��� �˻�
// Return type		: bool
//					: �����ϸ� true, �ƴϸ� return
// Argument         : const char *str
//					: ���� ���ڿ�
// Argument         : const char *inc
//					: �� ���ڿ�
bool strinc(const char *str, const char *inc)
{
	if(str == NULL || inc == NULL) return false;

	int str_len = strlen(str);
	int inc_len = strlen(inc);

	if (str_len < inc_len)
		return false;

	for (int i = 0; i <= str_len - inc_len; i++)
	{
		if (!strncmp(str+i, inc, inc_len))
			return true;
	}

	return false;
}

const char* DAnyOneArg(const char *argument, char *first_arg)
{
	while (*argument && (*argument != '.'))
	{
		*(first_arg++) = *argument;
		argument++;
	}

	*first_arg = '\0';
	argument++;
	return (argument);
}

void IPtoi(const char *org_IP,int *i_one, int *i_two, int *i_thr, int *i_for)
{
	const char *p_IP;
	char tempStr[10];

	p_IP = org_IP;
	p_IP = DAnyOneArg(p_IP,tempStr);
	*i_one = atoi(tempStr);
	p_IP = DAnyOneArg(p_IP,tempStr);
	*i_two = atoi(tempStr);
	p_IP = DAnyOneArg(p_IP,tempStr);
	*i_thr = atoi(tempStr);
	*i_for = atoi(p_IP);
}

// [selo: 101115] ���þƾ� �̿��� ���ڰ� �ִ��� Ȯ��
// ���þƾ� �̿��� ���ڰ� ������ true ��ȯ, ������ false ��ȯ
bool CheckNoRussianCharacter(const CLCString& str)
{
	int len = str.Length();
	for( int i = 0; i < len; i++ )
	{
		if( !(str[i] >= 48 && str[i] <= 57)
				&& !(str[i] >= -64 && str[i] <= -1)
				&& !(str[i] == -88)
				&& !(str[i] == -72)	)
		{
			return true;
		}
	}

	return false;
}
//