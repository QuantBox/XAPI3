// CPP_TEST.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#if defined WINDOWS || _WIN32
#include <Windows.h>
#endif
#include <time.h>

#include <string.h>
#include "../../include/XApiCpp.h"

#ifndef USE_CMAKE
#ifdef _WIN64
#pragma comment(lib, "../../lib/XAPI_CPP_x64.lib")
#else
#pragma comment(lib, "../../lib/XAPI_CPP_x86.lib")
#endif
#endif

class CXSpiImpl :public CXSpi
{
public:
	CXSpiImpl() :CXSpi()
	{
		count = 0;
	}

	virtual void OnConnectionStatus(CXApi* pApi, ConnectionStatus status, RspUserLoginField* pUserLogin, int size1)
	{
		printf("%d\r\n", status);
		if (pUserLogin)
		{
			printf("%s\r\n", pUserLogin->Text);
		}
		if (status == ConnectionStatus::ConnectionStatus_Done)
		{
			if ((pApi->GetApiTypes() & ApiType::ApiType_MarketData) == ApiType::ApiType_MarketData)
			{
				// 得到当前的时间
				time_t rawtime;
				struct tm* timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				int mon = timeinfo->tm_mon;
				char buf[64] = { 0 };
				for (int i = 0; i < 12; ++i)
				{
					int x = mon + i;
					int a = x / 12;
					int b = x % 12;
					sprintf(buf, "IF%d%02d", (1900 + timeinfo->tm_year + a) % 100, b + 1);
					pApi->Subscribe(buf, "");
					sprintf(buf, "TF%d%02d", (1900 + timeinfo->tm_year + a) % 100, b + 1);
					pApi->Subscribe(buf, "");
				}
			}
		}
	}
	virtual void OnRtnError(CXApi* pApi, ErrorField* pError) {};

	virtual void OnRtnDepthMarketDataN(CXApi* pApi, DepthMarketDataNField* pMarketData)
	{
		printf("%s,%f,%d\r\n", pMarketData->Symbol, pMarketData->LastPrice, ++count);

		int size = sizeof(DepthField);
		char* pBid = ((char*)pMarketData) + sizeof(DepthMarketDataNField);
		int AskCount = (pMarketData->Size - sizeof(DepthMarketDataNField)) / size - pMarketData->BidCount;
		char* pAsk = ((char*)pMarketData) + sizeof(DepthMarketDataNField) + pMarketData->BidCount * size;
		for (int i = 0; i < pMarketData->BidCount; ++i)
		{
			DepthField* pDF = (DepthField*)(pBid + i * size);
			printf("%d,%f,%d,%d\r\n", i, pDF->Price, pDF->Size, pDF->Count);
		}
		for (int i = 0; i < AskCount; ++i)
		{
			DepthField* pDF = (DepthField*)(pAsk + i * size);
			printf("%d,%f,%d,%d\r\n", i, pDF->Price, pDF->Size, pDF->Count);
		}
	}

	virtual void OnRtnQuoteRequest(CXApi* pApi, QuoteRequestField* pQuoteRequest) {};

	virtual void OnRspQryInstrument(CXApi* pApi, InstrumentField* pInstrument, int size1, bool bIsLast) {};
	virtual void OnRspQryTradingAccount(CXApi* pApi, AccountField* pAccount, int size1, bool bIsLast) {};
	virtual void OnRspQryInvestorPosition(CXApi* pApi, PositionField* pPosition, int size1, bool bIsLast)
	{

		if (pPosition == nullptr)
		{
			return;
		}
		if (strcmp(pPosition->InstrumentID, "al1810") == 0)
		{
			int dubug = 1;
		}
		printf("%s,%s\r\n", pPosition->InstrumentName, pPosition->InstrumentID);
	}
	virtual void OnRspQrySettlementInfo(CXApi* pApi, SettlementInfoField* pSettlementInfo, int size1, bool bIsLast) {};
	virtual void OnRspQryInvestor(CXApi* pApi, InvestorField* pInvestor, int size1, bool bIsLast) {};



	virtual void OnRtnOrder(CXApi* pApi, OrderField* pOrder)
	{
		printf("%d,%s\r\n", pOrder->RawErrorID, pOrder->Text);
	}
	virtual void OnRspQryOrder(CXApi* pApi, OrderField* pOrder, int size1, bool bIsLast)
	{
		printf("%d,%s\r\n", pOrder->RawErrorID, pOrder->Text);
	}
	virtual void OnRtnTrade(CXApi* pApi, TradeField* pTrade) {};
	virtual void OnRtnQuote(CXApi* pApi, QuoteField* pQuote) {};

