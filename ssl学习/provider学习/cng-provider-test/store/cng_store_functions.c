#include "cng_store_functions.h"
#include <ncrypt.h>
#include "debug.h"

#define DEBUG_LEVEL DEBUG_ALL

/**
 * 初始化Windows证书存储
 * 
 * 此函数通过打开Windows系统证书存储来初始化一个T_CNG_STORE_CTX结构体
 * 它使用CertOpenSystemStore函数来打开存储，并根据操作结果返回状态代码
 * 
 * 参数:
 * @store_ctx 指向T_CNG_STORE_CTX结构体的指针，用于存储证书存储上下文
 * 
 * 返回值:
 * - 返回1表示成功打开系统存储
 * - 返回0表示打开系统存储失败
 */
int initialize_windows_cert_store(T_CNG_STORE_CTX *store_ctx) {
    // 尝试打开系统证书存储
    store_ctx->windows_certificate_store = CertOpenSystemStore(0, store_ctx->windows_system_store_name);
    
    // 检查是否成功打开证书存储
    if (store_ctx->windows_certificate_store) {
        // 成功打开系统存储
        cb_printf("xxx STORE> The system store is now open.");
        return 1;
    } else {
        // 未能打开系统存储
        cb_printf("xxx STORE> The system store did not open.");
        return 0;
    }
}

/**
 * 从证书存储中加载另一个证书到上下文
 * 
 * 此函数的目的在于从一个Windows证书存储中枚举并加载下一个证书到给定的证书存储上下文中
 * 它通过更新prev_cert_ctx指针来实现，该指针指向存储中的当前证书这允许在一系列调用中遍历证书存储中的所有证书
 * 此外，函数通过设置cert_store_eof标志来指示是否已到达证书存储的末尾，这有助于调用者知道何时完成证书枚举
 * 
 * 参数:
 * - store_ctx: 一个指向T_CNG_STORE_CTX类型的指针，代表证书存储上下文这个结构包含了证书存储操作所需的信息，
 *              如存储中的当前证书上下文(prev_cert_ctx)和一个标志(cert_store_eof)，用于指示是否已到达存储末尾
 * 
 * 注意: 此函数不返回任何值，但它通过修改store_ctx参数来改变输入参数的行为
 */
void load_another_cert_from_store_into_context(T_CNG_STORE_CTX *store_ctx) {
    // 从证书存储中枚举下一个证书，并将其存储在prev_cert_ctx中
    store_ctx->prev_cert_ctx = CertEnumCertificatesInStore(store_ctx->windows_certificate_store, store_ctx->prev_cert_ctx);
    
    // 如果prev_cert_ctx为空，则表示已到达证书存储的末尾
    store_ctx->cert_store_eof = !store_ctx->prev_cert_ctx;
}

/**
 * 助手函数，用于将另一个私钥从windows商店加载到我们的商店管理中
 *
 * 没有返回值，当出现错误或没有更多证书时，我们将priv_key_store_eof设置为true。
 * 实际上，我们只是枚举证书，但我们会提取它们的私钥。
 *
 * @param store_ctx Our providers store management context
 */
/**
 * 从证书存储中加载另一个私钥到上下文
 * 
 * 本函数尝试从Windows证书存储中加载下一个带有私钥的证书
 * 它首先枚举存储中的证书，然后尝试获取其关联的私钥
 * 如果成功获取到私钥，将把私钥的句柄保存到上下文中
 * 如果无法获取私钥或枚举结束，将递归调用自身以尝试加载存储中的下一个证书
 * 
 * @param store_ctx 指向存储上下文的指针，用于存储证书和私钥信息
 * @return 成功加载带有私钥的证书则返回1，否则返回0
 */
