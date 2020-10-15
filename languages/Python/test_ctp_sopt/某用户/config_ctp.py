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

from test_ctp_sopt_api import main

configs = {
    'root_dir': r'D:\Users\Kan\Documents\GitHub\XAPI3\languages\Python\test_ctp_sopt\某用户',
    'md': {
        'Address': br'tcp://124.74.247.142:18213',
        'BrokerID': b'2001',
        'UserID': b'00000020781',
        'Password': b'123456',
    },
    'td': {
        'Address': br'tcp://124.74.247.142:18205',
        'BrokerID': b'2001',
        'AppID': b'1.3.0',
        'AuthCode': b'EG9FWWGUYV2U292T',
        'UserID': b'00000020781',
        'Password': b'123456',
    },
}

if __name__ == '__main__':
    main(configs)
