#ifndef _UFX_STRUCT_O32_H_
#define _UFX_STRUCT_O32_H_

#pragma once

/*
新旧两版UFX的部分信息不同，简单列出

参数名			旧		新		决定采用
asset_no		C32		C16		C32
combi_no		C8		C16		C16
extsystem_id	C20		N8		C20
position_str	C100	C32		C100

extsystem_id一定要控制成数字，同时处理好不要溢出，否则会找不到订单
*/

struct RSP_ErrorField3
{
	int		ErrorCode;
	char	ErrorMsg[256];
	char	MsgDetail[256];
	int		DataCount;
};

// 风控信息，在下单时返回
struct RSP_RiskField
{
	int		risk_serial_no;	// 一般等于委托号
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    entrust_direction[4 + 1];
	char    futures_direction;
	int		risk_no;
	char	risk_type;
	char	risk_summary[128 + 1];
	char	risk_operation;
	char	remark_short[128 + 1];
	double	risk_threshold_value;
	double	risk_value;
};

// 10000 心跳 
struct REQ_XT_10000
{
	char    user_token[512 + 1];
};

// 10000 心跳 
struct RSP_XT_10000
{
	char    user_token[512 + 1];
};

// 10001 登录 
struct REQ_DL_10001
{
	char	operator_no[16 + 1];
	char    password[16 + 1];
	char    mac_address[20 + 1];
	char    ip_address[15 + 1];
	char    hd_volserial[15 + 1];
	char    op_station[256 + 1];
	char    terminal_info[512 + 1];
	char    authorization_id[64 + 1];
};

// 10001 登录 
struct RSP_DL_10001
{
	char    user_token[512 + 1];
	char    version_no[32 + 1];
};

// 91090 篮子委托
struct REQ_LZWT_91090
{
	char    user_token[512 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char    stockholder_id[20 + 1];
	char    report_seat[6 + 1];
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    entrust_direction[4 + 1];
	char    futures_direction;
	char    close_direction;
	char    covered_flag;
	char	price_type;
	double  entrust_price;
	double  entrust_amount;
	double  limit_entrust_ratio;
	double  ftr_entrust_ratio;
	double  opt_entrust_ratio;
	char    third_reff[256 + 1];
	int		extsystem_id;

	// 以下内容会在柜台收到后返回，放在这地方为了撤单方便
	int     batch_no;

	//// 后期推送的委托回报
	//double  business_amount;
	//double  business_price;
	//int		record_no;
	//char    entrust_type;
	//char    entrust_status;

	//// 收到回报后要查两次map太麻烦，直接保存指针更快
	//void*	pUserData;
};

// 91090 篮子委托
struct RSP_LZWT_91090
{
	int		batch_no;
	int     entrust_no;
	int		extsystem_id;
	int		entrust_fail_code;
	char	fail_cause[256 + 1];
	int		risk_serial_no;
};

// 91102 委托撤单（按委托批号撤单）
struct REQ_LZCD_91102
{
	char    user_token[512 + 1];
	int     batch_no;
};

// 91102 委托撤单（按委托批号撤单）
struct RSP_LZCD_91102
{
	int		entrust_no;
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char	success_flag;
	char	fail_cause[256 + 1];
};

// 31001 证券持仓查询
struct REQ_ZQCCCX_31001
{
	char    user_token[512 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    stockholder_id[20 + 1];
	char    hold_seat[6 + 1];
};

// 31001 证券持仓查询
struct RSP_ZQCCCX_31001
{
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    stock_name[32 + 1];
	char    stockholder_id[20 + 1];
	char    hold_seat[6 + 1];
	char	position_flag;
	double	current_amount;
	double	enable_amount;
	double	begin_amount;
	double	begin_cost;
	double	current_cost;
	double	current_cost_price;
	double	pre_buy_amount;
	double	pre_sell_amount;
	double	pre_buy_balance;
	double	pre_sell_balance;
	double	buy_amount;
	double	sell_amount;
	double	buy_balance;
	double	sell_balance;
	double	buy_fee;
	double	sell_fee;

	char	invest_type;
	char	option_type;
	double	last_price;
	double	today_buy_amount;
	double	today_sell_amount;
	double	today_buy_balance;
	double	today_sell_balance;
	double	today_buy_fee;
	double	today_sell_fee;
	char	stock_type;
	double	cost_price;
	double	floating_profit;
	double	accumulate_profit;
	double	total_profit;
};


// 31003 期货持仓查询
struct REQ_QHCCCX_31003
{
	char    user_token[512 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    stockholder_id[20 + 1];
	char    hold_seat[6 + 1];
	char	invest_type;
	char	position_flag;
};

// 31003 期货持仓查询
struct RSP_QHCCCX_31003
{
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    stock_name[32 + 1];
	char    stockholder_id[20 + 1];
	char    hold_seat[6 + 1];
	char	position_flag;

