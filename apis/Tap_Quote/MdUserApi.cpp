#include "stdafx.h"
#include "MdUserApi.h"

#include "../../include/QueueEnum.h"

#include "../../include/ApiHeader.h"
#include "../../include/ApiStruct.h"

#include "../../include/ApiProcess.h"
#include "../../include/toolkit.h"

#include "../../include/queue/MsgQueue.h"

#include <cstring>
#include <assert.h>
#include <cfloat>


set<string> String2Set(const char* str, const char* seps = ";", int before = 1, const char* prefix = "")
{
	size_t len = (size_t)(strlen(str) * 1.5 + 1);
	char* buf = new char[len];
	strncpy(buf, str, len);

	set<string> ss;

	char* token = strtok(buf, seps);
	while (token)
	{
		if (strlen(token) > 0)
		{
			char temp[64] = { 0 };
			if (prefix)
			{
				if (before > 0)
				{
					sprintf(temp, "%s%s", prefix, token);
				}
				else
				{
					sprintf(temp, "%s%s", token, prefix);
				}
			}
			else
			{
				sprintf(temp, "%s", token);
			}

			ss.insert(temp);
		}
		token = strtok(nullptr, seps);
	}

	delete[] buf;
	return ss;
}

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CMdUserApi* pApi = (CMdUserApi*)pApi2;
	pApi->QueryInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

void CMdUserApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	switch (type)
	{
	case E_Init:
		// 初始化，登录交易与消息中心
		iRet = _Init();
		break;
	}

	//if (m_pApi)
	//{
	//	switch (type)
	//	{
	//	case E_ReqUserLoginField:
	//		iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	//		break;
	//	}
	//}

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

void CMdUserApi::Register(void* pCallback, void* pClass)
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

CMdUserApi::CMdUserApi(void)
{
	m_pApi = nullptr;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();
	m_msgQueue_Query->Register(Query);
	m_msgQueue_Query->StartThread();

}


CMdUserApi::~CMdUserApi(void)
{
	Disconnect();
}

