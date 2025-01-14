#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include "cng_signature_functions.h"
#include <ntstatus.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include "debug.h"

/**
 * 创建一个新的签名上下文
 * 
 * 该函数用于初始化签名操作所需的上下文结构体。签名上下文包含了进行签名操作所需要的各种参数，
 * 包括哈希句柄、算法标识符、密钥、属性字符串等。此函数返回的上下文指针在后续的签名操作中会被用到。
 * 
 * @param provctx 提供者上下文指针，通常由密码服务提供者（CSP）使用
 * @param propq 属性字符串，用于指定签名操作的特定属性
 * 
 * @return 返回一个指向签名上下文的指针，如果内存分配失败则返回NULL
 */
void *cng_signature_newctx(void *provctx, const char *propq) {
    cb_printf("SIGNATURE> cng_signature_newctx");   //xcs48 xcs51 xcs56
    // 分配签名上下文结构体的内存
    T_CNG_SIGNATURE_CTX *sig_ctx = malloc(sizeof(T_CNG_SIGNATURE_CTX));
    if (sig_ctx == NULL){ return NULL; }  // 如果内存分配失败，返回NULL

    // 初始化签名上下文中的成员变量
    sig_ctx->hash_handle = NULL;
    sig_ctx->alg_identifier = NULL;
    sig_ctx->key = NULL;
    sig_ctx->propq = propq;
    sig_ctx->provctx = provctx;
    sig_ctx->sign_hash_flags = 0;
    sig_ctx->pss_salt_len = 0;

    // 返回签名上下文指针
    return sig_ctx;
}

/**
 * cng_signature_dupctx函数用于复制签名上下文。
 * 这包括复制现有的签名上下文的所有必要元素，如散列句柄、算法标识符、密钥和签名配置。
 *
 * @param ctx 指向T_CNG_SIGNATURE_CTX结构的指针，表示待复制的签名上下文。
 * @return 返回指向新分配的T_CNG_SIGNATURE_CTX结构的指针，表示复制的签名上下文。如果复制失败，返回NULL。
 */
void *cng_signature_dupctx(void *ctx) {
    cb_printf("SIGNATURE> cng_signature_dupctx");   //xcs55
    T_CNG_SIGNATURE_CTX *old_sig_ctx = ctx;
    T_CNG_SIGNATURE_CTX *new_sig_ctx = cng_signature_newctx(old_sig_ctx->provctx, old_sig_ctx->propq);

    /* 复制散列句柄 */
    NTSTATUS s = BCryptDuplicateHash(old_sig_ctx->hash_handle, &new_sig_ctx->hash_handle, NULL, 0, 0);
    if (s != STATUS_SUCCESS) { return NULL; }

    /* 复制算法提供者 */
    new_sig_ctx->alg_identifier = old_sig_ctx->alg_identifier;

    /* 复制密钥数据 */
    new_sig_ctx->key = cng_keymgmt_dup(old_sig_ctx->key, OSSL_KEYMGMT_SELECT_ALL);
    if (old_sig_ctx->key != NULL && new_sig_ctx->key == NULL) { return NULL; }

    /* 复制BCryptSignHash所需的信息 */
    new_sig_ctx->sign_hash_flags = old_sig_ctx->sign_hash_flags;
    new_sig_ctx->pss_salt_len = old_sig_ctx->pss_salt_len;

    return new_sig_ctx;
}

/**
 * cng_signature_freectx 函数
 * 释放签名操作的上下文资源
 * 
 * 参数:
 * ctx - 签名操作的上下文指针
 * 
 * 该函数的主要作用是释放由 cng_signature_initctx 函数初始化的签名操作上下文
 * 它通过调用 BCryptDestroyHash 来关闭哈希句柄，并使用 free 函数释放上下文内存
 */
void cng_signature_freectx(void *ctx) {
    cb_printf("SIGNATURE> cng_signature_freectx");  //xcs50 xcs60

    // 注释: 将传入的 void 指针转换为具体的上下文类型指针，以便访问其成员
    T_CNG_SIGNATURE_CTX *sig_ctx = ctx;

    // 注释: 销毁哈希句柄，清理资源，避免内存泄漏
    BCryptDestroyHash(sig_ctx->hash_handle);

    // 注释: 释放签名操作上下文的内存
    free(sig_ctx);
}

