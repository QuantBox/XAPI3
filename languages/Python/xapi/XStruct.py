#!/usr/bin/env python
# -*- coding: utf-8 -*-


# import pydevd
from .XDataType import *
from .XEnum import *
from datetime import datetime
import ctypes
import json

# 定义枚举类型在C结接体中的类型
_BusinessType = c_byte
_PositionSide = c_byte
_HedgeFlagType = c_byte
_OrderStatus = c_byte
_ExecType = c_byte
_OpenCloseType = c_byte
_OrderSide = c_byte
_OrderType = c_byte
_TimeInForce = c_byte
_ResumeType = c_byte
_LogLevel = c_byte
_InstrumentType = c_byte
_PutCall = c_byte
_IdCardType = c_byte
_ExchangeType = c_byte
_TradingPhaseType = c_byte
_InstLifePhaseType = c_byte

import sys


# 转码工具，输入u，输出根据版本变化
def to_str(s):
    if isinstance(u'0', str):
        # py3
        # 但在PyCharm中的控制台中还是显示乱码
        return s
    else:
        # py2
        # DOS中需要设置成gbk
        # 但PyCharm Debug中需要设置成UTF-8才能看
        # print sys.stdin.encoding
        return s.encode(sys.stdin.encoding)
        # return s.encode('UTF-8')
        # return s.encode('gbk')


def from_dict(self, d):
    """
    将字典填充到结构体中
    1. 字符串做转码
    2. 枚举传入的是数字时不用额外处理
    :param self:
    :param d:
    :return:
    """
    for k, v in d.items():
        _v = getattr(self, k)
        if isinstance(_v, bytes):
            setattr(self, k, v.encode('gbk'))
        else:
            setattr(self, k, v)


def to_dict(self, d, skips):
    """
    将结构体转成字典
    1. 需要跳过的字段需要提前指定
    2. 字符串会转编码
    3. 特别处理的字段可以之前或之后进行处理
    :param self:
    :param d:
    :param skips:
    :return:
    """
    for k, v in self._fields_:
        if k in skips:
            continue
        _v = getattr(self, k)
        if v._type_ == ctypes.c_char and v._length_ > 1:
            _v = _v.decode('gbk')
        d[k] = _v

    return d


class ReqQueryField(Structure):
    _pack_ = 1
    _fields_ = [
        ('InstrumentName', InstrumentNameType),
        ('Symbol', SymbolType),
        ('InstrumentID', InstrumentIDType),
        ('ExchangeID', ExchangeIDType),
        ('ClientID', IDChar32Type),
        ('AccountID', IDChar32Type),

        ('CurrencyID', CurrencyIDType),

        ('DateStart', DateIntType),
        ('DateEnd', DateIntType),
        ('TimeStart', TimeIntType),
        ('TimeEnd', TimeIntType),

        ('Char64ID', OrderIDType),
        ('Int32ID', Int32Type),
        ('Char64PositionIndex', OrderIDType),
        ('Int32PositionIndex', Int32Type),

        ('PortfolioID1', IDChar32Type),
        ('PortfolioID2', IDChar32Type),
        ('PortfolioID3', IDChar32Type),
        ('Business', _BusinessType),
    ]


