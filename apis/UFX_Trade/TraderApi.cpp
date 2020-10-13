#include "stdafx.h"
#include "TraderApi.h"

#include "../../include/QueueEnum.h"

#include "../../include/ApiHeader.h"
#include "../../include/ApiStruct.h"

#include "../../include/ApiProcess.h"
#include "../../include/toolkit.h"
#include "../../include/IDGenerator.h"

#include "../../include/queue/MsgQueue.h"

#include "O32/O32Codec.h"
#include "O32/O32Processor.h"
#include "O32/TypeConvert_o32.h"
#include "OrderMap.h"

#include <cstring>
#include <assert.h>
#include <cfloat>

#ifdef ENABLE_LICENSE
#include "../../common/License/License.h"
#endif

#include "Log.h"

#pragma warning(disable:4996)

char buf[1024] = { 0 };

void PrintUnPack(IF2UnPacker* lpUnPack)
{
	printf("记录行数：           %d\n", lpUnPack->GetRowCount());
	printf("列行数：			 %d\n", lpUnPack->GetColCount());
	while (!lpUnPack->IsEOF())
	{
		for (int i = 0; i < lpUnPack->GetColCount(); i++)
		{
			char* colName = (char*)lpUnPack->GetColName(i);
			char colType = lpUnPack->GetColType(i);
			if (colType != 'R')
			{
				char* colValue = (char*)lpUnPack->GetStrByIndex(i);
				printf("%s:			[%s]\n", colName, colValue);
			}
			else
			{
				int colLength = 0;
				char* colValue = (char*)lpUnPack->GetRawByIndex(i, &colLength);
				printf("%s:			[%s](%d)\n", colName, colValue, colLength);
			}
		}
		lpUnPack->Next();
	}

}

void PrintSub(int subIndex, LPSUBSCRIBE_RECVDATA lpRecvData)
{
	//map<int, CSubscribeParamInterface*>::iterator itr = g_allSubscribeParam.find(subIndex);
	//if (itr == g_allSubscribeParam.end())
	//{
	//	printf("没有这个订阅项\n");
	//	return;
	//}
	//CSubscribeParamInterface* lpSubParam = (*itr).second;

	//printf("----------订阅项部分-------\n");
	//printf("主题名字：           %s\n", lpSubParam->GetTopicName());
	//printf("附加数据长度：       %d\n", lpRecvData->iAppDataLen);
	//if (lpRecvData->iAppDataLen>0)
	//{
	//	printf("附加数据：           %s\n", lpRecvData->lpAppData);
	//}
	printf("过滤字段部分：\n");
	if (lpRecvData->iFilterDataLen > 0)
	{
		IF2UnPacker* lpUnpack = NewUnPacker(lpRecvData->lpFilterData, lpRecvData->iFilterDataLen);
		lpUnpack->AddRef();
		PrintUnPack(lpUnpack);
		lpUnpack->Release();
	}

	printf("---------------------------\n");

}

