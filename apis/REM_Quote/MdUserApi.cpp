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

#if defined WINDOWS || _WIN32
//#include <libloaderapi.h>
#else
#include <dlfcn.h>
#include <errno.h>
#endif

bool CMdUserApi::LoadEESQuote()
{
#ifdef WIN32

	return Windows_LoadEESQuote();

#else

	return Linux_LoadEESQuote();

#endif
}

void CMdUserApi::UnloadEESQuote()
{
#ifdef WIN32

	return Windows_UnloadEESQuote();

#else

	return Linux_UnloadEESQuote();

#endif
}

bool CMdUserApi::Windows_LoadEESQuote()
{
#ifdef WIN32

	m_handle = LoadLibraryA(EES_QUOTE_DLL_NAME);
	if (!m_handle)
	{
		printf("Load (%s) Failed.\n", EES_QUOTE_DLL_NAME);
		return false;
	}

	funcCreateEESQuoteApi createFun = (funcCreateEESQuoteApi)GetProcAddress(m_handle, CREATE_EES_QUOTE_API_NAME);
	if (!createFun)
	{
		printf("GetProcAddress (%s) Failed!\n", CREATE_EES_QUOTE_API_NAME);
		return false;
	}

	m_distoryFun = (funcDestroyEESQuoteApi)GetProcAddress(m_handle, DESTROY_EES_QUOTE_API_NAME);
	if (!createFun)
	{
		printf("GetProcAddress (%s) Failed!\n", DESTROY_EES_QUOTE_API_NAME);
		return false;
	}

	m_eesApi = createFun();
	if (!m_eesApi)
	{
		printf("Create Quote Api Instance Failed!\n");
		return false;
	}

#endif

	return true;
}
void CMdUserApi::Windows_UnloadEESQuote()
{
#ifdef WIN32
	if (m_eesApi)
	{
		m_distoryFun(m_eesApi);
		m_eesApi = NULL;
		m_distoryFun = NULL;
	}

	if (m_handle)
	{
		FreeLibrary(m_handle);
		m_handle = NULL;
	}
#endif
}

bool CMdUserApi::Linux_LoadEESQuote()
{
#ifndef WIN32
	m_handle = dlopen(EES_QUOTE_DLL_NAME, RTLD_LAZY);
	if (!m_handle)
	{
		printf("加载EES行情动态库(%s)失败\n", EES_QUOTE_DLL_NAME);
		return false;
	}

	funcCreateEESQuoteApi createFun = (funcCreateEESQuoteApi)dlsym(m_handle, CREATE_EES_QUOTE_API_NAME);
	if (!createFun)
	{
		printf("获取EES创建函数地址失败!\n");
		return false;
	}

	m_distoryFun = (funcDestroyEESQuoteApi)dlsym(m_handle, DESTROY_EES_QUOTE_API_NAME);
	if (!createFun)
	{
		printf("获取EES销毁函数地址失败!\n");
		return false;
	}

	m_eesApi = createFun();
	if (!m_eesApi)
	{
		printf("创建EES行情对象失败!\n");
		return false;
	}

#endif

	return true;
}

