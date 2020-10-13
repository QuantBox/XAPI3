#if defined WINDOWS || _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <string>

#include "../../../include/XApiCpp.h"

static const char *szExchange_SH = "SSE";
static const char *szExchange_SZ = "SZSE";

class CXSpiImpl : public CXSpi
{
public:
	CXSpiImpl()
		: CXSpi()
	{
		client_id_ = 1;
	}

	virtual void OnRtnError(CXApi* pApi, ErrorField* pError)
	{
		std::cout << "OnRtnError: "
			<< pError->Source << ", "
			<< pError->RawErrorID << ", "
			<< pError->Text << std::endl;
	}

	virtual void OnConnectionStatus(CXApi* pApi, ConnectionStatus status, RspUserLoginField* pUserLogin, int size1)
	{
		std::cout << "connection status: " << status << std::endl;
		if (status == ConnectionStatus::ConnectionStatus_Done)
		{
			if ((pApi->GetApiTypes() & ApiType::ApiType_MarketData) == ApiType::ApiType_MarketData)
			{
				on_quote_ready(pApi);
			}
			else if ((pApi->GetApiTypes() & ApiType::ApiType_Trade) == ApiType::ApiType_Trade)
			{
				on_trade_ready(pApi);
			}
		}
	}

	virtual void OnRtnDepthMarketDataN(CXApi* pApi, DepthMarketDataNField* pMarketData)
	{
		std::cout << "{"
			<< "'msg': 'OnRtnDepthMarketDataN', "
			<< "'exchangeID': '" << pMarketData->ExchangeID << "', "
			<< "'exchangeEnum': " << pMarketData->Exchange << ", "
			<< "'symbol': '" << pMarketData->Symbol << "', "
			<< "'last_price': " << pMarketData->LastPrice << ", "
			<< "'volume': " << pMarketData->Volume << ", "
			<< "'turnover': " << pMarketData->Turnover << ", "
			<< "'avg_price': " << pMarketData->AveragePrice << ", "
			<< "'open_price': " << pMarketData->OpenPrice << ", "
			<< "'close_price': " << pMarketData->ClosePrice << ", "
			<< "'high_price': " << pMarketData->HighestPrice << ", "
			<< "'low_price': " << pMarketData->LowestPrice << ", "
			<< "'upper_limit_price': " << pMarketData->UpperLimitPrice << ", "
			<< "'lower_limit_price': " << pMarketData->LowerLimitPrice << ", "
			<< "'pre_close_price': " << pMarketData->PreClosePrice << ", "
			<< "'pre_settle_price': " << pMarketData->PreSettlementPrice << ", ";

		int size = sizeof(DepthField);
		char* pBid = ((char*)pMarketData) + sizeof(DepthMarketDataNField);
		int AskCount = (pMarketData->Size - sizeof(DepthMarketDataNField)) / size - pMarketData->BidCount;
		char* pAsk = ((char*)pMarketData) + sizeof(DepthMarketDataNField) + pMarketData->BidCount*size;

		std::cout << "'bids': [";
		for (int i = 0; i < pMarketData->BidCount; i++)
		{
			DepthField* pDF = (DepthField*)(pBid + i * size);
			std::cout << "[" << pDF->Price << ", " << pDF->Size << "]";
			if (i != pMarketData->BidCount - 1)
			{
				std::cout << ", ";
			}
		}
		std::cout << "], ";

		std::cout << "'asks': [";
		for (int i = 0; i < pMarketData->BidCount; i++)
		{
			DepthField* pDF = (DepthField*)(pAsk + i * size);
			std::cout << "[" << pDF->Price << ", " << pDF->Size << "]";
			if (i != pMarketData->BidCount - 1)
			{
				std::cout << ", ";
			}
		}
		std::cout << "]";

		std::cout << "}" << std::endl;
	}

	virtual void OnRspQryTradingAccount(CXApi* pApi, AccountField* pAccount, int size1, bool bIsLast)
	{
		print_account(pAccount);
	}
	virtual void OnRspQryInvestorPosition(CXApi* pApi, PositionField* pPosition, int size1, bool bIsLast)
	{
		print_position(pPosition);
	}