/**
 * 将OpenSSL的摘要算法名称转换为BCrypt的摘要算法名称。
 * 
 * 此函数的目的是在两种不同的加密库（OpenSSL和BCrypt）之间进行算法名称的映射。
 * 这对于在不同加密标准下实现密码学操作的互操作性是必要的。
 * 
 * @param sig_ctx 指向签名上下文的指针，用于算法上下文配置。当前未使用，未来可能支持特定算法的配置。
 * @param ossl_mdname 源摘要算法的名称（OpenSSL格式）。
 * @param bcrypt_mdname 目标摘要算法的名称（BCrypt格式）的指针。
 * 
 * @return 成功时返回非零值，失败时返回零。失败的原因可能是输入参数无效或指定的摘要算法不被支持。
 */
int ossl_digest_name_to_bcrypt_digest_name(T_CNG_SIGNATURE_CTX *sig_ctx, const char *ossl_mdname, LPCWSTR *bcrypt_mdname) {
    /* 检查输入参数是否有效。如果任何参数为NULL，则返回0表示失败。 */
    if (sig_ctx == NULL || ossl_mdname == NULL || bcrypt_mdname == NULL) { return 0; }
    /* 初始化目标算法名称指针为NULL。 */
    *bcrypt_mdname = 0x00;
    
    /* 通过OpenSSL的EVP_MD_fetch函数获取指定名称的摘要算法对象。 */
    const EVP_MD *md = EVP_MD_fetch(NULL, ossl_mdname, NULL);
    /* 如果获取的算法对象为NULL，则表示指定的算法不被支持，返回0表示失败。 */
    if (md == NULL) { return 0; }

    /* 根据获取的算法对象，判断其是否与特定的SHA2系列算法相同。 */
    /* 如果是，则将目标算法名称指针设置为相应的BCrypt算法名称。 */
    if (EVP_MD_is_a(md, OSSL_DIGEST_NAME_SHA2_256)) { *bcrypt_mdname = BCRYPT_SHA256_ALGORITHM; }
    if (EVP_MD_is_a(md, OSSL_DIGEST_NAME_SHA2_384)) { *bcrypt_mdname = BCRYPT_SHA384_ALGORITHM; }
    if (EVP_MD_is_a(md, OSSL_DIGEST_NAME_SHA2_512)) { *bcrypt_mdname = BCRYPT_SHA512_ALGORITHM; }

    /* 如果目标算法名称成功映射，则返回非零值表示成功，否则返回零表示失败。 */
    return *bcrypt_mdname != 0x00;
}

/**
 * 初始化签名哈希算法上下文.
 * 
 * 该函数根据给定的哈希算法名称初始化签名上下文，并为后续的签名操作做准备.
 * 
 * @param ctx 指向签名上下文的指针.
 * @param mdname 哈希算法的名称.
 * @param provkey 提供的密钥，用于后续的签名操作.
 * @param params 额外的参数，目前未使用.
 * 
 * @return 如果初始化成功，返回1；否则返回0.
 */
int cng_signature_digest_sign_init(void *ctx, const char *mdname,
                                   void *provkey,
                                   const OSSL_PARAM params[]) {
    cb_printf("SIGNATURE> cng_signature_digest_sign_init"); // 调试打印，标志着签名初始化的开始
    T_CNG_SIGNATURE_CTX *sig_ctx = (T_CNG_SIGNATURE_CTX *) ctx;
    sig_ctx->key = provkey;

    // 根据提供的哈希算法名称设置算法标识符
    if (!ossl_digest_name_to_bcrypt_digest_name(sig_ctx, mdname, &sig_ctx->alg_identifier)) {
        cb_printf("SIGN> Unsupported hashing algorithm"); // 调试打印，标识不支持的哈希算法
        return 0;
    }

    BCRYPT_ALG_HANDLE alg_prov_handle;
    NTSTATUS s = BCryptOpenAlgorithmProvider(&alg_prov_handle, sig_ctx->alg_identifier, NULL, 0);
    if (s != STATUS_SUCCESS) { return 0; }

    s = BCryptCreateHash(alg_prov_handle, &sig_ctx->hash_handle, NULL, 0, NULL, 0, 0);

    BCryptCloseAlgorithmProvider(alg_prov_handle, 0);
    return s == STATUS_SUCCESS; // 如果创建哈希对象成功，则返回1
}

