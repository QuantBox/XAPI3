#ifndef _UFX_ENUM_H_
#define _UFX_ENUM_H_

#pragma once

#define entrust_bs_MR			'1'			// 买入
#define entrust_bs_MC			'2'			// 卖出
typedef char _entrust_bs;					// 买卖方向

#define entrust_type_WT			'0'			// 委托
#define entrust_type_CX			'1'			// 查询
#define entrust_type_CD			'2'			// 撤单
#define entrust_type_BD			'3'			// 补单
#define entrust_type_QR			'4'			// 确认
#define entrust_type_DZ			'5'			// 大宗
#define entrust_type_XYRZ		'6'			// 信用融资
#define entrust_type_XYRQ		'7'			// 信用融券
#define entrust_type_XYPC		'8'			// 信用平仓
#define entrust_type_XYJQ		'9'			// 信用交易
#define entrust_type_CDBD		'A'			// 撤单补单
#define entrust_type_GD			'B'			// 改单
#define entrust_type_ZHMM		'C'			// 组合买卖
#define entrust_type_ZHZH		'D'			// 组合转换
#define entrust_type_ZHCX		'E'			// 组合撤销
#define entrust_type_MM			'F'			// Manual Match
#define entrust_type_QQQZPC		'G'			// 期权强制平仓
typedef char _entrust_type;					// 委托类别

#define exchange_type_unknown	"0"			// 未知交易所
#define	exchange_type_SSE		"1"			// 上海
#define	exchange_type_SZSE		"2"			// 深圳
#define	exchange_type_TZ_A		"9"			// 特转A
#define	exchange_type_TZ_B		"A"			// 特转B
#define	exchange_type_SSE_B		"D"			// 沪Ｂ
#define	exchange_type_SSE_HK	"G"			// 沪HK
#define	exchange_type_SZSE_B	"H"			// 深Ｂ
#define	exchange_type_QDCQ		"Q"			// 青岛产权
#define	exchange_type_ZRT		"U"			// 转融通
#define	exchange_type_JHJJ		"J"			// 金华基金
#define	exchange_type_HK		"K"			// 香港市场
#define	exchange_type_GDSY		"X"			// 固定收益
#define	exchange_type_NZSC		"B0"		// 内债市场
#define	exchange_type_ZZSC		"B1"		// 中债市场
#define	exchange_type_CZCE		"F1"		// 郑州交易所
#define	exchange_type_DCE		"F2"		// 大连交易所
#define	exchange_type_SHFE		"F3"		// 上海交易所
#define	exchange_type_CFFEX		"F4"		// 金融交易所
#define	exchange_type_NIE		"F5"		// 能源交易所
#define	exchange_type_YWSL		"Z1"		// 业务受理
#define	exchange_type_TJJJ		"H3"		// 天津基金
#define	exchange_type_STAQ		"H4"		// STAQ
#define	exchange_type_ZBJJ		"H5"		// 淄博基金
#define	exchange_type_LJJJ		"H6"		// 龙江基金
#define	exchange_type_WHJY		"H7"		// 武汉交易
#define	exchange_type_CY		"H8"		// 创业
#define	exchange_type_SZBZR		"HB"		// 深B转让
#define	exchange_type_HNCQ		"HE"		// 河南产权
#define	exchange_type_ZJGQ		"HF"		// 浙江股权
#define	exchange_type_ZJJJ		"HG"		// 浙江基金
#define	exchange_type_HNJJ		"HN"		// 湖南基金
#define	exchange_type_QDCQ_2	"HQ"		// 青岛产权
#define	exchange_type_SYJJ		"HS"		// 沈阳基金
#define	exchange_type_OTC		"HO"		// OTC
#define	exchange_type_FRANCE	"Hf"		// FRANCE STOCK
#define	exchange_type_DEUTSCHE	"Hg"		// DEUTSCHE STOCK
#define	exchange_type_NETHERLANDS	"Hh"		// NETHERLANDS STOCK
#define	exchange_type_JARDINE	"Hj"		// JARDINE GROUP
#define	exchange_type_FINLAND	"Hl"		// FINLAND STOCK
#define	exchange_type_NEW_ZEALAND	"Hn"		// NEW ZEALAND STOCK
#define	exchange_type_SWITZERLAND	"Hr"		// SWITZERLAND STOCK
typedef char _exchange_type[4];				// 交易类别

