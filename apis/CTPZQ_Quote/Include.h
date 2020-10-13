// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.1.0.20170921"
#define API_NAME	"CTPZQ"
#define DLL_PUBLIC_KEY ""

// 由于CTPZQ与CTP各种类型定义不一样，所以重新定义一下
#include "Redefine.h"

#include "../../include/CrossPlatform.h"
#include "../../include/CTPZQ/ZQThostFtdcUserApiDataType.h"
#include "../../include/CTPZQ/ZQThostFtdcUserApiStruct.h"
#include "../../include/CTPZQ/ZQThostFtdcMdApi.h"

#ifndef USE_CMAKE
#ifdef _WIN64
	#pragma comment(lib, "../../include/CTPZQ/win64/zqthostmdapi.lib")
#else
	#pragma comment(lib, "../../include/CTPZQ/win32/zqthostmdapi.lib")
#endif
#endif


// 条件编译
#include "../CTP/include_pub.h"

#undef HAS_TradingDay_UserLogin
#undef HAS_Quote
#undef CreateFtdcMdApi_argc_3
#undef SubscribeMarketData_argc_2