void CMdUserApi::Linux_UnloadEESQuote()
{
#ifndef WIN32
	if (m_eesApi)
	{
		m_distoryFun(m_eesApi);
		m_eesApi = NULL;
		m_distoryFun = NULL;
	}

	if (m_handle)
	{
		dlclose(m_handle);
		m_handle = NULL;
	}
#endif
}

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

	if (m_pApi)
	{
		switch (type)
		{
		case E_ReqUserLoginField:
			iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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
	EqsTcpInfo tcpInfo;
	EqsMulticastInfo mcInfo;

	strcpy(tcpInfo.m_eqsIp, m_ServerItem.eqsIp.c_str());
	tcpInfo.m_eqsPort = m_ServerItem.eqsPort;

	strcpy(mcInfo.m_mcIp, m_ServerItem.mcIp.c_str());
	mcInfo.m_mcPort = m_ServerItem.mcPort;
	strcpy(mcInfo.m_mcLoacalIp, m_ServerItem.mcLoacalIp.c_str());
	mcInfo.m_mcLocalPort = m_ServerItem.mcLocalPort;
	strcpy(mcInfo.m_exchangeId, m_ServerItem.exchangeId.c_str());

	m_Status = ConnectionStatus::ConnectionStatus_Initialized;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	if (LoadEESQuote())
	{
		m_pApi = m_eesApi;

		if (m_ServerItem.IsMulticast)
		{
			// 直接判断，不用登录
			vector<EqsMulticastInfo>	vec_info;
			vec_info.push_back(mcInfo);

			if (m_pApi->InitMulticast(vec_info, this))
			{
				m_Status = ConnectionStatus::ConnectionStatus_Done;
				m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);
			}
			else
			{
				RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
				strcpy(pField->Text, "InitMulticast failed!");

				m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
				m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
			}
		}
		else
		{
			vector<EqsTcpInfo>	vec_info;
			vec_info.push_back(tcpInfo);
			// 好奇怪，只有直连的函数才管用
			if (m_pApi->ConnServer(tcpInfo, this))
			{
				m_Status = ConnectionStatus::ConnectionStatus_Connecting;
				m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);
			}
			else
			{
				RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
				strcpy(pField->Text, "ConnServer failed!");

				m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
				m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
			}
		}
	}
	else
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
		strcpy(pField->Text, "LoadEESQuote failed!");

		m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
	
	return 0;
}

void CMdUserApi::ReqUserLogin()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CMdUserApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_Status = ConnectionStatus::ConnectionStatus_Logining;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	EqsLoginParam temp;

	strcpy(temp.m_loginId, m_UserItem.loginId);
	strcpy(temp.m_password, m_UserItem.password);

	m_pApi->LoginToEqs(temp);
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
	m_pApi->DisConnServer();
	UnloadEESQuote();
	m_pApi = nullptr;
}

void CMdUserApi::Clear()
{

}

void CMdUserApi::OnEqsConnected()
{
	m_Status = ConnectionStatus::ConnectionStatus_Connected;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	ReqUserLogin();
}

