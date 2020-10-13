#ifndef _UFX_ENUM_O32_H_
#define _UFX_ENUM_O32_H_

#pragma once

#define	market_no_SSE			"1"			// 上交所
#define	market_no_SZSE			"2"			// 深交所
#define	market_no_SHFE			"3"			// 上期所
#define	market_no_CZCE			"4"			// 郑商所
#define	market_no_CFFEX			"7"			// 中金所
#define	market_no_DCE			"9"			// 大商所
#define market_no_INE			"k"			// 能源交易所
#define	market_no_NEEQ			"10"		// 股转市场
#define	market_no_HK_SH			"35"		// 港股通（沪）
#define	market_no_HK_SZ			"o"			// 港股通（深）
typedef char _market_no[4];

// 转换后，可以用switch了
#define	dword_market_no_SSE			'1'			// 上交所
#define	dword_market_no_SZSE			'2'			// 深交所
#define	dword_market_no_SHFE			'3'			// 上期所
#define	dword_market_no_CZCE			'4'			// 郑商所
#define	dword_market_no_CFFEX			'7'			// 中金所
#define	dword_market_no_DCE			'9'			// 大商所
#define dword_market_no_INE			'k'			// 能源交易所
#define	dword_market_no_NEEQ			'10'		// 股转市场
#define	dword_market_no_HK_SH			'35'		// 港股通（沪）
#define	dword_market_no_HK_SZ			'o'			// 港股通（深）
typedef int _dword_market_no;

#define	entrust_direction_MR			"1"		// 买入
#define	entrust_direction_MC			"2"		// 卖出
#define	entrust_direction_ZQMR			"3"		// 债券买入
#define	entrust_direction_ZQMC			"4"		// 债券卖出
#define	entrust_direction_ZHG			"5"		// 融资(正)回购
#define	entrust_direction_NHG			"6"		// 融券(逆)回购
#define	entrust_direction_PGRG			"9"		// 配股认购
#define	entrust_direction_ZZG			"10"	// 债转股
#define	entrust_direction_ZHS			"11"	// 债回售
#define	entrust_direction_SG			"12"	// 申购
#define	entrust_direction_JJRG			"13"	// 基金认购
#define	entrust_direction_ZTG			"17"	// 转托管
#define	entrust_direction_ETFSG			"26"	// ETF申购
#define	entrust_direction_ETFSH			"27"	// ETF赎回
#define	entrust_direction_XQRGou		"28"	// 行权认购
#define	entrust_direction_XQRGu			"29"	// 行权认沽
#define	entrust_direction_TJZR			"30"	// 提交质押
#define	entrust_direction_HXZR			"31"	// 回转质押
#define	entrust_direction_ "50"	// 基金分拆
#define	entrust_direction_ "51"	// 基金合并
#define	entrust_direction_ "53"	// 开基申购
#define	entrust_direction_ "54"	// 开基赎回
#define	entrust_direction_ "55"	// 债券认购
#define	entrust_direction_ "63"	// 保证券锁定（股票期权）, 担保品交存（融资融券）
#define	entrust_direction_ "64"	// 保证券解锁（股票期权）, 担保品提取（融资融券）
#define	entrust_direction_ "67"	// 融券卖出
#define	entrust_direction_ "68"	// 买券还券
#define	entrust_direction_ "69"	// 直接还款
#define	entrust_direction_ "70"	// 直接还券
#define	entrust_direction_ "75"	// 融资买入
#define	entrust_direction_ "76"	// 卖券还款
#define	entrust_direction_ "83"	// 放弃行权（郑商所商品期权）

typedef char _entrust_direction[4];
typedef char* p_entrust_direction;

#define	futures_direction_KC			'1'	// 开仓
#define	futures_direction_PC			'2'	// 平仓
#define	futures_direction_JG			'3'	// 交割
#define	futures_direction_PJC			'4'	// 平今仓
typedef char _futures_direction;

