#pragma once
#include "../../../include/UFX/t2sdk_interface.h"

#include "ufx_struct_o32.h"


class CO32Processor;

class CO32Codec
{
public:
	CO32Codec();
	~CO32Codec();

public:
	// 错误
	int IsError(IF2UnPacker *lpUnPacker, RSP_ErrorField3* pRsp);
	// 风控
	int IsRisk(IF2UnPacker *lpUnPacker, int count, RSP_RiskField* pRsps);

	// 响应
	int OnResponse(int functionNo, int packetId, IF2UnPacker * lpUnPacker, IF2UnPacker * lpUnPacker_key, RSP_DL_10001* pRSP_DL_10001, RSP_ErrorField3* pErr);
	// 订阅
	int OnSub(IF2UnPacker * lpUnPacker);
public:
	// 10000 心跳 
	int ReqFunction_10000(REQ_XT_10000* pReq);
	// 10001 登录 
	int ReqFunction_10001(REQ_DL_10001* pReq);

	//// 91090 篮子委托
	//int ReqFunction_91090(REQ_LZWT_91090* pReqs, int count);
	//// 91102 委托撤单（按委托批号撤单）
	//int ReqFunction_91102(REQ_LZCD_91102* pReq);

	// 31001 证券持仓查询
	int ReqFunction_31001(REQ_ZQCCCX_31001* pReq);
	// 32001 证券委托查询
	int ReqFunction_32001(REQ_ZQWTCX_32001* pReq);
	// 33001 证券成交查询
	int ReqFunction_33001(REQ_ZQCJCX_33001* pReq);
	// 34001 账户资金查询
	int ReqFunction_34001(REQ_ZHZJCX_34001* pReq);

	// 91001 普通买卖委托
	int ReqFunction_91001(REQ_PTMMWT_91001* pReqs, int count);
	// 91101 委托撤单
	int ReqFunction_91101(REQ_WTCD_91101* pReq);

	// =======================================
	// 31003 期货持仓查询
	int ReqFunction_31003(REQ_QHCCCX_31003* pReq);
	// 32003 期货委托查询
	int ReqFunction_32003(REQ_QHWTCX_32003* pReq);
	// 33003 期货成交查询
	int ReqFunction_33003(REQ_QHCJCX_33003* pReq);
	// 34003 期货保证金账户查询
	int ReqFunction_34003(REQ_QHBZJZHCX_34003* pReq);

	// 91004 期货委托，平今仓不能混用成平仓，这点与CTP有区别
	int ReqFunction_91004(REQ_QHWT_91004* pReq);
	// 91105 期货委托撤单
	int ReqFunction_91105(REQ_QHWTCD_91105* pReq);
	
private:
	// 10001 登录 
	int OnResponse_10000(IF2UnPacker *lpUnPacker, int count, RSP_XT_10000* pRsps);
	// 10001 登录 
	int OnResponse_10001(IF2UnPacker *lpUnPacker, int count, RSP_DL_10001* pRsps);

	//// 91090 篮子委托
	//int OnResponse_91090(IF2UnPacker *lpUnPacker, int count, RSP_LZWT_91090* pRsps);
	//// 91102 委托撤单（按委托批号撤单）
	//int OnResponse_91102(IF2UnPacker *lpUnPacker, int count, RSP_LZCD_91102* pRsps);

	// 31001 证券持仓查询
	int OnResponse_31001(IF2UnPacker *lpUnPacker, int count, RSP_ZQCCCX_31001* pRsps);
	// 32001 证券委托查询
	int OnResponse_32001(IF2UnPacker *lpUnPacker, int count, RSP_ZQWTCX_32001* pRsps);
	// 33001 证券成交查询
	int OnResponse_33001(IF2UnPacker *lpUnPacker, int count, RSP_ZQCJCX_33001* pRsps);
	// 34001 账户资金查询
	int OnResponse_34001(IF2UnPacker *lpUnPacker, int count, RSP_ZHZJCX_34001* pRsps);
	
	// 31003 期货持仓查询
	int OnResponse_31003(IF2UnPacker *lpUnPacker, int count, RSP_QHCCCX_31003* pRsps);
	// 32003 期货委托查询
	int OnResponse_32003(IF2UnPacker *lpUnPacker, int count, RSP_QHWTCX_32003* pRsps);
	// 33003 期货成交查询
	int OnResponse_33003(IF2UnPacker *lpUnPacker, int count, RSP_QHCJCX_33003* pRsps);
	// 34003 期货保证金账户查询
	int OnResponse_34003(IF2UnPacker *lpUnPacker, int count, RSP_QHBZJZHCX_34003* pRsps);
	
	// 91004 期货委托，平今仓不能混用成平仓，这点与CTP有区别
	int OnResponse_91004(IF2UnPacker *lpUnPacker, int count, RSP_QHWT_91004* pRsps);
	// 91105 期货委托撤单
	int OnResponse_91105(IF2UnPacker *lpUnPacker, int count, RSP_QHWTCD_91105* pRsps);
	// 91001 普通买卖委托
	int OnResponse_91001(IF2UnPacker *lpUnPacker, int count, RSP_PTMMWT_91001* pRsps);
	// 91101 委托撤单
	int OnResponse_91101(IF2UnPacker *lpUnPacker, int count, RSP_WTCD_91101* pRsps);
	
	// =======================================

	int SubFunction_(SUB_* pReq);

	// a 委托下达
	int OnResponse_a(IF2UnPacker *lpUnPacker, int count, RSP_WTXD_a* pRsps);
	// b 委托确认
	int OnResponse_b(IF2UnPacker *lpUnPacker, int count, RSP_WTQR_b* pRsps);
	// c 委托废单
	int OnResponse_c(IF2UnPacker *lpUnPacker, int count, RSP_WTFD_c* pRsps);
	// d 委托撤单
	int OnResponse_d(IF2UnPacker *lpUnPacker, int count, RSP_WTCD_d* pRsps);
	// e 委托撤成
	int OnResponse_e(IF2UnPacker *lpUnPacker, int count, RSP_WTCC_e* pRsps);
	// f 委托撤废
	int OnResponse_f(IF2UnPacker *lpUnPacker, int count, RSP_WTCD_f* pRsps);
	// g 委托成交
	int OnResponse_g(IF2UnPacker *lpUnPacker, int count, RSP_WTCJ_g* pRsps);


	// =======================================
	

public:
	CO32Processor*				m_pProcessor;
	CConnectionInterface*		m_lpConnection;
};

