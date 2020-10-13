#include "stdafx.h"
#include "MdUserApi.h"
#include "../../include/QueueEnum.h"

#include "../../include/ApiHeader.h"
#include "../../include/ApiStruct.h"

#include "../../include/toolkit.h"
#include "../../include/ApiProcess.h"

#include "../../include/queue/MsgQueue.h"

#include "../../include/CSubscribeManager.h"
#include "../../include/synthetic/CSyntheticConfig.h"
#include "../../include/synthetic/CSyntheticManager.h"
#include "../../include/CSyntheticCalculate_DepthMarketDataNField.h"


#include <string>
#include <mutex>
#include <vector>
using namespace std;

ExchangeType TSecurityFtdcExchangeIDType_2_ExchangeType(TSecurityFtdcExchangeIDType In)
{
	switch (In[1])
	{
	case 'S':
		return ExchangeType::ExchangeType_SSE;
	case 'Z':
		return ExchangeType::ExchangeType_SZSE;
	default:
		return ExchangeType::ExchangeType_Undefined;
	}
}

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CMdUserApi* pApi = (CMdUserApi*)pApi2;
	pApi->QueryInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

CMdUserApi::CMdUserApi(void)
{
	m_pApi = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	m_pSyntheticConfig = new CSyntheticConfig();
	m_pSyntheticManager = new CSyntheticManager();
	m_pCalculateFactory = new CSyntheticCalculateFactory_XAPI();
	m_pSubscribeManager = new CSubscribeManager(m_pSyntheticConfig, m_pSyntheticManager, m_pCalculateFactory);
	
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

void CMdUserApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	switch (type)
	{
	case E_Init:
		iRet = _Init();
		break;
	case E_ReqUserLoginField:
		iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	default:
		break;
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

bool CMdUserApi::IsErrorRspInfo(const char* szSource, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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

bool CMdUserApi::IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));

	return bRet;
}

void CMdUserApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);

#ifdef _REMOTE
	// 将收到的行情通过ZeroMQ发送出去
	if (strlen(m_ServerInfo.ExtendInformation) > 0)
	{
		m_remoteQueue = new CRemoteQueue(m_ServerInfo.ExtendInformation);
		m_remoteQueue->StartThread();
	}
#endif
}

int CMdUserApi::_Init()
{
	char *pszPath = new char[m_szPath.length() + 1024];
	srand((unsigned int)time(nullptr));
	sprintf(pszPath, "%s/%s/%s/Md/%d/", m_szPath.c_str(), m_ServerInfo.BrokerID, m_UserInfo.UserID, rand());
	makedirs(pszPath);

	m_pApi = CSecurityFtdcMdApi::CreateFtdcMdApi(pszPath);
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

		//初始化连接
		m_pApi->Init();
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}

	return 0;
}

void CMdUserApi::ReqUserLogin()
{
	CSecurityFtdcReqUserLoginField* pBody = (CSecurityFtdcReqUserLoginField*)m_msgQueue_Query->new_block(sizeof(CSecurityFtdcReqUserLoginField));

	strncpy(pBody->BrokerID, m_ServerInfo.BrokerID, sizeof(TSecurityFtdcBrokerIDType));
	strncpy(pBody->UserID, m_UserInfo.UserID, sizeof(TSecurityFtdcInvestorIDType));
	strncpy(pBody->Password, m_UserInfo.Password, sizeof(TSecurityFtdcPasswordType));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
		pBody, sizeof(CSecurityFtdcReqUserLoginField), nullptr, 0, nullptr, 0);
}

int CMdUserApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	return m_pApi->ReqUserLogin((CSecurityFtdcReqUserLoginField*)ptr1, ++m_lRequestID);
}

void CMdUserApi::Disconnect()
{
	// 清理查询队列
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
		m_pApi->RegisterSpi(NULL);
		m_pApi->Release();
		m_pApi = NULL;

		// 全清理，只留最后一个
		m_msgQueue->Clear();
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		// 主动触发
		m_msgQueue->Process();
	}

	// 清理响应队列
	if (m_msgQueue)
	{
		m_msgQueue->StopThread();
		m_msgQueue->Register(nullptr);
		m_msgQueue->Clear();
		delete m_msgQueue;
		m_msgQueue = nullptr;
	}

	if (m_pSubscribeManager)
	{
		delete m_pSubscribeManager;
		m_pSubscribeManager = nullptr;
	}
	if (m_pSyntheticConfig)
	{
		delete m_pSyntheticConfig;
		m_pSyntheticConfig = nullptr;
	}
	if (m_pSyntheticManager)
	{
		delete m_pSyntheticManager;
		m_pSyntheticManager = nullptr;
	}
	if (m_pCalculateFactory)
	{
		delete m_pCalculateFactory;
		m_pCalculateFactory = nullptr;
	}
}

