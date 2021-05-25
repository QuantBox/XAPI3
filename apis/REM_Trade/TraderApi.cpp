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

#include <cstring>
#include <assert.h>
#include <cfloat>

#if defined WINDOWS || _WIN32
//#include <libloaderapi.h>
#else
#include <dlfcn.h>
#include <errno.h>
#endif

bool CTraderApi::LoadEESTrader()
{
#ifdef WIN32

	return Windows_LoadEESTrader();

#else

	return Linux_LoadEESTrader();

#endif
}

void CTraderApi::UnloadEESTrader()
{
#ifdef WIN32

	return Windows_UnloadEESTrader();

#else

	return Linux_UnloadEESTrader();

#endif
}

bool CTraderApi::Windows_LoadEESTrader()
{
#ifdef WIN32

	m_handle = LoadLibraryA(EES_TRADER_DLL_NAME);
	if (!m_handle)
	{
		printf("load library(%s) failed.\n", EES_TRADER_DLL_NAME);
		return false;
	}

	funcCreateEESTraderApi createFun = (funcCreateEESTraderApi)GetProcAddress(m_handle, CREATE_EES_TRADER_API_NAME);
	if (!createFun)
	{
		printf("get function addresss(%s) failed!\n", CREATE_EES_TRADER_API_NAME);
		return false;
	}

	m_distoryFun = (funcDestroyEESTraderApi)GetProcAddress(m_handle, DESTROY_EES_TRADER_API_NAME);
	if (!createFun)
	{
		printf("get function addresss(%s) failed!\n", DESTROY_EES_TRADER_API_NAME);
		return false;
	}

	m_tradeApi = createFun();
	if (!m_tradeApi)
	{
		printf("create trade API object failed!\n");
		return false;
	}

#endif

	return true;
}

void CTraderApi::Windows_UnloadEESTrader()
{
#ifdef WIN32

	if (m_tradeApi)
	{
		m_distoryFun(m_tradeApi);
		m_tradeApi = NULL;
		m_distoryFun = NULL;
	}

	if (m_handle)
	{
		FreeLibrary(m_handle);
		m_handle = NULL;
	}
#endif
}

bool CTraderApi::Linux_LoadEESTrader()
{
#ifndef WIN32
	m_handle = dlopen(EES_TRADER_DLL_NAME, RTLD_LAZY);
	if (!m_handle)
	{
		printf("load library(%s) failed.\n", EES_TRADER_DLL_NAME);
		return false;
	}

	funcCreateEESTraderApi createFun = (funcCreateEESTraderApi)dlsym(m_handle, CREATE_EES_TRADER_API_NAME);
	if (!createFun)
	{
		printf("get function addresss(%s) failed!\n", CREATE_EES_TRADER_API_NAME);
		return false;
	}

	m_distoryFun = (funcDestroyEESTraderApi)dlsym(m_handle, DESTROY_EES_TRADER_API_NAME);
	if (!createFun)
	{
		printf("get function addresss(%s) failed!\n", DESTROY_EES_TRADER_API_NAME);
		return false;
	}

	m_tradeApi = createFun();
	if (!m_tradeApi)
	{
		printf("create trade API object failed!\n");
		return false;
	}
#endif

	return true;
}

