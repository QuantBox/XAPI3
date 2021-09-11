#include "stdafx.h"
#include "TraderApi.h"

#include "../../include/QueueEnum.h"

#include "../../include/ApiHeader.h"
#include "../../include/ApiStruct.h"

#include "../../include/ApiProcess.h"
#include "../../include/toolkit.h"
#include "../../include/IDGenerator.h"

#include "../../include/queue/MsgQueue.h"

#include "OrderMap.h"
#include "CProcessor.h"
#include "TypeConvert.h"
#include "PositionManager.h"

#include <cstring>
#include <assert.h>
#include <cfloat>



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
		// 初始化，登录交易与消息中心
		iRet = _Init();
		break;
	}

	if (m_pApi)
	{
		switch (type)
		{
			//case E_ReqUserLoginField:
			//	iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			//	break;
			//case E_QuerySymbolStatus:
			//	iRet = _QuerySymbolStatus(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			//	break;
			//case E_CancelOrder:
			//	iRet = _CancelOrder(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			//	break;
			//case E_QueryMarketSession:
			//	iRet = _QueryMarketSession(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			//	break;
		case QueryType::QueryType_ReqQryTradingAccount:
			iRet = _QryFund(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case QueryType::QueryType_ReqQryInvestorPosition:
			iRet = _QryPosition(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	// 这里改快一点安全吗？
	m_msgQueue->m_bDirectOutput = true;
	m_msgQueue_Query = new CMsgQueue();
	m_msgQueue_Query->Register(Query);
	m_msgQueue_Query->StartThread();

	m_pProcessor = new CProcessor();
	m_pOrderMap = new COrderMap<string>();
	m_pProcessor->m_pOrderMap = m_pOrderMap;

	m_pPositionManager = new PositionManager();
}


CTraderApi::~CTraderApi(void)
{
	Disconnect();
}

void CTraderApi::Connect(
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
		serverConfig.WriteDefault(szServerPath);
	}
	if (!isFileExists_ifstream(szUserPath))
	{
		userConfig.WriteDefault(szUserPath);
	}

	m_ServerItem = serverConfig.Read(szServerPath);
	m_UserItem = userConfig.Read(szUserPath);

	m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_Init()
{
	m_Status = ConnectionStatus::ConnectionStatus_Initialized;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	//创建API实例
	TAPIINT32 iResult = TAPIERROR_SUCCEED;
	TapAPIApplicationInfo stAppInfo;
	strcpy(stAppInfo.AuthCode, m_ServerItem.AuthCode.c_str());
	strcpy(stAppInfo.KeyOperationLogPath, m_szPath.c_str());

	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
	pField->RawErrorID = 0;

	m_pApi = CreateTapTradeAPI(&stAppInfo, iResult);
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
		TapAPITradeLoginAuth stLoginAuth;
		memset(&stLoginAuth, 0, sizeof(stLoginAuth));
		strcpy(stLoginAuth.UserNo, m_UserItem.UserNo);
		strcpy(stLoginAuth.Password, m_UserItem.Password);
		strcpy(stLoginAuth.AuthCode, m_UserItem.AuthCode);
		strcpy(stLoginAuth.AppID, m_UserItem.AppID);

		stLoginAuth.ISModifyPassword = m_UserItem.ISModifyPassword;
		strcpy(stLoginAuth.NewPassword, m_UserItem.NewPassword);

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

void CTraderApi::OnConnect()
{
	m_Status = ConnectionStatus::ConnectionStatus_Connected;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);
}
void CTraderApi::OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo* loginRspInfo)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	if (TAPIERROR_SUCCEED == errorCode)
	{
		m_Status = ConnectionStatus::ConnectionStatus_Logined;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
	else
	{
		pField->RawErrorID = errorCode;
		sprintf(pField->Text, "OnRspLogin Error: %d", errorCode);

		m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}
void CTraderApi::OnAPIReady()
{
	TapAPICommodity req = { 0 };
	m_uiSessionID = 0;
	auto iret = m_pApi->QryContract(&m_uiSessionID, &req);

	m_Status = ConnectionStatus::ConnectionStatus_Doing;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);
}

void CTraderApi::OnDisconnect(TAPIINT32 reasonCode)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
	pField->RawErrorID = reasonCode;

	m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CTraderApi::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITradeContractInfo* info)
{
	// TODO: 这里只处理了期货，还有期权、跨期等没有处理
	if (info->CommodityType == TAPI_COMMODITY_TYPE_FUTURES)
	{
		InstrumentIDType	InstrumentID = { 0 };
		sprintf(InstrumentID, "%s%s", info->CommodityNo, info->ContractNo1);
		m_Contracts[InstrumentID] = *info;
	}

	if (APIYNFLAG_YES == isLast)
	{
		// 收到合约列表后再通知成功
		m_Status = ConnectionStatus::ConnectionStatus_Done;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);


		{
			TapAPIUpperChannelQryReq req = {};
			m_uiSessionID = 0;
			m_pApi->QryUpperChannel(&m_uiSessionID, &req);
		}
		//{
		//	TapAPIExchangeStateInfoQryReq req = {};
		//	m_uiSessionID = 0;
		//	m_pApi->QryExchangeStateInfo(&m_uiSessionID, &req);
		//}
		{
			ReqQueryField body = { 0 };
			ReqQuery(QueryType::QueryType_ReqQryTradingAccount, &body);
		}
		{
			ReqQueryField body = { 0 };
			ReqQuery(QueryType::QueryType_ReqQryInvestorPosition, &body);
		}
	}
}