#define entrust_status_WB		'0'			// 未报
#define entrust_status_DB		'1'			// 待报
#define entrust_status_YB		'2'			// 已报
#define entrust_status_YBDC		'3'			// 已报待撤
#define entrust_status_BCDC		'4'			// 部成待撤
#define entrust_status_BChe		'5'			// 部撤
#define entrust_status_YChe		'6'			// 已撤
#define entrust_status_BCheng	'7'			// 部成
#define entrust_status_YCheng	'8'			// 已成
#define entrust_status_FD		'9'			// 废单
#define entrust_status_YBDG_HK	'A'			// 已报待改(港股)
#define entrust_status_YMDCD_HK	'B'			// 预埋单撤单(港股)
#define entrust_status_ZB		'C'			// 正报
#define entrust_status_CF		'D'			// 撤废
#define entrust_status_BCDG_HK	'E'			// 部成待改(港股)
#define entrust_status_YMDFD_HK	'F'			// 预埋单废单(港股)
#define entrust_status_DCCJ		'G'			// 单腿成交
#define entrust_status_DSH_HK	'H'			// 待审核(港股)
#define entrust_status_FHWTG_HK	'J'			// 复核未通过(港股)
#define entrust_status_WFC		'M'			// Wait for Confirming
#define entrust_status_YQRDC	'U'			// 已确认待撤
#define entrust_status_YQR		'V'			// 已确认
#define entrust_status_DQR		'W'			// 待确认
#define entrust_status_YCJ		'X'			// 预成交
#define entrust_status_HGDQR	'Y'			// 购回待确认
#define entrust_status_YHG		'Z'			// 已购回
typedef char _entrust_status;				// 委托状态