int load_another_privkey_from_store_into_context(T_CNG_STORE_CTX *store_ctx) {
    // 枚举存储中的下一个证书
    store_ctx->prev_key_cert_ctx = CertEnumCertificatesInStore(store_ctx->windows_certificate_store,
                                                               store_ctx->prev_key_cert_ctx);
    // 检查是否到达存储的末尾
    store_ctx->priv_key_store_eof = !store_ctx->prev_key_cert_ctx;
    if (store_ctx->priv_key_store_eof) {
        cb_printf("xxx STORE> No more certificates in store to extract private keys from");   //存储中没有更多证书可以从中提取私钥
        return 0;
    }

    // 定义用于存储私钥信息的变量
    DWORD key_spec;
    BOOL caller_must_free;
    NCRYPT_KEY_HANDLE tmp_key_handle;
    // 尝试获取当前证书关联的私钥
    BOOL retval = CryptAcquireCertificatePrivateKey(store_ctx->prev_key_cert_ctx, CRYPT_ACQUIRE_PREFER_NCRYPT_KEY_FLAG,
                                                    NULL, &tmp_key_handle, &key_spec, &caller_must_free);
    // 检查私钥获取结果
    if (retval != TRUE || key_spec != CERT_NCRYPT_KEY_SPEC || caller_must_free != TRUE) {
        // 加载私钥失败，要么没有关联的私钥，要么加载时出错，但我们仍尝试加载另一个私钥
        cb_printf("STORE> Recursing into loading private key");             //xcs30
        // 递归调用自身尝试加载下一个证书的私钥
        if (!load_another_privkey_from_store_into_context(store_ctx)) {
            NCryptFreeObject(tmp_key_handle);
            cb_printf("xxx STORE> Could not extract private key from certificate");
            return 0;
        }
        return 1; /* Recursive call was a success */
    }
    // 成功获取私钥，将其句柄保存到上下文中
    store_ctx->key->windows_key_handle = tmp_key_handle;
    return 1;
}

/**
 * 检查存储器打开参数是否有效。
 * 
 * 此函数用于验证提供给存储器打开操作的参数是否有效。它主要检查两个方面：
 * 1. 参数本身是否为NULL，确保基本的参数完整性。
 * 2. URI参数是否以"cng://"方案开头，这是此函数支持的唯一标识方案。
 * 
 * @param provctx 提供者上下文指针，用于存储器操作。
 * @param uri 用于标识存储器的统一资源标识符（URI）。
 * 
 * @return 如果参数有效，返回1；否则，返回0。
 */
int are_store_open_args_ok(void *provctx, const char *uri) {
    // 检查provctx和uri是否为NULL，如果是，则记录错误并返回失败
    if (provctx == NULL || uri == NULL) {
        cb_printf("xxx STORE> Trying to open store with invalid arguments");
        return 0;
    }

    // 检查URI是否以"cng://"方案开头，如果不是，则记录错误并返回失败
    if (strncmp(uri, "cng://", 6) != 0) {
        cb_printf("xxx STORE> Store opened with invalid URI scheme");
        return 0;
    }

    // 如果所有检查都通过，则返回成功
    return 1;
}

/**
 * 从给定的URI中解析出系统存储的名称，并为打开操作初始化存储上下文。
 * 
 * @param store_ctx 指向T_CNG_STORE_CTX结构体的指针，用于存储解析后的信息。
 * @param uri 字符串，表示包含存储名称的URI。
 * 
 * @return 成功解析并设置存储名称返回1，否则返回0。
 * 
 * 此函数解析URI的特定部分，以确定并设置store_ctx->windows_system_store_name成员
 * 为对应的Windows系统存储名称。如果URI不包含有效的系统存储名称，则返回0，
 * 并打印一条错误消息。
 */
int parse_uri_from_store_open(T_CNG_STORE_CTX *store_ctx, const char *uri) {
    // 跳过URI中的方案部分，因为它已经在其他地方被验证过了
    const char *str = uri + 6; 

    // 根据URI的不同前缀，确定存储名称
    if (!strncmp(str, "CA", 2)) { 
        store_ctx->windows_system_store_name = "CA"; 
    }
    if (!strncmp(str, "MY", 2)) { 
        store_ctx->windows_system_store_name = "MY"; 
    }
    if (!strncmp(str, "ROOT", 4)) { 
        store_ctx->windows_system_store_name = "ROOT"; 
    }

    // 如果没有识别出有效的存储名称，打印错误信息并返回0
    if (store_ctx->windows_system_store_name == NULL) {
        cb_printf("xxx STORE> Could not parse valid system store name");
        return 0;
    }

    // 成功解析存储名称，返回1
    return 1;
}

