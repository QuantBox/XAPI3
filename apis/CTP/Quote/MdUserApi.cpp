#include "stdafx.h"
#include "MdUserApi.h"
#include "../../include/QueueEnum.h"

#include "../../include/ApiHeader.h"
#include "../../include/ApiStruct.h"

#include "../../include/toolkit.h"
#include "../../include/ApiProcess.h"
#include "../TypeConvert.h"

#include "../../include/queue/MsgQueue.h"


#include "../../include/CSubscribeManager.h"
#include "../../include/synthetic/CSyntheticConfig.h"
#include "../../include/synthetic/CSyntheticManager.h"
#include "../../include/CSyntheticCalculate_DepthMarketDataNField.h"

#include <string.h>
#include <cfloat>
#include <mutex>
#include <vector>

using namespace std;

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CMdUserApi* pApi = (CMdUserApi*)pApi2;
	pApi->QueryInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

CMdUserApi::CMdUserApi(void)
{
	//_CrtSetBreakAlloc(251);

	m_pApi = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	m_pSyntheticConfig = new CSyntheticConfig();
	m_pSyntheticManager = new CSyntheticManager();
	m_pCalculateFactory = new CSyntheticCalculateFactory_XAPI();
	m_pSubscribeManager = new CSubscribeManager(m_pSyntheticConfig, m_pSyntheticManager, m_pCalculateFactory);
	m_pQuoteSubscribeManager = new CSubscribeManager(m_pSyntheticConfig, m_pSyntheticManager, m_pCalculateFactory);

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();

	m_msgQueue_Query->Register(Query);
	m_msgQueue_Query->StartThread();

	//m_msgQueue->m_bDirectOutput = true;


	//m_delete = false;
}

CMdUserApi::~CMdUserApi(void)
{
	_Disconnect(false);
}

void CMdUserApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	switch (type)
	{
	case E_Init:
		iRet = _Init();
		break;
	case E_Disconnect:
		_Disconnect(true);
		// 不再循环
		return;

	}

	if (m_pApi)
	{
		switch (type)
		{
		case E_ReqUserLoginField:
			iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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

ConfigInfoField* CMdUserApi::Config(ConfigInfoField* pConfigInfo)
{
	return nullptr;
}

bool CMdUserApi::IsErrorRspInfo(const char* szSource, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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

bool CMdUserApi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));

	return bRet;
}

void CMdUserApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo,
	int count)
{
#if _WIN32
	char szExePath[MAX_PATH] = { 0 };
	GetExePath(szExePath);
	char szDllPath[MAX_PATH] = { 0 };
	GetDllPathByFunctionName("XRequest", szDllPath);

	char szExistingFileName[MAX_PATH] = { 0 };
	char szNewFileName[MAX_PATH] = { 0 };
	GetNewPathInSameDirectory(szDllPath, "Synthetic", "json", szExistingFileName);
	GetNewPathInSameDirectory(szExePath, "Synthetic", "json", szNewFileName);

	// TODO:先测试用
	auto sc = m_pSyntheticConfig->Read(szExistingFileName);
	if (sc.size() == 0)
	{
		m_pSyntheticConfig->Write(szExistingFileName);
	}
#endif

	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CMdUserApi::_Init()
{
	char *pszPath = new char[m_szPath.length() + 1024];
	srand((unsigned int)time(NULL));
	sprintf(pszPath, "%s/%s/%s/Md/%d/", m_szPath.c_str(), m_ServerInfo.BrokerID, m_UserInfo.UserID, rand());
	makedirs(pszPath);

	// 本来想使用chdir的方法解决Kingstar的证书问题，测试多次发现还是读取的exe目录下
	// 打算使用文件复制的方法来实现，
	// 1.先检查证书是否存在，存在就跳过
	// 2.复制，用完要删
#ifdef KS_COPYFILE

	char szExePath[MAX_PATH] = { 0 };
	GetExePath(szExePath);
	char szDllPath[MAX_PATH] = { 0 };
	GetDllPathByFunctionName("XRequest", szDllPath);

	char szExistingFileName[MAX_PATH] = { 0 };
	char szNewFileName[MAX_PATH] = { 0 };
	GetNewPathInSameDirectory(szDllPath, KS_LKC_FILENAME, KS_LKC_EXT, szExistingFileName);
	GetNewPathInSameDirectory(szExePath, KS_LKC_FILENAME, KS_LKC_EXT, szNewFileName);

	bool bRet = CopyFileA(szExistingFileName, szNewFileName, false);

	if (!bRet)
	{
		char szBuf[256] = { 0 };
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&lpMsgBuf,
			0, NULL);

		LogField* pField = (LogField*)m_msgQueue->new_block(sizeof(LogField));

		sprintf(pField->Message, "CopyFile:%s", lpMsgBuf);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnLog, m_msgQueue, m_pClass, true, 0, pField, sizeof(LogField), nullptr, 0, nullptr, 0);
	}
