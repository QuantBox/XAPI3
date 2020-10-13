# XAPI的Java接口

## 目的
提供Java接口供程序调用

## 设计思路
* 直接调用XAPI的C版本接口，所有使用方法与C接口完全一样
* 使用JNA技术，采用了JNAerator进行转换（感谢绿茶提供示例和指导）
* jnaerator-0.12-shaded.jar和jna-4.2.1.jar文件都很大，不适合放在github中，请自行下载

## 如何转换C接口为Java接口
1. 在2013年时CTP接口的多个文件可以直转成需要的库，而如今测试多次只能先手工将h文件合并
2. 新建一个XAPI.h文件，先复制XApiC.h中的内容到XAPI.h中，然后将对应的include用对应的文件内容来替换，直到XAPI.h中完全没有include.
3. 到jnaerator上下载最新版的jar并放到当前目录，注意，请下载jnaerator-X.XX-shaded.jar这个文件大小最大的版本。<br/>
项目路径：[https://github.com/nativelibs4java/JNAerator/releases](https://github.com/nativelibs4java/JNAerator/releases)<br/>
快速下载：[http://search.maven.org/#search%7Cga%7C1%7Cjnaerator](http://search.maven.org/#search%7Cga%7C1%7Cjnaerator)
4. 复制C接口的XAPI_CPP.dll到工程目录并改名成XAPI.dll
5. 可能要修改jnaerator.bat中相应jar文件名
6. 运行jnaerator.bat生成对应的目录或文件

## 如何使用
1. 下载JNA到工程目录. <br/>
项目路径：[https://github.com/java-native-access/jna](https://github.com/java-native-access/jna)<br/>
快速下载：[https://maven.java.net/content/repositories/releases/net/java/dev/jna/jna/](https://maven.java.net/content/repositories/releases/net/java/dev/jna/jna/)
2. 复制XAPI.jar和XAPI.dll到工程目录
3. XApiWrapper示例,修改账号等进行测试

