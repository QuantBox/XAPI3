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

	doc.AddMember("UserNo", StringRef(pItem->UserNo), allocator);
	doc.AddMember("Password", StringRef(pItem->Password), allocator);

	doc.EndObject(0);

	ofstream ofs(filename);
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	doc.Accept(writer);
}

void CUserConfig::WriteDefault(const char* filename)
{
	UserItem item;

	strcpy(item.UserNo, "UserNo");
	strcpy(item.Password, "Password");

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

	strcpy(item.UserNo, object["UserNo"].GetString());
	strcpy(item.Password, object["Password"].GetString());

	return item;
}
