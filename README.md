# XAPI统一行情交易接口

## 交流QQ群
XAPI统一行情交易接口 377319643  
XAPI标准委员会 142703128（需邀请，用于高手进行标准的制定）

## 介绍
现在市场上的交易与行情API太多，对每家都接特别麻烦，如果能统一用一套API接入就好了。  
如果进行接口的统一，要实现以下几点：

1. 统一的结构体
2. 统一的调用方式
3. 灵活的加载方式

实现统一的结构体必然会有些API的特殊字段没法照顾到;调用方式统一则有些功能将不会实现，而有些功能需要几个请求，或本机模拟的方法来实现;加载方式以前C#层是用的P/Invoke,现在改用LoadLibraryEx/dlopen来实现。

## 目录结构
1. **apis**:各种行情交易API的XAPI封装代码
2. **bin**:各项目的编译输出路径
3. **common**:一些通用库
4. **include**:各种头文件，或API的接口
5. **lib**:各种库文件
6. **languages**:各种语言的XAPI封装
7. **tests**：测试代码
8. **tools**:一些小工具。如FakeAPI用来获取CTP/LTS客户端授权码，请看项目中的READEME.md

## 编译XAPI

### 使用CMake
直接使用CMake生成工程即可，对于不熟悉cmake的朋友，可以安装cmake (https://cmake.org/) 后，直接执行在工程根目录中的 build 脚本即可。
1. windows环境下，运行build.bat，在build目录中，会生成对应的vs工程文件。注意，默认情况下，生成的是32位vs工程，若想指定生成工程的版本，运行cmake --help查看。
1. linux环境下，运行build.sh，在build目录中，会生成对应的makefile工程文件。

### 直接使用VS工程
1. 打开AllCppProjects.sln
2. `apis\CTP_Quote`等一类的项目，是对应的API的封装。编译好后还需要将API官方的库复制到同目录，你可以直接运行CopyApiDll.bat将依赖的dll复制到bin对应的目录下
3. 有些项目需要使用到一些授权库的功能`common\License`，比如说`apis\CTP_Trade`，直接编译你会发现编译不过，这是因用到的第三方的库文件太大，我在提交代码时提交的是zip。你需要先解压`include/cryptopp.zip`和`lib/cryptlib.zip`。你也可在项目属性中去掉**ENABLE\_LICENSE**，这样授权功能模块将不参与编译。具体的方式是，哪个项目编译的时候，报关于randpool.h的错误，就在哪个项目的属性`Configuration Properties -> C/C++ -> Preprossor`中找到**ENABLE\_LICENSE**，然后去掉它。

## 编译各种语言封装
### C/C++版
languages\CPP\XAPI\_CPP，直接编译即可。会生成XAPI_CPP.dll，内置了C和C++两种调用方式。tests\CPP中提供了C++版接口的调用示例。所要用到的头文件是include/XApiCpp.h，它的调用的机制模仿了CTP，使用CXApi和CXSpi.  
如想使用C版接口，可参考CXApi与CXSpi是如何实现的，直接使用include/XApiC.h头文件

### C#版
languages\CSharp\XAPI\_CSharp.sln  
编译后会生成XAPI\_CSharp.dll,这个项目支持COM功能，所以在编译时会进行COM注册。这个注册需要VS以管理员权限运行。如果你不想使用COM功能，也不想切换到管理员进行注册，可以在项目属性页中的Build->Register for COM interop前的勾去了。  
编译XAPI_CSharp，有两种输出方式

1. 生成XAPI_CSharp.dll,设置输出类型为类库，可以在其它项目中引用
2. 生成XAPI_COM.exe,设置输出类型为控制台程序，将运行项目中的Program.cs文件，是API的使用示例

### MATLAB版
实际上是调用的C#版的库，细节请参考languages\MATLAB下的readme.md

### COM版
其实也是用的C#版的库，注册好后直接到tests\COM下运行示例即可

## 部署
bin/Debug/x86下会编译一些库，运行CopyApiDll.bat复制一些API官方的库进来，然后再运行clearBin.bat将编译时的一些多余文件清理。在目标机器或客户端下新建XAPI文件夹，将x86文件夹复制到此。注意Queue\_x86.dll一定要放在合适的位置，实在搞不清就放在与CTP\_Quote\_x86.dll这些库在同一个目录下。

## 如何开发新的接口
1. 模仿已经提供的几个项目即可

## CTP穿透式监管
1. www.sfit.com.cn 上穿透式有分生产版与评测版。实盘使用生产版，simnow使用评测版
2. 需要根据不同的场景进行编译，到sfit网站上下载对应版本，解压，将对应文件覆盖到`include\CTP_SE`下
3. 编译`CTP_SE_Quote`和`CTP_SE_Trade`两个项目即可，使用时得复制相应版本的`thostmduserapi_se.lib`和`thosttraderapi_se.dll`