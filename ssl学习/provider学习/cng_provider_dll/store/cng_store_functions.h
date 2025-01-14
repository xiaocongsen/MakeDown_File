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

/*Basic store functions */
OSSL_FUNC_store_open_fn cng_store_open;
//OSSL_FUNC_store_attach_fn cng_store_attach;
OSSL_FUNC_store_load_fn cng_store_load;
OSSL_FUNC_store_close_fn cng_store_close;
OSSL_FUNC_store_eof_fn cng_store_eof;

/* For letting core set parameters of our store */
OSSL_FUNC_store_settable_ctx_params_fn cng_store_settable_ctx_params;
OSSL_FUNC_store_set_ctx_params_fn cng_store_set_ctx_params;

/* For supported types export from our provider back to OpenSSL core */
//OSSL_FUNC_store_export_object_fn cng_store_export_object;

const OSSL_DISPATCH cng_store_functions[9];

static const OSSL_ALGORITHM cng_store[] = {
        {"cng", CNG_DEFAULT_ALG_PROPERTIES, cng_store_functions, "CNG Provider Implementation"},
        {NULL, NULL, NULL}
};

/* Our custom providers store context */
typedef struct cng_store_ctx {
    /* 在获取参数时允许更快的响应 */
    int expected_parameter_type;

    /* Windows证书存储句柄 */
    HCERTSTORE windows_certificate_store;
    /* 从Windows证书存储区加载的最后一个证书 */
    PCCERT_CONTEXT prev_cert_ctx;
    /* 我们证书库的当前EOF状态 */
    int cert_store_eof;

    /* 从Windows密钥存储提供程序加载的最后一个私钥 */
    PCCERT_CONTEXT prev_key_cert_ctx;
    /* 我们提供商格式的最后一个私钥 */
    T_CNG_KEYMGMT_KEYDATA * key;
    /* 密钥存储提供商的当前EOF状态 */
    int priv_key_store_eof;

    /* OpenSSL核心请求的参数*/
    char *propquery;

    const char * windows_system_store_name;
} T_CNG_STORE_CTX;
