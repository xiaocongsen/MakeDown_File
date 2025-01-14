
#include "legacy_provider.h"

/**
 * legacy_gettable_params函数用于获取遗留参数类型
 * 
 * 该函数不需要任何输入参数
 * 
 * 返回值:
 *   返回一个指向OSSL_PARAM类型数组的指针，该数组定义了可供获取的参数类型
 * 
 * 说明:
 *   本函数主要用于兼容性处理，提供一种方式来获取在旧系统或库中定义的参数类型
 *   它不涉及具体的参数值，仅提供参数的类型信息
 */
static const OSSL_PARAM *legacy_gettable_params(void *provctx)
{
    return legacy_param_types;
}

/**
 * 设置遗留提供者参数
 * 
 * 该函数负责根据提供的参数列表，设置遗留提供者的相关信息。它会设置提供者的名称、版本、构建信息以及状态。
 * 
 * @param provctx 提供者上下文，未使用，但可能用于其他目的。
 * @param params 参数数组，用于存储提供者相关信息。
 * 
 * @return 返回1表示成功设置了所有参数，返回0表示至少有一个参数设置失败。
 */
static int legacy_get_params(void *provctx, OSSL_PARAM params[])
{
    OSSL_PARAM *p;

    // 尝试设置提供者名称为"OpenSSL Legacy Provider"
    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_NAME);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, "OpenSSL Legacy Provider"))
        return 0;

    // 尝试设置提供者版本信息
    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_VERSION);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, OPENSSL_VERSION_STR))
        return 0;

    // 尝试设置提供者构建信息
    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_BUILDINFO);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, OPENSSL_FULL_VERSION_STR))
        return 0;

    // 尝试设置提供者运行状态
    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_STATUS);
    if (p != NULL && !OSSL_PARAM_set_int(p, ossl_prov_is_running()))
        return 0;

    // 所有参数设置成功
    return 1;
}


// 定义一个静态常量数组legacy_digests，用于存储过时的摘要算法
static const OSSL_ALGORITHM legacy_digests[] = {
#ifndef OPENSSL_NO_MD2
    ALG(PROV_NAMES_MD2, ossl_md2_functions),
#endif
#ifndef OPENSSL_NO_MD4
    ALG(PROV_NAMES_MD4, ossl_md4_functions),
#endif
#ifndef OPENSSL_NO_MDC2
    ALG(PROV_NAMES_MDC2, ossl_mdc2_functions),
#endif /* OPENSSL_NO_MDC2 */
#ifndef OPENSSL_NO_WHIRLPOOL
    ALG(PROV_NAMES_WHIRLPOOL, ossl_wp_functions),
#endif /* OPENSSL_NO_WHIRLPOOL */
#ifndef OPENSSL_NO_RMD160
    ALG(PROV_NAMES_RIPEMD_160, ossl_ripemd160_functions),
#endif /* OPENSSL_NO_RMD160 */
    { NULL, NULL, NULL }
};

