// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.1.0.20170722"
#define API_NAME	"ATP"
#define DLL_PUBLIC_KEY "30819D300D06092A864886F70D010101050003818B0030818702818100A5CE42E5AA1A873D797C8E7F4618070A746E218ED3C7A739C3772FF85960A56BF29C0FD2BBBB3124CE58231BB89C72009A4FDEC28D56FFF557BE855412F8F32BC5A57873C70599A8E19B61746F52BCBAC910588D4C021E8FCF6FF275100C71EF0082F7D9465D9F2408D2962B73D2B119BDEB10B9E17D0F8BB75BEEDD57EE6561020111"

#include "../../include/ATP/ThostFtdcUserApiDataType.h"
#include "../../include/ATP/ThostFtdcUserApiStruct.h"
#include "../../include/ATP/ThostFtdcTraderApi.h"

#ifdef _WIN64
	#pragma comment(lib, "../../include/ATP/win64/thosttraderapi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x64d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x64.lib")
	#endif
#else
	#pragma comment(lib, "../../include/ATP/win32/thosttraderapi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x86d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x86.lib")
	#endif
#endif

// 条件编译
#include "../CTP/include_pub.h"

#undef USE_LONG_ORDER_REF
#undef HAS_ExchangeID_Position
#undef HAS_ExchangeID_Order
#undef HAS_ExchangeID_Quote
#undef HAS_ExchangeID_Quote_ForQuoteSysID