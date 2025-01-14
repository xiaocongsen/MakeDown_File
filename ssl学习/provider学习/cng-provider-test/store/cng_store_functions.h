#pragma once

#include <string.h>
#include <openssl/core_object.h>
#include <openssl/core_names.h>
#include <openssl/x509.h>
#include <openssl/store.h>
#include <windows.h>
#include <wincrypt.h>
#include <ncrypt.h>
#include "../cng_provider.h"
#include "../keymgmt/cng_keymgmt_functions.h"

/* 基本存储功能函数 */
/* 打开存储的函数 */
OSSL_FUNC_store_open_fn cng_store_open;
/* 附加存储的函数 (注释掉，可能未使用) */
//OSSL_FUNC_store_attach_fn cng_store_attach;
/* 加载存储的函数 */
OSSL_FUNC_store_load_fn cng_store_load;
/* 关闭存储的函数 */
OSSL_FUNC_store_close_fn cng_store_close;
/* 检查存储是否到达EOF(文件末尾)的函数 */
OSSL_FUNC_store_eof_fn cng_store_eof;

/* 用于让核心设置我们存储的参数 */
/* 获取可设置的上下文参数的函数 */
OSSL_FUNC_store_settable_ctx_params_fn cng_store_settable_ctx_params;
/* 设置上下文参数的函数 */
OSSL_FUNC_store_set_ctx_params_fn cng_store_set_ctx_params;

/* 用于从我们的提供程序返回到OpenSSL核心的导出对象 (注释掉，可能未使用) */
//OSSL_FUNC_store_export_object_fn cng_store_export_object;

/* 存储功能函数数组 */
const OSSL_DISPATCH cng_store_functions[9];

/* 定义支持的算法 */
static const OSSL_ALGORITHM cng_store[] = {
        /* 算法名称、属性、相关函数和描述 */
        {"cng", CNG_DEFAULT_ALG_PROPERTIES, cng_store_functions, "CNG Provider Implementation"},
        {NULL, NULL, NULL}
};

/* 我们的自定义提供程序的存储上下文结构体 */
typedef struct cng_store_ctx {
    /* 允许更快的参数响应时间 */
    int expected_parameter_type;

    /* Windows证书存储句柄 */
    HCERTSTORE windows_certificate_store;
    /* 从Windows证书存储加载的最后一个证书 */
    PCCERT_CONTEXT prev_cert_ctx;
    /* 我们证书库的当前EOF状态 */
    int cert_store_eof;

    /* 从Windows密钥存储提供程序加载的最后一个私钥 */
    PCCERT_CONTEXT prev_key_cert_ctx;
    /* 我们提供程序格式的最后一个私钥 */
    T_CNG_KEYMGMT_KEYDATA * key;
    /* 密钥存储提供程序的当前EOF状态 */
    int priv_key_store_eof;

    /* OpenSSL核心请求的参数 */
    char *propquery;

    /* Windows系统的存储名称 */
    const char * windows_system_store_name;
} T_CNG_STORE_CTX;