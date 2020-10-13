from app import socketio
from config import *
from .spi import *
from ..socketio_queue import EmitQueue

from flask_socketio import Namespace

import logging

logger = logging.getLogger("SIO_Server")


class XApiNamespace(Namespace):
    md = None
    td = None
    mq = None
    spi = None
    orders_map = {}

    def __init__(self, namespace=None):
        super(XApiNamespace, self).__init__(namespace)
        self.mq = EmitQueue(socketio)
        self.spi = md_spi(self.mq, self.namespace)

    def start(self):
        # 有客户端连接上来时才启动
        # 1. 网页已经连接过一没有关，重开服务端也会导致触发
        # 2. 服务端已经连接成功了，但没有通知
        if self.md is None:
            self.md = config_md()
            if enable_md:
                init_md(self.md)
        if self.td is None:
            self.td = config_td()
            init_td(self.td)

    def stop(self):
        if self.md is not None:
            self.md.disconnect()
            self.md = None
        if self.td is not None:
            self.td.disconnect()
            self.td = None

    def connect(self):
        self.spi.set_api(self.md, self.td)
        self.md.register_spi(self.spi)
        if not self.md.is_connected():
            if enable_md:
                self.md.connect()
        self.td.register_spi(self.spi)
        if not self.td.is_connected():
            if enable_td:
                self.td.connect()

    def on_connect(self):
        # 刷新网页时这里会触发两次，所以需要做到防止重连
        logger.info('on_connect')
        self.start()
        self.connect()
        self.spi.emit_is_connected()

    def on_disconnect(self):
        # 得所有连接都断开才能取消订阅行情
        logger.info('on_disconnect')

    def on_sub_quote(self, data):
        logger.info('on_sub_quote:%s', data)
        args = data['args']
        if not self.md.is_connected():
            return
        self.md.subscribe(args['instruments'], args['exchange'])

    def on_unsub_quote(self, data):
        logger.info('on_unsub_quote:%s', data)
        args = data['args']
        if not self.md.is_connected():
            return
        self.md.unsubscribe(args['instruments'], args['exchange'])

    def on_send_order(self, data):
        logger.info('on_send_order:%s', data)
        args = data['args']
        if not self.td.is_connected():
            return

        # 默认数据，如果输入的参数不够全，使用默认参数
        _d0 = {
            "InstrumentID": "c1909",
            "Type": "Limit",
            "Side": "Buy",
            "Qty": 1,
            "Price": 100.0,
            "OpenClose": "Open",
            "HedgeFlag": "Speculation",
        }

        _input = args
        # 使用输出的参数更新默认字典，防止下面取枚举时出错
        _d0.update(_input)

        # 将原订单中的枚举字符串都换成数字
        _d1 = {
            'Type': OrderType[_d0["Type"]],
            'Side': OrderSide[_d0["Side"]],
            'OpenClose': OpenCloseType[_d0["OpenClose"]],
            'HedgeFlag': HedgeFlagType[_d0["HedgeFlag"]],
        }
        _d0.update(_d1)
        local_id = _d0['LocalID']

        order_id = self.td.send_order(_d0)

        # 也可以不设置，但这样远程就无法关联了
        if len(local_id) > 0:
            self.orders_map[order_id] = local_id

    def on_cancel_order(self, data):
        logger.info('on_cancel_order:%s', data)
        args = data['args']
        if not self.td.is_connected():
            return
        self.td.cancel_order(args["ID"])

    def on_query_account(self, data):
        logger.info('on_query_account')
        query = ReqQueryField()
        self.td.req_query(QueryType.ReqQryTradingAccount, query)

    def on_query_positions(self, data):
        logger.info('on_query_positions')
        query = ReqQueryField()
        self.td.req_query(QueryType.ReqQryInvestorPosition, query)

    def on_query_instrument(self, data):
        logger.info('on_query_instrument')
        args = data['args']

        query = ReqQueryField()
        try:
            exchange_id = args['ExchangeID']
            query.ExchangeID = exchange_id.encode()
        except:
            pass
        self.td.req_query(QueryType.ReqQryInstrument, query)

    def on_query_order(self, data):
        logger.info('on_query_order')
        args = data['args']

        query = ReqQueryField()
        self.td.req_query(QueryType.ReqQryOrder, query)

    def on_query_settlement_info(self, data):
        logger.info('on_query_settlement_info:%s', data)
        args = data['args']

        query = ReqQueryField()
        query.DateStart = args["TradingDay"]
        self.td.req_query(QueryType.ReqQrySettlementInfo, query)

    def on_query_history_data(self, data):
        logger.info('on_query_history_data:%s', data)
        args = data['args']

        self.spi.emit_rsp_qry_history_data(args)
