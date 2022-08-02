TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#INCLUDEPATH += /home/xcs/GmSSL-gmbrowser-v0.1/ssl/
#INCLUDEPATH += /home/xcs/GmSSL-gmbrowser-v0.1/crypto/

INCLUDEPATH += /home/xcs/TASSL-1.1.1b-V_1.4/ssl/
INCLUDEPATH += /home/xcs/TASSL-1.1.1b-V_1.4/crypto/

SOURCES += \
    tasslclient.c



INCLUDEPATH += /home/xcs/TASSL-1.1.1b-V_1.4/include

LIBS +=  /home/xcs/TASSL-1.1.1b-V_1.4/libssl.a /home/xcs/TASSL-1.1.1b-V_1.4/libcrypto.a  -ldl -lpthread
LIBS += $$PWD/libdopra-64.a

HEADERS += \
    icode.h
