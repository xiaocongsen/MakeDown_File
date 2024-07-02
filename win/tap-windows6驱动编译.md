# 虚拟网卡tap编译

## 编译环境准备

1. vs2019
    C++的桌面开发
    Windows 10 SDK
    MSVC v142 - VS 2019 C++ x64/x86 Spectre缓解库
    MSVC v142 - VS 2019 C++ ARM64 Spectre缓解库
    Python 2 64-bit

2. wdk10
    下载和SDK相同版本
    提示插件安装Windows Driver Kit Visual Studio 


3. 编译Tap-Windows6
    开始菜单找到Developer Command Prompt for VS 2019，打开。
    跳转到代码目录，执行命令
    python2.7.exe  buildtap.py -c -b --ti=devcon --sdk=WDK

