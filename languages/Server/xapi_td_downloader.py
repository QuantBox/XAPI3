#!/bin/env python
import sys

sys.path.append(r'../Python')

import argparse

from td_spi import *

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--user', '-u', default='abc', help='用户名')
    parser.add_argument('--password', '-p', default='123456', help='密码')

    args = parser.parse_args()
    print(args.user)
    print(args.password)

    # 准备账号
    td = config_td()
    td.UserInfo.UserID = b'654321'
    td.UserInfo.Password = b'123456'
    td.UserInfo.UserID = bytes(args.user, encoding='utf-8')
    td.UserInfo.Password = bytes(args.password, encoding='utf-8')

    spi = td_spi()
    spi.set_api(td)
    init_td(td)
    td.register_spi(spi)
    if not td.is_connected():
        td.connect()

    input("查持仓")
    query = ReqQueryField()
    td.req_query(QueryType.ReqQryInvestorPosition, query)

    input("查委托")
    query = ReqQueryField()
    td.req_query(QueryType.ReqQryOrder, query)

    input("查成交")
    query = ReqQueryField()
    td.req_query(QueryType.ReqQryTrade, query)

    while True:
        input("查结算单")
        print("当前交易日", spi.TradingDay)
        date = input('请输入结算日:')
        query = ReqQueryField()
        query.DateStart = int(date)  # 20200110
        td.req_query(QueryType.ReqQrySettlementInfo, query)

    input("结束")
