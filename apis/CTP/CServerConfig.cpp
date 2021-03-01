#include "stdafx.h"
#include "CServerConfig.h"

#include <fstream>

#include "../../include/rapidjson/rapidjson.h"
#include "../../include/rapidjson/document.h"
#include "../../include/rapidjson/prettywriter.h"
#include "../../include/rapidjson/istreamwrapper.h"
#include "../../include/rapidjson/ostreamwrapper.h"

using namespace std;
using namespace rapidjson;

void CServerConfig::WriteMd(const char* filename, ServerItem* pItem)
{
	Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.StartObject();
	doc.AddMember("_IsUsingUdp", "是否启用UDP", allocator);
	doc.AddMember("IsUsingUdp", pItem->IsUsingUdp, allocator);
	doc.AddMember("_IsMulticast", "是否启用多播", allocator);
	doc.AddMember("IsMulticast", pItem->IsMulticast, allocator);
	doc.AddMember("_BrokerID", "经纪商ID", allocator);
	doc.AddMember("BrokerID", StringRef(pItem->BrokerID.c_str()), allocator);

	rapidjson::Value address_array(rapidjson::kArrayType);
	address_array.SetArray();
	for (auto iter = pItem->Address.begin(); iter != pItem->Address.end(); ++iter)
	{
		address_array.PushBack(StringRef(iter->c_str()), allocator);
	}
	doc.AddMember("_Address", "服务器地址，可以添加多个，随机的选择一个进行连接", allocator);
	doc.AddMember("Address", address_array, allocator);

	doc.EndObject(0);

	ofstream ofs(filename);
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	doc.Accept(writer);
}

void CServerConfig::WriteTd(const char* filename, ServerItem* pItem)
{
	Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.StartObject();
	doc.AddMember("_BrokerID", "经纪商ID", allocator);
	doc.AddMember("BrokerID", StringRef(pItem->BrokerID.c_str()), allocator);
	doc.AddMember("_UserProductInfo", "客户端产品ID", allocator);
	doc.AddMember("UserProductInfo", StringRef(pItem->UserProductInfo.c_str()), allocator);
	doc.AddMember("_AppID", "客户端应用ID", allocator);
	doc.AddMember("AppID", StringRef(pItem->AppID.c_str()), allocator);
	doc.AddMember("_AuthCode", "客户端授权码", allocator);
	doc.AddMember("AuthCode", StringRef(pItem->AuthCode.c_str()), allocator);

	rapidjson::Value address_array(rapidjson::kArrayType);
	address_array.SetArray();
	for (auto iter = pItem->Address.begin(); iter != pItem->Address.end(); ++iter)
	{
		address_array.PushBack(StringRef(iter->c_str()), allocator);
	}
	doc.AddMember("_Address", "服务器地址，可以添加多个，随机的选择一个进行连接", allocator);
	doc.AddMember("Address", address_array, allocator);


	rapidjson::Value resume_type_object(rapidjson::kObjectType);
	for (auto iter = pItem->ResumeType.begin(); iter != pItem->ResumeType.end(); ++iter)
	{
		resume_type_object.AddMember(StringRef(iter->first.c_str()), StringRef(iter->second.c_str()), allocator);
	}
	doc.AddMember("_ResumeType", "流订阅类型", allocator);
	doc.AddMember("ResumeType", resume_type_object, allocator);

	doc.EndObject(0);

	ofstream ofs(filename);
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	doc.Accept(writer);
}

void CServerConfig::WriteMdDefault(const char* filename)
{
	ServerItem item;

	item.IsUsingUdp = false;
	item.IsMulticast = false;
	item.BrokerID = "9999";

	item.Address.push_back("tcp://180.168.146.187:10110");

	WriteMd(filename, &item);
}

void CServerConfig::WriteTdDefault(const char* filename)
{
	ServerItem item;

	item.BrokerID = "9999";
	item.UserProductInfo = "OpenQuant";
	item.AppID = "8500342533";
	item.AuthCode = "0000000000000000";

	item.Address.push_back("tcp://180.168.146.187:10100");

	item.ResumeType.insert(pair<string, string>("PublicTopic", "Restart"));
	item.ResumeType.insert(pair<string, string>("PrivateTopic", "Restart"));

	WriteTd(filename, &item);
}

ServerItem CServerConfig::ReadMd(const char* filename)
{
	ifstream ifs(filename);
	IStreamWrapper isw(ifs);

	Document doc;
	doc.ParseStream(isw);

	ServerItem item;
	const Value& object = doc;
	item.IsUsingUdp = object["IsUsingUdp"].GetBool();
	item.IsMulticast = object["IsMulticast"].GetBool();
	item.BrokerID = object["BrokerID"].GetString();

	const Value& address_array = object["Address"];
	for (auto iter = address_array.Begin(); iter != address_array.End(); ++iter)
	{
		item.Address.push_back(iter->GetString());
	}

	return item;
}

ServerItem CServerConfig::ReadTd(const char* filename)
{
	ifstream ifs(filename);
	IStreamWrapper isw(ifs);

	Document doc;
	doc.ParseStream(isw);

	ServerItem item;
	const Value& object = doc;
	item.BrokerID = object["BrokerID"].GetString();
	item.UserProductInfo = object["UserProductInfo"].GetString();
	item.AppID = object["AppID"].GetString();
	item.AuthCode = object["AuthCode"].GetString();

	const Value& address_array = object["Address"];
	for (auto iter = address_array.Begin(); iter != address_array.End(); ++iter)
	{
		item.Address.push_back(iter->GetString());
	}
	const Value& resume_type_object = object["ResumeType"];
	for (auto iter = resume_type_object.MemberBegin(); iter != resume_type_object.MemberEnd(); ++iter)
	{
		item.ResumeType.insert(pair<string, string>(iter->name.GetString(), iter->value.GetString()));
	}

	return item;
}