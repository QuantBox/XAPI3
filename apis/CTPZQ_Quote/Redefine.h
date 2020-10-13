#pragma once

// 类型
#define TThostFtdcOptionsTypeType			TZQThostFtdcOptionsTypeType
#define TThostFtdcExchangeIDType			TZQThostFtdcExchangeIDType
#define TThostFtdcDirectionType				TZQThostFtdcDirectionType
#define TThostFtdcPosiDirectionType			TZQThostFtdcPosiDirectionType
#define TThostFtdcOffsetFlagType			TZQThostFtdcOffsetFlagType
#define TThostFtdcIdCardTypeType			TZQThostFtdcIdCardTypeType
#define TThostFtdcBrokerIDType				TZQThostFtdcBrokerIDType
#define TThostFtdcInvestorIDType			TZQThostFtdcInvestorIDType
#define TThostFtdcPasswordType				TZQThostFtdcPasswordType
#define TThostFtdcHedgeFlagType				TZQThostFtdcHedgeFlagType
#define TThostFtdcOrderPriceTypeType		TZQThostFtdcOrderPriceTypeType
#define TThostFtdcOrderSysIDType			TZQThostFtdcOrderSysIDType
#define TThostFtdcInstrumentIDType			TZQThostFtdcInstrumentIDType
#define TThostFtdcProductInfoType			TZQThostFtdcProductInfoType
#define TThostFtdcAuthCodeType				TZQThostFtdcAuthCodeType
#define	TThostFtdcOrderRefType				TZQThostFtdcOrderRefType
#define TThostFtdcInstrumentStatusType		TZQThostFtdcInstrumentStatusType
#define TThostFtdcUserIDType				TZQThostFtdcUserIDType

// 结构体
#define CThostFtdcRspInfoField				CZQThostFtdcRspInfoField
#define CThostFtdcOrderField				CZQThostFtdcOrderField
#define CThostFtdcTradeField				CZQThostFtdcTradeField
#define CThostFtdcInstrumentField			CZQThostFtdcInstrumentField
#define CThostFtdcRspUserLoginField			CZQThostFtdcRspUserLoginField
#define CThostFtdcReqUserLoginField			CZQThostFtdcReqUserLoginField
#define CThostFtdcUserLogoutField			CZQThostFtdcUserLogoutField
#define	CThostFtdcDepthMarketDataField		CZQThostFtdcDepthMarketDataField
#define CThostFtdcSpecificInstrumentField	CZQThostFtdcSpecificInstrumentField
#define CThostFtdcInvestorPositionField		CZQThostFtdcInvestorPositionField
#define CThostFtdcInputOrderField			CZQThostFtdcInputOrderField
#define CThostFtdcInputOrderActionField		CZQThostFtdcInputOrderActionField
#define CThostFtdcOrderActionField			CZQThostFtdcOrderActionField
#define	CThostFtdcRspAuthenticateField		CZQThostFtdcRspAuthenticateField
#define CThostFtdcInvestorField				CZQThostFtdcInvestorField
#define CThostFtdcQryInstrumentField		CZQThostFtdcQryInstrumentField
#define CThostFtdcReqAuthenticateField		CZQThostFtdcReqAuthenticateField
#define CThostFtdcTradingAccountField		CZQThostFtdcTradingAccountField
#define CThostFtdcQryOrderField				CZQThostFtdcQryOrderField
#define CThostFtdcQryTradeField				CZQThostFtdcQryTradeField
#define CThostFtdcQryInvestorField			CZQThostFtdcQryInvestorField
#define CThostFtdcQryInvestorPositionField	CZQThostFtdcQryInvestorPositionField
#define CThostFtdcQryTradingAccountField	CZQThostFtdcQryTradingAccountField
#define CThostFtdcUserPasswordUpdateField	CZQThostFtdcUserPasswordUpdateField


// 类
#define CThostFtdcMdApi						CZQThostFtdcMdApi
#define CThostFtdcMdSpi						CZQThostFtdcMdSpi
#define CThostFtdcTraderApi					CZQThostFtdcTraderApi
#define CThostFtdcTraderSpi					CZQThostFtdcTraderSpi

// 枚举值
#define THOST_FTDC_CP_CallOptions			ZQTHOST_FTDC_CP_CallOptions

#define THOST_FTDC_HF_Arbitrage				ZQTHOST_FTDC_HF_Arbitrage
#define THOST_FTDC_HF_Hedge					ZQTHOST_FTDC_HF_Hedge
#define THOST_FTDC_HF_Speculation			ZQTHOST_FTDC_HF_Speculation

#define THOST_FTDC_OF_CloseToday			ZQTHOST_FTDC_OF_CloseToday
#define THOST_FTDC_OF_Close					ZQTHOST_FTDC_OF_Close
#define THOST_FTDC_OF_CloseYesterday		ZQTHOST_FTDC_OF_CloseYesterday
#define THOST_FTDC_OF_Open					ZQTHOST_FTDC_OF_Open

