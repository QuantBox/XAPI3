#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
目标持仓调整功能
证券和期货兼容
"""
import shelve
import copy
import os
from time import sleep

from xapi import *
from xapi.XStruct import *
from xapi.utils import *
from xapi.symbol import *


class MyXSpi(XSpi):
    """
    继承XSpi可以实现回调方法的各种功能
    """

    def __init__(self, root_dir, portfolios, td, md, is_stock):
        # 测试时从本地读取数据
        self.test_query_from_local = False
        self.test_send_order = False
        self.times_tick_size = 2

        self.symbols = None
        self.target_position = None
        self.target_orders = None
        self.wait_lock = False
        self.done_cnt = 0
        # 股票打印行情，期货不主动打印行情
        self.print_quote = is_stock

        self.is_stock = is_stock
        # 根目录
        self.root_dir = root_dir
        self.target_position_path = os.path.join(self.root_dir, 'target_position.csv')
        self.target_orders_path = os.path.join(self.root_dir, 'target_orders.csv')
        # 子投资组合的仓位，合并成目标持仓时会做对冲
        self.portfolios_path = [os.path.join(self.root_dir, p) for p in portfolios]
        # 增量
        self.incremental_position_path = os.path.join(self.root_dir, 'incremental_position.csv')

        # 合约
        columns, formats = get_fields_columns_formats(InstrumentField)
        self.instrument_dtype = np.dtype({'names': columns, 'formats': formats})
        self.instrument_dict = {}
        self.instrument_df = pd.DataFrame(columns=columns)
        self.instrument_dict_path = os.path.join(self.root_dir, 'instrument_dict.db')

        # 持仓
        columns, formats = get_fields_columns_formats(PositionField)
        self.position_dtype = np.dtype({'names': columns, 'formats': formats})
        self.position_dict = {}
        self.position_df = pd.DataFrame(columns=columns)
        self.position_df_path = os.path.join(self.root_dir, 'position_df.db')

        # 报单
        columns, formats = get_fields_columns_formats(OrderField)
        self.order_dtype = np.dtype({'names': columns, 'formats': formats})
        self.order_dict = {}
        self.order_df = pd.DataFrame(columns=columns)

        # 行情
        self.marketdata_dict_symbol = {}
        self.marketdata_dict_instrument = {}
        # 一般不保存，要保存也是为了测试使用
        self.marketdata_dict_path = os.path.join(self.root_dir, 'marketdata_dict.db')

        # 行情连接
        self.md = md
        # 交易连接
        self.td = td

        # 开启就加载合约信息
        self.instrument_dict3 = {}
        try:
            f = shelve.open(self.instrument_dict_path, 'r')
            self.instrument_dict = f['instrument_dict']
            f.close()
            self.instruments_1to3()
        except:
            print('本地没有合约列表，请主动查询一次，否则TickSize是错的')

    def OnLog(self, pLog):
        # 由于TDX接口的日志太多，屏蔽一下，对于其它软件可能需要打开
        pass

    def OnConnectionStatus(self, status, pUserLogin, size1):
        super(MyXSpi, self).OnConnectionStatus(status, pUserLogin, size1)
        if ConnectionStatus[status] == 'Done':
            # 行情和交易都得收到，所以至少要加到2
            self.done_cnt += 1
        pass

    def OnRspQryInstrument(self, pInstrument, size1, bIsLast):
        if size1 <= 0:
            return

        # 一定要用copy,不然最后一个会覆盖前面的
        self.instrument_dict[pInstrument.get_symbol()] = copy.copy(pInstrument)
        if not bIsLast:
            return

        # 在本地存个盘
        f = shelve.open(self.instrument_dict_path)
        f['instrument_dict'] = self.instrument_dict
        f.close()
        # 只是打印一下，不用于其它处理
        self.instrument_df = ctypes_dict_2_dataframe(self.instrument_dict, self.instrument_dtype)
        self.instrument_df = decode_dataframe(self.instrument_df)
        self.wait_lock = False
        print(self.instrument_df[['Symbol', 'InstrumentName', 'ExchangeID', 'VolumeMultiple', 'PriceTick']])
        # 打印后再显示
        self.instruments_1to3()

    def instruments_1to3(self):
        # 将原始的数据由1份存成3份
        self.instrument_dict3 = {}
        for i in self.instrument_dict:
            tmp = self.instrument_dict[i]
            self.instrument_dict3[tmp.get_symbol()] = tmp
            self.instrument_dict3[tmp.get_instrument_id()] = tmp
            self.instrument_dict3[tmp.get_product_id()] = tmp

    def OnRspQryTradingAccount(self, pAccount, size1, bIsLast):
        if size1 <= 0:
            return
        self.wait_lock = False
        print(pAccount)

    def OnRspQryInvestorPosition(self, pPosition, size1, bIsLast):
        if size1 <= 0:
            return

        # 一定要用copy,不然最后一个会覆盖前面的
        self.position_dict[pPosition.get_id()] = copy.copy(pPosition)
        if not bIsLast:
            return

        self.position_df = ctypes_dict_2_dataframe(self.position_dict, self.position_dtype)
        self.position_df = decode_dataframe(self.position_df)
        # 是否要持仓为0的过滤掉？
        self.position_df.sort_values(by=['Symbol', 'HedgeFlag', 'Side'], ascending=True, inplace=True)
        self.wait_lock = False

        print("Side:多1空-1\tPosition=TodayPosition+HistoryPosition")
        df = self.position_df.copy()
        df['Side'] = 1 - 2 * df['Side']
        print(df[['Symbol', 'HedgeFlag', 'Side', 'Position', 'TodayPosition', 'InstrumentName']])

        # 每次查询完同时保存，做个记录
        f = shelve.open(self.position_df_path)
        f['position_df'] = self.position_df
        f.close()
        self.update_symbols()

    def OnRtnOrder(self, pOrder):
        self.order_dict[pOrder.get_id()] = copy.copy(pOrder)
        print(pOrder)

    def OnRtnDepthMarketData(self, ptr1, size1):
        obj = cast(ptr1, POINTER(DepthMarketDataNField)).contents
        # 打印行情，一般情况下都是关闭，因为内容太多了
        if self.print_quote:
            print(obj)

        # 由于对象中不带指针后的区域，所以涨跌停修正只能在这做
        # if obj.UpperLimitPrice == 0:
        #     ask_count = obj.get_ask_count()
        #     if ask_count > 0:
        #         ask = obj.get_ask(ptr1, ask_count - 1)
        #         obj.UpperLimitPrice = ask.Price
        # if obj.LowerLimitPrice == 0:
        #     bid_count = obj.get_bid_count()
        #     if bid_count > 0:
        #         bid = obj.get_bid(ptr1, bid_count - 1)
        #         obj.LowerLimitPrice = bid.Price

        cp_obj = copy.copy(obj)
        # 由于是期货，直接使用合约名就可以定位
        self.marketdata_dict_instrument[obj.get_instrument_id()] = cp_obj
        # 股票有指数行情，优先按符号查询更靠谱
        self.marketdata_dict_symbol[obj.get_symbol()] = cp_obj

    def disconnect(self):
        """
        断开连接
        :return:
        """
        self.td.disconnect()

        if self.td is self.md:
            return
        self.md.disconnect()
        return

    def connect(self):
        self.td.register_spi(self)
        self.td.connect()

        # 是否为同一对象
        if self.td is self.md:
            return
        self.md.register_spi(self)
        self.md.connect()
        return

    def reconnect(self):
        """
        重连，可能运行过程中出现断线了，这里可以主动重连一下
        :return:
        """
        self.disconnect()
        self.connect()
        return

    def load_positions(self):
        # 表格中东西很多，改成只取自己想要的部分？这样在表格合并时就不会出现大量的_x,_y
        df = pd.read_csv(self.target_position_path, dtype={'Symbol': str, 'InstrumentID': str}, quotechar='\'',
                         encoding='utf-8-sig')
        print("Side:多1空-1\tPosition=TodayPosition+HistoryPosition")
        print(df)
        # 人眼的1 -1要转成0 1
        df['Side'] = (1 - df['Side']) / 2
        self.target_position = df
        # print(self.target_position)
        self.update_symbols()

    def merge_portfolios(self):
        # 由于特殊性，合并时有两方案，相加与对冲,把两种方案都写上
        df = None
        for pp in self.portfolios_path:
            _pos = pd.read_csv(pp, dtype={'Symbol': str, 'InstrumentID': str}, quotechar='\'', encoding='utf-8-sig')
            if df is None:
                df = _pos
            else:
                df = pd.concat([df, _pos])

        # 合并
        df = merge_hedge_positions(df, True)
        df.to_csv(self.target_position_path, index=False, encoding='utf-8-sig')
        self.load_positions()
        print('合并多个投资组合完成，对冲投资组合完成，写入目标持仓文件完成，重新加载目标持仓文件完成')

    def save_portfolios(self):
        df = self.position_df.copy()
        df['Side'] = 1 - 2 * df['Side']
        df = df[['Symbol', 'InstrumentID', 'HedgeFlag', 'Side', 'Position', 'InstrumentName']]
        df.to_csv(self.target_position_path, index=False, encoding='utf-8-sig')
        print('保存当前持仓文件')

    def incremental_portfolios(self):
        # 由于特殊性，合并时有两方案，相加与对冲,把两种方案都写上
        _pos = pd.read_csv(self.target_position_path, dtype={'Symbol': str, 'InstrumentID': str}, quotechar='\'',
                           encoding='utf-8-sig')
        _pos2 = pd.read_csv(self.incremental_position_path, dtype={'Symbol': str, 'InstrumentID': str}, quotechar='\'',
                            encoding='utf-8-sig')
        df = pd.concat([_pos, _pos2])

        # 合并
        df = merge_hedge_positions(df, False)
        df.to_csv(self.target_position_path, index=False, encoding='utf-8-sig')
        self.load_positions()
        print('合并一个投资组合和目标持仓文件，对冲投资组合完成，写入目标持仓文件完成，重新加载目标持仓文件完成')

    def query_positions(self):
        # 测试使用
        if self.test_query_from_local:
            f = shelve.open(self.position_df_path, 'r')
            self.position_df = f['position_df']
            f.close()
            self.update_symbols()
            self.position_df.ix[0, 'TodayPosition'] = 8000
            self.position_df.ix[1, 'TodayPosition'] = 600
            self.position_df.ix[0, 'HistoryPosition'] = 2000
            self.position_df.ix[1, 'HistoryPosition'] = 400
            return

        self.wait_lock = True
        query = ReqQueryField()
        # 查持仓，需要先清空
        self.position_dict = {}
        self.position_df = pd.DataFrame(columns=self.position_dtype.names)
        self.td.req_query(QueryType.ReqQryInvestorPosition, query)

    def query_instruments(self):
        # 测试使用
        if self.test_query_from_local:
            f = shelve.open(self.instrument_dict_path, 'r')
            self.instrument_dict = f['instrument_dict']
            f.close()
            return

        self.wait_lock = True
        query = ReqQueryField()
        self.instrument_dict = {}
        self.instrument_df = pd.DataFrame(columns=self.instrument_dtype.names)
        self.td.req_query(QueryType.ReqQryInstrument, query)

    def query_account(self):
        self.wait_lock = True
        query = ReqQueryField()
        self.td.req_query(QueryType.ReqQryTradingAccount, query)

    def update_symbols(self):
        # 期货不用指定交易所，股票需要指定，但这里合并的时候没有使用到交易所
        a = set()
        b = set()
        if self.target_position is not None:
            a = set(self.target_position['InstrumentID'])
        if self.position_df is not None:
            b = set(self.position_df['InstrumentID'])
        self.symbols = list(a | b)
        print(self.symbols)

    def sub_quote(self):
        self.update_symbols()

        symbols_ = pd.Series(self.symbols).str.encode('gbk')
        for i in range(len(symbols_)):
            self.md.subscribe(symbols_[i], b'')
        print('订阅行情')

    def unsub_quote(self):
        symbols_ = pd.Series(self.symbols).str.encode('gbk')
        for i in range(len(symbols_)):
            self.md.unsubscribe(symbols_[i], b'')
        print('取消认阅')

    def hide_quote(self):
        self.print_quote = not self.print_quote
        if self.print_quote:
            print('显示行情')
        else:
            print('屏蔽行情')

    def calc_orders(self):
        """
        计算当前持仓与目标持仓的对比图
        :return:
        """
        if self.symbols is None:
            return 0

        # 股票与期货不一样
        if self.is_stock:
            self.position_df = extend_dataframe_product(self.position_df, (self.symbols, [0, 1]),
                                                        ['InstrumentID', 'Side'])
            self.target_position = extend_dataframe_product(self.target_position, (self.symbols, [0, 1]),
                                                            ['InstrumentID', 'Side'])
            z = pd.merge(self.position_df, self.target_position, how='outer', on=['InstrumentID', 'Side'])
        else:
            self.position_df = extend_dataframe_product(self.position_df, (self.symbols, [0, 1], [0, 1]),
                                                        ['InstrumentID', 'Side', 'HedgeFlag'])
            self.target_position = extend_dataframe_product(self.target_position, (self.symbols, [0, 1], [0, 1]),
                                                            ['InstrumentID', 'Side', 'HedgeFlag'])
            z = pd.merge(self.position_df, self.target_position, how='outer', on=['InstrumentID', 'HedgeFlag', 'Side'])

        z.fillna(0, inplace=True)
        # 用它来指代是股票，需要区分今昨
        z['IsSHFE'] = True

        if self.is_stock:
            self.target_orders = calc_target_orders(z, 'Position_y', 'Position_x', True, 100)
        else:
            # 需要标记出上期所，对于UFX这种不区分今昨的平台，不要做IsSHFE的修改
            z['IsSHFE'] = list(map(is_shfe, map(get_product, z['InstrumentID'])))
            # 这里有计算先平进还是平昨的过程
            self.target_orders = calc_target_orders(z, 'Position_y', 'Position_x', False, None)

        if self.target_orders is None:
            print('交易清单为空')
            return 0
        else:
            # 这里最好能对交易清单进行排序，在反手时，股指最好是先锁仓后平仓，而其它合约先平仓后开仓
            print("Long_Flag:多1空-1\tOpen_Amount:开+平-\tCloseToday_Flag:平今1\tBuy_Amount:买+卖-")
            self.target_orders.sort_values(by='Open_Amount', ascending=True, inplace=True)
            # 控制台中太长，看不清，写文件
            self.target_orders.to_csv(self.target_orders_path, index=False, encoding='utf-8-sig')
            print(self.target_orders[['InstrumentID', 'Long_Flag', 'Open_Amount', 'CloseToday_Flag', 'Buy_Amount']])
            return len(self.target_orders)

    def send_orders(self):
        if self.target_orders is None:
            print('交易清单为空，不下单')
            return

        # 提供一个临时变量用于下单
        order = (OrderField * 1)()
        orderid = (OrderIDTypeField * 1)()
        orderid[0].OrderIDType = b''

        # 将str转成b用于下单
        self.target_orders['InstrumentID_'] = encode_dataframe(self.target_orders[['InstrumentID']].copy())
        # 如果一开始没有持仓等信息，有可能持仓是空的，这个地方会出错
        self.target_orders['ExchangeID_'] = encode_dataframe(self.target_orders[['ExchangeID']].copy())
        for i in range(len(self.target_orders)):
            row = self.target_orders.iloc[i, :]
            order[0].InstrumentID = row['InstrumentID_']
            try:
                # 数字0的转换问题，最好能解决掉
                order[0].ExchangeID = row['ExchangeID_']
            except:
                pass
            order[0].Type = OrderType.Limit
            order[0].Side = OrderSide.Buy if row['Buy_Amount'] > 0 else OrderSide.Sell
            order[0].Qty = abs(row['Buy_Amount'])
            order[0].OpenClose = OpenCloseType.Open
            if row['Open_Amount'] < 0:
                order[0].OpenClose = OpenCloseType.Close
                if row['CloseToday_Flag'] == 1:
                    order[0].OpenClose = OpenCloseType.CloseToday

            # 这里必需要先查一次合约列表
            _symbol = row['InstrumentID']
            _instrument = row['InstrumentID']

            if self.is_stock:
                # 处现了债券怎么办？
                tick_size = 0.01
            else:
                tick_size = get_tick_size(self.instrument_dict3, _symbol, _instrument)

            marketdata = get_market_data(self.marketdata_dict_symbol, self.marketdata_dict_instrument, _symbol,
                                         _instrument)
            if marketdata is None:
                print('%s，没有行情和价格，跳过不下单' % _symbol)
                continue

            price = marketdata.LastPrice
            # 自己修改是挂单还是吃单
            if row['Buy_Amount'] > 0:
                price += self.times_tick_size * tick_size
            else:
                price -= self.times_tick_size * tick_size

            # 进行涨跌停修正，前面肯定取到的涨跌停价了，不然就直接
            price = min(price, marketdata.UpperLimitPrice)
            price = max(price, marketdata.LowerLimitPrice)

            order[0].Price = price
            # 下单
            if self.test_send_order:
                print(
                    '代码:%s\t价格:%f\t买卖:%d\t数量:%d' % (order[0].InstrumentID, order[0].Price, order[0].Side, order[0].Qty))
            else:
                if order[0].Price == 0:
                    print(
                        '代码:%s\t价格:%f\t买卖:%d\t数量:%d' % (
                            order[0].InstrumentID, order[0].Price, order[0].Side, order[0].Qty))
                    print('价格为0，可能停牌，不下单')
                    continue
                ret = self.td.send_order(order[0], orderid[0], 1)
                # 这里无所谓，全打印
                print('LocalID:%s' % ret)

        return

    def cancel_orders(self):
        """
        批量撤单，前提条件是收到所有的回报
        :return:
        """
        cnt = 0
        orderid = (OrderIDTypeField * 2)()
        orderid[0].OrderIDType = b''
        orderid[1].OrderIDType = b''

        for k, v in self.order_dict.items():
            # 只要有挂单就可以撤
            # LeavesQty是剩余数量，CumQty是成交数量，没有撤单信息，所以这个值永远等于0
            left_qty = v.Qty - v.CumQty - v.LeavesQty
            # 只能通过集合来进行撤单了
            if v.Status not in {OrderStatus.Rejected, OrderStatus.Cancelled, OrderStatus.Filled}:
                orderid[0].OrderIDType = v.ID
                self.td.cancel_order(orderid[0], orderid[1], 1)
                cnt += 1
        print('撤单过程执行完毕，有可能非交易时间，已经成交等各种情况导致撤单失败')
        return cnt

    def wait_locks(self, sec=10):
        cnt = sec
        while self.wait_lock and cnt > 0:
            cnt -= 1
            sleep(1)

    def wait_marketdata(self, sec=10):
        cnt = sec
        while len(self.symbols) != len(self.marketdata_dict_symbol) and cnt > 0:
            cnt -= 1
            sleep(1)

    def wait_thread(self, sec=5):
        sleep(sec)

    def wait_logined(self, sec=15, done_cnt=2):
        cnt = sec
        while self.done_cnt < done_cnt and cnt > 0:
            cnt -= 1
            sleep(1)

    def run_all(self):
        for i in range(3):
            print('轮数：', i, '+' * 60)
            self.load_positions()
            self.query_positions()
            # 需要等持仓查完才订阅行情
            self.wait_locks(5)
            ret1 = self.calc_orders()
            if ret1 > 0:
                self.sub_quote()
                self.wait_marketdata(5)
                # 需要等收到行情订阅才下单
                self.send_orders()
                # 可以等一会再撤单，为何要人工做，因为不知道等多久比较好
                if self.is_stock:
                    self.wait_thread(7)
                else:
                    self.wait_thread(3)
                ret2 = self.cancel_orders()
                if ret2 > 0:
                    print("有撤单，肯定要补单")
                else:
                    print("没撤单，有可能成交了，也有可能拒绝了")
            else:
                print('for内无单可下,已经完成目标')
                return

        self.load_positions()
        self.query_positions()
        self.wait_locks(5)
        ret3 = self.calc_orders()
        if ret3 > 0:
            print("for外：还有持仓没有调整完成，请人工检查原因")
        else:
            print('for外：无单可下,已经完成目标')

    def print_orders(self):
        self.order_df = ctypes_dict_2_dataframe(self.order_dict, self.order_dtype)
        self.order_df = decode_dataframe(self.order_df)
        print(self.order_df)

    def usage(self):
        # 1. 在保证没有挂单的前提下，再次查询持仓，计算仓差然后补单
        # 2. 生成委托单列表后，再下单时根据列表的完成度进行补单，这种方法下单比较快，但代码复杂，先不实现
        print(u'-' * 80)
        print(u'11 - 合并对冲多个组合到目标持仓\t12 - 回写查询持仓到目标持仓\t13 - 合并对冲目标持仓和增量仓到目标持仓')
        print(u'1 - 读取目标仓位\t2 - 查询实盘仓位\t3 - 订阅行情\t4 - 计算交易清单\t5 - 批量下单\t6 - 需延迟通过回报批量撤单\t7 - 顺序执行1-6')
        print(u'21 - 查合约列表（期货至少执行一次）\t22 - 查资金\t23 - 取消订阅行情\t24 - 打印订单')
        print(u'33 - 切换行情显示')
        print(u'99 - 重连\tq - 退出')
        print(u'-' * 80)

    def input(self, x):
        _menu = {
            11: self.merge_portfolios,
            12: self.save_portfolios,
            13: self.incremental_portfolios,

            1: self.load_positions,
            2: self.query_positions,  # 需要等待
            3: self.sub_quote,  # 需要等待
            4: self.calc_orders,  # 需要等待
            5: self.send_orders,  # 需要等待
            6: self.cancel_orders,  # 需要等待
            7: self.run_all,  # 需要等待

            21: self.query_instruments,
            22: self.query_account,
            23: self.unsub_quote,
            24: self.print_orders,

            33: self.hide_quote,

            41: self.wait_thread,
            42: self.wait_locks,
            43: self.wait_marketdata,
            44: self.wait_logined,  # 主要是登录的最重要

            99: self.reconnect,
        }

        _menu.get(x, self.usage)()


if __name__ == "__main__":
    # order = (OrderField * 1)()
    # orderid = (OrderIDTypeField * 1)()
    # orderid[0].OrderIDType = b''
    # x = {'InstrumentID': 'agc'}
    # for k, v in x.items():
    #     o = getattr(order[0], k)
    #     if isinstance(o, bytes):
    #         setattr(order[0], k, v.encode('gbk'))
    # orderid[0]
    x = {'InstrumentID': 'agc',
         'ExchangeID': '',
         'Type': OrderType.Market,
         'Side': OrderSide.Sell,
         'Qty': 45,
         'Price': 123.1,
         'OpenClose': OpenCloseType.CloseToday,
         'HedgeFlag': HedgeFlagType.Speculation,
         }
    order = OrderField()
    order.from_dict(x)
    order
