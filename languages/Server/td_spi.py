from xapi import *
from xapi.XStruct import *

from config import *

import logging

logger = logging.getLogger("Downloader")

import pandas as pd


class td_spi(XSpi):
    """
    继承XSpi可以实现回调方法的各种功能

    计划自动下载并存文件
    """
    td = None
    OnRspQryInvestorPosition_dict = {}
    OnRspQryInvestorPosition_cnt = 0
    OnRspQryOrder_dict = {}
    OnRspQryOrder_cnt = 0
    OnRspQryTrade_dict = {}
    OnRspQryTrade_cnt = 0
    TradingDay = 0
    AccountID = None

    def __init__(self):
        pass

    def set_api(self, td):
        self.td = td
        # 记录账号
        self.AccountID = str(td.UserInfo.UserID, encoding="utf-8")

    def OnLog(self, api, pLog):
        # 由于TDX接口的日志太多，屏蔽一下，对于其它软件可能需要打开
        pass

    def OnConnectionStatus(self, api, status, pUserLogin, size1):
        super(td_spi, self).OnConnectionStatus(api, status, pUserLogin, size1)
        # 记录当天的交易日
        if status == ConnectionStatus.Logined:
            self.TradingDay = pUserLogin.TradingDay

    def OnRtnOrder(self, api, pOrder):
        pass

    def OnRtnTrade(self, api, pTrade):
        pass

    def OnRspQryOrder(self, api, pOrder, size1, bIsLast):
        if size1 == 0:
            return
        logger.info(pOrder.__dict__)
        self.OnRspQryOrder_dict[self.OnRspQryOrder_cnt] = pOrder.__dict__
        self.OnRspQryOrder_cnt += 1
        if bIsLast:
            df = pd.DataFrame.from_dict(self.OnRspQryOrder_dict, orient='index')
            filename = f'{self.AccountID}_{self.TradingDay}_OnRspQryOrder.csv'
            df.to_csv(filename)

    def OnRspQryTrade(self, api, pTrade, size1, bIsLast):
        if size1 == 0:
            return
        logger.info(pTrade.__dict__)
        self.OnRspQryTrade_dict[self.OnRspQryTrade_cnt] = pTrade.__dict__
        self.OnRspQryTrade_cnt += 1
        if bIsLast:
            df = pd.DataFrame.from_dict(self.OnRspQryTrade_dict, orient='index')
            filename = f'{self.AccountID}_{self.TradingDay}_OnRspQryTrade.csv'
            df.to_csv(filename)

    def OnRspQryTradingAccount(self, api, pAccount, size1, bIsLast):
        logger.info(pAccount.__dict__)

    def OnRspQryInvestorPosition(self, api, pPosition, size1, bIsLast):
        if size1 == 0:
            return
        logger.info(pPosition.__dict__)
        self.OnRspQryInvestorPosition_dict[self.OnRspQryInvestorPosition_cnt] = pPosition.__dict__
        self.OnRspQryInvestorPosition_cnt += 1
        if bIsLast:
            df = pd.DataFrame.from_dict(self.OnRspQryInvestorPosition_dict, orient='index')
            filename = f'{self.AccountID}_{self.TradingDay}_OnRspQryInvestorPosition.csv'
            df.to_csv(filename)

    def OnRspQrySettlementInfo(self, api, ptr1, size1, bIsLast):
        # 可能没有月度结算单
        if size1 == 0:
            return

        obj = cast(ptr1, POINTER(SettlementInfoField)).contents
        # logger.info(obj.__dict__)
        content = obj.get_content()

        import re

        r = re.compile(r"日期 Date：(\d{8,8})")
        date = r.findall(content)[0]

        # 存文件
        with open(f'{self.AccountID}_{date}_OnRspQrySettlementInfo.txt', 'w') as f:
            f.write(content)

        print(content)
