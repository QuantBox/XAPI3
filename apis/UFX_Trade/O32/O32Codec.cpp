#include "stdafx.h"
#include "O32Codec.h"
#include "O32Processor.h"


#include <assert.h>

#include "..\..\..\include\ApiHeader.h"
#include "../../../include/switch_string.h"



#define SAFE_COPY_C(x) {x = lpUnPacker->GetCharByIndex(j);}
#define SAFE_COPY_I(x) {x = lpUnPacker->GetIntByIndex(j);}
#define SAFE_COPY_F(x) {x = lpUnPacker->GetDoubleByIndex(j);}
#define SAFE_COPY_S(x) {SAFE_STRNCPY(x, lpUnPacker->GetStrByIndex(j));}

CO32Codec::CO32Codec()
{
	m_pProcessor = nullptr;
	m_lpConnection = nullptr;
}


CO32Codec::~CO32Codec()
{
}

int CO32Codec::OnResponse(int functionNo, int packetId, IF2UnPacker * lpUnPacker, IF2UnPacker * lpUnPacker_key, RSP_DL_10001* pRSP_DL_10001, RSP_ErrorField3* pErr)
{
	if (m_pProcessor == nullptr)
	{
		return 0;
	}
	// 数据集有两个，第一个是错误信息，第二个是结果集
	// 有可能会没有结果集，如果强行选择第二个结果集，实际还是第一个错误信息
	// 数据集有三个，第三个是风控信息

	int datasetCount = lpUnPacker->GetDatasetCount();
	if (datasetCount <= 0)
		return -1;

	lpUnPacker->SetCurrentDatasetByIndex(0);
	IsError(lpUnPacker, pErr);

	int count = 0;
	int riskCount = 0;

	RSP_RiskField* pRiskRsps = nullptr;
	if (datasetCount > 2)
	{
		lpUnPacker->SetCurrentDatasetByIndex(2);
		riskCount = lpUnPacker->GetRowCount();
		// 可能有风控信息，取出来
		pRiskRsps = new RSP_RiskField[riskCount];
		IsRisk(lpUnPacker, riskCount, pRiskRsps);
	}

	if (datasetCount > 1)
	{
		lpUnPacker->SetCurrentDatasetByIndex(1);
		count = lpUnPacker->GetRowCount();
	}

	int iRet = -1;

	switch (functionNo)
	{
	case 10001:
	{
		// 需要对其进行user_token进行填充，后面还要用，不能删
		iRet = OnResponse_10001(lpUnPacker, count, pRSP_DL_10001);
		m_pProcessor->Do_10001(pRSP_DL_10001, count, pErr);
	}
	break;
	case 31001:
	{
		RSP_ZQCCCX_31001* pRsps = new RSP_ZQCCCX_31001[count];
		memset(pRsps, 0, sizeof(RSP_ZQCCCX_31001)*count);
		iRet = OnResponse_31001(lpUnPacker, count, pRsps);
		m_pProcessor->Do_31001(pRsps, count, pErr);
		delete[] pRsps;
		//iRet = -1;
	}
	break;
	case 34001:
	{
		RSP_ZHZJCX_34001* pRsps = new RSP_ZHZJCX_34001[count];
		memset(pRsps, 0, sizeof(RSP_ZHZJCX_34001)*count);
		iRet = OnResponse_34001(lpUnPacker, count, pRsps);
		m_pProcessor->Do_34001(pRsps, count, pErr);
		delete[] pRsps;
		//iRet = -1;
	}
	break;
	case 34003:
	{
		RSP_QHBZJZHCX_34003* pRsps = new RSP_QHBZJZHCX_34003[count];
		memset(pRsps, 0, sizeof(RSP_QHBZJZHCX_34003)*count);
		iRet = OnResponse_34003(lpUnPacker, count, pRsps);
		m_pProcessor->Do_34003(pRsps, count, pErr);
		delete[] pRsps;
	}
	break;
	case 91004:
	{
		RSP_QHWT_91004* pRsps = new RSP_QHWT_91004[count];
		memset(pRsps, 0, sizeof(RSP_QHWT_91004)*count);
		iRet = OnResponse_91004(lpUnPacker, count, pRsps);
		m_pProcessor->Do_91004(pRsps, count, pErr, packetId, pRiskRsps, riskCount);
		delete[] pRsps;
		iRet = -1;
	}
	break;
	case 91105:
	{
		RSP_QHWTCD_91105* pRsps = new RSP_QHWTCD_91105[count];
		memset(pRsps, 0, sizeof(RSP_QHWTCD_91105)*count);
		iRet = OnResponse_91105(lpUnPacker, count, pRsps);
		m_pProcessor->Do_91105(pRsps, count, pErr, packetId);
		delete[] pRsps;
		iRet = -1;
	}
	break;
	case 32003:
	{
		RSP_QHWTCX_32003* pRsps = new RSP_QHWTCX_32003[count];
		memset(pRsps, 0, sizeof(RSP_QHWTCX_32003)*count);
		iRet = OnResponse_32003(lpUnPacker, count, pRsps);
		m_pProcessor->Do_32003(pRsps, count, pErr);
		delete[] pRsps;
	}
	break;
	case 33003:
	{
		RSP_QHCJCX_33003* pRsps = new RSP_QHCJCX_33003[count];
		memset(pRsps, 0, sizeof(RSP_QHCJCX_33003)*count);
		iRet = OnResponse_33003(lpUnPacker, count, pRsps);
		m_pProcessor->Do_33003(pRsps, count, pErr);
		delete[] pRsps;
		iRet = -1;
	}
	break;
	case 31003:
	{
		RSP_QHCCCX_31003* pRsps = new RSP_QHCCCX_31003[count];
		memset(pRsps, 0, sizeof(RSP_QHCCCX_31003)*count);
		iRet = OnResponse_31003(lpUnPacker, count, pRsps);
		m_pProcessor->Do_31003(pRsps, count, pErr);
		delete[] pRsps;
		// 太多了，不打印
		iRet = 0;
	}
	break;
	case 32001:
	{
		RSP_ZQWTCX_32001* pRsps = new RSP_ZQWTCX_32001[count];
		memset(pRsps, 0, sizeof(RSP_ZQWTCX_32001)*count);
		iRet = OnResponse_32001(lpUnPacker, count, pRsps);
		m_pProcessor->Do_32001(pRsps, count, pErr);
		delete[] pRsps;
		iRet = 1;
	}
	break;
	case 33001:
	{
		RSP_ZQCJCX_33001* pRsps = new RSP_ZQCJCX_33001[count];
		memset(pRsps, 0, sizeof(RSP_ZQCJCX_33001)*count);
		iRet = OnResponse_33001(lpUnPacker, count, pRsps);
		m_pProcessor->Do_33001(pRsps, count, pErr);
		delete[] pRsps;
	}
	break;
	case 91001:
	{
		RSP_PTMMWT_91001* pRsps = new RSP_PTMMWT_91001[count];
		memset(pRsps, 0, sizeof(RSP_PTMMWT_91001)*count);
		iRet = OnResponse_91001(lpUnPacker, count, pRsps);
		m_pProcessor->Do_91001(pRsps, count, pErr, packetId, pRiskRsps, riskCount);
		delete[] pRsps;
		iRet = -1;
	}
	break;
	case 91101:
	{
		RSP_WTCD_91101* pRsps = new RSP_WTCD_91101[count];
		memset(pRsps, 0, sizeof(RSP_WTCD_91101)*count);
		iRet = OnResponse_91101(lpUnPacker, count, pRsps);
		m_pProcessor->Do_91101(pRsps, count, pErr, packetId);
		delete[] pRsps;
	}
	break;
	case 10000:
		// 心跳包是自己主动发出的，不用处理，
		iRet = 0;
		break;

		/*case 91090:
			OnResponse_91090(lpUnPacker, &err, &m_RSP_DL_10001);
			break;
		case 91102:
			OnResponse_91102(lpUnPacker, &err, &m_RSP_DL_10001);
			break;*/

	default:
		printf("---%d---\n", functionNo);
		iRet = -1;
		break;
	}
	delete[] pRiskRsps;
	return iRet;
}

int CO32Codec::IsError(IF2UnPacker *lpUnPacker, RSP_ErrorField3* pRsp)
{
	memset(pRsp, 0, sizeof(RSP_ErrorField3));

	for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
	{
		char type = lpUnPacker->GetColType(j);
		const char* name = lpUnPacker->GetColName(j);
		switch (hash_(name))
		{
		case "ErrorCode"_hash:	SAFE_COPY_I(pRsp->ErrorCode);	break;
		case "ErrorMsg"_hash:	SAFE_COPY_S(pRsp->ErrorMsg);	break;
		case "MsgDetail"_hash:	SAFE_COPY_S(pRsp->MsgDetail);	break;
		case "DataCount"_hash:	SAFE_COPY_I(pRsp->DataCount);	break;
		default:
			//assert(false);
			break;
		}
	}

	return pRsp->ErrorCode;
}

