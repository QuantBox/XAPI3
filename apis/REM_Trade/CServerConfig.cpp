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

void CServerConfig::WriteTd(const char* filename, ServerItem* pItem)
{
	Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.StartObject();
	doc.AddMember("RemoteTradeIp", StringRef(pItem->RemoteTradeIp.c_str()), allocator);
	doc.AddMember("RemoteTradeTCPPort", pItem->RemoteTradeTCPPort, allocator);
	doc.AddMember("RemoteTradeUDPPort", pItem->RemoteTradeUDPPort, allocator);

	doc.AddMember("RemoteQueryIp", StringRef(pItem->RemoteQueryIp.c_str()), allocator);
	doc.AddMember("RemoteQueryTCPPort", pItem->RemoteQueryTCPPort, allocator);

	doc.AddMember("LocalTradeIp", StringRef(pItem->LocalTradeIp.c_str()), allocator);
	doc.AddMember("LocalTradeUDPPort", pItem->LocalTradeUDPPort, allocator);

	doc.EndObject(0);

	ofstream ofs(filename);
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	doc.Accept(writer);
}



void CServerConfig::WriteTdDefault(const char* filename)
{
	ServerItem item;

	item.RemoteTradeIp = "183.129.220.106";
	item.RemoteTradeTCPPort = 21000;
	item.RemoteTradeUDPPort = 0;

	item.RemoteQueryIp = "183.129.220.106";
	item.RemoteQueryTCPPort = 21001;

	item.LocalTradeIp = "183.129.220.106";
	item.LocalTradeUDPPort = 0;

	WriteTd(filename, &item);
}

ServerItem CServerConfig::ReadTd(const char* filename)
{
	ifstream ifs(filename);
	IStreamWrapper isw(ifs);

	Document doc;
	doc.ParseStream(isw);

	ServerItem item;
	const Value& object = doc;
	item.RemoteTradeIp = object["RemoteTradeIp"].GetString();
	item.RemoteQueryIp = object["RemoteQueryIp"].GetString();
	item.LocalTradeIp = object["LocalTradeIp"].GetString();

	item.RemoteTradeTCPPort = object["RemoteTradeTCPPort"].GetInt();
	item.RemoteTradeUDPPort = object["RemoteTradeUDPPort"].GetInt();
	item.RemoteQueryTCPPort = object["RemoteQueryTCPPort"].GetInt();
	item.LocalTradeUDPPort = object["LocalTradeUDPPort"].GetInt();


	return item;
}