#define THOST_FTDC_D_Sell					ZQTHOST_FTDC_D_Sell
#define THOST_FTDC_D_Buy					ZQTHOST_FTDC_D_Buy

#define THOST_FTDC_PD_Short					ZQTHOST_FTDC_PD_Short

#define THOST_FTDC_OPT_AnyPrice				ZQTHOST_FTDC_OPT_AnyPrice
#define THOST_FTDC_OPT_LimitPrice			ZQTHOST_FTDC_OPT_LimitPrice

#define THOST_FTDC_OST_Unknown				ZQTHOST_FTDC_OST_Unknown
#define THOST_FTDC_OST_Canceled				ZQTHOST_FTDC_OST_Canceled
#define THOST_FTDC_OST_AllTraded			ZQTHOST_FTDC_OST_AllTraded
#define THOST_FTDC_OST_PartTradedQueueing	ZQTHOST_FTDC_OST_PartTradedQueueing

#define THOST_FTDC_OSS_InsertRejected		ZQTHOST_FTDC_OSS_InsertRejected
#define THOST_FTDC_OSS_InsertSubmitted		ZQTHOST_FTDC_OSS_InsertSubmitted

#define THOST_FTDC_TC_GFD					ZQTHOST_FTDC_TC_GFD
#define THOST_FTDC_TC_IOC					ZQTHOST_FTDC_TC_IOC

#define THOST_FTDC_VC_AV					ZQTHOST_FTDC_VC_AV
#define THOST_FTDC_VC_CV					ZQTHOST_FTDC_VC_CV

#define THOST_FTDC_CC_Immediately			ZQTHOST_FTDC_CC_Immediately

#define THOST_FTDC_AF_Delete				ZQTHOST_FTDC_AF_Delete

#define THOST_FTDC_FCC_NotForceClose		ZQTHOST_FTDC_FCC_NotForceClose

#define THOST_FTDC_PC_Futures				ZQTHOST_FTDC_PC_Futures
#define THOST_FTDC_PC_Options				ZQTHOST_FTDC_PC_Options
#define THOST_FTDC_PC_Combination			ZQTHOST_FTDC_PC_Combination
#define THOST_FTDC_PC_EFP					ZQTHOST_FTDC_PC_EFP
#define THOST_FTDC_PC_SpotOption			ZQTHOST_FTDC_PC_SpotOption

#define THOST_FTDC_ICT_EID					ZQTHOST_FTDC_ICT_EID
#define THOST_FTDC_ICT_IDCard				ZQTHOST_FTDC_ICT_IDCard
#define THOST_FTDC_ICT_Passport				ZQTHOST_FTDC_ICT_Passport
#define THOST_FTDC_ICT_LicenseNo			ZQTHOST_FTDC_ICT_LicenseNo
#define THOST_FTDC_ICT_TaxNo				ZQTHOST_FTDC_ICT_TaxNo
#define THOST_FTDC_ICT_DrivingLicense		ZQTHOST_FTDC_ICT_DrivingLicense
#define THOST_FTDC_ICT_SocialID				ZQTHOST_FTDC_ICT_SocialID
#define THOST_FTDC_ICT_LocalID				ZQTHOST_FTDC_ICT_LocalID
#define THOST_FTDC_ICT_OtherCard			ZQTHOST_FTDC_ICT_OtherCard

#define THOST_FTDC_IS_BeforeTrading			ZQTHOST_FTDC_IS_BeforeTrading
#define THOST_FTDC_IS_NoTrading				ZQTHOST_FTDC_IS_NoTrading
#define THOST_FTDC_IS_Continous				ZQTHOST_FTDC_IS_Continous
#define THOST_FTDC_IS_AuctionOrdering		ZQTHOST_FTDC_IS_AuctionOrdering
#define THOST_FTDC_IS_AuctionBalance		ZQTHOST_FTDC_IS_AuctionBalance
#define	THOST_FTDC_IS_AuctionMatch			ZQTHOST_FTDC_IS_AuctionMatch
#define THOST_FTDC_IS_Closed				ZQTHOST_FTDC_IS_Closed


#define THOST_TE_RESUME_TYPE				ZQTHOST_TE_RESUME_TYPE
#define THOST_TERT_QUICK					ZQTHOST_TERT_QUICK

// 没有定义的结构体
#define CThostFtdcQuoteField				CZQThostFtdcQuoteField
#define CThostFtdcForQuoteRspField			CZQThostFtdcForQuoteRspField
#define CThostFtdcSettlementInfoConfirmField	CZQThostFtdcSettlementInfoConfirmField
#define CThostFtdcInputQuoteActionField		CZQThostFtdcInputQuoteActionField