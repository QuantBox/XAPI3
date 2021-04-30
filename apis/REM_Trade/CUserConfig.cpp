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

	doc.AddMember("user_id", StringRef(pItem->user_id), allocator);
	doc.AddMember("user_pwd", StringRef(pItem->user_pwd), allocator);
	doc.AddMember("app_id", StringRef(pItem->app_id), allocator);
	doc.AddMember("auth_code", StringRef(pItem->auth_code), allocator);

	doc.AddMember("account_id", StringRef(pItem->account_id), allocator);

	doc.EndObject(0);

	ofstream ofs(filename);
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	doc.Accept(writer);
}

void CUserConfig::WriteDefault(const char* filename)
{
	UserItem item;

	strcpy(item.user_id, "38662071");
	strcpy(item.user_pwd, "7");
	strcpy(item.app_id, "client");
	strcpy(item.auth_code, "client_auth_code");

	strcpy(item.account_id, "3866207");

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

	strcpy(item.user_id, object["user_id"].GetString());
	strcpy(item.user_pwd, object["user_pwd"].GetString());
	strcpy(item.app_id, object["app_id"].GetString());
	strcpy(item.auth_code, object["auth_code"].GetString());

	strcpy(item.account_id, object["account_id"].GetString());

	return item;
}
