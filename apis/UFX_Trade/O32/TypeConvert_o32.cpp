#include "stdafx.h"
#include "TypeConvert_o32.h"

#include <stdio.h>
#include <string>

#include "../../../include/ApiHeader.h"


/// 类似于OpenQuant FIX一样的效果，插件层简单，基本不要做怎么计算或处理
/// 对于一个单子的某个状态可能是这样的，新单，部分成交，完全成交
/// EmitAccept,EmitFill
/// OnRtnOrder,OnRtnTrade,如何转成Emit
/// EmitAccept是什么
///
/// 接口向外提供的回报可以分两种方案，ExecutionReport或委托回报与成交回报
/// OpenQuant中使用ExecutionReport问题是因为OQ自己有OrderManager，如果其它软件要看到委托和成交列表是没法得到的
/// 所以接口应当返回委托与成交回报

int string_2_int(const char* x)
{
	int y = 0;
	for (int i = 0; i < 4; ++i)
	{
		if (x[i] == 0)
			break;
		y = (y << 8) + x[i];
	}
	return y;
}

ExchangeType market_no_2_ExchangeType(_market_no In)
{
	// *(int*)market_no 这种需要将"35"转成"53"才能使用
	int i = string_2_int(In);
	switch (i)
	{
	case dword_market_no_SHFE:
		return ExchangeType::ExchangeType_SHFE;
	case dword_market_no_CZCE:
		return ExchangeType::ExchangeType_CZCE;
	case dword_market_no_CFFEX:
		return ExchangeType::ExchangeType_CFFEX;
	case dword_market_no_DCE:
		return ExchangeType::ExchangeType_DCE;
	case dword_market_no_INE:
		return ExchangeType::ExchangeType_INE;
	case dword_market_no_SSE:

		return ExchangeType::ExchangeType_SSE;
	case dword_market_no_SZSE:
		return ExchangeType::ExchangeType_SZSE;

	default:
		return ExchangeType::ExchangeType_Undefined;
	}
}

BusinessType market_no_2_BusinessType(_market_no In)
{
	// *(int*)market_no 这种需要将"35"转成"53"才能使用
	int i = string_2_int(In);
	switch (i)
	{
	case dword_market_no_SHFE:
	case dword_market_no_CZCE:
	case dword_market_no_CFFEX:
	case dword_market_no_DCE:
	case dword_market_no_INE:
		return BusinessType::BusinessType_Future;
	default:
		return BusinessType::BusinessType_Stock;
	}
}

p_entrust_direction OrderSide_2_entrust_direction(OrderSide In)
{
	if (In == OrderSide::OrderSide_Sell)
		return entrust_direction_MC;
	return entrust_direction_MR;
}

p_entrust_direction OrderSide_2_entrust_direction_HG(OrderSide In)
{
	if (In == OrderSide::OrderSide_Sell)
		return entrust_direction_NHG;
	return entrust_direction_ZHG;
}

OrderSide entrust_direction_2_OrderSide(_entrust_direction In)
{
	if (strcmp(In, entrust_direction_MC) == 0)
	{
		return OrderSide::OrderSide_Sell;
	}
	else if (strcmp(In, entrust_direction_NHG) == 0)
	{
		return OrderSide::OrderSide_Sell;
	}
		
	return OrderSide::OrderSide_Buy;
}

OpenCloseType entrust_direction_2_OpenCloseType(_entrust_direction In)
{
	if (strcmp(In, entrust_direction_MC) == 0)
	{
		return OpenCloseType::OpenCloseType_Close;
	}
	else if (strcmp(In, entrust_direction_NHG) == 0)
	{
		return OpenCloseType::OpenCloseType_Close;
	}

	return OpenCloseType::OpenCloseType_Open;
}

_futures_direction OpenCloseType_2_futures_direction(OpenCloseType In)
{
	switch (In)
	{
	case OpenCloseType::OpenCloseType_CloseToday:
		return futures_direction_PC;
	case OpenCloseType::OpenCloseType_Close:
		return futures_direction_PC;
	case OpenCloseType::OpenCloseType_Open:
	default:
		return futures_direction_KC;
	}
}

_close_direction OpenCloseType_2_close_direction(OpenCloseType In)
{
	switch (In)
	{
	case OpenCloseType::OpenCloseType_CloseToday:
		return close_direction_PJ;
	case OpenCloseType::OpenCloseType_Close:
		return close_direction_PZ;
	default:
		return close_direction_YXPJ;
	}
}

