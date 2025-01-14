TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DEFINES  -= UNICODE _UNICODE

INCLUDEPATH += $$PWD\internal
INCLUDEPATH += $$PWD\openssl

LIBS += -L"D:/WinBuildTools/Windows Kits/8.0/Lib/win8/um/x64/" -lCrypt32 -lshlwapi -lshell32 -lOle32 -lWS2_32 -lUserEnv -lSetupAPI -lIPHlpApi -lWtsApi32
LIBS += -L$$PWD\ -llibssl-1_1-x64 -llibcrypto-1_1-x64
#LIBS += -L$$PWD\ -llibssl-3-x64 -llibcrypto-3-x64

SOURCES += \
        main.c

HEADERS += \
    SKFAPI.h
