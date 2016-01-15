#ifndef __OPTION_H__
#define __OPTION_H__

class CPC;
class APet;
class CItem;

class COptionProto
{
public:

	// DB ����
	int		m_idNum;						// DB Index
	int		m_type;							// �ɼ� ��ȣ

	std::vector<int> m_levelValue;
	std::vector<int> m_probValue;

	int		m_weaponType;					// � ���⿡ �ٴ°�
	int		m_wearType;						// � ���� �ٴ°�
	int		m_accessoryType;				// � �Ǽ��縮�� �ٴ°�
	COptionProto();
};

class COptionProtoList
{
public:
	typedef std::map<int, COptionProto*> map_t;

	map_t			map_;
	COptionProto*	m_proto;				// Option Proto
	int				m_nCount;				// Count

	COptionProtoList();
	~COptionProtoList();

	bool Load();							// DB Load
	COptionProto* FindProto(int type);		// Find Option Proto use Type
};

class COption
{
public:
	COptionProto*	m_proto;		// Option Proto

	int				m_type;			// Option Type
	int				m_level;		// Option Level
	int				m_value;		// Option Value

	short			m_dbValue;		// Option DB Value

	COption();

	void MakeOptionValue(COptionProto* proto, int factLevel, int num);	// Make Option Type, Level use Proto
	void SetOptionValue(short dbValue);			// Set Option Value	use dbValue
	void ApplyOptionValue(CPC* pc, CItem* item);	// Apply Option Value

	void GetDBValue(short dbValue);				// DB value -> type : level
	void SetDBValue();							// type : level -> DB value
	static short SetDBValue(int type, int level);

	static void ApplyOptionValue(CPC* pc, int nType, int nValue, CItem* pItem
								 , int nPosition = -1
								);

	void ApplyOptionValue(CAPet* apet, CItem* item);
	static void ApplyOptionValue(CAPet* apet, int nType, int nValue, CItem* pItem);
	static void ApplyOptionValue(CCharacter* ch, int nType, int nValue , CItem* pItem );
};

#endif
//
