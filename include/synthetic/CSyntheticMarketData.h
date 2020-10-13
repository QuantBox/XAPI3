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
	CSyntheticMarketData(const char* product, set<string> emit, CSyntheticCalculate* pCal);
	~CSyntheticMarketData();

	void Create(const char* instrument, double weight, char* pData);
	void UpdateWeight();
	// 注意，用于计算的内存块，第0号位是标记位
	void* Calculate(char* pBuf);
	void* GetValue();

	set<string> GetEmits() { return m_emits; }
	string GetProduct() { return m_product; }
	set<string> GetInstruments();
	// 判断指定位置字符是否为0，来确认数据是否已经有了
	bool CheckEmit(const char* instrument,int offest);
	
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

