TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
#        main.c
        sm2svr.c

INCLUDEPATH += /home/xcs/work/UniVPN/common/3rd/TASSL-1.1.1b-V_1.4/ssl/
INCLUDEPATH += /home/xcs/work/UniVPN/common/3rd/TASSL-1.1.1b-V_1.4/crypto/

INCLUDEPATH += /home/xcs/work/UniVPN/common/3rd/TASSL-1.1.1b-V_1.4/include

LIBS +=  /home/xcs/work/UniVPN/common/3rd/TASSL-1.1.1b-V_1.4/libssl.a /home/xcs/work/UniVPN/common/3rd/TASSL-1.1.1b-V_1.4/libcrypto.a  -ldl -lpthread
