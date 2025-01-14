TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DEFINES  -= UNICODE

INCLUDEPATH += D:/WinBuildTools/msys64/openssl-3.0.9/include

SOURCES += \
        main.c


LIBS += -L"D:/WinBuildTools/Windows Kits/8.0/Lib/win8/um/x64/" -lCrypt32 -lshlwapi -lshell32 -lOle32 -lWS2_32 -lUserEnv -lSetupAPI -lIPHlpApi -lWtsApi32
LIBS += -L$$PWD\ -llibssl-3-x64 -llibcrypto-3-x64

HEADERS +=
