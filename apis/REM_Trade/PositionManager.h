#pragma once

#include <string>
#include <map>

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

	void OnPosition(PositionField* pPosition)
	{
		// 根据持仓更新内存中的持仓
		PositionField* pField = nullptr;
		auto it = posiitons.find(pPosition->ID);
		if (it == posiitons.end())
		{
			pField = new PositionField();
			memcpy(pField, pPosition, sizeof(PositionField));

			posiitons.insert(pair<string, PositionField*>(pField->ID, pField));
		}
		else
		{
			pField = it->second;
			memcpy(pField, pPosition, sizeof(PositionField));
		}
	}

	PositionField* OnTrade(TradeField* pTrade)
	{
		// 根据成交回报计算持仓

		PositionSide position_side = TradeField_2_PositionSide(pTrade);
		PositionIDType positionId = { 0 };
		sprintf(positionId, "%s:%s:%d:%d", pTrade->ExchangeID, pTrade->InstrumentID, position_side, pTrade->HedgeFlag);

		PositionField* pField = nullptr;
		auto it = posiitons.find(positionId);
		if (it == posiitons.end())
		{
			pField = new PositionField();

			strcpy(pField->Symbol, pTrade->InstrumentID);
			strcpy(pField->InstrumentID, pTrade->InstrumentID);
			strcpy(pField->ExchangeID, pTrade->ExchangeID);
			strcpy(pField->AccountID, pTrade->AccountID);
			pField->Side = position_side;
			pField->HedgeFlag = pTrade->HedgeFlag;

			posiitons.insert(pair<string, PositionField*>(positionId, pField));
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
		for (auto it = posiitons.begin(); it != posiitons.end(); ++it)
			delete it->second;
		posiitons.clear();
	}

private:
	map<string, PositionField*> posiitons;
};