#endif // KS_COPYFILE	

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

#ifdef CreateFtdcMdApi_argc_3
	m_pApi = CThostFtdcMdApi::CreateFtdcMdApi(pszPath, m_ServerInfo.IsUsingUdp, m_ServerInfo.IsMulticast);
#else
	m_pApi = CThostFtdcMdApi::CreateFtdcMdApi(pszPath, m_ServerInfo.IsUsingUdp);
#endif // CreateFtdcMdApi_argc_3

	delete[] pszPath;

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
			if (strlen(token) > 0)
			{
				m_pApi->RegisterFront(token);
			}
			token = strtok(NULL, _QUANTBOX_SEPS_);
		}
		delete[] buf;

		//初始化连接
		m_pApi->Init();
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}
	else
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->RawErrorID = 0;
		strncpy(pField->Text, "(Api==null)", sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}

#ifdef KS_COPYFILE
	if (bRet)
	{
		DeleteFileA(szNewFileName);
	}
#endif // KS_COPYFILE

	return 0;
}

void CMdUserApi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField* pBody = (CThostFtdcReqUserLoginField*)m_msgQueue_Query->new_block(sizeof(CThostFtdcReqUserLoginField));

	strncpy(pBody->BrokerID, m_ServerInfo.BrokerID, sizeof(TThostFtdcBrokerIDType));
	strncpy(pBody->UserID, m_UserInfo.UserID, sizeof(TThostFtdcInvestorIDType));
	strncpy(pBody->Password, m_UserInfo.Password, sizeof(TThostFtdcPasswordType));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
		pBody, sizeof(CThostFtdcReqUserLoginField), nullptr, 0, nullptr, 0);
}

int CMdUserApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	return m_pApi->ReqUserLogin((CThostFtdcReqUserLoginField*)ptr1, ++m_lRequestID);
}

void CMdUserApi::Disconnect()
{
	_Disconnect(false);
}

void CMdUserApi::_DisconnectInThread()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_Disconnect, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

void CMdUserApi::_Disconnect(bool IsInQueue)
{
	// 清理查询队列
	if (IsInQueue)
	{

	}
	else
	{
		if (m_msgQueue_Query)
		{
			m_msgQueue_Query->StopThread();
			m_msgQueue_Query->Register(nullptr);
			m_msgQueue_Query->Clear();
			delete m_msgQueue_Query;
			m_msgQueue_Query = nullptr;
		}
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
	if (m_pQuoteSubscribeManager)
	{
		delete m_pQuoteSubscribeManager;
		m_pQuoteSubscribeManager = nullptr;
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
	if (nullptr == m_pApi)
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

		//订阅
#ifdef SubscribeMarketData_argc_2
		m_pApi->SubscribeMarketData(pArray, (int)vct.size());
#else
		m_pApi->SubscribeMarketData(pArray, (int)vct.size(), (char*)szExchangeID.c_str());
#endif // SubscribeMarketData_argc_2


		delete[] pArray;
	}
	delete[] pBuf;
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

		//订阅
#ifdef SubscribeMarketData_argc_2
		m_pApi->UnSubscribeMarketData(pArray, (int)vct.size());
#else
		m_pApi->UnSubscribeMarketData(pArray, (int)vct.size(), (char*)szExchangeID.c_str());
#endif // SubscribeMarketData_argc_2


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

	// 断开连接
	_DisconnectInThread();
}

void CMdUserApi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	if (!IsErrorRspInfo(pRspInfo)
		&& pRspUserLogin)
	{
#ifdef HAS_TradingDay_UserLogin
		pField->TradingDay = str_to_yyyyMMdd(pRspUserLogin->TradingDay);
#else
		pField->TradingDay = current_date();
#endif // HAS_TradingDay_UserLogin
		pField->LoginTime = str_to_HHmmss(pRspUserLogin->LoginTime);

		m_TradingDay = pField->TradingDay;
		//if (m_TradingDay == 0)
		{
			// 某些时候可能出现无交易日的情况，只能构造出交易日
			// 18点以后算第二天，之前算当天
			tm _tm0 = current_date_tm();
			if (_tm0.tm_hour >= 18)
			{
				_tm0 = get_next_trading_day(&_tm0);
			}
			else if (_tm0.tm_wday == 6 || _tm0.tm_wday == 0)
			{
				// 凌晨一般不用动，但周六日比较特别
				_tm0 = get_next_trading_day(&_tm0);
			}

			m_TradingDay = tm_to_yyyyMMdd(&_tm0);
		}

		m_PreTradingDay = tm_to_yyyyMMdd(&get_pre_trading_day(&yyyyMMdd_to_tm(m_TradingDay)));

		sprintf(pField->SessionID, "%d:%d", pRspUserLogin->FrontID, pRspUserLogin->SessionID);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logined, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		{
			set<string> exchanges = m_pSubscribeManager->GetExchanges();
			for (auto exchange = exchanges.begin(); exchange != exchanges.end(); ++exchange)
			{
				set<string> mapOld = m_pSubscribeManager->GetParentInstruments(exchange->data());//记下上次订阅的合约
				m_pSubscribeManager->Clear();
				Subscribe(mapOld, exchange->data());//订阅
			}
		}
		{
#ifdef HAS_Quote
			set<string> exchanges = m_pQuoteSubscribeManager->GetExchanges();
			for (auto exchange = exchanges.begin(); exchange != exchanges.end(); ++exchange)
			{
				set<string> mapOld = m_pQuoteSubscribeManager->GetParentInstruments(exchange->data());//记下上次订阅的合约
				m_pQuoteSubscribeManager->Clear();
				SubscribeQuote(mapOld, exchange->data());//订阅
			}
#endif // HAS_Quote
		}
	}
	else
	{
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

		// 断开连接
		_DisconnectInThread();
	}
}