int CO32Codec::IsRisk(IF2UnPacker *lpUnPacker, int count, RSP_RiskField* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_RiskField* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_RiskField));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "risk_serial_no"_hash:	SAFE_COPY_I(pRsp->risk_serial_no);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "entrust_direction"_hash:	SAFE_COPY_S(pRsp->entrust_direction);	break;
			case "futures_direction"_hash:	SAFE_COPY_C(pRsp->futures_direction);	break;
			case "risk_no"_hash:	SAFE_COPY_I(pRsp->risk_no);	break;
			case "risk_type"_hash:	SAFE_COPY_C(pRsp->risk_type);	break;
			case "risk_summary"_hash:	SAFE_COPY_S(pRsp->risk_summary);	break;
			case "risk_operation"_hash:	SAFE_COPY_C(pRsp->risk_operation);	break;
			case "remark_short"_hash:	SAFE_COPY_S(pRsp->remark_short);	break;
			case "risk_threshold_value"_hash:	SAFE_COPY_F(pRsp->risk_threshold_value);	break;
			case "risk_value"_hash:	SAFE_COPY_F(pRsp->risk_value);	break;
			default:
				assert(false);
				break;
			}
		}
	}

	return count;
}

int CO32Codec::ReqFunction_10000(REQ_XT_10000* pReq)
{
	// 还没有被设置就要求发心跳了，所过过滤一下
	if (m_lpConnection == nullptr)
		return 0;

	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction(10000);
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败！\r\n");
		return -1;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);

	///结束打包
	pPacker->EndPack();

	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	int iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

// 登录回报
int CO32Codec::OnResponse_10000(IF2UnPacker *lpUnPacker, int count, RSP_XT_10000* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_XT_10000* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_XT_10000));

		lpUnPacker->Next();
	}

	return 0;
}

int CO32Codec::ReqFunction_10001(REQ_DL_10001* pReq)
{
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction(10001);
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败！\r\n");
		return -1;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("operator_no", 'S', 16);
	pPacker->AddField("password", 'S', 16);
	pPacker->AddField("mac_address", 'S', 20);
	pPacker->AddField("ip_address", 'S', 15);
	pPacker->AddField("op_station", 'S', 256);
	pPacker->AddField("authorization_id", 'S', 64);

	///加入对应的字段值
	pPacker->AddStr(pReq->operator_no);
	pPacker->AddStr(pReq->password);
	pPacker->AddStr(pReq->mac_address);
	pPacker->AddStr(pReq->ip_address);
	pPacker->AddStr(pReq->op_station);
	pPacker->AddStr(pReq->authorization_id);
	//pPacker->AddStr("5C-26-0A-2F-82-4F");
	//pPacker->AddStr("192.168.88.123");
	//pPacker->AddStr("192.168.88.123|5C-26-0A-2F-82-4F");
	//pPacker->AddStr("hd123456");

	///结束打包
	pPacker->EndPack();

	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	int iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

// 登录回报
int CO32Codec::OnResponse_10001(IF2UnPacker *lpUnPacker, int count, RSP_DL_10001* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_DL_10001* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_DL_10001));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "user_token"_hash:	SAFE_COPY_S(pRsp->user_token);	break;
			case "version_no"_hash:	SAFE_COPY_S(pRsp->version_no);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}
//
//int CO32Codec::ReqFunction_91090(REQ_LZWT_91090* pReqs, int count)
//{
//	int iRet = 0, hSend = 0;
//	///获取版本为2类型的pack打包器
//	IF2Packer *pPacker = NewPacker(2);
//	if (!pPacker)
//	{
//		printf("取打包器失败!\r\n");
//		return -1;
//	}
//	pPacker->AddRef();
//
//	///定义解包器
//	//IF2UnPacker *pUnPacker = NULL;
//
//	IBizMessage* lpBizMessage = NewBizMessage();
//
//	lpBizMessage->AddRef();
//
//	///应答业务消息
//	IBizMessage* lpBizMessageRecv = NULL;
//	//功能号
//	lpBizMessage->SetFunction(91090);
//	//请求类型
//	lpBizMessage->SetPacketType(REQUEST_PACKET);
//
//	///其他的应答信息
//	LPRET_DATA pRetData = NULL;
//	///开始打包
//	pPacker->BeginPack();
//
//	///加入字段名
//	pPacker->AddField("user_token", 'S', 512);
//	pPacker->AddField("account_code", 'S', 32);
//	pPacker->AddField("asset_no", 'S', 16);
//	pPacker->AddField("combi_no", 'S', 16);
//	pPacker->AddField("stockholder_id", 'S', 20);
//	pPacker->AddField("report_seat", 'S', 6);
//	pPacker->AddField("market_no", 'S', 3);
//	pPacker->AddField("stock_code", 'S', 16);
//	pPacker->AddField("entrust_direction", 'S', 4);
//	pPacker->AddField("futures_direction", 'C', 1);
//	pPacker->AddField("close_direction", 'C', 1);
//	pPacker->AddField("covered_flag", 'C', 1);
//	pPacker->AddField("price_type", 'C', 1);
//	pPacker->AddField("entrust_price", 'F', 11, 4);
//	pPacker->AddField("entrust_amount", 'I', 12);
//	pPacker->AddField("limit_entrust_ratio", 'F', 9, 6);
//	pPacker->AddField("third_reff", 'S', 256);
//	pPacker->AddField("extsystem_id", 'I', 8);
//
//	for (int i = 0; i < count; ++i)
//	{
//		REQ_LZWT_91090* pReq = &pReqs[i];
//
//		///加入对应的字段值
//		pPacker->AddStr(pReq->user_token);
//		pPacker->AddStr(pReq->account_code);
//		pPacker->AddStr(pReq->asset_no);
//		pPacker->AddStr(pReq->combi_no);
//		pPacker->AddStr(pReq->stockholder_id);
//		pPacker->AddStr(pReq->report_seat);
//		pPacker->AddStr(pReq->market_no);
//		pPacker->AddStr(pReq->stock_code);
//		pPacker->AddStr(pReq->entrust_direction);
//		pPacker->AddChar(pReq->futures_direction);
//		pPacker->AddChar(pReq->close_direction);
//		pPacker->AddChar(pReq->covered_flag);
//		pPacker->AddChar(pReq->price_type);
//		pPacker->AddDouble(pReq->entrust_price);
//		pPacker->AddDouble(pReq->entrust_amount);
//		pPacker->AddDouble(pReq->limit_entrust_ratio);
//		pPacker->AddStr(pReq->third_reff);
//		pPacker->AddInt(pReq->extsystem_id);
//	}
//
//	pPacker->EndPack();
//	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
//	m_lpConnection->SendBizMsg(lpBizMessage, 1);
//	pPacker->FreeMem(pPacker->GetPackBuf());
//	pPacker->Release();
//	iRet = lpBizMessage->GetPacketId();
//	lpBizMessage->Release();
//
//	return iRet;
//}

//int CO32Codec::OnResponse_91090(IF2UnPacker *lpUnPacker, int count, RSP_LZWT_91090* pRsps)
//{
//	for (int i = 0; i < count; ++i)
//	{
//		int batch_no = lpUnPacker->GetInt("batch_no");
//
//		RSP_LZWT_91090* pRsp = &pRsps[i];
//		memset(pRsp, 0, sizeof(RSP_LZWT_91090));
//
//		pRsp->batch_no = batch_no;
//
//		lpUnPacker->Next();
//	}
//
//
//	return 0;
//}

//int CO32Codec::ReqFunction_91102(REQ_LZCD_91102* pReq)
//{
//	int iRet = 0, hSend = 0;
//	///获取版本为2类型的pack打包器
//	IF2Packer *pPacker = NewPacker(2);
//	if (!pPacker)
//	{
//		printf("取打包器失败!\r\n");
//		return -1;
//	}
//	pPacker->AddRef();
//
//	///定义解包器
//	//IF2UnPacker *pUnPacker = NULL;
//
//	IBizMessage* lpBizMessage = NewBizMessage();
//
//	lpBizMessage->AddRef();
//
//	///应答业务消息
//	IBizMessage* lpBizMessageRecv = NULL;
//	//功能号
//	lpBizMessage->SetFunction(91102);
//	//请求类型
//	lpBizMessage->SetPacketType(REQUEST_PACKET);
//
//	///其他的应答信息
//	LPRET_DATA pRetData = NULL;
//	///开始打包
//	pPacker->BeginPack();
//
//	///加入字段名
//	pPacker->AddField("user_token", 'S', 512);
//	pPacker->AddField("batch_no", 'I', 8);
//
//	///加入对应的字段值
//	pPacker->AddStr(pReq->user_token);
//	pPacker->AddInt(pReq->batch_no);
//
//
//	pPacker->EndPack();
//	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
//	m_lpConnection->SendBizMsg(lpBizMessage, 1);
//	pPacker->FreeMem(pPacker->GetPackBuf());
//	pPacker->Release();
//	iRet = lpBizMessage->GetPacketId();
//	lpBizMessage->Release();
//
//	return iRet;
//}

