#include "stdafx.h"
#include "toolkit.h"
#include <string.h>
#include <time.h>
#include <stdarg.h>

#if defined _WIN32 || WIN32 || _WINDOWS
#include <direct.h>
#define MKDIR(X) _mkdir((X));
#else
#include <sys/stat.h>
#define MKDIR(X) mkdir((X),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#define _vsnprintf vsnprintf
#endif // defined

void WriteLog(const char *fmt, ...)
{
	char buff[1024];
	va_list vl;
	va_start(vl, fmt);
	_vsnprintf(buff, 1024, fmt, vl);
	//fflush(stdout);
	va_end(vl);
	//char temp[2048] = "XSpeed:";
	//OutputDebugStringA(strcat(temp, buff));
#if defined _WIN32 || WIN32 || _WINDOWS
	OutputDebugStringA(buff);
#endif
}

void makedirs(const char* dir)
{
	if (nullptr == dir)
		return;

	size_t len = strlen(dir);

	char * p = new char[len + 1];
	strcpy(p, dir);
	for (size_t i = 0; i < len; ++i)
	{
		char ch = p[i];
		if ('\\' == ch || '/' == ch)
		{
			p[i] = '\0';
			MKDIR(p);
			p[i] = ch;
		}
	}
	delete[] p;
}

double my_round(float val, int x)
{
	double i = ((long)(val * 10000.0 + 0.5)) / 10000.0;
	return i;
}

void split_int_to_4_2_2(int HHmmss, int* _HH, int* _mm, int* _ss)
{
	// 20190917
	// 2019 09 17
	// 135400
	// 13 54 00
	*_HH = HHmmss / 10000;
	*_mm = HHmmss % 10000 / 100;
	*_ss = HHmmss % 100;
}

int contact_4_2_2_to_HHmmss(int yyyy, int MM, int ss)
{
	return yyyy * 10000 + MM * 100 + ss;
}

void split_str_to_4_2_2(const char* szHHmmss, int* _HH, int* _mm, int* _ss)
{
	int HHmmss = atoi(szHHmmss);
	split_int_to_4_2_2(HHmmss, _HH, _mm, _ss);
}

void split_HH_mm_ss_to_2_2_2(const char* szHH_mm_ss, int* _HH, int* _mm, int* _ss)
{
	// 处理有间隔的字符串,但没有处理HH:mm:ss.fff的情况
	// 13:54:00
	// 13 54 00
	// 2019-09-17
	// 2019 09 17
	int len = 8;

	*_HH = atoi(&szHH_mm_ss[0]);
	*_mm = atoi(&szHH_mm_ss[len - 2 - 3]);
	*_ss = atoi(&szHH_mm_ss[len - 2]);
}

void split_yyyy_MM_dd_to_2_2_2(const char* szyyyy_MM_dd, int* _HH, int* _mm, int* _ss)
{
	// 处理有间隔的字符串
	// 13:54:00
	// 13 54 00
	// 2019-09-17
	// 2019 09 17
	int len = strlen(szyyyy_MM_dd);

	*_HH = atoi(&szyyyy_MM_dd[0]);
	*_mm = atoi(&szyyyy_MM_dd[len - 2 - 3]);
	*_ss = atoi(&szyyyy_MM_dd[len - 2]);
}

tm tm_add_seconds(tm* _Tm, int sec)
{
	time_t _t = mktime(_Tm);
	_t += sec;
	// 需要立即取出，所以使用了*
	return *localtime(&_t);
}

tm yyyy_MM_dd_to_tm(int yyyy, int MM, int dd)
{
	tm _tm = { 0 }; // 记得要清空
	_tm.tm_year = yyyy - 1900;
	_tm.tm_mon = MM - 1;
	_tm.tm_mday = dd;

	// 直接返回的话wday还是空的，所以得转换一下
	// 因为在推算前后交易日时需要得到星期几
	return tm_add_seconds(&_tm, 0);
}

tm yyyyMMdd_to_tm(int yyyyMMdd)
{
	int yyyy = 0;
	int MM = 0;
	int dd = 0;
	split_int_to_4_2_2(yyyyMMdd, &yyyy, &MM, &dd);
	return yyyy_MM_dd_to_tm(yyyy, MM, dd);
}

int tm_to_yyyyMMdd(tm* _tm)
{
	return (_tm->tm_year + 1900) * 10000 + (_tm->tm_mon + 1) * 100 + _tm->tm_mday;
}

int tm_to_HHmmss(tm* _tm)
{
	return _tm->tm_hour * 10000 + _tm->tm_min * 100 + _tm->tm_sec;
}

int current_date()
{
	time_t now = time(0);
	return tm_to_yyyyMMdd(localtime(&now));
}

int current_time()
{
	time_t now = time(0);
	return tm_to_HHmmss(localtime(&now));
}

tm current_date_tm()
{
	time_t now = time(0);
	return *localtime(&now);
}

tm get_pre_trading_day(tm* _tm)
{
	// 没有处理长假问题，但使用时不用担心

	// 从周日开始，0-6

	// 周日周一，都要退到周五，其它只要退一天即可
	if (_tm->tm_wday == 1)
	{
		// 周一
		return tm_add_seconds(_tm, -86400 * 3);
	}
	if (_tm->tm_wday == 0)
	{
		// 周日
		return tm_add_seconds(_tm, -86400 * 2);
	}
	// 周六到周二
	return tm_add_seconds(_tm, -86400 * 1);
}