/**
 * 使用签名上下文对数据进行签名更新。
 * 
 * 本函数通过BCryptHashData函数将数据块哈希到签名上下文中，
 * 用于后续的签名操作。此函数不修改输入数据，只修改签名上下文。
 * 
 * @param ctx 签名操作的上下文，类型为指向T_CNG_SIGNATURE_CTX结构的指针。
 * @param data 要哈希的数据块，类型为指向unsigned char的常量指针。
 * @param datalen 数据块的长度，类型为size_t。
 * 
 * @return 如果哈希操作成功，返回1（真）；否则返回0（假）。
 * 
 * 注释参考：Microsoft文档指出，pbInput不会被修改。
 * 链接：https://learn.microsoft.com/en-us/windows/win32/api/bcrypt/nf-bcrypt-bcrypthashdata
 */
int cng_signature_digest_sign_update(void *ctx,
                                     const unsigned char *data,
                                     size_t datalen) {
    // 打印函数进入信息
    cb_printf("SIGNATURE> cng_signature_digest_sign_update");
    
    // 将void指针转换为签名上下文指针
    T_CNG_SIGNATURE_CTX *sig_ctx = (T_CNG_SIGNATURE_CTX *) ctx;
    
    // 调用BCryptHashData函数将数据哈希到签名上下文中
    // 根据Microsoft文档，pbInput不会被修改
    NTSTATUS s = BCryptHashData(sig_ctx->hash_handle, data, datalen, 0);
    
    // 如果操作成功，返回1（真）；否则返回0（假）
    return s == STATUS_SUCCESS;
}

/**
 * 使用私钥对先前计算的摘要进行签名。
 * 
 * @param ctx 签名上下文，包含签名所需的信息。
 * @param sig 用于存储签名的缓冲区。
 * @param siglen 指向签名长度的指针，将被更新为实际签名长度。
 * @param sigsize 签名缓冲区的大小。
 * @return 成功时返回1，失败时返回0。
 */
int cng_signature_digest_sign_final(void *ctx, unsigned char *sig,
                                    size_t *siglen, size_t sigsize) {
    cb_printf("SIGNATURE> cng_signature_digest_sign_final"); //xcs53 xcs59
    T_CNG_SIGNATURE_CTX *sig_ctx = (T_CNG_SIGNATURE_CTX *) ctx;
    ULONG pcb_result;
    void *padding_info;
    NTSTATUS s;
    SECURITY_STATUS ss;

    // 检查哈希大小
    DWORD hash_length;
    s = BCryptGetProperty(sig_ctx->hash_handle, BCRYPT_HASH_LENGTH, (PUCHAR) &hash_length, sizeof(hash_length),
                          &pcb_result, 0);
    if (s != STATUS_SUCCESS) { return 0; }

    // 如果sig为NULL，计算所需的签名长度
    if (sig == NULL) {
        unsigned char *hash_placeholder = malloc(hash_length);
        if (hash_placeholder == NULL) { return 0; }
        ss = NCryptSignHash(sig_ctx->key->windows_key_handle, NULL, hash_placeholder, hash_length, NULL, 0,
                            (DWORD *) siglen, 0);
        if (ss != ERROR_SUCCESS) {
            free(hash_placeholder);
            return 0;
        }
        free(hash_placeholder);
        return 1;
    } else {
        // 分配最终哈希值的缓冲区
        unsigned char *hash_buffer = malloc(hash_length);
        if (hash_buffer == NULL) { return 0; }
        // 完成哈希计算
        s = BCryptFinishHash(sig_ctx->hash_handle, hash_buffer, hash_length, 0);
        if (s != STATUS_SUCCESS) {
            free(hash_buffer);
            return 0;
        }

        // 设置正确的填充
        if (sig_ctx->sign_hash_flags == BCRYPT_PAD_PSS) {
            BCRYPT_PSS_PADDING_INFO *p = malloc(sizeof(BCRYPT_PSS_PADDING_INFO));
            if (p == NULL){ return 0; }
            p->pszAlgId = sig_ctx->alg_identifier;
            p->cbSalt = sig_ctx->pss_salt_len;
            padding_info = p;
        } else if (sig_ctx->sign_hash_flags == BCRYPT_PAD_PKCS1) {
            BCRYPT_PKCS1_PADDING_INFO *p = malloc(sizeof(BCRYPT_PKCS1_PADDING_INFO));
            if (p == NULL){ return 0; }
            p->pszAlgId = sig_ctx->alg_identifier;
            padding_info = p;
        } else {
            cb_printf("SIGN> Unknown padding recevied!");
            free(hash_buffer);
            return 0;
        }

        // 对哈希值进行签名
        ss = NCryptSignHash(sig_ctx->key->windows_key_handle, padding_info, hash_buffer, hash_length, sig, sigsize,
                            (DWORD *) siglen, sig_ctx->sign_hash_flags);

        free(padding_info);
        free(hash_buffer);
        return (ss == ERROR_SUCCESS);
    }
}

