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
	doc.AddMember("IsMulticast", pItem->IsMulticast, allocator);

	doc.AddMember("eqsIp", StringRef(pItem->eqsIp.c_str()), allocator);
	doc.AddMember("eqsPort", pItem->eqsPort, allocator);

	doc.AddMember("mcIp", StringRef(pItem->mcIp.c_str()), allocator);
	doc.AddMember("mcPort", pItem->mcPort, allocator);
	doc.AddMember("mcLoacalIp", StringRef(pItem->mcLoacalIp.c_str()), allocator);
	doc.AddMember("mcLocalPort", pItem->mcLocalPort, allocator);
	doc.AddMember("exchangeId", StringRef(pItem->exchangeId.c_str()), allocator);

	doc.EndObject(0);

	ofstream ofs(filename);
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	doc.Accept(writer);
}



void CServerConfig::WriteMdDefault(const char* filename)
{
	ServerItem item;

	item.IsMulticast = false;
	item.eqsIp = "183.129.220.106";
	item.eqsPort = 21006;
	item.mcIp = "183.129.220.106";
	item.mcPort = 0;
	item.mcLoacalIp = "127.0.0.1";
	item.mcLocalPort = 0;
	item.exchangeId = "DCE";

	WriteMd(filename, &item);
}

ServerItem CServerConfig::ReadMd(const char* filename)
{
	ifstream ifs(filename);
	IStreamWrapper isw(ifs);

	Document doc;
	doc.ParseStream(isw);

	ServerItem item;
	const Value& object = doc;
	item.eqsIp = object["eqsIp"].GetString();
	item.mcIp = object["mcIp"].GetString();
	item.mcLoacalIp = object["mcLoacalIp"].GetString();
	item.exchangeId = object["exchangeId"].GetString();

	item.eqsPort = object["eqsPort"].GetInt();
	item.mcPort = object["mcPort"].GetInt();
	item.mcLocalPort = object["mcLocalPort"].GetInt();

	item.IsMulticast = object["IsMulticast"].GetBool();


	return item;
}