tm get_next_trading_day(tm* _tm)
{
	// 没有处理长假问题，但使用时不用担心

	// 从周日开始，0-6

	if (_tm->tm_wday == 5)
	{
		// 周五
		return tm_add_seconds(_tm, 86400 * 3);
	}
	if (_tm->tm_wday == 6)
	{
		// 周六
		return tm_add_seconds(_tm, 86400 * 2);
	}
	// 周日到周四
	return tm_add_seconds(_tm, 86400 * 1);
}

int str_to_yyyyMMdd(const char* yyyyMMdd)
{
	return atoi(yyyyMMdd);
}

int str_to_HHmmss(const char* HHmmss)
{
	// UpdateTime处理
	int HH = 0;
	int mm = 0;
	int ss = 0;
	// 兼容
	int len = strlen(HHmmss);
	if (len <= 6)
	{
		split_str_to_4_2_2(HHmmss, &HH, &mm, &ss);
	}
	else
	{
		// 处理HH:mm:ss
		split_HH_mm_ss_to_2_2_2(HHmmss, &HH, &mm, &ss);
	}
	return contact_4_2_2_to_HHmmss(HH, mm, ss);
}

int GetUpdateTime(const char* UpdateTime, int* _UpdateTime, int* UpdateMillisec)
{
	*UpdateMillisec = 0;

	*_UpdateTime = str_to_HHmmss(UpdateTime);

	// 兼容
	int len = strlen(UpdateTime);
	if (len > 9)
	{
		// 处理HH:mm:ss.fff
		*UpdateMillisec = atoi(&UpdateTime[9]);
	}

	return *_UpdateTime / 10000;
}

void GetExchangeTime(int iTradingDay, int iPreTradingDay, char* TradingDay, char* ActionDay, char* UpdateTime, int* _TradingDay, int* _ActionDay, int* _UpdateTime, int* UpdateMillisec)
{
	// iTradingDay登录时获取的交易日，
	// iPreTradingDay登录时获取的交易日的前一交易日

	// 郑

	// UpdateTime处理
	int HH = GetUpdateTime(UpdateTime, _UpdateTime, UpdateMillisec);

	int tradingDay = atoi(TradingDay);
	int actionDay = atoi(ActionDay);

	// 白天直接使用
	*_TradingDay = tradingDay;
	*_ActionDay = actionDay;

	// 白天不处理，最早18点44就收到了数据
	if (HH > 6 && HH < 18)
		return;

	if (tradingDay != actionDay)
	{
		// 上海，21点到0点之间正好不相等，不需要处理
		// 上海第二天，两个值就相等了
		return;
	}

	// 1. 登录时间周一早上
	// 2. 登录时间周日
	// 3. 登录时间，周六晚
	// 4. 登录时间，周五晚
	if (HH <= 6)
	{
		// 只有上期所和能源中心的合约会进入到此状态，而它们的Day都是正确的，不用调整
		return;
	}

	if (HH >= 18)
	{
		// 大商所ActionDay存的是交易日，需要修正成交易日的前一交易日
		// 郑商所TradingDay存的是行情日，需要修正成交易日
		if (iTradingDay == tradingDay)
		{
			// 大商所
			// 由于长假前没有夜盘，所以不用担心前交易日计算错误
			*_ActionDay = iPreTradingDay;
		}
		else
		{
			// 郑商所
			*_TradingDay = iTradingDay;
		}
	}
}

void GetOnFrontDisconnectedMsg(int ErrorId, char* ErrorMsg)
{
	switch (ErrorId)
	{
	case 0x1001:
		strcpy(ErrorMsg, "0x1001 4097 网络读失败");
		break;
	case 0x1002:
		strcpy(ErrorMsg, "0x1002 4098 网络写失败");
		break;
	case 0x2001:
		strcpy(ErrorMsg, "0x2001 8193 接收心跳超时");
		break;
	case 0x2002:
		strcpy(ErrorMsg, "0x2002 8194 发送心跳失败");
		break;
	case 0x2003:
		strcpy(ErrorMsg, "0x2003 8195 收到错误报文");
		break;
	case 0x2004:
		strcpy(ErrorMsg, "0x2004 8196 服务器主动断开");
		break;
	default:
		sprintf(ErrorMsg, "%x %d 未知错误", ErrorId, ErrorId);
		break;
	}
}

#if defined _WIN32 || WIN32 || _WINDOWS
void GetDllPathByFunctionName(const char* szFunctionName, char* szPath)
{
	HMODULE hModule = nullptr;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, szFunctionName, &hModule);

	GetModuleFileNameA(hModule, szPath, MAX_PATH);
}

void GetExePath(char* szPath)
{
	HMODULE hModule = GetModuleHandleA(nullptr);
	//char szPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(hModule, szPath, MAX_PATH);
}

void GetDirectoryByPath(const char* szPath, char* szDirectory)
{
	//char szPath[_MAX_PATH] = { 0 };
	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char fname[_MAX_FNAME] = { 0 };
	char ext[_MAX_EXT] = { 0 };

	_splitpath(szPath, drive, dir, fname, ext);
	_makepath(szDirectory, drive, dir, nullptr, nullptr);
}

void GetNewPathInSameDirectory(const char* szPath, const char* szFname, const char* szExt, char* szFileName)
{
	//char szPath[_MAX_PATH] = { 0 };
	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char fname[_MAX_FNAME] = { 0 };
	char ext[_MAX_EXT] = { 0 };

	_splitpath(szPath, drive, dir, fname, ext);
	_makepath(szFileName, drive, dir, szFname, szExt);
}
#else
void GetDllPathByFunctionName(const char* szFunctionName, char* szPath)
{}
void GetExePath(char* szPath)
{}
void GetDirectoryByPath(const char* szPath, char* szDirectory)
{}
void GetNewPathInSameDirectory(const char* szPath, const char* szFname, const char* szExt, char* szFileName)
{}

#endif
