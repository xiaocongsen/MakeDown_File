#include "debug.h"
#include <stdio.h>

#define DEBUG_LEVEL DEBUG_ALL

typedef void (*LogFuncPtr)(const char*);
static LogFuncPtr g_LogFuncPtr = NULL;

void RegisterLogFunction(LogFuncPtr logFunc) {
    g_LogFuncPtr = logFunc;
}

void cb_printf(const char *string) {
    if(g_LogFuncPtr != NULL)
        g_LogFuncPtr(string);
}

/**
 * 启动OpenSSL追踪
 * 
 * 本函数初始化OpenSSL的追踪功能，为指定的追踪类别设置输出通道本函数通过向标准错误输出设置
 * 一系列追踪类别，以跟踪OpenSSL库内部的运行情况每个追踪类别都有独立的输出BIO（生物输入/输出）
 * 用于精细控制追踪信息的输出
 * 
 * 注意：此函数假设全局变量"openssl_enabled_tracing_categories"已经定义，并包含需要启用的追踪类别
 */
void start_tracing() {
    /* 定义启用追踪的OpenSSL类别数组 */
    static const int openssl_enabled_tracing_categories[] = {
                                                OSSL_TRACE_CATEGORY_CONF,
                                                OSSL_TRACE_CATEGORY_STORE,
                                                // OSSL_TRACE_CATEGORY_TLS,
                                                // OSSL_TRACE_CATEGORY_DECODER
    };

    /* 计算追踪类别的数量 */
    int categories_count = sizeof(openssl_enabled_tracing_categories) / sizeof(openssl_enabled_tracing_categories[0]);
    for (int i = 0; i < categories_count; i++) {
        /* 为每个追踪类别创建一个指向标准错误的BIO */
        BIO *berr = BIO_new_fp(stderr, BIO_NOCLOSE | BIO_FP_TEXT);
        /* 设置追踪类别的输出通道，传递所有权，不直接释放 */
        if (!OSSL_trace_set_channel(openssl_enabled_tracing_categories[i], berr)) {
            cb_printf("Error setting OpenSSL tracing");
            exit(0);
        }

    }

    // 以下代码行可用于设置不同追踪类别的前缀和后缀，以提供更详细的追踪信息
    // OSSL_trace_set_prefix(OSSL_TRACE_CATEGORY_CONF, "BEGIN TRACE[CONF]");
    // OSSL_trace_set_suffix(OSSL_TRACE_CATEGORY_CONF, "END TRACE[CONF]");
    // OSSL_trace_set_prefix(OSSL_TRACE_CATEGORY_STORE, "BEGIN TRACE[STORE]");
    // OSSL_trace_set_suffix(OSSL_TRACE_CATEGORY_STORE, "END TRACE[STORE]");
    // OSSL_trace_set_prefix(OSSL_TRACE_CATEGORY_TLS, "BEGIN TRACE[TLS]");
    // OSSL_trace_set_suffix(OSSL_TRACE_CATEGORY_TLS, "END TRACE[TLS]");
    // OSSL_trace_set_prefix(OSSL_TRACE_CATEGORY_DECODER, "BEGIN TRACE[DECODER]");
    // OSSL_trace_set_suffix(OSSL_TRACE_CATEGORY_DECODER, "END TRACE[DECODER]");
}

