#include "stdafx.h"
#include "TraderApi.h"

#include "../../include/QueueEnum.h"

#include "../../include/ApiHeader.h"
#include "../../include/ApiStruct.h"

#include "../../include/toolkit.h"

#include "../../include/queue/MsgQueue.h"

#include "../CTP/TypeConvert.h"



#include <cstring>
#include <assert.h>
#include <cfloat>
#if defined(_MSC_VER) || defined(_WIN32)
#include <direct.h>
#endif

#ifdef ENABLE_LICENSE
#include "../../common/License/License.h"
#endif


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
	case E_Disconnect:
		_Disconnect(true);
		// 不再循环
		return;
	}

	if (m_pApi)
	{
		switch (type)
		{
		case E_ReqAuthenticateField:
			iRet = _ReqAuthenticate(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case E_ReqUserLoginField:
			iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case E_UserLogoutField:
			iRet = _ReqUserLogout(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case E_ReqUserPasswordUpdateField:
			iRet = _ReqUserPasswordUpdate(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case QueryType::QueryType_ReqQryTradingAccount:
			iRet = _ReqQryTradingAccount(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case QueryType::QueryType_ReqQryInvestorPosition:
			iRet = _ReqQryInvestorPosition(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case QueryType::QueryType_ReqQryInstrument:
			iRet = _ReqQryInstrument(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case QueryType::QueryType_ReqQryInvestor:
			iRet = _ReqQryInvestor(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case QueryType::QueryType_ReqQryOrder:
			iRet = _ReqQryOrder(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case QueryType::QueryType_ReqQryTrade:
			iRet = _ReqQryTrade(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;

#ifdef HAS_Quote
		case QueryType::QueryType_ReqQryQuote:
			iRet = _ReqQryQuote(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
#endif // HAS_Quote

#ifdef HAS_Settlement
		case E_SettlementInfoConfirmField:
			iRet = _ReqSettlementInfoConfirm(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case QueryType::QueryType_ReqQrySettlementInfo:
			iRet = _ReqQrySettlementInfo(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
#endif // HAS_Settlement

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

#ifdef ENABLE_LICENSE
	// 为了这个加密库，居然把编译选项由/MDd改成了/MTd,以后一定要想办法改回去
	m_pLicense = new CLicense();

	char szPath[MAX_PATH] = { 0 };
	m_pLicense->GetDllPathByFunctionName("XRequest", szPath);
	m_pLicense->SetLicensePath(szPath);
	// 这里选的是从文件中加载公钥，可以写死到资源或代码中，这样用户就没有那么容易自己生成公私钥对替换了
	m_pLicense->SetPublicKeyString(DLL_PUBLIC_KEY);

	string signatureString = m_pLicense->LoadStringFromFile(m_pLicense->m_SignaturePath);
	m_pLicense->SetSignatureString(signatureString.c_str());
#endif

	//m_delete = false;
}


CTraderApi::~CTraderApi(void)
{
	_Disconnect(false);

#ifdef ENABLE_LICENSE
	if (m_pLicense == nullptr)
	{
		delete m_pLicense;
		m_pLicense = nullptr;
	}
#endif
}

bool CTraderApi::IsErrorRspInfo(const char* szSource, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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

bool CTraderApi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrorID != 0));

	return bRet;
}

void CTraderApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo,
	int count)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_Init, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_Init()
{
#ifdef ENABLE_LICENSE
	int err = m_pLicense->LoadIni();
	if (err != 0)
	{
		// 没有授权文件
		m_pLicense->CreateDefault();
		m_pLicense->AddUser(m_UserInfo.UserID, "*");
		err = m_pLicense->SaveIni();
	}

	if (err == 0)
	{
		err = m_pLicense->GetErrorCodeForMachineID();
	}

	if (err == 0)
	{
		err = m_pLicense->GetErrorCodeByAccount(m_UserInfo.UserID);
	}

	if (err == 0)
	{
		err = m_pLicense->GetErrorCodeForSign();
	}

	if (err == 0)
	{
		err = m_pLicense->GetErrorCodeForExpireDate();
	}

	if (err != 0)
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->RawErrorID = err;
		strncpy(pField->Text, m_pLicense->GetErrorInfo(), sizeof(Char256Type));

		// >0的错误表示不严重，可以继续
		if (err > 0)
		{
			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Initialized, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		}
		else if (err < 0)
		{
			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
			return 0;
		}
	}
#endif

	char *pszPath = new char[m_szPath.length() + 1024];
	srand((unsigned int)time(nullptr));
	sprintf(pszPath, "%s/%s/%s/Td/%d/", m_szPath.c_str(), m_ServerInfo.BrokerID, m_UserInfo.UserID, rand());
	makedirs(pszPath);


	// 本来想使用chdir的方法解决Kingstar的证书问题，测试多次发现还是读取的exe目录下
	// 打算使用文件复制的方法来实现，
	// 1.先检查证书是否存在，存在就跳过
	// 2.复制，用完要删
#ifdef KS_COPYFILE
	lock_guard<mutex> cl(m_csOrderRef);

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

	m_pApi = CThostFtdcTraderApi::CreateFtdcTraderApi(pszPath);
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
			token = strtok(nullptr, _QUANTBOX_SEPS_);
		}
		delete[] buf;

		//如果空着，反而是从头开始接收了，所以这里要特别处理一下
		if (m_ServerInfo.PublicTopicResumeType < ResumeType::ResumeType_Undefined)
		{
			m_pApi->SubscribePublicTopic((THOST_TE_RESUME_TYPE)m_ServerInfo.PublicTopicResumeType);
		}
		else
		{
			m_pApi->SubscribePublicTopic(THOST_TERT_QUICK);
		}

		if (m_ServerInfo.PrivateTopicResumeType < ResumeType::ResumeType_Undefined)
		{
			m_pApi->SubscribePrivateTopic((THOST_TE_RESUME_TYPE)m_ServerInfo.PrivateTopicResumeType);
		}
		else
		{
			m_pApi->SubscribePrivateTopic(THOST_TERT_QUICK);
		}

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

void CTraderApi::OnFrontConnected()
{
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	//连接成功后自动请求认证或登录
	if (strlen(m_ServerInfo.AuthCode) > 0)
	{
		//填了认证码就先认证
		ReqAuthenticate();
	}
	else
	{
		ReqUserLogin();
	}
}

void CTraderApi::OnFrontDisconnected(int nReason)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	//连接失败返回的信息是拼接而成，主要是为了统一输出
	pField->RawErrorID = nReason;
	GetOnFrontDisconnectedMsg(nReason, pField->Text);

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

	// 收到登录失败后并没有销毁API，飞鼠只能登录一个用户，所以导致飞鼠登录失败
	_DisconnectInThread();
}

void CTraderApi::ReqAuthenticate()
{
	CThostFtdcReqAuthenticateField* pBody = (CThostFtdcReqAuthenticateField*)m_msgQueue_Query->new_block(sizeof(CThostFtdcReqAuthenticateField));

	strncpy(pBody->BrokerID, m_ServerInfo.BrokerID, sizeof(TThostFtdcBrokerIDType));
	strncpy(pBody->UserID, m_UserInfo.UserID, sizeof(TThostFtdcInvestorIDType));
	strncpy(pBody->UserProductInfo, m_ServerInfo.UserProductInfo, sizeof(TThostFtdcProductInfoType));
	strncpy(pBody->AuthCode, m_ServerInfo.AuthCode, sizeof(TThostFtdcAuthCodeType));

#ifdef USE_APP_ID
	// CTP接口新加了穿透试认证
	strncpy(pBody->AppID, m_ServerInfo.AppID, sizeof(TThostFtdcAppIDType));
#endif

	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqAuthenticateField, m_msgQueue_Query, this, 0, 0,
		pBody, sizeof(CThostFtdcReqAuthenticateField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqAuthenticate(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Authorizing, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	return m_pApi->ReqAuthenticate((CThostFtdcReqAuthenticateField*)ptr1, ++m_lRequestID);
}

void CTraderApi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo)
		&& pRspAuthenticateField)
	{
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Authorized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		ReqUserLogin();
	}
	else
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField* pBody = (CThostFtdcReqUserLoginField*)m_msgQueue_Query->new_block(sizeof(CThostFtdcReqUserLoginField));

	strncpy(pBody->BrokerID, m_ServerInfo.BrokerID, sizeof(TThostFtdcBrokerIDType));
	strncpy(pBody->UserID, m_UserInfo.UserID, sizeof(TThostFtdcInvestorIDType));
	strncpy(pBody->Password, m_UserInfo.Password, sizeof(TThostFtdcPasswordType));
	strncpy(pBody->UserProductInfo, m_ServerInfo.UserProductInfo, sizeof(TThostFtdcProductInfoType));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
		pBody, sizeof(CThostFtdcReqUserLoginField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	return m_pApi->ReqUserLogin((CThostFtdcReqUserLoginField*)ptr1, ++m_lRequestID);
}

void CTraderApi::ReqUserLogout()
{
	CThostFtdcUserLogoutField* pBody = (CThostFtdcUserLogoutField*)m_msgQueue_Query->new_block(sizeof(CThostFtdcUserLogoutField));

	strncpy(pBody->BrokerID, m_ServerInfo.BrokerID, sizeof(TThostFtdcBrokerIDType));
	strncpy(pBody->UserID, m_UserInfo.UserID, sizeof(TThostFtdcInvestorIDType));

	if (m_msgQueue_Query)
	{
		m_msgQueue_Query->Input_NoCopy(RequestType::E_UserLogoutField, m_msgQueue_Query, this, 0, 0,
			pBody, sizeof(CThostFtdcReqUserLoginField), nullptr, 0, nullptr, 0);
	}
}

int CTraderApi::_ReqUserLogout(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi)
	{
		return m_pApi->ReqUserLogout((CThostFtdcUserLogoutField*)ptr1, ++m_lRequestID);
	}
	return 0;
}


void CTraderApi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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

		sprintf(pField->SessionID, "%d:%d", pRspUserLogin->FrontID, pRspUserLogin->SessionID);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logined, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

		// 记下登录信息，可能会用到
		memcpy(&m_RspUserLogin, pRspUserLogin, sizeof(CThostFtdcRspUserLoginField));

		// 飞鼠是报单引用是长字符串
#ifdef USE_LONG_ORDER_REF
		m_nMaxOrderRef = atol(pRspUserLogin->MaxOrderRef);
#else
		m_nMaxOrderRef = atol(pRspUserLogin->MaxOrderRef);
#endif
		// 自己发单时ID从1开始，不能从0开始
		//m_nMaxOrderRef = m_nMaxOrderRef>1 ? m_nMaxOrderRef : 1;

		// 飞鼠接口，这个数返回的是上次退出时最大编号，所以将要使用的得+1
		// 而其它接口是这次可以使用的报单编号，所以出于使用考虑，直接+1
		m_nMaxOrderRef += 1;

		// 结算单确认，CTP必须确认才能下单
#ifdef HAS_Settlement
		ReqSettlementInfoConfirm();
#else
		QueryOrderTrade(false);
#endif // HAS_Settlement



#ifdef IS_SGIT_API
		// 飞鼠返回的资金太多了，关闭掉
		m_pApi->IsReviveNtyCapital(FALSE);

#ifdef ENABLE_LICENSE
		// 飞鼠由于目前没办法查投资者列表，无法根据姓名来判断授权，所以只能提前设置为可用
		m_pLicense->SetSendOrderFlag(true);
#endif

#else
		// 查询投资查，授权认证时使
		// 飞鼠使用此功能需要服务器升级
		ReqQueryField body = { 0 };
		ReqQuery(QueryType::QueryType_ReqQryInvestor, &body);
#endif


		//TEST:测完要删
		//body.DateStart = 20151111;
		//ReqQuery(QueryType::ReqQrySettlementInfo, &body);
	}
	else
	{
		// 140,首次登录改密码
		// 131 弱密码
		//if (pRspInfo->ErrorID == 140)
		//{
		//	// ReqUserPasswordUpdate("123456", "123@abc");
		//	ReqUserPasswordUpdate("123456", "279183@ABC");
		//	return;
		//}

		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

		// 收到登录失败后并没有销毁API，飞鼠一次只能登录一个用户，所以导致飞鼠用户失败
		_DisconnectInThread();
	}
}

void CTraderApi::ReqUserPasswordUpdate(char* szOldPassword, char* szNewPassword)
{
	CThostFtdcUserPasswordUpdateField* pBody = (CThostFtdcUserPasswordUpdateField*)m_msgQueue_Query->new_block(sizeof(CThostFtdcUserPasswordUpdateField));

	strncpy(pBody->BrokerID, m_ServerInfo.BrokerID, sizeof(TThostFtdcBrokerIDType));
	strncpy(pBody->UserID, m_UserInfo.UserID, sizeof(TThostFtdcUserIDType));
	strncpy(pBody->OldPassword, szOldPassword, sizeof(TThostFtdcPasswordType));
	strncpy(pBody->NewPassword, szNewPassword, sizeof(TThostFtdcPasswordType));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserPasswordUpdateField, m_msgQueue_Query, this, 0, 0,
		pBody, sizeof(CThostFtdcUserPasswordUpdateField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqUserPasswordUpdate(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	return m_pApi->ReqUserPasswordUpdate((CThostFtdcUserPasswordUpdateField*)ptr1, ++m_lRequestID);
}

void CTraderApi::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo)
		&& pUserPasswordUpdate)
	{
	}
	else
	{
	}
}

void CTraderApi::QueryOrderTrade(bool bForceQuery)
{
	if (bForceQuery ||
		(m_ServerInfo.PrivateTopicResumeType > ResumeType::ResumeType_Restart
			&& (m_ServerInfo.PrivateTopicResumeType < ResumeType::ResumeType_Undefined)))
	{
		ReqQueryField body = { 0 };
		ReqQuery(QueryType::QueryType_ReqQryOrder, &body);
		ReqQuery(QueryType::QueryType_ReqQryQuote, &body);
	}
}

#ifdef HAS_Settlement
void CTraderApi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField* pBody = (CThostFtdcSettlementInfoConfirmField*)m_msgQueue_Query->new_block(sizeof(CThostFtdcSettlementInfoConfirmField));

	strncpy(pBody->BrokerID, m_ServerInfo.BrokerID, sizeof(TThostFtdcBrokerIDType));
	strncpy(pBody->InvestorID, m_UserInfo.UserID, sizeof(TThostFtdcInvestorIDType));

	m_msgQueue_Query->Input_NoCopy(RequestType::E_SettlementInfoConfirmField, m_msgQueue_Query, this, 0, 0,
		pBody, sizeof(CThostFtdcSettlementInfoConfirmField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqSettlementInfoConfirm(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Confirming, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	return m_pApi->ReqSettlementInfoConfirm((CThostFtdcSettlementInfoConfirmField*)ptr1, ++m_lRequestID);
}

void CTraderApi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo)
		&& pSettlementInfoConfirm)
	{
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Confirmed, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		// 结算单确认完成，后查询委托与成交
		QueryOrderTrade(false);
	}
	else
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
}

#endif // HAS_Settlement


void CTraderApi::Disconnect()
{
	_Disconnect(false);
}

void CTraderApi::_DisconnectInThread()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_Disconnect, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

void CTraderApi::_Disconnect(bool IsInQueue)
{
	//if (m_delete)
	//	return;

	//m_delete = true;

	// 由于飞鼠一次只能登录一个账号，以前的方法连接可能没有断开，这里只能主动退出，其它的程序就不管了
#ifdef IS_SGIT_API
	ReqUserLogout();
	Sleep(1000);
#endif

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
		// 为何感觉飞鼠没有登出成功，还在运行？
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

	m_lRequestID = 0;

	Clear();
}

void CTraderApi::Clear()
{
	for (unordered_map<string, OrderField*>::iterator it = m_id_platform_order.begin(); it != m_id_platform_order.end(); ++it)
		delete it->second;
	m_id_platform_order.clear();

	for (unordered_map<string, CThostFtdcOrderField*>::iterator it = m_id_api_order.begin(); it != m_id_api_order.end(); ++it)
		delete it->second;
	m_id_api_order.clear();

	for (unordered_map<string, PositionField*>::iterator it = m_id_platform_position.begin(); it != m_id_platform_position.end(); ++it)
		delete it->second;
	m_id_platform_position.clear();

	for (unordered_map<string, CThostFtdcInvestorPositionField*>::iterator it = m_id_api_position.begin(); it != m_id_api_position.end(); ++it)
		delete it->second;
	m_id_api_position.clear();

#ifdef HAS_Quote
	for (unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.begin(); it != m_id_platform_quote.end(); ++it)
		delete it->second;
	m_id_platform_quote.clear();

	for (unordered_map<string, CThostFtdcQuoteField*>::iterator it = m_id_api_quote.begin(); it != m_id_api_quote.end(); ++it)
		delete it->second;
	m_id_api_quote.clear();
#endif // HAS_Quote

}

char* CTraderApi::ReqOrderInsert(
	OrderField* pOrder,
	int count,
	char* pszLocalIDBuf)
{
	int OrderRef = -1;
	if (nullptr == m_pApi)
		return nullptr;

	CThostFtdcInputOrderField body = { 0 };

	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TThostFtdcBrokerIDType));
	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TThostFtdcInvestorIDType));