#define entrust_prop_MM			"0"			// 买卖
#define entrust_prop_PG			"1"			// 配股
#define entrust_prop_ZT			"2"			// 转托
#define entrust_prop_SG			"3"			// 申购
#define entrust_prop_HG			"4"			// 回购
#define entrust_prop_PS			"5"			// 配售
#define entrust_prop_ZD			"6"			// 指定
#define entrust_prop_ZG			"7"			// 转股
#define entrust_prop_HS			"8"			// 回售
#define entrust_prop_GX			"9"			// 股息
#define entrust_prop_SZPSQR		"A"			// 深圳配售确认
#define entrust_prop_PSFQ		"B"			// 配售放弃
#define entrust_prop_PSCZ		"G"			// 配售冲正
#define entrust_prop_TSYW		"H"			// 特殊业务
#define entrust_prop_YSYYJC		"J"			// 预受要约解除
#define entrust_prop_JJSH		"K"			// 基金设红
#define entrust_prop_JJSS		"L"			// 基金申赎
#define entrust_prop_KSZT		"M"			// 跨市转托
#define entrust_prop_ETFSS		"N"			// ETF申赎
#define entrust_prop_QZXQ		"P"			// 权证行权
#define entrust_prop_DSFZYJG	"Q"			// 对手方最优价格
#define entrust_prop_ZYWDJSCJSYXXJ		"R"			// 最优五档即时成交剩余转限价
#define entrust_prop_BFZYJG		"S"			// 本方最优价格
#define entrust_prop_JSCJSYCX	"T"			// 即时成交剩余撤销
#define entrust_prop_ZYWDJSCJSYCX		"U"			// 最优五档即时成交剩余撤销
#define entrust_prop_QECJHCD	"V"			// 全额成交或撤单
#define entrust_prop_JJZH		"W"			// 基金转换
#define entrust_prop_NXBJY		"X"			// 牛熊宝交易
#define entrust_prop_YYSGYS		"Y"			// 要约收购预受
#define entrust_prop_ZYCYK		"f"			// 质押出入库
#define entrust_prop_YXWT		"a"			// 意向委托
#define entrust_prop_DJWT		"b"			// 定价委托
#define entrust_prop_QRWT		"c"			// 确认委托
#define entrust_prop_XJWT		"d"			// 限价委托
#define entrust_prop_HBCJQRWT	"e"			// 互报成交确认委托
#define entrust_prop_BJCRK		"g"			// 报价出入库
#define entrust_prop_BJHG		"h"			// 报价回购
#define entrust_prop_AO			"Hd"		// AO - At - Auction
#define entrust_prop_YXWT_2		"He"		// 意向委托
#define entrust_prop_DJWT_2		"Hg"		// 定价委托
#define entrust_prop_QRWT_2		"Hh"		// 确认委托
#define entrust_prop_XJWT_2		"Hi"		// 限价委托
#define entrust_prop_ELO		"Hj"		// ELO
#define entrust_prop_AL			"Hk"		// Al - Auction Limit
#define entrust_prop_LIMIT		"Hl"		// Limit
#define entrust_prop_QUOTE		"Hm"		// Quote Order
#define entrust_prop_SLO		"Hn"		// SLO
#define entrust_prop_APO		"Ho"		// Average Price Order
#define entrust_prop_CCB		"Hp"		// Cancel(Cross Broker)
#define entrust_prop_SOL		"Hr"		// Special Odd Lot
#define entrust_prop_WTTD		"Hs"		// 委托填单
#define entrust_prop_MT			"Ht"		// Manual Trade
#define entrust_prop_BC			"Hu"		// Bulk Cancel
#define entrust_prop_PO			"Hv"		// Pre Opening
#define entrust_prop_OL			"HC"		// Odd Lot
#define entrust_prop_BCCB		"HD"		// Bulk Cancel(Cross Broker)
#define entrust_prop_OLI		"HE"		// Odd Lot Input
#define entrust_prop_O			"HF"		// Overseas
#define entrust_prop_WDZY		"HG"		// 无冻质押
#define entrust_prop_DJZY		"HH"		// 冻结质押
#define entrust_prop_WDJY		"HI"		// 无冻解押
#define entrust_prop_JDJY		"HJ"		// 解冻解押
#define entrust_prop_AFC		"AFC"		// 盘后大宗收盘价委托
#define entrust_prop_AFW		"AFW"		// 盘后大宗成交量加权平均价委托
#define entrust_prop_BIP		"BIP"		// 国债价格招标
#define entrust_prop_BIY		"BIY"		// 国债利率招标
#define entrust_prop_BPN		"BPN"		// 债券质押协议回购初始交易
#define entrust_prop_BPR		"BPR"		// 债券质押协议回购购回交易
#define entrust_prop_HKN		"HKN"		// 港股订单申报
#define entrust_prop_HKO		"HKO"		// 零股订单申报
#define entrust_prop_LFS		"LFS"		// 上证LOF认购
#define entrust_prop_LFC		"LFC"		// 上证LOF申购
#define entrust_prop_LFR		"LFR"		// 上证LOF赎回
#define entrust_prop_LFT		"LFT"		// 上证LOF转托管
#define entrust_prop_LFP		"LFP"		// 上证LOF母基金分拆
#define entrust_prop_LFM		"LFM"		// 上证LOF子基金合并
#define entrust_prop_OFC		"OFC"		// 货币基金申购
#define entrust_prop_OFR		"OFR"		// 货币基金赎回
#define entrust_prop_OPA		"OPA"		// 限价即时全部成交否则撤单
#define entrust_prop_OPB		"OPB"		// 市价即时成交剩余撤单
#define entrust_prop_OPC		"OPC"		// 市价即时全部成交否则撤单
#define entrust_prop_OPD		"OPD"		// 市价剩余转限价
#define entrust_prop_OTL		"OTL"		// 垫券还券
#define entrust_prop_OTD		"OTD"		// 待清偿扣划
#define entrust_prop_OMR		"OMR"		// 保证金查询
#define entrust_prop_OML		"OML"		// 组合拆分
#define entrust_prop_OTE		"OTE"		// 期权行权
#define entrust_prop_OTU		"OTU"		// 备兑证券划转
#define entrust_prop_OTT		"OTT"		// 会员申请转处置证券账户
#define entrust_prop_R1			"R1"		// 担保证券提交与返还
#define entrust_prop_R2			"R2"		// 融券券源划拨
#define entrust_prop_R3			"R3"		// 现券还券划拨
#define entrust_prop_QNE		"QNE"		// 报价回购
#define entrust_prop_QBD		"QBD"		// 报价入库
#define entrust_prop_QBF		"QBF"		// 报价担保资金提取
#define entrust_prop_QBW		"QBW"		// 报价出库
#define entrust_prop_QCA		"QCA"		// 报价回购提前购回
#define entrust_prop_QNP		"QNP"		// 报价回购展期买入
#define entrust_prop_RNA		"RNA"		// 约定购回补充交易
#define entrust_prop_RNB		"RNB"		// 约定购回回补交易
#define entrust_prop_RNE		"RNE"		// 约定购回初始交易
#define entrust_prop_RNL		"RNL"		// 约定购回零碎股购回
#define entrust_prop_RNP		"RNP"		// 约定购回配股权证购回
#define entrust_prop_RNR		"RNR"		// 约定购回购回交易
#define entrust_prop_REA		"REA"		// 转融券出借非约定申报
#define entrust_prop_REB		"REB"		// 转融通借入交易
#define entrust_prop_REC		"REC"		// 转融券出借约定申报
#define entrust_prop_REF		"REF"		// 转融通保证金业务
#define entrust_prop_SPN		"SPN"		// 股票质押初始交易
#define entrust_prop_SPS		"SPS"		// 股票质押补充质押
#define entrust_prop_SPR		"SPR"		// 股票质押购回交易
#define entrust_prop_SPB		"SPB"		// 股票质押违约处置
#define entrust_prop_SPD		"SPD"		// 股票质押部分解押
#define entrust_prop_SPT		"SPT"		// 股票质押终止购回
#define entrust_prop_TRS		"TRS"		// 资管份额转让
#define entrust_prop_VTE		"VTE"		// 网络投票
#define entrust_prop_XJD		"F0"		// 限价单
#define entrust_prop_SJD		"F1"		// 市价单
#define entrust_prop_ZSDD		"F2"		// 止损定单
#define entrust_prop_ZYDD		"F3"		// 止盈定单
#define entrust_prop_XJZSDD		"F4"		// 限价止损定单
#define entrust_prop_XJZYDD		"F5"		// 限价止盈定单
#define entrust_prop_ZS			"F6"		// 止损
#define entrust_prop_ZHDD		"F7"		// 组合定单
#define entrust_prop_KQTLQR		"FA"		// 跨期套利确认
#define entrust_prop_CCTBQR		"FB"		// 持仓套保确认
#define entrust_prop_QQXJ		"FC"		// 请求询价
#define entrust_prop_QQQLXS		"FD"		// 期权权力行使
#define entrust_prop_QQQLFQ		"FE"		// 期权权力放弃
#define entrust_prop_SBBJ		"FF"		// 双边报价
#define entrust_prop_WDSJ		"FG"		// 五档市价
#define entrust_prop_ZYJ		"FH"		// 最优价
#define entrust_prop_YZXQ		"+"			// 融资行权
#define entrust_prop_YZXQMQHK	"="			// 融资行权卖券还款
typedef char _entrust_prop[4];				// 委托属性