void ShowPacket(IF2UnPacker *lpUnPacker)
{
	int i = 0, t = 0, j = 0, k = 0;

	for (i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
	{
		// 设置当前结果集

		sprintf(buf, "记录集：%d/%d\r\n", i + 1, lpUnPacker->GetDatasetCount());
		LOG(buf);
		lpUnPacker->SetCurrentDatasetByIndex(i);

		// 打印所有记录
		for (j = 0; j < (int)lpUnPacker->GetRowCount(); ++j)
		{
			sprintf(buf, "\t第%d/%d条记录：\r\n", j + 1, lpUnPacker->GetRowCount());
			LOG(buf);
			// 打印每条记录
			for (k = 0; k < lpUnPacker->GetColCount(); ++k)
			{
				switch (lpUnPacker->GetColType(k))
				{
				case 'I':
					sprintf(buf, "\t【整数】%20s = %35d - %10d\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetIntByIndex(k), lpUnPacker->GetColWidth(k));
					LOG(buf);
					break;

				case 'C':
					sprintf(buf, "\t【字符】%20s = %35c - %10d\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetCharByIndex(k), lpUnPacker->GetColWidth(k));
					LOG(buf);
					break;

				case 'S':

					if (NULL != strstr((char *)lpUnPacker->GetColName(k), "password"))
					{
						sprintf(buf, "\t【字串】%20s = %35s - %10d\r\n", lpUnPacker->GetColName(k), "******", lpUnPacker->GetColWidth(k));
						LOG(buf);
					}
					else
					{
						sprintf(buf, "\t【字串】%20s = %35s - %10d\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetStrByIndex(k), lpUnPacker->GetColWidth(k));
						LOG(buf);
					}
					break;

				case 'F':
					sprintf(buf, "\t【数值】%20s = %35f - %10d\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetDoubleByIndex(k), lpUnPacker->GetColWidth(k));
					LOG(buf);
					break;

				case 'R':
				{
					int nLength = 0;
					void *lpData = lpUnPacker->GetRawByIndex(k, &nLength);
					switch (nLength) {
					case 0:
						sprintf(buf, "\t【数据】%20s = %35s - %10d\r\n", lpUnPacker->GetColName(k), "(N/A)", lpUnPacker->GetColWidth(k));
						LOG(buf);
						break;
					default:
						sprintf(buf, "\t【数据】%20s = 0x", lpUnPacker->GetColName(k));
						LOG(buf);
						for (t = nLength; t < 11; t++) {
							printf("   ");
							//LOG(buf);
						}
						unsigned char *p = (unsigned char *)lpData;
						for (t = 0; t < nLength; t++) {
							printf("%3x", *p++);
							//LOG(buf);
						}
						sprintf(buf, "\r\n");
						LOG(buf);
						break;
					}
					// 对2进制数据进行处理
					break;
				}

				default:
					// 未知数据类型
					sprintf(buf, "未知数据类型。\n");
					LOG(buf);
					break;
				}
			}

			sprintf(buf, "\n");
			LOG(buf);

			lpUnPacker->Next();
		}

		sprintf(buf, "\n");
		LOG(buf);
	}
}

void* __stdcall Recv(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CTraderApi* pApi = (CTraderApi*)pApi2;
	pApi->RecvInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

void CTraderApi::RecvInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	switch (type)
	{
	case E_OnReceivedBizMsg:
		// 初始化，登录交易与消息中心
		iRet = _OnReceivedBizMsg(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_OnReceived:
		// 初始化，登录交易与消息中心
		iRet = _OnReceived(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	}



	if (0 == iRet)
	{
		//返回成功，填加到已发送池
		m_nSleep = 1;
	}
	else
	{
		m_msgQueue_Recv->Input_Copy(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		//失败，按4的幂进行延时，但不超过1s
		m_nSleep *= 4;
		m_nSleep %= 1023;
	}

	this_thread::sleep_for(chrono::milliseconds(m_nSleep));
}

int CTraderApi::_OnReceivedBizMsg(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	IBizMessage* lpMsg = NewBizMessage();
	lpMsg->AddRef();

	lpMsg->SetBuff(ptr1, size1);

	int packetId = lpMsg->GetPacketId();
	int iLen = 0;
	const void * lpBuffer = lpMsg->GetContent(iLen);
	IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, iLen);

	if (lpUnPacker == nullptr)
	{
		printf("_OnReceivedBizMsg,lpUnPacker==nullptr");
		return 0;
	}

	int iLen_ley = 0;
	const void * lpBuffer_key = lpMsg->GetKeyInfo(iLen_ley);
	IF2UnPacker * lpUnPacker_key = NewUnPacker((void *)lpBuffer_key, iLen_ley);

	int functionId = lpMsg->GetFunction();
	RSP_ErrorField3 err = { 0 };
	sprintf(buf, "=======%d========\r\n", functionId);
	LOG(buf);
	if (functionId == 91004 || functionId == 91105)
	{
		// 能处理的包，就前前面处理了，不能处理的包，在这里打印出来
		ShowPacket(lpUnPacker);
	}
	if (m_pO32Codec)
	{
		int nRet = m_pO32Codec->OnResponse(functionId, packetId, lpUnPacker, lpUnPacker_key, &m_RspUserLogin_10001, &err);
	}


	if (lpUnPacker)
		lpUnPacker->Release();
	if (lpUnPacker_key)
		lpUnPacker_key->Release();

	lpMsg->Release();

	return 0;
}

int CTraderApi::_OnReceived(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	IF2UnPacker* lpUnPack = NewUnPacker(ptr1, size1);
	if (lpUnPack)
	{
		lpUnPack->AddRef();
		// 打印后再调整，以后能处理的情况下就得屏蔽了

		// 不能放后面只能放前面
		ShowPacket(lpUnPack);
		if (m_pO32Codec)
		{
			int nRet = m_pO32Codec->OnSub(lpUnPack);
			if (nRet != 0)
			{
				//ShowPacket(lpUnPack);
			}
		}

		lpUnPack->Release();
	}
	return 0;
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
	case E_ReqSub:
		// 初始化，登录交易与消息中心
		iRet = _Sub();
		break;
	}

	if (m_pO32Codec)
	{
		switch (type)
		{
		case E_ReqUserLoginField:
			iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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
		case QueryType::QueryType_ReqQryOrder:
			iRet = _ReqQryOrder(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			break;
		case QueryType::QueryType_ReqQryTrade:
			iRet = _ReqQryTrade(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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

	// 将时定时查询功能放在查询队列中，这样退出就不会出问题了
	// 由于队列中一直没有东西，所以不会再来触发这个循环，需要隔一定的时间向队列中放一个，最好放在最后
	TestInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	if (m_msgQueue_Query->Size() == 0)
	{
		// 数据为空时，向里插入数据，保证循环
		m_msgQueue_Query->Input_Copy(E_Heartbeat, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		m_nSleep = 32;
	}

	this_thread::sleep_for(chrono::milliseconds(m_nSleep));
}

void CTraderApi::TestInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	time_t _now = time(nullptr);

	CheckThenHeartbeat(_now);
}

void CTraderApi::CheckThenHeartbeat(time_t _now)
{
	if (_now > m_HeartbeatTime)
	{
		// 心跳超时默认是30s
		double _queryTime = 20;
		m_HeartbeatTime = time(nullptr) + _queryTime;

		ReqHeartbeat();
	}
}

int CTraderApi::ReqHeartbeat()
{
	// 可能开始就发心跳，还没登录呢
	// 重连时，可能消息中心登录了，但交易还没有，所以通过心跳再连上
	if (m_lpConnection == nullptr)
		return 0;

	int status = m_lpConnection->GetStatus();
	if (status & CConnectionInterface::Registered)
	{
	}
	else
	{
		if (m_lpMCConnection == nullptr)
			return 0;
		int status_mc = m_lpMCConnection->GetStatus();
		if (status_mc & CConnectionInterface::Registered)
		{
		}
		else
		{
			return 0;
		}
	}

	REQ_XT_10000 req = { 0 };
	SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
	int iRet = m_pO32Codec->ReqFunction_10000(&req);
	return 0;
}

void CTraderApi::Register(void* pCallback, void* pClass)
{
	m_pClass = pClass;
	if (m_msgQueue == nullptr)
		return;

	m_msgQueue_Query->Register(Query);
	m_msgQueue->Register((fnOnResponse)pCallback);
	m_msgQueue_Recv->Register(Recv);
	if (pCallback)
	{
		m_msgQueue_Query->StartThread();
		m_msgQueue->StartThread();
		m_msgQueue_Recv->StartThread();
	}
	else
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue->StopThread();
		m_msgQueue_Recv->StopThread();
	}
}


CTraderApi::CTraderApi(void)
{
	// 弹出这个只是为了调试，以后可能要关闭
	AllocConsole();
	freopen("CON", "r", stdin);
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	::DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);

	m_lpConfig = nullptr;
	m_lpConnection = nullptr;

	m_lpMCConfig = nullptr;
	m_lpMCConnection = nullptr;
	m_lpSub = nullptr;

	memset(&m_RspUserLogin_10001, 0, sizeof(RSP_DL_10001));

	/*memset(&m_ReqUserLogin_331100, 0, sizeof(REQ_DL_331100));
	memset(&m_RspUserLogin_331100, 0, sizeof(RSP_DL_331100));
	memset(&m_ReqUserLogin_10001, 0, sizeof(REQ_DL_10001));



	m_ReqUserLogin_331100.op_branch_no = 0;
	m_ReqUserLogin_331100.op_entrust_way = op_entrust_way_WSWT;
	SAFE_STRNCPY(m_ReqUserLogin_331100.op_station,"0");
	m_ReqUserLogin_331100.branch_no = 1;
	m_ReqUserLogin_331100.input_content = input_content_ZJZH;
	SAFE_STRNCPY(m_ReqUserLogin_331100.content_type, "0");
	m_ReqUserLogin_331100.password_type = password_type_JYMM;
	m_ReqUserLogin_331100.asset_prop = asset_prop_PTKH;*/
	// 在这没有填写账号与密码

	m_pO32Codec = new CO32Codec();
	m_pO32Processor = new CO32Processor();

	// 两边维护的是同一对象
	m_pOrderMap = new COrderMap<int>();
	m_pO32Processor->m_pOrderMap = m_pOrderMap;


	//m_pApi = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();
	m_msgQueue_Query->Register(Query);
	m_msgQueue_Query->StartThread();

	m_msgQueue_Recv = new CMsgQueue();
	// 记录日志，以后可以屏蔽掉
	char filename[256] = { 0 };
	sprintf(filename, "d:\\UFX_Recv_%ld.dat", time(nullptr));
	m_msgQueue_Recv->SaveFileTo(filename);

#ifdef ENABLE_LICENSE
	// 为了这个加密库，居然把编译选项由/MDd改成了/MTd,以后一定要想办法改回去
	m_pLicense = new CLicense();

	char szPath[MAX_PATH] = { 0 };
	m_pLicense->GetDllPathByFunctionName("XRequest", szPath);
	m_pLicense->SetLicensePath(szPath);
	// 这里选的是从文件中加载公钥，可以写死到资源或代码中，这样用户就没有那么容易自己生成公私钥对替换了
	m_pLicense->SetPublicKeyString("30819D300D06092A864886F70D010101050003818B0030818702818100E17434DC96FC6384E260C03953341E197CBBAF000DC81CA869FC3488B8BAA19F7E5ACDC2987951879A9CD61E7E053C53D2AC98BA8F5CEF4B7C92A0E1A571EF912A7F67905CEBA1AEFE58D886C32107501208FBDB84B1063FDF360DE7F6396F509E6AF161E2DECAA0AC4FF724C0F9B79DF12790FC0C61ADA5B4A242D3251FE77D020111");

	string signatureString = m_pLicense->LoadStringFromFile(m_pLicense->m_SignaturePath);
	m_pLicense->SetSignatureString(signatureString.c_str());
#endif
}


CTraderApi::~CTraderApi(void)
{
	Disconnect();

#ifdef ENABLE_LICENSE
	if (m_pLicense == nullptr)
	{
		delete m_pLicense;
		m_pLicense = nullptr;
	}
#endif
}

void CTraderApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo,
	int count)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_pIDGenerator = new CIDGenerator();
	m_pIDGenerator->SetPrefix(m_UserInfo.UserID);


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
		// 如果签名信息不存在，只给试用权限
		err = m_pLicense->GetErrorCodeForSign();
	}

	if (err == 0)
	{
		err = m_pLicense->GetErrorCode();
	}

	if (err == 0)
	{
		err = m_pLicense->GetErrorCodeByAccount(m_UserInfo.UserID);
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

	char szExePath[MAX_PATH] = { 0 };
	GetExePath(szExePath);
	char szDllPath[MAX_PATH] = { 0 };
	GetDllPathByFunctionName("XRequest", szDllPath);
	char szDirectory[MAX_PATH] = { 0 };
	GetDirectoryByPath(szDllPath, szDirectory);
	char szCurrDir[MAX_PATH] = { 0 };

	if (SetCurrentDirectoryA(szDirectory))
	{
		LogField* pField = (LogField*)m_msgQueue->new_block(sizeof(LogField));

		sprintf_s(pField->Message, "SetCurrentDirectory:%s", szDirectory);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnLog, m_msgQueue, m_pClass, true, 0, pField, sizeof(LogField), nullptr, 0, nullptr, 0);
	}

	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
	pField->RawErrorID = 0;
	sprintf_s(pField->Text, "ExePath:%s", szExePath);
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Initialized, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);

	if (m_lpConfig)
	{
		m_lpConfig->Release();
		m_lpConfig = nullptr;
	}

	m_lpConfig = NewConfig();
	m_lpConfig->AddRef();
	m_lpConfig->Load(m_ServerInfo.ConfigPath);

	if (m_lpConnection)
	{
		m_lpConnection->Release();
		m_lpConnection = nullptr;
	}
	m_lpConnection = NewConnection(m_lpConfig);
	m_lpConnection->AddRef();

	int iRet = 0;
	if (0 != (iRet = m_lpConnection->Create2BizMsg(this)))
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
		pField->RawErrorID = iRet;
		strncpy(pField->Text, m_lpConnection->GetErrorMsg(iRet), sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		return 0;
	}

	if (0 != (iRet = m_lpConnection->Connect(5000)))
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
		pField->RawErrorID = iRet;
		strncpy(pField->Text, m_lpConnection->GetErrorMsg(iRet), sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		return 0;
	}

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	// 登录消息中心
	_Init2();

	return 0;
}

int CTraderApi::_Init2()
{
	if (m_lpMCConfig)
	{
		m_lpMCConfig->Release();
		m_lpMCConfig = nullptr;
	}

	m_lpMCConfig = NewConfig();
	m_lpMCConfig->AddRef();
	m_lpMCConfig->Load(m_ServerInfo.ExtInfoChar128);

	if (m_lpMCConnection)
	{
		m_lpMCConnection->Release();
		m_lpMCConnection = nullptr;
	}
	m_lpMCConnection = NewConnection(m_lpMCConfig);
	m_lpMCConnection->AddRef();

	int iRet = 0;
	if (0 != (iRet = m_lpMCConnection->Create2BizMsg(this)))
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
		pField->RawErrorID = iRet;
		strncpy(pField->Text, m_lpMCConnection->GetErrorMsg(iRet), sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		return 0;
	}

	if (0 != (iRet = m_lpMCConnection->Connect(5000)))
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
		pField->RawErrorID = iRet;
		strncpy(pField->Text, m_lpMCConnection->GetErrorMsg(iRet), sizeof(Char256Type));

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		return 0;
	}

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	return 0;
}

void CTraderApi::ReqSub()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqSub, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_Sub()
{
	char* bizName = (char*)m_lpMCConfig->GetString("subcribe", "biz_name", "");
	//如果需要使用发布订阅功能，必须配置配置mc标签下面的client_name项
	m_lpSub = m_lpMCConnection->NewSubscriber(this, bizName, 5000);
	if (!m_lpSub)
	{
		printf("NewSubscribe Error: %s\n", m_lpMCConnection->GetMCLastError());
		// 这里特别，需要不重复订
		return 0;
	}
	m_lpSub->AddRef();

	CSubscribeParamInterface* lpSubscribeParam = NewSubscribeParam();
	lpSubscribeParam->AddRef();
	char* topicName = (char*)m_lpMCConfig->GetString("subcribe", "topic_name", "");//主题名字
	lpSubscribeParam->SetTopicName(topicName);
	char* isFromNow = (char*)m_lpMCConfig->GetString("subcribe", "is_rebulid", "");//是否补缺
	if (strcmp(isFromNow, "true") == 0)
	{
		lpSubscribeParam->SetFromNow(true);
	}
	else
	{
		lpSubscribeParam->SetFromNow(false);
	}

	char* isReplace = (char*)m_lpMCConfig->GetString("subcribe", "is_replace", "");//是否覆盖
	if (strcmp(isReplace, "true") == 0)
	{
		lpSubscribeParam->SetReplace(true);
	}
	else
	{
		lpSubscribeParam->SetReplace(false);
	}

	char* lpApp = "xuxinpeng";
	lpSubscribeParam->SetAppData(lpApp, 9);//添加附加数据

										   //添加过滤字段
	int nCount = m_lpMCConfig->GetInt("subcribe", "filter_count", 0);
	//{
	//	for (int i = 1; i <= nCount; i++)
	//	{
	//		char lName[128] = { 0 };
	//		sprintf(lName, "filter_name%d", i);
	//		char* filterName = (char*)m_lpMCConfig->GetString("subcribe", lName, "");
	//		char lValue[128] = { 0 };
	//		sprintf(lValue, "filter_value%d", i);
	//		char* filterValue = (char*)m_lpMCConfig->GetString("subcribe", lValue, "");
	//		lpSubscribeParam->SetFilter(filterName, filterValue);
	//	}
	//}
	{
		// 账号还是从界面中读，ini文件麻烦
		lpSubscribeParam->SetFilter("operator_no", m_UserInfo.UserID);
		//lpSubscribeParam->SetFilter("operator_no", "111");
	}

	//添加发送频率
	lpSubscribeParam->SetSendInterval(m_lpMCConfig->GetInt("subcribe", "send_interval", 0));
	//添加返回字段
	nCount = m_lpMCConfig->GetInt("subcribe", "return_count", 0);
	for (int k = 1; k <= nCount; k++)
	{
		char lName[128] = { 0 };
		sprintf(lName, "return_filed%d", k);
		char* filedName = (char*)m_lpMCConfig->GetString("subcribe", lName, "");
		lpSubscribeParam->SetReturnFiled(filedName);
	}

	IF2Packer* pack = NewPacker(2);
	pack->AddRef();

	//创建一个业务包
	pack->BeginPack();
	pack->AddField("login_operator_no");
	pack->AddField("password");
	//pack->AddStr("1000");
	//pack->AddStr("0");
	pack->AddStr(m_UserInfo.UserID);
	pack->AddStr(m_UserInfo.Password);
	pack->EndPack();
	IF2UnPacker* lpBack = NULL;

	printf("开始订阅\n");
	int  iRet = m_lpSub->SubscribeTopic(lpSubscribeParam, 5000, &lpBack, pack);
	if (iRet > 0)
	{
		m_subscribeIndex = iRet;
		printf("SubscribeTopic info:[%d] 成功\n", iRet);
		//g_allSubscribeParam[subscribeIndex] = lpSubscribeParam;//保存到map中，用于以后的取消订阅
	}
	else
	{
		if (lpBack != NULL)
			PrintUnPack(lpBack);
		printf("SubscribeTopic info:[%d] %s\n", iRet, m_lpMCConnection->GetErrorMsg(iRet));

		{
			RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
			pField->RawErrorID = iRet;
			SAFE_SNPRINTF(pField->Text, "消息中心:[%d]%s。MsgDetail:%s", iRet, m_lpMCConnection->GetErrorMsg(iRet), lpBack->GetStr("MsgDetail"));

			// 由于后面还有别的登录消息，这里不会导致OQ断开
			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		}
	}
	return 0;
}

int CTraderApi::_UnSub()
{
	if (m_lpSub == nullptr)
		return 0;

	int subscribeIndex = 0;
	printf("取消订阅\n");
	int  iRet = m_lpSub->CancelSubscribeTopic(m_subscribeIndex);
	if (iRet > 0)
	{
		subscribeIndex = iRet;
		printf("CancelSubscribeTopic info:[%d] 成功\n", iRet);
		//g_allSubscribeParam[subscribeIndex] = lpSubscribeParam;//保存到map中，用于以后的取消订阅
	}
	else
	{
		if(m_lpMCConnection)
			printf("CancelSubscribeTopic info:[%d] %s\n", iRet, m_lpMCConnection->GetErrorMsg(iRet));

	}
	if (m_lpSub)
	{
		m_lpSub->Release();
		m_lpSub = nullptr;
	}
	return 0;
}

void CTraderApi::ReqUserLogin()
{
	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);


	//SAFE_STRNCPY(m_ReqUserLogin_331100.account_content,m_UserInfo.UserID);
	//SAFE_STRNCPY(m_ReqUserLogin_331100.password, m_UserInfo.Password);

	SAFE_STRNCPY(m_ReqUserLogin_10001.operator_no, m_UserInfo.UserID);
	SAFE_STRNCPY(m_ReqUserLogin_10001.password, m_UserInfo.Password);

	// 这里固定写死，不管监管了
	//SAFE_STRNCPY(m_ReqUserLogin_10001.mac_address, m_lpConnection->GetSelfMac());
	//SAFE_STRNCPY(m_ReqUserLogin_10001.ip_address, m_lpConnection->GetSelfAddress());
	SAFE_STRNCPY(m_ReqUserLogin_10001.mac_address, "5C-26-0A-2F-82-4F");
	SAFE_STRNCPY(m_ReqUserLogin_10001.ip_address, "192.168.88.123");
	SAFE_STRNCPY(m_ReqUserLogin_10001.op_station, "192.168.88.123|5C-26-0A-2F-82-4F");
	SAFE_STRNCPY(m_ReqUserLogin_10001.authorization_id, "hd123456");

	m_pO32Codec->m_lpConnection = m_lpConnection;
	m_pO32Codec->m_pProcessor = m_pO32Processor;

	m_pO32Processor->m_msgQueue = m_msgQueue;
	m_pO32Processor->m_pClass = this;

	m_pO32Codec->ReqFunction_10001(&m_ReqUserLogin_10001);

	return 0;
}

int CTraderApi::_ReqQryTradingAccount(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	switch (pQuery->Business)
	{
	case BusinessType::BusinessType_Stock:
	{
		REQ_ZHZJCX_34001 req = { 0 };
		SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
		SAFE_STRNCPY(req.account_code, pQuery->PortfolioID1);
		SAFE_STRNCPY(req.asset_no, pQuery->PortfolioID2);
		SAFE_STRNCPY(req.combi_no, pQuery->PortfolioID3);
		m_pO32Codec->ReqFunction_34001(&req);
	}
	break;
	case BusinessType::BusinessType_Future:
	{
		REQ_QHBZJZHCX_34003 req = { 0 };
		SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
		SAFE_STRNCPY(req.account_code, pQuery->PortfolioID1);
		SAFE_STRNCPY(req.asset_no, pQuery->PortfolioID2);
		SAFE_STRNCPY(req.combi_no, pQuery->PortfolioID3);
		m_pO32Codec->ReqFunction_34003(&req);
	}
	break;
	default:
		break;
	}

	return 0;
}

int CTraderApi::_ReqQryInvestorPosition(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	switch (pQuery->Business)
	{
	case BusinessType::BusinessType_Stock:
	{
		REQ_ZQCCCX_31001 req = { 0 };
		SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
		SAFE_STRNCPY(req.account_code, pQuery->PortfolioID1);
		SAFE_STRNCPY(req.asset_no, pQuery->PortfolioID2);
		SAFE_STRNCPY(req.combi_no, pQuery->PortfolioID3);
		m_pO32Codec->ReqFunction_31001(&req);
	}
	break;
	case BusinessType::BusinessType_Future:
	{
		REQ_QHCCCX_31003 req = { 0 };
		SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
		SAFE_STRNCPY(req.account_code, pQuery->PortfolioID1);
		SAFE_STRNCPY(req.asset_no, pQuery->PortfolioID2);
		SAFE_STRNCPY(req.combi_no, pQuery->PortfolioID3);
		m_pO32Codec->ReqFunction_31003(&req);
	}
	break;
	default:
		break;
	}

	return 0;
}

int CTraderApi::_ReqQryInstrument(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	//ReqFunction_330300(&m_RspUserLogin_331100);

	return 0;
}

int CTraderApi::_ReqQryOrder(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	switch (pQuery->Business)
	{
	case BusinessType::BusinessType_Stock:
	{
		REQ_ZQWTCX_32001 req = { 0 };
		SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
		SAFE_STRNCPY(req.account_code, pQuery->PortfolioID1);
		SAFE_STRNCPY(req.asset_no, pQuery->PortfolioID2);
		SAFE_STRNCPY(req.combi_no, pQuery->PortfolioID3);
		//req.request_num = 50;
		m_pO32Codec->ReqFunction_32001(&req);
	}
	break;
	case BusinessType::BusinessType_Future:
	{
		REQ_QHWTCX_32003 req = { 0 };
		SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
		SAFE_STRNCPY(req.account_code, pQuery->PortfolioID1);
		SAFE_STRNCPY(req.asset_no, pQuery->PortfolioID2);
		SAFE_STRNCPY(req.combi_no, pQuery->PortfolioID3);
		//req.request_num = 50;
		m_pO32Codec->ReqFunction_32003(&req);
	}
	break;
	default:
		break;
	}

	return 0;
}

int CTraderApi::_ReqQryTrade(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	switch (pQuery->Business)
	{
	case BusinessType::BusinessType_Stock:
	{
		REQ_ZQCJCX_33001 req = { 0 };
		SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
		SAFE_STRNCPY(req.account_code, pQuery->PortfolioID1);
		SAFE_STRNCPY(req.asset_no, pQuery->PortfolioID2);
		SAFE_STRNCPY(req.combi_no, pQuery->PortfolioID3);
		//req.request_num = 1000;
		m_pO32Codec->ReqFunction_33001(&req);
	}
	break;
	case BusinessType::BusinessType_Future:
	{
		REQ_QHCJCX_33003 req = { 0 };
		SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
		SAFE_STRNCPY(req.account_code, pQuery->PortfolioID1);
		SAFE_STRNCPY(req.asset_no, pQuery->PortfolioID2);
		SAFE_STRNCPY(req.combi_no, pQuery->PortfolioID3);
		//req.request_num = 50;
		m_pO32Codec->ReqFunction_33003(&req);
	}
	break;
	default:
		break;
	}

	return 0;
}

void CTraderApi::Subscribe(const string& szInstrumentIDs, const string& szExchangeID)
{
	//ReqFunction_400(szExchangeID.c_str(),szInstrumentIDs.c_str());
}


void CTraderApi::OnHeartbeat(IBizMessage* lpMsg)
{
	lpMsg->ChangeReq2AnsMessage();
	m_lpConnection->SendBizMsg(lpMsg, 1);
	return;
}

void CTraderApi::Disconnect()
{
	_UnSub();

	if (m_pO32Codec)
	{
		m_pO32Codec->m_lpConnection = nullptr;
		m_pO32Codec->m_pProcessor = nullptr;
	}

	if (m_msgQueue_Query)
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue_Query->Register(nullptr);
		m_msgQueue_Query->Clear();
		delete m_msgQueue_Query;
		m_msgQueue_Query = nullptr;
	}

	if (m_lpMCConnection)
	{
		// 为何第一次断开消息中心没有回报，后面断开就有回报了呢？
		m_lpMCConnection->Release();
		m_lpMCConnection = nullptr;
	}

	if (m_lpConnection)
	{
		m_lpConnection->Release();
		m_lpConnection = nullptr;
	}

	if (m_lpConfig)
	{
		m_lpConfig->Release();
		m_lpConfig = nullptr;
	}

	if (m_lpMCConfig)
	{
		m_lpMCConfig->Release();
		m_lpMCConfig = nullptr;
	}

	if (m_msgQueue_Recv)
	{
		m_msgQueue_Recv->StopThread();
		m_msgQueue_Recv->Register(nullptr);
		m_msgQueue_Recv->Clear();
		delete m_msgQueue_Recv;
		m_msgQueue_Recv = nullptr;
	}

	Sleep(100);

	if (m_msgQueue)
	{
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
	if (m_pOrderMap)
	{
		m_pOrderMap->clear();
		delete m_pOrderMap;
		m_pOrderMap = nullptr;
	}
	if (m_pO32Codec)
	{
		delete m_pO32Codec;
		m_pO32Codec = nullptr;
	}
	if (m_pO32Processor)
	{
		delete m_pO32Processor;
		m_pO32Processor = nullptr;
	}
	if (m_pIDGenerator)
	{
		delete m_pIDGenerator;
		m_pIDGenerator = nullptr;
	}
}

char* CTraderApi::ReqOrderInsert(
	OrderField* pOrder,
	int count,
	char* pszLocalIDBuf)
{
	int iRet = 0;
	lock_guard<mutex> cl(m_csOrderRef);

	int _id = m_pIDGenerator->GetTimeID();
	OrderField* pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));

	memcpy(pField, pOrder, sizeof(OrderField));
	sprintf(pField->LocalID, "%d", _id);

	// 只传过来合约名，没有传合约类型，所以这个结构体中还无法直接判断
	// 可以通过market_no来
	pField->Business = market_no_2_BusinessType(pField->ExchangeID);

	switch (pField->Business)
	{
	case BusinessType::BusinessType_Future:
	{
		REQ_QHWT_91004* _pWTField = (REQ_QHWT_91004*)m_msgQueue->new_block(sizeof(REQ_QHWT_91004));
		OrderField_2_REQ_QHWT_91004(&m_RspUserLogin_10001, pField, _pWTField);
		iRet = m_pO32Codec->ReqFunction_91004(_pWTField);
		pField->pUserData1 = _pWTField;
	}
	break;
	case BusinessType::BusinessType_Stock:
	{
		REQ_PTMMWT_91001* _pWTField = (REQ_PTMMWT_91001*)m_msgQueue->new_block(sizeof(REQ_PTMMWT_91001));
		OrderField_2_REQ_PTMMWT_91001(&m_RspUserLogin_10001, pField, _pWTField);
		iRet = m_pO32Codec->ReqFunction_91001(_pWTField, count);
		pField->pUserData1 = _pWTField;
	}
	break;
	default:
		break;
	}

	// 保存
	m_pOrderMap->insert(_id, pField, pField->pUserData1);
	// 正在发单，撤单时需要返回状态，如果是NetSent会在撤单时失败
	pField->Status = OrderStatus::OrderStatus_PendingNew;


	if (iRet > 0)
	{
		// 啥情况下会出现iRet返回值为
		sprintf(pField->LocalID, "%d", iRet);
		m_pOrderMap->insert(iRet, pField, pField->pUserData1);
	}

	// 由于包ID与委托编号都是数字，有可能出现冲突，所以两种的编码风格要换
	// sizeof结果不同，所以换一下
	strncpy(pszLocalIDBuf, pField->LocalID, sizeof(pField->LocalID) - 1);

	return pszLocalIDBuf;
}