/**
 * 初始化存储上下文结构。
 * 
 * 该函数用于将存储上下文（T_CNG_STORE_CTX）结构中的指针字段初始化为NULL。
 * 这样可以确保在使用该结构之前，所有的指针字段都处于一个已知的初始状态，
 * 避免因为使用未初始化的指针而导致的潜在问题或错误。
 * 
 * @param store_ctx 指向T_CNG_STORE_CTX结构的指针。该结构需要被初始化。
 */
void init_store_ctx(T_CNG_STORE_CTX *store_ctx) {
    store_ctx->prev_cert_ctx = NULL; // 将前一个证书上下文指针初始化为NULL
    store_ctx->prev_key_cert_ctx = NULL; // 将前一个包含密钥的证书上下文指针初始化为NULL
    store_ctx->windows_system_store_name = NULL; // 将Windows系统存储名称初始化为NULL
}

/**
 * 打开一个加密服务提供者（CNG）存储。
 * 
 * 此函数接收一个提供者上下文和一个URI，并尝试打开对应的CNG存储。
 * 它首先检查提供的参数是否有效，然后分配存储上下文内存，
 * 初始化该上下文，创建一个管理密钥对象，并解析URI。
 * 如果所有步骤成功，它将尝试加载证书和私钥到上下文中。
 * 
 * @param provctx 提供者上下文，用于创建管理密钥对象。
 * @param uri 指定存储的URI。
 * 
 * @return 成功则返回指向打开的存储上下文的指针，失败则返回NULL。
 */
void *cng_store_open(void *provctx, const char *uri) {
    cb_printf("xxx cng_store_open");    // 调试信息：进入cng_store_open函数
    // 验证参数是否有效
    if (!are_store_open_args_ok(provctx, uri)) { return NULL; }

    // 分配存储上下文内存
    T_CNG_STORE_CTX *store_ctx = (T_CNG_STORE_CTX *) malloc(sizeof(T_CNG_STORE_CTX));
    if (store_ctx == NULL) {
        cb_printf("xxx Could not allocate memory for store context");
        return NULL;
    }

    // 初始化存储上下文
    init_store_ctx(store_ctx);

    // 创建管理密钥对象
    store_ctx->key = cng_keymgmt_new(provctx);
    if (store_ctx->key == NULL) {
        free(store_ctx);
        return NULL;
    }

    // 解析URI
    if (!parse_uri_from_store_open(store_ctx, uri)) {
        cb_printf("xxx STORE> Could not parse received URI");
        cng_keymgmt_free(store_ctx->key);
        free(store_ctx);
        return NULL;
    }

    // 初始化Windows证书存储
    if (!initialize_windows_cert_store(store_ctx)) {
        cng_keymgmt_free(store_ctx->key);
        free(store_ctx);
        return NULL;
    }

    // 准备加载证书
    load_another_cert_from_store_into_context(store_ctx);
    if (store_ctx->cert_store_eof){
        cb_printf("STORE> No certificates were found in the store when opening it.");
    }

    // 准备加载私钥
    load_another_privkey_from_store_into_context(store_ctx);
    if (store_ctx->priv_key_store_eof){
        cb_printf("STORE> No private keys were found in the store when opening it.");
    }

    // 返回存储上下文
    return store_ctx;
}

/**
 * cng_store_settable_ctx_params函数
 * 
 * 该函数用于在特定的提供者上下文中获取可设置的参数列表它主要用于在加密操作前后
 * 提供可以被设置或查询的上下文参数该函数被设计为允许在Cryptographic Next Generation (CNG)
 * 框架内进行操作
 * 
 * 参数:
 * provctx - 指向提供者特定的上下文这个上下文是在初始化提供者时建立的，可以被此函数
 *           用来存储或检索特定的数据
 * 
 * 返回:
 * 目前该函数总是返回NULL，表示没有实现具体的参数设置功能未来可能会扩展以返回更具体
 * 的上下文参数列表
 */
const OSSL_PARAM *cng_store_settable_ctx_params(void *provctx) {
    // 打印函数名，可能是为了调试目的
    cb_printf("cng_store_settable_ctx_params");
    // 当前实现没有返回具体的参数列表，返回NULL
    return NULL;
}