class PositionField(Structure):
    _pack_ = 1
    _fields_ = [
        ('InstrumentName', InstrumentNameType),
        ('Symbol', SymbolType),
        ('InstrumentID', InstrumentIDType),
        ('ExchangeID', ExchangeIDType),
        ('ClientID', IDChar32Type),
        ('AccountID', IDChar32Type),

        ('Side', _PositionSide),
        ('HedgeFlag', _HedgeFlagType),
        ('Date', DateIntType),
        ('PositionCost', MoneyType),

        ('Position', QtyType),
        ('TodayPosition', QtyType),
        ('HistoryPosition', QtyType),
        ('HistoryFrozen', QtyType),

        ('TodayBSPosition', QtyType),
        ('TodayBSFrozen', QtyType),
        ('TodayPRPosition', QtyType),
        ('TodayPRFrozen', QtyType),
        ('ID', PositionIDType),

        ('PortfolioID1', IDChar32Type),
        ('PortfolioID2', IDChar32Type),
        ('PortfolioID3', IDChar32Type),
        ('Business', _BusinessType),
    ]

    def get_instrument_id(self):
        return self.InstrumentID.decode('gbk')

    def get_exchange_id(self):
        return self.ExchangeID.decode('gbk')

    def get_instrument_name(self):
        return self.InstrumentName.decode('gbk')

    def get_id(self):
        return self.ID.decode('gbk')

    def __len__(self):
        return sizeof(self)

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        d = {
            'HedgeFlag': HedgeFlagType[self.HedgeFlag],
            'Side': PositionSide[self.Side]
        }
        return to_dict(self, d, ['Side', 'HedgeFlag', 'Business'])


class QuoteField(Structure):
    _pack_ = 1
    _fields_ = [
        ('InstrumentName', InstrumentNameType),
        ('Symbol', SymbolType),
        ('InstrumentID', InstrumentIDType),
        ('ExchangeID', ExchangeIDType),
        ('ClientID', IDChar32Type),
        ('AccountID', IDChar32Type),

        ('AskQty', QtyType),
        ('AskPrice', PriceType),
        ('AskOpenClose', _OpenCloseType),
        ('AskHedgeFlag', _HedgeFlagType),

        ('BidQty', QtyType),
        ('BidPrice', PriceType),
        ('BidOpenClose', _OpenCloseType),
        ('BidHedgeFlag', _HedgeFlagType),

        ('Status', _OrderStatus),
        ('ExecType', _ExecType),

        ('QuoteReqID', OrderIDType),
        ('LocalID', OrderIDType),
        ('ID', OrderIDType),
        ('AskID', OrderIDType),
        ('BidID', OrderIDType),
        ('AskOrderID', OrderIDType),
        ('BidOrderID', OrderIDType),

        ('XErrorID', Int32Type),
        ('RawErrorID', Int32Type),
        ('Text', Char256Type),
    ]


class OrderField(Structure):
    _pack_ = 1
    _fields_ = [
        ('InstrumentName', InstrumentNameType),
        ('Symbol', SymbolType),
        ('InstrumentID', InstrumentIDType),
        ('ExchangeID', ExchangeIDType),
        ('ClientID', IDChar32Type),
        ('AccountID', IDChar32Type),

        ('Side', _OrderSide),
        ('Qty', QtyType),
        ('Price', PriceType),
        ('OpenClose', _OpenCloseType),
        ('HedgeFlag', _HedgeFlagType),
        ('Date', DateIntType),
        ('Time', TimeIntType),

        ('ID', OrderIDType),

        ('OrderID', OrderIDType),
        ('LocalID', OrderIDType),

        ('Type', _OrderType),
        ('StopPx', PriceType),
        ('TimeInForce', _TimeInForce),
        ('Status', _OrderStatus),
        ('ExecType', _ExecType),
        ('LeavesQty', QtyType),
        ('CumQty', QtyType),
        ('AvgPx', PriceType),

        ('XErrorID', Int32Type),
        ('RawErrorID', Int32Type),
        ('Text', Char256Type),
        ('ReserveInt32', Int32Type),
        ('ReserveChar64', Char64Type),

        ('PortfolioID1', IDChar32Type),
        ('PortfolioID2', IDChar32Type),
        ('PortfolioID3', IDChar32Type),
        ('Business', _BusinessType),
    ]

    def get_instrument_id(self):
        return self.InstrumentID.decode('gbk')

    def get_exchange_id(self):
        return self.ExchangeID.decode('gbk')

    def get_local_id(self):
        return self.LocalID.decode('gbk')

    def get_id(self):
        return self.ID.decode('gbk')

    def get_order_id(self):
        return self.OrderID.decode('gbk')

    def get_text(self):
        return self.Text.decode('gbk')

    def __len__(self):
        return sizeof(self)

    def from_dict(self, d):
        from_dict(self, d)

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        d = {
            'Side': OrderSide[self.Side],
            'OpenClose': OpenCloseType[self.OpenClose],
            'HedgeFlag': HedgeFlagType[self.HedgeFlag],
            'Type': OrderType[self.Type],
            'TimeInForce': TimeInForce[self.TimeInForce],
            'Status': OrderStatus[self.Status],
            'ExecType': ExecType[self.ExecType],
            'Business': BusinessType[self.Business],
        }

        return to_dict(self,
                       d,
                       ['Side', 'OpenClose', 'HedgeFlag', 'Type',
                        'TimeInForce', 'Status', 'ExecType', 'Business'])


