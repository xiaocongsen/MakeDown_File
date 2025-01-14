# zlog

    一个高性能、线程安全、灵活、概念清晰的纯C日志函数库。

## 编译

    make

## 示例

```pro
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
#QMAKE_CFLAGS += -std=gnu11
SOURCES += \
        main.c

HEADERS += \
    zlog.h

LIBS += $$PWD/libzlog.a
LIBS += -lpthread
```

```c
main.c
#include <stdio.h>

#include "zlog.h"

int main1(int argc, char** argv)
{
    int rc;
    zlog_category_t *c;

    rc = zlog_init("./zlog.conf");
    if (rc) {
    printf("init failed\n");
    return -1;
    }

    c = zlog_get_category("my_cat");
    if (!c) {
    printf("get cat fail\n");
    zlog_fini();
    return -2;
    }

    zlog_info(c, "hello, zlog");

    zlog_fini();

    return 0;
}

#include <stdio.h>

int main(int argc, char** argv)

{

int rc;
rc = dzlog_init("./zlog.conf", "my_cat");

if (rc) {

printf("init failed\n");
return -1;

}
dzlog_debug("debug, zlog");
dzlog_info("info, zlog");
dzlog_notice("notice, zlog");
dzlog_warn("warn, zlog");
dzlog_error("error, zlog");
dzlog_fatal("fatal, zlog");
zlog_fini();

return 0;

}

```

```ini
[global]
# strict init = true
reload conf period = 10M

buffer min = 1024
buffer max = 2MB

#rotate lock file = /tmp/zlog.lock
rotate lock file = self
default format = "[%-6c %-6V %d(%y%d%m %H:%M:%S) %us][%p-%T]%m%n"

file perms = 600
#fsync period = 1K

[levels]
TRACE = 110

[rules]
*.TRACE    		"/dev/tty"

```