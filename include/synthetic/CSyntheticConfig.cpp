#include "stdafx.h"
#include "CSyntheticConfig.h"


CSyntheticConfig::CSyntheticConfig()
{
}


CSyntheticConfig::~CSyntheticConfig()
{
}


map<string, ConfigItem> CSyntheticConfig::Read(const char* filename)
{
	ifstream ifs(filename);
	IStreamWrapper isw(ifs);

	Document doc;
	doc.ParseStream(isw);

	map<string, ConfigItem> ls;
	if (!doc.IsArray())
	{
		return ls;
	}

	for (int i = 0; i < doc.Size(); i++)
	{
		ConfigItem item;
		const Value& object = doc[i];
		item.symbol = object["Symbol"].GetString();
		item.method = object["Method"].GetString();
		//item.emit = object["Emit"].GetString();

		const Value& emits_array = object["Emits"];
		for (int i=0;i<emits_array.Size();++i)
		{
			item.emits.insert(emits_array[i].GetString());
		}

		const Value& legs_object = object["Legs"];
		for (auto itr = legs_object.MemberBegin();
			itr != legs_object.MemberEnd(); ++itr)
		{
			item.legs.insert(pair<string, double>(itr->name.GetString(), itr->value.GetDouble()));
		}
		ls.insert(pair<string, ConfigItem>(item.symbol, item));
	}
	m_map = ls;
	return ls;
}

void CSyntheticConfig::Write(const char* filename)
{
	Document doc;
	doc.SetArray();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	rapidjson::Value info_array(rapidjson::kArrayType);

	{
		rapidjson::Value info_object(rapidjson::kObjectType);
		info_object.SetObject();
		info_object.AddMember("Symbol", "IF000", allocator);
		info_object.AddMember("Method", "OpenInterest", allocator);

		rapidjson::Value emits_array(rapidjson::kArrayType);
		emits_array.SetArray();
		emits_array.PushBack("IF1802", allocator);
		emits_array.PushBack("IF1806", allocator);
		info_object.AddMember("Emits", emits_array, allocator);

		rapidjson::Value legs_object(rapidjson::kObjectType);
		legs_object.SetObject();
		legs_object.AddMember("IF1802", 1.0, allocator);
		legs_object.AddMember("IF1803", 1.0, allocator);
		legs_object.AddMember("IF1806", 1.0, allocator);
		legs_object.AddMember("IF1809", 1.0, allocator);

		info_object.AddMember("Legs", legs_object, allocator);
		doc.PushBack(info_object, allocator);
	}

	{
		rapidjson::Value info_object(rapidjson::kObjectType);
		info_object.SetObject();
		info_object.AddMember("Symbol", "IF_WI", allocator);
		info_object.AddMember("Method", "Weight", allocator);

		rapidjson::Value emits_array(rapidjson::kArrayType);
		emits_array.SetArray();
		emits_array.PushBack("IF1802", allocator);
		info_object.AddMember("Emits", emits_array, allocator);

		rapidjson::Value legs_object(rapidjson::kObjectType);
		legs_object.SetObject();
		legs_object.AddMember("IF1802", 0.25, allocator);
		legs_object.AddMember("IF1803", 0.25, allocator);
		legs_object.AddMember("IF1806", 0.25, allocator);
		legs_object.AddMember("IF1809", 0.25, allocator);

		info_object.AddMember("Legs", legs_object, allocator);
		doc.PushBack(info_object, allocator);
	}

	{
		rapidjson::Value info_object(rapidjson::kObjectType);
		info_object.SetObject();
		info_object.AddMember("Symbol", "IF_IH_1803", allocator);
		info_object.AddMember("Method", "Spread", allocator);

		rapidjson::Value emits_array(rapidjson::kArrayType);
		emits_array.SetArray();
		emits_array.PushBack("IF1803", allocator);
		info_object.AddMember("Emits", emits_array, allocator);

		rapidjson::Value legs_object(rapidjson::kObjectType);
		legs_object.SetObject();
		legs_object.AddMember("IF1803", 1.0, allocator);
		legs_object.AddMember("IH1803", -1.2, allocator);

		info_object.AddMember("Legs", legs_object, allocator);
		doc.PushBack(info_object, allocator);
	}

	{
		rapidjson::Value info_object(rapidjson::kObjectType);
		info_object.SetObject();
		info_object.AddMember("Symbol", "IF888", allocator);
		info_object.AddMember("Method", "Copy", allocator);

		rapidjson::Value emits_array(rapidjson::kArrayType);
		emits_array.SetArray();
		emits_array.PushBack("IF1802", allocator);
		info_object.AddMember("Emits", emits_array, allocator);

		rapidjson::Value legs_object(rapidjson::kObjectType);
		legs_object.SetObject();
		legs_object.AddMember("IF1802", 1.0, allocator);

		info_object.AddMember("Legs", legs_object, allocator);
		doc.PushBack(info_object, allocator);
	}

	ofstream ofs(filename);
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	doc.Accept(writer);
}

map<string, ConfigItem> CSyntheticConfig::GetMap()
{
	return m_map;
}

bool CSyntheticConfig::ContainsKey(const char* product)
{
	auto it = m_map.find(product);
	if (it == m_map.end())
	{
		return false;
	}
	return true;
}

void CSyntheticConfig::Classify(set<string> two, set<string>& synthetic_set, set<string>& common_set)
{
	synthetic_set.clear();
	common_set.clear();

	for (auto it = two.begin(); it != two.end(); it++)
	{
		auto i = m_map.find(it->data());
		if (i == m_map.end())
		{
			// 没找到，普通合约
			common_set.insert(it->data());
		}
		else
		{
			// 找到了，合成合约
			synthetic_set.insert(it->data());
		}
	}
}