/**
 * cng_signature_settable_ctx_params函数返回一个静态OSSL_PARAM数组，其中包含了签名上下文中可设置的参数。
 * 这些参数用于配置签名和验证操作的各个方面，如摘要算法、填充模式以及与PSS相关的参数。
 *
 * @param ctx 签证或验证操作的上下文指针。这个参数在这个函数中未被使用。
 * @param provctx 提供者的上下文指针。这个参数在这个函数中未被使用。
 *
 * @return 返回一个指向OSSL_PARAM数组的指针，该数组定义了一组可由调用者用于配置签名操作的参数。
 */
const OSSL_PARAM *cng_signature_settable_ctx_params(void *ctx, void *provctx) {
    // 定义一个静态的OSSL_PARAM数组，包含了签名操作中可以被设置的各种参数。
    // 这些参数允许调用者指定例如摘要算法、填充模式等影响签名和验证过程的重要选项。
    static OSSL_PARAM settable[] = {
            OSSL_PARAM_utf8_string(OSSL_SIGNATURE_PARAM_DIGEST, NULL, 0),
            OSSL_PARAM_utf8_string(OSSL_SIGNATURE_PARAM_PROPERTIES, NULL, 0),
            OSSL_PARAM_utf8_string(OSSL_SIGNATURE_PARAM_PAD_MODE, NULL, 0),
            OSSL_PARAM_utf8_string(OSSL_SIGNATURE_PARAM_MGF1_DIGEST, NULL, 0),
            OSSL_PARAM_utf8_string(OSSL_SIGNATURE_PARAM_MGF1_PROPERTIES, NULL, 0),
            OSSL_PARAM_utf8_string(OSSL_SIGNATURE_PARAM_PSS_SALTLEN, NULL, 0),
            OSSL_PARAM_int(OSSL_SIGNATURE_PARAM_PSS_SALTLEN, NULL),
            OSSL_PARAM_END
    };
    // 返回定义好的参数数组指针。
    return settable;
}

/**
 * 设置签名上下文参数
 * 
 * 该函数根据提供的参数列表，设置签名上下文的各种参数，如填充模式和PSS盐长度
 * 主要处理两种参数：OSSL_SIGNATURE_PARAM_PAD_MODE和OSSL_SIGNATURE_PARAM_PSS_SALTLEN
 * 
 * @param ctx 上下文指针，指向T_CNG_SIGNATURE_CTX结构体
 * @param params 参数数组，包含签名操作的相关参数
 * 
 * @return 成功则返回1，否则返回0
 */