/**
 * cng_store_set_ctx_params 用于设置加密存储区的上下文参数。
 *
 * 参数:
 * @loaderctx: 指向存储区上下文的 void 指针，可以转换为适当类型以初始化参数。
 * @params: 一个常量 OSSL_PARAM 数组，用于指定存储区的各种选项。
 *
 * 返回值:
 * 返回一个整数，当前始终返回 1（表示成功）。
 *
 * 注意:
 * 该函数主要负责从 OSSL_PARAM 类型参数中解析期望的参数类型，并将其设置在存储区上下文中。
 * 如果输入参数列表为 NULL，则直接返回 1。
 * 目前没有针对不同参数类型的特定处理逻辑，但未来可能会扩展。
 */
int cng_store_set_ctx_params(void *loaderctx, const OSSL_PARAM params[]) {
    // 输出函数名称以便调试
    cb_printf("cng_store_set_ctx_params");

    // 将 void 指针转换为适当类型
    T_CNG_STORE_CTX *store_ctx = loaderctx;

    // 如果 params 为 NULL，则直接返回
    if (params == NULL) {
        return 1;
    }

    // 尝试定位 OSSL_STORE_PARAM_EXPECT 参数
    if ((p = OSSL_PARAM_locate_const(params, OSSL_STORE_PARAM_EXPECT))) {
        // 获取参数类型并设置在存储区上下文中
        int param_type = *(int *) p->data;
        // 调试输出，暂时注释掉
        //cb_printf("CORE EXPECTS A PARAMETER OF TYPE %d\n", param_type);
        store_ctx->expected_parameter_type = param_type;
    }

    // 注意: 还可能有其他参数，例如 OSSL_STORE_PARAM_SUBJECT, OSSL_STORE_PARAM_ISSUER 等，
    // 可以根据实际需求进行类似的处理。
    // Other possible values: OSSL_STORE_PARAM_SUBJECT, OSSL_STORE_PARAM_ISSUER, OSSL_STORE_PARAM_SERIAL
    // OSSL_STORE_PARAM_DIGEST, OSSL_STORE_PARAM_FINGERPRINT, OSSL_STORE_PARAM_ALIAS, OSSL_STORE_PARAM_PROPERTIES
    // OSSL_STORE_PARAM_INPUT_TYPE

    // 返回处理成功的标志
    return 1;
}

/**
 * 将证书数据从我们的上下文传递到OpenSSL格式
 *
 * @param store_ctx 我们的提供者存储管理上下文
 * @param object_cb 将存储的数据传递给该回调函数
 * @param object_cbarg 回调函数的附加参数
 */
void load_another_cert(T_CNG_STORE_CTX *store_ctx, OSSL_CALLBACK *object_cb, void *object_cbarg) {
    // 定义对象类型为证书
    static const int object_type_cert = OSSL_OBJECT_CERT;
    // 构建证书参数数组，包括对象类型和证书数据
    OSSL_PARAM cert_params[] = {
            OSSL_PARAM_int(OSSL_OBJECT_PARAM_TYPE, (int *) &object_type_cert),
            OSSL_PARAM_octet_string(OSSL_OBJECT_PARAM_DATA,
                                    store_ctx->prev_cert_ctx->pbCertEncoded,
                                    store_ctx->prev_cert_ctx->cbCertEncoded),
            OSSL_PARAM_END
    };
    // 调用回调函数，传递证书参数
    object_cb(cert_params, object_cbarg);
    // 枚举存储中的下一个证书
    store_ctx->prev_cert_ctx = CertEnumCertificatesInStore(store_ctx->windows_certificate_store, store_ctx->prev_cert_ctx);
    // 如果没有更多证书，则设置标志
    if (!store_ctx->prev_cert_ctx) {
        store_ctx->cert_store_eof = 1;
    }
}

/**
 * 将私钥数据从我们的上下文传递为OpenSSL格式
 * 该函数尝试加载一个额外的私钥到给定的上下文中这对于需要处理多个私钥的情况特别有用
 * 它通过回调函数来查找对象，并将其识别为一个私钥，然后进行加载
 * 
 * @param store_ctx 指向存储上下文的指针，用于加载私钥
 * @param object_cb 回调函数，用于从外部获取对象信息
 * @param object_cbarg 回调函数的用户数据指针
 * 
 * @return 成功加载私钥返回1，否则返回0
 */