#ifdef IS_SGIT_API
	// 为飞鼠加的，否则报席位不符
	strncpy(body.UserID, m_RspUserLogin.UserID, sizeof(TThostFtdcUserIDType));
#endif

	body.MinVolume = 1;
	body.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	body.IsAutoSuspend = 0;
	body.UserForceClose = 0;
	body.IsSwapOrder = 0;

	//合约
	strncpy(body.InstrumentID, pOrder->InstrumentID, sizeof(TThostFtdcInstrumentIDType));
#ifdef HAS_ExchangeID_Order
	strncpy(body.ExchangeID, pOrder->ExchangeID, sizeof(TThostFtdcExchangeIDType));
#endif // HAS_ExchangeID_Order

	//买卖
	body.Direction = OrderSide_2_TThostFtdcDirectionType(pOrder->Side);
	//开平
	body.CombOffsetFlag[0] = OpenCloseType_2_TThostFtdcOffsetFlagType(pOrder->OpenClose);
	//投保
	body.CombHedgeFlag[0] = HedgeFlagType_2_TThostFtdcHedgeFlagType(pOrder->HedgeFlag);
	//数量
	body.VolumeTotalOriginal = (int)pOrder->Qty;

	// 对于套利单，是用第一个参数的价格，还是用两个参数的价格差呢？
	body.LimitPrice = pOrder->Price;
	body.StopPrice = pOrder->StopPx;

	// 针对第二个进行处理，如果有第二个参数，认为是交易所套利单
	if (count > 1)
	{
		body.CombOffsetFlag[1] = OpenCloseType_2_TThostFtdcOffsetFlagType(pOrder[1].OpenClose);
		body.CombHedgeFlag[1] = HedgeFlagType_2_TThostFtdcHedgeFlagType(pOrder[1].HedgeFlag);
		// 交易所的移仓换月功能，没有实测过
		body.IsSwapOrder = (body.CombOffsetFlag[0] != body.CombOffsetFlag[1]);
	}

	// 市价与限价
	switch (pOrder->Type)
	{
	case OrderType_Market:
	case OrderType_Stop:
	case OrderType_MarketOnClose:
	case OrderType_TrailingStop:
		body.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		body.TimeCondition = THOST_FTDC_TC_IOC;
		body.LimitPrice = 0;
		break;
	case OrderType_Limit:
	case OrderType_StopLimit:
	case OrderType_TrailingStopLimit:
	default:
		body.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		body.TimeCondition = THOST_FTDC_TC_GFD;
		break;
	}

	// IOC与FOK
	switch (pOrder->TimeInForce)
	{
	case TimeInForce_IOC:
		body.TimeCondition = THOST_FTDC_TC_IOC;
		body.VolumeCondition = THOST_FTDC_VC_AV;
		break;
	case TimeInForce_FOK:
		body.TimeCondition = THOST_FTDC_TC_IOC;
		body.VolumeCondition = THOST_FTDC_VC_CV;
		//body.MinVolume = body.VolumeTotalOriginal; // 这个地方必须加吗？
		break;
	default:
		body.VolumeCondition = THOST_FTDC_VC_AV;
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
		body.ContingentCondition = THOST_FTDC_CC_Immediately;
		break;
	default:
		body.ContingentCondition = THOST_FTDC_CC_Immediately;
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
#ifdef USE_LONG_ORDER_REF
		sprintf(body.OrderRef, "%012lld", nRet);
#endif

		// 测试平台穿越速度，用完后需要注释掉
		//WriteLog("CTP:ReqOrderInsert:%s %d", body.InstrumentID, nRet);
#ifdef ENABLE_LICENSE
		int err = m_pLicense->GetErrorCodeForSendOrder();
		if (err < 0)
		{
			sprintf(m_orderInsert_Id, "%d", -5);

			OrderField* pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
			memcpy(pField, pOrder, sizeof(OrderField));
			strcpy(pField->ID, m_orderInsert_Id);
			strcpy(pField->LocalID, pField->ID);
			m_id_platform_order.insert(pair<string, OrderField*>(pField->LocalID, pField));

			pField->RawErrorID = err;
			strncpy(pField->Text, m_pLicense->GetErrorInfo(), sizeof(Char256Type));

			pField->Status = OrderStatus::OrderStatus_Rejected;
			pField->ExecType = ExecType::ExecType_Rejected;

			strncpy((char*)pszLocalIDBuf, m_orderInsert_Id, sizeof(OrderIDType));

			// 这个问题的关键是先到队列还是先返回
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);

			return pszLocalIDBuf;
		}