#define	close_direction_YXPJ		'0'	// 优先平今
#define	close_direction_PJ			'1'	// 平今
#define	close_direction_PZ			'2'	// 平昨
typedef char _close_direction;

#define	close_type_KC			'0'	// 开仓
#define	close_type_PTPC			'1'	// 普通平仓
#define	close_type_QZPC			'2'	// 强制平仓
#define	close_type_DQJG			'3'	// 到期交割
#define	close_type_PZC			'4'	// 平昨仓
#define	close_type_PJC			'5'	// 平今仓
typedef char _close_type;

#define	invest_type_KJY				'1'	// 可交易
#define	invest_type_CYDQ			'2'	// 持有到期
#define	invest_type_KGCS			'3'	// 可供出售
#define	invest_type_DKHYSKX			'4'	// 贷款和应收款项
#define	invest_type_TJ				'a'	// 投机(期货)
#define	invest_type_TB				'b'	// 套保(期货)
#define	invest_type_TL				'c'	// 套利(期货)	
typedef char _invest_type;

#define	position_flag_DTCC			'1'	// 多头持仓
#define	position_flag_KTCC			'2'	// 空头持仓
#define	position_flag_BDKT			'b'	// 备兑空头持仓（个股期权）
typedef char _position_flag;


// 感觉有些状态真的很奇怪
#define	entrust_state_WB	'1'	// 未报
#define	entrust_state_DB '2'	// 待报
#define	entrust_state_ZB '3'	// 正报
#define	entrust_state_YB '4'	// 已报
#define	entrust_state_FD '5'	// 废单
#define	entrust_state_BCheng '6'	// 部成
#define	entrust_state_YCheng '7'	// 已成
#define	entrust_state_BChe '8'	// 部撤
#define	entrust_state_YChe1 '9'	// 已撤
#define	entrust_state_DC1 'a'	// 待撤
#define	entrust_state_WSP 'b'	// 未审批
#define	entrust_state_SPJJ 'c'	// 审批拒绝
#define	entrust_state_WSPJCX 'd'	// 未审批即撤销
#define	entrust_state_WC 'A'	// 未撤
#define	entrust_state_DC2 'B'	// 待撤
#define	entrust_state_ZC 'C'	// 正撤
#define	entrust_state_CR 'D'	// 撤认
#define	entrust_state_CF 'E'	// 撤废
#define	entrust_state_YChe2 'F'	// 已撤

typedef char _entrust_state;

#define success_flag_CG '1'		// 成功
#define success_flag_SB '2'		// 失败
typedef char _success_flag;


#define price_type_XJ '0'	// 限价
#define price_type_RYJ '1'	// 任意价(期货)
#define price_type_SJSYZXJ '2'	// 市价剩余转限价（上交所股票期权）
#define price_type_SJSYCX '3'	// 市价剩余撤消（上交所股票期权）
#define price_type_FOKXJ '4'	// FOK限价（上交所股票期权）
#define price_type_FOKSJ '5'	// FOK市价（上交所股票期权）
#define price_type_WDJCSC 'a'	// 五档即成剩撤（上交所市价）
#define price_type_WDJCSZ 'b'	// 五档即成剩转（上交所市价）
#define price_type_XJB 'c'	// 限价盘（零股）(港股通)
#define price_type_JJXJB 'e'	// 竞价限价盘(港股通)
#define price_type_ZQXJP 'g'	// 增强现价盘(港股通)
#define price_type_ 'i'	// 竞价限价盘FOK(港股通)
#define price_type_ 'j'	// 增强现价盘FOK(港股通)
#define price_type_ 'k'	// 定价申报(股转市场)
#define price_type_ 'l'	// 协议转让成交确认申报(股转市场)
#define price_type_ 'm'	// 协议转让互报成交确认申报(股转市场)
#define price_type_ 'A'	// 五档即成剩撤（深交所市价）
#define price_type_ 'C'	// 即成剩撤（深交所市价）
#define price_type_ 'D'	// 对手方最优（深交所市价）
#define price_type_ 'E'	// 本方最优（深交所市价）
#define price_type_ 'F'	// 全额成或撤（FOK市价）（深交所市价）
#define price_type_ 'G'	// 全额成或撤(FOK限价)（上期所、中金所、深交所）
#define price_type_ 'K'	// 即成剩撤（FAK）（上期所、郑商所、中金所）
#define price_type_ 'X'	// 任意价转限价（中金所）
#define price_type_ 'L'	// 五档即成剩撤（中金所五档市价）
#define price_type_ 'M'	// 五档即成剩转（中金所五档市价转限价）
#define price_type_ 'N'	// 最优一档即成剩撤（中金所最优价）
#define price_type_ 'O'	// 最优一档即成剩转（中金所最优价）
typedef char _price_type;