void CTraderApi::Linux_UnloadEESTrader()
{
#ifndef WIN32
	if (m_tradeApi)
	{
		m_distoryFun(m_tradeApi);
		m_tradeApi = NULL;
		m_distoryFun = NULL;
	}

	if (m_handle)
	{
		dlclose(m_handle);
		m_handle = NULL;
	}
#endif
}

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
		case E_ReqUserLoginField:
			iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case E_QuerySymbolStatus:
			iRet = _QuerySymbolStatus(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case E_QueryUserAccount:
			iRet = _QueryUserAccount(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case E_CancelOrder:
			iRet = _CancelOrder(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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

	// 将时定时查询功能放在查询队列中，这样退出就不会出问题了
	// 由于队列中一直没有东西，所以不会再来触发这个循环，需要隔一定的时间向队列中放一个，最好放在最后
	TestInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	if (m_msgQueue_Query->Size() == 0)
	{
		// 数据为空时，向里插入数据，保证循环
		m_msgQueue_Query->Input_Copy(E_Heartbeat, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		m_nSleep = 512;
	}

	this_thread::sleep_for(chrono::milliseconds(m_nSleep));
}

void CTraderApi::TestInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	time_t _now = time(nullptr);

	m_HHmmss = tm_to_HHmmss(localtime(&_now));

	CheckThenHeartbeat(_now);
}

void CTraderApi::CheckThenHeartbeat(time_t _now)
{
	if (_now > m_HeartbeatTime)
	{
		// 心跳超时
		long _queryTime = 60;
		m_HeartbeatTime = time(nullptr) + _queryTime;

		QueryUserAccount();
		CancelOrder();
	}
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
	m_pOrderMap = new COrderMap<long long>();
	m_pProcessor->m_pOrderMap = m_pOrderMap;
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
		serverConfig.WriteTdDefault(szServerPath);
	}
	if (!isFileExists_ifstream(szUserPath))
	{
		userConfig.WriteDefault(szUserPath);
	}

	m_ServerItem = serverConfig.ReadTd(szServerPath);
	m_UserItem = userConfig.Read(szUserPath);

	m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_Init()
{
	EES_TradeSvrInfo svrInfo;
	strcpy(svrInfo.m_remoteTradeIp, m_ServerItem.RemoteTradeIp.c_str());
	svrInfo.m_remoteTradeTCPPort = m_ServerItem.RemoteTradeTCPPort;
	svrInfo.m_remoteTradeUDPPort = m_ServerItem.RemoteTradeUDPPort;
	strcpy(svrInfo.m_remoteQueryIp, m_ServerItem.RemoteQueryIp.c_str());
	svrInfo.m_remoteQueryTCPPort = m_ServerItem.RemoteQueryTCPPort;
	strcpy(svrInfo.m_LocalTradeIp, m_ServerItem.LocalTradeIp.c_str());
	svrInfo.m_LocalTradeUDPPort = m_ServerItem.LocalTradeUDPPort;

	m_Status = ConnectionStatus::ConnectionStatus_Initialized;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
	pField->RawErrorID = 0;

	// 这里一定要有，否则定时器被跳过
	m_HeartbeatTime = time(nullptr);

	if (LoadEESTrader())
	{
		m_pApi = m_tradeApi;
		// 3版不支持期权，极致版多了此功能
		m_pApi->SetAutoReconnect(true);

		RESULT ret_err = m_pApi->ConnServer(svrInfo, this);
		if (ret_err == NO_ERROR)
		{
			m_Status = ConnectionStatus::ConnectionStatus_Connecting;
			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);

			m_msgQueue->delete_block(pField);
			return 0;
		}

		sprintf(pField->Text, "connect to REM server failed! %d", ret_err);
	}
	else
	{
		strncpy(pField->Text, "(Api==null)", sizeof(Char256Type));
	}

	m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

	return 0;
}

void CTraderApi::ReqUserLogin()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_Status = ConnectionStatus::ConnectionStatus_Logining;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	// 登录前设置
	m_pProcessor->m_msgQueue = m_msgQueue;
	m_pProcessor->m_pClass = m_pClass;

	return m_pApi->UserLogon(m_UserItem.user_id, m_UserItem.user_pwd, m_UserItem.app_id, m_UserItem.auth_code);
}

void CTraderApi::QuerySymbolStatus()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_QuerySymbolStatus, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_QuerySymbolStatus(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;
	return m_pApi->QuerySymbolStatus();
}

void CTraderApi::QueryUserAccount()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_QueryUserAccount, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_QueryUserAccount(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;
	return m_pApi->QueryUserAccount();
}

void CTraderApi::CancelOrder()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_CancelOrder, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_CancelOrder(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	EES_CancelOrder CxlOrder;
	CxlOrder.m_MarketOrderToken = 0;
	strcpy(CxlOrder.m_Account, m_UserItem.account_id);

	return m_pApi->CancelOrder(&CxlOrder);
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
	UnloadEESTrader();
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
}