	virtual void OnRspQryHistoricalTicks(CXApi* pApi, TickField* pTicks, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast) {};
	virtual void OnRspQryHistoricalBars(CXApi* pApi, BarField* pBars, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast) {};

	virtual bool OnFilterSubscribe(CXApi* pApi, ExchangeType exchange, int instrument_part1, int instrument_part2, int instrument_part3, char* pInstrument)
	{
		// 当数字为0时，只判断交易所
		// 当交易所为
		if (instrument_part1 == 0)
			// 只要上海与深圳，不处理三板
			return exchange != ExchangeType::ExchangeType_NEEQ;

		//type = ExchangeType::SZSE;
		//double1 = 399300;

		int prefix1 = instrument_part1 / 100000;
		int prefix3 = instrument_part1 / 1000;
		switch (exchange)
		{
		case ExchangeType::ExchangeType_SSE:
			return (prefix1 == 6);
		case ExchangeType::ExchangeType_SZSE:
			return (prefix1 == 0) || (prefix3 == 300);
		default:
			break;
		}

		return true;
	}
public:
	//CXApi* m_pApi;
	int count;
};

int main_111(int argc, char* argv[])
{
	CXSpiImpl* p = new CXSpiImpl();
#if defined WINDOWS || _WIN32
	char DLLPath1[250] = "C:\\Program Files\\SmartQuant Ltd\\OpenQuant 2014\\XAPI\\x64\\CTP\\CTP_Quote_x86.dll";
	char DLLPath2[250] = "C:\\Program Files\\SmartQuant Ltd\\OpenQuant 2014\\XAPI\\x86\\CTP\\CTP_Trade_x86.dll";
#else
	char DLLPath1[250] = "libQuantBox_CTP_Quote.so";
	char DLLPath2[250] = "libQuantBox_CTP_Trade.so";
#endif

	ServerInfoField				m_ServerInfo1 = { 0 };
	ServerInfoField				m_ServerInfo2 = { 0 };
	UserInfoField				m_UserInfo = { 0 };

	strcpy(m_ServerInfo1.BrokerID, "4040");
	strcpy(m_ServerInfo1.Address, "tcp://yhzx-front1.yhqh.com:41213;tcp://yhzx-front3.yhqh.com:41213");

	strcpy(m_ServerInfo2.BrokerID, "4040");
	strcpy(m_ServerInfo2.Address, "tcp://yhzx-front1.yhqh.com:51205;tcp://yhzx-front3.yhqh.com:52205");

	strcpy(m_UserInfo.UserID, "00000025");
	strcpy(m_UserInfo.Password, "123456");

	CXApi* pApi1 = CXApi::CreateApi(DLLPath1);
	CXApi* pApi2 = CXApi::CreateApi(DLLPath2);
	if (pApi1 && pApi2)
	{
		if (!pApi1->Init())
		{
			printf("%s\r\n", pApi1->GetLastError());
			pApi1->Disconnect();
			return -1;
		}

		if (!pApi2->Init())
		{
			printf("%s\r\n", pApi2->GetLastError());
			pApi2->Disconnect();
			return -1;
		}

		pApi1->RegisterSpi(p);
		pApi2->RegisterSpi(p);

#if defined WINDOWS
		pApi1->Connect("D:\\", &m_ServerInfo1, &m_UserInfo, 1);
		pApi2->Connect("D:\\", &m_ServerInfo2, &m_UserInfo, 1);
#else
		pApi1->Connect("./", &m_ServerInfo1, &m_UserInfo, 1);
		pApi2->Connect("./", &m_ServerInfo2, &m_UserInfo, 1);
#endif

		getchar();

		pApi1->Subscribe("IF1504", "");

		OrderIDType Out = { 0 };
		OrderField* pOrder = new OrderField();
		memset(pOrder, 0, sizeof(OrderField));
		strcpy(pOrder->InstrumentID, "IF1504");
		pOrder->OpenClose = OpenCloseType::OpenCloseType_Open;
		pOrder->HedgeFlag = HedgeFlagType::HedgeFlagType_Speculation;
		pOrder->Price = 4000;
		pOrder->Qty = 1;
		pOrder->Type = OrderType::OrderType_Limit;
		pOrder->Side = OrderSide::OrderSide_Buy;

		pApi2->SendOrder(pOrder, 1, Out);

		printf("%s\r\n", Out);

		getchar();

		OrderIDType Out2 = { 0 };
		pApi2->CancelOrder(&Out, 1, Out2);

		getchar();

		pApi1->Disconnect();
		pApi2->Disconnect();
	}

	return 0;
}



