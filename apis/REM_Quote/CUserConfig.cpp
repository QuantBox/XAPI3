#include "stdafx.h"
#include "CUserConfig.h"

#include <fstream>

#include "../../include/rapidjson/rapidjson.h"
#include "../../include/rapidjson/document.h"
#include "../../include/rapidjson/prettywriter.h"
#include "../../include/rapidjson/istreamwrapper.h"
#include "../../include/rapidjson/ostreamwrapper.h"

using namespace std;
using namespace rapidjson;

void CUserConfig::Write(const char* filename, UserItem* pItem)
{
	Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.StartObject();

	doc.AddMember("loginId", StringRef(pItem->loginId), allocator);
	doc.AddMember("password", StringRef(pItem->password), allocator);

	doc.EndObject(0);

	ofstream ofs(filename);
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	doc.Accept(writer);
}

void CUserConfig::WriteDefault(const char* filename)
{
	UserItem item;

	strcpy(item.loginId, "38662071");
	strcpy(item.password, "7");

	Write(filename, &item);
}

UserItem CUserConfig::Read(const char* filename)
{
	ifstream ifs(filename);
	IStreamWrapper isw(ifs);

	Document doc;
	doc.ParseStream(isw);

	UserItem item;
	const Value& object = doc;

	strcpy(item.loginId, object["loginId"].GetString());
	strcpy(item.password, object["password"].GetString());

	return item;
}
