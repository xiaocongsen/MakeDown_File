TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DEFINES  -= UNICODE _UNICODE
INCLUDEPATH += F:\work\UniVPN\csdk/src/ssp/include

SOURCES += \
        main.c

HEADERS +=  \
        SKFAPI.h

LIBS += -L$$PWD\ -llibssl-3-x64 -llibcrypto-3-x64
