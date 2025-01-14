#pragma once

#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include <windows.h>
#include <wincrypt.h>
#include <ncrypt.h>
#include "../cng_provider.h"
#include "debug.h"

/* The primary responsibility of the KEYMGMT module is to hold the provider side key data for
 * the OpenSSL library EVP_PKEY structure.
 * - https://www.openssl.org/docs/man3.1/man7/provider-keymgmt.html */

/* Basic function to create, copy and destroy provider side key objects */
OSSL_FUNC_keymgmt_new_fn cng_keymgmt_new;               //管理密钥对象的创建
OSSL_FUNC_keymgmt_dup_fn cng_keymgmt_dup;               //复制 provider side key 对象
OSSL_FUNC_keymgmt_free_fn cng_keymgmt_free;             //释放 provider side key 对象

/* Create provider side key object from data received from another provider module (like storemgmt) */
OSSL_FUNC_keymgmt_load_fn cng_keymgmt_load;             //从其他模块的数据中加载 provider side key 对象

/* This function is used by OpenSSL to get type information about the current key
 * for example how many bits it's the modulus has */
OSSL_FUNC_keymgmt_get_params_fn cng_keymgmt_get_params; //获取当前密钥的类型信息
/* Although gettable_params was not called during testing, the documentation mandates, that
 * the existence of get_params implies the existence of gettable_params.
 * It should return an array of parameter that get_params() can output */
OSSL_FUNC_keymgmt_gettable_params_fn cng_keymgmt_gettable_params; //获取可获得的参数列表
/* Similar to get_params but answers questions about data subsets, for example if the key contains its private part */
OSSL_FUNC_keymgmt_has_fn cng_keymgmt_has;               //检查密钥是否包含特定部分

/* For exporting public parts of (or even whole) keys to OpenSSL format */
OSSL_FUNC_keymgmt_export_fn cng_keymgmt_export;         //导出密钥的公有部分或全部到 OpenSSL 格式

//Things below did not get called during development of the provider in our scenario
/* Match function is usually only needed when your provider does not allow exporting keys.
 * OpenSSL can export the private part of your key and check it itself against another. And since
 * equal public keys should mean equal private keys it is most of the time sufficient */
//OSSL_FUNC_keymgmt_match_fn cng_keymgmt_match;          //比较密钥是否匹配
/* This function should validate the key object passed to it. */
//OSSL_FUNC_keymgmt_validate_fn cng_keymgmt_validate;    //验证密钥对象的有效性

/* Return parameters that export() callback can receive, not actually called during testing.
 * Only needed for providers compatible with OpenSSL from 3.0.0 to 3.2.0 excluded. In 3.2.0
 * this has been superseded by export_types_ex()
 * Either this or its _ex variant is mandatory by https://www.openssl.org/docs/manmaster/man7/provider-keymgmt.html */
OSSL_FUNC_keymgmt_export_types_fn cng_keymgmt_export_types; //返回 export() 回调可以接收的参数类型

/* The same thing as export_types(), but for OpenSSL 3.2.0+ */
//OSSL_FUNC_keymgmt_export_types_ex_fn cng_keymgmt_export_types_ex; //导出类型扩展函数，用于 OpenSSL 3.2.0 及以上版本

/* This function is purely advisory and optional */
//OSSL_FUNC_keymgmt_query_operation_name_fn cng_keymgmt_query_operation_name; //查询操作名称，可选

/* These functions are needed when implementing key generation */
//OSSL_FUNC_keymgmt_gen_init_fn cng_keymgmt_gen_init;     //初始化密钥生成
//OSSL_FUNC_keymgmt_gen_set_template_fn cng_keymgmt_gen_set_template; //设置密钥生成模板
//OSSL_FUNC_keymgmt_gen_set_params_fn cng_keymgmt_gen_set_params;     //设置密钥生成参数
//OSSL_FUNC_keymgmt_gen_settable_params_fn cng_keymgmt_gen_settable_params; //获取可设置的密钥生成参数
//OSSL_FUNC_keymgmt_gen_fn cng_keymgmt_gen;               //执行密钥生成
//OSSL_FUNC_keymgmt_gen_cleanup_fn cng_keymgmt_gen_cleanup;           //清理密钥生成资源

/* These functions are needed when you want to import keys from other providers or OpenSSL to your provider */
//OSSL_FUNC_keymgmt_import_fn cng_keymgmt_import;         //导入密钥
//OSSL_FUNC_keymgmt_import_types_fn cng_keymgmt_import_types; //获取支持导入的密钥类型

/* 定义我们的提供方密钥对象数据类型 */
typedef struct s_cng_keymgmt_keydata {
    NCRYPT_KEY_HANDLE windows_key_handle; /* Windows 密钥句柄 */
} T_CNG_KEYMGMT_KEYDATA;

/* CNG 密钥管理功能的函数指针数组 */
const OSSL_DISPATCH cng_keymgmt_functions[11];

/** 定义我们通过 keymgmt_get_params() 提供的参数类型 */
static const OSSL_PARAM cng_keymgmt_param_types[] = {
        OSSL_PARAM_DEFN(OSSL_PKEY_PARAM_BITS, OSSL_PARAM_INTEGER, NULL, 0),
        OSSL_PARAM_DEFN(OSSL_PKEY_PARAM_SECURITY_BITS, OSSL_PARAM_INTEGER, NULL, 0),
        OSSL_PARAM_DEFN(OSSL_PKEY_PARAM_MAX_SIZE, OSSL_PARAM_INTEGER, NULL, 0),
        OSSL_PARAM_END
};

/* 定义支持的算法及其属性和函数指针 */
static const OSSL_ALGORITHM cng_keymgmt[] = {
        /*
         * 结构如下：
         * [OpenSSL 通过其获取此实现的名称，用冒号分隔],
         * [此实现的属性，如 https://www.openssl.org/docs/manmaster/man7/property.html 中所述],
         * [实际的函数指针], [可选描述]
         */
        {"rsaEncryption", CNG_DEFAULT_ALG_PROPERTIES,
                cng_keymgmt_functions, "CNG Provider RSA Implementation"},
        /* 其他算法名称包括（但不限于）ED25519、ED448、EC:id-ecPublicKey、DSA、X25519 */
        {NULL, NULL, NULL}
};