# 这个比较特殊，只是为了传一个结构体字符串
class OrderIDTypeField(Structure):
    _pack_ = 1
    _fields_ = [
        ('OrderIDType', OrderIDType),
    ]


class TradeField(Structure):
    _pack_ = 1
    _fields_ = [
        ('InstrumentName', InstrumentNameType),
        ('Symbol', SymbolType),
        ('InstrumentID', InstrumentIDType),
        ('ExchangeID', ExchangeIDType),
        ('ClientID', IDChar32Type),
        ('AccountID', IDChar32Type),

        ('Side', _OrderSide),
        ('Qty', QtyType),
        ('Price', PriceType),
        ('OpenClose', _OpenCloseType),
        ('HedgeFlag', _HedgeFlagType),
        ('Date', DateIntType),
        ('Time', TimeIntType),

        ('ID', OrderIDType),

        ('TradeID', TradeIDType),
        ('Commission', MoneyType),

        ('ReserveInt32', Int32Type),
        ('ReserveChar64', Char64Type),

        ('PortfolioID1', IDChar32Type),
        ('PortfolioID2', IDChar32Type),
        ('PortfolioID3', IDChar32Type),
        ('Business', _BusinessType),
    ]

    def get_instrument_id(self):
        return self.InstrumentID.decode('gbk')

    def get_instrument_name(self):
        return self.InstrumentName.decode('gbk')

    def get_exchange_id(self):
        return self.ExchangeID.decode('gbk')

    def get_id(self):
        return self.ID.decode('gbk')

    def get_trade_id(self):
        return self.TradeID.decode('gbk')

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        d = {
            'Side': OrderSide[self.Side],
            'OpenClose': OpenCloseType[self.OpenClose],
            'HedgeFlag': HedgeFlagType[self.HedgeFlag],
            'Business': BusinessType[self.Business],
        }

        return to_dict(self,
                       d,
                       ['Side', 'OpenClose', 'HedgeFlag', 'Business'])


class ServerInfoField(Structure):
    _pack_ = 1
    _fields_ = [
        ('IsUsingUdp', BooleanType),
        ('IsMulticast', BooleanType),
        ('TopicId', Int32Type),
        ('Port', Int32Type),

        ('MarketDataTopicResumeType', _ResumeType),
        ('PrivateTopicResumeType', _ResumeType),
        ('PublicTopicResumeType', _ResumeType),
        ('UserTopicResumeType', _ResumeType),

        ('BrokerID', BrokerIDType),
        ('UserProductInfo', ProductInfoType),
        ('AuthCode', AuthCodeType),
        ('AppID', AppIDType),
        ('Address', AddressType),
        ('ConfigPath', Char256Type),
        ('ExtInfoChar128', Char128Type),
    ]


class UserInfoField(Structure):
    _pack_ = 1
    _fields_ = [
        ('UserID', IDChar32Type),
        ('Password', PasswordType),
        ('ExtInfoChar64', Char64Type),
        ('ExtInfoInt32', Int32Type),
    ]


