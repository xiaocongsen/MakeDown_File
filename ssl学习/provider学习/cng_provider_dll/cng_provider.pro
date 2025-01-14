CONFIG -= qt

TEMPLATE = lib
DEFINES += SKF_PROVIDER_LIBRARY
DEFINES  -= UNICODE _UNICODE
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += D:/WinBuildTools/msys64/openssl-3.0.9/include

SOURCES += \
    cng_provider.c \
    debug.c \
    keymgmt/cng_keymgmt_functions.c \
    signature/cng_signature_functions.c \
    store/cng_store_functions.c

HEADERS += \
    cng_provider.h \
    debug.h \
    keymgmt/cng_keymgmt_functions.h \
    signature/cng_signature_functions.h \
    store/cng_store_functions.h


INSTALLS += target

LIBS += -L"D:/WinBuildTools/Windows Kits/8.0/Lib/win8/um/x64/" -lCrypt32 -lWS2_32 -lncrypt
LIBS += -L$$PWD\ -llibssl-3-x64 -llibcrypto-3-x64