int CTraderApi::_QryFund(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	TapAPIFundReq req = { 0 };
	m_uiSessionID = 0;
	m_pApi->QryFund(&m_uiSessionID, &req);

	return 0;
}

void CTraderApi::OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData* info)
{
	if (nullptr == info)
		return;
	OnRtnFund(info);
}
void CTraderApi::OnRtnFund(const TapAPIFundData* info)
{
	AccountField* pField = (AccountField*)m_msgQueue->new_block(sizeof(AccountField));
	strcpy(pField->AccountID, info->AccountNo);
	strcpy(pField->CurrencyID, info->CurrencyNo);
	pField->PreBalance = info->PreBalance;
	pField->Balance = info->Balance;
	pField->Available = info->Available;
	//pField->CurrMargin = info->m_Margin;
	pField->FrozenCash = info->FrozenDeposit;
	pField->Commission = info->AccountFee;
	pField->FrozenCommission = info->FrozenFee;

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTradingAccount, m_msgQueue, m_pClass, 0, 0, pField, sizeof(AccountField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_QryPosition(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	TapAPIPositionQryReq req;
	m_uiSessionID = 0;
	m_pApi->QryPosition(&m_uiSessionID, &req);
	return 0;
}

PositionField* CTraderApi::OnPosition(const TapAPIPositionInfo* info)
{
	PositionField* pField = (PositionField*)m_msgQueue->new_block(sizeof(PositionField));
	strcpy(pField->AccountID, info->AccountNo);
	strcpy(pField->ExchangeID, ExchangeNo_2_ExchangeID(info->ExchangeNo));
	sprintf(pField->InstrumentID, "%s%s", info->CommodityNo, info->ContractNo);
	sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pField->ExchangeID);
	pField->HedgeFlag = TAPIHedgeFlagType_2_HedgeFlagType(info->HedgeFlag);
	pField->Side = TAPISideType_2_PositionSide(info->MatchSide);
	pField->Position = info->PositionQty;
	if (APIYNFLAG_YES == info->IsHistory)
	{
		pField->HistoryPosition = info->PositionQty;
	}
	else
	{
		pField->TodayPosition = info->PositionQty;
	}

	auto it = m_pPositionManager->OnPositionDetail(pField);
	m_msgQueue->delete_block(pField);
	return it;
}

void CTraderApi::OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIPositionInfo* info)
{
	// 这里好像返回的是持仓明细
	if (nullptr == info)
		return;
	OnPosition(info);

	if (APIYNFLAG_YES == isLast)
	{
		auto ll = m_pPositionManager->All();
		for (auto it = ll.begin(); it != ll.end(); ++it)
		{
			auto pField = *it;
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, 0, 0, pField, sizeof(PositionField), nullptr, 0, nullptr, 0);
		}
	}
}
void CTraderApi::OnRtnPosition(const TapAPIPositionInfo* info)
{
	PositionField* pField = OnPosition(info);

	// 不在这里通过明细更新，因为每次成交都会收到此通知
	//m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, 0, 0, pField, sizeof(PositionField), nullptr, 0, nullptr, 0);
}