char* CTraderApi::ReqOrderAction(OrderIDType* szIds, int count, char* pzsRtn)
{
	memset(pzsRtn, 0, sizeof(OrderIDType)*count);
	lock_guard<mutex> cl(m_csOrderRef);

	// 由于撤单东西都一样，还是需要再处理一下
	// 第一，下单时
	//{
	//	REQ_QHWTCD_91105 req = { 0 };
	//	SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
	//	req.entrust_no = 298803;
	//	int iRet = m_pO32Codec->ReqFunction_91105(&req);
	//	m_pOrderMap->insertCancel(req.entrust_no, nullptr);
	//	return pzsRtn;
	//}
	int _id = atoi(szIds[0]);
	OrderField* pField = (OrderField*)m_pOrderMap->findOrderXAPI(_id);
	if (pField == nullptr)
	{
		return pzsRtn;
	}

	int entrust_no = atoi(pField->ID);
	if (entrust_no == 0)
	{
		// 回报，然后不发单，这种比较靠谱一些
		SAFE_STRNCPY(pField->Text, "entrust_no字段值不允许为空。XAPI直接返回");
		pField->ExecType = ExecType::ExecType_CancelReject;
		// 订单状态要取历史

		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		return pzsRtn;
	}

	switch (pField->Business)
	{
	case BusinessType::BusinessType_Future:
	{
		REQ_QHWTCD_91105 req = { 0 };
		SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
		req.entrust_no = entrust_no;

		int iRet = m_pO32Codec->ReqFunction_91105(&req);
		m_pOrderMap->insertCancel(req.entrust_no, pField);
	}
	break;
	case BusinessType::BusinessType_Stock:
	{
		REQ_WTCD_91101 req = { 0 };
		SAFE_STRNCPY(req.user_token, m_RspUserLogin_10001.user_token);
		req.entrust_no = entrust_no;

		int iRet = m_pO32Codec->ReqFunction_91101(&req);
		m_pOrderMap->insertCancel(req.entrust_no, pField);
	}
	break;
	default:
		// 未知的类型，说明数据坏了
		assert(false);
		break;
	}

	return pzsRtn;
}