OpenCloseType futures_direction_2_OpenCloseType(_futures_direction In)
{
	switch (In)
	{
	case futures_direction_PJC:
		return OpenCloseType::OpenCloseType_CloseToday;
	case futures_direction_PC:
		return OpenCloseType::OpenCloseType_Close;
	case futures_direction_KC:
	default:
		return OpenCloseType::OpenCloseType_Open;
	}
}

PositionSide position_flag_2_PositionSide(_position_flag In)
{
	switch (In)
	{
	case position_flag_DTCC:
		return PositionSide::PositionSide_Long;
	case position_flag_KTCC:
		return PositionSide::PositionSide_Short;
	case position_flag_BDKT:
		return PositionSide::PositionSide_Short;
	default:
		return PositionSide::PositionSide_Long;
	}
}

OrderStatus entrust_state_2_OrderStatus(_entrust_state In)
{
	switch (In)
	{
	case entrust_state_WB:
		return OrderStatus::OrderStatus_New;
	case entrust_state_DB:
		return OrderStatus::OrderStatus_New;
	case entrust_state_ZB:
		return OrderStatus::OrderStatus_New;
	case entrust_state_YB:
		return OrderStatus::OrderStatus_New;
	case entrust_state_FD:
		return OrderStatus::OrderStatus_Rejected;
	case entrust_state_BCheng:
		return OrderStatus::OrderStatus_PartiallyFilled;
	case entrust_state_YCheng:
		return OrderStatus::OrderStatus_Filled;
	case entrust_state_BChe:
		return OrderStatus::OrderStatus_Cancelled;
	case entrust_state_YChe1:
		return OrderStatus::OrderStatus_Cancelled;
	case entrust_state_DC1: // 这是什么鬼状态，待撤不就是已报吗？
		return OrderStatus::OrderStatus_PendingCancel;
	case entrust_state_WSP:
		return OrderStatus::OrderStatus_New;
	case entrust_state_SPJJ:
		return OrderStatus::OrderStatus_Rejected;
	case entrust_state_WSPJCX:
		return OrderStatus::OrderStatus_Cancelled;
	case entrust_state_WC: // 未撤不是待撤吗？不就是已报吗？
		return OrderStatus::OrderStatus_New;
	case entrust_state_DC2: // 出鬼了，还两处一样的
		return OrderStatus::OrderStatus_New;
	case entrust_state_ZC:
		return OrderStatus::OrderStatus_PendingCancel;
	case entrust_state_CR: // 撤认是啥？
		return OrderStatus::OrderStatus_Cancelled;
	case entrust_state_CF: // 撤废，只是执行结果是撤单被拒绝，但单子还是挂单中啊
		return OrderStatus::OrderStatus_New;
	case entrust_state_YChe2:
		return OrderStatus::OrderStatus_Cancelled;
	}

	return OrderStatus::OrderStatus_New;
}

ExecType entrust_state_2_ExecType(_entrust_state In)
{
	switch (In)
	{
	case entrust_state_WB:
		return ExecType::ExecType_New;
	case entrust_state_DB:
		return ExecType::ExecType_New;
	case entrust_state_ZB:
		return ExecType::ExecType_New;
	case entrust_state_YB:
		return ExecType::ExecType_New;
	case entrust_state_FD:
		return ExecType::ExecType_Rejected;
	case entrust_state_BCheng:
		return ExecType::ExecType_Trade;
	case entrust_state_YCheng:
		return ExecType::ExecType_Trade;
	case entrust_state_BChe:
		return ExecType::ExecType_Cancelled;
	case entrust_state_YChe1:
		return ExecType::ExecType_Cancelled;
	case entrust_state_DC1: // 这是什么鬼状态，待撤不就是已报吗？
		return ExecType::ExecType_PendingCancel;
	case entrust_state_WSP:
		return ExecType::ExecType_New;
	case entrust_state_SPJJ:
		return ExecType::ExecType_Rejected;
	case entrust_state_WSPJCX:
		return ExecType::ExecType_Cancelled;
	case entrust_state_WC: // 未撤不是待撤吗？不就是已报吗？
		return ExecType::ExecType_New;
	case entrust_state_DC2: // 出鬼了，还两处一样的
		return ExecType::ExecType_New;
	case entrust_state_ZC:
		return ExecType::ExecType_PendingCancel;
	case entrust_state_CR: // 撤认是啥？
		return ExecType::ExecType_PendingCancel;
	case entrust_state_CF: // 撤废，只是执行结果是撤单被拒绝，但单子还是挂单中啊
		return ExecType::ExecType_CancelReject;
	case entrust_state_YChe2:
		return ExecType::ExecType_Cancelled;
	}

	return ExecType::ExecType_New;
}

