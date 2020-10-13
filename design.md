# XAPI2设计文档

## 支持的API
目前想支持的：
* CTP 期货交易
* CTPZQ
* LTS 股票交易
* Femas 期货交易
* Kingstar
* KingstarGold 上海黄金交易所的接口
* SPTrader 想干的经纪公司基本都用这个
 
以后再说的：
* XSpeed
* XSpeed_Stock


## 设计中需要解决的问题

####1. 在`ApiStruct.h`中的结构体的命名

在[`ApiStruct.h`](https://github.com/QuantBox/XAPI2/blob/master/include/ApiStruct.h)中很多结构体的名称以Field结尾，比如`PositionField`。但是在C++中，结构体中的元素才叫做field，结构体名称不叫Field。

建议修改：把结构体的名称中的Field后缀去掉，比如`PositionField`更名为`Position`。这样从C++的概念上才正确，其次代码因为结构体的名称变短会更易读。

####2. [CXApi类](https://github.com/QuantBox/XAPI2/blob/master/include/XApiCpp.h)中的回调函数中的int size1参数
在这些回调函数中，有个参数`int size1`，似乎和业务不相关。

解决方案：（来自伍侃）回调函数的参数风格按照CTP回调函数的风格来修改：在回调函数中，参数为先是数据相关的，然后是一个错误，然后是一个`isLast`。

####3. 枚举类型`ExchangeType`
在[`ApiEnum.h`](https://github.com/QuantBox/XAPI2/blob/master/include/ApiEnum.h)中定义了`ExchangeType`枚举类型。但是在[CXApiCpp.h](https://github.com/QuantBox/XAPI2/blob/master/include/XApiCpp.h)中，有些地方使用的是`ExchangeType`，比如回调函数`OnFilterSubscribe`。但是有些地方使用的是以字符串指定的交易所，比如函数`Subscribe`，`Unsubscribe`，`ReqQryInstrument`和`ReqQryInvestorPosition`。从减少混淆的角度出发，应该统一使用`ExchangeType`来引用交易所。

建议修改：所有需要交易所的地方都使用ExchangeType。

####4. 结构体DepthMarketDataNField的名称
在[`ApiStruct.h`](https://github.com/QuantBox/XAPI2/blob/master/include/ApiStruct.h)中定义了结构体`DepthMarketDataNField`。在XAPI2中并没有找到`DepthMarketDataField`，是否可以把其中的N去掉，并且去掉Field后缀（见问题1）。

建议修改：把`DepthMarketNDataField`重命名为`DepthMarketData`。

####5. 是否需要增加一个OnRspQryOrder的回调函数
现在有一个OnRtnOrder，是否需要增加一个OnRspQryOrder？
问题：OnRspQryOrder是响应哪个request的回调函数？OnRspQryOrder在什么情况下会被调用？
同样，是否要增加一个OnRspQryTrade的回调函数？

####6. 对于股票的只有查询，没有主动回报的功能的API，可能查询委托功能会交给用户
这是什么意思？

####8. OrderField.ID在什么范围内能保证ID的唯一性
关于`OrderField.ID`，因为是字符串，如果把它变长一点，`yyyy-mm-dd:HHMMSS:SessionID:FrontID:OrderRef`，这样让ID在历史上都是唯一的，而且依然有实际意义（而不是像UUID）。用户只需要在其前面加上策略的名称，就能做到整个策略组合ID历史唯一。

修改建议：如上，伍侃反馈：同意，以后改。
新问题：通过查询进行订单的恢复，报单的时间可能没办法恢复成功，特别是在秒部分，本地下单时间与柜台转的时间基本上无法对应上。


####9. 撤单的问题
1.撤单一定要指定FrontID:SessionID:OrderRef，好像还要指定别的东西，但实在不记得了是哪些了
2.FrontID与SessionID不一定是本次会话的，而必须是发单时会话的值，也就是断线后重连，要撤上次的单必需用上次的值
3.所以我一般是在重新登录时，查询委托列表，然后要撤单时通过这个列表把撤单的结构体全写上

首先，我下单时会返回一个FrontID:SessionID:OrderRef字符串
撤单时通过这个字符串查一个map，找到对应的委托，然后撤单

存在的问题
1.断线时再重连，由于下单太多，这个表可能要返回很久
2.这时立即撤单会由于表没有更新完，撤单失败


####10. 下单时的问题
1.主要是批量下单时的结构体如何设计
2.目前的方法是SendOrders(OrderField[] orders,out string[] orderref)
这个存在的问题是stirng[]中的string是一种比较复杂的类型，不同语言或框架中都有可能不一样，再对其[]就更复杂了，还要out。。。
3.批量下单时，不一定每个单子都能下成功，如何通过string[]或别的方式来区分成功与失败呢？
4.我以前的方法是int SendOrders(order),登录时得到FrontID:SessionID，然后返回的int与FrontID:SessionID拼起来，就是一个唯一的字符串
后来因为Femas必须前面填充为0，填满共12个。这样FrontID:SessionID:OrderRef实际拼接方法可能就各不一样
后来又改成char* SendOrders(order)，也就是一开始就是传出的唯一的字符串，但这种方法需要解决内存释放的问题

所有现在需要大家共同设计一下SendOrders与CacelOrders。

####[已解决] 11. XAPI2的目录结构
目录中的项目名称不应该包括QuantBox前缀，因为没有必要，同时QuantBox是一个比XAPI2更大的概念。

解决方案：已经去掉了前缀。

####[已解决] 12. XAPI2的编译输出路径
XAPI2中各项目的输出路径都应该同一放到一个文件夹中。

解决方案：所有编译输出放在XAPI2\bin中，其中子目录结构举例：XAPI2\bin\Debug\x64\CTP\($solution)_($project)_($arch).dll。

#### 13. XAPI与XAPI2名称的使用
在XAPI2中的各项目中，XAPI和XAPI2不应该视为可混用(interchangable)。XAPI应该仅用来表达与之前XAPI相关的内容（如果有的话），否则应该一律使用XAPI2。

比如在C#的接口中，名称空间应该是QuantBox.XAPI2，而不是QuantBox.XAPI。

#### 14. 消息队列MessageQueue对远程推送数据的支持
MessageQueue需要能够将数据通过不同的消息中间件（zeromq, Rabbitmq）推送到其它地方（其它机器，网络）中的能力。考虑的解决方案为：将CMsgQueue中的回调函数m_fnOnResponse从一个回调函数扩展成一组回调函数，然后实现一些回调函数，可以向zeromq或Rabbitmq中发送数据。这样可以做到各种数据分发底层实现的低耦合。用户可以随意增加和替换。zeromq在网络中断的时候不负责消息丢失的维护，Rabbitmq则负责。所以，zeromq发送行情数据类的消息应该够用，Rabbitmq可能比较适合发送交易类的消息。
