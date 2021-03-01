#pragma once

#include <string>
#include <list>
#include <map>

using namespace std;

struct UserItem
{
	///用户名
	string UserID;
	///密码
	string Password;
};

class CUserConfig
{
public:
	void WriteDefault(const char* filename);
	void Write(const char* filename, UserItem* pItem);
	UserItem Read(const char* filename);
};

