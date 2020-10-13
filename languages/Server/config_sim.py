from xapi import XApi

import logging
from setup_logging import setup_logging

setup_logging()

logger = logging.getLogger(__name__)

# 启动行情或交易
enable_md = True
enable_td = True


def config_md():
    """
    行情信息
    :return:
    """
    print('config_md')
    # 行情连接
    api = XApi(r'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x64\XAPI_CPP_x64.dll', 'md')
    api.ServerInfo.Address = br'tcp://218.202.237.33:10012'
    api.ServerInfo.BrokerID = b'9999'
    api.UserInfo.UserID = b'654321'
    api.UserInfo.Password = b'123456'

    return api


def config_td():
    """
    交易信息
    :return:
    """
    print('config_td')
    # 行情连接
    api = XApi(r'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x64\XAPI_CPP_x64.dll', 'td')
    api.ServerInfo.Address = br'tcp://218.202.237.33:10002'
    api.ServerInfo.BrokerID = b'9999'
    api.UserInfo.UserID = b'037505'
    api.UserInfo.UserID = b'037504'
    api.UserInfo.Password = b'123456'

    return api


def init_md(api):
    print('init_md')
    ret = api.init(br'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x64\CTP\CTP_Quote_x64.dll')
    if not ret:
        print(api.get_last_error())
        exit(-1)

    print(ord(api.get_api_type()))
    print(api.get_api_name())
    print(api.get_api_version())
    return api


def init_td(api):
    print('init_td')
    ret = api.init(br'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x64\CTP\CTP_Trade_x64.dll')
    if not ret:
        print(api.get_last_error())
        exit(-1)

    print(ord(api.get_api_type()))
    print(api.get_api_name())
    print(api.get_api_version())
    return api