// 定义一个静态常量数组legacy_ciphers，用于存储过时的加密算法
static const OSSL_ALGORITHM legacy_ciphers[] = {
#ifndef OPENSSL_NO_CAST
    ALG(PROV_NAMES_CAST5_ECB, ossl_cast5128ecb_functions),
    ALG(PROV_NAMES_CAST5_CBC, ossl_cast5128cbc_functions),
    ALG(PROV_NAMES_CAST5_OFB, ossl_cast5128ofb64_functions),
    ALG(PROV_NAMES_CAST5_CFB, ossl_cast5128cfb64_functions),
#endif /* OPENSSL_NO_CAST */
#ifndef OPENSSL_NO_BF
    ALG(PROV_NAMES_BF_ECB, ossl_blowfish128ecb_functions),
    ALG(PROV_NAMES_BF_CBC, ossl_blowfish128cbc_functions),
    ALG(PROV_NAMES_BF_OFB, ossl_blowfish128ofb64_functions),
    ALG(PROV_NAMES_BF_CFB, ossl_blowfish128cfb64_functions),
#endif /* OPENSSL_NO_BF */
#ifndef OPENSSL_NO_IDEA
    ALG(PROV_NAMES_IDEA_ECB, ossl_idea128ecb_functions),
    ALG(PROV_NAMES_IDEA_CBC, ossl_idea128cbc_functions),
    ALG(PROV_NAMES_IDEA_OFB, ossl_idea128ofb64_functions),
    ALG(PROV_NAMES_IDEA_CFB, ossl_idea128cfb64_functions),
#endif /* OPENSSL_NO_IDEA */
#ifndef OPENSSL_NO_SEED
    ALG(PROV_NAMES_SEED_ECB, ossl_seed128ecb_functions),
    ALG(PROV_NAMES_SEED_CBC, ossl_seed128cbc_functions),
    ALG(PROV_NAMES_SEED_OFB, ossl_seed128ofb128_functions),
    ALG(PROV_NAMES_SEED_CFB, ossl_seed128cfb128_functions),
#endif /* OPENSSL_NO_SEED */
#ifndef OPENSSL_NO_RC2
    ALG(PROV_NAMES_RC2_ECB, ossl_rc2128ecb_functions),
    ALG(PROV_NAMES_RC2_CBC, ossl_rc2128cbc_functions),
    ALG(PROV_NAMES_RC2_40_CBC, ossl_rc240cbc_functions),
    ALG(PROV_NAMES_RC2_64_CBC, ossl_rc264cbc_functions),
    ALG(PROV_NAMES_RC2_CFB, ossl_rc2128cfb128_functions),
    ALG(PROV_NAMES_RC2_OFB, ossl_rc2128ofb128_functions),
#endif /* OPENSSL_NO_RC2 */
#ifndef OPENSSL_NO_RC4
    ALG(PROV_NAMES_RC4, ossl_rc4128_functions),
    ALG(PROV_NAMES_RC4_40, ossl_rc440_functions),
# ifndef OPENSSL_NO_MD5
    ALG(PROV_NAMES_RC4_HMAC_MD5, ossl_rc4_hmac_ossl_md5_functions),
# endif /* OPENSSL_NO_MD5 */
#endif /* OPENSSL_NO_RC4 */
#ifndef OPENSSL_NO_RC5
    ALG(PROV_NAMES_RC5_ECB, ossl_rc5128ecb_functions),
    ALG(PROV_NAMES_RC5_CBC, ossl_rc5128cbc_functions),
    ALG(PROV_NAMES_RC5_OFB, ossl_rc5128ofb64_functions),
    ALG(PROV_NAMES_RC5_CFB, ossl_rc5128cfb64_functions),
#endif /* OPENSSL_NO_RC5 */
#ifndef OPENSSL_NO_DES
    ALG(PROV_NAMES_DESX_CBC, ossl_tdes_desx_cbc_functions),
    ALG(PROV_NAMES_DES_ECB, ossl_des_ecb_functions),
    ALG(PROV_NAMES_DES_CBC, ossl_des_cbc_functions),
    ALG(PROV_NAMES_DES_OFB, ossl_des_ofb64_functions),
    ALG(PROV_NAMES_DES_CFB, ossl_des_cfb64_functions),
    ALG(PROV_NAMES_DES_CFB1, ossl_des_cfb1_functions),
    ALG(PROV_NAMES_DES_CFB8, ossl_des_cfb8_functions),
#endif /* OPENSSL_NO_DES */
    { NULL, NULL, NULL }
};

// 定义一个静态常量数组legacy_kdfs，用于存储过时的KDF（密钥派生函数）算法
static const OSSL_ALGORITHM legacy_kdfs[] = {
    // 指定PBKDF1算法及其相关函数
    ALG(PROV_NAMES_PBKDF1, ossl_kdf_pbkdf1_functions),
    // 数组结尾标志，用于表示算法列表的结束
    { NULL, NULL, NULL }
};

