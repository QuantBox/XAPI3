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
	doc.AddMember("AuthCode", StringRef(pItem->AuthCode), allocator);
	doc.AddMember("AppID", StringRef(pItem->AppID), allocator);

	doc.AddMember("NewPassword", StringRef(pItem->NewPassword), allocator);
	doc.AddMember("ISModifyPassword", pItem->ISModifyPassword, allocator);

	doc.EndObject(0);

	ofstream ofs(filename);
	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	doc.Accept(writer);
}

void CUserConfig::WriteDefault(const char* filename)
{
	UserItem item;

	strcpy(item.UserNo, "Q34123711");
	strcpy(item.Password, "12345678");
	strcpy(item.AuthCode, "Demo_TestCollect");
	strcpy(item.AppID, "Demo_TestCollect");
	
	// 是否修改密码
	item.ISModifyPassword = 'N';
	memset(item.NewPassword, 0, sizeof(item.NewPassword));

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
	strcpy(item.AuthCode, object["AuthCode"].GetString());
	strcpy(item.AppID, object["AppID"].GetString());

	item.ISModifyPassword = object["ISModifyPassword"].GetString()[0];
	strcpy(item.NewPassword, object["NewPassword"].GetString());

	return item;
}
