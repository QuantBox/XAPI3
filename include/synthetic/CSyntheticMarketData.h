#pragma once

#include <list>
#include <map>
#include <mutex>
#include <set>

using namespace std;

#define LEG_DATA_BUF_LEN (512)
// 用于内部计算用的结构体
struct LegData
{
	// 权重，可能在外部已经指定好，也可以先求和，然后再算权重
	double weight;
	// 添加此变量是用于解决可能出现的数据溢出问题
	double sum;
	// 计数
	int count;

	double ask;
	double askSize;
	double bid;
	double bidSize;

	// 使用内存块来兼容不同类型,数据区的第一个字节非0表示数据已经准备好
	char* pBuf;
};

class CSyntheticMarketData;

class CSyntheticCalculate
{
	// 注意：pBuf和LegData->pBuf都是0号位是标记位
public:
	virtual void Begin(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut) = 0;
	virtual void Sum(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut) = 0;
	virtual void ForEach(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut) = 0;
	virtual void End(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut) = 0;
};

class CSyntheticCalculateFactory
{
public:
	virtual CSyntheticCalculate* Create(string method) = 0;
};

// 处理的是某一个合成指数
// 如IF000,南华商品指数等
// 由外层的key最后决定合约名
class CSyntheticMarketData
{
public:
	CSyntheticMarketData(const char* product, set<string> emits, CSyntheticCalculate* pCal)
	{
		m_product = product;
		m_emits = emits;
		m_emit = nullptr;
		memset(m_emit_buf, 0, sizeof(m_emit_buf));
		if (m_emits.size() == 1)
		{
			// 只保存第一个
			strncpy(m_emit_buf, emits.begin()->data(), sizeof(m_emit_buf));
			m_emit = m_emit_buf;
		}

		memset(&m_leg, 0, sizeof(m_leg));
		m_leg.pBuf = new char[LEG_DATA_BUF_LEN]();
		memset(m_leg.pBuf, 0, LEG_DATA_BUF_LEN);
		m_pCal = pCal;

		m_size = 0;
		m_count = 0;
		m_last_count = -1;
	}
	~CSyntheticMarketData()
	{
		if (m_leg.pBuf)
		{
			delete[] m_leg.pBuf;
			m_leg.pBuf = nullptr;
		}
	}

	void Create(const char* instrument, double weight, char* pData)
	{
		auto it = m_map.find(instrument);
		if (it == m_map.end())
		{
			LegData lmd = { 0 };
			lmd.weight = weight;
			lmd.pBuf = pData;
			m_map.insert(pair<string, LegData>(instrument, lmd));

			m_set.insert(instrument);
		}
		else
		{
		}
		// 由于这个代码是在Register中调用，合成合约的成份大小是固定的
		m_size = m_map.size();
	}
	void UpdateWeight()
	{
		double sum = 0;
		for (auto iter = m_map.begin(); iter != m_map.end(); iter++)
		{
			sum += iter->second.weight;
		}
		for (auto iter = m_map.begin(); iter != m_map.end(); iter++)
		{
			iter->second.weight = iter->second.weight / sum;
		}
	}
	// 注意，用于计算的内存块，第0号位是标记位
	void* Calculate(char* pBuf)
	{
		if (m_pCal == nullptr)
		{
			return nullptr;
		}
		lock_guard<mutex> cl(m_cs);

		m_pCal->Begin(this, pBuf, &m_leg);

		for (auto iter = m_map.begin(); iter != m_map.end(); iter++)
		{
			// 求和
			m_pCal->Sum(this, &iter->second, &m_leg);
		}

		for (auto iter = m_map.begin(); iter != m_map.end(); iter++)
		{
			// 算权重,或权重已经算好
			m_pCal->ForEach(this, &iter->second, &m_leg);
		}

		m_pCal->End(this, pBuf, &m_leg);

		return m_leg.pBuf;
	}

	void* GetValue() { return m_leg.pBuf; }
	set<string> GetEmits() { return m_emits; }
	string GetProduct() { return m_product; }
	set<string> GetInstruments() { return m_set; }
	// 判断指定位置字符是否为0，来确认数据是否已经有了
	bool CheckEmit(const char* instrument, int offest)
	{
		// 想统计有多少数据齐了时，发现内存区其实初始化时就已经齐了
	// 只好强行加一个标志位
		if (m_size != m_count)
		{
			char ch = 0;
			for (auto iter = m_map.begin(); iter != m_map.end(); iter++)
			{
				auto c = iter->second.pBuf[offest];
				ch += c;
			}
			m_count = ch;
		}
		if (m_last_count == m_count)
		{
			if (m_emit)
			{
				// 觉得strcmp要比set中的find要快，所以。。。
				return strcmp(m_emit, instrument) == 0;
			}

			// 没有指定触发合约，都触发
			if (m_emits.size() == 0)
			{
				return true;
			}

			// 相同判断是否指定合约
			auto it = m_emits.find(instrument);
			return it != m_emits.end();
		}
		else
		{
			m_last_count = m_count;
			// 不同表示正在起步几个数据包
			return m_size == m_count;
		}
	}
	
private:
	string							m_product;
	set<string>						m_emits;
	const char*						m_emit;
	char							m_emit_buf[128];
	map<string, LegData>	m_map;
	LegData							m_leg;
	mutex							m_cs;
	CSyntheticCalculate*			m_pCal;
	set<string>						m_set;
	int								m_size;
	int								m_count;
	int								m_last_count;
};

