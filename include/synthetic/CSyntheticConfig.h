#pragma once

#include <string> 
#include <map>
#include <fstream>
#include <set>

#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/prettywriter.h"
#include "../rapidjson/istreamwrapper.h"
#include "../rapidjson/ostreamwrapper.h"

// https://github.com/Tencent/rapidjson/blob/master/doc/stream.zh-cn.md

using namespace std;
using namespace rapidjson;

struct ConfigItem
{
	string symbol;
	string method;
	string emit;
	set<string> emits;
	map<string, double> legs;
};

class CSyntheticConfig
{
public:
	CSyntheticConfig();
	~CSyntheticConfig();

	map<string, ConfigItem> Read(const char* filename);
	void Write(const char* filename);

	bool ContainsKey(const char* product);

	void Classify(set<string> two, set<string>& s1, set<string>& s2);

	map<string, ConfigItem> GetMap();
private:
	map<string, ConfigItem> m_map;
};