	virtual void OnRspQryOrder(CXApi* pApi, OrderField* pOrder, int size1, bool bIsLast)
	{
		print_order(pOrder);
	}
	virtual void OnRspQryTrade(CXApi* pApi, TradeField* pTrade, int size1, bool bIsLast)
	{
		print_trade(pTrade);
	}

	virtual void OnRtnOrder(CXApi* pApi, OrderField* pOrder)
	{
		print_order(pOrder);
	}
	virtual void OnRtnTrade(CXApi* pApi, TradeField* pTrade)
	{
		print_trade(pTrade);
	}

private:
	void on_quote_ready(CXApi* pApi)
	{
		std::cout << "XTP Quote Api Ready" << std::endl;

		char *symbols[] = {
			"600090",
			"600887",
		};
		int count = sizeof(symbols) / sizeof(char*) - 1;

		std::string buf = "";
		for (int i = 0; i < count; i++)
		{
			buf.append(symbols[i]);
			if (i != count - 1)
			{
				buf.append(";");
			}
		}
		pApi->Subscribe(buf.c_str(), szExchange_SH);
	}
	void on_trade_ready(CXApi* pApi)
	{
		std::cout << "XTP Trade Api Ready" << std::endl;

		// query trading account
		ReqQueryField query_account = { 0 };
		pApi->ReqQuery(QueryType::QueryType_ReqQryTradingAccount, &query_account);

		// query position
		ReqQueryField query_position = { 0 };
		pApi->ReqQuery(QueryType::QueryType_ReqQryInvestorPosition, &query_position);

		// query orders
		ReqQueryField query_order = { 0 };
		query_order.DateStart = 20180101;
		query_order.DateEnd = 20191231;
		pApi->ReqQuery(QueryType::QueryType_ReqQryOrder, &query_order);

		// query trades
		ReqQueryField query_trade = { 0 };
		query_trade.DateStart = 20180101;
		query_trade.DateEnd = 20191231;
		pApi->ReqQuery(QueryType::QueryType_ReqQryTrade, &query_trade);

//		// insert order
//		OrderField order = { 0 };
//		strcpy(order.ExchangeID, szExchange_SH);
//		strcpy(order.Symbol, "600090");
//		order.Price = 5.5;
//		order.Qty = 200;
//		order.Type = OrderType::OrderType_Limit;
//		order.Side = OrderSide::OrderSide_Buy;
//		pApi->SendOrder(&order, 1, nullptr);
	}

	void print_order(OrderField* pOrder)
	{
		if (pOrder == nullptr)
		{
			return;
		}

		std::cout << "{"
			<< "'msg': 'order', "
			<< "'order_id': '" << pOrder->ID << "', "
			<< "'local_id': '" << pOrder->LocalID << "', "
			<< "'order_id': '" << pOrder->OrderID << "', "
			<< "'exchange': '" << pOrder->ExchangeID << "', "
			<< "'symbol': '" << pOrder->Symbol << "', "
			<< "'price': '" << pOrder->Price << ", "
			<< "'vol': " << pOrder->Qty << ", "
			<< "'dir': " << pOrder->Side << ", "
			<< "'open_close': " << pOrder->OpenClose << ", "
			<< "'status': " << pOrder->Status << ", "
			<< "'type': " << pOrder->Type << ", "
			<< "'traded': " << pOrder->CumQty << ", "
			<< "'left': " << pOrder->LeavesQty
			<< "}" << std::endl;
	}
	void print_trade(TradeField* pTrade)
	{
		if (pTrade == nullptr)
		{
			return;
		}

		std::cout << "{"
			<< "'msg': 'trade', "
			<< "'order_id': '" << pTrade->ID << "', "
			<< "'trade_id': '" << pTrade->TradeID << "', "
			<< "'exchange': '" << pTrade->ExchangeID << "', "
			<< "'symbol': '" << pTrade->Symbol << "', "
			<< "'price': '" << pTrade->Price << ", "
			<< "'vol': " << pTrade->Qty << ", "
			<< "'dir': " << pTrade->Side << ", "
			<< "'open_close': " << pTrade->OpenClose 
			<< "}" << std::endl;
	}
	void print_account(AccountField* pAccount)
	{
		if (pAccount == nullptr)
		{
			return;
		}

		std::cout << "{"
			<< "'msg': 'account', "
			<< "'balance': " << pAccount->Balance << ", "
			<< "'available': " << pAccount->Available << ", "
			<< "'withdraw_quota': " << pAccount->WithdrawQuota
			<< "}" << std::endl;
	}
	void print_position(PositionField* position)
	{
		if (position == nullptr)
		{
			return;
		}

		std::cout << "{"
			<< "'msg': 'position', "
			<< "'exchange': '" << position->ExchangeID << "', "
			<< "'symbol': '" << position->Symbol << "', "
			<< "'dir': '" << position->Side << ", "
			<< "'position': " << position->Position << ", "
			<< "'sellable': " << position->Position - position->TodayPosition << ", "
			<< "'avg_price': " << position->PositionCost
			<< "}" << std::endl;
	}

public:
	int client_id_;
};