//int CO32Codec::OnResponse_91102(IF2UnPacker *lpUnPacker, int count, RSP_LZCD_91102* pRsps)
//{
//	for (int i = 0; i < count; ++i)
//	{
//		int entrust_no = lpUnPacker->GetInt("entrust_no");
//		const char* market_no = lpUnPacker->GetStr("market_no");
//		const char* stock_code = lpUnPacker->GetStr("stock_code");
//		char success_flag = lpUnPacker->GetChar("success_flag");
//		const char* fail_cause = lpUnPacker->GetStr("fail_cause");
//
//		RSP_LZCD_91102* pRsp = &pRsps[i];
//		memset(pRsp, 0, sizeof(RSP_LZCD_91102));
//
//		pRsp->entrust_no = entrust_no;
//		SAFE_STRNCPY(pRsp->market_no, market_no);
//		SAFE_STRNCPY(pRsp->stock_code, stock_code);
//		pRsp->success_flag = success_flag;
//		SAFE_STRNCPY(pRsp->fail_cause, fail_cause);
//
//		lpUnPacker->Next();
//	}
//
//	return 0;
//}

int CO32Codec::ReqFunction_31001(REQ_ZQCCCX_31001* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(31001);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("account_code", 'S', 32);
	pPacker->AddField("asset_no", 'S', 16);
	pPacker->AddField("combi_no", 'S', 16);
	//pPacker->AddField("market_no", 'S', 3);
	//pPacker->AddField("stock_code", 'S', 16);
	//pPacker->AddField("stockholder_id", 'S', 20);
	//pPacker->AddField("hold_seat", 'S', 6);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);
	pPacker->AddStr(pReq->account_code);
	pPacker->AddStr(pReq->asset_no);
	pPacker->AddStr(pReq->combi_no);
	//pPacker->AddStr(pReq->market_no);
	//pPacker->AddStr(pReq->stock_code);
	//pPacker->AddStr(pReq->stockholder_id);
	//pPacker->AddStr(pReq->hold_seat);


	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CO32Codec::OnResponse_31001(IF2UnPacker *lpUnPacker, int count, RSP_ZQCCCX_31001* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_ZQCCCX_31001* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_ZQCCCX_31001));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "asset_no"_hash:	SAFE_COPY_S(pRsp->asset_no);	break;
			case "combi_no"_hash:	SAFE_COPY_S(pRsp->combi_no);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "stock_name"_hash:	SAFE_COPY_S(pRsp->stock_name);	break;
			case "stockholder_id"_hash:	SAFE_COPY_S(pRsp->stockholder_id);	break;
			case "hold_seat"_hash:	SAFE_COPY_S(pRsp->hold_seat);	break;
			case "position_flag"_hash:	SAFE_COPY_C(pRsp->position_flag);	break;
			case "invest_type"_hash:	SAFE_COPY_C(pRsp->invest_type);	break;
			case "current_amount"_hash:	SAFE_COPY_F(pRsp->current_amount);	break;
				//case "today_amount"_hash:	SAFE_COPY_F(pRsp->today_amount);	break;
				//case "lastday_amount"_hash:	SAFE_COPY_F(pRsp->lastday_amount);	break;
			case "enable_amount"_hash:	SAFE_COPY_F(pRsp->enable_amount);	break;
				//case "today_enable_amount"_hash:	SAFE_COPY_F(pRsp->today_enable_amount);	break;
				//case "lastday_enable_amount"_hash:	SAFE_COPY_F(pRsp->lastday_enable_amount);	break;
			case "begin_cost"_hash:	SAFE_COPY_F(pRsp->begin_cost);	break;
			case "current_cost"_hash:	SAFE_COPY_F(pRsp->current_cost);	break;
			case "current_cost_price"_hash:	SAFE_COPY_F(pRsp->current_cost_price);	break;
			case "pre_buy_amount"_hash:	SAFE_COPY_F(pRsp->pre_buy_amount);	break;
			case "pre_sell_amount"_hash:	SAFE_COPY_F(pRsp->pre_sell_amount);	break;
			case "pre_buy_balance"_hash:	SAFE_COPY_F(pRsp->pre_buy_balance);	break;
			case "pre_sell_balance"_hash:	SAFE_COPY_F(pRsp->pre_sell_balance);	break;
			case "buy_amount"_hash:	SAFE_COPY_F(pRsp->buy_amount);	break;
			case "sell_amount"_hash:	SAFE_COPY_F(pRsp->sell_amount);	break;
			case "buy_balance"_hash:	SAFE_COPY_I(pRsp->buy_balance);	break;
			case "sell_balance"_hash:	SAFE_COPY_F(pRsp->sell_balance);	break;
			case "buy_fee"_hash:	SAFE_COPY_F(pRsp->buy_fee);	break;
			case "sell_fee"_hash:	SAFE_COPY_F(pRsp->sell_fee);	break;

			case "option_type"_hash:	SAFE_COPY_C(pRsp->option_type);	break;
			case "last_price"_hash:	SAFE_COPY_F(pRsp->last_price);	break;
			case "today_buy_amount"_hash:	SAFE_COPY_F(pRsp->today_buy_amount);	break;
			case "today_sell_amount"_hash:	SAFE_COPY_F(pRsp->today_sell_amount);	break;
			case "today_buy_balance"_hash:	SAFE_COPY_F(pRsp->today_buy_balance);	break;
			case "today_sell_balance"_hash:	SAFE_COPY_F(pRsp->today_sell_balance);	break;
			case "today_buy_fee"_hash:	SAFE_COPY_F(pRsp->today_buy_fee);	break;
			case "today_sell_fee"_hash:	SAFE_COPY_F(pRsp->today_sell_fee);	break;
			case "stock_type"_hash:	SAFE_COPY_C(pRsp->stock_type);	break;
			case "cost_price"_hash:	SAFE_COPY_F(pRsp->cost_price);	break;
			case "floating_profit"_hash:	SAFE_COPY_F(pRsp->floating_profit);	break;
			case "accumulate_profit"_hash:	SAFE_COPY_F(pRsp->accumulate_profit);	break;
			case "total_profit"_hash:	SAFE_COPY_F(pRsp->total_profit);	break;

			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}

int CO32Codec::ReqFunction_32001(REQ_ZQWTCX_32001* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(32001);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("batch_no", 'I', 8);
	pPacker->AddField("entrust_no", 'I', 8);
	pPacker->AddField("account_code", 'S', 32);
	pPacker->AddField("asset_no", 'S', 16);
	pPacker->AddField("combi_no", 'S', 16);
	//pPacker->AddField("instance_no", 'I', 10);
	//pPacker->AddField("stockholder_id", 'S', 20);
	//pPacker->AddField("market_no", 'S', 3);
	//pPacker->AddField("stock_code", 'S', 16);
	//pPacker->AddField("entrust_direction", 'S', 3);
	//pPacker->AddField("entrust_state_list", 'S', 500);
	//pPacker->AddField("extsystem_id", 'S', 20);
	//pPacker->AddField("third_reff", 'S', 255);
	pPacker->AddField("position_str", 'S', 32);
	pPacker->AddField("request_num", 'I', 8);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);
	pPacker->AddInt(pReq->batch_no);
	pPacker->AddInt(pReq->entrust_no);
	pPacker->AddStr(pReq->account_code);
	pPacker->AddStr(pReq->asset_no);
	pPacker->AddStr(pReq->combi_no);
	//pPacker->AddInt(pReq->instance_no);
	//pPacker->AddStr(pReq->stockholder_id);
	//pPacker->AddStr(pReq->market_no);
	//pPacker->AddStr(pReq->stock_code);
	//pPacker->AddStr(pReq->entrust_direction);
	//pPacker->AddStr(pReq->entrust_state_list);
	//pPacker->AddStr(pReq->extsystem_id);
	//pPacker->AddStr(pReq->third_reff);
	pPacker->AddStr(pReq->position_str);
	pPacker->AddInt(pReq->request_num);


	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CO32Codec::OnResponse_32001(IF2UnPacker *lpUnPacker, int count, RSP_ZQWTCX_32001* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_ZQWTCX_32001* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_ZQWTCX_32001));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "entrust_date"_hash:	SAFE_COPY_I(pRsp->entrust_date);	break;
			case "entrust_time"_hash:	SAFE_COPY_I(pRsp->entrust_time);	break;
			case "operator_no"_hash:	SAFE_COPY_S(pRsp->operator_no);	break;
			case "batch_no"_hash:	SAFE_COPY_I(pRsp->batch_no);	break;
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "report_no"_hash:	SAFE_COPY_S(pRsp->report_no);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "third_reff"_hash:	SAFE_COPY_S(pRsp->third_reff);	break;
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "asset_no"_hash:	SAFE_COPY_S(pRsp->asset_no);	break;
			case "combi_no"_hash:	SAFE_COPY_S(pRsp->combi_no);	break;
			case "instance_no"_hash:	SAFE_COPY_S(pRsp->instance_no);	break;
			case "stockholder_id"_hash:	SAFE_COPY_S(pRsp->stockholder_id);	break;
			case "report_seat"_hash:	SAFE_COPY_S(pRsp->report_seat);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "entrust_direction"_hash:	SAFE_COPY_S(pRsp->entrust_direction);	break;
			//case "futures_direction"_hash:	SAFE_COPY_C(pRsp->futures_direction);	break;
			//case "close_direction"_hash:	SAFE_COPY_C(pRsp->close_direction);	break;
			case "price_type"_hash:	SAFE_COPY_C(pRsp->price_type);	break;
			case "entrust_price"_hash:	SAFE_COPY_F(pRsp->entrust_price);	break;
			case "entrust_amount"_hash:	SAFE_COPY_F(pRsp->entrust_amount);	break;
			case "pre_buy_frozen_balance"_hash:	SAFE_COPY_F(pRsp->pre_buy_frozen_balance);	break;
			case "pre_sell_balance"_hash:	SAFE_COPY_F(pRsp->pre_sell_balance);	break;
			case "confirm_no"_hash:	SAFE_COPY_S(pRsp->confirm_no);	break;
			case "entrust_state"_hash:	SAFE_COPY_C(pRsp->entrust_state);	break;
			case "first_deal_time"_hash:	SAFE_COPY_I(pRsp->first_deal_time);	break;
			case "deal_amount"_hash:	SAFE_COPY_F(pRsp->deal_amount);	break;
			case "deal_balance"_hash:	SAFE_COPY_F(pRsp->deal_balance);	break;
			case "deal_price"_hash:	SAFE_COPY_F(pRsp->deal_price);	break;
			case "deal_times"_hash:	SAFE_COPY_I(pRsp->deal_times);	break;
			case "withdraw_amount"_hash:	SAFE_COPY_F(pRsp->withdraw_amount);	break;
			case "withdraw_cause"_hash:	SAFE_COPY_S(pRsp->withdraw_cause);	break;
			case "position_str"_hash:	SAFE_COPY_S(pRsp->position_str);	break;
			case "exchange_report_no"_hash:	SAFE_COPY_S(pRsp->exchange_report_no);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}

