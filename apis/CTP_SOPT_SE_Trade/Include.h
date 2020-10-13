// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.2.0.20200916"
#define API_NAME	"CTP_SOPT_SE"
#define DLL_PUBLIC_KEY "0"

#include "../../include/CrossPlatform.h"
#include "../../include/CTP_SOPT_SE/ThostFtdcUserApiDataType.h"
#include "../../include/CTP_SOPT_SE/ThostFtdcUserApiStruct.h"
#include "../../include/CTP_SOPT_SE/ThostFtdcTraderApi.h"

using namespace ctp_sopt;

#ifndef USE_CMAKE
#ifdef _WIN64
	#pragma comment(lib, "../../include/CTP_SOPT_SE/win64/soptthosttraderapi_se.lib")
#else
	#pragma comment(lib, "../../include/CTP_SOPT_SE/win32/soptthosttraderapi_se.lib")
#endif
#endif

// 条件编译
#include "../CTP/include_pub.h"

// #undef HAS_ExchangeID_Position
#undef USE_LONG_ORDER_REF
#define USE_APP_ID	1
