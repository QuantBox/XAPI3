# FakeAPI伪造官方API

## 介绍
现在很多API都需要填写授权码才能登录，但这个授权码一般都已经硬编码或加密在了客户端当中，而申请授权码可能因为政策原因很难通过，所以获取已经授权的客户端中的授权码成了一些用户的需求。

## 原理
伪造一个dll,然后替换客户端中官方的dll，当客户端运行时调用伪造的dll时，转入的参数就有我们需要的信息。由于官方都会公布头文件，所以伪造dll变得相当简单。

## 使用方法
1. 先确定客户端是用的什么API,比如快期CTP、盈佳LTS
2. 打开AllCppProjects.sln解决方案，找到tools/FakeAPI。设置项目属性的Target Name。如CTP用thosttraderapi,LTS用securitytraderapi，注意没有带“.dll”
3. 由于本人在一个项目中同时集成了CTP和LTS的伪接口，所以编译一次，直接改名dll也能用
4. 备份原客户端中的dll,然后将生成的dll覆盖过去
5. 运行客户端并登录，会弹出一些你所想要的信息
6. 取到后，再换回原来的dll

## 自行伪造其它API
1. 添加一个cpp
2. 添加对应的头文件
3. 参考头文件的方式，在cpp开头添加合适的define进行函数或类的导出
4. 从官方从文件中复制对应的Api类定义到cpp中，修改成继承于这个类，然后把所有的函数先都实现成一个空函数，按情况构造一些假的返回值。参考代码已经提供的CTP与LTS的示列即可
5. 对于有些API如通视接口，由于导出函数名命问题，你可以在export.def中添加。

## 穿透式
1. FakeAPI设置项目属性的Target Name为`thosttraderapi_se`
2. 由于同一项目中`CTP`与`CTP_SE`两个项目的导出函数重复，无法直接编译，所以目前先将`ThostFtdcTraderApi.cpp`移除，只保留了`ThostFtdcTraderApi_SE.cpp`,如果还想使用老版本，反过来操作即可。