int CO32Codec::ReqFunction_33001(REQ_ZQCJCX_33001* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(33001);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("account_code", 'S', 32);
	pPacker->AddField("asset_no", 'S', 16);
	pPacker->AddField("combi_no", 'S', 16);
	//pPacker->AddField("instance_no", 'I', 10);
	pPacker->AddField("entrust_no", 'I', 8);
	//pPacker->AddField("deal_no", 'S', 64);
	//pPacker->AddField("stockholder_id", 'S', 20);
	//pPacker->AddField("market_no", 'S', 3);
	//pPacker->AddField("stock_code", 'S', 16);
	//pPacker->AddField("entrust_direction", 'S', 3);
	//pPacker->AddField("extsystem_id", 'S', 20);
	//pPacker->AddField("third_reff", 'S', 256);
	pPacker->AddField("position_str", 'S', 32);
	pPacker->AddField("request_num", 'I', 8);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);
	pPacker->AddStr(pReq->account_code);
	pPacker->AddStr(pReq->asset_no);
	pPacker->AddStr(pReq->combi_no);
	//pPacker->AddInt(pReq->instance_no);
	pPacker->AddInt(pReq->entrust_no);
	//pPacker->AddStr(pReq->deal_no);
	//pPacker->AddStr(pReq->stockholder_id);
	//pPacker->AddStr(pReq->market_no);
	//pPacker->AddStr(pReq->stock_code);
	//pPacker->AddStr(pReq->entrust_direction);
	//pPacker->AddStr(pReq->extsystem_id);
	//pPacker->AddStr(pReq->third_reff);
	pPacker->AddStr(pReq->position_str);
	pPacker->AddInt(pReq->request_num);


	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CO32Codec::OnResponse_33001(IF2UnPacker *lpUnPacker, int count, RSP_ZQCJCX_33001* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_ZQCJCX_33001* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_ZQCJCX_33001));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "deal_date"_hash:	SAFE_COPY_I(pRsp->deal_date);	break;
			case "deal_no"_hash:	SAFE_COPY_S(pRsp->deal_no);	break;
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "third_reff"_hash:	SAFE_COPY_S(pRsp->third_reff);	break;
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "asset_no"_hash:	SAFE_COPY_S(pRsp->asset_no);	break;
			case "combi_no"_hash:	SAFE_COPY_S(pRsp->combi_no);	break;
			case "instance_no"_hash:	SAFE_COPY_S(pRsp->instance_no);	break;
			case "stockholder_id"_hash:	SAFE_COPY_S(pRsp->stockholder_id);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "entrust_direction"_hash:	SAFE_COPY_S(pRsp->entrust_direction);	break;
			//case "futures_direction"_hash:	SAFE_COPY_C(pRsp->futures_direction);	break;
			case "deal_amount"_hash:	SAFE_COPY_F(pRsp->deal_amount);	break;
			case "deal_balance"_hash:	SAFE_COPY_F(pRsp->deal_balance);	break;
			case "deal_price"_hash:	SAFE_COPY_F(pRsp->deal_price);	break;
			case "total_fee"_hash:	SAFE_COPY_F(pRsp->total_fee);	break;
			case "deal_time"_hash:	SAFE_COPY_F(pRsp->deal_time);	break;
			//case "close_type"_hash:	SAFE_COPY_C(pRsp->close_type);	break;
			case "position_str"_hash:	SAFE_COPY_S(pRsp->position_str);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}

int CO32Codec::ReqFunction_34001(REQ_ZHZJCX_34001* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(34001);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("account_code", 'S', 32);
	pPacker->AddField("asset_no", 'S', 16);
	pPacker->AddField("combi_no", 'S', 16);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);
	pPacker->AddStr(pReq->account_code);
	pPacker->AddStr(pReq->asset_no);
	pPacker->AddStr(pReq->combi_no);


	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CO32Codec::OnResponse_34001(IF2UnPacker *lpUnPacker, int count, RSP_ZHZJCX_34001* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_ZHZJCX_34001* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_ZHZJCX_34001));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "asset_no"_hash:	SAFE_COPY_S(pRsp->asset_no);	break;
			case "enable_balance_t0"_hash:	SAFE_COPY_F(pRsp->enable_balance_t0);	break;
			case "enable_balance_t1"_hash:	SAFE_COPY_F(pRsp->enable_balance_t1);	break;
			case "current_balance"_hash:	SAFE_COPY_F(pRsp->current_balance);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}

int CO32Codec::ReqFunction_31003(REQ_QHCCCX_31003* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(31003);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("account_code", 'S', 32);
	pPacker->AddField("asset_no", 'S', 16);
	pPacker->AddField("combi_no", 'S', 16);
	//pPacker->AddField("market_no", 'S', 3);
	//pPacker->AddField("stock_code", 'S', 16);
	//pPacker->AddField("stockholder_id", 'S', 20);
	//pPacker->AddField("hold_seat", 'S', 6);
	//pPacker->AddField("invest_type", 'C', 1);
	//pPacker->AddField("position_flag", 'C', 1);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);
	pPacker->AddStr(pReq->account_code);
	pPacker->AddStr(pReq->asset_no);
	pPacker->AddStr(pReq->combi_no);
	//pPacker->AddStr(pReq->market_no);
	//pPacker->AddStr(pReq->stock_code);
	//pPacker->AddStr(pReq->stockholder_id);
	//pPacker->AddStr(pReq->hold_seat);
	//pPacker->AddChar(pReq->invest_type);
	//pPacker->AddChar(pReq->position_flag);


	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CO32Codec::OnResponse_31003(IF2UnPacker *lpUnPacker, int count, RSP_QHCCCX_31003* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_QHCCCX_31003* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_QHCCCX_31003));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "asset_no"_hash:	SAFE_COPY_S(pRsp->asset_no);	break;
			case "combi_no"_hash:	SAFE_COPY_S(pRsp->combi_no);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "stock_name"_hash:	SAFE_COPY_S(pRsp->stock_name);	break;
			case "stockholder_id"_hash:	SAFE_COPY_S(pRsp->stockholder_id);	break;
			case "hold_seat"_hash:	SAFE_COPY_S(pRsp->hold_seat);	break;
			case "position_flag"_hash:	SAFE_COPY_C(pRsp->position_flag);	break;
			case "invest_type"_hash:	SAFE_COPY_C(pRsp->invest_type);	break;
			case "current_amount"_hash:	SAFE_COPY_F(pRsp->current_amount);	break;
			case "today_amount"_hash:	SAFE_COPY_F(pRsp->today_amount);	break;
			case "lastday_amount"_hash:	SAFE_COPY_F(pRsp->lastday_amount);	break;
			case "enable_amount"_hash:	SAFE_COPY_F(pRsp->enable_amount);	break;
			case "today_enable_amount"_hash:	SAFE_COPY_F(pRsp->today_enable_amount);	break;
			case "lastday_enable_amount"_hash:	SAFE_COPY_F(pRsp->lastday_enable_amount);	break;
			case "begin_cost"_hash:	SAFE_COPY_F(pRsp->begin_cost);	break;
			case "current_cost"_hash:	SAFE_COPY_F(pRsp->current_cost);	break;
			case "current_cost_price"_hash:	SAFE_COPY_F(pRsp->current_cost_price);	break;
			case "pre_buy_amount"_hash:	SAFE_COPY_F(pRsp->pre_buy_amount);	break;
			case "pre_sell_amount"_hash:	SAFE_COPY_F(pRsp->pre_sell_amount);	break;
			case "pre_buy_balance"_hash:	SAFE_COPY_F(pRsp->pre_buy_balance);	break;
			case "pre_sell_balance"_hash:	SAFE_COPY_F(pRsp->pre_sell_balance);	break;
			case "buy_amount"_hash:	SAFE_COPY_F(pRsp->buy_amount);	break;
			case "sell_amount"_hash:	SAFE_COPY_F(pRsp->sell_amount);	break;
			case "buy_balance"_hash:	SAFE_COPY_I(pRsp->buy_balance);	break;
			case "sell_balance"_hash:	SAFE_COPY_F(pRsp->sell_balance);	break;
			case "buy_fee"_hash:	SAFE_COPY_F(pRsp->buy_fee);	break;
			case "sell_fee"_hash:	SAFE_COPY_F(pRsp->sell_fee);	break;

			case "option_type"_hash:	SAFE_COPY_C(pRsp->option_type);	break;
			case "last_price"_hash:	SAFE_COPY_F(pRsp->last_price);	break;
			case "today_buy_amount"_hash:	SAFE_COPY_F(pRsp->today_buy_amount);	break;
			case "today_sell_amount"_hash:	SAFE_COPY_F(pRsp->today_sell_amount);	break;
			case "today_buy_balance"_hash:	SAFE_COPY_F(pRsp->today_buy_balance);	break;
			case "today_sell_balance"_hash:	SAFE_COPY_F(pRsp->today_sell_balance);	break;
			case "today_buy_fee"_hash:	SAFE_COPY_F(pRsp->today_buy_fee);	break;
			case "today_sell_fee"_hash:	SAFE_COPY_F(pRsp->today_sell_fee);	break;
			case "stock_type"_hash:	SAFE_COPY_C(pRsp->stock_type);	break;
			case "cost_price"_hash:	SAFE_COPY_F(pRsp->cost_price);	break;
			case "floating_profit"_hash:	SAFE_COPY_F(pRsp->floating_profit);	break;
			case "accumulate_profit"_hash:	SAFE_COPY_F(pRsp->accumulate_profit);	break;
			case "total_profit"_hash:	SAFE_COPY_F(pRsp->total_profit);	break;

			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}