#endif


		//不保存到队列，而是直接发送
		int n = m_pApi->ReqOrderInsert(&body, ++m_lRequestID);
		if (n < 0)
		{
			nRet = n;
			sprintf(m_orderInsert_Id, "%d", nRet);
		}
		else
		{
			// 用于各种情况下找到原订单，用于进行响应的通知
			sprintf(m_orderInsert_Id, "%d:%d:%d", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, nRet);
#ifdef USE_LONG_ORDER_REF
			sprintf(m_orderInsert_Id, "%d:%d:%012lld", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, nRet);
#endif

			OrderField* pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
			memcpy(pField, pOrder, sizeof(OrderField));
			strcpy(pField->ID, m_orderInsert_Id);
			strcpy(pField->LocalID, pField->ID);
			m_id_platform_order.insert(pair<string, OrderField*>(pField->LocalID, pField));

		}
		strncpy((char*)pszLocalIDBuf, m_orderInsert_Id, sizeof(OrderIDType));
	}

	return pszLocalIDBuf;
}

void CTraderApi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OrderIDType orderId = { 0 };

	// 飞鼠报单成功也会走此路，所以需要处理一下，即只有发现有错误信息时才报出去

	if (pInputOrder)
	{
		sprintf(orderId, "%d:%d:%s", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, pInputOrder->OrderRef);
	}
	else
	{
		// IsErrorRspInfo("OnRspOrderInsert", pRspInfo, nRequestID, bIsLast);
	}

	if (IsErrorRspInfo(pRspInfo))
	{
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
			pField->ExecType = ExecType::ExecType_Rejected;
			pField->Status = OrderStatus::OrderStatus_Rejected;
			pField->RawErrorID = pRspInfo->ErrorID;
			strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
	}
}

void CTraderApi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
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
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		OrderField* pField = it->second;
		pField->ExecType = ExecType::ExecType_Rejected;
		pField->Status = OrderStatus::OrderStatus_Rejected;
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	OnTrade(pTrade, 0, true);
}

char* CTraderApi::ReqOrderAction(OrderIDType* szIds, int count, char* pzsRtn)
{
	unordered_map<string, CThostFtdcOrderField*>::iterator it = m_id_api_order.find(szIds[0]);
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

int CTraderApi::ReqOrderAction(OrderField *pOrder, int count, OrderIDType* pOutput)
{
	/*if (nullptr == m_pApi)
		return 0;

	CThostFtdcInputOrderActionField body = { 0 };

	///经纪公司代码
	strncpy(body.BrokerID, pOrder->BrokerID, sizeof(TThostFtdcBrokerIDType));
	///投资者代码
	strncpy(body.InvestorID, pOrder->InvestorID, sizeof(TThostFtdcInvestorIDType));
	///报单引用
	strncpy(body.OrderRef, pOrder->OrderRef, sizeof(TThostFtdcOrderRefType));
	///前置编号
	body.FrontID = pOrder->FrontID;
	///会话编号
	body.SessionID = pOrder->SessionID;
	///交易所代码
	strncpy(body.ExchangeID, pOrder->ExchangeID, sizeof(TThostFtdcExchangeIDType));
	///报单编号
	strncpy(body.OrderSysID, pOrder->OrderSysID, sizeof(TThostFtdcOrderSysIDType));
	///操作标志
	body.ActionFlag = THOST_FTDC_AF_Delete;
	///合约代码
	strncpy(body.InstrumentID, pOrder->InstrumentID, sizeof(TThostFtdcInstrumentIDType));

	int nRet = m_pApi->ReqOrderAction(&body, ++m_lRequestID);
	if (nRet < 0)
	{
		sprintf(m_orderAction_Id, "%d", nRet);
	}
	else
	{
		memset(m_orderAction_Id, 0, sizeof(OrderIDType));
	}
	strncpy((char*)pOutput, m_orderAction_Id, sizeof(OrderIDType));


	return nRet;*/
	return 0;
}

char* CTraderApi::ReqOrderAction(CThostFtdcOrderField *pOrder, int count, char* pzsRtn)
{
	if (nullptr == m_pApi)
		return 0;

	CThostFtdcInputOrderActionField body = { 0 };

	///经纪公司代码
	strncpy(body.BrokerID, pOrder->BrokerID, sizeof(TThostFtdcBrokerIDType));
	///投资者代码
	strncpy(body.InvestorID, pOrder->InvestorID, sizeof(TThostFtdcInvestorIDType));
	///报单引用
	strncpy(body.OrderRef, pOrder->OrderRef, sizeof(TThostFtdcOrderRefType));
	///前置编号
	body.FrontID = pOrder->FrontID;
	///会话编号
	body.SessionID = pOrder->SessionID;
	///交易所代码
	strncpy(body.ExchangeID, pOrder->ExchangeID, sizeof(TThostFtdcExchangeIDType));
	///报单编号
	strncpy(body.OrderSysID, pOrder->OrderSysID, sizeof(TThostFtdcOrderSysIDType));
	///操作标志
	body.ActionFlag = THOST_FTDC_AF_Delete;
	///合约代码
	strncpy(body.InstrumentID, pOrder->InstrumentID, sizeof(TThostFtdcInstrumentIDType));

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

void CTraderApi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OrderIDType orderId = { 0 };
	if (pInputOrderAction)
	{
		sprintf(orderId, "%d:%d:%s", pInputOrderAction->FrontID, pInputOrderAction->SessionID, pInputOrderAction->OrderRef);
	}
	else
	{
		//IsErrorRspInfo("OnRspOrderAction", pRspInfo, nRequestID, bIsLast);
	}

	if (IsErrorRspInfo(pRspInfo))
	{
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
			pField->ExecType = ExecType::ExecType_CancelReject;
			pField->RawErrorID = pRspInfo->ErrorID;
			strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
	}
}

void CTraderApi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
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
		strcpy(pField->ID, orderId);
		strcpy(pField->LocalID, pField->ID);
		pField->ExecType = ExecType::ExecType_CancelReject;
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	OnOrder(pOrder, 0, true);
}


