## XAPI行情交易网关接口

本项目使用了以下项目实现基本功能
1. Flask-SocketIO
2. XAPI Python版接口

## 安装与使用
1. 解压`QuantBox统一接口完整版_v0.9.8.8.zip`
2. 执行其中的`X1.复制bin和System32目录_需右键以管理员身份运行.bat`，自动将CTP等库文件安装到默认目录下
3. 配置`config.py` 中的服务器地址与账号,可以根据需要配置只连接行情或交易
4. 运行`python xapi_sio_server.py`即启动了服务，等待客户端连接
5. 访问http://127.0.0.1:5000可以在浏览器中登录并测试下单撤单
6. 如果有些库没有安装，可以参考requirements.txt使用pip进行安装，一定要安装eventlet

## 工作原理
1. Flask Web服务等待客户端的接入后初始化CTP API，自动连接CTP柜台
2. 行情调用流程：Python->XAPI_CPP_x64.dll->CTP_Quote_x64.dll->thostmduserapi.dll
3. 交易调用流程：Python->XAPI_CPP_x64.dll->CTP_Trade_x64.dll->thosttradeapi.dll
4. 收到订阅请求后，开始调用api，然后收到的行情转发
5. 交易接口与行情接口相同

## 已知问题
1. 单用户版，只支持一个用户
2. 没有提供历史数据功能