int CO32Codec::ReqFunction_32003(REQ_QHWTCX_32003* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(32003);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("batch_no", 'I', 8);
	pPacker->AddField("entrust_no", 'I', 8);
	pPacker->AddField("account_code", 'S', 32);
	pPacker->AddField("asset_no", 'S', 16);
	pPacker->AddField("combi_no", 'S', 16);
	//pPacker->AddField("instance_no", 'I', 10);
	//pPacker->AddField("stockholder_id", 'S', 20);
	//pPacker->AddField("market_no", 'S', 3);
	//pPacker->AddField("stock_code", 'S', 16);
	//pPacker->AddField("entrust_direction", 'S', 3);
	//pPacker->AddField("futures_direction", 'C', 1);
	//pPacker->AddField("entrust_state_list", 'S', 500);
	//pPacker->AddField("extsystem_id", 'S', 20);
	//pPacker->AddField("third_reff", 'S', 255);
	pPacker->AddField("position_str", 'S', 32);
	pPacker->AddField("request_num", 'I', 8);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);
	pPacker->AddInt(pReq->batch_no);
	pPacker->AddInt(pReq->entrust_no);
	pPacker->AddStr(pReq->account_code);
	pPacker->AddStr(pReq->asset_no);
	pPacker->AddStr(pReq->combi_no);
	//pPacker->AddInt(pReq->instance_no);
	//pPacker->AddStr(pReq->stockholder_id);
	//pPacker->AddStr(pReq->market_no);
	//pPacker->AddStr(pReq->stock_code);
	//pPacker->AddStr(pReq->entrust_direction);
	//pPacker->AddChar(pReq->futures_direction);
	//pPacker->AddStr(pReq->entrust_state_list);
	//pPacker->AddStr(pReq->extsystem_id);
	pPacker->AddStr(pReq->position_str);
	pPacker->AddInt(pReq->request_num);


	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CO32Codec::OnResponse_32003(IF2UnPacker *lpUnPacker, int count, RSP_QHWTCX_32003* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_QHWTCX_32003* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_QHWTCX_32003));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "entrust_date"_hash:	SAFE_COPY_I(pRsp->entrust_date);	break;
			case "entrust_time"_hash:	SAFE_COPY_I(pRsp->entrust_time);	break;
			case "operator_no"_hash:	SAFE_COPY_S(pRsp->operator_no);	break;
			case "batch_no"_hash:	SAFE_COPY_I(pRsp->batch_no);	break;
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "report_no"_hash:	SAFE_COPY_S(pRsp->report_no);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "third_reff"_hash:	SAFE_COPY_S(pRsp->third_reff);	break;
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "asset_no"_hash:	SAFE_COPY_S(pRsp->asset_no);	break;
			case "combi_no"_hash:	SAFE_COPY_S(pRsp->combi_no);	break;
			case "instance_no"_hash:	SAFE_COPY_S(pRsp->instance_no);	break;
			case "stockholder_id"_hash:	SAFE_COPY_S(pRsp->stockholder_id);	break;
			case "report_seat"_hash:	SAFE_COPY_S(pRsp->report_seat);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "entrust_direction"_hash:	SAFE_COPY_S(pRsp->entrust_direction);	break;
			case "futures_direction"_hash:	SAFE_COPY_C(pRsp->futures_direction);	break;
			case "close_direction"_hash:	SAFE_COPY_C(pRsp->close_direction);	break;
			case "price_type"_hash:	SAFE_COPY_C(pRsp->price_type);	break;
			case "entrust_price"_hash:	SAFE_COPY_F(pRsp->entrust_price);	break;
			case "entrust_amount"_hash:	SAFE_COPY_F(pRsp->entrust_amount);	break;
			case "pre_buy_frozen_balance"_hash:	SAFE_COPY_F(pRsp->pre_buy_frozen_balance);	break;
			case "pre_sell_balance"_hash:	SAFE_COPY_F(pRsp->pre_sell_balance);	break;
			case "confirm_no"_hash:	SAFE_COPY_S(pRsp->confirm_no);	break;
			case "entrust_state"_hash:	SAFE_COPY_C(pRsp->entrust_state);	break;
			case "first_deal_time"_hash:	SAFE_COPY_I(pRsp->first_deal_time);	break;
			case "deal_amount"_hash:	SAFE_COPY_F(pRsp->deal_amount);	break;
			case "deal_balance"_hash:	SAFE_COPY_F(pRsp->deal_balance);	break;
			case "deal_price"_hash:	SAFE_COPY_F(pRsp->deal_price);	break;
			case "deal_times"_hash:	SAFE_COPY_I(pRsp->deal_times);	break;
			case "withdraw_amount"_hash:	SAFE_COPY_F(pRsp->withdraw_amount);	break;
			case "withdraw_cause"_hash:	SAFE_COPY_S(pRsp->withdraw_cause);	break;
			case "position_str"_hash:	SAFE_COPY_S(pRsp->position_str);	break;
			case "exchange_report_no"_hash:	SAFE_COPY_S(pRsp->exchange_report_no);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}

int CO32Codec::ReqFunction_33003(REQ_QHCJCX_33003* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(33003);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("account_code", 'S', 32);
	pPacker->AddField("asset_no", 'S', 16);
	pPacker->AddField("combi_no", 'S', 16);
	//pPacker->AddField("instance_no", 'I', 10);
	pPacker->AddField("entrust_no", 'I', 8);
	//pPacker->AddField("deal_no", 'I', 8);
	//pPacker->AddField("stockholder_id", 'S', 20);
	//pPacker->AddField("market_no", 'S', 3);
	//pPacker->AddField("stock_code", 'S', 16);
	//pPacker->AddField("entrust_direction", 'S', 3);
	//pPacker->AddField("futures_direction", 'C', 1);
	//pPacker->AddField("extsystem_id", 'S', 20);
	//pPacker->AddField("third_reff", 'S', 255);
	pPacker->AddField("position_str", 'S', 32);
	pPacker->AddField("request_num", 'I', 8);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);
	pPacker->AddStr(pReq->account_code);
	pPacker->AddStr(pReq->asset_no);
	pPacker->AddStr(pReq->combi_no);
	//pPacker->AddInt(pReq->instance_no);
	pPacker->AddInt(pReq->entrust_no);
	//pPacker->AddStr(pReq->deal_no);
	//pPacker->AddStr(pReq->stockholder_id);
	//pPacker->AddStr(pReq->market_no);
	//pPacker->AddStr(pReq->stock_code);
	//pPacker->AddStr(pReq->entrust_direction);
	//pPacker->AddChar(pReq->futures_direction);
	//pPacker->AddStr(pReq->extsystem_id);
	//pPacker->AddStr(pReq->third_reff);
	pPacker->AddStr(pReq->position_str);
	pPacker->AddInt(pReq->request_num);


	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