void CMdUserApi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo("OnRspError", pRspInfo, nRequestID, bIsLast);
}

void CMdUserApi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//在模拟平台可能这个函数不会触发，所以要自己维护一张已经订阅的合约列表
	if (!IsErrorRspInfo("OnRspSubMarketData", pRspInfo, nRequestID, bIsLast)
		&& pSpecificInstrument)
	{
	}
}

void CMdUserApi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//模拟平台可能这个函数不会触发
	if (!IsErrorRspInfo("OnRspUnSubMarketData", pRspInfo, nRequestID, bIsLast)
		&& pSpecificInstrument)
	{
	}
}

//行情回调，得保证此函数尽快返回
void CMdUserApi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	//for (int i = 0; i < 50; ++i)
	//{
	//	// 测试平台穿越速度，用完后需要注释掉
	//	WriteLog("CTP:OnRtnDepthMarketData:%s %f %s.%03d", pDepthMarketData->InstrumentID, pDepthMarketData->LastPrice, pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec);

	DepthMarketDataNField* pField = (DepthMarketDataNField*)m_msgQueue->new_block(sizeof(DepthMarketDataNField) + sizeof(DepthField) * 10);

	strcpy(pField->InstrumentID, pDepthMarketData->InstrumentID);
	strcpy(pField->ExchangeID, pDepthMarketData->ExchangeID);
	pField->Exchange = TThostFtdcExchangeIDType_2_ExchangeType(pDepthMarketData->ExchangeID);

	sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pDepthMarketData->ExchangeID);


	// 交易时间
	GetExchangeTime(m_TradingDay, m_PreTradingDay,
		pDepthMarketData->TradingDay, pDepthMarketData->ActionDay, pDepthMarketData->UpdateTime,
		&pField->TradingDay, &pField->ActionDay, &pField->UpdateTime, &pField->UpdateMillisec);

	pField->UpdateMillisec = pDepthMarketData->UpdateMillisec;

	pField->LastPrice = pDepthMarketData->LastPrice == DBL_MAX ? 0 : pDepthMarketData->LastPrice;
	pField->Volume = pDepthMarketData->Volume;
	pField->Turnover = pDepthMarketData->Turnover;
	pField->OpenInterest = pDepthMarketData->OpenInterest;
	pField->AveragePrice = pDepthMarketData->AveragePrice;

	if (pDepthMarketData->OpenPrice != DBL_MAX)
	{
		pField->OpenPrice = pDepthMarketData->OpenPrice;
		pField->HighestPrice = pDepthMarketData->HighestPrice;
		pField->LowestPrice = pDepthMarketData->LowestPrice;
	}
	else
	{
		pField->OpenPrice = 0;
		pField->HighestPrice = 0;
		pField->LowestPrice = 0;
	}
	pField->SettlementPrice = pDepthMarketData->SettlementPrice != DBL_MAX ? pDepthMarketData->SettlementPrice : 0;

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

	// 不使用CThostFtdcDepthMarketDataField是因为不通用
	char* pBuf = m_pSyntheticManager->Update(pField->InstrumentID, pField, pField->Size);

	// 先通知成分，再通知合成，这样合成要触发事件时，成分的数据已经完成了
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, pField, pField->Size, nullptr, 0, nullptr, 0);

	if (pBuf)
	{
		// 0号位是标记位
		auto pSyntheticBuf = (DepthMarketDataNField*)(pBuf);
		// 合约不能为空，空了就不知道通知谁了
		int offest = pSyntheticBuf->InstrumentID - pBuf;
		// 有相关的合约与数据
		auto products = m_pSyntheticManager->GetByInstrument(pSyntheticBuf->InstrumentID);
		for (auto iter = products.begin(); iter != products.end(); iter++)
		{
			auto p = *iter;
			// 只对需要的进行计算和触发
			if (p->CheckEmit(pSyntheticBuf->InstrumentID, offest))
			{
				// 0号位是标记位
				auto q = (DepthMarketDataNField*)p->Calculate(pBuf);
				if (q)
				{
					m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, q, q->Size, nullptr, 0, nullptr, 0);
				}
			}
		}
	}
	//}
}