void CTraderApi::ReqQuery(QueryType type, ReqQueryField* pQuery)
{
	m_msgQueue_Query->Input_Copy(type, m_msgQueue_Query, this, 0, 0,
		pQuery, sizeof(ReqQueryField), nullptr, 0, nullptr, 0);
}


unsigned long CTraderApi::QueryInterface(const char *iid, IKnown **ppv)
{
	return 0;
}

unsigned long CTraderApi::AddRef()
{
	return 0;
}

unsigned long CTraderApi::Release()
{
	return 0;
}

void CTraderApi::OnConnect(CConnectionInterface *lpConnection)
{
}

void CTraderApi::OnSafeConnect(CConnectionInterface *lpConnection)
{
	// 交易与消息中心分别会发送
	LogField* pField = (LogField*)m_msgQueue->new_block(sizeof(LogField));

	if (lpConnection == m_lpConnection)
	{
		sprintf(pField->Message, "OnSafeConnect,交易");
	}
	else if (lpConnection == m_lpMCConnection)
	{
		sprintf(pField->Message, "OnSafeConnect,消息中心");
	}
	else
	{
		sprintf(pField->Message, "OnSafeConnect,未知");
	}

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnLog, m_msgQueue, m_pClass, true, 0, pField, sizeof(LogField), nullptr, 0, nullptr, 0);
}