// 下单响应
int CO32Codec::OnResponse_33003(IF2UnPacker *lpUnPacker, int count, RSP_QHCJCX_33003* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_QHCJCX_33003* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_QHCJCX_33003));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "deal_date"_hash:	SAFE_COPY_I(pRsp->deal_date);	break;
			case "deal_no"_hash:	SAFE_COPY_S(pRsp->deal_no);	break;
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "third_reff"_hash:	SAFE_COPY_S(pRsp->third_reff);	break;
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "asset_no"_hash:	SAFE_COPY_S(pRsp->asset_no);	break;
			case "combi_no"_hash:	SAFE_COPY_S(pRsp->combi_no);	break;
			case "instance_no"_hash:	SAFE_COPY_S(pRsp->instance_no);	break;
			case "stockholder_id"_hash:	SAFE_COPY_S(pRsp->stockholder_id);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "entrust_direction"_hash:	SAFE_COPY_S(pRsp->entrust_direction);	break;
			case "futures_direction"_hash:	SAFE_COPY_C(pRsp->futures_direction);	break;
			case "deal_amount"_hash:	SAFE_COPY_F(pRsp->deal_amount);	break;
			case "deal_balance"_hash:	SAFE_COPY_F(pRsp->deal_balance);	break;
			case "deal_price"_hash:	SAFE_COPY_F(pRsp->deal_price);	break;
			case "total_fee"_hash:	SAFE_COPY_F(pRsp->total_fee);	break;
			case "deal_time"_hash:	SAFE_COPY_F(pRsp->deal_time);	break;
			case "close_type"_hash:	SAFE_COPY_C(pRsp->close_type);	break;
			case "position_str"_hash:	SAFE_COPY_S(pRsp->position_str);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}

int CO32Codec::ReqFunction_34003(REQ_QHBZJZHCX_34003* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(34003);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("account_code", 'S', 32);
	pPacker->AddField("asset_no", 'S', 16);
	pPacker->AddField("combi_no", 'S', 16);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);
	pPacker->AddStr(pReq->account_code);
	pPacker->AddStr(pReq->asset_no);
	pPacker->AddStr(pReq->combi_no);


	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

// 下单响应
int CO32Codec::OnResponse_34003(IF2UnPacker *lpUnPacker, int count, RSP_QHBZJZHCX_34003* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_QHBZJZHCX_34003* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_QHBZJZHCX_34003));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "asset_no"_hash:	SAFE_COPY_S(pRsp->asset_no);	break;
			case "occupy_deposit_balance"_hash:	SAFE_COPY_F(pRsp->occupy_deposit_balance);	break;
			case "enable_deposit_balance"_hash:	SAFE_COPY_F(pRsp->enable_deposit_balance);	break;
			case "futu_deposit_balance"_hash:	SAFE_COPY_F(pRsp->futu_deposit_balance);	break;
			case "futu_temp_occupy_deposit"_hash:	SAFE_COPY_F(pRsp->futu_temp_occupy_deposit);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}


	return 0;
}

// 委托
int CO32Codec::ReqFunction_91004(REQ_QHWT_91004* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(91004);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("batch_no", 'I', 8);
	pPacker->AddField("account_code", 'S', 32);
	pPacker->AddField("asset_no", 'S', 16);
	pPacker->AddField("combi_no", 'S', 16);
	pPacker->AddField("stockholder_id", 'S', 20);
	pPacker->AddField("market_no", 'S', 3);
	pPacker->AddField("stock_code", 'S', 16);
	pPacker->AddField("entrust_direction", 'S', 4);
	pPacker->AddField("futures_direction", 'C', 1);
	pPacker->AddField("close_direction", 'C', 1);
	pPacker->AddField("price_type", 'C', 1);
	pPacker->AddField("entrust_price", 'F', 11, 4);
	pPacker->AddField("entrust_amount", 'F', 12, 0);
	pPacker->AddField("limit_deal_amount", 'F', 12, 0);
	pPacker->AddField("extsystem_id", 'I', 8);
	pPacker->AddField("third_reff", 'S', 256);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);
	pPacker->AddInt(pReq->batch_no);
	pPacker->AddStr(pReq->account_code);
	pPacker->AddStr(pReq->asset_no);
	pPacker->AddStr(pReq->combi_no);
	pPacker->AddStr(pReq->stockholder_id);
	pPacker->AddStr(pReq->market_no);
	pPacker->AddStr(pReq->stock_code);
	pPacker->AddStr(pReq->entrust_direction);
	pPacker->AddChar(pReq->futures_direction);
	pPacker->AddChar(pReq->close_direction);
	pPacker->AddChar(pReq->price_type);
	pPacker->AddDouble(pReq->entrust_price);
	pPacker->AddDouble(pReq->entrust_amount);
	pPacker->AddDouble(pReq->limit_deal_amount);
	pPacker->AddInt(pReq->extsystem_id);
	pPacker->AddStr(pReq->third_reff);

	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

// 下单响应
int CO32Codec::OnResponse_91004(IF2UnPacker *lpUnPacker, int count, RSP_QHWT_91004* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_QHWT_91004* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_QHWT_91004));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "batch_no"_hash:	SAFE_COPY_I(pRsp->batch_no);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "entrust_fail_code"_hash:	SAFE_COPY_I(pRsp->entrust_fail_code);	break;
			case "fail_cause"_hash:	SAFE_COPY_S(pRsp->fail_cause);	break;
			case "risk_serial_no"_hash:	SAFE_COPY_I(pRsp->risk_serial_no);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}


	return 0;
}

// 普通下单，按批量下单来写，但实际发现不支持
int CO32Codec::ReqFunction_91001(REQ_PTMMWT_91001* pReqs, int count)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(91001);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("batch_no", 'I', 8);
	pPacker->AddField("account_code", 'S', 32);
	pPacker->AddField("asset_no", 'S', 16);
	pPacker->AddField("combi_no", 'S', 16);
	//pPacker->AddField("instance_no", 'I', 8);
	pPacker->AddField("stockholder_id", 'S', 20);
	//pPacker->AddField("report_seat", 'S', 8);
	pPacker->AddField("market_no", 'S', 3);
	pPacker->AddField("stock_code", 'S', 16);
	pPacker->AddField("entrust_direction", 'S', 4);
	//pPacker->AddField("futures_direction", 'C', 1);
	pPacker->AddField("price_type", 'C', 1);
	pPacker->AddField("entrust_price", 'F', 11, 4);
	pPacker->AddField("entrust_amount", 'F', 12, 0);
	pPacker->AddField("extsystem_id", 'I', 8);
	pPacker->AddField("third_reff", 'S', 256);

	for (int i = 0; i < count; ++i)
	{
		REQ_PTMMWT_91001* pReq = &pReqs[i];//FIXME:不支持批量

		///加入对应的字段值
		pPacker->AddStr(pReq->user_token);
		pPacker->AddInt(pReq->batch_no);
		pPacker->AddStr(pReq->account_code);
		pPacker->AddStr(pReq->asset_no);
		pPacker->AddStr(pReq->combi_no);
		pPacker->AddStr(pReq->stockholder_id);
		pPacker->AddStr(pReq->market_no);
		pPacker->AddStr(pReq->stock_code);
		pPacker->AddStr(pReq->entrust_direction);
		//pPacker->AddChar(pReq->futures_direction);
		pPacker->AddChar(pReq->price_type);
		pPacker->AddDouble(pReq->entrust_price);
		pPacker->AddDouble(pReq->entrust_amount);
		//pPacker->AddDouble(pReq->limit_deal_amount);
		pPacker->AddInt(pReq->extsystem_id);
		pPacker->AddStr(pReq->third_reff);
	}

	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CO32Codec::OnResponse_91001(IF2UnPacker *lpUnPacker, int count, RSP_PTMMWT_91001* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_PTMMWT_91001* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_PTMMWT_91001));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "batch_no"_hash:	SAFE_COPY_I(pRsp->batch_no);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "entrust_fail_code"_hash:	SAFE_COPY_I(pRsp->entrust_fail_code);	break;
			case "fail_cause"_hash:	SAFE_COPY_S(pRsp->fail_cause);	break;
			case "risk_serial_no"_hash:	SAFE_COPY_I(pRsp->risk_serial_no);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}

int CO32Codec::ReqFunction_91105(REQ_QHWTCD_91105* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(91105);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("entrust_no", 'I', 8);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);
	pPacker->AddInt(pReq->entrust_no);

	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return pReq->entrust_no;
}

int CO32Codec::OnResponse_91105(IF2UnPacker *lpUnPacker, int count, RSP_QHWTCD_91105* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_QHWTCD_91105* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_QHWTCD_91105));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "success_flag"_hash:	SAFE_COPY_I(pRsp->success_flag);	break;
			case "fail_cause"_hash:	SAFE_COPY_S(pRsp->fail_cause);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}

