#ifndef _UFX_STRUCT_ZQ_H_
#define _UFX_STRUCT_ZQ_H_

#pragma once

struct RSP_ErrorField2
{
	int		error_no;
	char	error_info[256];
};

//331100 登入
struct REQ_DL_331100
{
    int     op_branch_no;
    char    op_entrust_way;
    char    op_station[256];
    int     branch_no;
	char    password[51];
	char    password_type;
    char    input_content;
    char    account_content[31];
    char    content_type[7];
	char	asset_prop;
};

// 登录回报
struct RSP_DL_331100
{
    int     init_date;
    int     branch_no;
    char    client_id[19];
    char    user_token[513];
    int     sysnode_id;
    int     op_branch_no;
    char    client_name[15];
};

//400 证券行情查询
struct REQ_ZQHQCX_400
{
    char    exchange_type[5];
    char    stock_code[17];
};

struct RSP_ZQHQCX_400
{
	char	exchange_index[33];
	double	last_price;
	double	open_price;
	double	close_price;
	double	high_price;
	double	low_price;
	double	business_balance;
	int		business_amount;
	double	buy_price1;
	double	buy_price2;
	double	buy_price3;
	double	buy_price4;
	double	buy_price5;
	double	sale_price1;
	double	sale_price2;
	double	sale_price3;
	double	sale_price4;
	double	sale_price5;
	int		buy_amount1;
	int		buy_amount2;
	int		buy_amount3;
	int		buy_amount4;
	int		buy_amount5;
	int		sale_amount1;
	int		sale_amount2;
	int		sale_amount3;
	int		sale_amount4;
	int		sale_amount5;
	double	stock_interest;
	char	stock_name[33];
	int		marketmaker_amount;
};

//333002 普通委托请求
struct REQ_PTWT_333002
{
    int     op_branch_no;
    char    op_entrust_way;
    char    op_station[256];
    int     branch_no;
    char    client_id[19];
    char    fund_account[19];
    char    password[51];
    char    password_type;
    char    user_token[513];
    char    exchange_type[5];
    char    stock_account[12];
    char    stock_code[16];
    double  entrust_amount;
    double  entrust_price;
    char    entrust_bs;
    char    entrust_prop[4];
    int     batch_no;

	// 以下内容会在柜台收到后返回，放在这地方为了撤单方便
	int		init_date;
	int		entrust_no;
	int		report_no;
	char	seat_no[7];
	int		entrust_time;

	// 后期推送的委托回报
	double  business_amount;
	double  business_price;
	int		record_no;
	char    entrust_type;
	char    entrust_status;

	// 收到回报后要查两次map太麻烦，直接保存指针更快
	void*	pUserData;
};

struct RSP_PTWT_333002
{
	int		init_date;
	int		entrust_no;
	int		batch_no;
	int		report_no;
	char	seat_no[7];
	int		entrust_time;
};

//333017 委托撤单
struct REQ_WTCD_333017
{
    int     op_branch_no;
    char    op_entrust_way;
    char    op_station[256];
    int     branch_no;
    char    client_id[19];
    char    fund_account[19];
    char    password[50];
    char    password_type;
    char    user_token[513];
    char    batch_flag;
    char    exchange_type[5];
    int     entrust_no;
};

//333017 委托撤单回报
struct RSP_WTCD_333017
{
	
};

//332254 资金快速查询
struct REQ_ZJKSCX_332254
{
	int     op_branch_no;
	char    op_entrust_way;
	char    op_station[256];
	int     branch_no;
	char    client_id[19];
	char    fund_account[19];
	char    password[51];
	char    password_type;
	char    user_token[513];
	char	momoney_type[4];
};

struct RSP_ZJKSCX_332254
{
	char	momoney_type[4];
	double	current_balance;
	double	enable_balance;
	double	fetch_balance;
	double	frozen_balance;
	double	unfrozen_balance;
	double	fund_balance;
};

