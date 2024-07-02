## smali 语法学习

``` Smali
.method public static add(II)I      # 方法名为add，参数为两个int，返回值为int
    .registers 3                    # 寄存器数量为3
    .param p0, "a"                  # 表示第一个参数的名称为“a”，存储在寄存器p0中
    .param p1, "b"                  # 表示第二个参数的名称为“b”，存储在寄存器p1中

    add-int v0, p0, p1              # 执行相加操作,将结果存储在寄存器v0中
    return v0                       # 将结果从寄存器v0中返回。 
.end method                         # 该行表示该方法的结束
```


# 对照
``` Smali
.class public Landroid/acquisitionservice/BYDAcquisitionClient;                         # 类名为BYDAcquisitionClient
.super Ljava/lang/Object;                                                               # 父类为Object
.source "BYDAcquisitionClient.java"                                                     # 源文件名为BYDAcquisitionClient.java


# direct methods                                                                        # 构造函数                                                  
.method public constructor <init>(Landroid/content/Context;Ljava/lang/String;)V         # 构造函数名为<init>，参数为Context和String，返回值为void
    .locals 0                                                                           # 寄存器数量为0

    .line 6                                                                             # 表示该行代码在源文件的第6行
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V                                   # 调用父类的构造函数

    .line 7                                                                             # 表示该行代码在源文件的第7行
    new-instance p1, Ljava/lang/RuntimeException;                                       # 创建一个RuntimeException对象，存储在寄存器p1中

    const-string p2, "Stub!"                                                            # 将字符串"Stub!"存储在寄存器p2中

    invoke-direct {p1, p2}, Ljava/lang/RuntimeException;-><init>(Ljava/lang/String;)V   # 调用RuntimeException的构造函数，将p2作为参数传入

    throw p1                                                                            # 抛出p1中的异常
.end method                                                                             # 该行表示该方法的结束


# virtual methods                                                                       # 普通方法
.method public sendBuffer(I[B)V                                                         # 方法名为sendBuffer，参数为int和byte数组，返回值为void
    .locals 0                                                                           # 寄存器数量为0

    .line 11                                                                            # 表示该行代码在源文件的第11行
    new-instance p1, Ljava/lang/RuntimeException;                                       # 创建一个RuntimeException对象，存储在寄存器p1中

    const-string p2, "Stub!"                                                            # 将字符串"Stub!"存储在寄存器p2中

    invoke-direct {p1, p2}, Ljava/lang/RuntimeException;-><init>(Ljava/lang/String;)V   # 调用RuntimeException的构造函数，将p2作为参数传入

    throw p1                                                                            # 抛出p1中的异常
.end method                                                                             # 该行表示该方法的结束

```

``` java
package android.acquisitionservice;

import android.content.Context;

public class BYDAcquisitionClient {
    
    public BYDAcquisitionClient(Context context, String str) {
        throw new RuntimeException("Stub!");
    }

    public void sendBuffer(int i, byte[] bArr) {
        throw new RuntimeException("Stub!");
    }
}
```