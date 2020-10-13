#!/usr/bin/env python
# -*- coding: utf-8 -*-

# 参考此文进行测试
# 提供的方法很多，最后找了一个简单又能达到目的
# http://stackoverflow.com/questions/36932/how-can-i-represent-an-enum-in-python

class EnumBase(type):
    def __init__(self, name, base, fields):
        super(EnumBase, self).__init__(name, base, fields)
        self.__mapping_int = dict((v, k) for k, v in fields.items())
        self.__mapping_str = dict((k, v) for k, v in fields.items())

    def __getitem__(self, val):
        try:
            return self.__mapping_int[val]
        except KeyError:
            return self.__mapping_str[val]


def enum_kv(*seq, **named):
    enums = dict(zip(seq, range(len(seq))), **named)
    return EnumBase('Enum', (), enums)


def enum_k(start, *sequential, **named):
    enums = dict(zip(sequential, range(start, start + len(sequential))), **named)
    return EnumBase('Enum', (), enums)


"""
连接状态,此枚举主要供程序进行识别使用
"""
ConnectionStatus = enum_k(
    0,
    'Uninitialized',
    'Initialized',
    'Disconnected',
    'Connecting',
    'Connected',
    'Authorizing',
    'Authorized',
    'Logining',
    'Logined',
    'Confirming',
    'Confirmed',
    'Doing',
    'Done',
    'Unknown',
)

"""
API功能类型，功能可叠加
"""
ApiType = enum_kv(
    Nono=0,
    Trade=1,
    MarketData=2,
    Level2=4,
    QuoteRequest=8,
    HistoricalData=16,
    Instrument=32,
    Query=64,
)

"""
流恢复类型
"""
ResumeType = enum_k(
    0,
    'Restart',
    'Resume',
    'Quick',
    'Undefined',
)

"""
日志记录级别
"""
LogLevel = enum_k(
    0,
    'Trace',
    'Debug',
    'Info',
    'Error',
    'Fatal',
)

"""
期权类型
"""
PutCall = enum_k(
    0,
    'Put',
    'Call',
)

"""
委托状态，与OpenQuant2014中的值一样
"""
OrderStatus = enum_k(
    0,
    'NotSent',
    'PendingNew',
    'New',
    'Rejected',
    'PartiallyFilled',
    'Filled',
    'PendingCancel',
    'Cancelled',
    'Expired',
    'PendingReplace',
    'Replaced',
)

"""
方向
新股申购、回购，质押都暂时不写入，等业务掌握熟了后再加
"""
OrderSide = enum_k(
    0,
    'Buy',
    'Sell',
    'LOFCreation',
    'LOFRedemption',
    'ETFCreation',
    'ETFRedemption',
    'Merge',
    'Split',
    'CBConvert',
    'CBRedemption',
    'Unknown',
)

"""
报单类型，与OpenQuant2014中的值一样
各交易所的报单指令相当多，实际交易时只取市价和限价两种
"""
OrderType = enum_k(
    0,
    'Market',
    'Stop',
    'Limit',
    'StopLimit',
    'MarketOnClose',
    'Pegged',
    'TrailingStop',
    'TrailingStopLimit',
)

"""
TimeInForce，与OpenQuant2014中的值一样
实际只识别IOC和FOK，其它都当成普通类型
"""
TimeInForce = enum_k(
    0,
    'ATC',
    'Day',
    'GTC',
    'IOC',
    'OPG',
    'OC',
    'FOK',
    'GTX',
    'GTD',
    'GFS',
    'AUC',
)

"""
持仓方向，与OpenQuant2014中的值一样
"""
PositionSide = enum_k(
    0,
    'Long',
    'Short',
)

"""
执行类型，与OpenQuant2014中的值一样
一般与OrderStatus对应，但ExecCancelReject与ExecReplaceReject时与OrderStatus不对应
"""
ExecType = enum_k(
    0,
    'New',
    'Stopped',
    'Rejected',
    'Expired',
    'Trade',
    'PendingCancel',
    'Cancelled',
    'CancelReject',
    'PendingReplace',
    'Replace',
    'ReplaceReject',
    'TradeCorrect',
    'TradeCancel',
    'OrderStatus',
    'PendingNew',
    'ClearingHold',
)

"""
开平标志，没有提供专门的平昨，而是用Close
"""
OpenCloseType = enum_k(
    0,
    'Undefined',
    'Open',
    'Close',
    'CloseToday',
)
"""
投机套保标志
"""
HedgeFlagType = enum_k(
    0,
    'Speculation',
    'Arbitrage',
    'Hedge',
    'Covered',
    'MarketMaker',
)

"""
合约类型，与OpenQuant2014中的值一样
"""
InstrumentType = enum_k(
    0,
    'Stock',
    'Future',
    'Option',
    'FutureOption',
    'Bond',
    'FX',
    'Index',
    'ETF',
    'MultiLeg',
    'Synthetic',
    'LOF',
)

"""
证件类型，参考于CTP，去除了一些感觉不大可能用到的类型
一般登录时返回，原计划是对机构账号与个人账号进行区别收费
"""
IdCardType = enum_k(
    0,
    'EID',
    'IDCard',
    'Passport',
    'LicenseNo',
    'TaxNo',
    'DrivingLicense',
    'SocialID',
    'LocalID',
    'OtherCard',
)
"""
# 交易所类型，按交易所官网的缩写决定
# 例如：
# 上交所有SH(chr(国内网站))/SS(chr(Yahoo))/SSE(chr(LTS))/SHA(chr(Google))
# 深交所有SZ(chr(国内网站))/SZE(chr(LTS))/SZSE(chr(官网))
# 港交所有HK(chr(国内网站))/HKEx(chr(官网))
"""
ExchangeType = enum_k(
    0,
    'Undefined',
    'SHFE',
    'DCE',
    'CZCE',
    'CFFEX',
    'INE',
    'SSE',
    'SZSE',
    'SGE',
    'NEEQ',
    'HKEx',
)

"""
合约生命周期状态类型
"""
InstLifePhaseType = enum_k(
    0,
    'NotStart',
    'Started',
    'Pause',
    'Expired',
    'Issue',
    'FirstList',
    'UnList',
)

"""
交易阶段类型
"""
TradingPhaseType = enum_k(
    0,
    'BeforeTrading',
    'NoTrading',
    'Continuous',
    'AuctionOrdering',
    'AuctionBalance',
    'AuctionMatch',
    'Closed',
    'Suspension',
    'Fuse',
)

"""
业务类型，主要是有些API实际上已经对接了各种市场
不同的市场需要不同的函数，统一到一起实在太难,所以这个地方交给用户自己来指定
"""
BusinessType = enum_k(
    0,
    'Undefined',
    'Future',
    'Stock',
    'Option',
)

if __name__ == '__main__':
    Numbers = enum_kv(ONE=1, TWO=2, THREE='three')
    print(Numbers.TWO)
    print(Numbers[Numbers.ONE])
    print(Numbers[2])
    print(Numbers['three'])

    print(ConnectionStatus)
    print(type(ConnectionStatus))
    print(ConnectionStatus.Authorized)
    print(ConnectionStatus[ConnectionStatus.Authorized])
    print(type(ConnectionStatus.Authorized))
    print(ConnectionStatus['Authorized'])