int main_2(int argc, char* argv[])
{
	CXSpiImpl* p = new CXSpiImpl();

	char DLLPath1[250] = "TongShi_Quote\\TongShi_Quote_x86.dll";

	ServerInfoField				m_ServerInfo1 = { 0 };
	UserInfoField				m_UserInfo = { 0 };

	strcpy(m_ServerInfo1.Address, "D:\\Scengine\\Stock.dll");
	//strcpy(m_ServerInfo1.Address, "D:\\YjStock\\Stock.dll");

	CXApi* pApi1 = CXApi::CreateApi(DLLPath1);
	if (pApi1)
	{
		if (!pApi1->Init())
		{
			printf("%s\r\n", pApi1->GetLastError());
			pApi1->Disconnect();
			return -1;
		}

		pApi1->RegisterSpi(p);
		pApi1->Connect("D:\\", &m_ServerInfo1, &m_UserInfo, 1);


		getchar();

		printf("退出");

		pApi1->Disconnect();

		printf("退出成功");
	}

	return 0;
}


int main_4(int argc, char* argv[])
{
	CXSpiImpl* p = new CXSpiImpl();
#if defined WINDOWS || _WIN32
	char DLLPath1[250] = "CTP_Quote\\CTP_Quote_x86.dll";
#else
	char DLLPath1[250] = "libQuantBox_CTP_Quote.so";
#endif

	ServerInfoField				m_ServerInfo1 = { 0 };
	UserInfoField				m_UserInfo = { 0 };

	strcpy(m_ServerInfo1.BrokerID, "0272");
	strcpy(m_ServerInfo1.Address, "tcp://180.168.146.181:10210");
	m_ServerInfo1.TopicId = 100;// femas这个地方一定不能省
	strcpy(m_ServerInfo1.ExtInfoChar128, "tcp://*:5555");//这需要对Femas进行改造

	strcpy(m_UserInfo.UserID, "00049");
	strcpy(m_UserInfo.Password, "123456");

	CXApi* pApi1 = CXApi::CreateApi(DLLPath1);
	if (pApi1)
	{
		if (!pApi1->Init())
		{
			printf("%s\r\n", pApi1->GetLastError());
			pApi1->Disconnect();
			return -1;
		}

		//p->m_pApi = pApi1;
		pApi1->RegisterSpi(p);

#if defined WINDOWS || _WIN32
		pApi1->Connect("D:\\", &m_ServerInfo1, &m_UserInfo, 1);
#else
		pApi1->Connect("./", &m_ServerInfo1, &m_UserInfo, 1);
#endif

		do
		{
			int c = getchar();
			if (c == 'q')
				break;
		} while (true);

		pApi1->Disconnect();
	}

	return 0;
}

int main_122(int argc, char* argv[])
{
	CXSpiImpl* p = new CXSpiImpl();
#if defined WINDOWS || _WIN32
	char DLLPath1[250] = "C:\\Program Files\\SmartQuant Ltd\\OpenQuant 2014\\XAPI\\x86\\Tdx\\Tdx_Trade_x86.dl";
#else
	char DLLPath1[250] = "libQuantBox_CTP_Quote.so";
#endif

	ServerInfoField				m_ServerInfo1 = { 0 };
	UserInfoField				m_UserInfo = { 0 };

	strcpy(m_ServerInfo1.Address, "D:\\new_hbzq_qq\\Login.lua");
	strcpy(m_ServerInfo1.ExtInfoChar128, "D:\\new_hbzq_qq\\");

	strcpy(m_UserInfo.UserID, "05000000");
	strcpy(m_UserInfo.Password, "4");

	CXApi* pApi1 = CXApi::CreateApi(DLLPath1);
	if (pApi1)
	{
		if (!pApi1->Init())
		{
			printf("%s\r\n", pApi1->GetLastError());
			pApi1->Disconnect();
			return -1;
		}

		pApi1->RegisterSpi(p);

#if defined WINDOWS || _WIN32
		pApi1->Connect("D:\\", &m_ServerInfo1, &m_UserInfo, 1);
#else
		pApi1->Connect("./", &m_ServerInfo1, &m_UserInfo, 1);
#endif

		getchar();

		pApi1->Disconnect();

		getchar();

		do
		{
			int c = getchar();
			if (c == 'q')
				break;
		} while (true);

		pApi1->Disconnect();
	}

	return 0;
}

#include <stdlib.h>

void split_int_to_4_2_2(int HHmmss, int* _HH, int* _mm, int* _ss)
{
	// 20190917
	// 2019 09 17
	// 135400
	// 13 54 00
	*_HH = HHmmss / 10000;
	*_mm = HHmmss % 10000 / 100;
	*_ss = HHmmss % 100;
}

