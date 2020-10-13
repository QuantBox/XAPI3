// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// 条件编译

// 使用长报单引用，飞鼠的12位前需要补0
#define USE_LONG_ORDER_REF	1
// 是否有做市商报价功能
#define HAS_Quote					1
#define HAS_Account_Future			1
#define HAS_InstrumentStatus		1
#define HAS_Settlement				1
#define HAS_TradingDay_UserLogin	1
#define CreateFtdcMdApi_argc_3		1
#define SubscribeMarketData_argc_2	1

#define HAS_ExchangeID_Position	1
#define HAS_ExchangeID_Order	1
#define HAS_ExchangeID_Quote	1
#define HAS_ExchangeID_Quote_ForQuoteSysID	1

//使用穿透试认证
#define USE_APP_ID	1
#undef USE_APP_ID