#define instance_type_ 'A'	// 股指期货期现套利
#define instance_type_ 'B'	// 股指期货跨期套利
#define instance_type_ 'C'	// ETF套利
#define instance_type_ 'D'	// 国债期货期现套利
#define instance_type_ 'E'	// 国债期货跨期套利
#define instance_type_ 'F'	// 股票期权套利
#define instance_type_ 'Z'	// 自定义
typedef char _instance_type;

#define	currency_code_CNY	"CNY"	// 人民币
#define	currency_code_USD	"USD"	// 美元
#define	currency_code_HKD	"HKD"	// 港币(未开通深港通)
#define	currency_code_HK1	"HK1"	// 港币(沪港通)(开通深港通)
#define	currency_code_HK2	"HK2"	// 港币(深港通)(开通深港通)
typedef char _currency_code[4];


#define	risk_type_ZQCCSL 'A' // 证券持仓数量控制
#define	risk_type_ZQCCCB 'B' // 证券持仓成本控制
#define	risk_type_ZQCCSZ 'C' // 证券持仓市值控制
#define	risk_type_ZQSZAHY 'D' // 证券市值按行业控制（社保）
#define	risk_type_ZCLBSZ 'E' // 资产类别市值控制
#define	risk_type_JYE 'F' // 交易额控制
#define	risk_type_DRJYL 'G' // 当日交易量控制
#define	risk_type_ZXJ 'H' // 执行价格控制
#define	risk_type_ZQMM 'I' // 证券买卖控制
#define	risk_type_TXFX 'J' // 同向反向控制
#define	risk_type_SYTS 'K' // 剩余天数控制
#define	risk_type_DQHGZC 'L' // 到期回购资产控制
#define	risk_type_JYDSSXED 'M' // 交易对手授信额度控制
#define	risk_type_CK 'N' // 存款控制
#define	risk_type_HGZYQED 'P' // 回购质押券额度控制
#define	risk_type_JYPL 'Q' // 交易频率控制
#define	risk_type_QHZYBZZ 'R' // 期货占用保证金控制
#define	risk_type_HTTZ 'S' // 合同投资控制
#define	risk_type_ZCSZ 'T' // 资产数值控制
typedef char _risk_type;

#define	risk_operation_W '0'	// 无
#define	risk_operation_YJ '1'	// 预警
#define	risk_operation_JZ '2'	// 禁止
#define	risk_operation_SQZLSP '3'	// 申请指令审批
#define	risk_operation_SQFXFZ '4'	// 申请风险阀值
#define	risk_operation_BTS '5'	// 不提示
typedef char _risk_operation;

#define	entrust_fail_code_CG	0 // 成功
#define	entrust_fail_code_FKJZ	1 // 风控禁止
#define	entrust_fail_code_KYBZ	2 // 可用不足
#define	entrust_fail_code_QT	3 // 其他
#define	entrust_fail_code_OTHER	99 // 因为其他委托导致此笔委托失败
typedef int _entrust_fail_code;
#endif