//333103 持仓快速查询
struct REQ_CCKSCX_333103
{
	int     op_branch_no;
	char    op_entrust_way;
	char    op_station[256];
	int     branch_no;
	char    client_id[19];
	char    fund_account[19];
	char    password[51];
	char    password_type;
	char    user_token[513];
	char	exchange_type[5];
	char	stock_account[12];
	char	stock_code[17];
	char	position_str[101];
	int		request_num;
};

struct RSP_CCKSCX_333103
{
	char	fund_account[19];
	int		ffare_kind;
	int		bfare_kind;
	char	profit_flag;
	char	exchange_type[5];
	char	stock_account[12];
	char	stock_code[17];
	char	stock_type[5];
	char	stock_name[33];
	int		hold_amount;
	int		current_amount;
	double	cost_price;
	double	income_balance;
	char	hand_flag;
	int		frozen_amount;
	int		unfrozen_amount;
	int		enable_amount;
	int		real_buy_amount;
	int		real_sell_amount;
	int		uncome_buy_amount;
	int		uncome_sell_amount;
	int		entrust_sell_amount;
	double	asset_price;
	double	last_price;
	double	market_value;
	char	position_str[101];
	int		sum_buy_amount;
	double	sum_buy_balance;
	int		sum_sell_amount;
	double	sum_sell_balance;
	double	real_buy_balance;
	double	real_sell_balance;
	char	delist_flag;
	int		delist_date;
	double	par_value;
};

//330300 证券代码信息查询
struct REQ_ZQDMXXCX_330300
{
	int     op_branch_no;
	char    op_entrust_way;
	char    op_station[256];
	char	query_type;
	char	exchange_type[5];
	char	stock_type[5];
	char	stock_code[17];
	char	position_str[101];
	int		request_num;
};

struct RSP_ZQDMXXCX_330300
{
	char	internal_code[7];
	char	exchange_type[5];
	char	stock_code[17];
	char	stock_name[33];
	int		buy_unit;
	int		price_step;
	int		store_unit;
	double	up_price;
	double	down_price;
	char	stock_type[5];
	int		high_amount;
	int		low_amount;
	char	stock_real_back;
	char	fund_real_back;
	char	delist_flag;
	int		delist_date;
	char	trade_plat;
	char	position_str[101];
	double	par_value;
	char	stbtrans_type;
	char	stbtrans_status;
	char	sub_stock_type[5];
	double	low_balance;
	double	high_balance;
};

// 620001 主动订阅
struct SUB_620001
{
	int		branch_no;
	char    fund_account[19];
	int     op_branch_no;
	char    op_entrust_way;
	char    op_station[256];
	char    client_id[19];
	char    password[51];
	char    user_token[513];
	int		issue_type;
};

//620003_12证券成交回报
struct RSP_CJHB_620003_12
{
	int		init_date;
	int		branch_no;
	char    client_id[19];
	char    fund_account[19];
	char    real_type;
	char    real_status;
	char    exchange_type[5];
	char    report_seat[7];
	int     report_no;
	char    report_account[12];
	char    stock_account[12];
	char    stock_code[7];
	char    stock_type[5];
	int		record_no;
	char    report_bs;
	char    entrust_bs;
	char    entrust_status;
	int	    entrust_no;
	char    opp_account[12];
	int		business_no;
	double  business_amount;
	double  business_price;
	int     business_time;
	int     report_time;
	double  entrust_amount;
	int     batch_no;
	double  entrust_price;
	char    entrust_type;
};

//620003_12证券委托回报
struct RSP_CJHB_620003_23
{
	double  business_amount;
	double  business_price;
	double  entrust_price;
	double  entrust_amount;
	char    stock_code[7];
	char    entrust_bs;
	int	    entrust_no;
	int		record_no;
	int     batch_no;
	char    entrust_type;
	char    entrust_status;
	char    fund_account[19];
	int     branch_no;
};


#endif