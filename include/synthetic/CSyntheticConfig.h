#pragma once

#include <string> 
#include <map>
#include <fstream>
#include <set>

using namespace std;

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

	map<string, ConfigItem> Read(const char* filename);
	void Write(const char* filename);

	bool ContainsKey(const char* product);

	void Classify(set<string> two, set<string>& s1, set<string>& s2);

	map<string, ConfigItem> GetMap();
private:
	map<string, ConfigItem> m_map;
};