void CMdUserApi::Connect(
	const char* szServerPath,
	const char* szUserPath,
	const char* szPath)
{
	m_szServerPath = szServerPath;
	m_szUserPath = szUserPath;
	m_szPath = szPath;

	CServerConfig serverConfig;
	CUserConfig	userConfig;

	if (!isFileExists_ifstream(szServerPath))
	{
		serverConfig.WriteMdDefault(szServerPath);
	}
	if (!isFileExists_ifstream(szUserPath))
	{
		userConfig.WriteDefault(szUserPath);
	}

	m_ServerItem = serverConfig.ReadMd(szServerPath);
	m_UserItem = userConfig.Read(szUserPath);

	m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CMdUserApi::_Init()
{
	m_Status = ConnectionStatus::ConnectionStatus_Initialized;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);


	//创建API实例
	TAPIINT32 iResult = TAPIERROR_SUCCEED;
	TapAPIApplicationInfo stAppInfo;
	strcpy(stAppInfo.AuthCode, m_ServerItem.AuthCode.c_str());
	strcpy(stAppInfo.KeyOperationLogPath, m_szPath.c_str());

	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	m_pApi = CreateTapQuoteAPI(&stAppInfo, iResult);
	if (m_pApi)
	{
		m_pApi->SetAPINotify(this);

		TAPIINT32 iErr = TAPIERROR_SUCCEED;
		iErr = m_pApi->SetHostAddress(m_ServerItem.IP.c_str(), m_ServerItem.Port);
		if (TAPIERROR_SUCCEED != iErr) {
			pField->RawErrorID = iErr;
			strcpy(pField->Text, "SetHostAddress failed!");

			m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

			return 0;
		}

		//登录服务器
		TapAPIQuoteLoginAuth stLoginAuth;
		memset(&stLoginAuth, 0, sizeof(stLoginAuth));
		strcpy(stLoginAuth.UserNo, m_UserItem.UserNo);
		strcpy(stLoginAuth.Password, m_UserItem.Password);
		stLoginAuth.ISModifyPassword = APIYNFLAG_NO;
		stLoginAuth.ISDDA = APIYNFLAG_NO;
		iErr = m_pApi->Login(&stLoginAuth);
		if (TAPIERROR_SUCCEED != iErr) {
			pField->RawErrorID = iErr;
			strcpy(pField->Text, "Login failed!");

			m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

			return 0;
		}
	}
	else
	{
		pField->RawErrorID = iResult;
		strcpy(pField->Text, "CreateTapQuoteAPI failed!");

		m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

ConnectionStatus CMdUserApi::GetStatus()
{
	return m_Status;
}

void CMdUserApi::Disconnect()
{

	if (m_msgQueue_Query)
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue_Query->Register(nullptr);
		m_msgQueue_Query->Clear();
		delete m_msgQueue_Query;
		m_msgQueue_Query = nullptr;
	}

	this_thread::sleep_for(chrono::milliseconds(100));

	if (m_msgQueue)
	{
		// 全清理，只留最后一个
		m_msgQueue->Clear();
		m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);
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

	Clear();
	m_pApi->Disconnect();
	FreeTapQuoteAPI(m_pApi);
	m_pApi = nullptr;
}

void CMdUserApi::Clear()
{
}


void TAP_CDECL CMdUserApi::OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo* info)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	if (TAPIERROR_SUCCEED == errorCode)
	{
		//int yyyy = 0;
		//int MM = 0;
		//int dd = 0;
		//split_yyyy_MM_dd_to_2_2_2(info->TradeDate, &yyyy, &MM, &dd);
		//pField->TradingDay = yyyy * 10000 + MM * 100 + dd;
		//m_TradingDay = pField->TradingDay;
		//m_PreTradingDay = tm_to_yyyyMMdd(&get_pre_trading_day(&yyyyMMdd_to_tm(m_TradingDay)));

		m_Status = ConnectionStatus::ConnectionStatus_Logined;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
	else
	{
		pField->RawErrorID = errorCode;
		// strcpy(pField->Text, pReason);

		m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

void TAP_CDECL CMdUserApi::OnAPIReady()
{
	TapAPICommodity commodity = { 0 };
	m_uiSessionID = 0;
	m_pApi->QryContract(&m_uiSessionID, &commodity);

	m_Status = ConnectionStatus::ConnectionStatus_Doing;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);
}

void TAP_CDECL CMdUserApi::OnDisconnect(TAPIINT32 reasonCode)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
	pField->RawErrorID = reasonCode;

	m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void TAP_CDECL CMdUserApi::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteContractInfo* info)
{
	// TODO: 这里只处理了期货，还有期权、跨期等没有处理
	if (info->Contract.Commodity.CommodityType == TAPI_COMMODITY_TYPE_FUTURES)
	{
		InstrumentIDType	InstrumentID = { 0 };
		sprintf(InstrumentID, "%s%s", info->Contract.Commodity.CommodityNo, info->Contract.ContractNo1);
		m_Contracts[InstrumentID] = info->Contract;
	}

	if (APIYNFLAG_YES == isLast)
	{
		// 收到合约列表后再通知成功
		m_Status = ConnectionStatus::ConnectionStatus_Done;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
}

void TAP_CDECL CMdUserApi::OnRspSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteWhole* info)
{
	if (TAPIERROR_SUCCEED != errorCode)
		return;

	OnRtnQuote(info);
}
void TAP_CDECL CMdUserApi::OnRspUnSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIContract* info)
{}

