#include "stdafx.h"
#include "MdUserApi.h"
#include "../../include/QueueEnum.h"

#include "../../include/ApiHeader.h"
#include "../../include/ApiStruct.h"

#include "../../include/toolkit.h"
#include "../../include/ApiProcess.h"

#include "../../include/queue/MsgQueue.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mutex>
#include <vector>
#include <iostream>

using namespace std;

int Today(int day)
{
    time_t now = time(0);
    now += day * 86400;
    struct tm *ptmNow = localtime(&now);

    return (ptmNow->tm_year + 1900) * 10000
        + (ptmNow->tm_mon + 1) * 100
        + ptmNow->tm_mday;
}

void WindGetErrorMessage(LONG errCode, char* buffer, int bufferSize){
    WCHAR errMsg[1024] = { 0 };
    int msgLength = sizeof errMsg;
    CWAPIWrapperCpp::getErrorMsg(errCode, eCHN, errMsg, msgLength);
    auto strLen = wcslen(errMsg) + 1;
    char* str = new char[strLen];
    size_t converted;
    wcstombs_s(&converted, str, strLen, errMsg, _TRUNCATE);
    strcpy_s(buffer, bufferSize, str);
    delete[] str;
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
	pThis = this;

    m_CoInitialized = false;
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
    case Start:
        WindStart();
        break;
    case Stop:
        WindStop();
        break;
    case QueryInstrument:
        WindQryInstrument();
        break;
    default:
        break;
    }

    this_thread::sleep_for(chrono::milliseconds(1));
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

void CMdUserApi::Connect()
{
    m_msgQueue_Query->Input_NoCopy(RequestType::Start, m_msgQueue_Query, this, 0, 0, nullptr, 0, nullptr, 0, nullptr, 0);
}

void CMdUserApi::WindStart()
{
	// 弹出这个只是为了调试，以后可能要关闭
	AllocConsole();
	freopen("CON", "r", stdin);
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	::DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);

    if (!m_CoInitialized){
        CoInitialize(nullptr);
        m_CoInitialized = true;
    }

    m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);

    auto errCode = CWAPIWrapperCpp::start();
    if (errCode == 0){
        //m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
        m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);
    }
    else{
        RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
        pField->RawErrorID = errCode;
        WindGetErrorMessage(errCode, pField->Text, sizeof pField->Text);
        m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
    }
}

void CMdUserApi::WindStop()
{
    if (WindConnected()){
		// 取消所有请求
		CWAPIWrapperCpp::cancelRequest(0);

        CWAPIWrapperCpp::stop();
        // 全清理，只留最后一个
        m_msgQueue->Clear();
        m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
        // 主动触发
        m_msgQueue->Process();
    }

    if (m_CoInitialized){
        CoUninitialize();
        m_CoInitialized = false;
    }

    SetEvent(m_ExitEvent);
}

bool CMdUserApi::WindConnected()
{
    return CWAPIWrapperCpp::isconnected();
}

void CMdUserApi::WindQryInstrument()
{
    WindData wd;
    WCHAR options[1024];
    _snwprintf_s(options, sizeof options, _TRUNCATE, L"date=%d;sectorId=a001010100000000;field=wind_code,sec_name", Today(0));
    auto errCode = CWAPIWrapperCpp::wset(wd, L"SectorConstituent", options);
    if (errCode != 0){
        ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));
        pField->RawErrorID = errCode;
        strcpy_s(pField->Source, sizeof pField->Source, "WindQryInstrument");
        WindGetErrorMessage(errCode, pField->Text, sizeof pField->Text);
        m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnError, m_msgQueue, m_pClass, 0, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
        return;
    }

}