void CTraderApi::OnRspUpperChannelInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIUpperChannelInfo* info)
{
	printf("OnRspUpperChannelInfo: UpperChannelNo:%s, UpperChannelName:%s, UpperNo:%s, UpperUserNo:%s\n",
		info->UpperChannelNo, info->UpperChannelName, info->UpperNo, info->UpperUserNo);
}
void CTraderApi::OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp* info)
{
	OrderIDType id = { 0 };
	sprintf(id, "%d", sessionID);
	// 表示柜台接受了，会提供一个委托ID
	OrderField* pOrder = (OrderField*)m_pOrderMap->findOrderXAPI(info->OrderInfo->OrderNo);
	if (pOrder == nullptr)
	{
		pOrder = (OrderField*)m_pOrderMap->findOrderXAPI(id);
		if (pOrder == nullptr)
			return;
	}

	//m_pProcessor->OnOrderReject(info, pOrder);
}
void CTraderApi::OnRtnOrder(const TapAPIOrderInfoNotice* info)
{
	OrderIDType id = { 0 };
	sprintf(id, "%d", info->SessionID);
	// 表示柜台接受了，会提供一个委托ID
	OrderField* pOrder = (OrderField*)m_pOrderMap->findOrderXAPI(info->OrderInfo->OrderNo);
	if (pOrder == nullptr)
	{
		pOrder = (OrderField*)m_pOrderMap->findOrderXAPI(id);
		if (pOrder == nullptr)
			return;
	}

	switch (info->OrderInfo->OrderState)
	{
	case TAPI_ORDER_STATE_ACCEPT:
		m_pProcessor->OnOrderAccept(info, pOrder);
		// 换成用柜台的ID
		m_pOrderMap->replace(id, info->OrderInfo->OrderNo, pOrder, pOrder->pUserData1);
		break;
	case TAPI_ORDER_STATE_QUEUED:
		m_pProcessor->OnOrderMarketAccept(info, pOrder);
		break;
	case TAPI_ORDER_STATE_PARTFINISHED:
		break;
	case TAPI_ORDER_STATE_FINISHED:
		break;
	case TAPI_ORDER_STATE_CANCELED:
		m_pProcessor->OnOrderCxled(info, pOrder);
		break;
	case TAPI_ORDER_STATE_FAIL:
		m_pProcessor->OnOrderReject(info, pOrder);
		break;
	default:
		break;
	}

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, true, 0, pOrder, sizeof(OrderField), nullptr, 0, nullptr, 0);
}

void CTraderApi::OnRtnFill(const TapAPIFillInfo* info)
{
	OrderField* pOrder = (OrderField*)m_pOrderMap->findOrderXAPI(info->OrderNo);
	if (pOrder == nullptr)
		return;
	TradeField* pTrade = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));
	m_pProcessor->OnOrderExecution(info, pOrder, pTrade);
	PositionField* pPosition = m_pPositionManager->OnTrade(pTrade);
	// 返回成交
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnTrade, m_msgQueue, m_pClass, true, 0, pTrade, sizeof(TradeField), nullptr, 0, nullptr, 0);

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, true, 0, pOrder, sizeof(OrderField), nullptr, 0, nullptr, 0);

	//if (m_pPositionManager->CheckPosition(pPosition))
	//{
	//	// 返回持仓
	//	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, false, 0, pPosition, sizeof(PositionField), nullptr, 0, nullptr, 0);
	//}
	//else
	//{
	//	// 计算出错，重新查询
	//	ReqQueryField body = { 0 };
	//	ReqQuery(QueryType::QueryType_ReqQryInvestorPosition, &body);
	//}
}

ConnectionStatus CTraderApi::GetStatus()
{
	return m_Status;
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
	if (m_pApi)
		FreeTapTradeAPI(m_pApi);
	m_pApi = nullptr;
}

void CTraderApi::Clear()
{
	if (m_pOrderMap)
	{
		m_pOrderMap->clear();
		delete m_pOrderMap;
		m_pOrderMap = nullptr;
	}
	if (m_pProcessor)
	{
		delete m_pProcessor;
		m_pProcessor = nullptr;
	}
	if (m_pPositionManager)
	{
		delete m_pPositionManager;
		m_pPositionManager = nullptr;
	}
}