void CMdUserApi::OnEqsDisconnected()
{
	// 实测禁用网卡并不会触发此函数

	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CMdUserApi::OnLoginResponse(bool bSuccess, const char* pReason)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	if (bSuccess)
	{
		pField->TradingDay = current_date();
		m_TradingDay = pField->TradingDay;
		m_PreTradingDay = tm_to_yyyyMMdd(&get_pre_trading_day(&yyyyMMdd_to_tm(m_TradingDay)));

		m_Status = ConnectionStatus::ConnectionStatus_Logined;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		m_Status = ConnectionStatus::ConnectionStatus_Done;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
	else
	{
		pField->RawErrorID = 1;
		strcpy(pField->Text, pReason);

		m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
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
		printf("RegisterSymbol %s\n", it->c_str());
		m_pApi->RegisterSymbol(EQS_FUTURE, it->c_str());
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
		printf("UnregisterSymbol %s\n", it->c_str());
		m_pApi->UnregisterSymbol(EQS_FUTURE, it->c_str());
	}
}

void CMdUserApi::OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData)
{
	DepthMarketDataNField* pField = (DepthMarketDataNField*)m_msgQueue->new_block(sizeof(DepthMarketDataNField) + sizeof(DepthField) * 10);

	strcpy(pField->InstrumentID, pDepthQuoteData->InstrumentID);
	strcpy(pField->ExchangeID, pDepthQuoteData->ExchangeID);
	// pField->Exchange = TThostFtdcExchangeIDType_2_ExchangeType(pDepthMarketData->ExchangeID);

	sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pDepthQuoteData->ExchangeID);


	// 交易时间
	GetExchangeTime(m_TradingDay, m_PreTradingDay,
		pDepthQuoteData->TradingDay, pDepthQuoteData->TradingDay, pDepthQuoteData->UpdateTime,
		&pField->TradingDay, &pField->ActionDay, &pField->UpdateTime, &pField->UpdateMillisec);

	pField->UpdateMillisec = pDepthQuoteData->UpdateMillisec;

	pField->LastPrice = pDepthQuoteData->LastPrice == DBL_MAX ? 0 : pDepthQuoteData->LastPrice;
	pField->Volume = pDepthQuoteData->Volume;
	pField->Turnover = pDepthQuoteData->Turnover;
	pField->OpenInterest = pDepthQuoteData->OpenInterest;
	pField->AveragePrice = pDepthQuoteData->AveragePrice;

	if (pDepthQuoteData->OpenPrice != DBL_MAX)
	{
		pField->OpenPrice = pDepthQuoteData->OpenPrice;
		pField->HighestPrice = pDepthQuoteData->HighestPrice;
		pField->LowestPrice = pDepthQuoteData->LowestPrice;
	}
	else
	{
		pField->OpenPrice = 0;
		pField->HighestPrice = 0;
		pField->LowestPrice = 0;
	}
	pField->SettlementPrice = pDepthQuoteData->SettlementPrice != DBL_MAX ? pDepthQuoteData->SettlementPrice : 0;

	pField->UpperLimitPrice = pDepthQuoteData->UpperLimitPrice;
	pField->LowerLimitPrice = pDepthQuoteData->LowerLimitPrice;
	pField->PreClosePrice = pDepthQuoteData->PreClosePrice;
	pField->PreSettlementPrice = pDepthQuoteData->PreSettlementPrice;
	pField->PreOpenInterest = pDepthQuoteData->PreOpenInterest;

	InitBidAsk(pField);

	do
	{
		if (pDepthQuoteData->BidVolume1 == 0)
			break;
		AddBid(pField, pDepthQuoteData->BidPrice1, pDepthQuoteData->BidVolume1, 0);

		if (pDepthQuoteData->BidVolume2 == 0)
			break;
		AddBid(pField, pDepthQuoteData->BidPrice2, pDepthQuoteData->BidVolume2, 0);

		if (pDepthQuoteData->BidVolume3 == 0)
			break;
		AddBid(pField, pDepthQuoteData->BidPrice3, pDepthQuoteData->BidVolume3, 0);

		if (pDepthQuoteData->BidVolume4 == 0)
			break;
		AddBid(pField, pDepthQuoteData->BidPrice4, pDepthQuoteData->BidVolume4, 0);

		if (pDepthQuoteData->BidVolume5 == 0)
			break;
		AddBid(pField, pDepthQuoteData->BidPrice5, pDepthQuoteData->BidVolume5, 0);
	} while (false);

	do
	{
		if (pDepthQuoteData->AskVolume1 == 0)
			break;
		AddAsk(pField, pDepthQuoteData->AskPrice1, pDepthQuoteData->AskVolume1, 0);

		if (pDepthQuoteData->AskVolume2 == 0)
			break;
		AddAsk(pField, pDepthQuoteData->AskPrice2, pDepthQuoteData->AskVolume2, 0);

		if (pDepthQuoteData->AskVolume3 == 0)
			break;
		AddAsk(pField, pDepthQuoteData->AskPrice3, pDepthQuoteData->AskVolume3, 0);

		if (pDepthQuoteData->AskVolume4 == 0)
			break;
		AddAsk(pField, pDepthQuoteData->AskPrice4, pDepthQuoteData->AskVolume4, 0);

		if (pDepthQuoteData->AskVolume5 == 0)
			break;
		AddAsk(pField, pDepthQuoteData->AskPrice5, pDepthQuoteData->AskVolume5, 0);
	} while (false);

	// 先通知成分，再通知合成，这样合成要触发事件时，成分的数据已经完成了
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
}
