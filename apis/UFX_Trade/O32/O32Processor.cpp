#include "stdafx.h"
#include "O32Processor.h"

#include "../../../include/ApiStruct.h"
#include "../../../include/QueueEnum.h"

#include "../../../include/queue/MsgQueue.h"

#include "../../../include/ApiProcess.h"
#include "..\..\..\include\ApiHeader.h"


#include "TypeConvert_o32.h"


CO32Processor::CO32Processor()
{
}


CO32Processor::~CO32Processor()
{
}

int CO32Processor::Do_Error(RSP_ErrorField3* pErr, const char* source)
{
	if (pErr == nullptr)
		return 0;

	if (0 != pErr->ErrorCode)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));
		pField->RawErrorID = pErr->ErrorCode;
		SAFE_SNPRINTF(pField->Text, "ErrorMsg:%s,MsgDetail:%s", pErr->ErrorMsg, pErr->MsgDetail);
		SAFE_STRNCPY(pField->Source, source);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnError, m_msgQueue, m_pClass, 0, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}

	return pErr->ErrorCode;
}

int CO32Processor::Do_10001(RSP_DL_10001* pRsp, int count, RSP_ErrorField3* pErr)
{
	RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

	if (pErr->ErrorCode == 0)
	{
		SAFE_STRNCPY(pField->Text, pRsp->version_no);
		SAFE_STRNCPY(pField->SessionID, pRsp->user_token);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logined, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	}
	else
	{
		pField->RawErrorID = pErr->ErrorCode;
		SAFE_SNPRINTF(pField->Text, "ErrorMsg:%s,MsgDetail:%s", pErr->ErrorMsg, pErr->MsgDetail);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_34001(RSP_ZHZJCX_34001* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (0 != Do_Error(pErr, "34001 账户资金查询"))
		return 0;

	if (count == 0)
	{
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTradingAccount, m_msgQueue, m_pClass, true, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		return 0;
	}

	for (int i = 0; i < count; ++i)
	{
		RSP_ZHZJCX_34001* pRsp = &pRsps[i];

		AccountField* pField = (AccountField*)m_msgQueue->new_block(sizeof(AccountField));

		SAFE_STRNCPY(pField->ClientID, pRsp->account_code);
		SAFE_STRNCPY(pField->AccountID, pRsp->asset_no);

		//pField->CurrencyID = money_type;
		pField->Available = pRsp->enable_balance_t0;
		pField->WithdrawQuota = pRsp->enable_balance_t1;
		pField->Balance = pRsp->current_balance;
		//unfrozen_balance;

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTradingAccount, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(AccountField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_34003(RSP_QHBZJZHCX_34003* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (0 != Do_Error(pErr, "34003 期货保证金账户查询"))
		return 0;

	if (count == 0)
	{
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTradingAccount, m_msgQueue, m_pClass, true, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		return 0;
	}

	for (int i = 0; i < count; ++i)
	{
		RSP_QHBZJZHCX_34003* pRsp = &pRsps[i];

		AccountField* pField = (AccountField*)m_msgQueue->new_block(sizeof(AccountField));


		SAFE_STRNCPY(pField->ClientID, pRsp->account_code);
		SAFE_STRNCPY(pField->AccountID, pRsp->asset_no);

		//pField->CurrencyID = money_type;
		pField->Available = pRsp->enable_deposit_balance;
		pField->CurrMargin = pRsp->occupy_deposit_balance;
		pField->Balance = pRsp->futu_deposit_balance;
		pField->FrozenCash = pRsp->futu_temp_occupy_deposit;

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTradingAccount, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(AccountField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_31001(RSP_ZQCCCX_31001* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (0 != Do_Error(pErr, "31001 证券持仓查询"))
		return 0;

	if (count == 0)
	{
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, true, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		return 0;
	}

	for (int i = 0; i < count; ++i)
	{
		RSP_ZQCCCX_31001* pRsp = &pRsps[i];

		PositionField* pField = (PositionField*)m_msgQueue->new_block(sizeof(PositionField));
		SAFE_SNPRINTF(pField->Symbol, "%s.%s", pRsp->stock_code,
			ExchangeType_2_String(market_no_2_ExchangeType(pRsp->market_no)));
		SAFE_STRNCPY(pField->InstrumentID, pRsp->stock_code);
		SAFE_STRNCPY(pField->ExchangeID, pRsp->market_no);
		SAFE_STRNCPY(pField->InstrumentName, pRsp->stock_name);
		SAFE_STRNCPY(pField->AccountID, pRsp->stockholder_id);
		SAFE_STRNCPY(pField->PortfolioID1, pRsp->account_code);
		SAFE_STRNCPY(pField->PortfolioID2, pRsp->asset_no);
		SAFE_STRNCPY(pField->PortfolioID3, pRsp->combi_no);
		pField->Business = BusinessType::BusinessType_Stock;
		pField->Side = PositionSide::PositionSide_Long; //position_flag_2_PositionSide(pRsp->position_flag);
		pField->Position = pRsp->current_amount;
		pField->HistoryPosition = pRsp->enable_amount;

		sprintf(pField->ID, "%s:%s:%d:%d",
			pField->ExchangeID,
			pField->InstrumentID,
			pField->Side,
			pField->HedgeFlag);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(PositionField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_31003(RSP_QHCCCX_31003* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (0 != Do_Error(pErr, "31003 期货持仓查询"))
		return 0;

	if (count == 0)
	{
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, true, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		return 0;
	}

	for (int i = 0; i < count; ++i)
	{
		RSP_QHCCCX_31003* pRsp = &pRsps[i];

		PositionField* pField = (PositionField*)m_msgQueue->new_block(sizeof(PositionField));
		sprintf(pField->Symbol, "%s.%s", pRsp->stock_code,
			ExchangeType_2_String(market_no_2_ExchangeType(pRsp->market_no)));
		SAFE_STRNCPY(pField->InstrumentID, pRsp->stock_code);
		SAFE_STRNCPY(pField->InstrumentName, pRsp->stock_name);
		SAFE_STRNCPY(pField->ExchangeID, pRsp->market_no);
		SAFE_STRNCPY(pField->AccountID, pRsp->stockholder_id);
		SAFE_STRNCPY(pField->PortfolioID1, pRsp->account_code);
		SAFE_STRNCPY(pField->PortfolioID2, pRsp->asset_no);
		SAFE_STRNCPY(pField->PortfolioID3, pRsp->combi_no);
		pField->Business = BusinessType::BusinessType_Future;
		pField->Side = position_flag_2_PositionSide(pRsp->position_flag);
		pField->Position = pRsp->current_amount;
		pField->TodayPosition = pRsp->today_amount;
		pField->HistoryPosition = pRsp->lastday_amount;

		sprintf(pField->ID, "%s:%s:%d:%d",
			pField->ExchangeID,
			pField->InstrumentID,
			pField->Side,
			pField->HedgeFlag);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(PositionField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_91004(RSP_QHWT_91004* pRsps, int count, RSP_ErrorField3* pErr, int packetId, RSP_RiskField* pRiskRsps, int riskCount)
{
	// 如果下单后立即撤单，会先收到撤单失败的错误91105，主要是说没有委托编号
	// 然后在这里收到与91105一样的错误信息，导致单子被拒绝，其实单子正常
	// 当触发风控时，也直接走的这里，这时顺序是理想的
	// 总结后，还是直接在XAPI层就拒绝更合适

	// 1.有可能出错了，没有回应
	// 2.有可能消息中心的数据先到达
	OrderField* pField = nullptr;
	if (count == 0)
	{
		// 如果平今仓，会出现futures_direction字段值不在值域范围内，这时不知道返回的错误是哪家的，只能还是用回packetId
		pField = (OrderField*)m_pOrderMap->findOrderXAPI(packetId);
		if (pField)
		{
			pField->ExecType = ExecType::ExecType_Rejected;
			pField->Status = OrderStatus::OrderStatus_Rejected;

			pField->RawErrorID = pErr->ErrorCode;
			SAFE_SNPRINTF(pField->Text, "91004 期货委托:ErrorMsg:%s,MsgDetail:%s", pErr->ErrorMsg, pErr->MsgDetail);

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, true, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
		else
		{
			// 找不到对应的订单
			Do_Error(pErr, "91004 期货委托");
		}

		return 0;
	}

	// 将包ID的删除
	m_pOrderMap->erase(packetId);


	for (int i = 0; i < count; ++i)
	{
		RSP_QHWT_91004* pRsp = &pRsps[i];

		// 两个地方都会出现
		pField = (OrderField*)m_pOrderMap->findOrderXAPI(pRsp->extsystem_id);
		if (pField == nullptr)
		{
			printf("91004 期货委托，未找到订单，extsystem_id:%d，entrust_no:%d\r\n", pRsp->extsystem_id, pRsp->entrust_no);
			continue;
		}
		// TODO:测试用，为了是测试无法撤单的问题
		//continue;

		// 更新订单号
		if (pRsp->entrust_no == 0)
		{
			// 价格超出涨跌停时，委托编号不会返回
		}
		else
		{
			sprintf(pField->ID, "%d", pRsp->entrust_no);
			sprintf(pField->OrderID, "%d", pRsp->entrust_no);

			// 更新Map，如果自定义数字与原有数据冲突是否有可能？所以要自义定
			m_pOrderMap->replace(pRsp->extsystem_id, pRsp->entrust_no, pField, pField->pUserData1);
		}

		// 可以从回报中取，也可以从历史中取
		pField->CumQty = 0;
		pField->LeavesQty = pField->Qty;
		pField->RawErrorID = pErr->ErrorCode;
		memset(pField->Text, 0, sizeof(pField->Text));
		SAFE_STRNCPY(pField->Text, "91004 期货委托");

		REQ_QHWT_91004* pWTField = (REQ_QHWT_91004*)pField->pUserData1;
		pWTField->batch_no = pRsp->batch_no;
		pWTField->entrust_no = pRsp->entrust_no;

		SAFE_STRNCPY(m_Text, pField->Text);
		SAFE_SNPRINTF(pField->Text, "%s:%s", m_Text, pRsp->fail_cause);
		if (pRsp->risk_serial_no != 0)
		{
			for (int j = 0; j < riskCount; ++j)
			{
				RSP_RiskField* pRiskRsp = &pRiskRsps[j];
				if (pRiskRsp == nullptr)
					continue;
				if (pRsp->risk_serial_no == pRiskRsp->risk_serial_no)
				{
					// 找到相同序号的，但可能因为风控触发多个，如何通知多个呢？
					SAFE_STRNCPY(m_Text, pField->Text);
					SAFE_SNPRINTF(pField->Text, "%s:%s:%d:%s", m_Text, risk_operation_2_String(pRiskRsp->risk_operation), pRiskRsp->risk_no, pRiskRsp->risk_summary);
					break;
				}
			}
		}

		if (pRsp->entrust_fail_code == entrust_fail_code_CG)
		{
			// 成功了，但风控可能预警，所以还是要返回
			pField->ExecType = ExecType::ExecType_New;
			pField->Status = OrderStatus::OrderStatus_New;
		}
		else
		{
			// 如果立即撤单而走这里，会导致错误
			pField->ExecType = ExecType::ExecType_Rejected;
			pField->Status = OrderStatus::OrderStatus_Rejected;
		}

		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_91105(RSP_QHWTCD_91105* pRsps, int count, RSP_ErrorField3* pErr, int packetId)
{
	// 如果在盘中休息时撤单，会收到撤单成功的状态，其实并没有成功，而是撤单被拒绝了
	if (count == 0)
	{
		Do_Error(pErr, "91105 期货委托撤单");
		return 0;
	}

	// 还需要再测试
	OrderField* pField = nullptr;
	// 成功，看收到的编号是什么
	for (int i = 0; i < count; ++i)
	{
		RSP_QHWTCD_91105* pRsp = &pRsps[i];

		pField = (OrderField*)m_pOrderMap->findCancelAPI(pRsp->entrust_no);
		if (pField == nullptr)
			continue;
		memset(pField->Text, 0, sizeof(pField->Text));

		if (pRsp->success_flag == success_flag_CG)
		{
			// 盘中休息时没并有撤单成功，而是拒绝了
			//pField->ExecType = ExecType::ExecType_Cancelled;
			//pField->Status = OrderStatus::OrderStatus_Cancelled;

			// m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
		else
		{
			pField->ExecType = ExecType::ExecType_CancelReject;
			// 无法获取entrust_state，只能用上次的状态
			//pField->Status = entrust_state_2_OrderStatus(pRsp->entrust_state);

			pField->RawErrorID = pRsp->success_flag;
			SAFE_SNPRINTF(pField->Text, "91105 期货委托撤单:%s", pRsp->fail_cause);

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
	}

	return 0;
}

int CO32Processor::Do_91001(RSP_PTMMWT_91001* pRsps, int count, RSP_ErrorField3* pErr, int packetId, RSP_RiskField* pRiskRsps, int riskCount)
{
	OrderField* pField = nullptr;
	if (count == 0)
	{
		// 如果平今仓，会出现futures_direction字段值不在值域范围内，这时不知道返回的错误是哪家的，只能还是用回packetId
		pField = (OrderField*)m_pOrderMap->findOrderXAPI(packetId);
		if (pField)
		{
			pField->ExecType = ExecType::ExecType_Rejected;
			pField->Status = OrderStatus::OrderStatus_Rejected;

			pField->RawErrorID = pErr->ErrorCode;
			SAFE_SNPRINTF(pField->Text, "91001 普通买卖委托:ErrorMsg:%s,MsgDetail:%s", pErr->ErrorMsg, pErr->MsgDetail);

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, true, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
		else
		{
			// 找不到对应的订单
			Do_Error(pErr, "91001 普通买卖委托");
		}

		return 0;
	}

	// 将包ID的删除
	m_pOrderMap->erase(packetId);

	for (int i = 0; i < count; ++i)
	{
		RSP_PTMMWT_91001* pRsp = &pRsps[i];

		// 两个地方都会出现
		pField = (OrderField*)m_pOrderMap->findOrderXAPI(pRsp->extsystem_id);
		if (pField == nullptr)
		{
			printf("91001 普通买卖委托，未找到订单，extsystem_id:%d，entrust_no:%d\r\n", pRsp->extsystem_id, pRsp->entrust_no);
			continue;
		}

		// 更新订单号
		if (pRsp->entrust_no == 0)
		{
			// 价格超出涨跌停时，委托编号不会返回
		}
		else
		{
			sprintf(pField->ID, "%d", pRsp->entrust_no);
			sprintf(pField->OrderID, "%d", pRsp->entrust_no);

			// 更新Map，如果自定义数字与原有数据冲突是否有可能？所以要自义定
			m_pOrderMap->replace(pRsp->extsystem_id, pRsp->entrust_no, pField, pField->pUserData1);
		}

		// 可以从回报中取，也可以从历史中取
		pField->CumQty = 0;
		pField->LeavesQty = pField->Qty;
		pField->RawErrorID = pErr->ErrorCode;
		memset(pField->Text, 0, sizeof(pField->Text));
		SAFE_SNPRINTF(pField->Text, "91001 普通买卖委托");

		REQ_PTMMWT_91001* pWTField = (REQ_PTMMWT_91001*)pField->pUserData1;
		pWTField->batch_no = pRsp->batch_no;
		pWTField->entrust_no = pRsp->entrust_no;

		SAFE_STRNCPY(m_Text, pField->Text);
		SAFE_SNPRINTF(pField->Text, "%s:%s", m_Text, pRsp->fail_cause);

		if (pRsp->risk_serial_no != 0)
		{
			// 可能出现有风控编号，但没有风控条目
			for (int j = 0; j < riskCount; ++j)
			{
				RSP_RiskField* pRiskRsp = &pRiskRsps[j];
				if (pRiskRsp == nullptr)
					continue;
				if (pRsp->risk_serial_no == pRiskRsp->risk_serial_no)
				{
					// 找到相同序号的，但可能因为风控触发多个，如何通知多个呢？
					SAFE_STRNCPY(m_Text, pField->Text);
					SAFE_SNPRINTF(pField->Text, "%s:%s:%d:%s", m_Text, risk_operation_2_String(pRiskRsp->risk_operation), pRiskRsp->risk_no, pRiskRsp->risk_summary);
					break;
				}
			}
		}

		if (pRsp->entrust_fail_code == entrust_fail_code_CG)
		{
			// 成功了，但风控可能预警，所以还是要返回
			pField->ExecType = ExecType::ExecType_New;
			pField->Status = OrderStatus::OrderStatus_New;
		}
		else
		{
			// 如果立即撤单而走这里，会导致错误
			pField->ExecType = ExecType::ExecType_Rejected;
			pField->Status = OrderStatus::OrderStatus_Rejected;
		}

		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_91101(RSP_WTCD_91101* pRsps, int count, RSP_ErrorField3* pErr, int packetId)
{
	if (count == 0)
	{
		Do_Error(pErr, "91101 委托撤单");
		return 0;
	}

	OrderField* pField = nullptr;
	// 成功，看收到的编号是什么
	for (int i = 0; i < count; ++i)
	{
		RSP_WTCD_91101* pRsp = &pRsps[i];

		pField = (OrderField*)m_pOrderMap->findCancelAPI(pRsp->entrust_no);
		if (pField == nullptr)
			continue;
		memset(pField->Text, 0, sizeof(pField->Text));
		if (pRsp->success_flag == success_flag_CG)
		{
			//pField->ExecType = ExecType::ExecType_Cancelled;
			//pField->Status = OrderStatus::OrderStatus_Cancelled;
		}
		else
		{
			pField->ExecType = ExecType::ExecType_CancelReject;
			//pField->Status = entrust_state_2_OrderStatus(pRsp->entrust_state);

			pField->RawErrorID = pRsp->success_flag;
			SAFE_SNPRINTF(pField->Text, "91101 委托撤单:%s", pRsp->fail_cause);

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}
	}

	return 0;
}

int CO32Processor::Do_32001(RSP_ZQWTCX_32001* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (0 != Do_Error(pErr, "32001 证券委托查询"))
		return 0;

	if (count == 0)
	{
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryOrder, m_msgQueue, m_pClass, true, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		return 0;
	}

	OrderField* pField = nullptr;
	for (int i = 0; i < count; ++i)
	{
		RSP_ZQWTCX_32001* pRsp = &pRsps[i];

		pField = (OrderField*)m_pOrderMap->findOrderXAPI(pRsp->entrust_no);
		if (pField)
		{
			pField->Status = entrust_state_2_OrderStatus(pRsp->entrust_state);
			pField->ExecType = entrust_state_2_ExecType(pRsp->entrust_state);
			pField->CumQty = pRsp->deal_amount;
			pField->LeavesQty = pField->Qty - pField->CumQty;
		}
		else
		{
			pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
			sprintf(pField->Symbol, "%s.%s", pRsp->stock_code,
				ExchangeType_2_String(market_no_2_ExchangeType(pRsp->market_no)));
			SAFE_STRNCPY(pField->InstrumentID, pRsp->stock_code);
			SAFE_STRNCPY(pField->ExchangeID, pRsp->market_no);
			//SAFE_STRNCPY(pField->InstrumentName, pRsp->);
			SAFE_STRNCPY(pField->AccountID, pRsp->stockholder_id);
			pField->Side = entrust_direction_2_OrderSide(pRsp->entrust_direction);
			pField->OpenClose = entrust_direction_2_OpenCloseType(pRsp->entrust_direction);
			pField->Qty = pRsp->entrust_amount;
			pField->Price = pRsp->entrust_price;
			pField->Date = pRsp->entrust_date;
			pField->Time = pRsp->entrust_time;
			sprintf(pField->ID, "%d", pRsp->entrust_no);
			sprintf(pField->OrderID, "%d", pRsp->entrust_no);

			pField->Type = OrderType::OrderType_Limit;
			pField->TimeInForce = TimeInForce::TimeInForce_Day;
			pField->Status = entrust_state_2_OrderStatus(pRsp->entrust_state);
			pField->ExecType = entrust_state_2_ExecType(pRsp->entrust_state);
			pField->CumQty = pRsp->deal_amount;
			pField->LeavesQty = pRsp->entrust_amount - pRsp->deal_amount;

			SAFE_STRNCPY(pField->PortfolioID1, pRsp->account_code);
			SAFE_STRNCPY(pField->PortfolioID2, pRsp->asset_no);
			SAFE_STRNCPY(pField->PortfolioID3, pRsp->combi_no);

			//REQ_PTMMWT_91001* _pWTField = (REQ_PTMMWT_91001*)pField->pUserData1;
			//OrderField_2_REQ_PTMMWT_91001(nullptr, pField, _pWTField);
			//pField->pUserData1 = _pWTField;
			m_pOrderMap->insert(pRsp->entrust_no, pField, pField->pUserData1);
		}

		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_32003(RSP_QHWTCX_32003* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (0 != Do_Error(pErr, "32003 期货委托查询"))
		return 0;

	if (count == 0)
	{
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryOrder, m_msgQueue, m_pClass, true, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		return 0;
	}

	OrderField* pField = nullptr;
	for (int i = 0; i < count; ++i)
	{
		RSP_QHWTCX_32003* pRsp = &pRsps[i];

		pField = (OrderField*)m_pOrderMap->findOrderXAPI(pRsp->entrust_no);
		if (pField)
		{
			pField->Status = entrust_state_2_OrderStatus(pRsp->entrust_state);
			pField->ExecType = entrust_state_2_ExecType(pRsp->entrust_state);
			pField->CumQty = pRsp->deal_amount;
			pField->LeavesQty = pField->Qty - pField->CumQty;
		}
		else
		{
			pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
			sprintf(pField->Symbol, "%s.%s", pRsp->stock_code,
				ExchangeType_2_String(market_no_2_ExchangeType(pRsp->market_no)));
			SAFE_STRNCPY(pField->InstrumentID, pRsp->stock_code);
			SAFE_STRNCPY(pField->ExchangeID, pRsp->market_no);
			//SAFE_STRNCPY(pField->InstrumentName, pRsp->);
			SAFE_STRNCPY(pField->AccountID, pRsp->stockholder_id);
			pField->Side = entrust_direction_2_OrderSide(pRsp->entrust_direction);
			pField->Qty = pRsp->entrust_amount;
			pField->Price = pRsp->entrust_price;
			pField->OpenClose = futures_direction_2_OpenCloseType(pRsp->futures_direction);
			pField->Date = pRsp->entrust_date;
			pField->Time = pRsp->entrust_time;
			sprintf(pField->ID, "%d", pRsp->entrust_no);
			sprintf(pField->OrderID, "%s", pRsp->exchange_report_no);

			pField->Type = OrderType::OrderType_Limit;
			pField->TimeInForce = TimeInForce::TimeInForce_Day;
			pField->Status = entrust_state_2_OrderStatus(pRsp->entrust_state);
			pField->ExecType = entrust_state_2_ExecType(pRsp->entrust_state);
			pField->CumQty = pRsp->deal_amount;
			pField->LeavesQty = pField->Qty - pField->CumQty;

			SAFE_STRNCPY(pField->PortfolioID1, pRsp->account_code);
			SAFE_STRNCPY(pField->PortfolioID2, pRsp->asset_no);
			SAFE_STRNCPY(pField->PortfolioID3, pRsp->combi_no);

			m_pOrderMap->insert(pRsp->entrust_no, pField, pField->pUserData1);
		}

		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_33003(RSP_QHCJCX_33003* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (0 != Do_Error(pErr, "33003 期货成交查询"))
		return 0;

	if (count == 0)
	{
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTrade, m_msgQueue, m_pClass, true, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		return 0;
	}

	for (int i = 0; i < count; ++i)
	{
		RSP_QHCJCX_33003* pRsp = &pRsps[i];

		TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));
		sprintf(pField->Symbol, "%s.%s", pRsp->stock_code,
			ExchangeType_2_String(market_no_2_ExchangeType(pRsp->market_no)));
		SAFE_STRNCPY(pField->InstrumentID, pRsp->stock_code);
		SAFE_STRNCPY(pField->ExchangeID, pRsp->market_no);
		//SAFE_STRNCPY(pField->InstrumentName, pRsp->);
		SAFE_STRNCPY(pField->AccountID, pRsp->stockholder_id);
		pField->Side = entrust_direction_2_OrderSide(pRsp->entrust_direction);
		pField->Qty = pRsp->deal_amount;
		pField->Price = pRsp->deal_price;
		pField->OpenClose = futures_direction_2_OpenCloseType(pRsp->futures_direction);
		pField->Date = pRsp->deal_date;
		pField->Time = pRsp->deal_time;
		sprintf(pField->ID, "%d", pRsp->entrust_no);
		SAFE_SNPRINTF(pField->TradeID, "%s", pRsp->deal_no);
		pField->Commission = pRsp->total_fee;

		SAFE_STRNCPY(pField->PortfolioID1, pRsp->account_code);
		SAFE_STRNCPY(pField->PortfolioID2, pRsp->asset_no);
		SAFE_STRNCPY(pField->PortfolioID3, pRsp->combi_no);
		//pField->Position = real_buy_amount;

		//m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnTrade, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTrade, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_33001(RSP_ZQCJCX_33001* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (0 != Do_Error(pErr, "33001 证券成交查询"))
		return 0;

	if (count == 0)
	{
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTrade, m_msgQueue, m_pClass, true, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		return 0;
	}

	for (int i = 0; i < count; ++i)
	{
		RSP_ZQCJCX_33001* pRsp = &pRsps[i];

		TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));
		sprintf(pField->Symbol, "%s.%s", pRsp->stock_code,
			ExchangeType_2_String(market_no_2_ExchangeType(pRsp->market_no)));
		SAFE_STRNCPY(pField->InstrumentID, pRsp->stock_code);
		SAFE_STRNCPY(pField->ExchangeID, pRsp->market_no);
		//SAFE_STRNCPY(pField->InstrumentName, pRsp->);
		SAFE_STRNCPY(pField->AccountID, pRsp->stockholder_id);
		pField->Side = entrust_direction_2_OrderSide(pRsp->entrust_direction);
		pField->OpenClose = entrust_direction_2_OpenCloseType(pRsp->entrust_direction);
		pField->Qty = pRsp->deal_amount;
		pField->Price = pRsp->deal_price;
		pField->Date = pRsp->deal_date;
		pField->Time = pRsp->deal_time;
		sprintf(pField->ID, "%d", pRsp->extsystem_id);
		SAFE_SNPRINTF(pField->TradeID, "%s", pRsp->deal_no);
		pField->Commission = pRsp->total_fee;

		SAFE_STRNCPY(pField->PortfolioID1, pRsp->account_code);
		SAFE_STRNCPY(pField->PortfolioID2, pRsp->asset_no);
		SAFE_STRNCPY(pField->PortfolioID3, pRsp->combi_no);
		//pField->Position = real_buy_amount;

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTrade, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_a(RSP_WTXD_a* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (count == 0)
	{
		Do_Error(pErr, "RSP_WTXD_a 委托下达");
		return 0;
	}

	// 表示柜台接受了，会提供一个委托ID
	OrderField* pField = nullptr;
	for (int i = 0; i < count; ++i)
	{
		RSP_WTXD_a* pRsp = &pRsps[i];

		if (pRsp->extsystem_id == 0)
		{
			pField = (OrderField*)m_pOrderMap->findOrderXAPI(pRsp->entrust_no);
		}
		else
		{
			pField = (OrderField*)m_pOrderMap->findOrderXAPI(pRsp->extsystem_id);
		}

		if (pField == nullptr)
		{
			pField = (OrderField*)m_pOrderMap->findOrderXAPI(pRsp->entrust_no);
			if (pField == nullptr)
			{
				printf("RSP_WTXD_a 委托下达，未找到订单，extsystem_id:%d，entrust_no:%d\r\n", pRsp->extsystem_id, pRsp->entrust_no);
				//continue;
			}
		}

		if (pField)
		{
		}
		else
		{
			pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
			SAFE_SNPRINTF(pField->Symbol, "%s.%s", pRsp->stock_code,
				ExchangeType_2_String(market_no_2_ExchangeType(pRsp->market_no)));
			SAFE_STRNCPY(pField->InstrumentID, pRsp->stock_code);
			SAFE_STRNCPY(pField->ExchangeID, pRsp->market_no);
			//SAFE_STRNCPY(pField->InstrumentName, pRsp->);
			SAFE_STRNCPY(pField->AccountID, pRsp->stockholder_id);
			pField->Side = entrust_direction_2_OrderSide(pRsp->entrust_direction);
			pField->Qty = pRsp->entrust_amount;
			pField->Price = pRsp->entrust_price;
			pField->OpenClose = futures_direction_2_OpenCloseType(pRsp->futures_direction);
			pField->Date = pRsp->business_date;
			pField->Time = pRsp->business_time;

			pField->Type = OrderType::OrderType_Limit;
			pField->TimeInForce = TimeInForce::TimeInForce_Day;

			SAFE_STRNCPY(pField->PortfolioID1, pRsp->account_code);
			SAFE_STRNCPY(pField->PortfolioID2, pRsp->asset_no);
			SAFE_STRNCPY(pField->PortfolioID3, pRsp->combi_no);

			m_pOrderMap->insert(pRsp->entrust_no, pField, pField->pUserData1);
		}

		// 更新订单号
		sprintf(pField->ID, "%d", pRsp->entrust_no);
		sprintf(pField->OrderID, "%s", pRsp->report_no);

		// 更新Map，如果自定义数字与原有数据冲突是否有可能？所以要自义定
		m_pOrderMap->replace(pRsp->extsystem_id, pRsp->entrust_no, pField, pField->pUserData1);

		// 可以从回报中取，也可以从历史中取
		pField->CumQty = 0;
		pField->LeavesQty = pField->Qty;
		memset(pField->Text, 0, sizeof(pField->Text));
		SAFE_SNPRINTF(pField->Text, "RSP_WTXD_a 委托下达");

		pField->Status = entrust_state_2_OrderStatus(pRsp->entrust_status);
		pField->ExecType = entrust_state_2_ExecType(pRsp->entrust_status);

		if (pErr->ErrorCode == 0)
		{
		}
		else
		{
			// 不应当走的路径
			pField->RawErrorID = pErr->ErrorCode;
			SAFE_STRNCPY(m_Text, pField->Text);
			SAFE_SNPRINTF(pField->Text, "%s:ErrorMsg:%s,MsgDetail:%s", m_Text, pErr->ErrorMsg, pErr->MsgDetail);
		}

		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_b(RSP_WTQR_b* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (count == 0)
	{
		Do_Error(pErr, "RSP_WTQR_b 委托确认");
		return 0;
	}

	for (int i = 0; i < count; ++i)
	{
		RSP_WTQR_b* pRsp = &pRsps[i];

		// 这里收到的报单会由 1变4，即由未报变成已报
		// 这种状态我们并不是很关心
		//TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));
		//sprintf(pField->Symbol, "%s.%s", pRsp->stock_code,
		//	ExchangeType_2_String(market_no_2_ExchangeType(pRsp->market_no)));
		//SAFE_STRNCPY(pField->InstrumentID, pRsp->stock_code);
		//SAFE_STRNCPY(pField->ExchangeID, pRsp->market_no);
		////SAFE_STRNCPY(pField->InstrumentName, pRsp->stock_name); // 股票的中文没办法输出了
		//SAFE_STRNCPY(pField->AccountID, pRsp->stockholder_id);
		//pField->Side = entrust_direction_2_OrderSide(pRsp->entrust_direction);
		//pField->OpenClose = futures_direction_2_OpenCloseType(pRsp->futures_direction);
		//pField->Qty = pRsp->entrust_amount;
		//sprintf(pField->ID, "%d", pRsp->entrust_no);

		//SAFE_STRNCPY(pField->PortfolioID1, pRsp->account_code);
		////SAFE_STRNCPY(pField->PortfolioID2, pRsp->asset_no); // 这里是个坑
		//SAFE_STRNCPY(pField->PortfolioID3, pRsp->combi_no);

		//m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnTrade, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);

		//m_msgQueue->delete_block(pField);
	}

	return 0;
}

int CO32Processor::Do_c(RSP_WTFD_c* pRsps, int count, RSP_ErrorField3* pErr)
{
	// 已撤单报单被拒绝DCE:该品种当前是开市暂停!
	// a -> b -> c
	if (count == 0)
	{
		Do_Error(pErr, "RSP_WTFD_c 委托废单");
		return 0;
	}

	OrderField* pField = nullptr;
	for (int i = 0; i < count; ++i)
	{
		RSP_WTFD_c* pRsp = &pRsps[i];

		pField = (OrderField*)m_pOrderMap->findOrderXAPI(pRsp->entrust_no);
		if (pField == nullptr)
			continue;

		pField->Status = entrust_state_2_OrderStatus(pRsp->entrust_status);
		pField->ExecType = entrust_state_2_ExecType(pRsp->entrust_status);
		pField->CumQty = 0;
		pField->LeavesQty = pField->Qty;

		// 这个出现过单子有部分乱码的问题
		memset(pField->Text, 0, sizeof(pField->Text));
		SAFE_SNPRINTF(pField->Text, "RSP_WTFD_c 委托废单:%s", pRsp->revoke_cause);

		if (pErr->ErrorCode == 0)
		{
		}
		else
		{
			// 不应当走的路径
			pField->RawErrorID = pErr->ErrorCode;
			SAFE_SNPRINTF(pField->Text, "RSP_WTFD_c 委托废单:ErrorMsg:%s,MsgDetail:%s", pErr->ErrorMsg, pErr->MsgDetail);
		}

		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_d(RSP_WTCD_d* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (count == 0)
	{
		Do_Error(pErr, "RSP_WTCD_d 委托撤单");
		return 0;
	}

	OrderField* pField = nullptr;
	for (int i = 0; i < count; ++i)
	{
		RSP_WTCD_d* pRsp = &pRsps[i];

		pField = (OrderField*)m_pOrderMap->findOrderXAPI(pRsp->cancel_entrust_no);
		if (pField)
		{
		}
		else
		{
			// 由于信息不全，只好返回
			continue;
		}

		pField->Status = entrust_state_2_OrderStatus(pRsp->entrust_status);
		pField->ExecType = entrust_state_2_ExecType(pRsp->entrust_status);
		memset(pField->Text, 0, sizeof(pField->Text));
		SAFE_SNPRINTF(pField->Text, "RSP_WTCD_d 委托撤单:%s", pRsp->revoke_cause);

		if (pErr->ErrorCode == 0)
		{
		}
		else
		{
			// 不应当走的路径
			pField->RawErrorID = pErr->ErrorCode;
			SAFE_SNPRINTF(pField->Text, "RSP_WTCD_d 委托撤单:ErrorMsg:%s,MsgDetail:%s", pErr->ErrorMsg, pErr->MsgDetail);
		}

		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_e(RSP_WTCC_e* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (count == 0)
	{
		Do_Error(pErr, "RSP_WTCC_e 委托撤成");
		return 0;
	}

	OrderField* pField = nullptr;
	for (int i = 0; i < count; ++i)
	{
		RSP_WTCC_e* pRsp = &pRsps[i];

		pField = (OrderField*)m_pOrderMap->findOrderXAPI(pRsp->cancel_entrust_no);
		if (pField)
		{
		}
		else
		{
			pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
			SAFE_SNPRINTF(pField->Symbol, "%s.%s", pRsp->stock_code,
				ExchangeType_2_String(market_no_2_ExchangeType(pRsp->market_no)));
			SAFE_STRNCPY(pField->InstrumentID, pRsp->stock_code);
			SAFE_STRNCPY(pField->ExchangeID, pRsp->market_no);
			//SAFE_STRNCPY(pField->InstrumentName, pRsp->);
			SAFE_STRNCPY(pField->AccountID, pRsp->stockholder_id);
			pField->Side = entrust_direction_2_OrderSide(pRsp->entrust_direction);
			pField->Qty = pRsp->entrust_amount;
			//pField->Price = pRsp->entrust_price;
			pField->OpenClose = futures_direction_2_OpenCloseType(pRsp->futures_direction);
			pField->Date = pRsp->business_date;
			pField->Time = pRsp->business_time;

			pField->Type = OrderType::OrderType_Limit;
			pField->TimeInForce = TimeInForce::TimeInForce_Day;

			SAFE_STRNCPY(pField->PortfolioID1, pRsp->account_code);
			//SAFE_STRNCPY(pField->PortfolioID2, pRsp->asset_no);
			SAFE_STRNCPY(pField->PortfolioID3, pRsp->combi_no);

			m_pOrderMap->insert(pRsp->cancel_entrust_no, pField, pField->pUserData1);
		}

		sprintf(pField->ID, "%d", pRsp->cancel_entrust_no);
		//sprintf(pField->OrderID, "%s", pRsp->report_no);

		pField->Status = entrust_state_2_OrderStatus(pRsp->entrust_status);
		pField->ExecType = entrust_state_2_ExecType(pRsp->entrust_status);

		pField->CumQty = pRsp->entrust_amount - pRsp->cancel_amount;
		pField->LeavesQty = 0;
		memset(pField->Text, 0, sizeof(pField->Text));
		SAFE_STRNCPY(pField->Text, "RSP_WTCC_e 委托撤成");
		if (pErr->ErrorCode == 0)
		{
		}
		else
		{
			// 不应当走的路径
			pField->RawErrorID = pErr->ErrorCode;
			SAFE_SNPRINTF(pField->Text, "RSP_WTCC_e 委托撤成:ErrorMsg:%s,MsgDetail:%s", pErr->ErrorMsg, pErr->MsgDetail);
		}
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CO32Processor::Do_f(RSP_WTCD_f* pRsps, int count, RSP_ErrorField3* pErr)
{
	// 有必要测试一下重复撤单和全成后撤单
	if (count == 0)
	{
		Do_Error(pErr, "RSP_WTCD_f 委托撤废");
		return 0;
	}

	OrderField* pField = nullptr;
	for (int i = 0; i < count; ++i)
	{
		RSP_WTCD_f* pRsp = &pRsps[i];

		pField = (OrderField*)m_pOrderMap->findOrderXAPI(pRsp->cancel_entrust_no);
		if (pField == nullptr)
			continue;

		pField->Status = entrust_state_2_OrderStatus(pRsp->entrust_status);
		pField->ExecType = entrust_state_2_ExecType(pRsp->entrust_status);

		memset(pField->Text, 0, sizeof(pField->Text));
		SAFE_SNPRINTF(pField->Text, "RSP_WTCD_f 委托撤废:%s", pRsp->revoke_cause);

		if (pErr->ErrorCode == 0)
		{
		}
		else
		{
			// 不应当走的路径
			pField->RawErrorID = pErr->ErrorCode;
			SAFE_SNPRINTF(pField->Text, "RSP_WTCD_f 委托撤废:ErrorMsg:%s,MsgDetail:%s", pErr->ErrorMsg, pErr->MsgDetail);
		}
		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	}

	return 0;
}


// 收到成交回报了，但上层并没有反应过来
int CO32Processor::Do_g(RSP_WTCJ_g* pRsps, int count, RSP_ErrorField3* pErr)
{
	if (count == 0)
	{
		Do_Error(pErr, "RSP_WTCJ_g 委托成交");
		return 0;
	}

	for (int i = 0; i < count; ++i)
	{
		RSP_WTCJ_g* pRsp = &pRsps[i];

		TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));
		SAFE_SNPRINTF(pField->Symbol, "%s.%s", pRsp->stock_code,
			ExchangeType_2_String(market_no_2_ExchangeType(pRsp->market_no)));
		SAFE_STRNCPY(pField->InstrumentID, pRsp->stock_code);
		SAFE_STRNCPY(pField->ExchangeID, pRsp->market_no);
		//SAFE_STRNCPY(pField->InstrumentName, pRsp->stock_name); // 股票的中文没办法输出了
		SAFE_STRNCPY(pField->AccountID, pRsp->stockholder_id);
		pField->Side = entrust_direction_2_OrderSide(pRsp->entrust_direction);
		pField->OpenClose = futures_direction_2_OpenCloseType(pRsp->futures_direction);
		pField->Qty = pRsp->deal_amount;
		pField->Price = pRsp->deal_price;
		pField->Date = pRsp->deal_date;
		pField->Time = pRsp->deal_time;
		sprintf(pField->ID, "%d", pRsp->entrust_no);
		SAFE_SNPRINTF(pField->TradeID, "%s", pRsp->deal_no);
		pField->Commission = pRsp->deal_fee;

		SAFE_STRNCPY(pField->PortfolioID1, pRsp->account_code);
		//SAFE_STRNCPY(pField->PortfolioID2, pRsp->asset_no); // 这里是个坑
		SAFE_STRNCPY(pField->PortfolioID3, pRsp->combi_no);

		m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnTrade, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);

		m_msgQueue->delete_block(pField);
	}

	return 0;
}