void OrderField_2_EES_EnterOrderField(UserItem* pUserItem, OrderField* pOrder, EES_EnterOrderField* pEnter)
{
	// TODO: Test
	//pOrder->TimeInForce = TimeInForce_IOC;
	//pOrder->Price = 7000;
	//pOrder->Qty = 2;

	pEnter->m_HedgeFlag = HedgeFlagType_2_EES_HedgeFlag(pOrder->HedgeFlag);

	strcpy(pEnter->m_Account, pUserItem->account_id);
	strcpy(pEnter->m_Symbol, pOrder->InstrumentID);
	pEnter->m_Side = OrderSide_OpenCloseType_2_EES_SideType(pOrder->Side, pOrder->OpenClose);
	pEnter->m_Exchange = ExchangeID_2_EES_ExchangeID(pOrder->ExchangeID);
	pEnter->m_SecType = EES_SecType_fut;
	pEnter->m_Price = pOrder->Price;
	pEnter->m_Qty = pOrder->Qty;

	// IOC与FOK
	switch (pOrder->TimeInForce)
	{
	case TimeInForce_IOC:
		pEnter->m_Tif = EES_OrderTif_IOC;
		break;
	case TimeInForce_FOK:
		pEnter->m_Tif = EES_OrderTif_IOC;
		pEnter->m_MinQty = pEnter->m_Qty;
		break;
	default:
		pEnter->m_Tif = EES_OrderTif_Day;
		break;
	}
}

char* CTraderApi::ReqOrderInsert(
	OrderField* pOrder,
	int count,
	char* pszLocalIDBuf)
{
	if (nullptr == m_pApi)
		return nullptr;

	EES_EnterOrderField* pEnter = (EES_EnterOrderField*)m_msgQueue->new_block(sizeof(EES_EnterOrderField));
	OrderField_2_EES_EnterOrderField(&m_UserItem, pOrder, pEnter);

	OrderField* pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
	memcpy(pField, pOrder, sizeof(OrderField));
	pField->pUserData1 = pEnter;

	RESULT iRet = NO_ERROR;

	{
		lock_guard<mutex> cl(m_csOrderRef);
		pEnter->m_ClientOrderToken = m_nMaxOrderRef;
		++m_nMaxOrderRef;
		// 如果网络有问题，可能这个返回值还是成功，等网络恢复会发
		iRet = m_pApi->EnterOrder(pEnter);
	}
	sprintf(pField->LocalID, "%d", pEnter->m_ClientOrderToken);
	if (iRet != NO_ERROR)
	{
		sprintf(pField->Text, "send order failed(%d)\n", iRet);
		pField->Status = OrderStatus::OrderStatus_Rejected;

		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, true, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
	else
	{
		pField->Status = OrderStatus::OrderStatus_PendingNew;
	}


	// 保存
	m_pOrderMap->insert(pEnter->m_ClientOrderToken, pField, pField->pUserData1);

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

	long long _id = atoll(szIds[0]);
	OrderField* pField = (OrderField*)m_pOrderMap->findOrderXAPI(_id);
	if (pField == nullptr)
	{
		return pzsRtn;
	}

	EES_CancelOrder CxlOrder;
	CxlOrder.m_MarketOrderToken = _id;
	strcpy(CxlOrder.m_Account, m_UserItem.account_id);

	m_pOrderMap->insertCancel(_id, pField);
	RESULT iRet = m_pApi->CancelOrder(&CxlOrder);
	if (iRet == NO_ERROR)
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

void CTraderApi::OnConnection(ERR_NO errNo, const char* pErrStr)
{
	if (errNo == NO_ERROR)
	{
		m_Status = ConnectionStatus::ConnectionStatus_Connected;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		ReqUserLogin();
	}
	else
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
		pField->RawErrorID = errNo;
		sprintf(pField->Text, "%d, %s", errNo, pErrStr);

		m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnDisConnection(ERR_NO errNo, const char* pErrStr)
{
	// 实测禁用网卡并不会触发此函数

	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
	pField->RawErrorID = errNo;
	sprintf(pField->Text, "%d, %s", errNo, pErrStr);

	m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
}

void CTraderApi::OnUserLogon(EES_LogonResponse* pLogon)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	if (pLogon->m_Result == EES_LOGON_OK)
	{
		pField->TradingDay = pLogon->m_TradingDate;
		pField->LoginTime = 0;

		sprintf(pField->SessionID, "%d", pLogon->m_UserId);
		sprintf(pField->Text, "OrderFC: %d/%d, CancelFC: %d/%d",
			pLogon->m_OrderFCCount, pLogon->m_OrderFCInterval,
			pLogon->m_CancelFCCount, pLogon->m_CancelFCInterval);

		m_Status = ConnectionStatus::ConnectionStatus_Logined;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		m_Status = ConnectionStatus::ConnectionStatus_Done;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, nullptr, 0, nullptr, 0, nullptr, 0);


		// 记下登录信息，可能会用到
		memcpy(&m_RspUserLogin, pLogon, sizeof(EES_LogonResponse));

		m_nMaxOrderRef = pLogon->m_MaxToken;
		// 自己发单时ID从1开始，不能从0开始
		m_nMaxOrderRef = ++m_nMaxOrderRef;

		// TEST: 登录成功后就查询交易所状态
		// 发现查询出来的全是空，只能后期再试
		//QuerySymbolStatus();
	}
	else
	{
		pField->RawErrorID = pLogon->m_Result;
		sprintf(pField->Text, "%d", pLogon->m_Result);

		m_Status = ConnectionStatus::ConnectionStatus_Disconnected;
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, m_Status, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

		// 收到登录失败后并没有销毁API，飞鼠一次只能登录一个用户，所以导致飞鼠用户失败
		//_DisconnectInThread();
	}
}

void CTraderApi::OnOrderAccept(EES_OrderAcceptField* pAccept)
{
	m_pProcessor->OnOrderAccept(pAccept);
}


/// 下单被市场接受的事件

	/// \brief 表示这个订单已经被交易所正式的接受
	/// \param  pAccept	                    订单被接受以后的消息体，里面包含了市场订单ID
	/// \return void 
void CTraderApi::OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept)
{
	m_pProcessor->OnOrderMarketAccept(pAccept);
}