#define input_content_ZJZH		'1'			// 资金账号
#define input_content_GDNM		'2'			// 股东内码
#define input_content_JYKH		'3'			// 交易卡号
#define input_content_YHZH		'4'			// 银行帐号
#define input_content_ZQZH		'5'			// 证券账号
#define input_content_KHBH		'6'			// 客户编号
#define input_content_QHZH		'A'			// 期货账号
typedef char _input_content;				// 输入方法 

#define password_type_NONE		'0'			// 无密码
#define password_type_ZJMM		'1'			// 资金密码
#define password_type_JYMM		'2'			// 交易密码
#define password_type_FWMM		'3'			// 服务密码
#define password_type_TXMM		'4'			// 通讯密码
#define password_type_ZFMM		'5'			// 支付密码
#define password_type_YHTMM		'6'			// 一户通密码
#define password_type_XDMM		'7'			// 下单密码
#define password_type_YZM		'8'			// 验证码
typedef char _password_type;				// 密码类别

#define batch_flag_NO			'0'			// 单笔
#define batch_flag_YES			'1'			// 批量
typedef char _batch_flag;					// 批量标志

#define op_entrust_way_WXXJ		'+'			// 网下现金
#define op_entrust_way_WXZQ		'-'			// 网下证券
#define op_entrust_way_ZZWT		'0'			// 自助委托
#define op_entrust_way_DHWT		'1'			// 电话委托
#define op_entrust_way_ZLWT		'2'			// 驻留委托
#define op_entrust_way_YCWT		'3'			// 远程委托
#define op_entrust_way_GTWT		'4'			// 柜台委托
#define op_entrust_way_MYWT		'5'			// 漫游委托
#define op_entrust_way_FZWT		'6'			// 分支委托
#define op_entrust_way_WSWT		'7'			// 网上委托
#define op_entrust_way_SJWT		'8'			// 手机委托
#define op_entrust_way_WYWT		'9'			// 网页委托
#define op_entrust_way_DKLC		'a'			// 代客理财
#define op_entrust_way_DKWT		'b'			// 代理委托
#define op_entrust_way_CATS		'c'			// CATS委托
#define op_entrust_way_SMS		'd'			// 短信委托
#define op_entrust_way_CLOUD	'e'			// 云委托
#define op_entrust_way_GPRS		'g'			// GPRS委托
#define op_entrust_way_QTCN		'h'			// 其他场内
#define op_entrust_way_QTCW		'i'			// 其他场外
#define op_entrust_way_ATM		'm'			// 银行ATM
#define op_entrust_way_POS		'p'			// 银行POS
#define op_entrust_way_BREW		'r'			// BREW委托
#define op_entrust_way_ETF		's'			// ETF算法交易
#define op_entrust_way_RZWW		't'			// 融资外围
#define op_entrust_way_YHGT		'u'			// 银行柜台
#define op_entrust_way_DSFGT	'v'			// 第三方柜台
#define op_entrust_way_JZGM		'z'			// 集中柜面
#define op_entrust_way_A		'A'			// 自定义委托A
#define op_entrust_way_B		'B'			// 自定义委托B
#define op_entrust_way_C		'C'			// 自定义委托C
#define op_entrust_way_D		'D'			// 自定义委托D
#define op_entrust_way_E		'E'			// 自定义委托E
#define op_entrust_way_F		'F'			// 自定义委托F
#define op_entrust_way_G		'G'			// 自定义委托G
#define op_entrust_way_H		'H'			// 自定义委托H
#define op_entrust_way_I		'I'			// 自定义委托I
#define op_entrust_way_J		'J'			// 自定义委托J
#define op_entrust_way_K		'K'			// 自定义委托K
#define op_entrust_way_UFT		'L'			// UFT委托
#define op_entrust_way_LGWT		'O'			// 离柜委托
#define op_entrust_way_RZZ2		'P'			// 热自助2
#define op_entrust_way_DHNX		'Q'			// 电话内线
#define op_entrust_way_YYSG		'U'			// 要约收购
#define op_entrust_way_ZQTY		'Y'			// 证券托银
#define op_entrust_way_VIP		'Z'			// VIP终端委托
#define op_entrust_way_DZJY		'~'			// 大宗交易
#define op_entrust_way_WXXJ		'+'			// 网下现金
#define op_entrust_way_WXZQ		'-'			// 网下证券
typedef char _op_entrust_way;				// 委托方式

