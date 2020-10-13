// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.1.0.20170921"
#define API_NAME	"CTPZQ"
#define DLL_PUBLIC_KEY ""

// 由于CTPZQ与CTP各种类型定义不一样，所以重新定义一下
#include "../CTPZQ_Quote/Redefine.h"

#include "../../include/CrossPlatform.h"
#include "../../include/CTPZQ/ZQThostFtdcUserApiDataType.h"
#include "../../include/CTPZQ/ZQThostFtdcUserApiStruct.h"
#include "../../include/CTPZQ/ZQThostFtdcTraderApi.h"

#ifndef USE_CMAKE
#ifdef _WIN64
	#pragma comment(lib, "../../include/CTPZQ/win64/zqthosttraderapi.lib")
#else
	#pragma comment(lib, "../../include/CTPZQ/win32/zqthosttraderapi.lib")
#endif
#endif

// 条件编译
#include "../CTP/include_pub.h"

#undef HAS_Quote
#undef HAS_Account_Future
#undef HAS_TradingDay_UserLogin
#undef HAS_InstrumentStatus
#undef HAS_Settlement
#undef USE_LONG_ORDER_REF
