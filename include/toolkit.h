#ifndef _TOOLKIT_H_
#define _TOOLKIT_H_

#include <vector>
#include <set>
#include <string>

#pragma warning(disable:4996)

using namespace std;

void WriteLog(const char *fmt, ...);

//输入路径，生成多级目录
void makedirs(const char* dir);

void split_int_to_4_2_2(int HHmmss, int* _HH, int* _mm, int* _ss);
int contact_4_2_2_to_HHmmss(int yyyy, int MM, int ss);
void split_str_to_4_2_2(const char* szHHmmss, int* _HH, int* _mm, int* _ss);

void split_HH_mm_ss_to_2_2_2(const char* szHH_mm_ss, int* _HH, int* _mm, int* _ss);

void split_yyyy_MM_dd_to_2_2_2(const char* szyyyy_MM_dd, int* _HH, int* _mm, int* _ss);

tm tm_add_seconds(tm* _Tm, int sec);

tm yyyy_MM_dd_to_tm(int yyyy, int MM, int dd);
tm yyyyMMdd_to_tm(int yyyyMMdd);

int tm_to_yyyyMMdd(tm* _tm);

int tm_to_HHmmss(tm* _tm);

int current_date();

int current_time();

tm current_date_tm();

tm get_pre_trading_day(tm* _tm);
tm get_next_trading_day(tm* _tm);

int str_to_yyyyMMdd(const char* yyyyMMdd);

int str_to_HHmmss(const char* HHmmss);

int GetUpdateTime(const char* UpdateTime, int* _UpdateTime, int* UpdateMillisec);

void GetExchangeTime(int iTradingDay, int iPreTradingDay, char* TradingDay, char* ActionDay, char* UpdateTime, int* _TradingDay, int* _ActionDay, int* _UpdateTime, int* UpdateMillisec);

double my_round(float val, int x = 0);

//根据OnFrontDisconnected(int nReason)的值填上错误消息
void GetOnFrontDisconnectedMsg(int ErrorId, char* ErrorMsg);

#if _WIN32
// 通过公开的函数名，得到当前的路径
void GetDllPathByFunctionName(const char* szFunctionName, char* szPath);
// 得到当前可执行程序的路径
void GetExePath(char* szPath);
// 传入路径，得到目录
void GetDirectoryByPath(const char* szPath, char* szDirectory);
// 通过一个路径得到新的路径
void GetNewPathInSameDirectory(const char* szPath, const char* szFname, const char* szExt, char* szFileName);
#endif

#endif