int contact_4_2_2_to_HHmmss(int yyyy, int MM, int ss)
{
	return yyyy * 10000 + MM * 100 + ss;
}

void split_str_to_4_2_2(const char* szHHmmss, int* _HH, int* _mm, int* _ss)
{
	int HHmmss = atoi(szHHmmss);
	split_int_to_4_2_2(HHmmss, _HH, _mm, _ss);
}

void split_HH_mm_ss_to_2_2_2(const char* szHH_mm_ss, int* _HH, int* _mm, int* _ss)
{
	// 处理有间隔的字符串,但没有处理HH:mm:ss.fff的情况
	// 13:54:00
	// 13 54 00
	// 2019-09-17
	// 2019 09 17
	int len = 8;

	*_HH = atoi(&szHH_mm_ss[0]);
	*_mm = atoi(&szHH_mm_ss[len - 2 - 3]);
	*_ss = atoi(&szHH_mm_ss[len - 2]);
}

void split_yyyy_MM_dd_to_2_2_2(const char* szyyyy_MM_dd, int* _HH, int* _mm, int* _ss)
{
	// 处理有间隔的字符串
	// 13:54:00
	// 13 54 00
	// 2019-09-17
	// 2019 09 17
	int len = strlen(szyyyy_MM_dd);

	*_HH = atoi(&szyyyy_MM_dd[0]);
	*_mm = atoi(&szyyyy_MM_dd[len - 2 - 3]);
	*_ss = atoi(&szyyyy_MM_dd[len - 2]);
}

tm tm_add_seconds(tm* _Tm, int sec)
{
	time_t _t = mktime(_Tm);
	_t += sec;
	// 需要立即取出，所以使用了*
	return *localtime(&_t);
}

tm yyyy_MM_dd_to_tm(int yyyy, int MM, int dd)
{
	tm _tm = { 0 }; // 记得要清空
	_tm.tm_year = yyyy - 1900;
	_tm.tm_mon = MM - 1;
	_tm.tm_mday = dd;
	return _tm;
}

int tm_to_yyyyMMdd(tm* _tm)
{
	return (_tm->tm_year + 1900) * 10000 + (_tm->tm_mon + 1) * 100 + _tm->tm_mday;
}

int tm_to_HHmmss(tm* _tm)
{
	return _tm->tm_hour * 10000 + _tm->tm_min * 100 + _tm->tm_sec;
}

int current_date()
{
	time_t now = time(0);
	return tm_to_yyyyMMdd(localtime(&now));
}

int current_time()
{
	time_t now = time(0);
	return tm_to_HHmmss(localtime(&now));
}

tm get_pre_trading_day(tm* _tm)
{
	// 没有处理长假问题，但使用时不用担心

	// 从周日开始，0-6

	// 周日周一，都要退到周五，其它只要退一天即可
	if (_tm->tm_wday == 1)
	{
		// 周一
		return tm_add_seconds(_tm, -86400 * 3);
	}
	if (_tm->tm_wday == 0)
	{
		// 周日
		return tm_add_seconds(_tm, -86400 * 2);
	}
	// 周六到周二
	return tm_add_seconds(_tm, -86400 * 1);
}

int GetUpdateTime(const char* UpdateTime, int* _UpdateTime, int* UpdateMillisec)
{
	*UpdateMillisec = 0;

	// UpdateTime处理
	int HH = 0;
	int mm = 0;
	int ss = 0;
	// 兼容
	int len = strlen(UpdateTime);
	if (len <= 6)
	{
		split_str_to_4_2_2(UpdateTime, &HH, &mm, &ss);
	}
	else
	{
		// 处理HH:mm:ss
		split_HH_mm_ss_to_2_2_2(UpdateTime, &HH, &mm, &ss);
		if (len > 9)
		{
			// 处理HH:mm:ss.fff
			*UpdateMillisec = atoi(&UpdateTime[9]);
		}
	}

	*_UpdateTime = contact_4_2_2_to_HHmmss(HH, mm, ss);
	//if (*_UpdateTime == 0)
	{
		//// 时间为0
		//int datetime = get_time();
		//if (datetime > 1500 && datetime < 234500)
		//	*_UpdateTime = datetime;
	}

	return HH;
}

