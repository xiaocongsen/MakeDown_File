# SSL出错源码位置获取
``` c
    SSL_load_error_strings();           //加载错误码对应信息
    unsigned long ret = ERR_get_error(); //获取错误码
    char *szErrMsg[1024] = {0};
    ERR_error_string(ret,szErrMsg); //获取错误信息 格式： error:errld:库:函数:原因
```