	double	current_amount;
	double	today_amount;
	double	lastday_amount;
	double	enable_amount;
	double	today_enable_amount;
	double	lastday_enable_amount;
	double	begin_cost;
	double	current_cost;
	double	current_cost_price;
	double	pre_buy_amount;
	double	pre_sell_amount;
	double	pre_buy_balance;
	double	pre_sell_balance;
	double	buy_amount;
	double	sell_amount;
	double	buy_balance;
	double	sell_balance;
	double	buy_fee;
	double	sell_fee;

	char	invest_type;
	char	option_type;
	double	last_price;
	double	today_buy_amount;
	double	today_sell_amount;
	double	today_buy_balance;
	double	today_sell_balance;
	double	today_buy_fee;
	double	today_sell_fee;
	char	stock_type;
	double	cost_price;
	double	floating_profit;
	double	accumulate_profit;
	double	total_profit;
};

// 34003 期货保证金账户查询
struct REQ_QHBZJZHCX_34003
{
	char    user_token[512 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
};

// 34003 期货保证金账户查询
struct RSP_QHBZJZHCX_34003
{
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	double	occupy_deposit_balance;
	double	enable_deposit_balance;
	double	futu_deposit_balance;
	double	futu_temp_occupy_deposit;
};

// 34001 账户资金查询
struct REQ_ZHZJCX_34001
{
	char    user_token[512 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
};

// 34001 账户资金查询
struct RSP_ZHZJCX_34001
{
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	double	enable_balance_t0;
	double	enable_balance_t1;
	double	current_balance;
};

// 32003 期货委托查询
struct REQ_QHWTCX_32003
{
	char    user_token[512 + 1];
	int		batch_no;
	int		entrust_no;
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	//int		instance_no;
	char    stockholder_id[20 + 1];
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    entrust_direction[4 + 1];
	char    futures_direction;
	char	entrust_state_list[256 + 1];
	int		extsystem_id;
	char    third_reff[256 + 1];
	char	position_str[32 + 1];
	int		request_num;
};

// 32003 期货委托查询
struct RSP_QHWTCX_32003
{
	int		entrust_date;
	int		entrust_time;
	char	operator_no[16 + 1];
	int		batch_no;
	int		entrust_no;
	char	report_no[64 + 1];
	int		extsystem_id;
	char	third_reff[256 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char	instance_no[128+1];
	char	stockholder_id[20 + 1];
	char	report_seat[6 + 1];
	char	market_no[3 + 1];
	char	stock_code[16 + 1];
	char	entrust_direction[4 + 1];
	char	futures_direction;
	char	close_direction;
	char	price_type;
	double	entrust_price;
	double	entrust_amount;
	double	pre_buy_frozen_balance;
	double	pre_sell_balance;
	char	confirm_no[32 + 1];
	char	entrust_state;

	int		first_deal_time;
	double	deal_amount;
	double	deal_balance;
	double	deal_price;
	int		deal_times;
	double	withdraw_amount;
	char	withdraw_cause[256 + 1];
	char	position_str[32 + 1];
	char	exchange_report_no[64 + 1];
};

// 33003 期货成交查询
struct REQ_QHCJCX_33003
{
	char    user_token[512 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	int		entrust_no;
	char	deal_no[64 + 1];
	char    stockholder_id[20 + 1];
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    entrust_direction[4 + 1];
	char    futures_direction;
	int		extsystem_id;
	char    third_reff[256 + 1];
	char	position_str[32 + 1];
	int		request_num;
};

// 33003 期货成交查询
struct RSP_QHCJCX_33003
{
	int		deal_date;
	char	deal_no[64 + 1];
	int		entrust_no;
	int		extsystem_id;
	char	third_reff[256 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char	instance_no[128 + 1];
	char	stockholder_id[20 + 1];
	char	market_no[3 + 1];
	char	stock_code[16 + 1];
	char	entrust_direction[4 + 1];
	char	futures_direction;
	double	deal_amount;
	double	deal_price;
	double	deal_balance;
	double	total_fee;
	int		deal_time;
	char	close_type;
	char	position_str[32 + 1];
	char	exchange_report_no[64 + 1];
};

// 32001 证券委托查询
struct REQ_ZQWTCX_32001
{
	char    user_token[512 + 1];
	int		batch_no;
	int		entrust_no;
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	//int		instance_no;
	char    stockholder_id[20 + 1];
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    entrust_direction[4 + 1];
	//char    futures_direction;
	char	entrust_state_list[256 + 1];
	int		extsystem_id;
	char    third_reff[256 + 1];
	char	position_str[32 + 1];
	int		request_num;
};

// 32001 证券委托查询
struct RSP_ZQWTCX_32001
{
	int		entrust_date;
	int		entrust_time;
	char	operator_no[16 + 1];
	int		batch_no;
	int		entrust_no;
	char	report_no[64 + 1];
	int		extsystem_id;
	char	third_reff[256 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char    instance_no[128 + 1];
	char	stockholder_id[20 + 1];
	char	report_seat[6 + 1];
	char	market_no[3 + 1];
	char	stock_code[16 + 1];
	char	entrust_direction[4 + 1];
	//char	futures_direction;
	char	price_type;
	double	entrust_price;
	double	entrust_amount;
	double	pre_buy_frozen_balance;
	double	pre_sell_balance;
	char	confirm_no[32 + 1];
	char	entrust_state;
	int		first_deal_time;
	double	deal_amount;
	double	deal_balance;
	double	deal_price;
	int		deal_times;
	double	withdraw_amount;
	char	withdraw_cause[256 + 1];
	char	position_str[32 + 1];
	char	exchange_report_no[64 + 1];
};

// 33001 证券成交查询
struct REQ_ZQCJCX_33001
{
	char    user_token[512 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	int		entrust_no;
	char	deal_no[64 + 1];
	char    stockholder_id[20 + 1];
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    entrust_direction[4 + 1];
	//char    futures_direction;
	int		extsystem_id;
	char    third_reff[256 + 1];
	char	position_str[32 + 1];
	int		request_num;
};

// 33001 证券成交查询
struct RSP_ZQCJCX_33001
{
	int		deal_date;
	char	deal_no[64 + 1];
	int		entrust_no;
	int		extsystem_id;
	char	third_reff[256 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char	instance_no[128 + 1];
	char	stockholder_id[20 + 1];
	char	market_no[3 + 1];
	char	stock_code[16 + 1];
	char	entrust_direction[4 + 1];
	//char	futures_direction;
	double	deal_amount;
	double	deal_price;
	double	deal_balance;
	double	total_fee;
	int		deal_time;
	char	position_str[32 + 1];
};

// 91004 期货委托
struct REQ_QHWT_91004
{
	char    user_token[512 + 1];
	int		batch_no;
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char    stockholder_id[20 + 1];
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    entrust_direction[4 + 1];
	char    futures_direction;
	char	close_direction;
	char	price_type;
	double  entrust_price;
	double  entrust_amount;
	double  limit_deal_amount;
	char    third_reff[256 + 1];
	int		extsystem_id;

