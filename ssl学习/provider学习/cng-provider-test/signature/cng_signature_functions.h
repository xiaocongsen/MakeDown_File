#pragma once

#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include <windows.h>
#include <wincrypt.h>
#include <ncrypt.h>
#include "../cng_provider.h"
#include "../keymgmt/cng_keymgmt_functions.h"

/* 定义CNG签名上下文结构体，用于存储签名操作的相关信息 */
typedef struct s_cng_signature_ctx {
    /* 指向提供者的上下文 */
    T_CNG_PROVIDER_CTX *provctx;
    /* 实现选择时的属性查询字符串 */
    const char *propq;
    /* Windows算法标识符，Unicode字符串 */
    LPCWSTR alg_identifier;
    /* 指向用于签名的提供者密钥对象 */
    T_CNG_KEYMGMT_KEYDATA *key;
    /* Windows哈希句柄 */
    BCRYPT_HASH_HANDLE hash_handle;
    /* Windows签名哈希的标志 */
    ULONG sign_hash_flags;
    /* Windows PSS盐长度 */
    ULONG pss_salt_len;
} T_CNG_SIGNATURE_CTX;

/* 声明创建、克隆和销毁签名上下文的基本函数 */
OSSL_FUNC_signature_newctx_fn cng_signature_newctx;
OSSL_FUNC_signature_dupctx_fn cng_signature_dupctx;
OSSL_FUNC_signature_freectx_fn cng_signature_freectx;

/* 声明用于数据摘要签名的函数 */
OSSL_FUNC_signature_digest_sign_init_fn cng_signature_digest_sign_init;
OSSL_FUNC_signature_digest_sign_update_fn cng_signature_digest_sign_update;
OSSL_FUNC_signature_digest_sign_final_fn cng_signature_digest_sign_final;
/* 可能的单步数据摘要签名函数实现 */
//OSSL_FUNC_signature_digest_sign_fn cng_signature_digest_sign;

OSSL_FUNC_signature_set_ctx_params_fn cng_signature_set_ctx_params;
OSSL_FUNC_signature_settable_ctx_params_fn cng_signature_settable_ctx_params;

/* 以下是实现客户证书发送的最小需求，其余函数应分组实现 */

/* 签名函数，_sign_init()初始化实际签名操作，由_sign()完成 */
//OSSL_FUNC_signature_sign_init_fn cng_signature_sign_init;
//OSSL_FUNC_signature_sign_fn cng_signature_sign;

/* 验证签名的函数，与上述函数类似 */
//OSSL_FUNC_signature_verify_init_fn cng_signature_verify_init;
//OSSL_FUNC_signature_verify_fn cng_signature_verify;

/* 从签名中恢复消息的函数，同样分为初始化和实际恢复操作 */
//OSSL_FUNC_signature_verify_recover_init_fn cng_signature_verify_recover_init;
//OSSL_FUNC_signature_verify_recover_fn cng_signature_verify_recover;

/* 验证摘要签名的函数，与摘要签名函数类似 */
//OSSL_FUNC_signature_digest_verify_init_fn cng_signature_digest_verify_init;
//OSSL_FUNC_signature_digest_verify_update_fn cng_signature_digest_verify_update;
//OSSL_FUNC_signature_digest_verify_final_fn cng_signature_digest_verify_final;
/* 上述功能的单步验证函数 */
//OSSL_FUNC_signature_digest_verify_fn cng_signature_digest_verify;

/* 获取当前签名类型信息的函数，例如填充类型 */
//OSSL_FUNC_signature_get_ctx_params_fn cng_signature_get_ctx_params;
/* 应返回get_params()可以输出的参数数组 */
//OSSL_FUNC_signature_gettable_ctx_params_fn cng_signature_gettable_ctx_params;

/* 与给定签名相关联的消息摘要函数的获取和设置 */
//OSSL_FUNC_signature_get_ctx_md_params_fn cng_signature_get_ctx_md_params;
//OSSL_FUNC_signature_gettable_ctx_md_params_fn cng_signature_gettable_ctx_md_params;
//OSSL_FUNC_signature_set_ctx_md_params_fn cng_signature_set_ctx_md_params;
//OSSL_FUNC_signature_settable_ctx_md_params_fn cng_signature_settable_ctx_md_params;

/* CNG签名函数的分派表 */
const OSSL_DISPATCH cng_signature_functions[9];

/* 定义支持的签名算法及其属性 */
static const OSSL_ALGORITHM cng_signature[] = {
    /* 结构体格式：
     * [OpenSSL获取此实现的名称，由冒号分隔],
     * [此实现的属性，参见https://www.openssl.org/docs/manmaster/man7/property.html],
     * [实际函数指针], [可选描述]
     */
    {"RSA:rsaEncryption", CNG_DEFAULT_ALG_PROPERTIES, cng_signature_functions, "CNG RSA signature functions"},
    /* 其他算法名称包括（但不限于）ED25519, ED448, EC:id-ecPublicKey, DSA, X25519 */
    {NULL, NULL, NULL}
};