int CTraderApi::_ReqQryTradingAccount(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	CThostFtdcQryTradingAccountField body = { 0 };

	strcpy(body.BrokerID, m_RspUserLogin.BrokerID);
	strcpy(body.InvestorID, m_RspUserLogin.UserID);

	return m_pApi->ReqQryTradingAccount(&body, ++m_lRequestID);
}

void CTraderApi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo("OnRspQryTradingAccount", pRspInfo, nRequestID, bIsLast))
	{
		if (pTradingAccount)
		{
			AccountField* pField = (AccountField*)m_msgQueue->new_block(sizeof(AccountField));

			strcpy(pField->AccountID, pTradingAccount->AccountID);
			pField->CurrMargin = pTradingAccount->CurrMargin;
			pField->Commission = pTradingAccount->Commission;
			pField->Available = pTradingAccount->Available;
			pField->Deposit = pTradingAccount->Deposit;
			pField->Withdraw = pTradingAccount->Withdraw;
			pField->WithdrawQuota = pTradingAccount->WithdrawQuota;

#ifdef HAS_Account_Future
			pField->PreBalance = pTradingAccount->PreBalance;
			pField->CloseProfit = pTradingAccount->CloseProfit;
			pField->PositionProfit = pTradingAccount->PositionProfit;
			pField->Balance = pTradingAccount->Balance;
#endif

			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTradingAccount, m_msgQueue, m_pClass, bIsLast, 0, pField, sizeof(AccountField), nullptr, 0, nullptr, 0);
		}
		else
		{
			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTradingAccount, m_msgQueue, m_pClass, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}
}


int CTraderApi::_ReqQryInvestorPosition(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	CThostFtdcQryInvestorPositionField body = { 0 };

	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TThostFtdcBrokerIDType));
	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TThostFtdcInvestorIDType));
	strncpy(body.InstrumentID, pQuery->InstrumentID, sizeof(TThostFtdcInstrumentIDType));

	return m_pApi->ReqQryInvestorPosition(&body, ++m_lRequestID);
}

void CTraderApi::GetPositionID(CThostFtdcInvestorPositionField *pInvestorPosition, PositionIDType positionId)
{

#ifdef HAS_ExchangeID_Position
	//PositionIDType positionId = { 0 };
	sprintf(positionId, "%s:%s:%d:%d:%c",
		pInvestorPosition->ExchangeID,
		pInvestorPosition->InstrumentID,
		TThostFtdcPosiDirectionType_2_PositionSide(pInvestorPosition->PosiDirection),
		TThostFtdcHedgeFlagType_2_HedgeFlagType(pInvestorPosition->HedgeFlag),
		pInvestorPosition->PositionDate);
#else
	//PositionIDType positionId = { 0 };
	sprintf(positionId, "%s:%s:%d:%d:%c",
		"",
		pInvestorPosition->InstrumentID,
		TThostFtdcPosiDirectionType_2_PositionSide(pInvestorPosition->PosiDirection),
		TThostFtdcHedgeFlagType_2_HedgeFlagType(pInvestorPosition->HedgeFlag),
		pInvestorPosition->PositionDate);

#endif // HAS_ExchangeID
}

void CTraderApi::GetPositionID2(CThostFtdcInvestorPositionField *pInvestorPosition, PositionIDType positionId)
{
#ifdef HAS_ExchangeID_Position
	//PositionIDType positionId = { 0 };
	sprintf(positionId, "%s:%s:%d:%d",
		pInvestorPosition->ExchangeID,
		pInvestorPosition->InstrumentID,
		TThostFtdcPosiDirectionType_2_PositionSide(pInvestorPosition->PosiDirection),
		TThostFtdcHedgeFlagType_2_HedgeFlagType(pInvestorPosition->HedgeFlag));
#else
	//PositionIDType positionId = { 0 };
	sprintf(positionId, "%s:%s:%d:%d",
		"",
		pInvestorPosition->InstrumentID,
		TThostFtdcPosiDirectionType_2_PositionSide(pInvestorPosition->PosiDirection),
		TThostFtdcHedgeFlagType_2_HedgeFlagType(pInvestorPosition->HedgeFlag));

#endif // HAS_ExchangeID
}

// 如果是请求查询，就将数据全部返回
// 如果是后期的成交回报，就只返回更新的记录
// 对于中金所，同时有今昨两天的持仓时，只返回今天的两条多空数据
// 对于上期所，目前没条件测，当成是也只有两条
void CTraderApi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	// 融航接口的设计缺陷，20180830
	// 它会通过返回-1来表示没有记录，而查询委托就没有这个问题
	if (pRspInfo && pRspInfo->ErrorID == -1)
	{
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		return;
	}
	if (!IsErrorRspInfo("OnRspQryInvestorPosition", pRspInfo, nRequestID, bIsLast))
	{
		// 如果没有持仓，返回空的数据
		if (nullptr == pInvestorPosition)
		{
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
			return;
		}

		if (strcmp(pInvestorPosition->InstrumentID, "al1810") == 0)
		{
			int dubug = 1;
		}

		// 得到持仓ID
		PositionIDType positionId = { 0 };
		GetPositionID(pInvestorPosition, positionId);

		// 先将原始的持仓都记录下来
		{
			unordered_map<string, CThostFtdcInvestorPositionField*>::iterator it = m_id_api_position.find(positionId);
			if (it == m_id_api_position.end())
			{
				// 找不到此订单，表示是新单
				CThostFtdcInvestorPositionField* pField = (CThostFtdcInvestorPositionField*)m_msgQueue->new_block(sizeof(CThostFtdcInvestorPositionField));
				memcpy(pField, pInvestorPosition, sizeof(CThostFtdcInvestorPositionField));
				m_id_api_position.insert(pair<string, CThostFtdcInvestorPositionField*>(positionId, pField));
			}
			else
			{
				// 找到了订单
				// 需要再复制保存最后一次的状态，还是只要第一次的用于撤单即可？记下，这样最后好比较
				CThostFtdcInvestorPositionField* pField = it->second;
				memcpy(pField, pInvestorPosition, sizeof(CThostFtdcInvestorPositionField));
			}
		}

		// 等最后一条记录，开始遍历列表，将所有持仓整理后推送出去
		// 为何要这样做？因为今昨是两条记录，但我记在一个里面
		if (!bIsLast)
			return;

		// 清理一下否则可能加错
		for (unordered_map<string, PositionField*>::iterator it = m_id_platform_position.begin(); it != m_id_platform_position.end(); ++it)
			delete it->second;
		m_id_platform_position.clear();

		// 遍历原始的持仓，生成自己的结构体
		for (unordered_map<string, CThostFtdcInvestorPositionField*>::iterator iter = m_id_api_position.begin(); iter != m_id_api_position.end(); iter++)
		{
			CThostFtdcInvestorPositionField* pField2 = iter->second;

			//if (strcmp(pField2->InstrumentID, "al1810") == 0)
			if (strcmp(pField2->InstrumentID, "al1811") == 0)
			{
				int dubug = 1;
			}

			// 得到持仓ID
			PositionIDType positionId2 = { 0 };
			GetPositionID2(pField2, positionId2);

			// 没到找，创建
			// 找到了，求和，修改
			PositionField* pField = nullptr;
			unordered_map<string, PositionField*>::iterator it = m_id_platform_position.find(positionId2);
			if (it == m_id_platform_position.end())
			{
				pField = (PositionField*)m_msgQueue->new_block(sizeof(PositionField));

				strcpy(pField->ID, positionId2);

				strcpy(pField->InstrumentID, pField2->InstrumentID);
#ifdef HAS_ExchangeID_Position
				strcpy(pField->ExchangeID, pField2->ExchangeID);
#endif // HAS_ExchangeID
				sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pField->ExchangeID);
				strcpy(pField->AccountID, pField2->InvestorID);

				pField->Side = TThostFtdcPosiDirectionType_2_PositionSide(pField2->PosiDirection);
				pField->HedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pField2->HedgeFlag);

				m_id_platform_position.insert(pair<string, PositionField*>(positionId2, pField));

				pField->Position = pField2->Position;
				pField->TodayPosition = pField2->TodayPosition;
				pField->HistoryPosition = pField2->YdPosition;
			}
			else
			{
				pField = it->second;

				pField->Position += pField2->Position;
				pField->TodayPosition += pField2->TodayPosition;
				pField->HistoryPosition += pField2->YdPosition;
			}

			// 飞鼠的定义与CTP有所不同
