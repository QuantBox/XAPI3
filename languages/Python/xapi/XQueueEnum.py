#!/usr/bin/env python
# -*- coding: utf-8 -*-

from .XEnum import *

RequestType = enum_k(
    0,
    'GetApiTypes',
    'GetApiVersion',
    'GetApiName',
    'Create',
    'Release',
    'Register',
    'Config',
    'Connect',
    'Disconnect',
    'Clear',
    'Process',
    'Subscribe',
    'Unsubscribe',
    'SubscribeQuote',
    'UnsubscribeQuote',
    'ReqOrderInsert',
    'ReqQuoteInsert',
    'ReqOrderAction',
    'ReqQuoteAction',
)

"""
查询
"""
QueryType = enum_k(
    32,
    'ReqQryInstrument',
    'ReqQryTradingAccount',
    'ReqQryInvestorPosition',
    'ReqQryOrder',
    'ReqQryTrade',
    'ReqQryQuote',
    'ReqQryInstrumentCommissionRate',
    'ReqQryInstrumentMarginRate',
    'ReqQrySettlementInfo',
    'ReqQryInvestor',
    'ReqQryHistoricalTicks',
    'ReqQryHistoricalBars',
)

"""
响应
"""
ResponseType = enum_k(
    64,
    'OnConnectionStatus',
    'OnRtnError',
    'OnLog',

    'OnRtnDepthMarketData',
    'OnRspQryInstrument',
    'OnRspQryTradingAccount',
    'OnRspQryInvestorPosition',
    'OnRspQrySettlementInfo',

    'OnRspQryOrder',
    'OnRspQryTrade',
    'OnRspQryQuote',

    'OnRtnOrder',
    'OnRtnTrade',
    'OnRtnQuote',

    'OnRtnQuoteRequest',

    'OnRspQryHistoricalTicks',
    'OnRspQryHistoricalBars',
    'OnRspQryInvestor',
    'OnRtnInstrumentStatus',

    'OnFilterSubscribe',
)