void TAP_CDECL CMdUserApi::OnRtnQuote(const TapAPIQuoteWhole* info)
{
	if (info == nullptr)
		return;

	DepthMarketDataNField* pField = (DepthMarketDataNField*)m_msgQueue->new_block(sizeof(DepthMarketDataNField) + sizeof(DepthField) * 10);

	// 这里最好统一生成字符串
	sprintf(pField->InstrumentID, "%s%s", info->Contract.Commodity.CommodityNo, info->Contract.ContractNo1);
	strcpy(pField->ExchangeID, info->Contract.Commodity.ExchangeNo);

	sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pField->ExchangeID);

	TAPIDTSTAMP action_day = { 0 };
	memcpy(&action_day[0], &info->DateTimeStamp[0], 4);
	memcpy(&action_day[4], &info->DateTimeStamp[5], 2);
	memcpy(&action_day[6], &info->DateTimeStamp[8], 2);

	// 交易时间
	GetExchangeTime(m_TradingDay, m_PreTradingDay,
		action_day, action_day, &info->DateTimeStamp[11],
		&pField->TradingDay, &pField->ActionDay, &pField->UpdateTime, &pField->UpdateMillisec);

	pField->LastPrice = info->QLastPrice;
	pField->Volume = info->QTotalQty;
	pField->Turnover = info->QTotalTurnover;
	pField->OpenInterest = info->QPositionQty;
	pField->AveragePrice = info->QAveragePrice;

	if (info->QOpeningPrice != DBL_MAX)
	{
		pField->OpenPrice = info->QOpeningPrice;
		pField->HighestPrice = info->QHighPrice;
		pField->LowestPrice = info->QLowPrice;
	}
	else
	{
		pField->OpenPrice = 0;
		pField->HighestPrice = 0;
		pField->LowestPrice = 0;
	}
	pField->SettlementPrice = info->QSettlePrice;

	pField->UpperLimitPrice = info->QLimitUpPrice;
	pField->LowerLimitPrice = info->QLimitDownPrice;
	pField->PreClosePrice = info->QPreClosingPrice;
	pField->PreSettlementPrice = info->QPreSettlePrice;
	pField->PreOpenInterest = info->QPrePositionQty;

	InitBidAsk(pField);

	for (int i = 0; i < 20; ++i)
	{
		if (info->QBidQty[i] == 0)
			break;
		AddBid(pField, info->QBidPrice[i], info->QBidQty[i], 0);
	}
	for (int i = 0; i < 20; ++i)
	{
		if (info->QAskQty[i] == 0)
			break;
		AddAsk(pField, info->QAskPrice[i], info->QAskQty[i], 0);
	}


	// 先通知成分，再通知合成，这样合成要触发事件时，成分的数据已经完成了
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
}

void CMdUserApi::Subscribe(const string& szInstrumentIDs, const string& szExchangeID)
{
	if (nullptr == m_pApi)
		return;

	set<string> ss_new = String2Set(szInstrumentIDs.c_str());
	Subscribe(ss_new, szExchangeID);
}

void CMdUserApi::Subscribe(const set<string>& instrumentIDs, const string& szExchangeID)
{
	if (nullptr == m_pApi)
		return;

	for (auto it = instrumentIDs.begin(); it != instrumentIDs.end(); ++it)
	{
		auto iter = m_Contracts.find(*it);
		if (iter == m_Contracts.end())
			continue;

		printf("SubscribeQuote:%s\n", it->c_str());

		m_uiSessionID = 0;
		m_pApi->SubscribeQuote(&m_uiSessionID, &iter->second);
	}
}

void CMdUserApi::Unsubscribe(const string& szInstrumentIDs, const string& szExchangeID)
{
	if (nullptr == m_pApi)
		return;

	set<string> ss_new = String2Set(szInstrumentIDs.c_str());
	Unsubscribe(ss_new, szExchangeID);
}

void CMdUserApi::Unsubscribe(const set<string>& instrumentIDs, const string& szExchangeID)
{
	if (nullptr == m_pApi)
		return;

	for (auto it = instrumentIDs.begin(); it != instrumentIDs.end(); ++it)
	{
		auto iter = m_Contracts.find(*it);
		if (iter == m_Contracts.end())
			continue;

		printf("UnSubscribeQuote:%s\n", it->c_str());

		m_uiSessionID = 0;
		m_pApi->UnSubscribeQuote(&m_uiSessionID, &iter->second);
	}
}