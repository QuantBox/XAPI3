#include "stdafx.h"
#include "TraderApi.h"

#include "../../include/QueueEnum.h"

#include "../../include/ApiHeader.h"
#include "../../include/ApiStruct.h"

#include "../../include/toolkit.h"

#include "../../include/queue/MsgQueue.h"

#include "TypeConvert.h"

#include <cstring>
#include <assert.h>

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CTraderApi* pApi = (CTraderApi*)pApi2;
	pApi->QueryInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

void CTraderApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	switch (type)
	{
	case E_Init:
		iRet = _Init();
		break;
	}

	if (m_pApi)
	{
		switch (type)
		{
		case E_ReqUserLoginField:
			iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case E_AuthRandCodeField:
			iRet = _ReqFetchAuthRandCode(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		default:
			break;
		}
	}

	if (0 == iRet)
	{
		//返回成功，填加到已发送池
		m_nSleep = 1;
	}
	else
	{
		m_msgQueue_Query->Input_Copy(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		//失败，按4的幂进行延时，但不超过1s
		m_nSleep *= 4;
		m_nSleep %= 1023;
	}
	this_thread::sleep_for(chrono::milliseconds(m_nSleep));
}

void CTraderApi::Register(void* pCallback, void* pClass)
{
	m_pClass = pClass;
	if (m_msgQueue == nullptr)
		return;

	m_msgQueue_Query->Register(Query);
	m_msgQueue->Register((fnOnResponse)pCallback);
	if (pCallback)
	{
		m_msgQueue_Query->StartThread();
		m_msgQueue->StartThread();
	}
	else
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue->StopThread();
	}
}

CTraderApi::CTraderApi(void)
{
	m_pApi = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();

	m_msgQueue_Query->Register(Query);
	m_msgQueue_Query->StartThread();
}


CTraderApi::~CTraderApi(void)
{
	Disconnect();
}

bool CTraderApi::IsErrorRspInfo(const char* szSource, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bRet)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

		pField->RawErrorID = pRspInfo->ErrorID;
		strcpy(pField->Text, pRspInfo->ErrorMsg);
		strcpy(pField->Source, szSource);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnError, m_msgQueue, m_pClass, bIsLast, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
	return bRet;
}

bool CTraderApi::IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));

	return bRet;
}

void CTraderApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_Init()
{
	char *pszPath = new char[m_szPath.length() + 1024];
	srand((unsigned int)time(nullptr));
	sprintf(pszPath, "%s/%s/%s/Td/%d/", m_szPath.c_str(), m_ServerInfo.BrokerID, m_UserInfo.UserID, rand());
	makedirs(pszPath);

	m_pApi = CSecurityFtdcTraderApi::CreateFtdcTraderApi(pszPath);
	delete[] pszPath;

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	if (m_pApi)
	{
		m_pApi->RegisterSpi(this);

		//添加地址
		size_t len = strlen(m_ServerInfo.Address) + 1;
		char* buf = new char[len];
		strncpy(buf, m_ServerInfo.Address, len);

		char* token = strtok(buf, _QUANTBOX_SEPS_);
		while (token)
		{
			if (strlen(token)>0)
			{
				m_pApi->RegisterFront(token);
			}
			token = strtok(nullptr, _QUANTBOX_SEPS_);
		}
		delete[] buf;

		if (m_ServerInfo.PublicTopicResumeType<ResumeType::ResumeType_Undefined)
			m_pApi->SubscribePublicTopic((SECURITY_TE_RESUME_TYPE)m_ServerInfo.PublicTopicResumeType);
		if (m_ServerInfo.PrivateTopicResumeType<ResumeType::ResumeType_Undefined)
			m_pApi->SubscribePrivateTopic((SECURITY_TE_RESUME_TYPE)m_ServerInfo.PrivateTopicResumeType);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		//初始化连接
		m_pApi->Init();
	}

	return 0;
}