void OrderField_2_TapAPINewOrder(UserItem* pUserItem, OrderField* pOrder, TapAPINewOrder* pEnter, map<string, TapAPITradeContractInfo>& contracts)
{
	auto iter = contracts.find(pOrder->InstrumentID);
	if (iter == contracts.end())
		return;

	auto contract = iter->second;

	strcpy(pEnter->AccountNo, pUserItem->UserNo);
	strcpy(pEnter->ExchangeNo, contract.ExchangeNo);
	pEnter->CommodityType = contract.CommodityType;
	strcpy(pEnter->CommodityNo, contract.CommodityNo);
	strcpy(pEnter->ContractNo, contract.ContractNo1);
	// StrikePrice
	pEnter->CallOrPutFlag = TAPI_CALLPUT_FLAG_NONE;
	// ContractNo2
	// StrikePrice2
	pEnter->CallOrPutFlag2 = TAPI_CALLPUT_FLAG_NONE;
	pEnter->OrderType = TAPI_ORDER_TYPE_LIMIT;
	pEnter->OrderSource = TAPI_ORDER_SOURCE_ESUNNY_API;
	pEnter->OrderQty = pOrder->Qty;
	switch (pOrder->TimeInForce)
	{
	case TimeInForce_IOC:
		pEnter->TimeInForce = TAPI_ORDER_TIMEINFORCE_FAK;
		break;
	case TimeInForce_FOK:
		pEnter->TimeInForce = TAPI_ORDER_TIMEINFORCE_FOK;
		pEnter->OrderMinQty = pEnter->OrderQty;
		break;
	default:
		pEnter->TimeInForce = TAPI_ORDER_TIMEINFORCE_GFD;
		break;
	}
	pEnter->IsRiskOrder = APIYNFLAG_NO;
	pEnter->OrderSide = OrderSide_2_TAPISideType(pOrder->Side);
	pEnter->PositionEffect = OpenCloseType_2_TAPIPositionEffectType(pOrder->OpenClose);
	pEnter->PositionEffect2 = TAPI_PositionEffect_NONE;
	// InquiryNo
	pEnter->HedgeFlag = TAPI_HEDGEFLAG_T;
	pEnter->OrderPrice = pOrder->Price;
	// OrderPrice2;
	// StopPrice;
	pEnter->OrderQty = pOrder->Qty;
	// OrderMinQty;
	// MinClipSize;
	// MaxClipSize;
	// RefInt;
	// RefString;
	pEnter->TacticsType = TAPI_TACTICS_TYPE_NONE;
	pEnter->TriggerCondition = TAPI_TRIGGER_CONDITION_NONE;
	pEnter->TriggerPriceType = TAPI_TRIGGER_PRICE_NONE;
	pEnter->AddOneIsValid = APIYNFLAG_NO;
	// OrderQty2;
	pEnter->HedgeFlag2 = TAPI_HEDGEFLAG_NONE;
	pEnter->MarketLevel = TAPI_MARKET_LEVEL_0;
	pEnter->FutureAutoCloseFlag = APIYNFLAG_NO;
}