	// 以下内容会在柜台收到后返回，放在这地方为了撤单方便
	int     entrust_no;

	// 收到回报后要查两次map太麻烦，直接保存指针更快
	void*	pUserData;
};

// 91004 期货委托
struct RSP_QHWT_91004
{
	int		batch_no;
	int     entrust_no;
	int		extsystem_id;
	int		entrust_fail_code;
	char	fail_cause[256 + 1];
	int		risk_serial_no;
};

// 91001 普通买卖委托
struct REQ_PTMMWT_91001
{
	char    user_token[512 + 1];
	int		batch_no;
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char    stockholder_id[20 + 1];
	char	report_seat[6 + 1];
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char    entrust_direction[4 + 1];
	char	price_type;
	double  entrust_price;
	double  entrust_amount;
	char    third_reff[256 + 1];
	int		extsystem_id;

	// 以下内容会在柜台收到后返回，放在这地方为了撤单方便
	int     entrust_no;

	// 收到回报后要查两次map太麻烦，直接保存指针更快
	void*	pUserData;
};

// 91001 普通买卖委托
struct RSP_PTMMWT_91001
{
	int		batch_no;
	int     entrust_no;
	int		extsystem_id;
	int		entrust_fail_code;
	char	fail_cause[256 + 1];
	int		risk_serial_no;
};

// 91105 期货委托撤单
struct REQ_QHWTCD_91105
{
	char    user_token[512 + 1];
	int     entrust_no;
};

// 91105 期货委托撤单
struct RSP_QHWTCD_91105
{
	int     entrust_no;
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char	success_flag;
	char	fail_cause[256 + 1];
	//char	entrust_state;
	//char	entrust_status;
};

// 91101 委托撤单
struct REQ_WTCD_91101
{
	char    user_token[512 + 1];
	int     entrust_no;
};

// 91101 委托撤单
struct RSP_WTCD_91101
{
	int     entrust_no;
	char    market_no[3 + 1];
	char    stock_code[16 + 1];
	char	success_flag;
	char	fail_cause[256 + 1];
	//char	entrust_state;
};
// ======================

// a 委托下达
struct RSP_WTXD_a
{
	char	msgtype;
	char	operator_no[16 + 1];
	int		business_date;
	int		business_time;
	int		batch_no;
	int		entrust_no;
	char	report_no[64 + 1];
	char    account_code[32 + 1];
	char    asset_no[16 + 1];
	char    combi_no[16 + 1];
	char	instance_no[128 + 1];
	char    stockholder_id[20 + 1];
	char	report_seat[6 + 1];
	char	market_no[3 + 1];
	char	stock_code[16 + 1];
	char	entrust_direction[4 + 1];
	char	futures_direction;
	char	close_direction;
	char	price_type;
	double	entrust_price;
	int		entrust_amount;
	char	invest_type;
	char	entrust_state;
	char	entrust_status;
	int		extsystem_id;
	char    third_reff[256 + 1];
};

// b 委托确认
struct RSP_WTQR_b
{
	char	msgtype;
	char	operator_no[16+1];
	int		business_date;
	int		business_time;
	int		batch_no;
	int		entrust_no;
	char    account_code[32 + 1];
	char    combi_no[16 + 1];
	char	instance_no[128 + 1];
	char    stockholder_id[20 + 1];
	char	report_seat[6 + 1];
	char	market_no[3 + 1];
	char	stock_code[16 + 1];
	char	entrust_direction[4 + 1];
	char	futures_direction;
	char	close_direction;
	char	price_type;
	double	entrust_price;
	int		entrust_amount;
	char	invest_type;
	char	entrust_state;
	char	entrust_status;
	int		extsystem_id;
	char    third_reff[256 + 1];
	char	confirm_no[32 + 1];
};

// c 委托废单
struct RSP_WTFD_c
{
	char	msgtype;
	char	operator_no[16 + 1];
	int		business_date;
	int		business_time;
	int		batch_no;
	int		entrust_no;
	char	entrust_state;
	char	entrust_status;
	int		extsystem_id;
	char    third_reff[256 + 1];
	char    revoke_cause[256 + 1];
	char	market_no[3 + 1];
	char	stock_code[16 + 1];
	char	entrust_direction[4 + 1];
	char	futures_direction;
	int		entrust_amount;
};

// d 委托撤单
struct RSP_WTCD_d
{
	char	msgtype;
	char	operator_no[16 + 1];
	int		business_date;
	int		business_time;
	int		batch_no;
	int		entrust_no;
	int		cancel_entrust_no;
	char	entrust_state;
	char	entrust_status;
	int		extsystem_id;
	char    third_reff[256 + 1];
	char    revoke_cause[256 + 1];
	char	market_no[3 + 1];
	char	stock_code[16 + 1];
};

// e 委托撤成
struct RSP_WTCC_e
{
	char	msgtype;
	char	operator_no[16 + 1];
	int		business_date;
	int		business_time;
	int		batch_no;
	int		entrust_no;
	int		cancel_entrust_no;
	char    account_code[32 + 1];
	char    combi_no[16 + 1];
	char	instance_no[128 + 1];
	char    stockholder_id[20 + 1];
	char	report_seat[6 + 1];
	char	market_no[3 + 1];
	char	stock_code[16 + 1];
	char	entrust_direction[4 + 1];
	char	futures_direction;
	int		entrust_amount;
	int		cancel_amount;
	char	invest_type;
	char	entrust_state;
	char	entrust_status;
	int		extsystem_id;
	char    third_reff[256 + 1];
};

// f 委托撤废
struct RSP_WTCD_f
{
	char	msgtype;
	char	operator_no[16 + 1];
	int		batch_no;
	int		entrust_no;
	int		cancel_entrust_no;
	char	entrust_state;
	char	entrust_status;
	int		extsystem_id;
	char    third_reff[256 + 1];
	char    revoke_cause[256 + 1];
	char	market_no[3 + 1];
	char	stock_code[16 + 1];
};

// g 委托成交
struct RSP_WTCJ_g
{
	char	msgtype;
	char	operator_no[16 + 1];
	int		deal_date;
	int		deal_time;
	char	deal_no[64 + 1];
	int		batch_no;
	int		entrust_no;
	char	market_no[3 + 1];
	char	stock_code[16 + 1];
	char    account_code[32 + 1];
	char    combi_no[16 + 1];
	char	instance_no[128 + 1];
	char	stockholder_id[20 + 1];
	char	report_seat[6 + 1];
	char	entrust_direction[4 + 1];
	char	futures_direction;
	double	entrust_amount;
	char	entrust_state;
	char	entrust_status;
	double	deal_amount;
	double	deal_price;
	double	deal_balance;
	double	deal_fee;
	double	total_deal_amount;
	double	total_deal_balance;
	int		cancel_amount;
	int		extsystem_id;
	char	third_reff[256 + 1];
	char	close_type;
	char	report_direction[2 + 1];
};

// 主推订阅
struct SUB_
{
	char	operator_no[16 + 1];
	char	market_no[3 + 1];
};
#endif