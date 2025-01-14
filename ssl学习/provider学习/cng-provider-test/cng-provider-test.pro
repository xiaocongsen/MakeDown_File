TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DEFINES  -= UNICODE

INCLUDEPATH += D:/WinBuildTools/msys64/openssl-3.0.9/include
INCLUDEPATH += $$PWD/include

SOURCES += \
        cng_provider.c \
        debug.c \
        keymgmt/cng_keymgmt_functions.c \
        legacy_provider.c \
        main.c \
        signature/cng_signature_functions.c \
        store/cng_store_functions.c


LIBS += -L"D:/WinBuildTools/Windows Kits/8.0/Lib/win8/um/x64/" -lCrypt32 -lshlwapi -lshell32 -lOle32 -lWS2_32 -lncrypt -lUserEnv -lSetupAPI -lIPHlpApi -lWtsApi32
LIBS += -L$$PWD\ -llibssl-3-x64 -llibcrypto-3-x64
LIBS += $$PWD/liblegacy.a
LIBS += $$PWD/libcommon.a

HEADERS += \
    cng_provider.h \
    debug.h \
    keymgmt/cng_keymgmt_functions.h \
    legacy_provider.h \
    signature/cng_signature_functions.h \
    store/cng_store_functions.h
