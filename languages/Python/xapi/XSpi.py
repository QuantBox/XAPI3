#!/usr/bin/env python
# -*- coding: utf-8 -*-

# import pydevd
from .XStruct import *

import logging

logger = logging.getLogger("XApi")


class XSpi(object):

    def OnConnectionStatus(self, api, status, pUserLogin, size1):
        logger.info('OnConnectionStatus={0}'.format(ConnectionStatus[status]))
        if size1 > 0:
            logger.info(pUserLogin)

    def OnRtnError(self, api, pError):
        logger.error(pError)

    def OnRtnInstrumentStatus(self, api, pInstrumentStatus):
        logger.debug(pInstrumentStatus)

    def OnLog(self, api, pLog):
        logger.info(pLog)

    def OnRspQryInstrument(self, api, pInstrument, size1, bIsLast):
        if size1 > 0:
            logger.info(pInstrument)

    def OnRspQryTradingAccount(self, api, pAccount, size1, bIsLast):
        if size1 > 0:
            logger.info(pAccount)

    def OnRspQryInvestorPosition(self, api, pPosition, size1, bIsLast):
        if size1 > 0:
            logger.info(pPosition)

    def OnRspQryOrder(self, api, pOrder, size1, bIsLast):
        if size1 > 0:
            logger.info(pOrder)

    def OnRspQryTrade(self, api, pTrade, size1, bIsLast):
        if size1 > 0:
            logger.info(pTrade)

    def OnRspQryInvestor(self, api, pInvestor, size1, bIsLast):
        if size1 > 0:
            logger.info(pInvestor)

    def OnRtnOrder(self, api, pOrder):
        logger.info(pOrder)

    def OnRtnTrade(self, api, pTrade):
        logger.info(pTrade)

    def OnRtnDepthMarketData(self, api, ptr1, size1):
        # 这是从交易接口拿到的行情
        obj = cast(ptr1, POINTER(DepthMarketDataNField)).contents
        logger.info(obj)

        # 打印N档行情
        for i in range(obj.get_ticks_count()):
            p = ptr1 + sizeof(DepthMarketDataNField) + sizeof(DepthField) * i
            d = cast(p, POINTER(DepthField)).contents
            logger.info(d)

    def OnRspQrySettlementInfo(self, api, ptr1, size1, bIsLast):
        if size1 > 0:
            obj = cast(ptr1, POINTER(SettlementInfoField)).contents
            logger.info(obj)
