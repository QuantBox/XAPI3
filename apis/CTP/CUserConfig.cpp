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

	doc.AddMember("_UserID", "用户ID", allocator);
	doc.AddMember("UserID", StringRef(pItem->UserID.c_str()), allocator);
	doc.AddMember("_Password", "密码", allocator);
	doc.AddMember("Password", StringRef(pItem->Password.c_str()), allocator);
	//doc.AddMember("_ExtInfo", "其它信息", allocator);
	//doc.AddMember("ExtInfo", StringRef(pItem->ExtInfo.c_str()), allocator);

	doc.EndObject(0);

	ofstream ofs(filename);
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	doc.Accept(writer);
}

void CUserConfig::WriteDefault(const char* filename)
{
	UserItem item;

	item.UserID = "654321";
	item.Password = "123456";
	//item.ExtInfo = "";

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

	item.UserID = object["UserID"].GetString();
	item.Password = object["Password"].GetString();
	//item.ExtInfo = object["ExtInfo"].GetString();

	return item;
}