#ifdef IS_SGIT_API
			double tmp = pField->Position;
			pField->Position = pField->TodayPosition;
			pField->TodayPosition = tmp;
			pField->HistoryPosition = pField->HistoryPosition;
#else
			pField->HistoryPosition = pField->Position - pField->TodayPosition;
#endif
		}

		// 将持仓通知出来
		{
			int cnt = 0;
			size_t count = m_id_platform_position.size();
			for (unordered_map<string, PositionField*>::iterator iter = m_id_platform_position.begin(); iter != m_id_platform_position.end(); iter++)
			{
				++cnt;
				m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, cnt == count, 0, iter->second, sizeof(PositionField), nullptr, 0, nullptr, 0);
			}
		}
	}
}

void CTraderApi::ReqQuery(QueryType type, ReqQueryField* pQuery)
{
	m_msgQueue_Query->Input_Copy(type, m_msgQueue_Query, this, 0, 0,
		pQuery, sizeof(ReqQueryField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryInstrument(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	CThostFtdcQryInstrumentField body = { 0 };
	strncpy(body.InstrumentID, pQuery->InstrumentID, sizeof(TThostFtdcInstrumentIDType));
	strncpy(body.ExchangeID, pQuery->ExchangeID, sizeof(TThostFtdcExchangeIDType));

	return m_pApi->ReqQryInstrument(&body, ++m_lRequestID);
}

void CTraderApi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo("OnRspQryInstrument", pRspInfo, nRequestID, bIsLast))
	{
		if (pInstrument)
		{
			InstrumentField* pField = (InstrumentField*)m_msgQueue->new_block(sizeof(InstrumentField));

			strncpy(pField->InstrumentID, pInstrument->InstrumentID, sizeof(InstrumentIDType));
			strncpy(pField->ExchangeID, pInstrument->ExchangeID, sizeof(ExchangeIDType));

			strncpy(pField->Symbol, pInstrument->InstrumentID, sizeof(SymbolType));
			strncpy(pField->ProductID, pInstrument->ProductID, sizeof(InstrumentIDType));

			strncpy(pField->InstrumentName, pInstrument->InstrumentName, sizeof(InstrumentNameType));
			pField->Type = CThostFtdcInstrumentField_2_InstrumentType(pInstrument);
			pField->VolumeMultiple = pInstrument->VolumeMultiple;
			pField->PriceTick = pInstrument->PriceTick;
			pField->ExpireDate = str_to_yyyyMMdd(pInstrument->ExpireDate);
			pField->OptionsType = TThostFtdcOptionsTypeType_2_PutCall(pInstrument->OptionsType);
			pField->StrikePrice = (pInstrument->StrikePrice < DBL_EPSILON || pInstrument->StrikePrice == DBL_MAX) ? 0 : pInstrument->StrikePrice;
			strcpy(pField->UnderlyingInstrID, pInstrument->UnderlyingInstrID);

			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryInstrument, m_msgQueue, m_pClass, bIsLast, 0, pField, sizeof(InstrumentField), nullptr, 0, nullptr, 0);
		}
		else
		{
			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryInstrument, m_msgQueue, m_pClass, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}
}

//void CTraderApi::ReqQryInstrumentCommissionRate(const string& szInstrumentId)
//{
//	CThostFtdcQryInstrumentCommissionRateField body = {0};
//
//	strncpy(pBody->BrokerID, m_RspUserLogin.BrokerID,sizeof(TThostFtdcBrokerIDType));
//	strncpy(pBody->InvestorID, m_RspUserLogin.UserID,sizeof(TThostFtdcInvestorIDType));
//	strncpy(pBody->InstrumentID,szInstrumentId.c_str(),sizeof(TThostFtdcInstrumentIDType));
//}
//
//void CTraderApi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//
//}

//void CTraderApi::ReqQryInstrumentMarginRate(const string& szInstrumentId,TThostFtdcHedgeFlagType HedgeFlag)
//{
//	CThostFtdcQryInstrumentMarginRateField body = {0};
//
//	strncpy(pBody->BrokerID, m_RspUserLogin.BrokerID,sizeof(TThostFtdcBrokerIDType));
//	strncpy(pBody->InvestorID, m_RspUserLogin.UserID,sizeof(TThostFtdcInvestorIDType));
//	strncpy(pBody->InstrumentID,szInstrumentId.c_str(),sizeof(TThostFtdcInstrumentIDType));
//	pBody->HedgeFlag = HedgeFlag;
//
//	//AddToSendQueue(pRequest);
//}

//void CTraderApi::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//
//}
//

#ifdef HAS_Settlement
int CTraderApi::_ReqQrySettlementInfo(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	CThostFtdcQrySettlementInfoField body = { 0 };
	strncpy(body.BrokerID, m_RspUserLogin.BrokerID, sizeof(TThostFtdcBrokerIDType));
	strncpy(body.InvestorID, m_RspUserLogin.UserID, sizeof(TThostFtdcInvestorIDType));
	sprintf(body.TradingDay, "%d", pQuery->DateStart);

	return m_pApi->ReqQrySettlementInfo(&body, ++m_lRequestID);
}

vector<CThostFtdcSettlementInfoField *> vct_SettlementInfo;

void CTraderApi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo("OnRspQrySettlementInfo", pRspInfo, nRequestID, bIsLast))
	{
		if (pSettlementInfo)
		{
			CThostFtdcSettlementInfoField* pField = new CThostFtdcSettlementInfoField;
			memcpy(pField, pSettlementInfo, sizeof(CThostFtdcSettlementInfoField));
			vct_SettlementInfo.push_back(pField);

			if (bIsLast)
			{
				int len = 0;
				for (vector<CThostFtdcSettlementInfoField *>::iterator it = vct_SettlementInfo.begin(); it != vct_SettlementInfo.end(); ++it)
				{
					CThostFtdcSettlementInfoField* p = *it;
					len += sizeof(p->Content);
				}

				SettlementInfoField* pField = (SettlementInfoField*)m_msgQueue->new_block(sizeof(SettlementInfoField) + len);
				pField->TradingDay = atoi(pSettlementInfo->TradingDay);
				pField->Size = sizeof(SettlementInfoField) + len;
				char* pContent = (((char*)pField) + sizeof(SettlementInfoField));
				memset(pContent, 0, len);
				for (vector<CThostFtdcSettlementInfoField *>::iterator it = vct_SettlementInfo.begin(); it != vct_SettlementInfo.end(); ++it)
				{
					CThostFtdcSettlementInfoField* p = *it;
					strcat(pContent, p->Content);
				}
				m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQrySettlementInfo, m_msgQueue, this, bIsLast, 0, pField, pField->Size, nullptr, 0, nullptr, 0);

				// 需要将结算单清理
				for (vector<CThostFtdcSettlementInfoField *>::iterator it = vct_SettlementInfo.begin(); it != vct_SettlementInfo.end(); ++it)
				{
					CThostFtdcSettlementInfoField* p = *it;
					delete[] p;
				}
				vct_SettlementInfo.clear();
			}
		}
		else
		{
			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQrySettlementInfo, m_msgQueue, this, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}
}
#endif



void CTraderApi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo("OnRspError", pRspInfo, nRequestID, bIsLast);
}

int CTraderApi::_ReqQryOrder(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	CThostFtdcQryOrderField body = { 0 };

	strcpy(body.BrokerID, m_RspUserLogin.BrokerID);
	strcpy(body.InvestorID, m_RspUserLogin.UserID);

	return m_pApi->ReqQryOrder(&body, ++m_lRequestID);
}

