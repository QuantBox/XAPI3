# Python版XAPI行情交易接口

## 工作原理
通过Python ctypes调用XAPI_CPP_x86.dll，通过它来调用其它的C封装的接口。
例如针对CTP的thosttraderapi.dll，我们开发一个C封装——CTP_Trade_x86.dll，
由XAPI_CPP_x86.dll来调用CTP_Trade_x86.dll。
其实也可以通过ctypes直接调用CTP_Trade_x86.dll，只是当前没有这样设计，后期有时间再可以改。

## 安装使用
0. 目前项目是在Python2.7/3.5 Win32位下进行的开发，欢迎在Linux等平台下测试反馈和贡献代码
1. 整个项目涉及到C/C++，高手可以参考对应的项目的说明文档进行自行编译。Win64位用户也得自行编译
2. 对于初学者，建议直接使用我们专为OpenQuant插件C#项目发布的安装包
3. 此安装包一般通过QQ群或论坛进行分发
4. 运行安装包中的批处理后实际上是为OpenQuant复制了一些文件到指定目录下。Python用户将直接利用这些文件,减少部署的麻烦

## 测试
1. 通过前一节提到的安装包复制指定文件后
2. 编辑test_xapi.py，设置所需要调用的库dll，如CTP或Tdx
3. 编辑连接的参数，不同的库参数可能不同，需要查找对应的手册或示例
4. 编辑用户名和密码，以及其它准备工作
5. 编辑MyXSpi.py文件，根据需要添加和设置回调的响应函数，需要的响应函数请从XSpi.py中复制
6. 运行test_xapi.py

## 调试
1. 由于XAPI底层另有一个线程通过回调的方法向Python通知事件，所以这些回调函数都无法直接下断点
2. 通过 http://stackoverflow.com/questions/34299082/not-working-python-breakpoints-in-c-thread-in-pycharm-or-eclipsepydev 介绍的方法可以在回调函中断点
3. 运行安装pip install pydevd，在需要下断点的文件加入import pydevd
4. 在要下断点的地方加上pydevd.settrace(suspend=True, trace_only_current_thread=True)即可
5. 对于运行过程中忽然崩溃退出，问题可能出在C层，需要用VS启动Python.exe和.py进行C层代码的调试

## 感谢
本项目参考学习了 https://github.com/jasonweiyi/xapi_python/tree/master/Python 感谢他们所做的工作

## 新测试项目
- 新建了test_ctp_api.py/test_tdx_api.py，主要功能是编辑target_position.csv文件，确定目标仓位是多少。启动后通过1至6等几个步骤，快速的将当前持仓调整为目标持仓。
- 此测试的功能适用于交易频率比低的团队，例如每天交易一两次的。
- 已经在期货上测试通过，股票已经完成，还没有严格测试
- 对于上海的先平今还是先平昨，对于股指是先锁仓再平仓，没有细化，后期可以再改进
- 股票部分