void CTraderApi::OnRegister(CConnectionInterface *lpConnection)
{
	// 交易与消息中心分别会发送
	LogField* pField = (LogField*)m_msgQueue->new_block(sizeof(LogField));

	if (lpConnection == m_lpConnection)
	{
		sprintf(pField->Message, "OnRegister,交易");
	}
	else if (lpConnection == m_lpMCConnection)
	{
		sprintf(pField->Message, "OnRegister,消息中心");
	}
	else
	{
		sprintf(pField->Message, "OnRegister,未知");
	}

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnLog, m_msgQueue, m_pClass, true, 0, pField, sizeof(LogField), nullptr, 0, nullptr, 0);

	// 行情与消息中心都会过来，只需要登录一次即可_Sub();
	if (lpConnection == m_lpConnection)
	{
		ReqUserLogin();
	}
	else if (lpConnection == m_lpMCConnection)
	{
		ReqSub();
	}
}

void CTraderApi::OnClose(CConnectionInterface *lpConnection)
{
	LogField* pField = (LogField*)m_msgQueue->new_block(sizeof(LogField));

	if (lpConnection == m_lpConnection)
	{
		sprintf(pField->Message, "OnClose,交易");
	}
	else if (lpConnection == m_lpMCConnection)
	{
		sprintf(pField->Message, "OnClose,消息中心");
	}
	else
	{
		sprintf(pField->Message, "OnClose,未知");
	}

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnLog, m_msgQueue, m_pClass, true, 0, pField, sizeof(LogField), nullptr, 0, nullptr, 0);
}