void CTraderApi::OnOrder(CThostFtdcOrderField *pOrder, int nRequestID, bool bIsLast)
{
	if (nullptr == pOrder)
	{
		// 如果是请求报单而当日无报单，也需要回调告知 bIsLast = true
		if (nRequestID != 0)
		{
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryOrder, m_msgQueue, m_pClass, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
		return;
	}

	OrderIDType orderId = { 0 };
	sprintf(orderId, "%d:%d:%s", pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef);
	OrderIDType orderSydId = { 0 };

	{
		// 保存原始订单信息，用于撤单

		unordered_map<string, CThostFtdcOrderField*>::iterator it = m_id_api_order.find(orderId);
		if (it == m_id_api_order.end())
		{
			// 找不到此订单，表示是新单
			CThostFtdcOrderField* pField = (CThostFtdcOrderField*)m_msgQueue->new_block(sizeof(CThostFtdcOrderField));
			memcpy(pField, pOrder, sizeof(CThostFtdcOrderField));
			m_id_api_order.insert(pair<string, CThostFtdcOrderField*>(orderId, pField));
		}
		else
		{
			// 找到了订单
			// 需要再复制保存最后一次的状态，还是只要第一次的用于撤单即可？记下，这样最后好比较
			CThostFtdcOrderField* pField = it->second;
			memcpy(pField, pOrder, sizeof(CThostFtdcOrderField));
		}

		// 保存SysID用于定义成交回报与订单
		sprintf(orderSydId, "%s:%s:%s", pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID);
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
				pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));

				// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
				CThostFtdcOrderField_2_OrderField_0(orderId, pOrder, pField);
				pField->Time = str_to_HHmmss(pOrder->InsertTime);

				// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
				m_id_platform_order.insert(pair<string, OrderField*>(orderId, pField));
			}
			else
			{
				pField = it->second;
				strcpy(pField->ID, orderId);
				strcpy(pField->LocalID, pField->ID);
				// 为了解决发送时交易所与接收时交易所不一样的情况
				strcpy(pField->ExchangeID, pOrder->ExchangeID);
				pField->LeavesQty = pOrder->VolumeTotal;
				pField->Price = pOrder->LimitPrice;
				pField->Status = CThostFtdcOrderField_2_OrderStatus(pOrder);
				pField->ExecType = CThostFtdcOrderField_2_ExecType(pOrder);
				strcpy(pField->OrderID, pOrder->OrderSysID);
				strncpy(pField->Text, pOrder->StatusMsg, sizeof(Char256Type));
				//pField->Time = GetTime(pOrder->InsertTime);
			}

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
		else
		{
			OrderField* pField = nullptr;
			pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));

			// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
			CThostFtdcOrderField_2_OrderField_0(orderId, pOrder, pField);
			pField->Time = str_to_HHmmss(pOrder->InsertTime);

			// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
			//m_id_platform_order.insert(pair<string, OrderField*>(orderId, pField));

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryOrder, m_msgQueue, m_pClass, bIsLast, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);

			m_msgQueue->delete_block(pField);
		}
	}
}

void CTraderApi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo("OnRspQryOrder", pRspInfo, nRequestID, bIsLast))
	{
		OnOrder(pOrder, nRequestID, bIsLast);
	}
}

//void CTraderApi::ReqQryTrade()
//{
//	CThostFtdcQryTradeField* pBody = (CThostFtdcQryTradeField*)m_msgQueue_Query->new_block(sizeof(CThostFtdcQryTradeField));
//
//	strncpy(pBody->BrokerID, m_RspUserLogin.BrokerID, sizeof(TThostFtdcBrokerIDType));
//	strncpy(pBody->InvestorID, m_RspUserLogin.UserID, sizeof(TThostFtdcInvestorIDType));
//
//	m_msgQueue_Query->Input_NoCopy(RequestType::E_QryTradeField, m_msgQueue_Query, this, 0, 0,
//		pBody, sizeof(CThostFtdcQryTradeField), nullptr, 0, nullptr, 0);
//}

int CTraderApi::_ReqQryTrade(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	CThostFtdcQryTradeField body = { 0 };

	strcpy(body.BrokerID, m_RspUserLogin.BrokerID);
	strcpy(body.InvestorID, m_RspUserLogin.UserID);

	return m_pApi->ReqQryTrade(&body, ++m_lRequestID);
}

void CTraderApi::OnTrade(CThostFtdcTradeField *pTrade, int nRequestID, bool bIsLast)
{
	if (nullptr == pTrade)
	{
		if (nRequestID != 0)
		{
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryTrade, m_msgQueue, m_pClass, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
		return;
	}

	TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));
	strcpy(pField->InstrumentID, pTrade->InstrumentID);
	strcpy(pField->ExchangeID, pTrade->ExchangeID);
	strcpy(pField->AccountID, pTrade->InvestorID);
	pField->Side = TThostFtdcDirectionType_2_OrderSide(pTrade->Direction);
	pField->Qty = pTrade->Volume;
	pField->Price = pTrade->Price;
	pField->OpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pTrade->OffsetFlag);
	pField->HedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pTrade->HedgeFlag);
	pField->Commission = 0;//TODO收续费以后要计算出来
	pField->Time = str_to_HHmmss(pTrade->TradeTime);
	strcpy(pField->TradeID, pTrade->TradeID);

	if (nRequestID == 0)
	{
		OrderIDType orderSysId = { 0 };
		sprintf(orderSysId, "%s:%s:%s", pTrade->ExchangeID, pTrade->InstrumentID, pTrade->OrderSysID);
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

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnTrade, m_msgQueue, m_pClass, bIsLast, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);

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

			// 实时根据本地持仓进行计算
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
		pTrade->ExchangeID, pTrade->InstrumentID, TradeField_2_PositionSide(pTrade), pTrade->HedgeFlag);

	PositionField* pField = nullptr;
	unordered_map<string, PositionField*>::iterator it = m_id_platform_position.find(positionId);
	if (it == m_id_platform_position.end())
	{
		pField = (PositionField*)m_msgQueue->new_block(sizeof(PositionField));

		strcpy(pField->Symbol, pTrade->InstrumentID);
		strcpy(pField->InstrumentID, pTrade->InstrumentID);
		strcpy(pField->ExchangeID, pTrade->ExchangeID);
		strcpy(pField->AccountID, pTrade->AccountID);
		pField->Side = TradeField_2_PositionSide(pTrade);
		pField->HedgeFlag = pTrade->HedgeFlag;

		m_id_platform_position.insert(pair<string, PositionField*>(positionId, pField));
	}
	else
	{
		pField = it->second;
	}

	if (pTrade->OpenClose == OpenCloseType::OpenCloseType_Open)
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
			if (pField->HistoryPosition < 0)
			{
				pField->TodayPosition += pField->HistoryPosition;
				pField->HistoryPosition = 0;
			}
		}

		// 计算错误，直接重新查询
		if (pField->Position < 0 || pField->TodayPosition < 0 || pField->HistoryPosition < 0)
		{
			//ReqQryInvestorPosition("", "");
			ReqQueryField body = { 0 };
			ReqQuery(QueryType::QueryType_ReqQryInvestorPosition, &body);
			return;
		}
	}

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, false, 0, pField, sizeof(PositionField), nullptr, 0, nullptr, 0);
}

void CTraderApi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo("OnRspQryTrade", pRspInfo, nRequestID, bIsLast))
	{
		OnTrade(pTrade, nRequestID, bIsLast);
	}
}

//void CTraderApi::ReqQryQuote()
//{
//	CThostFtdcQryQuoteField* pBody = (CThostFtdcQryQuoteField*)m_msgQueue_Query->new_block(sizeof(CThostFtdcQryQuoteField));
//
//	strncpy(pBody->BrokerID, m_RspUserLogin.BrokerID, sizeof(TThostFtdcBrokerIDType));
//	strncpy(pBody->InvestorID, m_RspUserLogin.UserID, sizeof(TThostFtdcInvestorIDType));
//
//	m_msgQueue_Query->Input_NoCopy(RequestType::E_QryQuoteField, m_msgQueue_Query, this, 0, 0,
//		pBody, sizeof(CThostFtdcQryQuoteField), nullptr, 0, nullptr, 0);
//}

#ifdef HAS_InstrumentStatus
void CTraderApi::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus)
{
	if (pInstrumentStatus)
	{
		InstrumentStatusField* pField = (InstrumentStatusField*)m_msgQueue->new_block(sizeof(InstrumentStatusField));

		strcpy(pField->InstrumentID, pInstrumentStatus->InstrumentID);
		strcpy(pField->ExchangeID, pInstrumentStatus->ExchangeID);
		sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, pField->ExchangeID);

		pField->InstrumentStatus = TThostFtdcInstrumentStatusType_2_TradingPhaseType(pInstrumentStatus->InstrumentStatus);
		pField->EnterTime = str_to_HHmmss(pInstrumentStatus->EnterTime);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnInstrumentStatus, m_msgQueue, m_pClass, true, 0, pField, sizeof(InstrumentStatusField), nullptr, 0, nullptr, 0);
	}
}
#endif // HAS_InstrumentStatus


int CTraderApi::_ReqQryInvestor(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	CThostFtdcQryInvestorField body = { 0 };

	strcpy(body.BrokerID, m_RspUserLogin.BrokerID);
	strcpy(body.InvestorID, m_RspUserLogin.UserID);

	return m_pApi->ReqQryInvestor(&body, ++m_lRequestID);
}

