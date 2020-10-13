// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.4.0.20190417"
#define API_NAME	"CTP_SE"
#define DLL_PUBLIC_KEY ""

#include "../../include/CrossPlatform.h"
#include "../../include/CTP_SE/ThostFtdcUserApiDataType.h"
#include "../../include/CTP_SE/ThostFtdcUserApiStruct.h"
#include "../../include/CTP_SE/ThostFtdcMdApi.h"

#ifndef USE_CMAKE
#ifdef _WIN64
	#pragma comment(lib, "../../include/CTP_SE/win64/thostmduserapi_se.lib")
#else
	#pragma comment(lib, "../../include/CTP_SE/win32/thostmduserapi_se.lib")
#endif
#endif


// Ìõ¼þ±àÒë
#include "../CTP/include_pub.h"