///	下单被柜台系统拒绝的事件

	/// \brief	订单被柜台系统拒绝，可以查看语法检查或是风控检查。 
	/// \param  pReject	                    订单被接受以后的消息体
	/// \return void 

void CTraderApi::OnOrderReject(EES_OrderRejectField* pReject)
{
	m_pProcessor->OnOrderReject(pReject);
}


///	下单被市场拒绝的事件

/// \brief	订单被市场拒绝，可以查看语法检查或是风控检查。 
/// \param  pReject	                    订单被接受以后的消息体，里面包含了市场订单ID
/// \return void 

void CTraderApi::OnOrderMarketReject(EES_OrderMarketRejectField* pReject)
{
	m_pProcessor->OnOrderMarketReject(pReject);
}


///	订单成交的消息事件

	/// \brief	成交里面包括了订单市场ID，建议用这个ID查询对应的订单
	/// \param  pExec	                   订单被接受以后的消息体，里面包含了市场订单ID
	/// \return void 

void CTraderApi::OnOrderExecution(EES_OrderExecutionField* pExec)
{
	m_pProcessor->OnOrderExecution(pExec);
}

///	订单成功撤销事件

	/// \brief	成交里面包括了订单市场ID，建议用这个ID查询对应的订单
	/// \param  pCxled		               订单被接受以后的消息体，里面包含了市场订单ID
	/// \return void 

void CTraderApi::OnOrderCxled(EES_OrderCxled* pCxled)
{
	m_pProcessor->OnOrderCxled(pCxled);
}

///	撤单被拒绝的消息事件

	/// \brief	一般会在发送撤单以后，收到这个消息，表示撤单被拒绝
	/// \param  pReject	                   撤单被拒绝消息体
	/// \return void 

void CTraderApi::OnCxlOrderReject(EES_CxlOrderRej* pReject)
{
	m_pProcessor->OnCxlOrderReject(pReject);
}

