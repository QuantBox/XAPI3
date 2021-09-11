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

void CServerConfig::Write(const char* filename, ServerItem* pItem)
{
	Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.StartObject();

	doc.AddMember("AuthCode", StringRef(pItem->AuthCode.c_str()), allocator);
	doc.AddMember("IP", StringRef(pItem->IP.c_str()), allocator);
	doc.AddMember("Port", pItem->Port, allocator);

	doc.EndObject(0);

	ofstream ofs(filename);
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	doc.Accept(writer);
}



void CServerConfig::WriteDefault(const char* filename)
{
	ServerItem item;

	item.AuthCode = "67EA896065459BECDFDB924B29CB7DF1946CED\
32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC5211AF9FEE\
541DDE41BCBAB68D525B0D111A0884D847D57163FF7F329FA574E7946CED32E26C1E\
AC946CED32E26C1EAC733827B0CE853869ABD9B8F170E14F8847D3EA0BF4E191F5D9\
7B3DFE4CCB1F01842DD2B3EA2F4B20CAD19B8347719B7E20EA1FA7A3D1BFEFF22290\
F4B5C43E6C520ED5A40EC1D50ACDF342F46A92CCF87AEE6D73542C42EC17818349C7\
DEDAB0E4DB16977714F873D505029E27B3D57EB92D5BEDA0A710197EB67F94BB1892\
B30F58A3F211D9C3B3839BE2D73FD08DD776B9188654853DDA57675EBB7D6FBBFC";
	item.IP = "123.161.206.213";
	item.Port = 6060;

	Write(filename, &item);
}

ServerItem CServerConfig::Read(const char* filename)
{
	ifstream ifs(filename);
	IStreamWrapper isw(ifs);

	Document doc;
	doc.ParseStream(isw);

	ServerItem item;
	const Value& object = doc;
	item.AuthCode = object["AuthCode"].GetString();

	item.IP = object["IP"].GetString();
	item.Port = object["Port"].GetInt();

	return item;
}