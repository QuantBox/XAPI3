#pragma once

#include <unordered_map>
#include <map>
#include <mutex>
#include <set>

using namespace std;

template<typename T>
class COrderMap
{
public:
	COrderMap()
	{
	}
	~COrderMap()
	{
	}

	void insert(T id, void* pOrderXAPI, void* pOrderAPI)
	{
		lock_guard<mutex> cl(m_cs);
		//m_id_platform_order.insert(pair<T, void*>(id, pOrderXAPI));
		//m_id_api_order.insert(pair<T, void*>(id, pOrderAPI));
		m_id_platform_order[id] = pOrderXAPI;
		m_id_api_order[id] = pOrderAPI;
	}

	void erase(T id)
	{
		lock_guard<mutex> cl(m_cs);

		{
			auto it = m_id_platform_order.find(id);
			if(it!= m_id_platform_order.end())
				m_id_platform_order.erase(it);
		}
		{
			auto it = m_id_api_order.find(id);
			if (it != m_id_api_order.end())
				m_id_api_order.erase(it);
		}
	}

	void replace(T erase_id, T insert_id, void* pOrderXAPI, void* pOrderAPI)
	{
		lock_guard<mutex> cl(m_cs);

		{
			auto it = m_id_platform_order.find(erase_id);
			if (it != m_id_platform_order.end())
				m_id_platform_order.erase(it);
		}
		{
			auto it = m_id_api_order.find(erase_id);
			if (it != m_id_api_order.end())
				m_id_api_order.erase(it);
		}

		m_id_platform_order[insert_id] = pOrderXAPI;
		m_id_api_order[insert_id] = pOrderAPI;
	}

	//// 建议结构体中提供UserData这类的指针，方便互相找到相关订单
	//void* findOrderAPI(T id)
	//{
	//	auto it = m_id_api_order.find(id);
	//	if (it == m_id_api_order.end())
	//	{
	//		return nullptr;
	//	}
	//	else
	//	{
	//		return it->second;
	//	}
	//}

	void* findOrderXAPI(T id)
	{
		lock_guard<mutex> cl(m_cs);
		auto it = m_id_platform_order.find(id);
		if (it == m_id_platform_order.end())
		{
			return nullptr;
		}
		else
		{
			return it->second;
		}
	}

	// =================
	void insertCancel(T id, void* pCancelAPI)
	{
		lock_guard<mutex> cl(m_cs);
		m_id_api_order_cancel.insert(pair<T, void*>(id, pCancelAPI));
	}

	void* findCancelAPI(T id)
	{
		lock_guard<mutex> cl(m_cs);
		auto it = m_id_api_order_cancel.find(id);
		if (it == m_id_api_order_cancel.end())
		{
			return nullptr;
		}
		else
		{
			return it->second;
		}
	}

	// =================
	void clear()
	{
		lock_guard<mutex> cl(m_cs);
		set<void*> _set;
		for (auto it = m_id_platform_order.begin(); it != m_id_platform_order.end(); ++it)
			_set.insert((void*)it->second);
		m_id_platform_order.clear();

		for (auto it = m_id_api_order.begin(); it != m_id_api_order.end(); ++it)
			_set.insert((void*)it->second);
		m_id_api_order.clear();

		for (auto it = m_id_api_order_cancel.begin(); it != m_id_api_order_cancel.end(); ++it)
			_set.insert((void*)it->second);
		m_id_api_order_cancel.clear();

		for (auto it = _set.begin(); it != _set.end(); ++it)
		{
			delete[] * it;
		}
		_set.clear();
	}

private:
	map<T, void*>			m_id_platform_order;
	map<T, void*>			m_id_api_order;
	map<T, void*>			m_id_api_order_cancel;

	mutex					m_cs;
};

