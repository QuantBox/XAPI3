from xapi import *
from xapi.XStruct import *
import time

from config import *

import logging

logger = logging.getLogger("SIO_Server")




class md_spi(XSpi):
    """
    继承XSpi可以实现回调方法的各种功能
    """
    md = None
    td = None
    coll = None

    def __init__(self, mq, namespace):
        self.mq = mq
        self.namespace = namespace
        self.coll = connnect_collection()

    def set_api(self, md, td):
        self.md = md
        self.td = td
        # 启动行情连接
        self.mq.on_connect()

    def OnLog(self, api, pLog):
        # 由于TDX接口的日志太多，屏蔽一下，对于其它软件可能需要打开
        pass

    def OnConnectionStatus(self, api, status, pUserLogin, size1):
        super(md_spi, self).OnConnectionStatus(api, status, pUserLogin, size1)

        self.mq.emit('connection_status',
                     {
                         "data": {
                             "data": pUserLogin.__dict__,
                             "status": ConnectionStatus[status],
                             "api": str(api),
                             "size": size1,
                         }
                     },
                     namespace=self.namespace)
        self.emit_is_connected()

    def OnRtnDepthMarketData(self, api, ptr1, size1):
        # 穿越速度测试，为了取时间，导致不精确，实际时间要长一些
        obj = cast(ptr1, POINTER(DepthMarketDataNField)).contents
        # logger.error("行情穿越速度测试：%s", obj.__dict__)
        # logger.debug(obj.__dict__)

        self.mq.emit('rtn_market_data',
                     {
                         "data": obj.__dict__
                     },
                     namespace=self.namespace)

    def OnRtnOrder(self, api, pOrder):
        logger.debug(pOrder.__dict__)

        # 需要根据委托回报更新持仓信息中的冻结字段

        self.mq.emit('rtn_order',
                     {
                         "data": pOrder.__dict__,
                     },
                     namespace=self.namespace)

    def OnRtnTrade(self, api, pTrade):
        self.mq.emit('rtn_trade',
                     {
                         "data": pTrade.__dict__
                     },
                     namespace=self.namespace)

    def OnRspQryTradingAccount(self, api, pAccount, size1, bIsLast):
        logger.debug(pAccount.__dict__)
        self.mq.emit('rsp_qry_trading_account',
                     {
                         "data": pAccount.__dict__,
                         "size": size1,
                         "is_last": bIsLast,
                     },
                     namespace=self.namespace)

    def OnRspQryInvestorPosition(self, api, pPosition, size1, bIsLast):
        logger.debug(pPosition.__dict__)

        # 需要根据持仓更新持仓

        self.mq.emit('rsp_qry_investor_position',
                     {
                         "data": pPosition.__dict__,
                         "size": size1,
                         "is_last": bIsLast,
                     },
                     namespace=self.namespace)

    def OnRspQryInstrument(self, api, pInstrument, size1, bIsLast):
        logger.debug(pInstrument.__dict__)
        self.mq.emit('rsp_qry_instrument',
                     {
                         "data": pInstrument.__dict__,
                         "size": size1,
                         "is_last": bIsLast,
                     },
                     namespace=self.namespace)

    def OnRspQryOrder(self, api, pOrder, size1, bIsLast):
        logger.debug(pOrder.__dict__)
        self.mq.emit('rsp_qry_order',
                     {
                         "data": pOrder.__dict__,
                         "size": size1,
                         "is_last": bIsLast,
                     },
                     namespace=self.namespace)

    def OnRspQrySettlementInfo(self, api, ptr1, size1, bIsLast):
        # 可能没有月度结算单
        if size1 == 0:
            return

        obj = cast(ptr1, POINTER(SettlementInfoField)).contents
        logger.info(obj.__dict__)
        self.mq.emit('rsp_qry_settlement_info',
                     {
                         "data": obj.__dict__,
                         "size": size1,
                         "is_last": bIsLast,
                     },
                     namespace=self.namespace)

    def emit_is_connected(self):
        # 通知连接状态，只通知要连的api
        d = {}
        if enable_md:
            d[self.md.name] = self.md.is_connected()
        if enable_td:
            d[self.td.name] = self.td.is_connected()

        args = {
            'data': d
        }
        logger.info('emit_is_connected:%s', args)
        self.mq.emit('is_connected', args,
                     namespace=self.namespace)

    def emit_rsp_qry_history_data_csv(self, _args):
        # TODO：测试使用，不接数据库
        # 推送历史行情
        datas = list()
        import pandas as pd
        df = pd.read_csv('399673.XSHE.csv', index_col=0, parse_dates=True)
        for item in df.iterrows():
            row = item[1].tolist()
            row.append(str(item[0]))
            datas.append(row)

        args = {
            'data': {
                "datas": datas,
                'args': _args,
            }
        }
        logger.info('emit_rsp_qry_history_data:%s', args)
        self.mq.emit('rsp_qry_history_data', args,
                     namespace=self.namespace)

    def emit_rsp_qry_history_data(self, _args):
        from .history_mongo import *
        # 推送历史行情
        symbol = _args['Symbol']
        datas = kline_download_list(self.coll, symbol)

        args = {
            'data': {
                "datas": datas,
                'args': _args,
            }
        }
        logger.info('emit_rsp_qry_history_data:%s', args)
        self.mq.emit('rsp_qry_history_data', args,
                     namespace=self.namespace)
