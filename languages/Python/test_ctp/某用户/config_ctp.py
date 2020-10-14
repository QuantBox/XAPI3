#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys

sys.path.append(r'D:\Users\Kan\Documents\GitHub\XAPI3\languages\Python')

# 解决Python 3.6的pandas不支持中文路径的问题
print(sys.getfilesystemencoding())  # 查看修改前的
try:
    sys._enablelegacywindowsfsencoding()  # 修改
    print(sys.getfilesystemencoding())  # 查看修改后的
except:
    pass

from test_ctp_api import main

configs = {
    'root_dir': r'D:\Users\Kan\Documents\GitHub\XAPI3\languages\Python\test_ctp\某用户',
    'api': {
        'Address': br'tcp://180.168.146.187:10110',
        'BrokerID': b'9999',
        'UserID': b'654321',
        'Password': b'123456',
    },
    'td': {
        'Address': br'tcp://180.168.146.187:10100',
        'BrokerID': b'9999',
        'AppID': b'8500342533',
        'AuthCode': b'0000000000000000',
        'UserID': b'14078611',
        'Password': b'zaq1xsw2',
    },
}

if __name__ == '__main__':
    main(configs)
