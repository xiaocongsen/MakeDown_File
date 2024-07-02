# Unicode
> 是一种字符编码标准，用于表示世界上所有语言的字符集,常见的 Unicode 编码方式有 UTF-8、UTF-16 和 UTF-32 等。win一般使用UTF-16编码的宽字符。UTF-16是一种变长编码，使用1或2个16位值来表示一个字符，可以表示Unicode字符集中的所有字符。

# ASCII
> ASCII是最早的字符编码标准之一，它只能表示128个字符，包括英文字母、数字和一些标点符号，每个字符占用一个字节。ASCII编码通常用于英文字符的传输和存储。

# ANSI
> 是一种字符编码标准，只能表示有限的字符集，它定义了几种不同的编码方式，用于表示英语和西欧语言中使用的字符集。ANSI编码方式是由不同的国家和地区制定的，不同的语言和地区可能会使用不同的ANSI编码，每个字符占用一个字节或两个字节



# win函数结尾的A和W说明
> CertGetNameString 有两个版本：
1. CertGetNameStringA 用于 ANSI 字符集
2. CertGetNameStringW 用于 Unicode 字符集。



# 字符集转换
```c

//LPCSTR 等于char*
//LPWSTR 等于WCHAR wchar_t
char szContainerA[256];
WideCharToMultiByte(CP_ACP, 0, L"Hello", -1, szContainerA, sizeof(szContainerA), NULL, NULL);    //转换成char*
LPCSTR szContainerAStr = szContainerA;


// 将 char* 类型的字符串转换为 LPWSTR 类型的字符串
int len = MultiByteToWideChar(CP_ACP, 0, "Hello", -1, NULL, 0); //获取转换后的长度
LPWSTR str3 = (LPWSTR)LocalAlloc(LPTR, len);
MultiByteToWideChar(CP_ACP, 0, "Hello", -1, str3, len);     //转换成LPWSTR类型字符串
LocalFree(str3);

```


```c

// 函数：将 GBK 编码的 char 字符串转换为 Windows-1252 编码的 char 字符串
char* gbk_to_windows1252(const char* gbk_str) {
    // 计算转换为宽字符所需的缓冲区大小
    int wide_char_len = MultiByteToWideChar(CP_ACP, 0, gbk_str, -1, NULL, 0);
    if (wide_char_len == 0) {
        return NULL;
    }

    // 分配内存
    wchar_t* wide_str = (wchar_t*)malloc(wide_char_len * sizeof(wchar_t));
    if (wide_str == NULL) {
        return NULL;
    }

    // 将 GBK 转换为宽字符（Unicode）
    MultiByteToWideChar(CP_ACP, 0, gbk_str, -1, wide_str, wide_char_len);

    // 计算转换为 Windows-1252 所需的缓冲区大小
    int windows1252_len = WideCharToMultiByte(1252, 0, wide_str, -1, NULL, 0, NULL, NULL);
    if (windows1252_len == 0) {
        free(wide_str);
        return NULL;
    }

    // 分配内存
    char* windows1252_str = (char*)malloc(windows1252_len);
    if (windows1252_str == NULL) {
        free(wide_str);
        return NULL;
    }

    // 将宽字符（Unicode）转换为 Windows-1252
    WideCharToMultiByte(1252, 0, wide_str, -1, windows1252_str, windows1252_len, NULL, NULL);

    // 释放宽字符字符串的内存
    free(wide_str);

    return windows1252_str;
}

// 函数：将 GBK 编码的 char 字符串转换为 Unicode 编码的 wchar_t 字符串
wchar_t* gbk_to_unicode(const char* gbk_str) {
    // 计算转换为宽字符所需的缓冲区大小
    int wide_char_len = MultiByteToWideChar(CP_ACP, 0, gbk_str, -1, NULL, 0);
    if (wide_char_len == 0) {
        return NULL;
    }

    // 分配内存
    wchar_t* wide_str = (wchar_t*)malloc(wide_char_len * sizeof(wchar_t));
    if (wide_str == NULL) {
        return NULL;
    }

    // 将 GBK 转换为宽字符（Unicode）
    MultiByteToWideChar(CP_ACP, 0, gbk_str, -1, wide_str, wide_char_len);

    return wide_str;
}

// 函数：将 GBK 编码的 char 字符串转换为 UTF-8 编码的 char 字符串
char* gbk_to_utf8(const char* gbk_str) {
    // 计算转换为宽字符所需的缓冲区大小
    int wide_char_len = MultiByteToWideChar(CP_ACP, 0, gbk_str, -1, NULL, 0);
    if (wide_char_len == 0) {
        return NULL;
    }

    // 分配内存
    wchar_t* wide_str = (wchar_t*)malloc(wide_char_len * sizeof(wchar_t));
    if (wide_str == NULL) {
        return NULL;
    }

    // 将 GBK 转换为宽字符
    MultiByteToWideChar(CP_ACP, 0, gbk_str, -1, wide_str, wide_char_len);

    // 计算转换为 UTF-8 所需的缓冲区大小
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide_str, -1, NULL, 0, NULL, NULL);
    if (utf8_len == 0) {
        free(wide_str);
        return NULL;
    }

    // 分配内存
    char* utf8_str = (char*)malloc(utf8_len);
    if (utf8_str == NULL) {
        free(wide_str);
        return NULL;
    }

    // 将宽字符转换为 UTF-8
    WideCharToMultiByte(CP_UTF8, 0, wide_str, -1, utf8_str, utf8_len, NULL, NULL);

    // 释放宽字符字符串的内存
    free(wide_str);

    return utf8_str;
}

// 函数：将 char 字符串转换为 wchar_t 字符串
wchar_t *char_to_wchar(const char *str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    wchar_t *wstr = (wchar_t *)malloc(size_needed * sizeof(wchar_t));
    if (wstr == NULL) {
        return NULL;
    }
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, size_needed);
    return wstr;
}

```