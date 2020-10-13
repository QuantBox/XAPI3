#pragma once
#include "../../../include/UFX/t2sdk_interface.h"

#include "ufx_struct_zq.h"


class CZQCodec
{
public:
	CZQCodec();
	~CZQCodec();

public:
	int IsError(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError);

	//331100 登入 
	int ReqFunction_331100(REQ_DL_331100* pReq);
	int OnResponse_331100(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_DL_331100* pRsp);

	//400 证券行情查询
	int ReqFunction_400(REQ_ZQHQCX_400* pReq);
	int OnResponse_400(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_ZQHQCX_400* pRsp);

	//333002 普通委托
	int ReqFunction_333002(REQ_PTWT_333002* pReq);
	int OnResponse_333002(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_PTWT_333002* pRsp);

	//332254 资金快速查询
	int ReqFunction_332254(REQ_ZJKSCX_332254* pReq);
	int OnResponse_332254(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_ZJKSCX_332254* pRsp);

	//333103 持仓快速查询
	int ReqFunction_333103(REQ_CCKSCX_333103* pReq);
	int OnResponse_333103(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_CCKSCX_333103* pRsp);

	//330300 证券代码信息查询
	int ReqFunction_330300(REQ_ZQDMXXCX_330300* pReq);
	int OnResponse_330300(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_ZQDMXXCX_330300* pRsp);

	//620001_12 订阅证券成交回报功能
	//620001_23 订阅证券委托回报
	int SubFunction_620001(SUB_620001* pReq);
	//订阅证券成交回报
	int OnResponse_620003_12(IF2UnPacker *lpUnPacker);
	//订阅委托回报
	int OnResponse_620003_23(IF2UnPacker *lpUnPacker);

public:
	CConnectionInterface*		m_lpConnection;
};

