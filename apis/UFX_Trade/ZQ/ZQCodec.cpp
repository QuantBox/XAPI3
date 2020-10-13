#include "stdafx.h"
#include "ZQCodec.h"


CZQCodec::CZQCodec()
{
}


CZQCodec::~CZQCodec()
{
}

int CZQCodec::IsError(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError)
{
	pError->error_no = lpUnPacker->GetInt("error_no");
	if (pError->error_no != 0)
	{
		const char *error_info = lpUnPacker->GetStr("error_info");

		strcpy(pError->error_info, error_info);
	}

	return pError->error_no;
}

int CZQCodec::ReqFunction_331100(REQ_DL_331100* pReq)
{
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction(331100);
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败！\r\n");
		return -1;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("op_branch_no", 'I', 5);//操作分支机构
	pPacker->AddField("op_entrust_way", 'C', 1);//委托方式 
	pPacker->AddField("op_station", 'S', 255);//站点地址
	pPacker->AddField("branch_no", 'I', 5);
	pPacker->AddField("password", 'S', 50);
	pPacker->AddField("password_type", 'C');
	pPacker->AddField("input_content", 'C');
	pPacker->AddField("account_content", 'S', 30);
	pPacker->AddField("content_type", 'S', 6);
	pPacker->AddField("asset_prop", 'C', 1);

	///加入对应的字段值
	pPacker->AddInt(pReq->op_branch_no);
	pPacker->AddChar(pReq->op_entrust_way);
	pPacker->AddStr(pReq->op_station);
	pPacker->AddInt(pReq->branch_no);
	pPacker->AddStr(pReq->password);
	pPacker->AddChar(pReq->password_type);
	pPacker->AddChar(pReq->input_content);
	pPacker->AddStr(pReq->account_content);
	pPacker->AddStr(pReq->content_type);
	pPacker->AddChar(pReq->asset_prop);

	///结束打包
	pPacker->EndPack();

	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	int iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

// 登录回报
int CZQCodec::OnResponse_331100(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_DL_331100* pRsp)
{
	if (0 != IsError(lpUnPacker, pError))
	{
		return -1;
	}

	int init_date = lpUnPacker->GetInt("init_date");
	int branch_no = lpUnPacker->GetInt("branch_no");
	const char *client_id = lpUnPacker->GetStr("client_id");
	const char *user_token = lpUnPacker->GetStr("user_token");
	int sysnode_id = lpUnPacker->GetInt("sysnode_id");
	int op_branch_no = lpUnPacker->GetInt("op_branch_no");
	const char *client_name = lpUnPacker->GetStr("client_name");

	// 与接口规范中不对应
	pRsp->init_date = init_date;
	pRsp->branch_no = branch_no;
	strcpy(pRsp->client_id, client_id);
	strcpy(pRsp->user_token, user_token);
	pRsp->sysnode_id = sysnode_id;
	pRsp->op_branch_no = op_branch_no;
	strcpy(pRsp->client_name, client_name);

	return 0;
}

// 行情查询
int CZQCodec::ReqFunction_400(REQ_ZQHQCX_400* pReq)
{
	IF2Packer *pPacker = NewPacker(2);
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction(400);
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	if (!pPacker)
	{
		printf("取打包器失败！\r\n");
		return -1;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	//加入字段名
	pPacker->AddField("exchange_type", 'S', 4);
	pPacker->AddField("stock_code", 'S', 16);

	//加入字段值
	pPacker->AddStr(pReq->exchange_type);
	pPacker->AddStr(pReq->stock_code);

	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	int iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

// 行情查询响应
int CZQCodec::OnResponse_400(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_ZQHQCX_400* pRsp)
{
	if (0 != IsError(lpUnPacker, pError))
	{
		return -1;
	}

	const char *exchange_index = lpUnPacker->GetStr("exchange_index");
	double last_price = lpUnPacker->GetDouble("last_price");
	double open_price = lpUnPacker->GetDouble("open_price");
	double close_price = lpUnPacker->GetDouble("close_price");
	double high_price = lpUnPacker->GetDouble("high_price");
	double low_price = lpUnPacker->GetDouble("low_price");
	double business_balance = lpUnPacker->GetDouble("business_balance");
	int business_amount = lpUnPacker->GetInt("business_amount");
	double buy_price1 = lpUnPacker->GetDouble("buy_price1");
	double buy_price2 = lpUnPacker->GetDouble("buy_price2");
	double buy_price3 = lpUnPacker->GetDouble("buy_price3");
	double buy_price4 = lpUnPacker->GetDouble("buy_price4");
	double buy_price5 = lpUnPacker->GetDouble("buy_price5");
	double sale_price1 = lpUnPacker->GetDouble("sale_price1");
	double sale_price2 = lpUnPacker->GetDouble("sale_price2");
	double sale_price3 = lpUnPacker->GetDouble("sale_price3");
	double sale_price4 = lpUnPacker->GetDouble("sale_price4");
	double sale_price5 = lpUnPacker->GetDouble("sale_price5");
	int buy_amount1 = lpUnPacker->GetInt("buy_amount1");
	int buy_amount2 = lpUnPacker->GetInt("buy_amount2");
	int buy_amount3 = lpUnPacker->GetInt("buy_amount3");
	int buy_amount4 = lpUnPacker->GetInt("buy_amount4");
	int buy_amount5 = lpUnPacker->GetInt("buy_amount5");
	int sale_amount1 = lpUnPacker->GetInt("sale_amount1");
	int sale_amount2 = lpUnPacker->GetInt("sale_amount2");
	int sale_amount3 = lpUnPacker->GetInt("sale_amount3");
	int sale_amount4 = lpUnPacker->GetInt("sale_amount4");
	int sale_amount5 = lpUnPacker->GetInt("sale_amount5");
	double stock_interest = lpUnPacker->GetDouble("stock_interest");
	const char *stock_name = lpUnPacker->GetStr("stock_name");
	int marketmaker_amount = lpUnPacker->GetInt("marketmaker_amount");

	return 0;
}

int CZQCodec::ReqFunction_333002(REQ_PTWT_333002* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();
	
	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;
	
	IBizMessage* lpBizMessage = NewBizMessage();
	
	lpBizMessage->AddRef();
	
	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(333002);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	
	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();
	
	///加入字段名
	pPacker->AddField("op_branch_no", 'I', 5);//名字 类型 长度
	pPacker->AddField("op_entrust_way", 'C', 1);
	pPacker->AddField("op_station", 'S', 255);
	pPacker->AddField("branch_no", 'I', 5);
	pPacker->AddField("client_id", 'S', 18);//客户ID
	pPacker->AddField("fund_account", 'S', 18);//资金账号
	pPacker->AddField("password", 'S', 50);
	pPacker->AddField("password_type", 'C', 1);
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("exchange_type", 'S', 4);
	pPacker->AddField("stock_account", 'S', 11);
	pPacker->AddField("stock_code", 'S', 16);
	pPacker->AddField("entrust_amount", 'F', 10, 0);	// 需要再确认一下N10
	pPacker->AddField("entrust_price", 'F', 18, 4);		// 需要再确认一下N11.4
	pPacker->AddField("entrust_bs", 'C', 1);
	pPacker->AddField("entrust_prop", 'S', 3);
	pPacker->AddField("batch_no", 'I', 8);

	///加入对应的字段值
	pPacker->AddInt(pReq->op_branch_no);
	pPacker->AddChar(pReq->op_entrust_way);
	pPacker->AddStr(pReq->op_station);
	pPacker->AddInt(pReq->branch_no);
	pPacker->AddStr(pReq->client_id);
	pPacker->AddStr(pReq->fund_account);
	pPacker->AddStr(pReq->password);
	pPacker->AddChar(pReq->password_type);
	pPacker->AddStr(pReq->user_token);
	pPacker->AddStr(pReq->exchange_type);
	pPacker->AddStr(pReq->stock_account);
	pPacker->AddStr(pReq->stock_code);
	pPacker->AddDouble(pReq->entrust_amount);
	pPacker->AddDouble(pReq->entrust_price);
	pPacker->AddChar(pReq->entrust_bs);
	pPacker->AddStr(pReq->entrust_prop);
	pPacker->AddInt(pReq->batch_no);
	
	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();
	
	return iRet;
}

int CZQCodec::OnResponse_333002(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_PTWT_333002* pRsp)
{
	if (0 != IsError(lpUnPacker, pError))
	{
		return -1;
	}

	int init_date = lpUnPacker->GetInt("init_date");
	int entrust_no = lpUnPacker->GetInt("entrust_no");
	int batch_no = lpUnPacker->GetInt("batch_no");
	int report_no = lpUnPacker->GetInt("report_no");
	const char* seat_no = lpUnPacker->GetStr("seat_no");
	int entrust_time = lpUnPacker->GetInt("entrust_time");

	pRsp->init_date = init_date;
	pRsp->entrust_no = entrust_no;
	pRsp->batch_no = batch_no;
	pRsp->report_no = report_no;
	strcpy(pRsp->seat_no,seat_no);
	pRsp->entrust_time = entrust_time;

	return 0;
}

int CZQCodec::ReqFunction_332254(REQ_ZJKSCX_332254* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(332254);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("op_branch_no", 'I', 5);//名字 类型 长度
	pPacker->AddField("op_entrust_way", 'C', 1);
	pPacker->AddField("op_station", 'S', 255);
	pPacker->AddField("branch_no", 'I', 5);
	pPacker->AddField("client_id", 'S', 18);//客户ID
	pPacker->AddField("fund_account", 'S', 18);//资金账号
	pPacker->AddField("password", 'S', 50);
	pPacker->AddField("password_type", 'C', 1);
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("money_type", 'S', 3);

	///加入对应的字段值
	pPacker->AddInt(pReq->op_branch_no);
	pPacker->AddChar(pReq->op_entrust_way);
	pPacker->AddStr(pReq->op_station);
	pPacker->AddInt(pReq->branch_no);
	pPacker->AddStr(pReq->client_id);
	pPacker->AddStr(pReq->fund_account);
	pPacker->AddStr(pReq->password);
	pPacker->AddChar(pReq->password_type);
	pPacker->AddStr(pReq->user_token);
	pPacker->AddStr("");


	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CZQCodec::OnResponse_332254(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_ZJKSCX_332254* pRsp)
{
	if (0 != IsError(lpUnPacker, pError))
	{
		return -1;
	}

	int count = lpUnPacker->GetRowCount();
	for (int i = 0; i < count; ++i)
	{
		const char* momoney_type = lpUnPacker->GetStr("momoney_type");
		double current_balance = lpUnPacker->GetDouble("current_balance");
		double enable_balance = lpUnPacker->GetDouble("enable_balance");
		double fetch_balance = lpUnPacker->GetDouble("fetch_balance");
		double frozen_balance = lpUnPacker->GetDouble("frozen_balance");
		double unfrozen_balance = lpUnPacker->GetDouble("unfrozen_balance");
		double fund_balance = lpUnPacker->GetDouble("fund_balance");

		strcpy(pRsp->momoney_type, momoney_type);
		pRsp->current_balance = current_balance;
		pRsp->enable_balance = enable_balance;
		pRsp->fetch_balance = fetch_balance;
		pRsp->frozen_balance = frozen_balance;
		pRsp->unfrozen_balance = unfrozen_balance;
		pRsp->fund_balance = fund_balance;

		lpUnPacker->Next();
	}

	return 0;
}

int CZQCodec::ReqFunction_333103(REQ_CCKSCX_333103* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(333103);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("op_branch_no", 'I', 5);//名字 类型 长度
	pPacker->AddField("op_entrust_way", 'C', 1);
	pPacker->AddField("op_station", 'S', 255);
	pPacker->AddField("branch_no", 'I', 5);
	pPacker->AddField("client_id", 'S', 18);//客户ID
	pPacker->AddField("fund_account", 'S', 18);//资金账号
	pPacker->AddField("password", 'S', 10);
	pPacker->AddField("password_type", 'C', 1);
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("exchange_type", 'S', 4);
	pPacker->AddField("stock_account", 'S', 15);
	pPacker->AddField("stock_code", 'S', 16);
	pPacker->AddField("position_str", 'S', 100);
	pPacker->AddField("request_num", 'I', 10);

	///加入对应的字段值
	pPacker->AddInt(pReq->op_branch_no);
	pPacker->AddChar(pReq->op_entrust_way);
	pPacker->AddStr(pReq->op_station);
	pPacker->AddInt(pReq->branch_no);
	pPacker->AddStr(pReq->client_id);
	pPacker->AddStr(pReq->fund_account);
	pPacker->AddStr(pReq->password);
	pPacker->AddChar(pReq->password_type);
	pPacker->AddStr(pReq->user_token);
	pPacker->AddStr(pReq->exchange_type);
	pPacker->AddStr(pReq->stock_account);
	pPacker->AddStr(pReq->stock_code);
	pPacker->AddStr(pReq->position_str);
	pPacker->AddInt(pReq->request_num);

	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CZQCodec::OnResponse_333103(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_CCKSCX_333103* pRsp)
{
	if (0 != IsError(lpUnPacker, pError))
	{
		return -1;
	}

	int count = lpUnPacker->GetRowCount();
	for (int i = 0; i < count; ++i)
	{
		const char* fund_account = lpUnPacker->GetStr("fund_account");
		int ffare_kind = lpUnPacker->GetInt("ffare_kind");
		int bfare_kind = lpUnPacker->GetInt("bfare_kind");
		char profit_flag = lpUnPacker->GetChar("profit_flag");
		const char* exchange_type = lpUnPacker->GetStr("exchange_type");
		const char* stock_account = lpUnPacker->GetStr("stock_account");
		const char* stock_code = lpUnPacker->GetStr("stock_code");
		const char* stock_type = lpUnPacker->GetStr("stock_type");
		const char* stock_name = lpUnPacker->GetStr("stock_name");
		int hold_amount = lpUnPacker->GetInt("hold_amount");
		int current_amount = lpUnPacker->GetInt("current_amount");
		double cost_price = lpUnPacker->GetDouble("cost_price");
		double income_balance = lpUnPacker->GetDouble("income_balance");
		char hand_flag = lpUnPacker->GetChar("hand_flag");
		int frozen_amount = lpUnPacker->GetInt("frozen_amount");
		int unfrozen_amount = lpUnPacker->GetInt("unfrozen_amount");
		int enable_amount = lpUnPacker->GetInt("enable_amount");
		int real_buy_amount = lpUnPacker->GetInt("real_buy_amount");
		int real_sell_amount = lpUnPacker->GetInt("real_sell_amount");
		int uncome_buy_amount = lpUnPacker->GetInt("uncome_buy_amount");
		int uncome_sell_amount = lpUnPacker->GetInt("uncome_sell_amount");
		int entrust_sell_amount = lpUnPacker->GetInt("entrust_sell_amount");

		double asset_price = lpUnPacker->GetDouble("asset_price");
		double last_price = lpUnPacker->GetDouble("last_price");
		double market_value = lpUnPacker->GetDouble("market_value");
		const char* position_str = lpUnPacker->GetStr("position_str");
		int sum_buy_amount = lpUnPacker->GetInt("sum_buy_amount");
		double sum_buy_balance = lpUnPacker->GetDouble("sum_buy_balance");
		int sum_sell_amount = lpUnPacker->GetInt("sum_sell_amount");
		double sum_sell_balance = lpUnPacker->GetDouble("sum_sell_balance");
		double real_buy_balance = lpUnPacker->GetDouble("real_buy_balance");
		double real_sell_balance = lpUnPacker->GetDouble("real_sell_balance");
		char delist_flag = lpUnPacker->GetChar("delist_flag");
		int delist_date = lpUnPacker->GetInt("delist_date");
		double par_value = lpUnPacker->GetDouble("par_value");


		strcpy(pRsp->fund_account, fund_account);
		pRsp->ffare_kind = ffare_kind;
		pRsp->bfare_kind = bfare_kind;
		pRsp->profit_flag = profit_flag;
		strcpy(pRsp->exchange_type, exchange_type);
		strcpy(pRsp->stock_account, stock_account);
		strcpy(pRsp->stock_code, stock_code);
		strcpy(pRsp->stock_type, stock_type);
		strcpy(pRsp->stock_name, stock_name);
		pRsp->hold_amount = hold_amount;
		pRsp->current_amount = current_amount;
		pRsp->cost_price = cost_price;
		pRsp->income_balance = income_balance;
		pRsp->hand_flag = hand_flag;
		pRsp->frozen_amount = frozen_amount;
		pRsp->unfrozen_amount = unfrozen_amount;
		pRsp->enable_amount = enable_amount;
		pRsp->real_buy_amount = real_buy_amount;
		pRsp->real_sell_amount = real_sell_amount;
		pRsp->uncome_buy_amount = uncome_buy_amount;
		pRsp->uncome_sell_amount = uncome_sell_amount;
		pRsp->entrust_sell_amount = entrust_sell_amount;
		pRsp->asset_price = asset_price;
		pRsp->last_price = last_price;
		pRsp->market_value = market_value;
		strcpy(pRsp->position_str, position_str);
		pRsp->sum_buy_amount = sum_buy_amount;
		pRsp->sum_buy_balance = sum_buy_balance;
		pRsp->sum_sell_amount = sum_sell_amount;
		pRsp->sum_sell_balance = sum_sell_balance;
		pRsp->real_buy_balance = real_buy_balance;
		pRsp->real_sell_balance = real_sell_balance;
		pRsp->delist_flag = delist_flag;
		pRsp->delist_date = delist_date;
		pRsp->par_value = par_value;


		lpUnPacker->Next();
	}

	return 0;
}

int CZQCodec::ReqFunction_330300(REQ_ZQDMXXCX_330300* pReq)
{
	int iRet = 0, hSend = 0;
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(330300);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///其他的应答信息
	LPRET_DATA pRetData = NULL;
	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("op_branch_no", 'I', 5);//名字 类型 长度
	pPacker->AddField("op_entrust_way", 'C', 1);
	pPacker->AddField("op_station", 'S', 255);
	pPacker->AddField("query_type", 'C', 1);
	pPacker->AddField("exchange_type", 'S', 4);
	pPacker->AddField("stock_type", 'S', 4);
	pPacker->AddField("stock_code", 'S', 16);
	pPacker->AddField("position_str", 'S', 100);
	pPacker->AddField("request_num", 'I', 10);

	///加入对应的字段值
	pPacker->AddInt(pReq->op_branch_no);
	pPacker->AddChar(pReq->op_entrust_way);
	pPacker->AddStr(pReq->op_station);
	pPacker->AddChar(pReq->query_type);
	pPacker->AddStr(pReq->exchange_type);
	pPacker->AddStr(pReq->stock_type);
	pPacker->AddStr(pReq->stock_code);
	pPacker->AddStr(pReq->position_str);
	pPacker->AddInt(pReq->request_num);

	pPacker->EndPack();
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CZQCodec::OnResponse_330300(IF2UnPacker *lpUnPacker, RSP_ErrorField2* pError, RSP_ZQDMXXCX_330300* pRsp)
{
	if (0 != IsError(lpUnPacker, pError))
	{
		return -1;
	}

	int count = lpUnPacker->GetRowCount();
	for (int i = 0; i < count; ++i)
	{
		const char* internal_code = lpUnPacker->GetStr("internal_code");
		const char* exchange_type = lpUnPacker->GetStr("exchange_type");
		const char* stock_code = lpUnPacker->GetStr("stock_code");
		const char* stock_name = lpUnPacker->GetStr("stock_name");
		int buy_unit = lpUnPacker->GetInt("buy_unit");
		int price_step = lpUnPacker->GetInt("price_step");
		int store_unit = lpUnPacker->GetInt("store_unit");
		double up_price = lpUnPacker->GetDouble("up_price");
		double down_price = lpUnPacker->GetDouble("down_price");
		const char* stock_type = lpUnPacker->GetStr("stock_type");
		int high_amount = lpUnPacker->GetInt("high_amount");
		int low_amount = lpUnPacker->GetInt("low_amount");
		char stock_real_back = lpUnPacker->GetChar("stock_real_back");
		char fund_real_back = lpUnPacker->GetChar("fund_real_back");
		char delist_flag = lpUnPacker->GetChar("delist_flag");
		int delist_date = lpUnPacker->GetInt("delist_date");
		char trade_plat = lpUnPacker->GetChar("trade_plat");
		const char* position_str = lpUnPacker->GetStr("position_str");
		double par_value = lpUnPacker->GetDouble("par_value");
		char stbtrans_type = lpUnPacker->GetChar("stbtrans_type");
		char stbtrans_status = lpUnPacker->GetChar("stbtrans_status");
		const char* sub_stock_type = lpUnPacker->GetStr("sub_stock_type");
		double low_balance = lpUnPacker->GetDouble("low_balance");
		double high_balance = lpUnPacker->GetDouble("high_balance");

		strcpy(pRsp->internal_code, internal_code);
		strcpy(pRsp->exchange_type, exchange_type);
		strcpy(pRsp->stock_code, stock_code);
		strcpy(pRsp->stock_name, stock_name);
		pRsp->buy_unit = buy_unit;
		pRsp->price_step = price_step;
		pRsp->store_unit = store_unit;
		pRsp->up_price = up_price;
		pRsp->down_price = down_price;
		strcpy(pRsp->stock_type, stock_type);
		pRsp->high_amount = high_amount;
		pRsp->low_amount = low_amount;
		pRsp->stock_real_back = stock_real_back;
		pRsp->fund_real_back = fund_real_back;
		pRsp->delist_flag = delist_flag;
		pRsp->delist_date = delist_date;
		pRsp->trade_plat = trade_plat;
		strcpy(pRsp->position_str, position_str);
		pRsp->par_value = par_value;
		pRsp->stbtrans_type = stbtrans_type;
		pRsp->stbtrans_status = stbtrans_status;
		strcpy(pRsp->sub_stock_type, sub_stock_type);
		pRsp->low_balance = low_balance;
		pRsp->high_balance = high_balance;


		lpUnPacker->Next();
	}

	return 0;
}

//620001_12订阅证券成交回报功能
int CZQCodec::SubFunction_620001(SUB_620001* pReq)
{
	int iRet = 0, hSend = 0;
	IF2UnPacker *pMCUnPacker = NULL;

	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		printf("取打包器失败!\r\n");
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;

	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();

	///应答业务消息
	IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(620001);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///开始打包
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("branch_no", 'I', 5);
	pPacker->AddField("fund_account", 'S', 18);
	pPacker->AddField("op_branch_no", 'I', 5);
	pPacker->AddField("op_entrust_way", 'C', 1);
	pPacker->AddField("op_station", 'S', 255);
	pPacker->AddField("client_id", 'S', 18);
	pPacker->AddField("password", 'S', 50);
	pPacker->AddField("user_token", 'S', 512);
	pPacker->AddField("issue_type", 'I', 8);

	///加入对应的字段值
	pPacker->AddInt(pReq->branch_no);
	pPacker->AddStr(pReq->fund_account);
	pPacker->AddInt(pReq->op_branch_no);
	pPacker->AddChar(pReq->op_entrust_way);
	pPacker->AddStr(pReq->op_station);	//op_station
	pPacker->AddStr(pReq->client_id);
	pPacker->AddStr(pReq->password);
	pPacker->AddStr(pReq->user_token);
	pPacker->AddInt(pReq->issue_type);
	///结束打包
	pPacker->EndPack();
	lpBizMessage->SetKeyInfo(pPacker->GetPackBuf(), pPacker->GetPackLen());
	m_lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	iRet = lpBizMessage->GetPacketId();
	lpBizMessage->Release();

	return iRet;
}

int CZQCodec::OnResponse_620003_12(IF2UnPacker *lpUnPacker)
{
	int init_date = lpUnPacker->GetInt("init_date");
	int branch_no = lpUnPacker->GetInt("branch_no");
	const char *client_id = lpUnPacker->GetStr("client_id");
	const char *fund_account = lpUnPacker->GetStr("fund_account");
	char real_type = lpUnPacker->GetChar("real_type");
	char real_status = lpUnPacker->GetChar("real_status");
	const char *exchange_type = lpUnPacker->GetStr("exchange_type");
	const char *report_seat = lpUnPacker->GetStr("report_seat");
	int report_no = lpUnPacker->GetInt("report_no");
	const char *report_account = lpUnPacker->GetStr("report_account");
	const char *stock_account = lpUnPacker->GetStr("stock_account");
	const char *stock_code = lpUnPacker->GetStr("stock_code");
	const char *stock_type = lpUnPacker->GetStr("stock_type");
	int record_no = lpUnPacker->GetInt("record_no");
	char report_bs = lpUnPacker->GetChar("report_bs");
	char entrust_bs = lpUnPacker->GetChar("entrust_bs");
	char entrust_status = lpUnPacker->GetChar("entrust_status");
	int entrust_no = lpUnPacker->GetInt("entrust_no");
	const char *opp_account = lpUnPacker->GetStr("opp_account");
	int business_no = lpUnPacker->GetInt("business_no");
	double business_amount = lpUnPacker->GetDouble("business_amount");
	double business_price = lpUnPacker->GetDouble("business_price");
	int business_time = lpUnPacker->GetInt("business_time");
	int report_time = lpUnPacker->GetInt("report_time");
	double entrust_amount = lpUnPacker->GetDouble("entrust_amount");
	int batch_no = lpUnPacker->GetInt("batch_no");
	double entrust_price = lpUnPacker->GetDouble("entrust_price");
	char entrust_type = lpUnPacker->GetChar("entrust_type");



	//// 原来这里有成交、有撤单，全走这一个渠道，所以要在这里
	//if (real_type == real_type_CD)
	//{
	//	return;
	//}

	////成交回报，只是生成TradeField就可以了吗？
	////这个地方撤单也会走它，所以需要注意了
	//OrderIDType orderId = { 0 };
	//sprintf(orderId, "%d", entrust_no);

	//unordered_map<string, REQ_PTWT_333002*>::iterator it2 = m_id_api_order.find(orderId);
	//if (it2 == m_id_api_order.end())
	//{
	//}
	//else
	//{
	//	REQ_PTWT_333002* pWTField = it2->second;

	//	TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));
	//	strcpy(pField->InstrumentID, stock_code);
	//	strcpy(pField->ExchangeID, exchange_type);
	//	//pField->Side = TThostFtdcDirectionType_2_OrderSide(pTrade->Direction);
	//	pField->Qty = business_amount;
	//	pField->Price = business_price;
	//	/*pField->OpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pTrade->OffsetFlag);
	//	pField->HedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pTrade->HedgeFlag);*/
	//	pField->Commission = 0;//TODO收续费以后要计算出来
	//	pField->Time = business_time;
	//	sprintf(pField->TradeID, "%d", report_no);

	//	sprintf(pField->ID, "%d", pWTField->entrust_no);

	//	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnTrade, m_msgQueue, m_pClass, true, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
	//}

	return 0;
}

int CZQCodec::OnResponse_620003_23(IF2UnPacker *lpUnPacker)
{
	double business_amount = lpUnPacker->GetDouble("business_amount");
	double business_price = lpUnPacker->GetDouble("business_price");
	double entrust_price = lpUnPacker->GetDouble("entrust_price");
	double entrust_amount = lpUnPacker->GetDouble("entrust_amount");
	const char *stock_code = lpUnPacker->GetStr("stock_code");
	char entrust_bs = lpUnPacker->GetChar("entrust_bs");
	int entrust_no = lpUnPacker->GetInt("entrust_no");
	int report_no = lpUnPacker->GetInt("report_no");
	int batch_no = lpUnPacker->GetInt("batch_no");
	char entrust_type = lpUnPacker->GetChar("entrust_type");
	char entrust_status = lpUnPacker->GetChar("entrust_status");
	const char *fund_account = lpUnPacker->GetStr("fund_account");
	int branch_no = lpUnPacker->GetInt("branch_no");

	// 委托回报，需要查找下单时的原始信息，因为有些信息用当前内容无法完全还原
	// 需要找到原始的OrderField用来向上层通知，然后更新一下内存中的一些信息
	//if (entrust_type)
	//{
	//}

	return 0;
}