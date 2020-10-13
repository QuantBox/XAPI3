// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.1.0.20161121"
#define API_NAME	"Sgit"
#define DLL_PUBLIC_KEY "30819D300D06092A864886F70D010101050003818B0030818702818100DD88CBCCD46787C55F942428A33ED92F1F4BE6EEDFE382E1C8350451382E7E9774942C8B1B02E12A7D34B9832D21134361F51F400E372097F5CFB71F9655EDD8F6F29EBB35D94C7502C04849485389ABD1BCA977E098327435D62413ABC56637C0638C839A2496714BEE502064DACFF621E1B5E5DA101B3BE8879988E4E8D939020111"

#include "../../include/CrossPlatform.h"
#include "../../include/Sgit/SgitFtdcUserApiDataType.h"
#include "../../include/Sgit/SgitFtdcUserApiStruct.h"
#include "../../include/Sgit/SgitFtdcTraderApi.h"

using namespace fstech;

#ifndef USE_CMAKE
#ifdef _WIN64
	#pragma comment(lib, "../../include/Sgit/win64/sgittradeapi.lib")
#else
	#pragma comment(lib, "../../include/Sgit/win32/sgittradeapi.lib")
#endif
#endif

// 是否飞鼠API
#define IS_SGIT_API		1

// 条件编译
#include "../CTP/include_pub.h"

#undef HAS_ExchangeID_Position
#undef HAS_ExchangeID_Quote
