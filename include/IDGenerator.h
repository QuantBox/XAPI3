#pragma once

#include <atomic>

#include "ApiStruct.h"

using namespace std;

class CIDGenerator
{
public:
	CIDGenerator();
	~CIDGenerator();

	// 不能太长了，需要自动截断，目前计划不能超过32
	void SetPrefix(const char* prefix);

	const char* GetIDString();
	unsigned int GetID();
	// 保证当天不重复
	unsigned int GetTimeID();

	const unsigned int sec_per_year = 86400 * 365;
private:
	atomic<unsigned int> m_id;
	OrderIDType m_IDString;
	OrderIDType m_Prefix;

};