int CO32Codec::ReqFunction_91101(REQ_WTCD_91101* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(91101);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("entrust_no", 'I', 8);

	///加入对应的字段值
	pPacker->AddStr(pReq->user_token);
	pPacker->AddInt(pReq->entrust_no);

	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CO32Codec::OnResponse_91101(IF2UnPacker *lpUnPacker, int count, RSP_WTCD_91101* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_WTCD_91101* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_WTCD_91101));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "success_flag"_hash:	SAFE_COPY_I(pRsp->success_flag);	break;
			case "fail_cause"_hash:	SAFE_COPY_S(pRsp->fail_cause);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}

// 订阅
int CO32Codec::SubFunction_(SUB_* pReq)
{
	int iRet = 0, hSend = 0;
	IF2UnPacker *pMCUnPacker = NULL;

	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	//lpBizMessage->SetFunction(620001);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("operator_no", 'S', 18);
	pPacker->AddField("market_no", 'S', 3);

	///加入对应的字段值
	pPacker->AddStr(pReq->operator_no);
	pPacker->AddStr(pReq->market_no);

	///结束打包
	pPacker->EndPack();
	lpBizMessage->SetKeyInfo(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	//int  iRet = lpSub->SubscribeTopic(lpSubscribeParam, 5000, &lpBack, pack);

	return iRet;
}

int CO32Codec::OnSub(IF2UnPacker * lpUnPacker)
{
	if (m_pProcessor == nullptr)
	{
		return 0;
	}
	RSP_ErrorField3 err = { 0 };
	int iRet = 0;
	int datasetCount = lpUnPacker->GetDatasetCount();
	int count = lpUnPacker->GetRowCount();

	char msgtype = lpUnPacker->GetChar("msgtype");
	printf("========%c========\r\n", msgtype);
	switch (msgtype)
	{
	case 'a':
	{
		RSP_WTXD_a* pRsps = new RSP_WTXD_a[count];
		memset(pRsps, 0, sizeof(RSP_WTXD_a)*count);
		iRet = OnResponse_a(lpUnPacker, count, pRsps);
		m_pProcessor->Do_a(pRsps, count, &err);
		delete[] pRsps;
	}
	break;
	case 'b':
	{
		RSP_WTQR_b* pRsps = new RSP_WTQR_b[count];
		memset(pRsps, 0, sizeof(RSP_WTQR_b)*count);
		iRet = OnResponse_b(lpUnPacker, count, pRsps);
		m_pProcessor->Do_b(pRsps, count, &err);
		delete[] pRsps;
	}
	break;
	case 'c':
	{
		RSP_WTFD_c* pRsps = new RSP_WTFD_c[count];
		memset(pRsps, 0, sizeof(RSP_WTFD_c)*count);
		iRet = OnResponse_c(lpUnPacker, count, pRsps);
		m_pProcessor->Do_c(pRsps, count, &err);
		delete[] pRsps;
	}
	break;
	case 'd':
	{
		RSP_WTCD_d* pRsps = new RSP_WTCD_d[count];
		memset(pRsps, 0, sizeof(RSP_WTCD_d)*count);
		iRet = OnResponse_d(lpUnPacker, count, pRsps);
		m_pProcessor->Do_d(pRsps, count, &err);
		delete[] pRsps;
	}
	break;
	case 'e':
	{
		RSP_WTCC_e* pRsps = new RSP_WTCC_e[count];
		memset(pRsps, 0, sizeof(RSP_WTCC_e)*count);
		iRet = OnResponse_e(lpUnPacker, count, pRsps);
		m_pProcessor->Do_e(pRsps, count, &err);
		delete[] pRsps;
	}
	break;
	case 'f':
	{
		RSP_WTCD_f* pRsps = new RSP_WTCD_f[count];
		memset(pRsps, 0, sizeof(RSP_WTCD_f)*count);
		iRet = OnResponse_f(lpUnPacker, count, pRsps);
		m_pProcessor->Do_f(pRsps, count, &err);
		delete[] pRsps;
	}
	break;
	case 'g':
	{
		RSP_WTCJ_g* pRsps = new RSP_WTCJ_g[count];
		memset(pRsps, 0, sizeof(RSP_WTCJ_g)*count);
		iRet = OnResponse_g(lpUnPacker, count, pRsps);
		m_pProcessor->Do_g(pRsps, count, &err);
		delete[] pRsps;
		iRet = -1;
	}
	break;
	default:
		break;
	}

	return iRet;
}

int CO32Codec::OnResponse_a(IF2UnPacker *lpUnPacker, int count, RSP_WTXD_a* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_WTXD_a* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_WTXD_a));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "msgtype"_hash:	SAFE_COPY_C(pRsp->msgtype);	break;
			case "operator_no"_hash:	SAFE_COPY_S(pRsp->operator_no);	break;
			case "business_date"_hash:	SAFE_COPY_I(pRsp->business_date);	break;
			case "business_time"_hash:	SAFE_COPY_I(pRsp->business_time);	break;
			case "batch_no"_hash:	SAFE_COPY_I(pRsp->batch_no);	break;
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "report_no"_hash:	SAFE_COPY_S(pRsp->report_no);	break;
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "combi_no"_hash:	SAFE_COPY_S(pRsp->combi_no);	break;
			case "instance_no"_hash:	SAFE_COPY_S(pRsp->instance_no);	break;
			case "stockholder_id"_hash:	SAFE_COPY_S(pRsp->stockholder_id);	break;
			case "report_seat"_hash:	SAFE_COPY_S(pRsp->report_seat);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "entrust_direction"_hash:	SAFE_COPY_S(pRsp->entrust_direction);	break;
			case "futures_direction"_hash:	SAFE_COPY_C(pRsp->futures_direction);	break;
			case "close_direction"_hash:	SAFE_COPY_C(pRsp->close_direction);	break;
			case "price_type"_hash:	SAFE_COPY_C(pRsp->price_type);	break;
			case "entrust_price"_hash:	SAFE_COPY_F(pRsp->entrust_price);	break;
			case "entrust_amount"_hash:	SAFE_COPY_F(pRsp->entrust_amount);	break;
			case "invest_type"_hash:	SAFE_COPY_C(pRsp->invest_type);	break;
			case "entrust_state"_hash:	SAFE_COPY_C(pRsp->entrust_state);	break;
			case "entrust_status"_hash:	SAFE_COPY_C(pRsp->entrust_status);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "third_reff"_hash:	SAFE_COPY_S(pRsp->third_reff);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}

int CO32Codec::OnResponse_b(IF2UnPacker *lpUnPacker, int count, RSP_WTQR_b* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_WTQR_b* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_WTQR_b));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "msgtype"_hash:	SAFE_COPY_C(pRsp->msgtype);	break;
			case "operator_no"_hash:	SAFE_COPY_S(pRsp->operator_no);	break;
			case "business_date"_hash:	SAFE_COPY_I(pRsp->business_date);	break;
			case "business_time"_hash:	SAFE_COPY_I(pRsp->business_time);	break;
			case "batch_no"_hash:	SAFE_COPY_I(pRsp->batch_no);	break;
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			//case "report_no"_hash:	SAFE_COPY_S(pRsp->report_no);	break;
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "combi_no"_hash:	SAFE_COPY_S(pRsp->combi_no);	break;
			case "instance_no"_hash:	SAFE_COPY_S(pRsp->instance_no);	break;
			case "stockholder_id"_hash:	SAFE_COPY_S(pRsp->stockholder_id);	break;
			case "report_seat"_hash:	SAFE_COPY_S(pRsp->report_seat);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "entrust_direction"_hash:	SAFE_COPY_S(pRsp->entrust_direction);	break;
			case "futures_direction"_hash:	SAFE_COPY_C(pRsp->futures_direction);	break;
			case "close_direction"_hash:	SAFE_COPY_C(pRsp->close_direction);	break;
			case "price_type"_hash:	SAFE_COPY_C(pRsp->price_type);	break;
			case "entrust_price"_hash:	SAFE_COPY_F(pRsp->entrust_price);	break;
			case "entrust_amount"_hash:	SAFE_COPY_F(pRsp->entrust_amount);	break;
			case "invest_type"_hash:	SAFE_COPY_C(pRsp->invest_type);	break;
			case "entrust_state"_hash:	SAFE_COPY_C(pRsp->entrust_state);	break;
			case "entrust_status"_hash:	SAFE_COPY_C(pRsp->entrust_status);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "third_reff"_hash:	SAFE_COPY_S(pRsp->third_reff);	break;
			case "confirm_no"_hash:	SAFE_COPY_S(pRsp->confirm_no);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}