char* CTraderApi::ReqOrderInsert(
	OrderField* pOrder,
	int count,
	char* pszLocalIDBuf)
{
	if (nullptr == m_pApi)
		return nullptr;

	TapAPINewOrder* pEnter = (TapAPINewOrder*)m_msgQueue->new_block(sizeof(TapAPINewOrder));
	OrderField_2_TapAPINewOrder(&m_UserItem, pOrder, pEnter, m_Contracts);

	OrderField* pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
	memcpy(pField, pOrder, sizeof(OrderField));
	pField->pUserData1 = pEnter;

	//// 循环切换席位
	//m_curr_session = (++m_curr_session) % m_SessionCount;
	//pEnter->m_MarketSessionId = m_SessionId[m_curr_session];

	TAPIINT32 iRet = TAPIERROR_SUCCEED;

	{
		lock_guard<mutex> cl(m_csOrderRef);
		//pEnter->m_ClientOrderToken = m_nMaxOrderRef;
		//++m_nMaxOrderRef;
		m_uiSessionID = 0;
		iRet = m_pApi->InsertOrder(&m_uiSessionID, pEnter);
		sprintf(pField->LocalID, "%d", m_uiSessionID);
	}

	m_pOrderMap->insert(pField->LocalID, pField, pField->pUserData1);

	if (iRet != TAPIERROR_SUCCEED)
	{
		// 标记ID，防止ID相同去重
		//sprintf(pField->ID, "%d", pEnter->m_ClientOrderToken);
		sprintf(pField->Text, "InsertOrder Error: %d", iRet);
		pField->Status = OrderStatus::OrderStatus_Rejected;

		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, true, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
	else
	{
		pField->Status = OrderStatus::OrderStatus_PendingNew;
	}

	// 由于包ID与委托编号都是数字，有可能出现冲突，所以两种的编码风格要换
	// sizeof结果不同，所以换一下
	strcpy(pszLocalIDBuf, pField->LocalID);

	return pszLocalIDBuf;
}

char* CTraderApi::ReqOrderAction(OrderIDType* szIds, int count, char* pzsRtn)
{
	if (nullptr == m_pApi)
		return nullptr;

	memset(pzsRtn, 0, sizeof(OrderIDType) * count);
	lock_guard<mutex> cl(m_csOrderRef);

	//OrderField* pField = (OrderField*)m_pOrderMap->findOrderXAPI(szIds[0]);
	//if (pField == nullptr)
	//{
	//	return pzsRtn;
	//}

	//m_pOrderMap->insertCancel(szIds[0], pField);

	TapAPIOrderCancelReq req = { 0 };
	//strcpy(req.OrderNo, pField->ID);
	strcpy(req.OrderNo, szIds[0]);
	m_uiSessionID = 0;
	auto iRet = m_pApi->CancelOrder(&m_uiSessionID, &req);
	if (iRet == TAPIERROR_SUCCEED)
	{
		memset(pzsRtn, 0, sizeof(OrderIDType));
	}
	else
	{
		sprintf(pzsRtn, "%d", iRet);
	}

	return pzsRtn;
}

void CTraderApi::ReqQuery(QueryType type, ReqQueryField* pQuery)
{
	m_msgQueue_Query->Input_Copy(type, m_msgQueue_Query, this, 0, 0,
		pQuery, sizeof(ReqQueryField), nullptr, 0, nullptr, 0);
}


void CTraderApi::OnRspQryExchangeStateInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeStateInfo* info)
{
	if (nullptr == info)
		return;
	auto it = info;

	InstrumentStatusField* pField = (InstrumentStatusField*)m_msgQueue->new_block(sizeof(InstrumentStatusField));

	// 这里需要调试一下
	//strcpy(pField->InstrumentID, it->CommodityNo);
	strcpy(pField->ExchangeID, ExchangeNo_2_ExchangeID(it->ExchangeNo));
	sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pField->ExchangeID);

	//printf("OnSymbolStatusReport: %s, %s\n", pField->Symbol, pSymbolStatus->m_EnterTime);

	pField->InstrumentStatus = TAPITradingStateType_2_InstrumentStatus(it->TradingState); //TradingState;

	int yyyy = 0;
	int MM = 0;
	int dd = 0;
	split_yyyy_MM_dd_to_2_2_2(&it->ExchangeTime[11], &yyyy, &MM, &dd);
	pField->EnterTime = yyyy * 10000 + MM * 100 + dd;


	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnInstrumentStatus, m_msgQueue, m_pClass, true, 0, pField, sizeof(InstrumentStatusField), nullptr, 0, nullptr, 0);

}
void CTraderApi::OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice* info)
{
	// TODO: 可能要实盘才能测试了
	auto it = &info->ExchangeStateInfo;

	InstrumentStatusField* pField = (InstrumentStatusField*)m_msgQueue->new_block(sizeof(InstrumentStatusField));

	// 这里需要调试一下
	//strcpy(pField->InstrumentID, it->CommodityNo);
	strcpy(pField->ExchangeID, ExchangeNo_2_ExchangeID(it->ExchangeNo));
	sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pField->ExchangeID);

	//printf("OnSymbolStatusReport: %s, %s\n", pField->Symbol, pSymbolStatus->m_EnterTime);

	pField->InstrumentStatus = TAPITradingStateType_2_InstrumentStatus(it->TradingState); //TradingState;

	int yyyy = 0;
	int MM = 0;
	int dd = 0;
	split_yyyy_MM_dd_to_2_2_2(&it->ExchangeTime[11], &yyyy, &MM, &dd);
	pField->EnterTime = yyyy * 10000 + MM * 100 + dd;


	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnInstrumentStatus, m_msgQueue, m_pClass, true, 0, pField, sizeof(InstrumentStatusField), nullptr, 0, nullptr, 0);

}