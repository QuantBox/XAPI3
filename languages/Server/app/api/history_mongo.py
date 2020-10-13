import pymongo
import pandas as pd
from datetime import datetime

import logging

logger = logging.getLogger(__name__)


def connnect_collection():
    """
    连接数据库并选择表
    :return:
    """
    client = pymongo.MongoClient(
        host='192.168.15.159',
        port=20000,
        username='user-read',
        password='123456',
        authSource='market-data-v1',
        authMechanism='SCRAM-SHA-1')

    db = client['market-data-v1']
    coll = db['KLine']
    return coll


def kline_download_df(coll, code='AG1906', period=1):
    """
    增量下载数据
    :param coll:
    :param code:
    :param period: 周期，数据库中目前是1min对应1
    :param barTime: 格式是：年月日时分秒
    :return:
    """
    # 有可能放假时间比较长，所以还是反过来取感觉更快
    q = coll.find(filter={'code': code.upper(), 'period': period},
                  projection={'code': 1, 'date': 1, 'time': 1,
                              'open': 1, 'high': 1, 'low': 1, 'close': 1,
                              'barTime': 1,
                              'businessAmount': 1,
                              'businessBalance': 1,
                              'amount': 1,
                              'tradingDay': 1,
                              '_id': 0}).sort([('barTime', -1)]).limit(2000)  # 测试时只查一部分
    print(datetime.now(), '查询语句准备好', code)
    # 由于使用了迭代器，所以只能直接list，不能先套用别的处理方法，否则丢失数据
    df = pd.DataFrame(list(reversed(list(q))))
    print(datetime.now(), '数据下载封装完成', code, len(df))
    if len(df) == 0:
        return None
    # 为了实现数据过滤，先不处理
    df['barTime'] = pd.to_datetime(df['barTime'].astype(str), format='%Y%m%d%H%M%S')
    df[['open', 'high', 'low', 'close']] = df[['open', 'high', 'low', 'close']] / 1000000
    return df

def kline_download_list(coll, code='AG1906', period=1):
    """
    增量下载数据
    :param coll:
    :param code:
    :param period: 周期，数据库中目前是1min对应1
    :param barTime: 格式是：年月日时分秒
    :return:
    """
    # 有可能放假时间比较长，所以还是反过来取感觉更快
    q = coll.find(filter={'code': code.upper(), 'period': period},
                  projection={'code': 1, 'date': 1, 'time': 1,
                              'open': 1, 'high': 1, 'low': 1, 'close': 1,
                              'barTime': 1,
                              'businessAmount': 1,
                              'businessBalance': 1,
                              'amount': 1,
                              'tradingDay': 1,
                              '_id': 0}).sort([('barTime', -1)]).limit(2000)  # 测试时只查一部分
    logger.info('查询准备:%s', code)
    l = list(q)
    logger.info('数据下载封装完成:%s,%d', code, len(l))
    return list(reversed(l))


if __name__ == "__main__":
    coll = connnect_collection()
    df = kline_download_df(coll)
    print(df)