#define real_type_MM			'0'			// 买卖
#define real_type_CX			'1'			// 查询
#define real_type_CD			'2'			// 撤单
#define real_type_BTI			'3'			// Buyer Transfer in
#define real_type_MO			'4'			// Modify Order
#define real_type_MMT			'5'			// Modify Match Type
#define real_type_RZ			'6'			// 融资
#define real_type_RQ			'7'			// 融券
#define real_type_PC			'8'			// 平仓
#define real_type_XY			'9'			// 信用
typedef char _real_type;					// 成交类型

#define real_status_CJ			'0'			// 成交
#define real_status_FD			'2'			// 废单
#define real_status_BDSCJHFD	'3'			// 补单时成交后废单
#define real_status_QR			'4'			// 确认
typedef char _real_status;					// 处理标志

#define money_type_CNY			'0'			// 人民币
#define money_type_USD			'1'			// 美元
#define money_type_HKD			'2'			// 港币
#define money_type_SZ_HKD		'3'			// SZ - HKD
#define money_type_AUD			'6'			// AUD
#define money_type_CAD			'E'			// CAD
#define money_type_GBP			'Q'			// GBP
#define money_type_DKK			'k'			// 丹麦克朗
#define money_type_MOP			'o'			// 澳门元
#define money_type_SEK			'r'			// 瑞典克朗
#define money_type_SGD			't'			// SGD
#define money_type_THB			'v'			// THB
#define money_type_IDR			'S'			// IDR
#define money_type_MYR			'f'			// MYR
#define money_type_PHP			'l'			// PHP
#define money_type_EUR			'M'			// EUR
#define money_type_JPY			'W'			// JPY
#define money_type_TWD			'w'			// TWD
typedef char _money_type;					// 币种类别

