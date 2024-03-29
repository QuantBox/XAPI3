﻿#pragma once

#include <string>
#include <map>
#include <list>

#include "../../include/ApiStruct.h"

using namespace std;

PositionSide TradeField_2_PositionSide(TradeField* pIn)
{
	if (pIn->OpenClose == OpenCloseType::OpenCloseType_Open)
	{
		if (pIn->Side == OrderSide::OrderSide_Buy)
			return PositionSide::PositionSide_Long;
		return PositionSide::PositionSide_Short;
	}
	else
	{
		if (pIn->Side == OrderSide::OrderSide_Buy)
			return PositionSide::PositionSide_Short;
		return PositionSide::PositionSide_Long;
	}
}


class PositionManager
{
public:
	PositionManager(void) {}
	virtual ~PositionManager(void)
	{
		Clear();
	}

	PositionField* OnPosition(PositionField* pPosition)
	{
		// 根据持仓合计更新内存中的持仓
		PositionField* pField = nullptr;
		MakeID(pPosition);
		auto it = positions.find(pPosition->ID);
		if (it == positions.end())
		{
			pField = new PositionField();
			memcpy(pField, pPosition, sizeof(PositionField));

			positions.insert(pair<string, PositionField*>(pField->ID, pField));
		}
		else
		{
			pField = it->second;
			memcpy(pField, pPosition, sizeof(PositionField));
		}
		return pField;
	}

	PositionField* OnPositionDetail(PositionField* pPosition)
	{
		// 持仓明细，需要进行汇总
		PositionField* pField = nullptr;
		MakeID(pPosition);
		auto it = positions.find(pPosition->ID);
		if (it == positions.end())
		{
			pField = new PositionField();
			memcpy(pField, pPosition, sizeof(PositionField));

			positions.insert(pair<string, PositionField*>(pField->ID, pField));
		}
		else
		{
			pField = it->second;
			pField->Position += pPosition->Position;
			pField->TodayPosition += pPosition->TodayPosition;
			pField->HistoryPosition += pPosition->HistoryPosition;
		}

		return pField;
	}

	void MakeID(PositionField* pField)
	{
		sprintf(pField->ID, "%s:%s:%d:%d", pField->ExchangeID, pField->InstrumentID, pField->Side, pField->HedgeFlag);
	}

	PositionField* OnTrade(TradeField* pTrade)
	{
		// 根据成交回报计算持仓

		PositionSide position_side = TradeField_2_PositionSide(pTrade);
		PositionIDType positionId = { 0 };
		sprintf(positionId, "%s:%s:%d:%d", pTrade->ExchangeID, pTrade->InstrumentID, position_side, pTrade->HedgeFlag);

		PositionField* pField = nullptr;
		auto it = positions.find(positionId);
		if (it == positions.end())
		{
			pField = new PositionField();

			strcpy(pField->ID, positionId);
			strcpy(pField->Symbol, pTrade->InstrumentID);
			strcpy(pField->InstrumentID, pTrade->InstrumentID);
			strcpy(pField->ExchangeID, pTrade->ExchangeID);
			strcpy(pField->AccountID, pTrade->AccountID);
			pField->Side = position_side;
			pField->HedgeFlag = pTrade->HedgeFlag;

			positions.insert(pair<string, PositionField*>(positionId, pField));
		}
		else
		{
			pField = it->second;
		}

		if (pTrade->OpenClose == OpenCloseType::OpenCloseType_Open)
		{
			pField->Position += pTrade->Qty;
			pField->TodayPosition += pTrade->Qty;
		}
		else
		{
			pField->Position -= pTrade->Qty;
			if (pTrade->OpenClose == OpenCloseType::OpenCloseType_CloseToday)
			{
				pField->TodayPosition -= pTrade->Qty;
			}
			else
			{
				pField->HistoryPosition -= pTrade->Qty;
				// 如果昨天的被减成负数，从今天开始继续减
				if (pField->HistoryPosition < 0)
				{
					pField->TodayPosition += pField->HistoryPosition;
					pField->HistoryPosition = 0;
				}
			}
		}

		return pField;
	}

	bool CheckPosition(PositionField* pField)
	{
		// 计算错误，需要重新查询
		if (pField->Position < 0 || pField->TodayPosition < 0 || pField->HistoryPosition < 0)
		{
			return false;
		}
		return true;
	}

	void Clear()
	{
		for (auto it = positions.begin(); it != positions.end(); ++it)
			delete it->second;
		positions.clear();
	}

	list<PositionField*> All()
	{
		list<PositionField*> ll;
		for (auto it = positions.begin(); it != positions.end(); ++it)
			ll.push_back(it->second);

		return ll;
	}

private:
	map<string, PositionField*> positions;
};

