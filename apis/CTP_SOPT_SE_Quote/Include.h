// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.2.0.20200916"
#define API_NAME	"CTP_SOPT_SE"
#define DLL_PUBLIC_KEY ""

#include "../../include/CrossPlatform.h"
#include "../../include/CTP_SOPT_SE/ThostFtdcUserApiDataType.h"
#include "../../include/CTP_SOPT_SE/ThostFtdcUserApiStruct.h"
#include "../../include/CTP_SOPT_SE/ThostFtdcMdApi.h"

using namespace ctp_sopt;

#ifndef USE_CMAKE
#ifdef _WIN64
	#pragma comment(lib, "../../include/CTP_SOPT_SE/win64/soptthostmduserapi_se.lib")
#else
	#pragma comment(lib, "../../include/CTP_SOPT_SE/win32/soptthostmduserapi_se.lib")
#endif
#endif


// Ìõ¼þ±àÒë
#include "../CTP/include_pub.h"
