#pragma once

#include "stdafx.h"
/**
* 用于输出log文件的类.
*/


#ifndef LOG_H    
#define LOG_H    


//log文件路径  
#define LOG_FILE_NAME "log.txt"  

//启用开关  
#define LOG_ENABLE  

#include <fstream>    
#include <string>    
#include <ctime>  
#include <iostream>

//#include "Toolkit.h"

using namespace std;

class CLog
{
public:
	static void GetLogFilePath(CHAR* szPath)
	{
		GetModuleFileNameA(NULL, szPath, MAX_PATH);
		ZeroMemory(strrchr(szPath, '\\'), strlen(strrchr(szPath, '\\'))*sizeof(CHAR));
		strcat(szPath, "\\");
		strcat(szPath, LOG_FILE_NAME);

		//char szPath[MAX_PATH] = { 0 };
		//char drive[_MAX_DRIVE];
		//char dir[_MAX_DIR];
		//_splitpath(szPath, drive, dir, NULL, NULL);

		//_makepath(szPath, drive, dir, LOG_FILE_NAME, NULL);
	}

	static void Reset()
	{
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);

		ofstream fout(szPath, ios::trunc);
		fout.close();
	}

	//输出一个内容，可以是字符串(ascii)、整数、浮点数、布尔、枚举  
	//格式为：[2011-11-11 11:11:11] aaaaaaa并换行  
	template <class T>
	static void WriteLog(T x)
	{
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);

		// 配合一下，输出到DOS中
		cout << GetSystemTime() << x << flush;

		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << x << flush;

		// 写文件时使用密文
		//char buf[1024] = { 0 };
		//endecrypt((char*)x, buf, 1);
		//fout << buf << endl;

		//fout << x << endl;

		fout.close();
	}

	//输出2个内容，以等号连接。一般用于前面是一个变量的描述字符串，后面接这个变量的值  
	template<class T1, class T2>
	static void WriteLog2(T1 x1, T2 x2)
	{
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);
		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << x1 << " = " << x2 << endl;
		fout.close();
	}

	//输出一行当前函数开始的标志,宏传入__FUNCTION__  
	template <class T>
	static void WriteFuncBegin(T x)
	{
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);
		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << "    --------------------" << x << "  Begin--------------------" << endl;
		fout.close();
	}

	//输出一行当前函数结束的标志，宏传入__FUNCTION__  
	template <class T>
	static void WriteFuncEnd(T x)
	{
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);
		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << "--------------------" << x << "  End  --------------------" << endl;
		fout.close();
	}


private:
	//获取本地时间，格式如"[2011-11-11 11:11:11] ";   
	static string GetSystemTime()
	{
		time_t tNowTime;
		time(&tNowTime);
		tm* tLocalTime = localtime(&tNowTime);
		char szTime[30] = { '\0' };
		strftime(szTime, 30, "[%Y-%m-%d %H:%M:%S] ", tLocalTime);
		string strTime = szTime;
		return strTime;
	}

};

#ifdef LOG_ENABLE  

#define LOG_RESET              CLog::Reset();          //括号内可以是字符串(ascii)、整数、浮点数、bool等

//用下面这些宏来使用本文件  
#define LOG(x)              CLog::WriteLog(x);          //括号内可以是字符串(ascii)、整数、浮点数、bool等  
#define LOG2(x1,x2)     CLog::WriteLog2(x1,x2);  
#define LOG_FUNC        LOG(__FUNCTION__)               //输出当前所在函数名  
#define LOG_LINE        LOG(__LINE__)                       //输出当前行号  
#define LOG_FUNC_BEGIN  CLog::WriteFuncBegin(__FUNCTION__);     //形式如：[时间]"------------FuncName  Begin------------"  
#define LOG_FUNC_END     CLog::WriteFuncEnd(__FUNCTION__);      //形式如：[时间]"------------FuncName  End------------"  

#else  

#define LOG(x)                
#define LOG2(x1,x2)       
#define LOG_FUNC          
#define LOG_LINE          
#define LOG_FUNC_BEGIN    
#define LOG_FUNC_END      

#endif  

#endif