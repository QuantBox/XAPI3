#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
处理与合约名字有关的变量
"""
import re

# 上期所
PRODUCTS_SHFE = {'cu', 'al', 'zn', 'pb', 'ni', 'sn', 'au', 'ag', 'rb', 'wr', 'hc', 'fu', 'bu', 'ru'}
# 中金所
PRODUCTS_CFFEX = {'IF', 'IC', 'IH', 'T', 'TF'}
# 郑商所
PRODUCTS_CZCE = {'SR', 'CF', 'ZC', 'FG', 'TA', 'WH', 'PM', 'RI', 'LR', 'JR', 'RS', 'OI', 'RM', 'SF', 'SM', 'MA', 'WT',
                 'WS', 'RO', 'ER', 'ME', 'TC'}
# 大商所
PRODUCTS_DCE = {'m', 'y', 'a', 'b', 'p', 'c', 'cs', 'jd', 'fb', 'bb', 'l', 'v', 'pp', 'j', 'jm', 'i'}

EXCHANGES_wind_code_xapi = {
    'CFE': 'CFFEX',
    'SHF': 'SHFE',
    'CZC': 'CZCE',
    'DCE': 'DCE',
    'SH': 'SSE',
    'SZ': 'SZSE',
}

EXCHANGES_xapi_wind_code = dict((v, k) for k, v in EXCHANGES_wind_code_xapi.items())


def product_to_exchange(product):
    """
    将合约产品码转成交易所
    :param product:
    :return:
    """
    PRODUCT_ = product.upper()
    if PRODUCT_ in PRODUCTS_CFFEX:
        return 'CFFEX'
    if PRODUCT_ in PRODUCTS_CZCE:
        return 'CZCE'
    product_ = product.lower()
    if product_ in PRODUCTS_SHFE:
        return 'SHFE'
    if product_ in PRODUCTS_DCE:
        return 'DCE'
    return 'Unknown'


def is_shfe(product):
    """
    是否上期所
    多添加此函数的主要原因是上期所需要区分平今与平昨
    :param product:
    :return:
    """
    product_ = product.lower()
    return product_ in PRODUCTS_SHFE


def get_product(symbol):
    """
    从合约名中提取产品名
    :param symbol:
    :return:
    """
    pattern = re.compile(r'(\D{1,2})(\d{0,1})(\d{3})')
    match = pattern.match(symbol)
    if match:
        return match.expand(r'\g<1>')
    else:
        return symbol


def get_exchange(symbol):
    """
    从带.的合约名中提取交易所
    :param symbol:
    :return:
    """
    pattern = re.compile(r'(\.)(\D{1,4})')
    match = pattern.match(symbol)
    if match:
        return match.expand(r'\g<2>')
    else:
        return symbol


if __name__ == '__main__':
    import pandas as pd

    df = pd.DataFrame({'Symbol': ['IF1603', 'rb1708','600000']})
    df['IsSHFE'] = list(map(is_shfe, map(get_product, df['Symbol'])))

    df['product'] = list(map(get_product, df['Symbol']))
    df['IsSHFE'] = list(map(is_shfe, df['product']))
    print(df)
    print(get_product('IF1406'))
    print(EXCHANGES_wind_code_xapi.get('SH'))