void GetExchangeTime(int iTradingDay, int iPreTradingDay, char* TradingDay, char* ActionDay, char* UpdateTime, int* _TradingDay, int* _ActionDay, int* _UpdateTime, int* UpdateMillisec)
{
	// iTradingDay登录时获取的交易日，
	// iPreTradingDay登录时获取的交易日的前一交易日

	// 郑

	// UpdateTime处理
	int HH = GetUpdateTime(UpdateTime, _UpdateTime, UpdateMillisec);

	int tradingDay = atoi(TradingDay);
	int actionDay = atoi(ActionDay);

	// 白天直接使用
	*_TradingDay = tradingDay;
	*_ActionDay = actionDay;

	// 白天不处理，最早18点44就收到了数据
	if (HH > 6 && HH < 18)
		return;

	if (tradingDay != actionDay)
	{
		// 上海，21点到0点之间正好不相等，不需要处理
		// 上海第二天，两个值就相等了
		return;
	}

	// 1. 登录时间周一早上
	// 2. 登录时间周日
	// 3. 登录时间，周六晚
	// 4. 登录时间，周五晚
	if (HH <= 6)
	{
		// 只有上期所和能源中心的合约会进入到此状态，而它们的Day都是正确的，不用调整
		return;
	}

	if (HH >= 18)
	{
		// 大商所ActionDay存的是交易日，需要修正成交易日的前一交易日
		// 郑商所TradingDay存的是行情日，需要修正成交易日
		if (iTradingDay == tradingDay)
		{
			// 大商所
			// 由于长假前没有夜盘，所以不用担心前交易日计算错误
			*_ActionDay = iPreTradingDay;
		}
		else
		{
			// 郑商所
			*_TradingDay = iTradingDay;
		}
	}
}

int main(int argc, char* argv[])
{
	CXSpiImpl* p = new CXSpiImpl();
#if defined WINDOWS || _WIN32
#ifdef USE_CMAKE
#if _WIN64
	char DLLPath1[250] = "CTP_Quote_x64.dll";
	char DLLPath2[250] = "CTP_Trade_x64.dll";
#else
	char DLLPath1[250] = "CTP_Quote_x86.dll";
	char DLLPath2[250] = "CTP_Trade_x86.dll";
#endif
#else
	char DLLPath1[250] = "C:\\Program Files\\SmartQuant Ltd\\OpenQuant 2014\\XAPI\\x86\\CTP_SOPT_SE\\CTP_SOPT_SE_Quote_x86.dll";
	char DLLPath2[250] = "C:\\Program Files\\SmartQuant Ltd\\OpenQuant 2014\\XAPI\\x86\\CTP_SOPT_SE\\CTP_SOPT_SE_Trade_x86.dll";
#endif
#else
	char DLLPath1[250] = "libQuantBox_CTP_Quote.so";
	char DLLPath2[250] = "libQuantBox_CTP_Trade.so";
#endif

	ServerInfoField				m_ServerInfo1 = { 0 };
	ServerInfoField				m_ServerInfo2 = { 0 };
	UserInfoField				m_UserInfo = { 0 };

	strcpy(m_ServerInfo1.BrokerID, "2001");
	strcpy(m_ServerInfo1.Address, "tcp://124.74.247.142:18213");

	strcpy(m_ServerInfo2.BrokerID, "2001");
	strcpy(m_ServerInfo2.Address, "tcp://124.74.247.142:18205");
	strcpy(m_ServerInfo2.AppID, "TD_MJ_V2.3.0");
	strcpy(m_ServerInfo2.AuthCode, "EG9FWWGUYV2U292T");

	strcpy(m_UserInfo.UserID, "0000002078");
	strcpy(m_UserInfo.Password, "920782");



	while (true)
	{
		CXApi* pApi1 = CXApi::CreateApi(DLLPath1);
		CXApi* pApi2 = CXApi::CreateApi(DLLPath2);

		if (!pApi1->Init())
		{
			printf("%s\r\n", pApi1->GetLastError());
			pApi1->Disconnect();
			break;
		}

		if (!pApi2->Init())
		{
			printf("%s\r\n", pApi2->GetLastError());
			pApi2->Disconnect();
			break;
		}

		pApi1->RegisterSpi(p);
		pApi1->Connect("D:\\", &m_ServerInfo1, &m_UserInfo, 1);
		printf("已经执行完Connect\n");

		pApi2->RegisterSpi(p);
		pApi2->Connect("D:\\", &m_ServerInfo2, &m_UserInfo, 1);
		printf("已经执行完Connect\n");
		getchar();

		pApi1->Subscribe("10002303", "");

		ReqQueryField field = { 0 };
		pApi2->ReqQuery(QueryType::QueryType_ReqQryInvestorPosition, &field);
		//pApi2->ReqQuery(QueryType::QueryType_ReqQryOrder, &field);

		getchar();
		//pApi1->Disconnect();
		pApi2->Disconnect();
		printf("已经执行完Disconnect");
		getchar();
		getchar();
	}

	return 0;
}