void CTraderApi::OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData)
{
}

void CTraderApi::Reserved1(void *a, void *b, void *c, void *d)
{

}

void CTraderApi::Reserved2(void *a, void *b, void *c, void *d)
{

}

int CTraderApi::Reserved3()
{
	return 0;
}

void CTraderApi::Reserved4()
{

}

void CTraderApi::Reserved5()
{

}

void CTraderApi::Reserved6()
{

}

void CTraderApi::Reserved7()
{

}

void CTraderApi::OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult)
{
	LogField* pField = (LogField*)m_msgQueue->new_block(sizeof(LogField));

	sprintf(pField->Message, "OnReceivedBiz");

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnLog, m_msgQueue, m_pClass, true, 0, pField, sizeof(LogField), nullptr, 0, nullptr, 0);
}

void CTraderApi::OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult)
{
	LogField* pField = (LogField*)m_msgQueue->new_block(sizeof(LogField));

	sprintf(pField->Message, "OnReceivedBizEx");

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnLog, m_msgQueue, m_pClass, true, 0, pField, sizeof(LogField), nullptr, 0, nullptr, 0);
}

void CTraderApi::OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg)
{
	int issue_type = 0;

	if (lpMsg == nullptr)
		return;

	if (lpMsg->GetErrorNo() != 0)
	{
		puts(lpMsg->GetErrorInfo());
		return;
	}

	// 将数据复制出来
	int iMsgLen = 0;
	void * lpMsgBuffer = lpMsg->GetBuff(iMsgLen);

	lock_guard<mutex> cl(m_csRecv);
	m_msgQueue_Recv->Input_Copy(RequestType::E_OnReceivedBizMsg, m_msgQueue_Recv, this, 0, 0, lpMsgBuffer, iMsgLen, nullptr, 0, nullptr, 0);

}

// 下单时居然此处先到，这个地方应当考虑处理一下
// 1.也有可能是请求对应的响应先到，需要处理一下
// 方案，能到这表示单子已下成功，只要处理成交即可
void CTraderApi::OnReceived(CSubscribeInterface *lpSub, int subscribeIndex, const void *lpData, int nLength, LPSUBSCRIBE_RECVDATA lpRecvData)
{
	lock_guard<mutex> cl(m_csRecv);
	m_msgQueue_Recv->Input_Copy(RequestType::E_OnReceived, m_msgQueue_Recv, this, 0, 0, (void*)lpData, nLength, nullptr, 0, nullptr, 0);

}

void CTraderApi::OnRecvTickMsg(CSubscribeInterface *lpSub, int subscribeIndex, const char* TickMsgInfo)
{

}

#pragma warning(default:4996)