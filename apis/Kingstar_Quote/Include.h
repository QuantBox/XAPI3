// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.3.0.20151216"
#define API_NAME	"Kingstar"
#define DLL_PUBLIC_KEY ""

#include "../../include/CrossPlatform.h"
#include "../../include/Kingstar/IncEx/KSUserApiDataTypeEx.h"
#include "../../include/Kingstar/IncEx/KSUserApiStructEx.h"
#include "../../include/Kingstar/inc/KSMarketDataAPI.h"

using namespace KingstarAPI;

#ifndef USE_CMAKE
#ifdef _WIN64
	#pragma comment(lib, "../../include/Kingstar/win64/KSMarketDataAPI.lib")
#else
	#pragma comment(lib, "../../include/Kingstar/win32/KSMarketDataAPI.lib")
#endif
#endif

// 为了解决金仕达授权问题而加的功能
#define KS_LKC_FILENAME	"KSInterB2C"
#define KS_LKC_EXT	"lkc"

#define KS_COPYFILE 1

// 条件编译
#include "../CTP/include_pub.h"
