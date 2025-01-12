# FaultInjection

FaultInjection是一个控制开发板产生流量数据的简易自研工具，由前端Front和后端Back构成，前端是一个网页界面，用于操作后端。
后端是运行在windows上面的程序， 接受前端命令，转换为ssh命令发送给各个开发板子。所以此程序可以控制开发板的前提是二者网络互通。

## 安装环境

### Back 后端
使用visual studio打开项目**back/Fault.sln**，运行代码，无报错，出现cmd如下窗口即成功。
![success](image/success.png)

* visual studio 2022  
    从官网下载安装，需要组件
    ![vs](image/vs.png)

    如果已经安装了vs，可以重新打开此界面  
    ![vs_se](image/vs_set.png)

* vcpkg  
    vcpkg作用是c++库管理器，下载管理依赖库
    * 下载vcpkg
        ```
        git clone https://github.com/microsoft/vcpkg.git
        ```
    * 添加vcpkg目录至环境变量
    * 安装，vcpkg目录下
        ```
        .\bootstrap-vcpkg.bat
        ```
    * 整合至vs
        ```
        .\vcpkg integrate install
        ```
    * 安装依赖库
        ```
        .\vcpkg install
        ```
        * libssh 
        * cpprestsdk 
        * jsoncpp
        * boost-filesystem
        * fmt  
            fmt存在兼容性问题，当报错时注释掉红框内模版
            ![fmt](image/fmt.png)
    * utf-8兼容性  
        不一定会出现，如果出现了注释掉红框模板
        ![utf8](image/utf8.png)

### Front 前端
安装npm软件，npm可以直接在cmd中运行验证安装成功。

### 终端工具
控制板子需要终端工具，推荐**MobaXterm**，控制orangepi的板子需要一个额外的usb转串口硬件CH340，串口属性:
![fmt](image/serial.jpg)