void CMdUserApi::Disconnect()
{
    // 清理查询队列
    if (m_msgQueue_Query)
    {
        m_ExitEvent = CreateEvent(nullptr, true, false, nullptr);
        m_msgQueue_Query->Input_NoCopy(RequestType::Stop, m_msgQueue_Query, this, 0, 0, nullptr, 0, nullptr, 0, nullptr, 0);
        WaitForSingleObject(m_ExitEvent, INFINITE);
        m_msgQueue_Query->StopThread();
        m_msgQueue_Query->Register(nullptr);
        m_msgQueue_Query->Clear();
        delete m_msgQueue_Query;
        m_msgQueue_Query = nullptr;
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
}

CMdUserApi* CMdUserApi::pThis = nullptr;

LONG WINAPI WindCallback(ULONGLONG reqId, const WindData &wd)
{
	//wsq订阅回调函数,返回数据存放在参数wd中，可以对其进行分析操作
	
	return CMdUserApi::pThis->_WindCallback(reqId, wd);
}

struct MyWindWsq
{
	double rt_date;
	double rt_time;
	double rt_last;
	double rt_vol;
	double rt_oi;
	double rt_bid1;
	double rt_ask1;
	double rt_bsize1;
	double rt_asize1;
};

string toString(const LPVARIANT data)
{
	string msg;
	switch (data->vt & VT_BSTR_BLOB)
	{
	/*case VT_I4:
		msg(_T("%d"), data->intVal);
		break;
	case VT_I8:
		msg.Format(_T("%I64d"), data->llVal);
		break;
	case VT_R8:
		msg.Format(_T("%f"), data->dblVal);
		break;
	case VT_EMPTY:
		msg = _T("NaN");
		break;
	case VT_BSTR:
		msg.Format(_T("%s"), data->bstrVal);
		break;
	case VT_DATE:
		msg = COleDateTime(data->date).Format();
		break;*/
	default:
		break;
	}
	return msg;
}


LONG CMdUserApi::_WindCallback(ULONGLONG reqId, const WindData &wd)
{
	int timeLength = wd.GetTimesLength();
	int codeLength = wd.GetCodesLength();
	int fieldsLength = wd.GetFieldsLength();

	for (int i = 0; i < codeLength; ++i)
	{
		//for (int j = 0; j < fieldsLength; ++j)
		//{
		//	VARIANT data;
		//	wcout << wd.GetDataItem(0, i, j, data) << endl;

		//	wcout << data.vt << endl;
		//}
		MyWindWsq st = { 0 };
		VARIANT data;

		wd.GetDataItem(0, i, 0, data);
		st.rt_date = data.dblVal;
		wd.GetDataItem(0, i, 1, data);
		st.rt_time = data.dblVal;
		wd.GetDataItem(0, i, 2, data);
		st.rt_last = data.dblVal;
		wd.GetDataItem(0, i, 3, data);
		st.rt_vol = data.dblVal;
		wd.GetDataItem(0, i, 4, data);
		st.rt_oi = data.dblVal;
		wd.GetDataItem(0, i, 5, data);
		st.rt_bid1 = data.dblVal;
		wd.GetDataItem(0, i, 6, data);
		st.rt_ask1 = data.dblVal;
		wd.GetDataItem(0, i, 7, data);
		st.rt_bsize1 = data.dblVal;
		wd.GetDataItem(0, i, 8, data);
		st.rt_asize1 = data.dblVal;

		//printf("rt_date:%f", st.rt_date);
		//printf("rt_time:%f", st.rt_time);
		//printf("rt_last:%f", st.rt_last);
		//printf("rt_vol:%f", st.rt_vol);
		//printf("rt_oi:%f", st.rt_oi);
		//printf("rt_bid1:%f", st.rt_bid1);
		//printf("rt_ask1:%f", st.rt_ask1);
		//printf("rt_bsize1:%f", st.rt_bsize1);
		//printf("rt_asize1:%f", st.rt_asize1);

		DepthMarketDataNField* pField = (DepthMarketDataNField*)m_msgQueue->new_block(sizeof(DepthMarketDataNField) + sizeof(DepthField) * 2);

		wstring wstr(wd.GetCodeByIndex(i));
		std::string str(wstr.length(), '\0');
		std::copy(wstr.begin(), wstr.end(), str.begin());

		sprintf(pField->Symbol, "%s", str.c_str());
		pField->TradingDay = st.rt_date;
		pField->ActionDay = st.rt_date;
		pField->UpdateTime = st.rt_time;
		pField->LastPrice = st.rt_last;
		pField->Volume = st.rt_vol;
		//pField->Turnover = pDepthMarketData->Turnover;
		pField->OpenInterest = st.rt_oi;
		//pField->AveragePrice = pDepthMarketData->AveragePrice;

		InitBidAsk(pField);

		do
		{
			if (st.rt_bsize1 == 0)
				break;
			AddBid(pField, st.rt_bid1, st.rt_bsize1, 0);
		} while (false);

		do
		{
			if (st.rt_asize1 == 0)
				break;
			AddAsk(pField, st.rt_ask1, st.rt_asize1, 0);
		} while (false);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
	}

	//if (timeLength > 1)
	//{
	//	if (fieldsLength > 1)
	//	{
	//		likeWsdList1(wd);
	//	}
	//	else //fieldsLength <= 1
	//	{
	//		if (codeLength >= 1)
	//		{
	//			likeWsdList2(wd);
	//		}
	//	}
	//}
	//else if (codeLength > 1)//timeLength <= 1
	//{
	//	if (fieldsLength > 1)
	//	{
	//		likeWssList(wd);
	//	}
	//	else
	//	{
	//		likeWsdList2(wd);
	//	}
	//}
	//else //timeLength <= 1 && codeLength <=1
	//{
	//	likeWssList(wd);
	//}
	//printf("==================");


	return 0;
}

void CMdUserApi::Subscribe(const string& szInstrumentIDs, const string& szExchangeID)
{
	ULONGLONG reqId = 0;
	//WCHAR options[1024];
	//_snwprintf_s(options, sizeof options, _TRUNCATE, L"date=%d;sectorId=a001010100000000;field=wind_code,sec_name", Today(0));
	std::wstring wstr(szInstrumentIDs.length(), L'\0');
	std::copy(szInstrumentIDs.begin(), szInstrumentIDs.end(), wstr.begin());


	auto errCode = CWAPIWrapperCpp::wsq(reqId, wstr.c_str(), L"rt_date,rt_time,rt_last,rt_vol,rt_oi,rt_bid1,rt_ask1,rt_bsize1,rt_asize1", WindCallback, L"", TRUE);
	if (errCode != 0) {
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));
		pField->RawErrorID = errCode;
		strcpy_s(pField->Source, sizeof pField->Source, "wsq");
		WindGetErrorMessage(errCode, pField->Text, sizeof pField->Text);
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnError, m_msgQueue, m_pClass, 0, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
		return;
	}

	// 对订阅的合约与请求号进行分类
	m_map_code_reqId[szInstrumentIDs] = reqId;
}

void CMdUserApi::Subscribe(const set<string>& instrumentIDs, const string& szExchangeID)
{

}

void CMdUserApi::Unsubscribe(const string& szInstrumentIDs, const string& szExchangeID)
{
	// 通过合约来查找请求号，然后再判断是否要再订阅
	map<string, ULONGLONG>::iterator it = m_map_code_reqId.find(szInstrumentIDs);
	if (it == m_map_code_reqId.end())
	{

	}
	else
	{
		ULONGLONG reqId = it->second;

		CWAPIWrapperCpp::cancelRequest(reqId);
	}
}