void CTraderApi::OnRspQryInvestor(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo("OnRspQryInvestor", pRspInfo, nRequestID, bIsLast))
	{
		if (pInvestor)
		{
			memcpy(&m_Investor, pInvestor, sizeof(CThostFtdcInvestorField));

			InvestorField* pField = (InvestorField*)m_msgQueue->new_block(sizeof(InvestorField));

			strcpy(pField->BrokerID, pInvestor->BrokerID);
			strcpy(pField->InvestorID, pInvestor->InvestorID);
			strcpy(pField->InvestorName, pInvestor->InvestorName);
			strcpy(pField->IdentifiedCardNo, pInvestor->IdentifiedCardNo);
			pField->IdentifiedCardType = TThostFtdcIdCardTypeType_2_IdCardType(pInvestor->IdentifiedCardType);

			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryInvestor, m_msgQueue, m_pClass, bIsLast, 0, pField, sizeof(InvestorField), nullptr, 0, nullptr, 0);

#ifdef ENABLE_LICENSE
			// 这里不使用pInvestor->InvestorID，因为有可能股东代码与登录账号不一样
			if (m_pLicense->HasSaved())
			{
				m_pLicense->AddUser(m_UserInfo.UserID, pField->InvestorName);
				m_pLicense->SaveIni();
			}

			m_pLicense->SetSendOrderFlag(true);
			int err = m_pLicense->GetErrorCodeByNameThenAccount(pField->InvestorName, m_UserInfo.UserID);
			if (err < 0)
			{
				m_pLicense->SetSendOrderFlag(false);

				RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
				pField->RawErrorID = err;
				strncpy(pField->Text, m_pLicense->GetErrorInfo(), sizeof(Char256Type));

				m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

				//IsErrorRspInfo("")
			}
#endif
		}
		else
		{
			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryInvestor, m_msgQueue, m_pClass, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
	}
}

#ifdef HAS_Quote
char* CTraderApi::ReqQuoteInsert(
	QuoteField* pQuote,
	OrderIDType* pAskRef,
	OrderIDType* pBidRef)
{
	int QuoteRef = -1;
	if (nullptr == m_pApi)
		return nullptr;

	CThostFtdcInputQuoteField body = { 0 };

	strcpy(body.BrokerID, m_RspUserLogin.BrokerID);
	strcpy(body.InvestorID, m_RspUserLogin.UserID);

	//合约,目前只从订单1中取
	strncpy(body.InstrumentID, pQuote->InstrumentID, sizeof(TThostFtdcInstrumentIDType));

#ifdef HAS_ExchangeID_Quote
	strncpy(body.ExchangeID, pQuote->ExchangeID, sizeof(TThostFtdcExchangeIDType));
#endif // HAS_ExchangeID

	//开平
	body.AskOffsetFlag = OpenCloseType_2_TThostFtdcOffsetFlagType(pQuote->AskOpenClose);
	body.BidOffsetFlag = OpenCloseType_2_TThostFtdcOffsetFlagType(pQuote->BidOpenClose);
	//投保
	body.AskHedgeFlag = HedgeFlagType_2_TThostFtdcHedgeFlagType(pQuote->AskHedgeFlag);
	body.BidHedgeFlag = HedgeFlagType_2_TThostFtdcHedgeFlagType(pQuote->BidHedgeFlag);

	//价格
	body.AskPrice = pQuote->AskPrice;
	body.BidPrice = pQuote->BidPrice;

	//数量
	body.AskVolume = (int)pQuote->AskQty;
	body.BidVolume = (int)pQuote->BidQty;
#ifdef HAS_ExchangeID_Quote_ForQuoteSysID
	strncpy(body.ForQuoteSysID, pQuote->QuoteReqID, sizeof(TThostFtdcOrderSysIDType));
#endif // HAS_ExchangeID_Quote_ForQuoteSysID



	int nRet = 0;
	{
		//可能报单太快，m_nMaxOrderRef还没有改变就提交了
		lock_guard<mutex> cl(m_csOrderRef);

		if (QuoteRef < 0)
		{
			nRet = m_nMaxOrderRef;
			sprintf(body.QuoteRef, "%d", m_nMaxOrderRef);
			sprintf(body.AskOrderRef, "%d", m_nMaxOrderRef);
			sprintf(body.BidOrderRef, "%d", ++m_nMaxOrderRef);
			++m_nMaxOrderRef;
		}
		else
		{
			nRet = QuoteRef;
			sprintf(body.QuoteRef, "%d", QuoteRef);
			sprintf(body.AskOrderRef, "%d", QuoteRef);
			sprintf(body.BidOrderRef, "%d", ++QuoteRef);
			++QuoteRef;
		}

		//不保存到队列，而是直接发送
		int n = m_pApi->ReqQuoteInsert(&body, ++m_lRequestID);
		if (n < 0)
		{
			nRet = n;
			return nullptr;
		}
		else
		{
			sprintf(m_orderInsert_Id, "%d:%d:%d", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, nRet);

			QuoteField* pField = (QuoteField*)m_msgQueue->new_block(sizeof(QuoteField));
			memcpy(pField, pQuote, sizeof(QuoteField));
			strcpy(pField->ID, m_orderInsert_Id);
			strcpy(pField->LocalID, pField->ID);
			strcpy(pField->AskID, m_orderInsert_Id);
			sprintf(pField->BidID, "%d:%d:%d", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, nRet + 1);

			m_id_platform_quote.insert(pair<string, QuoteField*>(m_orderInsert_Id, pField));
		}
	}

	return m_orderInsert_Id;
}