int load_another_private_key(T_CNG_STORE_CTX *store_ctx, OSSL_CALLBACK *object_cb, void *object_cbarg) {
    /* 健全性检查 */
    if (store_ctx->priv_key_store_eof) { return 0; }

    /* 定义私钥对象类型 */
    static const int object_type_pkey = OSSL_OBJECT_PKEY;
    /* 构建私钥参数数组 */
    OSSL_PARAM privkey_params[] = {
            /* 设置对象类型为公私钥对 */
            OSSL_PARAM_int(OSSL_OBJECT_PARAM_TYPE, (int *) &object_type_pkey),
            /* 指定私钥数据类型为RSA加密 */
            OSSL_PARAM_utf8_string(OSSL_OBJECT_PARAM_DATA_TYPE, "rsaEncryption", 14),
            /* 提供公钥/私钥对的引用 */
            OSSL_PARAM_octet_string(OSSL_OBJECT_PARAM_REFERENCE, store_ctx->key, sizeof(T_CNG_KEYMGMT_KEYDATA)),
            /* 结束标志 */
            OSSL_PARAM_END
    };
    /* 通过回调函数查找并验证对象 */
    if (!object_cb(privkey_params, object_cbarg)) {
        return 0;
    }

    /* 不检查返回值，遇到错误时设置EOF标志 */
    cb_printf("STORE> Preloading private key for future use");              //xcs29
    /* 加载私钥到上下文中 */
    load_another_privkey_from_store_into_context(store_ctx);

    return 1;
}

/**
 * 从 OSSL_FUNC_store_open() 打开的URI加载下一个对象,
 * cng_store_load函数的作用是根据提供的上下文和回调函数来加载证书或私钥等信息。
 * 它是 OpenSSL 库中用于从存储中加载对象的更高级或特定于提供程序的机制的一部分。
 * 
 * @param loaderctx 一个指向加载器上下文的指针，通常包含关于如何进行加载的信息。
 * @param object_cb 一个回调函数指针，当加载到对象时会调用它。并尽其所能将其包装或解码为OpenSSL结构.
 * @param object_cbarg 传递给object_cb的附加参数。
 * @param pw_cb 如果需要密码回调来解密私钥等，则为此目的提供密码回调函数指针。
 * @param pw_cbarg 传递给pw_cb的附加参数。
 * 
 * @return 返回1表示加载操作成功，返回0表示没有更多项目可以加载，或者在某些情况下可能表示失败。
 */
int cng_store_load(void *loaderctx,
                   OSSL_CALLBACK *object_cb,
                   void *object_cbarg,
                   OSSL_PASSPHRASE_CALLBACK *pw_cb,
                   void *pw_cbarg) {
    cb_printf("cng_store_load");  // 调试信息：标记函数被调用
    T_CNG_STORE_CTX *store_ctx = loaderctx;

    // 注释掉的代码段解释了如何根据期望的参数类型来加载不同的信息类型
    /*
    switch (store_ctx->expected_parameter_type) {
        case OSSL_STORE_INFO_NAME:
            break;
        case OSSL_STORE_INFO_PARAMS:
            break;
        case OSSL_STORE_INFO_PUBKEY:
            break;
        case OSSL_STORE_INFO_PKEY:
            load_another_pkey(store_ctx, object_cb, object_cbarg);
            break;
        case OSSL_STORE_INFO_CERT:
            load_another_cert(store_ctx, object_cb, object_cbarg);
            break;
        case OSSL_STORE_INFO_CRL:
            break;
        default:
            cb_printf("IT IS IMPOLITE TO LOAD WITHOUT SETTING EXPECTATIONS\n");
            load_another_cert(store_ctx, object_cb, object_cbarg);
            return 1;
    }
    */

    // 检查期望的参数类型，如果不是证书并且未设置期望类型，则打印信息
    if (store_ctx->expected_parameter_type != OSSL_STORE_INFO_CERT && !store_ctx->expected_parameter_type) {
        cb_printf("STORE> Core asked for something else than a certificate while loading.");
    }

    // 如果证书存储未结束，则尝试加载另一个证书
    if (!store_ctx->cert_store_eof) {
        load_another_cert(store_ctx, object_cb, object_cbarg);
        // 当有证书时，我们不能让这个查询失败
        return 1;
    }

    // 如果私钥存储未结束，则尝试加载另一个私钥
    if (!store_ctx->priv_key_store_eof) {
        return load_another_private_key(store_ctx, object_cb, object_cbarg);
    }

    // 没有更多项目可以加载
    return 0;
}

