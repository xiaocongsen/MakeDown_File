TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /home/xcs/GmSSL-gmbrowser-v0.1/ssl/
INCLUDEPATH += /home/xcs/GmSSL-gmbrowser-v0.1/crypto/

SOURCES += \
    gmclient.c

INCLUDEPATH += /home/xcs/GmSSL-gmbrowser-v0.1/include

LIBS +=  /home/xcs/GmSSL-gmbrowser-v0.1/libssl.a /home/xcs/GmSSL-gmbrowser-v0.1/libcrypto.a  -ldl -lpthread