class ErrorField(Structure):
    _pack_ = 1
    _fields_ = [
        ('XErrorID', Int32Type),
        ('RawErrorID', Int32Type),
        ('Text', Char256Type),
        ('Source', Char64Type),
    ]

    def get_text(self):
        return self.Text.decode('gbk')

    def get_source(self):
        return self.Source.decode('gbk')

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        return to_dict(self, {}, [])


class InstrumentStatusField(Structure):
    _pack_ = 1
    _fields_ = [
        ('Symbol', SymbolType),
        ('InstrumentID', InstrumentIDType),
        ('ExchangeID', ExchangeIDType),
        ('InstrumentStatus', _TradingPhaseType),
        ('EnterTime', TimeIntType),
    ]

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        d = {
            'InstrumentStatus': TradingPhaseType[self.InstrumentStatus],
        }

        return to_dict(self,
                       d,
                       ['InstrumentStatus'])


class LogField(Structure):
    _pack_ = 1
    _fields_ = [
        ('Level', _LogLevel),
        ('Message', Char256Type),
    ]

    def get_message(self):
        return self.Message.decode('gbk')

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        d = {
            'Level': LogLevel[self.Level]
        }
        return to_dict(self, d, ['Level'])


class RspUserLoginField(Structure):
    _pack_ = 1
    _fields_ = [
        ('TradingDay', DateIntType),
        ('LoginTime', TimeIntType),
        ('SessionID', SessionIDType),
        ('UserID', IDChar32Type),
        ('AccountID', IDChar32Type),
        ('InvestorName', PartyNameType),
        ('XErrorID', Int32Type),
        ('RawErrorID', Int32Type),
        ('Text', Char256Type),
        ('Version', Char32Type),
        ('Lang', Char32Type),
    ]

    def get_session_id(self):
        return self.SessionID.decode('gbk')

    def get_investor_name(self):
        return self.InvestorName.decode('gbk')

    def get_text(self):
        return self.Text.decode('gbk')

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        return to_dict(self, {}, [])


class DepthField(Structure):
    _pack_ = 1
    _fields_ = [
        ('Price', PriceType),
        ('Size', VolumeType),
        ('Count', VolumeType)
    ]

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        return to_dict(self, {}, [])