/// \brief	当合约状态发生变化时报告
/// \param  pSymbolStatus: 参见EES_SymbolStatus合约状态结构体定义
/// \return void 
void CTraderApi::OnSymbolStatusReport(EES_SymbolStatus* pSymbolStatus)
{
	// 由于请求太多，想办法立即返回
	if (m_HHmmss_OnSymbolStatusReport == m_HHmmss)
		return;
	m_HHmmss_OnSymbolStatusReport = m_HHmmss;

	if (pSymbolStatus)
	{
		InstrumentStatusField* pField = (InstrumentStatusField*)m_msgQueue->new_block(sizeof(InstrumentStatusField));

		strcpy(pField->InstrumentID, pSymbolStatus->m_Symbol);
		strcpy(pField->ExchangeID, EES_ExchangeID_2_ExchangeID(pSymbolStatus->m_ExchangeID));
		sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pField->ExchangeID);

		//printf("OnSymbolStatusReport: %s, %s\n", pField->Symbol, pSymbolStatus->m_EnterTime);

		pField->InstrumentStatus = EES_InstrumentStatus_2_InstrumentStatus(pSymbolStatus->m_InstrumentStatus);
		// 取定时器中的时间，这样能快一些
		// m_EnterTime中的值为空
		pField->EnterTime = m_HHmmss_OnSymbolStatusReport;


		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnInstrumentStatus, m_msgQueue, m_pClass, true, 0, pField, sizeof(InstrumentStatusField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnQuerySymbolStatus(EES_SymbolStatus* pSymbolStatus, bool bFinish)
{
	// 1. m_EnterTime的值全为空
	// 2. 只有最近的状态
	if (bFinish)
		return;

	if (pSymbolStatus)
	{
		InstrumentStatusField* pField = (InstrumentStatusField*)m_msgQueue->new_block(sizeof(InstrumentStatusField));

		strcpy(pField->InstrumentID, pSymbolStatus->m_Symbol);
		strcpy(pField->ExchangeID, EES_ExchangeID_2_ExchangeID(pSymbolStatus->m_ExchangeID));
		sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pField->ExchangeID);

		// m_EnterTime的值全为空，
		// printf("OnQuerySymbolStatus: %s, %s\n", pField->Symbol, pSymbolStatus->m_EnterTime);

		pField->InstrumentStatus = EES_InstrumentStatus_2_InstrumentStatus(pSymbolStatus->m_InstrumentStatus);
		if (pSymbolStatus->m_EnterTime[0] == 0)
		{
			pField->EnterTime = m_HHmmss;
		}
		else
		{
			pField->EnterTime = str_to_HHmmss(pSymbolStatus->m_EnterTime);
		}

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnInstrumentStatus, m_msgQueue, m_pClass, true, 0, pField, sizeof(InstrumentStatusField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnQueryUserAccount(EES_AccountInfo* pAccoutnInfo, bool bFinish)
{
	if (bFinish)
		return;

	if (pAccoutnInfo)
	{
		//printf("OnQueryUserAccount:%f,%f,%f,%f,%f,%f,\n",
		//	pAccoutnInfo->m_InitialBp,
		//	pAccoutnInfo->m_AvailableBp,
		//	pAccoutnInfo->m_Margin,
		//	pAccoutnInfo->m_FrozenMargin,
		//	pAccoutnInfo->m_CommissionFee,
		//	pAccoutnInfo->m_FrozenCommission);

		AccountField* pField = (AccountField*)m_msgQueue->new_block(sizeof(AccountField));
		strcpy(pField->AccountID, pAccoutnInfo->m_Account);
		pField->PreBalance = pAccoutnInfo->m_InitialBp;
		pField->Available = pAccoutnInfo->m_AvailableBp;
		pField->CurrMargin = pAccoutnInfo->m_Margin;
		pField->FrozenCash = pAccoutnInfo->m_FrozenMargin;
		pField->Commission = pAccoutnInfo->m_CommissionFee;
		pField->FrozenCommission = pAccoutnInfo->m_FrozenCommission;

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTradingAccount, m_msgQueue, m_pClass, 0, 0, pField, sizeof(AccountField), nullptr, 0, nullptr, 0);
	}
}