void CMdUserApi::Subscribe(const string& szInstrumentIDs, const string& szExchangeID)
{
	if (nullptr == m_pApi)
		return;

	set<string> ss_new = m_pSubscribeManager->String2Set(szInstrumentIDs.c_str());
	Subscribe(ss_new, szExchangeID);
}

void CMdUserApi::Subscribe(const set<string>& instrumentIDs, const string& szExchangeID)
{
	if(nullptr == m_pApi)
		return;

	if (instrumentIDs.size() == 0)
		return;

	set<string> s_dif = m_pSubscribeManager->Subscribe(instrumentIDs, szExchangeID);
	string str_dif = m_pSubscribeManager->Set2String(s_dif);

	vector<char*> vct;
	char* pBuf = m_pSubscribeManager->String2Buf(str_dif.c_str(), vct);

	if (vct.size() > 0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j < vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}
		m_pApi->SubscribeMarketData(pArray, (int)vct.size(), (char*)(szExchangeID.c_str()));

		delete[] pArray;
	}
	delete[] pBuf;
}

void CMdUserApi::Unsubscribe(const string& szInstrumentIDs, const string& szExchangeID)
{
	if (nullptr == m_pApi)
		return;

	set<string> ss_new = m_pSubscribeManager->String2Set(szInstrumentIDs.c_str());
	Unsubscribe(ss_new, szExchangeID);
}

void CMdUserApi::Unsubscribe(const set<string>& instrumentIDs, const string& szExchangeID)
{
	if (nullptr == m_pApi)
		return;

	if (instrumentIDs.size() == 0)
		return;

	set<string> s_dif = m_pSubscribeManager->Unsubscribe(instrumentIDs, szExchangeID);
	string str_dif = m_pSubscribeManager->Set2String(s_dif);

	vector<char*> vct;
	char* pBuf = m_pSubscribeManager->String2Buf(str_dif.c_str(), vct);

	if (vct.size() > 0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j < vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		m_pApi->UnSubscribeMarketData(pArray, (int)vct.size(), (char*)(szExchangeID.c_str()));

		delete[] pArray;
	}
	delete[] pBuf;
}

void CMdUserApi::OnFrontConnected()
{
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	//连接成功后自动请求登录
	ReqUserLogin();
}

void CMdUserApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
	//连接失败返回的信息是拼接而成，主要是为了统一输出
	pField->RawErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason, pField->Text);

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CMdUserApi::OnRspUserLogin(CSecurityFtdcRspUserLoginField *pRspUserLogin, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	if (!IsErrorRspInfo(pRspInfo)
		&&pRspUserLogin)
	{
		pField->TradingDay = str_to_yyyyMMdd(pRspUserLogin->TradingDay);
		pField->LoginTime = str_to_HHmmss(pRspUserLogin->LoginTime);

		m_TradingDay = pField->TradingDay;
		tm _tm1 = yyyyMMdd_to_tm(m_TradingDay);
		tm _tm2 = get_pre_trading_day(&_tm1);
		m_PreTradingDay = tm_to_yyyyMMdd(&_tm2);

		sprintf(pField->SessionID, "%d:%d", pRspUserLogin->FrontID, pRspUserLogin->SessionID);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logined, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		//有可能断线了，本处是断线重连后重新订阅
		set<string> exchanges = m_pSubscribeManager->GetExchanges();
		for (auto exchange = exchanges.begin(); exchange != exchanges.end(); ++exchange)
		{
			set<string> mapOld = m_pSubscribeManager->GetParentInstruments(exchange->data());//记下上次订阅的合约
			m_pSubscribeManager->Clear();
			Subscribe(mapOld, exchange->data());//订阅
		}
	}
	else
	{
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

void CMdUserApi::OnRspError(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo("OnRspError", pRspInfo, nRequestID, bIsLast);
}

void CMdUserApi::OnRspSubMarketData(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//在模拟平台可能这个函数不会触发，所以要自己维护一张已经订阅的合约列表
	if (!IsErrorRspInfo("OnRspSubMarketData", pRspInfo, nRequestID, bIsLast)
		&&pSpecificInstrument)
	{
	}
}

void CMdUserApi::OnRspUnSubMarketData(CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//模拟平台可能这个函数不会触发
	if (!IsErrorRspInfo("OnRspUnSubMarketData", pRspInfo, nRequestID, bIsLast)
		&&pSpecificInstrument)
	{
	}
}

//行情回调，得保证此函数尽快返回
void CMdUserApi::OnRtnDepthMarketData(CSecurityFtdcDepthMarketDataField *pDepthMarketData)
{
	DepthMarketDataNField* pField = (DepthMarketDataNField*)m_msgQueue->new_block(sizeof(DepthMarketDataNField)+sizeof(DepthField)* 10);


	strcpy(pField->InstrumentID, pDepthMarketData->InstrumentID);
	strcpy(pField->ExchangeID, pDepthMarketData->ExchangeID);
	pField->Exchange = TSecurityFtdcExchangeIDType_2_ExchangeType(pDepthMarketData->ExchangeID);

	sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pDepthMarketData->ExchangeID);

	GetExchangeTime(m_TradingDay, m_PreTradingDay,
		pDepthMarketData->TradingDay, pDepthMarketData->ActionDay, pDepthMarketData->UpdateTime,
		&pField->TradingDay, &pField->ActionDay, &pField->UpdateTime, &pField->UpdateMillisec);

	pField->UpdateMillisec = pDepthMarketData->UpdateMillisec;

	pField->LastPrice = pDepthMarketData->LastPrice;
	pField->Volume = pDepthMarketData->Volume;
	pField->Turnover = pDepthMarketData->Turnover;
	pField->OpenInterest = pDepthMarketData->OpenInterest;
	pField->AveragePrice = pDepthMarketData->AveragePrice;

	pField->OpenPrice = pDepthMarketData->OpenPrice == DBL_MAX ? 0 : pDepthMarketData->OpenPrice;
	pField->HighestPrice = pDepthMarketData->HighestPrice == DBL_MAX ? 0 : pDepthMarketData->HighestPrice;
	pField->LowestPrice = pDepthMarketData->LowestPrice == DBL_MAX ? 0 : pDepthMarketData->LowestPrice;
	pField->ClosePrice = pDepthMarketData->ClosePrice == DBL_MAX ? 0 : pDepthMarketData->ClosePrice;
	pField->SettlementPrice = pDepthMarketData->SettlementPrice;

	pField->UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
	pField->LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
	pField->PreClosePrice = pDepthMarketData->PreClosePrice;
	pField->PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
	pField->PreOpenInterest = pDepthMarketData->PreOpenInterest;

	InitBidAsk(pField);

	do
	{
		if (pDepthMarketData->BidVolume1 == 0)
			break;
		AddBid(pField, pDepthMarketData->BidPrice1, pDepthMarketData->BidVolume1, 0);

		if (pDepthMarketData->BidVolume2 == 0)
			break;
		AddBid(pField, pDepthMarketData->BidPrice2, pDepthMarketData->BidVolume2, 0);

		if (pDepthMarketData->BidVolume3 == 0)
			break;
		AddBid(pField, pDepthMarketData->BidPrice3, pDepthMarketData->BidVolume3, 0);

		if (pDepthMarketData->BidVolume4 == 0)
			break;
		AddBid(pField, pDepthMarketData->BidPrice4, pDepthMarketData->BidVolume4, 0);

		if (pDepthMarketData->BidVolume5 == 0)
			break;
		AddBid(pField, pDepthMarketData->BidPrice5, pDepthMarketData->BidVolume5, 0);
	} while (false);

	do
	{
		if (pDepthMarketData->AskVolume1 == 0)
			break;
		AddAsk(pField, pDepthMarketData->AskPrice1, pDepthMarketData->AskVolume1, 0);

		if (pDepthMarketData->AskVolume2 == 0)
			break;
		AddAsk(pField, pDepthMarketData->AskPrice2, pDepthMarketData->AskVolume2, 0);

		if (pDepthMarketData->AskVolume3 == 0)
			break;
		AddAsk(pField, pDepthMarketData->AskPrice3, pDepthMarketData->AskVolume3, 0);

		if (pDepthMarketData->AskVolume4 == 0)
			break;
		AddAsk(pField, pDepthMarketData->AskPrice4, pDepthMarketData->AskVolume4, 0);

		if (pDepthMarketData->AskVolume5 == 0)
			break;
		AddAsk(pField, pDepthMarketData->AskPrice5, pDepthMarketData->AskVolume5, 0);
	} while (false);


	// 这两个队列先头循序不要搞混，有删除功能的语句要放在后面
	// 如果放前面，会导致远程收到乱码
#ifdef _REMOTE
	if (m_remoteQueue)
	{
		m_remoteQueue->Input_Copy(ResponseType::ResponseType_OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
	}
#endif

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
}