static const OSSL_ALGORITHM *legacy_query(void *provctx, int operation_id,
                                          int *no_cache)
{
    // 设置no_cache值为0，表示不缓存查询结果
    *no_cache = 0;

    // 根据operation_id的值选择不同的算法列表返回
    switch (operation_id) {
    case OSSL_OP_DIGEST:
        // 如果operation_id为OSSL_OP_DIGEST，返回摘要算法列表
        return legacy_digests;
    case OSSL_OP_CIPHER:
        // 如果operation_id为OSSL_OP_CIPHER，返回加密算法列表
        return legacy_ciphers;
    case OSSL_OP_KDF:
        // 如果operation_id为OSSL_OP_KDF，返回密钥派生函数算法列表
        return legacy_kdfs;
    }

    // 如果operation_id不匹配任何已知操作，则返回NULL
    return NULL;
}

/**
 * legacy_teardown函数用于在传统（legacy）提供者卸载时执行清理操作。
 * 它接收一个提供者上下文指针provctx，并使用它来访问和释放相关资源。
 * 
 * @param provctx 提供者上下文指针，用于访问提供者的资源和状态。
 */
static void legacy_teardown(void *provctx)
{
    // 释放与提供者上下文关联的库上下文。
    OSSL_LIB_CTX_free(PROV_LIBCTX_OF(provctx));
    
    // 释放提供者上下文本身。
    ossl_prov_ctx_free(provctx);
}


/* 我们向核心提供的函数 */
static const OSSL_DISPATCH legacy_dispatch_table[] = {
    /* 当提供者被撤销时调用 */
    { OSSL_FUNC_PROVIDER_TEARDOWN, (void (*)(void))legacy_teardown },
    /* 获取可配置参数的函数指针 */
    { OSSL_FUNC_PROVIDER_GETTABLE_PARAMS, (void (*)(void))legacy_gettable_params },
    /* 获取当前配置参数的函数指针 */
    { OSSL_FUNC_PROVIDER_GET_PARAMS, (void (*)(void))legacy_get_params },
    /* 查询操作支持情况的函数指针 */
    { OSSL_FUNC_PROVIDER_QUERY_OPERATION, (void (*)(void))legacy_query },
    /* 表结束标志 */
    { 0, NULL }
};

/**
 * 初始化自定义的遗留提供者。
 *
 * 该函数为遗留提供者初始化操作，它创建一个新的提供者上下文并设置必要的环境链接，
 * 以便能够响应加密算法操作的调用。
 *
 * @param handle 提供者核心的句柄，用于访问核心功能。
 * @param in 提供者的功能列表，表明该提供者支持哪些加密算法操作。
 * @param out 初始化后，该指针指向提供者功能列表的地址。
 * @param provctx 提供者上下文的指针的地址，用于存储新创建的提供者上下文。
 *
 * @return 成功初始化返回1，否则返回0。
 */
int OSSL_mylegacy_provider_init(const OSSL_CORE_HANDLE *handle,
                       const OSSL_DISPATCH *in,
                       const OSSL_DISPATCH **out,
                       void **provctx)
{
    // 创建一个局部变量，用于存储新的库上下文
    OSSL_LIB_CTX *libctx = NULL;

    // 尝试创建提供者上下文和库上下文。如果任一失败，则执行清理操作。
    if ((*provctx = ossl_prov_ctx_new()) == NULL
        || (libctx = OSSL_LIB_CTX_new_child(handle, in)) == NULL) {
        OSSL_LIB_CTX_free(libctx);
        legacy_teardown(*provctx);
        *provctx = NULL;
        return 0;
    }

    // 成功创建上下文后，将库上下文和句柄设置到提供者上下文中
    ossl_prov_ctx_set0_libctx(*provctx, libctx);
    ossl_prov_ctx_set0_handle(*provctx, handle);

    // 将初始化后使用的派发表返回给调用者
    *out = legacy_dispatch_table;

    // 初始化成功
    return 1;
}