int CO32Codec::OnResponse_c(IF2UnPacker *lpUnPacker, int count, RSP_WTFD_c* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_WTFD_c* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_WTFD_c));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "msgtype"_hash:	SAFE_COPY_C(pRsp->msgtype);	break;
			case "operator_no"_hash:	SAFE_COPY_S(pRsp->operator_no);	break;
			case "business_date"_hash:	SAFE_COPY_I(pRsp->business_date);	break;
			case "business_time"_hash:	SAFE_COPY_I(pRsp->business_time);	break;
			case "batch_no"_hash:	SAFE_COPY_I(pRsp->batch_no);	break;
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "entrust_state"_hash:	SAFE_COPY_C(pRsp->entrust_state);	break;
			case "entrust_status"_hash:	SAFE_COPY_C(pRsp->entrust_status);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "third_reff"_hash:	SAFE_COPY_S(pRsp->third_reff);	break;
			case "revoke_cause"_hash:	SAFE_COPY_S(pRsp->revoke_cause);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "entrust_direction"_hash:	SAFE_COPY_S(pRsp->entrust_direction);	break;
			case "futures_direction"_hash:	SAFE_COPY_C(pRsp->futures_direction);	break;
			case "entrust_amount"_hash:	SAFE_COPY_F(pRsp->entrust_amount);	break;
			default:
				assert(false);
				break;
			}
		}
		lpUnPacker->Next();
	}

	return 0;
}
int CO32Codec::OnResponse_d(IF2UnPacker *lpUnPacker, int count, RSP_WTCD_d* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_WTCD_d* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_WTCD_d));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "msgtype"_hash:	SAFE_COPY_C(pRsp->msgtype);	break;
			case "operator_no"_hash:	SAFE_COPY_S(pRsp->operator_no);	break;
			case "business_date"_hash:	SAFE_COPY_I(pRsp->business_date);	break;
			case "business_time"_hash:	SAFE_COPY_I(pRsp->business_time);	break;
			case "batch_no"_hash:	SAFE_COPY_I(pRsp->batch_no);	break;
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "cancel_entrust_no"_hash:	SAFE_COPY_I(pRsp->cancel_entrust_no);	break;
			case "entrust_state"_hash:	SAFE_COPY_C(pRsp->entrust_state);	break;
			case "entrust_status"_hash:	SAFE_COPY_C(pRsp->entrust_status);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "third_reff"_hash:	SAFE_COPY_S(pRsp->third_reff);	break;
			case "revoke_cause"_hash:	SAFE_COPY_S(pRsp->revoke_cause);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			default:
				assert(false);
				break;
			}
		}
		lpUnPacker->Next();
	}

	return 0;
}
int CO32Codec::OnResponse_e(IF2UnPacker *lpUnPacker, int count, RSP_WTCC_e* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_WTCC_e* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_WTCC_e));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "msgtype"_hash:	SAFE_COPY_C(pRsp->msgtype);	break;
			case "operator_no"_hash:	SAFE_COPY_S(pRsp->operator_no);	break;
			case "business_date"_hash:	SAFE_COPY_I(pRsp->business_date);	break;
			case "business_time"_hash:	SAFE_COPY_I(pRsp->business_time);	break;
			case "batch_no"_hash:	SAFE_COPY_I(pRsp->batch_no);	break;
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "cancel_entrust_no"_hash:	SAFE_COPY_I(pRsp->cancel_entrust_no);	break;
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "combi_no"_hash:	SAFE_COPY_S(pRsp->combi_no);	break;
			case "instance_no"_hash:	SAFE_COPY_S(pRsp->instance_no);	break;
			case "stockholder_id"_hash:	SAFE_COPY_S(pRsp->stockholder_id);	break;
			case "report_seat"_hash:	SAFE_COPY_S(pRsp->report_seat);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "entrust_direction"_hash:	SAFE_COPY_S(pRsp->entrust_direction);	break;
			case "futures_direction"_hash:	SAFE_COPY_C(pRsp->futures_direction);	break;
			
			case "entrust_amount"_hash:	SAFE_COPY_F(pRsp->entrust_amount);	break;
			case "cancel_amount"_hash:	SAFE_COPY_F(pRsp->cancel_amount);	break;
			case "invest_type"_hash:	SAFE_COPY_C(pRsp->invest_type);	break;
			case "entrust_state"_hash:	SAFE_COPY_C(pRsp->entrust_state);	break;
			case "entrust_status"_hash:	SAFE_COPY_C(pRsp->entrust_status);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "third_reff"_hash:	SAFE_COPY_S(pRsp->third_reff);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}
int CO32Codec::OnResponse_f(IF2UnPacker *lpUnPacker, int count, RSP_WTCD_f* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_WTCD_f* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_WTCD_f));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "msgtype"_hash:	SAFE_COPY_C(pRsp->msgtype);	break;
			case "operator_no"_hash:	SAFE_COPY_S(pRsp->operator_no);	break;
			case "batch_no"_hash:	SAFE_COPY_I(pRsp->batch_no);	break;
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "cancel_entrust_no"_hash:	SAFE_COPY_I(pRsp->cancel_entrust_no);	break;
			case "entrust_state"_hash:	SAFE_COPY_C(pRsp->entrust_state);	break;
			case "entrust_status"_hash:	SAFE_COPY_C(pRsp->entrust_status);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "third_reff"_hash:	SAFE_COPY_S(pRsp->third_reff);	break;
			case "revoke_cause"_hash:	SAFE_COPY_S(pRsp->revoke_cause);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}
int CO32Codec::OnResponse_g(IF2UnPacker *lpUnPacker, int count, RSP_WTCJ_g* pRsps)
{
	for (int i = 0; i < count; ++i)
	{
		RSP_WTCJ_g* pRsp = &pRsps[i];
		memset(pRsp, 0, sizeof(RSP_WTCJ_g));

		for (int j = 0; j < lpUnPacker->GetColCount(); ++j)
		{
			char type = lpUnPacker->GetColType(j);
			const char* name = lpUnPacker->GetColName(j);
			switch (hash_(name))
			{
			case "msgtype"_hash:	SAFE_COPY_C(pRsp->msgtype);	break;
			case "operator_no"_hash:	SAFE_COPY_S(pRsp->operator_no);	break;
			case "deal_date"_hash:	SAFE_COPY_I(pRsp->deal_date);	break;
			case "deal_time"_hash:	SAFE_COPY_I(pRsp->deal_time);	break;
			case "deal_no"_hash:	SAFE_COPY_S(pRsp->deal_no);	break;
			case "batch_no"_hash:	SAFE_COPY_I(pRsp->batch_no);	break;
			case "entrust_no"_hash:	SAFE_COPY_I(pRsp->entrust_no);	break;
			case "account_code"_hash:	SAFE_COPY_S(pRsp->account_code);	break;
			case "combi_no"_hash:	SAFE_COPY_S(pRsp->combi_no);	break;
			case "instance_no"_hash:	SAFE_COPY_S(pRsp->instance_no);	break;
			case "stockholder_id"_hash:	SAFE_COPY_S(pRsp->stockholder_id);	break;
			case "report_seat"_hash:	SAFE_COPY_S(pRsp->report_seat);	break;
			case "market_no"_hash:	SAFE_COPY_S(pRsp->market_no);	break;
			case "stock_code"_hash:	SAFE_COPY_S(pRsp->stock_code);	break;
			case "entrust_direction"_hash:	SAFE_COPY_S(pRsp->entrust_direction);	break;
			case "futures_direction"_hash:	SAFE_COPY_C(pRsp->futures_direction);	break;
			//case "close_direction"_hash:	SAFE_COPY_C(pRsp->close_direction);	break;
			//case "price_type"_hash:	SAFE_COPY_C(pRsp->price_type);	break;
			//case "entrust_price"_hash:	SAFE_COPY_F(pRsp->entrust_price);	break;
			case "entrust_amount"_hash:	SAFE_COPY_F(pRsp->entrust_amount);	break;
			//case "invest_type"_hash:	SAFE_COPY_C(pRsp->invest_type);	break;
			case "entrust_state"_hash:	SAFE_COPY_C(pRsp->entrust_state);	break;
			case "entrust_status"_hash:	SAFE_COPY_C(pRsp->entrust_status);	break;
			case "deal_amount"_hash:	SAFE_COPY_F(pRsp->deal_amount);	break;
			case "deal_price"_hash:	SAFE_COPY_F(pRsp->deal_price);	break;
			case "deal_balance"_hash:	SAFE_COPY_F(pRsp->deal_balance);	break;
			case "deal_fee"_hash:	SAFE_COPY_F(pRsp->deal_fee);	break;
			case "total_deal_amount"_hash:	SAFE_COPY_F(pRsp->total_deal_amount);	break;
			case "total_deal_balance"_hash:	SAFE_COPY_F(pRsp->total_deal_balance);	break;
			case "cancel_amount"_hash:	SAFE_COPY_I(pRsp->cancel_amount);	break;
			case "extsystem_id"_hash:	SAFE_COPY_I(pRsp->extsystem_id);	break;
			case "third_reff"_hash:	SAFE_COPY_S(pRsp->third_reff);	break;
			case "report_direction"_hash:	SAFE_COPY_S(pRsp->report_direction);	break;
			default:
				assert(false);
				break;
			}
		}

		lpUnPacker->Next();
	}

	return 0;
}