void CTraderApi::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OrderIDType quoteId = { 0 };

	if (pInputQuote)
	{
		sprintf(quoteId, "%d:%d:%s", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, pInputQuote->QuoteRef);
	}
	else
	{
		IsErrorRspInfo("OnRspQuoteInsert", pRspInfo, nRequestID, bIsLast);
	}

	unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.find(quoteId);
	if (it == m_id_platform_quote.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		QuoteField* pField = it->second;
		pField->ExecType = ExecType::ExecType_Rejected;
		pField->Status = OrderStatus::OrderStatus_Rejected;
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnQuote, m_msgQueue, m_pClass, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnErrRtnQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo)
{
	OrderIDType quoteId = { 0 };

	if (pInputQuote)
	{
		sprintf(quoteId, "%d:%d:%s", m_RspUserLogin.FrontID, m_RspUserLogin.SessionID, pInputQuote->QuoteRef);
	}
	else
	{
		IsErrorRspInfo("", pRspInfo, 0, true);
	}

	unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.find(quoteId);
	if (it == m_id_platform_quote.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		QuoteField* pField = it->second;
		pField->ExecType = ExecType::ExecType_Rejected;
		pField->Status = OrderStatus::OrderStatus_Rejected;
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnQuote, m_msgQueue, m_pClass, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRtnQuote(CThostFtdcQuoteField *pQuote)
{
	OnQuote(pQuote, 0, true);
}

int CTraderApi::ReqQuoteAction(const string& szId, OrderIDType* pOutput)
{
	unordered_map<string, CThostFtdcQuoteField*>::iterator it = m_id_api_quote.find(szId);
	if (it == m_id_api_quote.end())
	{
		sprintf((char*)pOutput, "%d", -100);
		return -100;
	}
	else
	{
		// 找到了订单
		ReqQuoteAction(it->second, pOutput);
	}
	return 0;
}

int CTraderApi::ReqQuoteAction(CThostFtdcQuoteField *pQuote, OrderIDType* pOutput)
{
	if (nullptr == m_pApi)
		return 0;

	CThostFtdcInputQuoteActionField body = { 0 };

	///经纪公司代码
	strcpy(body.BrokerID, pQuote->BrokerID);
	///投资者代码
	strcpy(body.InvestorID, pQuote->InvestorID);
	///报单引用
	strcpy(body.QuoteRef, pQuote->QuoteRef);
	///前置编号
	body.FrontID = pQuote->FrontID;
	///会话编号
	body.SessionID = pQuote->SessionID;
	///交易所代码
	strcpy(body.ExchangeID, pQuote->ExchangeID);
	///报单编号
	strcpy(body.QuoteSysID, pQuote->QuoteSysID);
	///操作标志
	body.ActionFlag = THOST_FTDC_AF_Delete;
	///合约代码
	strcpy(body.InstrumentID, pQuote->InstrumentID);

	int nRet = m_pApi->ReqQuoteAction(&body, ++m_lRequestID);
	if (nRet < 0)
	{
		sprintf(m_orderAction_Id, "%d", nRet);
	}
	else
	{
		memset(m_orderAction_Id, 0, sizeof(OrderIDType));
	}
	strncpy((char*)pOutput, m_orderAction_Id, sizeof(OrderIDType));

	return nRet;
}

void CTraderApi::OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OrderIDType quoteId = { 0 };
	if (pInputQuoteAction)
	{
		sprintf(quoteId, "%d:%d:%s", pInputQuoteAction->FrontID, pInputQuoteAction->SessionID, pInputQuoteAction->QuoteRef);
	}
	else
	{
		IsErrorRspInfo("OnRspQuoteAction", pRspInfo, nRequestID, bIsLast);
	}

	unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.find(quoteId);
	if (it == m_id_platform_quote.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		QuoteField* pField = it->second;
		strcpy(pField->ID, quoteId);
		strcpy(pField->LocalID, pField->ID);
		//sprintf(pField->AskID, "%d:%d:%s", pInputQuoteAction->FrontID, pInputQuoteAction->SessionID, pInputQuoteAction->);
		//sprintf(pField->BidID, "%d:%d:%s", pInputQuoteAction->FrontID, pInputQuoteAction->SessionID, pInputQuoteAction->QuoteRef);
		pField->ExecType = ExecType::ExecType_CancelReject;
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnQuote, m_msgQueue, m_pClass, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnErrRtnQuoteAction(CThostFtdcQuoteActionField *pQuoteAction, CThostFtdcRspInfoField *pRspInfo)
{
	OrderIDType quoteId = { 0 };

	if (pQuoteAction)
	{
		sprintf(quoteId, "%d:%d:%s", pQuoteAction->FrontID, pQuoteAction->SessionID, pQuoteAction->QuoteRef);
	}
	else
	{
		IsErrorRspInfo("OnRspQuoteAction", pRspInfo, 0, true);
	}

	unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.find(quoteId);
	if (it == m_id_platform_quote.end())
	{
		// 没找到？不应当，这表示出错了
		//assert(false);
	}
	else
	{
		// 找到了，要更新状态
		// 得使用上次的状态
		QuoteField* pField = it->second;
		strcpy(pField->ID, quoteId);
		strcpy(pField->LocalID, pField->ID);
		pField->ExecType = ExecType::ExecType_CancelReject;
		pField->RawErrorID = pRspInfo->ErrorID;
		strncpy(pField->Text, pRspInfo->ErrorMsg, sizeof(Char256Type));
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnQuote, m_msgQueue, m_pClass, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
	}
}

void CTraderApi::OnRspQryQuote(CThostFtdcQuoteField *pQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo("OnRspQryQuote", pRspInfo, nRequestID, bIsLast))
	{
		OnQuote(pQuote, nRequestID, bIsLast);
	}
}

int CTraderApi::_ReqQryQuote(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	CThostFtdcQryQuoteField body = { 0 };

	strcpy(body.BrokerID, m_RspUserLogin.BrokerID);
	strcpy(body.InvestorID, m_RspUserLogin.UserID);

	return m_pApi->ReqQryQuote(&body, ++m_lRequestID);
}

void CTraderApi::OnQuote(CThostFtdcQuoteField *pQuote, int nRequestID, bool bIsLast)
{
	if (nullptr == pQuote)
	{
		if (nRequestID != 0)
		{
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryQuote, m_msgQueue, m_pClass, bIsLast, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		}
		return;
	}

	OrderIDType quoteId = { 0 };
	sprintf(quoteId, "%d:%d:%s", pQuote->FrontID, pQuote->SessionID, pQuote->QuoteRef);
	OrderIDType orderSydId = { 0 };

	{
		// 保存原始订单信息，用于撤单

		unordered_map<string, CThostFtdcQuoteField*>::iterator it = m_id_api_quote.find(quoteId);
		if (it == m_id_api_quote.end())
		{
			// 找不到此订单，表示是新单
			CThostFtdcQuoteField* pField = new CThostFtdcQuoteField();
			memcpy(pField, pQuote, sizeof(CThostFtdcQuoteField));
			m_id_api_quote.insert(pair<string, CThostFtdcQuoteField*>(quoteId, pField));
		}
		else
		{
			// 找到了订单
			// 需要再复制保存最后一次的状态，还是只要第一次的用于撤单即可？记下，这样最后好比较
			CThostFtdcQuoteField* pField = it->second;
			memcpy(pField, pQuote, sizeof(CThostFtdcQuoteField));
		}

		// 这个地方是否要进行其它处理？

		// 保存SysID用于定义成交回报与订单
		//sprintf(orderSydId, "%s:%s", pQuote->ExchangeID, pQuote->QuoteSysID);
		//m_sysId_quoteId.insert(pair<string, string>(orderSydId, quoteId));
	}

	{
		// 从API的订单转换成自己的结构体

		if (nRequestID == 0)
		{
			QuoteField* pField = nullptr;
			unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.find(quoteId);
			if (it == m_id_platform_quote.end())
			{
				// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
				pField = (QuoteField*)m_msgQueue->new_block(sizeof(QuoteField));

				strcpy(pField->InstrumentID, pQuote->InstrumentID);
				strcpy(pField->ExchangeID, pQuote->ExchangeID);
				strcpy(pField->AccountID, pQuote->InvestorID);

				pField->AskQty = pQuote->AskVolume;
				pField->AskPrice = pQuote->AskPrice;
				pField->AskOpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pQuote->AskOffsetFlag);
				pField->AskHedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pQuote->AskHedgeFlag);

				pField->BidQty = pQuote->BidVolume;
				pField->BidPrice = pQuote->BidPrice;
				pField->BidOpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pQuote->BidOffsetFlag);
				pField->BidHedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pQuote->BidHedgeFlag);

				strcpy(pField->ID, quoteId);
				strcpy(pField->LocalID, pField->ID);
				strcpy(pField->AskOrderID, pQuote->AskOrderSysID);
				strcpy(pField->BidOrderID, pQuote->BidOrderSysID);

				strncpy(pField->Text, pQuote->StatusMsg, sizeof(Char256Type));

				//pField->ExecType = ExecType::ExecNew;
				pField->Status = CThostFtdcQuoteField_2_OrderStatus(pQuote);
				pField->ExecType = ExecType::ExecType_New;


				// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
				m_id_platform_quote.insert(pair<string, QuoteField*>(quoteId, pField));
			}
			else
			{
				pField = it->second;

				strcpy(pField->ID, quoteId);
				strcpy(pField->LocalID, pField->ID);
				strcpy(pField->AskOrderID, pQuote->AskOrderSysID);
				strcpy(pField->BidOrderID, pQuote->BidOrderSysID);

				pField->Status = CThostFtdcQuoteField_2_OrderStatus(pQuote);
				pField->ExecType = CThostFtdcQuoteField_2_ExecType(pQuote);

				strncpy(pField->Text, pQuote->StatusMsg, sizeof(Char256Type));
			}

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnQuote, m_msgQueue, m_pClass, 0, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);
		}
		else
		{
			QuoteField* pField = nullptr;
			unordered_map<string, QuoteField*>::iterator it = m_id_platform_quote.find(quoteId);
			if (it == m_id_platform_quote.end())
			{
				// 开盘时发单信息还没有，所以找不到对应的单子，需要进行Order的恢复
				pField = (QuoteField*)m_msgQueue->new_block(sizeof(QuoteField));

				strcpy(pField->InstrumentID, pQuote->InstrumentID);
				strcpy(pField->ExchangeID, pQuote->ExchangeID);
				strcpy(pField->AccountID, pQuote->InvestorID);

				pField->AskQty = pQuote->AskVolume;
				pField->AskPrice = pQuote->AskPrice;
				pField->AskOpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pQuote->AskOffsetFlag);
				pField->AskHedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pQuote->AskHedgeFlag);

				pField->BidQty = pQuote->BidVolume;
				pField->BidPrice = pQuote->BidPrice;
				pField->BidOpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pQuote->BidOffsetFlag);
				pField->BidHedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pQuote->BidHedgeFlag);

				strcpy(pField->ID, quoteId);
				strcpy(pField->LocalID, pField->ID);
				strcpy(pField->AskOrderID, pQuote->AskOrderSysID);
				strcpy(pField->BidOrderID, pQuote->BidOrderSysID);

				strncpy(pField->Text, pQuote->StatusMsg, sizeof(Char256Type));

				//pField->ExecType = ExecType::ExecNew;
				pField->Status = CThostFtdcQuoteField_2_OrderStatus(pQuote);
				pField->ExecType = CThostFtdcQuoteField_2_ExecType(pQuote);
				strncpy(pField->Text, pQuote->StatusMsg, sizeof(Char256Type));


				// 添加到map中，用于其它工具的读取，撤单失败时的再通知等
				//m_id_platform_quote.insert(pair<string, QuoteField*>(quoteId, pField));

				m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryQuote, m_msgQueue, m_pClass, bIsLast, 0, pField, sizeof(QuoteField), nullptr, 0, nullptr, 0);

				m_msgQueue->delete_block(pField);
			}
		}
	}
}
#endif // HAS_Quote