void OrderField_2_REQ_QHWT_91004(RSP_DL_10001* pRspUserLogin, OrderField* pIn, REQ_QHWT_91004* pOut)
{
	if (pRspUserLogin)
	{
		SAFE_STRNCPY(pOut->user_token, pRspUserLogin->user_token);
	}
	
	pOut->batch_no = 0;
	SAFE_STRNCPY(pOut->account_code, pIn->PortfolioID1);
	SAFE_STRNCPY(pOut->asset_no, pIn->PortfolioID2);
	SAFE_STRNCPY(pOut->combi_no, pIn->PortfolioID3);
	SAFE_STRNCPY(pOut->stockholder_id, pIn->AccountID);
	SAFE_STRNCPY(pOut->market_no, pIn->ExchangeID);
	SAFE_STRNCPY(pOut->stock_code, pIn->InstrumentID);
	SAFE_STRNCPY(pOut->entrust_direction, OrderSide_2_entrust_direction(pIn->Side));
	pOut->futures_direction = OpenCloseType_2_futures_direction(pIn->OpenClose);
	
	if (pOut->market_no[0] == market_no_SHFE[0])
	{
		// 只有上海可能需要标记平今功能
		// TODO:这个功能先关闭，等UFX版本上去了再开放
		// pOut->close_direction = OpenCloseType_2_close_direction(pIn->OpenClose);
	}
	
	pOut->price_type = price_type_XJ;
	pOut->entrust_price = pIn->Price;
	pOut->entrust_amount = pIn->Qty;
	pOut->limit_deal_amount = 1;
	pOut->extsystem_id = atoi(pIn->LocalID);
}

void OrderField_2_REQ_PTMMWT_91001(RSP_DL_10001* pRspUserLogin, OrderField* pIn, REQ_PTMMWT_91001* pOut)
{
	if (pRspUserLogin)
	{
		SAFE_STRNCPY(pOut->user_token, pRspUserLogin->user_token);
	}
	pOut->batch_no = 0;
	SAFE_STRNCPY(pOut->account_code, pIn->PortfolioID1);
	SAFE_STRNCPY(pOut->asset_no, pIn->PortfolioID2);
	SAFE_STRNCPY(pOut->combi_no, pIn->PortfolioID3);
	SAFE_STRNCPY(pOut->stockholder_id, pIn->AccountID);
	SAFE_STRNCPY(pOut->market_no, pIn->ExchangeID);
	SAFE_STRNCPY(pOut->stock_code, pIn->InstrumentID);
	SAFE_STRNCPY(pOut->entrust_direction, OrderSide_2_entrust_direction(pIn->Side));
	int stock_num = atoi(pIn->InstrumentID);
	int stock_num_xxx___ = (int)(stock_num / 1000);
	// 回购
	if (stock_num_xxx___ == 204 || stock_num_xxx___ == 131)
	{
		SAFE_STRNCPY(pOut->entrust_direction, OrderSide_2_entrust_direction_HG(pIn->Side));
	}

	//pOut->futures_direction = OpenCloseType_2_futures_direction(pIn->OpenClose);
	pOut->price_type = price_type_XJ;
	pOut->entrust_price = pIn->Price;
	pOut->entrust_amount = pIn->Qty;
	//pOut->limit_deal_amount = 1;
	pOut->extsystem_id = atoi(pIn->LocalID);
}

const char* risk_operation_2_String(_risk_operation In)
{
	switch (In)
	{
	case risk_operation_W:
		return "无";
	case risk_operation_YJ:
		return "预警";
	case risk_operation_JZ:
		return "禁止";
	case risk_operation_SQZLSP:
		return "申请指令审批";
	case risk_operation_SQFXFZ:
		return "申请风险阀值";
	case risk_operation_BTS:
		return "不提示";
	default:
		return "未知";
	}
}