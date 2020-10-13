#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys

sys.path.append(r'D:\Users\Kan\Documents\GitHub\XAPI2\languages\Python')

# 解决Python 3.6的pandas不支持中文路径的问题
print(sys.getfilesystemencoding())  # 查看修改前的
try:
    sys._enablelegacywindowsfsencoding()  # 修改
    print(sys.getfilesystemencoding())  # 查看修改后的
except:
    pass

from test_ctp_api import main

configs = {
    'root_dir': r'd:' + '\\test_ctp\\某用户',
    'api': {
        'Address': br'tcp://218.202.237.33:10012',
        'BrokerID': b'9999',
        'UserID': b'654321',
        'Password': b'123456',
    },
    'td': {
        'Address': br'tcp://180.168.146.187:10000',
        'BrokerID': b'9999',
        'UserID': b'654321',
        'Password': b'123456',
    },
}

if __name__ == '__main__':
    main(configs)
