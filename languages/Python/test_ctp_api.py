#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
调用CTP接口进行目标持仓调整示例
"""
import getopt
from MyXSpi import MyXSpi
from xapi import *

# 从当前config类中加载配置文件
# import config

def init(config):
    # 交易连接
    td = XApi(r'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\XAPI_CPP_x86.dll')
    td.ServerInfo.Address = config['td']['Address']
    td.ServerInfo.BrokerID = config['td']['BrokerID']
    td.UserInfo.UserID = config['td']['UserID']
    td.UserInfo.Password = config['td']['Password']

    # 行情连接
    md = XApi(r'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\XAPI_CPP_x86.dll')
    md.ServerInfo.Address = config['api']['Address']
    md.ServerInfo.BrokerID = config['api']['BrokerID']
    md.UserInfo.UserID = config['api']['UserID']
    md.UserInfo.Password = config['api']['Password']

    ret = td.init(br'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\CTP\CTP_Trade_x86.dll')
    if not ret:
        print(td.get_last_error())
        exit(-1)
    print(ord(td.get_api_type()))
    print(td.get_api_name())
    print(td.get_api_version())

    ret = md.init(br'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\CTP\CTP_Quote_x86.dll')
    if not ret:
        print(md.get_last_error())
        exit(-1)

    print(ord(md.get_api_type()))
    print(md.get_api_name())
    print(md.get_api_version())

    return td, md


def main(configs):
    print(sys.stdin.encoding)
    td, md = init(config.configs)
    spi = MyXSpi(root_dir=config.configs['root_dir'],
                 portfolios=['portfolio_1.csv', 'portfolio_2.csv', 'portfolio_3.csv'],
                 td=td, md=md, is_stock=False)
    spi.connect()
    # 在这最好等那么一会儿
    spi.wait_logined(15, 2)
    print("登录等待结束")

    # 参数的依次执行，由于等待的问题存在，1-6怎么处理
    opts, args = getopt.getopt(sys.argv[1:], "", ["input="])
    for op, value in opts:
        print("参数", op, value)
        if op == "--input":
            input_str = value
            input_list = input_str.split(';')
            for x in input_list:
                print(x)
                i = int(x)
                spi.input(i)
            # exit(0)

    while True:
        spi.usage()
        x = input('请输入：\r\n')

        # 输入数字时，python3是str，python2是int，需要处理一下
        print(x, type(x))

        if x == "q":
            break

        # 处理输入是回车或别符号的可能性
        try:
            i = int(x)
        except:
            continue

        spi.input(i)

    # 循环外，退出
    spi.disconnect()