#define query_type_All			'0'			// 全部
#define query_type_Exclude		'1'			// 不查询委托类别为撤单的委托（默认）	
typedef char _query_type;					// 查询类别

#define asset_prop_PTKH			'0'			// 普通客户
#define asset_prop_XYZH			'7'			// 信用账户
#define asset_prop_LCZH			'8'			// 理财账户
#define asset_prop_QHZH			'9'			// 期货账户
#define asset_prop_ZGZH			'A'			// 资管账户
#define asset_prop_YSPZH		'B'			// 衍生品账户
#define asset_prop_CASH			'C'			// Cash Account
#define asset_prop_DAP			'X'			// DAP Account
#define asset_prop_HOURCE		'Y'			// Hource Account
#define asset_prop_MARGIN		'Z'			// Margin Account
typedef char _asset_prop;					// 资产属性

//===================================================

#define	market_no_SSE			"1"			// 上交所
#define	market_no_SZSE			"2"			// 深交所
#define	market_no_SHFE			"3"			// 上期所
#define	market_no_CZCE			"4"			// 郑商所
#define	market_no_CFFEX			"7"			// 中金所
#define	market_no_DCE			"9"			// 大商所
#define	market_no_NEEQ			"10"		// 股转市场
typedef char _market_no[4];

#define	entrust_direction_MR			"1"	// 买入
#define	entrust_direction_MC			"2"	// 卖出
#define	entrust_direction_ZQMR			"3"	// 债券买入
#define	entrust_direction_ZQMC			"4"	// 债券卖出
#define	entrust_direction_ZHG			"5"	// 融资(正)回购
#define	entrust_direction_NHG			"6"	// 融券(逆)回购
#define	entrust_direction_PGRG			"9"	// 配股认购
#define	entrust_direction_ZZG			"10"	// 债转股
#define	entrust_direction_ZHS			"11"	// 债回售
#define	entrust_direction_SG			"12"	// 申购
#define	entrust_direction_JJRG			"13"	// 基金认购
#define	entrust_direction_ETFSG			"26"	// ETF申购
#define	entrust_direction_ETFSH			"27"	// ETF赎回
#define	entrust_direction_XQRGou		"28"	// 行权认购
#define	entrust_direction_XQRGu			"29"	// 行权认沽
#define	entrust_direction_TJZR			"30"	// 提交质押
#define	entrust_direction_HXZR			"31"	// 回转质押
#define	entrust_direction_DBQJC			"63"	// 担保券交存(锁定)
#define	entrust_direction_DBQTQ			"64"	// 担保券提取（解锁）
typedef char _entrust_direction[4];

#define	futures_direction_KC			'1'	// 开仓
#define	futures_direction_PC			'2'	// 平仓
#define	futures_direction_JG			'3'	// 交割
#define	futures_direction_PJC			'4'	// 平今仓
typedef char _futures_direction;


#define	invest_type_GDSY			'1'	// 固定收益类
#define	invest_type_QY				'2'	// 权益类
#define	invest_type_YHRZRQ			'3'	// 期货融资融券类
#define	invest_type_FZGFX			'4'	// 复杂高风险类
#define	invest_type_QT				'5'	// 其他
typedef char _invest_type;

#define	position_flag_DTCC			'1'	// 多头持仓
#define	position_flag_KTCC			'2'	// 空头持仓
#define	position_flag_BDKT			'b'	// 备兑空头持仓（个股期权）
typedef char _position_flag;
#endif
