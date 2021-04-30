#pragma once

#include <string>
#include <list>
#include <map>

using namespace std;

struct UserItem
{
	char user_id[32];
	char user_pwd[32];
	char app_id[32];
	char auth_code[32];

	char account_id[32];
};

class CUserConfig
{
public:
	void WriteDefault(const char* filename);
	void Write(const char* filename, UserItem* pItem);
	UserItem Read(const char* filename);
};

