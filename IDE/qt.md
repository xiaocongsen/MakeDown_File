# qt问题集合

## 解决Could not start process "make" qmake_all问题

    sudo apt install build-essential

## Qt error: /usr/bin/ld: cannot find -lGL: No such file or directory

    sudo apt install mesa-common-dev libglu1-mesa-dev

## error   Qt5Config.cmake qt5-config.cmake

    sudo apt-get install qtbase5-dev

## error   Qt5LinguistToolsConfig.cmake  qt5linguisttools-config.cmake

    sudo apt-get install qttools5-dev

## qt Ui工程

```pro
QT += gui core widgets network
TEMPLATE = app
TARGET = SecoClient
CONFIG += c++14
# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += common/src/ui/include/
linux{
    message("src编译平台为: linux")
INCLUDEPATH += linux/src/
}
win32{
    message("src编译平台为: win32")
INCLUDEPATH += win/src/
}

mac{
    message("src编译平台为: mac")
INCLUDEPATH += mac/src/
}

HEADERS += aboutui.h \
           mainui.h 

linux{
HEADERS +=  ic.h \
            uz.h \
            ase.h \
            ode.h \
            dm.h 
}

win32{
HEADERS += ecFunc.h \
           auz.h \
           base.h \
           buildef.h \
           code.h 
}

FORMS += common/src/ui/aboutui.ui \
         common/src/ui/mainui.ui 

SOURCES += ui.cpp \
           idget.cpp \
           submit.cpp \
           ingWidget.cpp 

linux{
SOURCES +=  fm.c \
            ni.c 
}

win32{
SOURCES += api.c \
           win32.c \
           zip.c
}

RESOURCES += common/src/ui/resources.qrc

message(QMAKE_HOST.arch)
contains(QMAKE_HOST.arch, x86){
    DEFINES += PLAT_CPU=0x10000000
}else{
    contains(QMAKE_HOST.arch, x86_64){
        DEFINES += PLAT_CPU=0x20000000
    }else{
    contains(QMAKE_HOST.arch, armv7s){
        DEFINES += PLAT_CPU=0x40000000
    }else{
    contains(QMAKE_HOST.arch, arm64){
        DEFINES += PLAT_CPU=0x80000000
    }else{
    contains(QMAKE_HOST.arch, arm){
        DEFINES += PLAT_CPU=0x00000001
    }else{
    contains(QMAKE_HOST.arch, armv7){
        DEFINES += PLAT_CPU=0x00000002
    }else{}}}}}}

message($$QMAKE_HOST.os)
contains(QMAKE_HOST.os, Windows){
    DEFINES += PLAT_OS_WIN
    DEFINES += PLAT_OS=0x10000000
}else{
contains(QMAKE_HOST.os, Mac){
    DEFINES += PLAT_OS_MAC
    DEFINES += PLAT_OS=0x20000000
}else{
contains(QMAKE_HOST.os, Linux){
    DEFINES += PLAT_OS_LINUX
    DEFINES += PLAT_OS=0x40000000
}else{
contains(QMAKE_HOST.os, Ios){
    DEFINES += PLAT_OS=0x80000000
}else{
contains(QMAKE_HOST.os, Android){
    DEFINES += PLAT_OS=0x00000001
}else{}}}}}

linux{
QMAKE_LFLAGS += -Wl,-z,relro
QMAKE_LFLAGS += -Wl,-z,noexecstack
QMAKE_LFLAGS += -Wall -O2 -fsigned-char -m64
QMAKE_LFLAGS += -Wl,-gc-sections
QMAKE_LFLAGS += -Wl,-O1
QMAKE_LFLAGS += -Wl,--disable-new-dtags,--rpath,./lib

LIBS += $$PWD/app/linux/lib/libff.a
LIBS += -L/usr/local/toolchains/Qt5.12.8/5.12.8/gcc_64/lib -lQt5Widgets -lQt5Gui -lQt5Core -lQt5Network
LIBS += -lpthread -ldl
}

win32{
QMAKE_LFLAGS += -Wl,-subsystem,windows -mthreads
LIBS += $$PWD/app/win/libs/lb.a
LIBS += -L"C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x86/" -lWS2_32 -lshlwapi -lIPHlpApi
LIBS += C:/Windows/System32/dbghelp.dll
LIBS += -LD:\Qt5.12.8\5.12.8\mingw73_32\lib -lQt5Widgets -lQt5Gui -lQt5Core -lQt5Network
}

```

## qt加载库问题调试

    export QT_DEBUG_PLUGINS=1       开启调试信息输出
    然后运行qt程序

    unset QT_DEBUG_PLUGINS  取消调试信息
