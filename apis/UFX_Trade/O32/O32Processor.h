#pragma once

#include <mutex>
#include "ufx_struct_o32.h"
#include "../OrderMap.h"

class CMsgQueue;

class CO32Processor
{
public:
	CO32Processor();
	~CO32Processor();

	int Do_Error(RSP_ErrorField3* pErr, const char* source);
	// 10001 登录 
	int Do_10001(RSP_DL_10001* pRsp, int count, RSP_ErrorField3* pErr);

	// 31001 证券持仓查询
	int Do_31001(RSP_ZQCCCX_31001* pRsps, int count, RSP_ErrorField3* pErr);
	// 31003 期货持仓查询
	int Do_31003(RSP_QHCCCX_31003* pRsps, int count, RSP_ErrorField3* pErr);

	// 32001 证券委托查询
	int Do_32001(RSP_ZQWTCX_32001* pRsps, int count, RSP_ErrorField3* pErr);
	// 32003 期货委托查询
	int Do_32003(RSP_QHWTCX_32003* pRsps, int count, RSP_ErrorField3* pErr);

	// 33001 证券成交查询
	int Do_33001(RSP_ZQCJCX_33001* pRsps, int count, RSP_ErrorField3* pErr);
	// 33003 期货成交查询
	int Do_33003(RSP_QHCJCX_33003* pRsps, int count, RSP_ErrorField3* pErr);

	// 34001 账户资金查询
	int Do_34001(RSP_ZHZJCX_34001* pRsps, int count, RSP_ErrorField3* pErr);
	// 34003 期货保证金账户查询
	int Do_34003(RSP_QHBZJZHCX_34003* pRsps, int count, RSP_ErrorField3* pErr);

	// 91004 期货委托
	int Do_91004(RSP_QHWT_91004* pRsps, int count, RSP_ErrorField3* pErr, int packetId, RSP_RiskField* pRiskRsps, int riskCount);
	// 91105 期货委托撤单
	int Do_91105(RSP_QHWTCD_91105* pRsps, int count, RSP_ErrorField3* pErr, int packetId);

	// 91001 普通买卖委托
	int Do_91001(RSP_PTMMWT_91001* pRsps, int count, RSP_ErrorField3* pErr, int packetId, RSP_RiskField* pRiskRsps, int riskCount);
	// 91101 委托撤单
	int Do_91101(RSP_WTCD_91101* pRsps, int count, RSP_ErrorField3* pErr, int packetId);

	// a 委托下达
	int Do_a(RSP_WTXD_a* pRsps, int count, RSP_ErrorField3* pErr);
	// b 委托确认
	int Do_b(RSP_WTQR_b* pRsps, int count, RSP_ErrorField3* pErr);
	// c 委托废单
	int Do_c(RSP_WTFD_c* pRsps, int count, RSP_ErrorField3* pErr);
	// d 委托撤单
	int Do_d(RSP_WTCD_d* pRsps, int count, RSP_ErrorField3* pErr);
	// e 委托撤成
	int Do_e(RSP_WTCC_e* pRsps, int count, RSP_ErrorField3* pErr);
	// f 委托撤废
	int Do_f(RSP_WTCD_f* pRsps, int count, RSP_ErrorField3* pErr);
	// g 委托成交
	int Do_g(RSP_WTCJ_g* pRsps, int count, RSP_ErrorField3* pErr);

public:
	CMsgQueue*					m_msgQueue;
	void*						m_pClass;

public:
	// 外部传入，不要动它
	COrderMap<int>*			m_pOrderMap;
	mutex					m_csOrderMap;
private:
	char					m_Text[256];
};