# 深度行情N档
class DepthMarketDataNField(Structure):
    _pack_ = 1
    _fields_ = [
        # 占用总字节大小
        ('Size', SizeType),
        # 交易所时间
        # 交易日，用于给数据接收器划分到同一文件使用，基本没啥别的用处
        ('TradingDay', DateIntType),
        ('ActionDay', DateIntType),
        ('UpdateTime', TimeIntType),
        ('UpdateMillisec', TimeIntType),

        # 唯一符号
        ('Symbol', SymbolType),
        ('InstrumentID', InstrumentIDType),
        ('ExchangeID', ExchangeIDType),
        ('Exchange', _ExchangeType),

        # 最新价
        ('LastPrice', PriceType),
        # 数量
        ('Volume', LargeVolumeType),
        # 成交金额
        ('Turnover', MoneyType),
        # 持仓量
        ('OpenInterest', LargeVolumeType),
        # 当日均价
        ('AveragePrice', PriceType),
        # 今开盘
        ('OpenPrice', PriceType),
        # 最高价
        ('HighestPrice', PriceType),
        # 最低价
        ('LowestPrice', PriceType),
        # 今收盘
        ('ClosePrice', PriceType),
        # 本次结算价
        ('SettlementPrice', PriceType),

        # 涨停板价
        ('UpperLimitPrice', PriceType),
        # 跌停板价
        ('LowerLimitPrice', PriceType),
        # 昨收盘
        ('PreClosePrice', PriceType),
        # 上次结算价
        ('PreSettlementPrice', PriceType),
        # 昨持仓量
        ('PreOpenInterest', LargeVolumeType),

        ('TradingPhase', _TradingPhaseType),
        # 买档个数
        ('BidCount', SizeType)
    ]

    def get_symbol(self):
        return self.Symbol.decode('gbk')

    def get_instrument_id(self):
        return self.InstrumentID.decode('gbk')

    def get_exchange_id(self):
        return self.ExchangeID.decode('gbk')

    def get_bid_count(self):
        return self.BidCount

    def get_ticks_count(self):
        count = (self.Size - sizeof(DepthMarketDataNField)) / sizeof(DepthField)
        return int(count)

    def get_ask_count(self):
        return int(self.get_ticks_count() - self.BidCount)

    def get_bids(self):
        ticks = []
        for i in range(self.BidCount):
            p = addressof(self) + sizeof(DepthMarketDataNField) + sizeof(DepthField) * i
            p_t = cast(p, POINTER(DepthField)).contents
            ticks.append((p_t.Price, p_t.Size))
        return ticks

    def get_asks(self):
        ask_count = self.get_ask_count()

        ticks = []
        for i in range(ask_count):
            p = addressof(self) + sizeof(DepthMarketDataNField) + sizeof(DepthField) * (self.BidCount + i)
            p_t = cast(p, POINTER(DepthField)).contents
            ticks.append((p_t.Price, p_t.Size))
        return ticks

    def get_trading_date(self):
        return datetime.strptime(f'{self.TradingDay}', '%Y%m%d').date()

    def get_trading_date_str(self):
        return self.get_trading_date().strftime('%F')

    def get_action_datetime(self):
        if self.ActionDay == 0:
            return datetime.strptime(f'{self.TradingDay} {self.UpdateTime}.{self.UpdateMillisec:03}',
                                     '%Y%m%d %H%M%S.%f')
        return datetime.strptime(f'{self.ActionDay} {self.UpdateTime}.{self.UpdateMillisec:03}',
                                 '%Y%m%d %H%M%S.%f')

    def get_action_datetime_str(self):
        return self.get_action_datetime().strftime('%Y-%m-%d %H:%M:%S.%f')

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        d = {
            'DateTime': self.get_action_datetime_str(),
            'Bids': self.get_bids(),
            'Asks': self.get_asks(),
            # 'Exchange': ExchangeType[self.Exchange],
            # 'TradingPhase': TradingPhaseType[self.TradingPhase],
        }

        return to_dict(self,
                       d,
                       ['Size', 'BidCount', 'Exchange', 'TradingPhase',
                        'ActionDay', 'UpdateTime', 'UpdateMillisec'])


# 合约
class InstrumentField(Structure):
    _pack_ = 1
    _fields_ = [
        ('InstrumentName', InstrumentNameType),
        ('Symbol', SymbolType),
        ('InstrumentID', InstrumentIDType),
        ('ExchangeID', ExchangeIDType),
        ('ClientID', IDChar32Type),
        ('AccountID', IDChar32Type),
        ('ExchangeInstID', InstrumentIDType),

        # 合约名称
        ('Type', _InstrumentType),
        # 合约数量乘数
        ('VolumeMultiple', VolumeMultipleType),
        # 最小变动价位
        ('PriceTick', PriceType),
        # 到期日
        ('ExpireDate', DateIntType),
        # 执行价
        ('StrikePrice', PriceType),
        # 期权类型
        ('OptionsType', _PutCall),

        # 产品代码
        ('ProductID', InstrumentIDType),
        # 基础商品代码
        ('UnderlyingInstrID', InstrumentIDType),
        ('InstLifePhase', _InstLifePhaseType),
    ]

    def get_symbol(self):
        return self.Symbol.decode('gbk')

    def get_instrument_id(self):
        return self.InstrumentID.decode('gbk')

    def get_exchange_id(self):
        return self.ExchangeID.decode('gbk')

    def get_product_id(self):
        return self.ProductID.decode('gbk')

    def get_instrument_name(self):
        return self.InstrumentName.decode('gbk')

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        d = {
            'Type': InstrumentType[self.Type],
            'OptionsType': PutCall[self.OptionsType],
            'InstLifePhase': InstLifePhaseType[self.InstLifePhase],
        }

        return to_dict(self,
                       d,
                       ['Type', 'OptionsType', 'InstLifePhase'])