/**
 * 检查是否已到达从URI获取的对象集的末尾。
 * 此函数用于确定是否已从URI完全加载所有对象，例如证书和私钥。当到达末尾时，
 * 进一步尝试加载更多对象将变得没有必要。
 *
 * @param loaderctx 加载上下文指针。
 *                 上下文中包含当前加载状态的信息，
 *                 包括指示证书存储和私钥存储是否已到达末尾的标志。
 * @return 返回一个整数，表示证书和私钥存储是否都已到达末尾。1 表示真（已到达末尾），
 *         0 表示假（未到达末尾）。
 */
int cng_store_eof(void *loaderctx) {
    cb_printf("cng_store_eof");
    T_CNG_STORE_CTX *store_ctx = loaderctx;
    return (store_ctx->cert_store_eof && store_ctx->priv_key_store_eof); // 或其他任何末尾标志
}

/**
 * Free loader context
 * @param loaderctx Loader context
 * @return True if operation was success
 */
/**
 * 关闭CNG存储的函数
 * 
 * 此函数旨在释放与CNG存储相关联的资源，包括关闭Windows证书存储和释放分配的内存
 * 它接受一个void指针loaderctx，该指针应指向一个T_CNG_STORE_CTX结构体
 * 如果成功关闭存储并释放资源，则返回1；否则，如果在关闭存储时遇到错误，则返回0
 * 
 * 参数:
 *   loaderctx - 指向T_CNG_STORE_CTX结构体的指针，包含加载上下文
 * 
 * 返回值:
 *   如果成功关闭存储并释放所有资源，则返回1
 *   如果无法关闭存储，则返回0
 */
int cng_store_close(void *loaderctx) {
    // 将loaderctx转换为T_CNG_STORE_CTX指针
    T_CNG_STORE_CTX *store_ctx = (T_CNG_STORE_CTX *) loaderctx;
    
    // 打印函数进入消息，用于调试目的
    cb_printf("cng_store_close");
    
    // 尝试关闭Windows证书存储
    BOOL cs = CertCloseStore(store_ctx->windows_certificate_store, 0);
    
    // 如果无法关闭存储，则返回失败
    if (cs != TRUE) { return 0; }
    
    // 释放密钥管理资源
    cng_keymgmt_free(store_ctx->key);
    
    // 释放存储上下文内存
    free(store_ctx);
    
    // 成功关闭存储并释放所有资源
    return 1;
}


// 定义一个名为cng_store_functions的常量数组，用于存储OSSL_DISPATCH类型的函数指针
// 这些函数指针对应到具体的存储操作功能
const OSSL_DISPATCH cng_store_functions[] = {
        // 对应于OSSL_STORE_open_ex函数，当调用该函数时，将执行cng_store_open
        // 第一个参数uri会被传递给cng_store_open的第二个参数
        {OSSL_FUNC_STORE_OPEN,                (void (*)(void)) cng_store_open},
        
        // 设置存储上下文可设置参数的函数指针
        {OSSL_FUNC_STORE_SETTABLE_CTX_PARAMS, (void (*)(void)) cng_store_settable_ctx_params},
        
        // 设置存储上下文参数的函数指针
        {OSSL_FUNC_STORE_SET_CTX_PARAMS,      (void (*)(void)) cng_store_set_ctx_params},
        
        // 对应于OSSL_STORE_load函数，执行加载操作
        {OSSL_FUNC_STORE_LOAD,                (void (*)(void)) cng_store_load},
        
        // 判断存储是否到达文件末尾的函数指针
        {OSSL_FUNC_STORE_EOF,                 (void (*)(void)) cng_store_eof},
        
        // 关闭存储的函数指针
        {OSSL_FUNC_STORE_CLOSE,               (void (*)) cng_store_close},
        
        // 数组结束标志
        {0, NULL}
};