void CTraderApi::Disconnect()
{
	if (m_msgQueue_Query)
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue_Query->Register(nullptr);
		m_msgQueue_Query->Clear();
		delete m_msgQueue_Query;
		m_msgQueue_Query = nullptr;
	}

	if (m_pApi)
	{
		m_pApi->RegisterSpi(nullptr);
		m_pApi->Release();
		m_pApi = nullptr;

		// 全清理，只留最后一个
		m_msgQueue->Clear();
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		// 主动触发
		m_msgQueue->Process();
	}

	if (m_msgQueue)
	{
		m_msgQueue->StopThread();
		m_msgQueue->Register(nullptr);
		m_msgQueue->Clear();
		delete m_msgQueue;
		m_msgQueue = nullptr;
	}

	m_lRequestID = 0;//由于线程已经停止，没有必要用原子操作了
	
	Clear();
}

void CTraderApi::Clear()
{
	for (unordered_map<string, OrderField*>::iterator it = m_id_platform_order.begin(); it != m_id_platform_order.end(); ++it)
		delete it->second;
	m_id_platform_order.clear();

	for (unordered_map<string, CSecurityFtdcOrderField*>::iterator it = m_id_api_order.begin(); it != m_id_api_order.end(); ++it)
		delete it->second;
	m_id_api_order.clear();

	//for (unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.begin(); it != m_id_platform_quote.end(); ++it)
	//	delete it->second;
	//m_id_platform_quote.clear();

	//for (unordered_map<string, CThostFtdcQuoteField*>::iterator it = m_id_api_quote.begin(); it != m_id_api_quote.end(); ++it)
	//	delete it->second;
	//m_id_api_quote.clear();

	for (unordered_map<string, PositionField*>::iterator it = m_id_platform_position.begin(); it != m_id_platform_position.end(); ++it)
		delete it->second;
	m_id_platform_position.clear();
}

void CTraderApi::OnFrontConnected()
{
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	//ReqUserLogin();
	ReqFetchAuthRandCode();
}

void CTraderApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	//连接失败返回的信息是拼接而成，主要是为了统一输出
	pField->RawErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason, pField->Text);

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CTraderApi::ReqFetchAuthRandCode()
{
	CSecurityFtdcAuthRandCodeField* pBody = (CSecurityFtdcAuthRandCodeField*)m_msgQueue_Query->new_block(sizeof(CSecurityFtdcAuthRandCodeField));

	strncpy(pBody->RandCode, "", sizeof(TSecurityFtdcAuthCodeType));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_AuthRandCodeField, m_msgQueue_Query, this, 0, 0,
		pBody, sizeof(CSecurityFtdcAuthRandCodeField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqFetchAuthRandCode(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Authorizing, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	return m_pApi->ReqFetchAuthRandCode((CSecurityFtdcAuthRandCodeField*)ptr1, ++m_lRequestID);
}

void CTraderApi::OnRspFetchAuthRandCode(CSecurityFtdcAuthRandCodeField *pAuthRandCode, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	if (!IsErrorRspInfo(pRspInfo)
		&& pAuthRandCode)
	{
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Authorized, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

		ReqUserLogin(pAuthRandCode->RandCode);
	}
	else
	{
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::ReqUserLogin(TSecurityFtdcAuthCodeType	RandCode)
{
	CSecurityFtdcReqUserLoginField* pBody = (CSecurityFtdcReqUserLoginField*)m_msgQueue_Query->new_block(sizeof(CSecurityFtdcReqUserLoginField));

	strncpy(pBody->BrokerID, m_ServerInfo.BrokerID, sizeof(TSecurityFtdcBrokerIDType));
	strncpy(pBody->UserID, m_UserInfo.UserID, sizeof(TSecurityFtdcInvestorIDType));
	strncpy(pBody->Password, m_UserInfo.Password, sizeof(TSecurityFtdcPasswordType));
	strncpy(pBody->UserProductInfo, m_ServerInfo.UserProductInfo, sizeof(TSecurityFtdcProductInfoType));
	strncpy(pBody->AuthCode, m_ServerInfo.AuthCode, sizeof(TSecurityFtdcAuthCodeType));
	strncpy(pBody->RandCode, RandCode, sizeof(TSecurityFtdcAuthCodeType));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
		pBody, sizeof(CSecurityFtdcReqUserLoginField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	return m_pApi->ReqUserLogin((CSecurityFtdcReqUserLoginField*)ptr1, ++m_lRequestID);
}

void CTraderApi::OnRspUserLogin(CSecurityFtdcRspUserLoginField *pRspUserLogin, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	if (!IsErrorRspInfo(pRspInfo)
		&&pRspUserLogin)
	{
		pField->TradingDay = str_to_yyyyMMdd(pRspUserLogin->TradingDay);
		pField->LoginTime = str_to_HHmmss(pRspUserLogin->LoginTime);

		sprintf(pField->SessionID, "%d:%d", pRspUserLogin->FrontID, pRspUserLogin->SessionID);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logined, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

		// 记下登录信息，可能会用到
		memcpy(&m_RspUserLogin,pRspUserLogin,sizeof(CSecurityFtdcRspUserLoginField));
		m_nMaxOrderRef = atol(pRspUserLogin->MaxOrderRef);
		// 自己发单时ID从1开始，不能从0开始
		m_nMaxOrderRef = m_nMaxOrderRef>1 ? m_nMaxOrderRef : 1;
		//ReqSettlementInfoConfirm();
		//ReqQryInvestor();

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		if (m_ServerInfo.PrivateTopicResumeType > ResumeType::ResumeType_Restart
			&& (m_ServerInfo.PrivateTopicResumeType<ResumeType::ResumeType_Undefined))
		{
			//ReqQryOrder();
			//ReqQryTrade();
			//ReqQryQuote();
		}
	}
	else
	{
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

char* CTraderApi::ReqOrderInsert(
	OrderField* pOrder,
	int count,
	char* pszLocalIDBuf)
{
	int OrderRef = -1;
	if (nullptr == m_pApi)
		return nullptr;

	CSecurityFtdcInputOrderField body = {0};

	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TSecurityFtdcBrokerIDType));
	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TSecurityFtdcInvestorIDType));

	body.MinVolume = 1;
	body.ForceCloseReason = SECURITY_FTDC_FCC_NotForceClose;
	body.IsAutoSuspend = 0;
	body.UserForceClose = 0;
	//body.IsSwapOrder = 0;

	//合约
	strncpy(body.InstrumentID, pOrder->InstrumentID, sizeof(TSecurityFtdcInstrumentIDType));
	strncpy(body.ExchangeID, pOrder->ExchangeID, sizeof(TSecurityFtdcExchangeIDType));
	//买卖
	body.Direction = OrderSide_2_TSecurityFtdcDirectionType(pOrder->Side);
	//开平
	body.CombOffsetFlag[0] = OpenCloseType_2_TSecurityFtdcOffsetFlagType(pOrder->OpenClose);
	//投保
	body.CombHedgeFlag[0] = HedgeFlagType_2_TSecurityFtdcHedgeFlagType(pOrder->HedgeFlag);
	//数量
	body.VolumeTotalOriginal = (int)pOrder->Qty;

	// 对于套利单，是用第一个参数的价格，还是用两个参数的价格差呢？
	//body.LimitPrice = pOrder1->Price;
	sprintf(body.LimitPrice, "%f", pOrder->Price);
	body.StopPrice = pOrder->StopPx;

	// 针对第二个进行处理，如果有第二个参数，认为是交易所套利单
	if (count>1)
	{
		body.CombOffsetFlag[1] = OpenCloseType_2_TSecurityFtdcOffsetFlagType(pOrder[1].OpenClose);
		body.CombHedgeFlag[1] = HedgeFlagType_2_TSecurityFtdcHedgeFlagType(pOrder[1].HedgeFlag);
		// 交易所的移仓换月功能，没有实测过
		//body.IsSwapOrder = (body.CombOffsetFlag[0] != body.CombOffsetFlag[1]);
	}

	//价格
	//body.OrderPriceType = OrderType_2_TSecurityFtdcOrderPriceTypeType(pOrder1->Type);

	// 市价与限价
	switch (pOrder->Type)
	{
	case OrderType_Market:
	case OrderType_Stop:
	case OrderType_MarketOnClose:
	case OrderType_TrailingStop:
		body.OrderPriceType = SECURITY_FTDC_OPT_BestPrice;
		body.TimeCondition = SECURITY_FTDC_TC_IOC;
		break;
	case OrderType_Limit:
	case OrderType_StopLimit:
	case OrderType_TrailingStopLimit:
	default:
		body.OrderPriceType = SECURITY_FTDC_OPT_LimitPrice;
		body.TimeCondition = SECURITY_FTDC_TC_GFD;
		break;
	}

	// IOC与FOK
	switch (pOrder->TimeInForce)
	{
	case TimeInForce_IOC:
		body.TimeCondition = SECURITY_FTDC_TC_IOC;
		body.VolumeCondition = SECURITY_FTDC_VC_AV;
		break;
	case TimeInForce_FOK:
		body.TimeCondition = SECURITY_FTDC_TC_IOC;
		body.VolumeCondition = SECURITY_FTDC_VC_CV;
		//body.MinVolume = body.VolumeTotalOriginal; // 这个地方必须加吗？
		break;
	default:
		body.VolumeCondition = SECURITY_FTDC_VC_AV;
		break;
	}

	// 条件单
	switch (pOrder->Type)
	{
	case OrderType_Stop:
	case OrderType_TrailingStop:
	case OrderType_StopLimit:
	case OrderType_TrailingStopLimit:
		// 条件单没有测试，先留空
		body.ContingentCondition = SECURITY_FTDC_CC_Immediately;
		break;
	default:
		body.ContingentCondition = SECURITY_FTDC_CC_Immediately;
		break;
	}

	int nRet = 0;
	{
		//可能报单太快，m_nMaxOrderRef还没有改变就提交了
		lock_guard<mutex> cl(m_csOrderRef);

		if (OrderRef < 0)
		{
			nRet = m_nMaxOrderRef;
			++m_nMaxOrderRef;
		}
		else
		{
			nRet = OrderRef;
		}
		sprintf(body.OrderRef, "%d", nRet);

		//不保存到队列，而是直接发送
		int n = m_pApi->ReqOrderInsert(&body, ++m_lRequestID);
		if (n < 0)
		{
			nRet = n;
			sprintf(m_orderInsert_Id, "%d", nRet);
		}
		else
		{
			sprintf(m_orderInsert_Id, "%d:%d:%d", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, nRet);

			OrderField* pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
			memcpy(pField, pOrder, sizeof(OrderField));
			strcpy(pField->ID, m_orderInsert_Id);
			strcpy(pField->LocalID, pField->ID);
			m_id_platform_order.insert(pair<string, OrderField*>(m_orderInsert_Id, pField));
		}
		strncpy(pszLocalIDBuf, m_orderInsert_Id, sizeof(OrderIDType));
	}

	return pszLocalIDBuf;
}

void CTraderApi::OnRspOrderInsert(CSecurityFtdcInputOrderField *pInputOrder, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OrderIDType orderId = { 0 };
	if (pInputOrder)
	{
		sprintf(orderId, "%d:%d:%s", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, pInputOrder->OrderRef);
	}
	else
	{
		IsErrorRspInfo("OnRspOrderInsert", pRspInfo, nRequestID, bIsLast);
	}

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		OrderField* pField = it->second;
		strcpy(pField->ID, orderId);
		strcpy(pField->LocalID, pField->ID);
		pField->ExecType = ExecType::ExecType_Rejected;
		pField->Status = OrderStatus::OrderStatus_Rejected;
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnErrRtnOrderInsert(CSecurityFtdcInputOrderField *pInputOrder, CSecurityFtdcRspInfoField *pRspInfo)
{
	OrderIDType orderId = { 0 };
	if (pInputOrder)
	{
		sprintf(orderId, "%d:%d:%s", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, pInputOrder->OrderRef);
	}
	else
	{
		IsErrorRspInfo("OnErrRtnOrderInsert", pRspInfo, 0, true);
	}

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
		// LTS在开始连接时也会收到此回报，无语啊
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		OrderField* pField = it->second;
		strcpy(pField->ID, orderId);
		strcpy(pField->LocalID, pField->ID);
		pField->ExecType = ExecType::ExecType_Rejected;
		pField->Status = OrderStatus::OrderStatus_Rejected;
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRtnTrade(CSecurityFtdcTradeField *pTrade)
{
	OnTrade(pTrade, 0, true);
}

char* CTraderApi::ReqOrderAction(OrderIDType* szIds, int count, char* pzsRtn)
{
	unordered_map<string, CSecurityFtdcOrderField*>::iterator it = m_id_api_order.find(szIds[0]);
	if (it == m_id_api_order.end())
	{
		sprintf(pzsRtn, "%d", -100);
	}
	else
	{
		// 找到了订单
		return ReqOrderAction(it->second, count, pzsRtn);
	}

	return pzsRtn;
}

char* CTraderApi::ReqOrderAction(CSecurityFtdcOrderField *pOrder, int count, char* pzsRtn)
{
	if (nullptr == m_pApi)
		return nullptr;

	CSecurityFtdcInputOrderActionField body = {0};

	///经纪公司代码
	strncpy(body.BrokerID, pOrder->BrokerID,sizeof(TSecurityFtdcBrokerIDType));
	///投资者代码
	strncpy(body.InvestorID, pOrder->InvestorID,sizeof(TSecurityFtdcInvestorIDType));
	///报单引用
	strncpy(body.OrderRef, pOrder->OrderRef,sizeof(TSecurityFtdcOrderRefType));
	///前置编号
	body.FrontID = pOrder->FrontID;
	///会话编号
	body.SessionID = pOrder->SessionID;
	///交易所代码
	strncpy(body.ExchangeID,pOrder->ExchangeID,sizeof(TSecurityFtdcExchangeIDType));
	///报单编号
	//strncpy(body.OrderSysID,pOrder->OrderSysID,sizeof(TSecurityFtdcOrderSysIDType));
	///操作标志
	body.ActionFlag = SECURITY_FTDC_AF_Delete;
	///合约代码
	strncpy(body.InstrumentID, pOrder->InstrumentID,sizeof(TSecurityFtdcInstrumentIDType));

	int nRet = m_pApi->ReqOrderAction(&body, ++m_lRequestID);
	if (nRet < 0)
	{
		sprintf(m_orderAction_Id, "%d", nRet);
	}
	else
	{
		memset(m_orderAction_Id, 0, sizeof(OrderIDType));
	}
	strncpy(pzsRtn, m_orderAction_Id, sizeof(OrderIDType));

	return pzsRtn;
}

void CTraderApi::OnRspOrderAction(CSecurityFtdcInputOrderActionField *pInputOrderAction, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OrderIDType orderId = { 0 };
	if (pInputOrderAction)
	{
		sprintf(orderId, "%d:%d:%s", pInputOrderAction->FrontID, pInputOrderAction->SessionID, pInputOrderAction->OrderRef);
	}
	else
	{
		IsErrorRspInfo("OnRspOrderAction", pRspInfo, nRequestID, bIsLast);
	}

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		OrderField* pField = it->second;
		pField->ExecType = ExecType::ExecType_CancelReject;
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnErrRtnOrderAction(CSecurityFtdcOrderActionField *pOrderAction, CSecurityFtdcRspInfoField *pRspInfo)
{
	OrderIDType orderId = { 0 };
	if (pOrderAction)
	{
		sprintf(orderId, "%d:%d:%s", pOrderAction->FrontID, pOrderAction->SessionID, pOrderAction->OrderRef);
	}
	else
	{
		IsErrorRspInfo("OnErrRtnOrderAction", pRspInfo, 0, true);
	}

	unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
	if (it == m_id_platform_order.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		OrderField* pField = it->second;
		pField->ExecType = ExecType::ExecType_CancelReject;
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRtnOrder(CSecurityFtdcOrderField *pOrder)
{
	OnOrder(pOrder, 0, true);
}

void CTraderApi::OnRspError(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo("OnRspError", pRspInfo, nRequestID, bIsLast);
}

void CTraderApi::OnOrder(CSecurityFtdcOrderField *pOrder, int nRequestID, bool bIsLast)
{
	if (nullptr == pOrder)
		return;

	OrderIDType orderId = { 0 };
	sprintf(orderId, "%d:%d:%s", pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef);
	OrderIDType orderSydId = { 0 };

	{
		// 保存原始订单信息，用于撤单

		unordered_map<string, CSecurityFtdcOrderField*>::iterator it = m_id_api_order.find(orderId);
		if (it == m_id_api_order.end())
		{
			// 找不到此订单，表示是新单
			CSecurityFtdcOrderField* pField = new CSecurityFtdcOrderField();
			memcpy(pField, pOrder, sizeof(CSecurityFtdcOrderField));
			m_id_api_order.insert(pair<string, CSecurityFtdcOrderField*>(orderId, pField));
		}
		else
		{
			// 找到了订单
			// 需要再复制保存最后一次的状态，还是只要第一次的用于撤单即可？记下，这样最后好比较
			CSecurityFtdcOrderField* pField = it->second;
			memcpy(pField, pOrder, sizeof(CSecurityFtdcOrderField));
		}

		// 保存SysID用于定义成交回报与订单
		sprintf(orderSydId, "%s:%s", pOrder->ExchangeID, pOrder->OrderSysID);
		m_sysId_orderId.insert(pair<string, string>(orderSydId, orderId));
	}

	{
		// 从API的订单转换成自己的结构体
		if (nRequestID == 0)
		{
			OrderField* pField = nullptr;
			unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(orderId);
			if (it == m_id_platform_order.end())
			{
				// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
				pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));

				// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
				CSecurityFtdcOrderField_2_OrderField_0(orderId, pOrder, pField);


				// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
				m_id_platform_order.insert(pair<string, OrderField*>(orderId, pField));
			}
			else
			{
				pField = it->second;
				strcpy(pField->ID, orderId);
				strcpy(pField->LocalID, pField->ID);
				pField->LeavesQty = pOrder->VolumeTotal;
				pField->Price = atof(pOrder->LimitPrice);
				pField->Status = CSecurityFtdcOrderField_2_OrderStatus(pOrder);
				pField->ExecType = CSecurityFtdcOrderField_2_ExecType(pOrder);
				strcpy(pField->OrderID, pOrder->OrderSysID);
				strncpy(pField->Text, pOrder->StatusMsg, sizeof(Char256Type));
			}

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
		else
		{
			OrderField* pField = nullptr;
			pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));

			// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
			CSecurityFtdcOrderField_2_OrderField_0(orderId, pOrder, pField);

			// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
			//m_id_platform_order.insert(pair<string, OrderField*>(orderId, pField));

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryOrder, m_msgQueue, m_pClass, bIsLast, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);

			m_msgQueue->delete_block(pField);
		}
	}
}

void CTraderApi::OnTrade(CSecurityFtdcTradeField *pTrade, int nRequestID, bool bIsLast)
{
	if (nullptr == pTrade)
		return;

	TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));
	strcpy(pField->InstrumentID, pTrade->InstrumentID);
	strcpy(pField->ExchangeID, pTrade->ExchangeID);
	pField->Side = TSecurityFtdcDirectionType_2_OrderSide(pTrade->Direction);
	pField->Qty = pTrade->Volume;
	pField->Price = atof(pTrade->Price);
	pField->OpenClose = TSecurityFtdcOffsetFlagType_2_OpenCloseType(pTrade->OffsetFlag);
	pField->HedgeFlag = TSecurityFtdcHedgeFlagType_2_HedgeFlagType(pTrade->HedgeFlag);
	pField->Commission = 0;//TODO收续费以后要计算出来
	pField->Time = str_to_HHmmss(pTrade->TradeTime);
	strcpy(pField->TradeID, pTrade->TradeID);

	if (nRequestID == 0)
	{
		OrderIDType orderSysId = { 0 };
		sprintf(orderSysId, "%s:%s", pTrade->ExchangeID, pTrade->OrderSysID);
		unordered_map<string, string>::iterator it = m_sysId_orderId.find(orderSysId);
		if (it == m_sysId_orderId.end())
		{
			// 此成交找不到对应的报单
			//assert(false);
		}
		else
		{
			// 找到对应的报单
			strcpy(pField->ID, it->second.c_str());

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);

			unordered_map<string, OrderField*>::iterator it2 = m_id_platform_order.find(it->second);
			if (it2 == m_id_platform_order.end())
			{
				// 此成交找不到对应的报单
				//assert(false);
			}
			else
			{
				// 更新订单的状态
				// 是否要通知接口
			}

			// 本地更新持仓的功能失效
			//OnTrade(pField);
		}
	}
	else
	{
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryTrade, m_msgQueue, m_pClass, bIsLast, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
	}

	// 清理内存
	m_msgQueue->delete_block(pField);
}

void CTraderApi::OnTrade(TradeField *pTrade)
{
	PositionIDType positionId = { 0 };
	sprintf(positionId, "%s:%s:%d:%d",
		pTrade->InstrumentID, pTrade->ExchangeID, TradeField_2_PositionSide(pTrade), pTrade->HedgeFlag);

	PositionField* pField = nullptr;
	unordered_map<string, PositionField*>::iterator it = m_id_platform_position.find(positionId);
	if (it == m_id_platform_position.end())
	{
		pField = (PositionField*)m_msgQueue->new_block(sizeof(PositionField));

		sprintf(pField->Symbol, "%s.%s", pTrade->InstrumentID, pTrade->ExchangeID);
		strcpy(pField->InstrumentID, pTrade->InstrumentID);
		strcpy(pField->ExchangeID, pTrade->ExchangeID);
		pField->Side = TradeField_2_PositionSide(pTrade);
		pField->HedgeFlag = TSecurityFtdcHedgeFlagType_2_HedgeFlagType(pTrade->HedgeFlag);

		m_id_platform_position.insert(pair<string, PositionField*>(positionId, pField));
	}
	else
	{
		pField = it->second;
	}

	if (pTrade->Side == OrderSide::OrderSide_Buy)
	{
		pField->Position += pTrade->Qty;
		pField->TodayPosition += pTrade->Qty;
	}
	else
	{
		pField->Position -= pTrade->Qty;
		if (pTrade->OpenClose == OpenCloseType::OpenCloseType_CloseToday)
		{
			pField->TodayPosition -= pTrade->Qty;
		}
		else
		{
			pField->HistoryPosition -= pTrade->Qty;
			// 如果昨天的被减成负数，从今天开始继续减
			if (pField->HistoryPosition<0)
			{
				pField->TodayPosition += pField->HistoryPosition;
				pField->HistoryPosition = 0;
			}
		}

		// 计算错误，直接重新查询
		//if (pField->Position < 0 || pField->TdPosition < 0 || pField->YdPosition < 0)
		//{
		//	ReqQryInvestorPosition("", "");
		//	return;
		//}
	}

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, false, 0, pField, sizeof(PositionField), nullptr, 0, nullptr, 0);
}