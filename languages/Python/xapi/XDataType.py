#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ctypes import *

"""
SymbolType是一个合约唯一符号类型，通过某种方式算出，不同API的同一合约完全相同
"""
SymbolType = c_char*64

"""
InstrumentIDType是一个合约代码类型，API对应的合约代码
"""
InstrumentIDType = c_char*31

"""
ExchangeIDType是一个交易所代码类型，API对应的交易所代码，可能各不相同
"""
ExchangeIDType = c_char*9

"""
DateTimeType是一个日期时间类型
"""
DateTimeType = c_char*32
"""
DateTimeLongType是一个日期时间类型,yyyyMMddHHmmss
"""
DateTimeLongType = c_int64

"""
DateType是一个日期类型
"""
DateType = c_char*9
"""
DateIntType是一个日期类型,yyyyMMdd
"""
DateIntType = c_int32

"""
TimeType是一个时间类型
"""
TimeType = c_char*9
"""
TimeIntType是一个时间类型,HHmmss
"""
TimeIntType = c_int32


"""
Int32Type
"""
Int32Type = c_int32

"""
Char256Type
"""
Char256Type = c_char*256
"""
Char128Type
"""
Char128Type = c_char*128
"""
Char64Type
"""
Char64Type = c_char*64
"""
Char32Type
"""
Char32Type = c_char*32
"""
BooleanType
"""
BooleanType = c_bool

"""
PriceType是一个价格类型
"""
PriceType = c_double

"""
VolumeType是一个交易量类型
"""
VolumeType = c_int32
"""
LargeVolumeType是一个大额数量类型
"""
LargeVolumeType = c_double

"""
QtyType是一个交易量类型
"""
QtyType = c_double
"""
MoneyType是一个资金类型
"""
MoneyType = c_double
"""
CurrencyIDType是一个币种类型
"""
CurrencyIDType = c_char*4

"""
IDChar32Type
"""
IDChar32Type = c_char*32

"""
PasswordType是一个密码类型
"""
PasswordType = c_char*41




"""
 PartyNameType是一个参与人名称类型
"""
PartyNameType = c_char*81

"""
 IdentifiedCardNoType是一个证件号码类型
"""
IdentifiedCardNoType = c_char*51

"""
ProductInfoType是一个产品信息类型
"""
ProductInfoType = c_char*11

"""
BrokerIDType是一个经纪公司代码类型
"""
BrokerIDType = c_char*11

"""
AuthCodeType是一个客户端认证码类型
"""
AuthCodeType = c_char*17

"""
AppIDType是一个App代码类型
"""
AppIDType = c_char*33

"""
AddressType是一个地址类型
"""
AddressType = c_char*512

"""
VolumeMultipleType是一个合约数量乘数类型
"""
VolumeMultipleType = c_int32

"""
InstrumentNameType是一个合约名称类型
"""
InstrumentNameType = c_char*64

"""
ContentType是一个消息正文类型
"""
ContentType = c_char*501

"""
SessionIDType是一个会话编号类型
"""
SessionIDType = c_char*32

"""
OrderIDType是一个订单唯一编号类型
"""
OrderIDType = c_char*64

"""
TradeIDType是一个成交编号类型
"""
TradeIDType = c_char*64

"""
PositionIDType是一个持仓编号类型
"""
PositionIDType = c_char*64

"""
BarSizeType是一个BarSize类型
"""
BarSizeType = c_long

SizeType = c_int