# 账号
class AccountField(Structure):
    _pack_ = 1
    _fields_ = [
        ('ClientID', IDChar32Type),
        ('AccountID', IDChar32Type),
        ('CurrencyID', CurrencyIDType),

        # 上次结算准备金
        ('PreBalance', MoneyType),
        # 当前保证金总额
        ('CurrMargin', MoneyType),
        # 平仓盈亏
        ('CloseProfit', MoneyType),
        # 持仓盈亏
        ('PositionProfit', MoneyType),
        # 期货结算准备金
        ('Balance', MoneyType),
        # 可用资金
        ('Available', MoneyType),
        # 入金金额
        ('Deposit', MoneyType),
        # 出金金额
        ('Withdraw', MoneyType),
        ('WithdrawQuota', MoneyType),

        # 冻结的过户费
        ('FrozenTransferFee', MoneyType),
        # 冻结的印花税
        ('FrozenStampTax', MoneyType),
        # 冻结的手续费
        ('FrozenCommission', MoneyType),
        # 冻结的资金
        ('FrozenCash', MoneyType),

        # 过户费
        ('TransferFee', MoneyType),
        # 印花税
        ('StampTax', MoneyType),
        # 手续费
        ('Commission', MoneyType),
        # 资金差额
        ('CashIn', MoneyType),
    ]

    def get_account_id(self):
        return self.AccountID.decode('gbk')

    def get_currency_id(self):
        return self.CurrencyID.decode('gbk')

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        return to_dict(self, {}, [])


# 发给做市商的询价请求
class QuoteRequestField(Structure):
    _pack_ = 1
    _fields_ = [
        # 唯一符号
        ('Symbol', SymbolType),
        # 合约代码
        ('InstrumentID', InstrumentIDType),
        # 交易所代码
        ('ExchangeID', ExchangeIDType),

        # 交易日
        ('TradingDay', DateIntType),
        # 询价时间
        ('QuoteTime', TimeIntType),
        # 询价编号
        ('QuoteID', OrderIDType),
    ]


# 账号
class SettlementInfoField(Structure):
    _pack_ = 1
    _fields_ = [
        ('Size', SizeType),
        # 交易日
        ('TradingDay', DateIntType),
    ]

    def get_content(self):
        p = addressof(self) + sizeof(SettlementInfoField)
        p_t = cast(p, c_char_p)
        return p_t.value.decode('gbk')

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        d = {
            'Content': self.get_content()
        }

        return to_dict(self,
                       d,
                       ['Size'])


# 投资者
class InvestorField(Structure):
    _pack_ = 1
    _fields_ = [
        # 用户代码
        ('InvestorID', IDChar32Type),

        ('BrokerID', BrokerIDType),

        ('IdentifiedCardType', _IdCardType),

        # 证件号码
        ('IdentifiedCardNo', IdentifiedCardNoType),
        # 投资者名称
        ('InvestorName', PartyNameType),
    ]

    def get_investor_id(self):
        return self.InvestorID.decode('gbk')

    def get_broker_id(self):
        return self.BrokerID.decode('gbk')

    def get_identified_card_no(self):
        return self.IdentifiedCardNo.decode('gbk')

    def get_investor_name(self):
        return self.InvestorName.decode('gbk')

    def __str__(self):
        return json.dumps(self.__dict__, ensure_ascii=False)

    @property
    def __dict__(self):
        d = {
            'IdentifiedCardType': IdCardType[self.IdentifiedCardType],
        }

        return to_dict(self,
                       d,
                       ['IdentifiedCardType'])
