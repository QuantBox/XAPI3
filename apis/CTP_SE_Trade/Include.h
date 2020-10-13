// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.4.0.20190418"
#define API_NAME	"CTP_SE"
#define DLL_PUBLIC_KEY "30819D300D06092A864886F70D010101050003818B0030818702818100A5411C1BAC072723B8EC84F609B88DF913308C6B38D2F304B4285AA290686888D05D731793AD30342E8917ACC57AF8DA4976877AC1B701A664EDEB668B14027F5ED4A96A61DA00AB0208E3E7DD4C8249E88B7A3BA511C7C082979B23AA1876ED2B7A77BB1A405D15B164E586431CD3ECFB3E448A193A41BA21C34A659196C353020111"

#include "../../include/CrossPlatform.h"
#include "../../include/CTP_SE/ThostFtdcUserApiDataType.h"
#include "../../include/CTP_SE/ThostFtdcUserApiStruct.h"
#include "../../include/CTP_SE/ThostFtdcTraderApi.h"

#ifndef USE_CMAKE
#ifdef _WIN64
	#pragma comment(lib, "../../include/CTP_SE/win64/thosttraderapi_se.lib")
#else
	#pragma comment(lib, "../../include/CTP_SE/win32/thosttraderapi_se.lib")
#endif
#endif

// 条件编译
#include "../CTP/include_pub.h"

// #undef HAS_ExchangeID_Position
#undef USE_LONG_ORDER_REF
#define USE_APP_ID	1
