#!/usr/bin/env python
# -*- coding: utf-8 -*-

from .XQueueEnum import *
from .XSpi import *
import threading
import queue
import time

import logging

logger = logging.getLogger("XApi")

# function
fnOnRespone = WINFUNCTYPE(c_void_p, c_char, c_void_p, c_void_p, c_double, c_double, c_void_p, c_int, c_void_p, c_int,
                          c_void_p, c_int)


# fnOnRespone = CFUNCTYPE(c_void_p, c_char, c_void_p, c_void_p, c_double, c_double, c_void_p, c_int, c_void_p, c_int,
#                           c_void_p, c_int)


class XApi(object):
    def __init__(self, xapi_path, name=""):
        self._lib = None
        self._fun = None
        self._api = None
        self._spi = None
        self._log = None
        self._status = ConnectionStatus.Uninitialized
        self.name = name
        self.subscribed = {}
        self.orders_map = {}
        self.q_check = queue.Queue()
        self.t = None

        self.ServerInfo = ServerInfoField()
        self.UserInfo = UserInfoField()

        # 加载XAPI_CPP_x86.dll，使用它来加载其它的API的C封装
        self._cdll_api = CDLL(xapi_path)
        # 记录回调函数
        self._response = fnOnRespone(self._on_response)
        # 记录回调dispatcher,将一个回调函数的请求分解为多个函数
        self._response_type_dict = {
            ResponseType.OnConnectionStatus: self._OnConnectionStatus,
            ResponseType.OnRtnError: self._OnRtnError,
            ResponseType.OnLog: self._OnLog,
            ResponseType.OnRspQryInstrument: self._OnRspQryInstrument,
            ResponseType.OnRspQryOrder: self._OnRspQryOrder,
            ResponseType.OnRspQryTrade: self._OnRspQryTrade,
            ResponseType.OnRspQryInvestorPosition: self._OnRspQryInvestorPosition,
            ResponseType.OnRspQryInvestor: self._OnRspQryInvestor,
            ResponseType.OnRtnOrder: self._OnRtnOrder,
            ResponseType.OnRtnTrade: self._OnRtnTrade,
            ResponseType.OnRtnDepthMarketData: self._OnRtnDepthMarketData,
            ResponseType.OnRspQryTradingAccount: self._OnRspQryTradingAccount,
            ResponseType.OnRtnInstrumentStatus: self._OnRtnInstrumentStatus,
            ResponseType.OnRspQrySettlementInfo: self._OnRspQrySettlementInfo,
        }

    def __str__(self):
        return self.name or "XApi"

    def init(self, lib_path):
        func = self._cdll_api.X_LoadLib
        func.restype = c_void_p
        func.argtypes = [c_char_p]
        self._lib = func(lib_path)
        if self._lib is None:
            return False
        func = self._cdll_api.X_GetFunction
        func.restype = c_void_p
        func.argtypes = [c_void_p, c_char_p]
        self._fun = func(self._lib, b"XRequest")
        return True

    def get_last_error(self):
        func = self._cdll_api.X_GetLastError
        func.restype = c_char_p
        func.argtypes = []
        ptr = func()
        return ptr.decode('GBK')

    def get_api_type(self):
        func = self._cdll_api.X_GetApiTypes
        func.restype = c_char
        func.argtypes = [c_void_p]
        ret = func(self._fun)
        return ret

    def get_api_version(self):
        func = self._cdll_api.X_GetApiVersion
        func.restype = c_char_p
        func.argtypes = [c_void_p]
        ptr = func(self._fun)
        return ptr.decode('GBK')

    def get_api_name(self):
        func = self._cdll_api.X_GetApiName
        func.restype = c_char_p
        func.argtypes = [c_void_p]
        ptr = func(self._fun)
        return ptr.decode('GBK')

    def register_spi(self, spi):
        self._spi = spi

    def connect(self):
        if self.is_connected():
            return
        if self.is_connecting():
            return

        self._status = ConnectionStatus.Connecting

        func = self._cdll_api.X_Create
        func.restype = c_void_p
        func.argtypes = [c_void_p]
        self._api = func(self._fun)

        func = self._cdll_api.X_Register
        func.restype = None
        func.argtypes = [c_void_p, c_void_p, c_void_p, c_void_p]
        func(self._fun, self._api, self._response, id(self))

        func = self._cdll_api.X_Connect
        func.restype = None
        func.argtypes = [c_void_p, c_void_p, c_char_p, POINTER(ServerInfoField), POINTER(UserInfoField), c_int]
        func(self._fun, self._api, b'tmp', byref(self.ServerInfo), byref(self.UserInfo), 1)

        if self.t is None:
            self.t = threading.Thread(target=self._t_check)
            self.t.daemon = True
            self.t.start()

    def _t_check(self):
        while True:
            try:
                q = self.q_check.get(timeout=2)
                if q is None:
                    break
            except:
                pass

            if self.is_connected():
                time.sleep(30)
                continue
            if self.is_connecting():
                # 正在连接中，会收到回报，致时间有点久
                time.sleep(30)
                if self.is_connected():
                    continue
                # N秒还没连上，销毁重连
                self.disconnect(exit=False)
                time.sleep(2)
                continue
            logger.info('%s:重连中...', self.name)
            self.connect()
            time.sleep(10)

    def disconnect(self, exit=True):
        # 退出重连
        if exit:
            self.q_check.put(None)

        func = self._cdll_api.X_Disconnect
        func.restype = None
        func.argtypes = [c_void_p, c_void_p]
        func(self._fun, self._api)

        if exit:
            func = self._cdll_api.X_FreeLib
            func.restype = None
            func.argtypes = [c_void_p]
            func(self._lib)

        self._api = None

    def is_connected(self):
        return self._status == ConnectionStatus.Done

    def is_connecting(self):
        return self._status not in [ConnectionStatus.Disconnected,
                                    ConnectionStatus.Done,
                                    ConnectionStatus.Uninitialized]

    def req_query(self, query_type, query):
        func = self._cdll_api.X_ReqQuery
        func.restype = None
        func.argtypes = [c_void_p, c_void_p, c_char, POINTER(ReqQueryField)]
        # c_char(query_type) python3中可用
        # c_char(chr(query_type)) python2中可用
        # query_type是int
        func(self._fun, self._api, c_char(b'%c' % query_type), byref(query))

    def send_order(self, order):
        _cnt = 1
        _orders = (OrderField * _cnt)()
        _orderids = (OrderIDTypeField * _cnt)()
        local_ids = []
        for i, o in enumerate([order]):
            _orders[i].from_dict(o)
            _orderids[i].OrderIDType = b''
            # 记下转入的数据，后要映射用
            local_ids.append(_orders[i].LocalID)

        func = self._cdll_api.X_SendOrder
        func.restype = c_char_p
        func.argtypes = [c_void_p, c_void_p, POINTER(OrderField), c_int, POINTER(OrderIDTypeField)]
        p1 = POINTER(OrderField)()
        p1.contents = _orders
        p2 = POINTER(OrderIDTypeField)()
        p2.contents = _orderids
        p3 = func(self._fun, self._api, p1, _cnt, p2)

        for i in local_ids:
            self.orders_map[p3] = i

        return p3.decode('gbk')

    def cancel_order(self, id):
        p_in = OrderIDTypeField()
        p_out = OrderIDTypeField()
        p_out.OrderIDType = b''
        p_in.OrderIDType = id.encode('gbk')

        func = self._cdll_api.X_CancelOrder
        func.restype = c_char_p
        func.argtypes = [c_void_p, c_void_p, POINTER(OrderIDTypeField), c_int, POINTER(OrderIDTypeField)]
        p1 = POINTER(OrderIDTypeField)()
        p1.contents = p_in
        p2 = POINTER(OrderIDTypeField)()
        p2.contents = p_out
        p3 = func(self._fun, self._api, p1, 1, p2)
        return p3.decode('gbk')

    def subscribe(self, instruments, exchange):
        """
        订阅行情
        :param instruments: 字符串列表
        :param exchange: 字符串
        :return:
        """
        func = self._cdll_api.X_Subscribe
        func.restype = None
        func.argtypes = [c_void_p, c_void_p, c_char_p, c_char_p]
        instruments_str = ';'.join(instruments)
        func(self._fun, self._api, c_char_p(instruments_str.encode('gbk')), c_char_p(exchange.encode('gbk')))
        self.subscribed.update({i: exchange for i in instruments})

    def unsubscribe(self, instruments, exchange):
        func = self._cdll_api.X_Unsubscribe
        func.restype = None
        func.argtypes = [c_void_p, c_void_p, c_char_p, c_char_p]
        instruments_str = ';'.join(instruments)
        func(self._fun, self._api, c_char_p(instruments_str.encode('gbk')), c_char_p(exchange.encode('gbk')))
        for k in instruments:
            self.subscribed.pop(k)

    def _on_response(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        # 解决在回调函数中断点不生效的问题，把这行代码放在要断的地方的前面
        # http://stackoverflow.com/questions/34299082/not-working-python-breakpoints-in-c-thread-in-pycharm-or-eclipsepydev
        # import pydevd
        # pydevd.settrace(suspend=True, trace_only_current_thread=True)
        self._response_type_dict.get(ord(response_type), self._on_default)(response_type, p_api1, p_api2, double1,
                                                                           double2,
                                                                           ptr1, size1, ptr2, size2, ptr3, size3)

    def _on_default(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        logger.warning("功能还没实现:%d" % ord(response_type))

    def _OnConnectionStatus(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3,
                            size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(RspUserLoginField)).contents
        else:
            obj = RspUserLoginField()
        self._status = int(double1)
        self._spi.OnConnectionStatus(self, self._status, obj, size1)
        if self.is_connected():
            _subscribed = self.subscribed.copy()
            for k, v in _subscribed.items():
                self.subscribe(k, v)

    def _OnRtnInstrumentStatus(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3,
                               size3):
        if size1 <= 0:
            return
        obj = cast(ptr1, POINTER(InstrumentStatusField)).contents
        self._spi.OnRtnInstrumentStatus(self, obj)

    def _OnRtnError(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 <= 0:
            return
        obj = cast(ptr1, POINTER(ErrorField)).contents
        self._spi.OnRtnError(self, obj)

    def _OnLog(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 <= 0:
            return
        obj = cast(ptr1, POINTER(LogField)).contents
        self._spi.OnLog(self, obj)

    def _OnRspQryOrder(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(OrderField)).contents
        else:
            obj = OrderField()
        self._spi.OnRspQryOrder(self, obj, size1, bool(double1))

    def _OnRspQryTrade(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(TradeField)).contents
        else:
            obj = TradeField()
        self._spi.OnRspQryTrade(self, obj, size1, bool(double1))

    def _OnRspQryInvestorPosition(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3,
                                  size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(PositionField)).contents
        else:
            obj = PositionField()
        self._spi.OnRspQryInvestorPosition(self, obj, size1, bool(double1))

    def _OnRspQrySettlementInfo(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3,
                                size3):
        # 同行情一样，也是一个大内存块
        self._spi.OnRspQrySettlementInfo(self, ptr1, size1, bool(double1))

    def _OnRspQryInstrument(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3,
                            size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(InstrumentField)).contents
        else:
            obj = InstrumentField()
        self._spi.OnRspQryInstrument(self, obj, size1, bool(double1))

    def _OnRspQryInvestor(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(InvestorField)).contents
        else:
            obj = InvestorField()
        self._spi.OnRspQryInvestor(self, obj, size1, bool(double1))

    def _OnRtnOrder(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 <= 0:
            return
        obj = cast(ptr1, POINTER(OrderField)).contents
        # 将用户指定的id进行映射
        _uuid = self.orders_map.get(obj.LocalID, None)
        if _uuid is not None:
            obj.LocalID = _uuid
        self._spi.OnRtnOrder(self, obj)

    def _OnRtnTrade(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 <= 0:
            return
        obj = cast(ptr1, POINTER(TradeField)).contents
        # _uuid = self.orders_map[obj.LocaID]
        # obj.LocalID = _uuid
        self._spi.OnRtnTrade(self, obj)

    def _OnRtnDepthMarketData(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3,
                              size3):
        # 由于这个结构体比较特殊，内存区域需要解析，所以直接传回去
        self._spi.OnRtnDepthMarketData(self, ptr1, size1)

    def _OnRspQryTradingAccount(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3,
                                size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(AccountField)).contents
        else:
            obj = AccountField()
        self._spi.OnRspQryTradingAccount(self, obj, size1, bool(double1))