int main(int argc, char *argv[])
{
	CXSpiImpl* p = new CXSpiImpl();

#if _WIN32
	#if _WIN64
		char shared_lib_quote[250] = "XTP_Quote_x64.dll";
		char shared_lib_trade[250] = "XTP_Trade_x64.dll";
	#else
		char shared_lib_quote[250] = "XTP_Quote_x86.dll";
		char shared_lib_trade[250] = "XTP_Trade_x86.dll";
	#endif
#else
	char shared_lib_quote[1024] = "libXTP_Quote.so";
	char shared_lib_trade[1024] = "libXTP_Trade.so";
	// char dir_path[512] = {0};
	// char shared_lib_quote[1024] = {0};
	// char shared_lib_trade[1024] = {0};

	// if (getcwd(dir_path, sizeof(dir_path)) == NULL)
	// {
	// 	return 1;
	// }
	// snprintf(shared_lib_quote, "%s/%s", dir_path, "libXTP_Quote.so");
	// snprintf(shared_lib_trade, "%s/%s", dir_path, "libXTP_Trade.so");
#endif

	ServerInfoField				quote_server_info = { 0 };
	ServerInfoField				trade_server_info = { 0 };
	UserInfoField				user_info = { 0 };

	strcpy(quote_server_info.Address, "120.27.164.138");
	quote_server_info.IsUsingUdp = false;
	quote_server_info.Port = 6002;

	strcpy(trade_server_info.Address, "120.27.164.69");
	trade_server_info.Port = 6001;

	// 注意，这里要填入用户名，密码和key
	strcpy(user_info.UserID, "userid");		// 用户名
	strcpy(user_info.Password, "password");		// 密码
	user_info.ExtInfoInt32 = p->client_id_;		// 自定义 client_id
	strcpy(user_info.ExtInfoChar64, "xxxxxxxx"); // key

	CXApi* p_api_quote = CXApi::CreateApi(shared_lib_quote);
	CXApi* p_api_trade = CXApi::CreateApi(shared_lib_trade);

	if (!p_api_quote->Init())
	{
		printf("%s\r\n", p_api_quote->GetLastError());
		p_api_quote->Disconnect();
		return 1;
	}

	if (!p_api_trade->Init())
	{
		printf("%s\r\n", p_api_trade->GetLastError());
		p_api_trade->Disconnect();
		return 1;
	}

	p_api_quote->RegisterSpi(p);
	p_api_quote->Connect("./", &quote_server_info, &user_info, 0);
	printf("Quote已经执行完Connect\n");

	p_api_trade->RegisterSpi(p);
	p_api_trade->Connect("./", &trade_server_info, &user_info, 0);
	printf("Trade已经执行完Connect\n");

	getchar();

	p_api_quote->Disconnect();
	printf("Quote已经执行完Disconnect");

	p_api_trade->Disconnect();
	printf("Trade已经执行完Disconnect");

	return 0;
}