#ifdef HAS_Quote

void CMdUserApi::SubscribeQuote(const string& szInstrumentIDs, const string& szExchangeID)
{
	if (nullptr == m_pApi)
		return;

	set<string> ss_new = m_pQuoteSubscribeManager->String2Set(szInstrumentIDs.c_str());
	SubscribeQuote(ss_new, szExchangeID);
}

void CMdUserApi::SubscribeQuote(const set<string>& instrumentIDs, const string& szExchangeID)
{
	if (nullptr == m_pApi)
		return;

	if (instrumentIDs.size() == 0)
		return;

	set<string> ss_new = m_pQuoteSubscribeManager->Subscribe(instrumentIDs, szExchangeID);
	string str_new = m_pQuoteSubscribeManager->Set2String(ss_new);
	vector<char*> vct;
	char* pBuf = m_pQuoteSubscribeManager->String2Buf(str_new.c_str(), vct);

	if (vct.size() > 0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j < vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
#ifdef SubscribeMarketData_argc_2
		m_pApi->SubscribeForQuoteRsp(pArray, (int)vct.size());
#else
		m_pApi->SubscribeForQuoteRsp(pArray, (int)vct.size(), (char*)szExchangeID.c_str());
#endif // SubscribeMarketData_argc_2


		delete[] pArray;
	}
	delete[] pBuf;
}

void CMdUserApi::UnsubscribeQuote(const set<string>& instrumentIDs, const string& szExchangeID)
{
	if (nullptr == m_pApi)
		return;

	if (instrumentIDs.size() == 0)
		return;

	set<string> ss_new = m_pQuoteSubscribeManager->Unsubscribe(instrumentIDs, szExchangeID);
	string str_new = m_pQuoteSubscribeManager->Set2String(ss_new);
	vector<char*> vct;
	char* pBuf = m_pQuoteSubscribeManager->String2Buf(str_new.c_str(), vct);

	if (vct.size() > 0)
	{
		//转成字符串数组
		char** pArray = new char*[vct.size()];
		for (size_t j = 0; j < vct.size(); ++j)
		{
			pArray[j] = vct[j];
		}

		//订阅
#ifdef SubscribeMarketData_argc_2
		m_pApi->UnSubscribeForQuoteRsp(pArray, (int)vct.size());
#else
		m_pApi->UnSubscribeForQuoteRsp(pArray, (int)vct.size(), (char*)szExchangeID.c_str());
#endif // SubscribeMarketData_argc_2


		delete[] pArray;
	}
	delete[] pBuf;
}

void CMdUserApi::UnsubscribeQuote(const string& szInstrumentIDs, const string& szExchangeID)
{
	if (nullptr == m_pApi)
		return;

	set<string> ss_new = m_pSubscribeManager->String2Set(szInstrumentIDs.c_str());
	UnsubscribeQuote(ss_new, szExchangeID);
}


void CMdUserApi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo("OnRspSubForQuoteRsp", pRspInfo, nRequestID, bIsLast)
		&& pSpecificInstrument)
	{
	}
}

void CMdUserApi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo("OnRspUnSubForQuoteRsp", pRspInfo, nRequestID, bIsLast)
		&& pSpecificInstrument)
	{
	}
}

void CMdUserApi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	// 上期技术的人说，上海中金走的交易接口，大商，郑商走行情，所以这个地方后期可能要改
	QuoteRequestField* pField = (QuoteRequestField*)m_msgQueue->new_block(sizeof(QuoteRequestField));

	pField->TradingDay = str_to_yyyyMMdd(pForQuoteRsp->TradingDay);
	pField->QuoteTime = str_to_yyyyMMdd(pForQuoteRsp->ForQuoteTime);
	strcpy(pField->Symbol, pForQuoteRsp->InstrumentID);
	strcpy(pField->InstrumentID, pForQuoteRsp->InstrumentID);
	strcpy(pField->ExchangeID, pForQuoteRsp->ExchangeID);
	sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pField->ExchangeID);
	strcpy(pField->QuoteID, pForQuoteRsp->ForQuoteSysID);

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnQuoteRequest, m_msgQueue, m_pClass, 0, 0, pField, sizeof(QuoteRequestField), nullptr, 0, nullptr, 0);
}
#endif // HAS_Quote


