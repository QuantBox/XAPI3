#pragma once

#include <list>
#include <unordered_map>

using namespace std;

// 合成合约配置信息
// 由于可以从配置文件中直接解析成需要的格式
// 这里的反向映射最后也没啥实际用处，废弃了
class CSyntheticInfos
{
public:
	CSyntheticInfos();
	~CSyntheticInfos();

	void Add(const char* product, const char* instrument, double weight = 0);
	unordered_map<string, double> GetByProduct(const char* product);
	unordered_map<string, double> GetByInstrument(const char* instrument);

	list<string> GetProducts();
private:
	void Add_Product(const char* product, const char* instrument, double weight);
	void Add_Instrument(const char* instrument, const char* product, double weight);

private:
	unordered_map<string, unordered_map<string,double>> m_map_by_product;
	unordered_map<string, unordered_map<string, double>> m_map_by_instrument;

	list<string> m_list_product;
};

