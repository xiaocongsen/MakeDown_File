# MakeFile学习

```makefile
#gcc 编译器，如果要在ARM上应用就需要交叉编译器。
CC=g++

#参数
CCFLAGS= -g -Wall --std=c++11

#当前文件夹下所有.cpp文件，如果还有其他.cpp文件，也放到括号里面 
SRC:= $(wildcard  *.cpp )

#.c文件链接成的.o文件
OBJS:=$(patsubst %.cpp,%.o,$(SRC))

#所需要的头文件目录
INCLDPATH=-I.

#RC链接成静态库
AR:=ar rc

#静态库所在目录和名字
LIB_STATIC_OBJECT = ./so/libmyBase.a

#动态库所在目录和名字
LIB_DYNAMIC_OBJECT = ./so/libmyBase.so

.PHONY:all
all:$(LIB_DYNAMIC_OBJECT)
$(LIB_DYNAMIC_OBJECT):$(OBJS)
	$(CC) -shared -o $(LIB_DYNAMIC_OBJECT) $(OBJS)
%.o : %.cpp  
	$(CC) -fPIC  -c $(CCFLAGS) $(INCLDPATH) -o $@ $<


static:$(LIB_STATIC_OBJECT)
$(LIB_STATIC_OBJECT):$(OBJS)
	$(AR)   $(LIB_STATIC_OBJECT) $(OBJS)
# %.o : %.cpp  
# 	$(CC) -c $(CCFLAGS) $(INCLDPATH) -o $@ $<           #只能存在一条 以最后一条为准

clean:
	rm  -rf  $(LIB_STATIC_OBJECT) $(LIB_DYNAMIC_OBJECT) $(OBJS)
```



## GNU C编译器的gnu11和c11
	国际标准组织发布c11后，gnu为自己的编译器发布两种标准gnu11和c11
	gnu11：带gnu c扩展的c11标准，如果你的代码包含了typeof，__attribute__等等gnu的扩展，就必须用这个。
	c11：这个就是纯c11的标准，不带gnu扩展。

	可以在Makefile中声明：

>	CFLAGS=-std=gnu11 -g -Wall

	或者，纯标准的c11，玩linux的要慎用，因为linux代码到处都是gnu的痕迹哦，哈哈

>	CFLAGS=-std=c11 -g -Wall
