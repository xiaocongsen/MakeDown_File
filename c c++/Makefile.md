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