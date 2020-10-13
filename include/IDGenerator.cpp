#include "stdafx.h"
#include "IDGenerator.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

CIDGenerator::CIDGenerator()
{
	m_id = 0;
	memset(m_IDString, 0, sizeof(OrderIDType));
	memset(m_Prefix, 0, sizeof(OrderIDType));
}

CIDGenerator::~CIDGenerator()
{
}

void CIDGenerator::SetPrefix(const char* prefix)
{
	strncpy(m_Prefix, prefix, 32);
}

unsigned int CIDGenerator::GetID()
{
	return ++m_id;
}

unsigned int CIDGenerator::GetTimeID()
{
	time_t lt;
	lt = time(nullptr);
	struct tm *ptr;
	ptr = localtime(&lt);
	// 1970年开始移动到2015年开始
	// long 其实是无符号int,所以*10000会溢出
	// *1000 在当天下单数超过1000时才会影响前面的秒
	// 如果快速的重复登录，出现第一次已经下到1000笔，第二次登录时在1秒后又下，才会出现ID重复
	unsigned int x = ((ptr->tm_wday * 24 + ptr->tm_hour) * 450 + (ptr->tm_min * 60 + ptr->tm_sec)) * 100 + GetID();
	return x;
}

const char* CIDGenerator::GetIDString()
{
	snprintf(m_IDString, sizeof(m_IDString), "%s:%ld", m_Prefix, ++m_id);
	return m_IDString;
}