int cng_signature_set_ctx_params(void *ctx, const OSSL_PARAM params[]) {
    const OSSL_PARAM *p;
    T_CNG_SIGNATURE_CTX *sig_ctx = ctx;
    NTSTATUS s;
    int param_set = 0;

    // 检查并设置填充模式
    p = OSSL_PARAM_locate_const(params, OSSL_SIGNATURE_PARAM_PAD_MODE);
    if (p != NULL) {
        if (p->data_type == OSSL_PARAM_INTEGER) {
            if (*(int *) p->data == RSA_PKCS1_PSS_PADDING) {
                sig_ctx->sign_hash_flags = BCRYPT_PAD_PSS;
            } else if (*(int *) p->data == RSA_PKCS1_PADDING) {
                sig_ctx->sign_hash_flags = BCRYPT_PAD_PKCS1;
            } else {
                cb_printf("SIGN> 请求了未知的填充模式！");
                return 0;
            }
        } else if (p->data_type == OSSL_PARAM_UTF8_STRING) {
            if (!strcmp(p->data, OSSL_PKEY_RSA_PAD_MODE_PSS)) {
                // 注意：此处似乎没有设置sign_hash_flags，可能是代码不完整
            } else if (!strcmp(p->data, OSSL_PKEY_RSA_PAD_MODE_PKCSV15)) {
                sig_ctx->sign_hash_flags = BCRYPT_PAD_PKCS1;
            } else {
                cb_printf("SIGN> 请求了未知的填充模式！");
                return 0;
            }
        } else {
            cb_printf("SIGN> 请求了意外数据类型的填充模式！");
            return 0;
        }
        param_set = 1;
    }

    // 检查并设置PSS盐长度
    p = OSSL_PARAM_locate_const(params, OSSL_SIGNATURE_PARAM_PSS_SALTLEN);
    if (p != NULL) {
        if (p->data_type == OSSL_PARAM_INTEGER) {
            sig_ctx->pss_salt_len = *(ULONG *) p->data;
        } else if (p->data_type == OSSL_PARAM_UTF8_STRING) {
            if (!strcmp(p->data, OSSL_PKEY_RSA_PSS_SALT_LEN_DIGEST)) {
                ULONG receivedBytes;
                s = BCryptGetProperty(sig_ctx->hash_handle, BCRYPT_HASH_LENGTH, (PUCHAR) &sig_ctx->pss_salt_len,
                                      sizeof(sig_ctx->pss_salt_len), &receivedBytes, 0);
                if (s != STATUS_SUCCESS || receivedBytes == 0) {
                    sig_ctx->pss_salt_len = 0;
                    return 0;
                }
            } else if (!strcmp(p->data, OSSL_PKEY_RSA_PSS_SALT_LEN_MAX)) {
                cb_printf("SIGN> 未实现最大盐长度检测！");
                return 0;
            } else if (!strcmp(p->data, OSSL_PKEY_RSA_PSS_SALT_LEN_AUTO)) {
                cb_printf("SIGN> 未实现自动盐长度检测！");
                return 0;
            } else {
                return 0;
            }
        } else {
            cb_printf("SIGN> 收到了未知数据类型的盐长度！");
            return 0;
        }
        param_set = 1;
    }
    if (!param_set) {
        cb_printf("SIGN> 设置了未知参数！");
        return 0;
    }
    return 1;
}

/**
 * cng_signature_functions: 提供给OpenSSL库的一组签名操作函数指针数组。
 * 这个数组定义了与签名相关的各个操作接口，用于实现基于CNG（Cryptographic Next Generation）的签名功能。
 * 每个元素都是一个OSSL_DISPATCH结构体，包含了一个标识符和对应的函数指针。
 */
const OSSL_DISPATCH cng_signature_functions[] = {
        {OSSL_FUNC_SIGNATURE_NEWCTX,              (void (*)(void)) cng_signature_newctx}, // 创建一个新的签名上下文
        {OSSL_FUNC_SIGNATURE_DUPCTX,              (void (*)(void)) cng_signature_dupctx}, // 复制一个现有的签名上下文
        {OSSL_FUNC_SIGNATURE_FREECTX,             (void (*)(void)) cng_signature_freectx}, // 释放签名上下文
        {OSSL_FUNC_SIGNATURE_DIGEST_SIGN_INIT,    (void (*)(void)) cng_signature_digest_sign_init}, // 初始化签名操作
        {OSSL_FUNC_SIGNATURE_DIGEST_SIGN_UPDATE,  (void (*)(void)) cng_signature_digest_sign_update}, // 更新签名数据
        {OSSL_FUNC_SIGNATURE_DIGEST_SIGN_FINAL,   (void (*)(void)) cng_signature_digest_sign_final}, // 完成签名并获取结果
        {OSSL_FUNC_SIGNATURE_SET_CTX_PARAMS,      (void (*)(void)) cng_signature_set_ctx_params}, // 设置签名上下文参数
        {OSSL_FUNC_SIGNATURE_SETTABLE_CTX_PARAMS, (void (*)(void)) cng_signature_settable_ctx_params}, // 获取可设置的签名上下文参数列表
        {0, NULL} // 结束标志
};