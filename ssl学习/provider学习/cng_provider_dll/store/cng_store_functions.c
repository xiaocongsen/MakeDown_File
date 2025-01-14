#include "cng_store_functions.h"
#include <ncrypt.h>
#include "debug.h"

#define DEBUG_LEVEL DEBUG_ALL

/**
 * Helper function to initialize the windows certificate store
 * @param store_ctx Our providers store management context
 * @return One on success, zero on error
 */
int initialize_windows_cert_store(T_CNG_STORE_CTX *store_ctx) { //xcs4 xcs18
    store_ctx->windows_certificate_store = CertOpenSystemStore(0, store_ctx->windows_system_store_name);
    if (store_ctx->windows_certificate_store) {
        cb_printf("xxx STORE> The system store is now open.");
        return 1;
    } else {
        cb_printf("xxx STORE> The system store did not open.");
        return 0;
    }
}


/**
 * Helper function to load another certificate from windows store to our store management
 *
 * No return value, on error or when there are no more certificates we set the cert_store_eof to true
 * @param store_ctx Our providers store management context
 */
void load_another_cert_from_store_into_context(T_CNG_STORE_CTX *store_ctx) {
    store_ctx->prev_cert_ctx = CertEnumCertificatesInStore(store_ctx->windows_certificate_store, store_ctx->prev_cert_ctx);
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
int load_another_privkey_from_store_into_context(T_CNG_STORE_CTX *store_ctx) {
    store_ctx->prev_key_cert_ctx = CertEnumCertificatesInStore(store_ctx->windows_certificate_store,
                                                               store_ctx->prev_key_cert_ctx);
    store_ctx->priv_key_store_eof = !store_ctx->prev_key_cert_ctx;
    if (store_ctx->priv_key_store_eof) {
        cb_printf("xxx STORE> No more certificates in store to extract private keys from");   //存储中没有更多证书可以从中提取私钥
        return 0;
    }

    DWORD key_spec;
    BOOL caller_must_free;
    NCRYPT_KEY_HANDLE tmp_key_handle;
    BOOL retval = CryptAcquireCertificatePrivateKey(store_ctx->prev_key_cert_ctx, CRYPT_ACQUIRE_PREFER_NCRYPT_KEY_FLAG,
                                                    NULL, &tmp_key_handle, &key_spec, &caller_must_free);
    if (retval != TRUE || key_spec != CERT_NCRYPT_KEY_SPEC || caller_must_free != TRUE) {
        /* 加载私钥失败，要么没有关联的私钥，要么加载时出错，但我们仍尝试加载另一个私钥 */
        cb_printf("STORE> Recursing into loading private key");             //xcs30
        if (!load_another_privkey_from_store_into_context(store_ctx)) {
            NCryptFreeObject(tmp_key_handle);
            cb_printf("xxx STORE> Could not extract private key from certificate");
            return 0;
        }
        return 1; /* Recursive call was a success */
    }
    store_ctx->key->windows_key_handle = tmp_key_handle;
    return 1;
}

/**
 * Returns true if arguments for cng_store_open are valid
 *
 * @param provctx provider context
 * @param uri URI from which we load data into our store
 * @return 1 on valid arguments, zero otherwise
 */
int are_store_open_args_ok(void *provctx, const char *uri) {
    if (provctx == NULL || uri == NULL) {
        cb_printf("xxx STORE> Trying to open store with invalid arguments");
        return 0;
    }
    if (strncmp(uri, "cng://", 6) != 0) {
        cb_printf("xxx STORE> Store opened with invalid URI scheme");
        return 0;
    }
    return 1;
}

int parse_uri_from_store_open(T_CNG_STORE_CTX *store_ctx, const char *uri) {
    const char *str = uri + 6; //in are_store_open_args_ok() we demand full scheme name, so we can skip that now
    if (!strncmp(str, "CA", 2)) { store_ctx->windows_system_store_name = "CA"; }
    if (!strncmp(str, "MY", 2)) { store_ctx->windows_system_store_name = "MY"; }
    if (!strncmp(str, "ROOT", 4)) { store_ctx->windows_system_store_name = "ROOT"; }
    if (store_ctx->windows_system_store_name == NULL) {
        cb_printf("xxx STORE> Could not parse valid system store name");
        return 0;
    }
    return 1;
}

void init_store_ctx(T_CNG_STORE_CTX *store_ctx) {
    store_ctx->prev_cert_ctx = NULL;
    store_ctx->prev_key_cert_ctx = NULL;
    store_ctx->windows_system_store_name = NULL;
}

/**
 * Create a provider side context with data based on URI
 *
 * @param provctx Provider context
 * @param uri 我们将数据加载到存储中的URI
 * @return Loader context or NULL on error
 */
void *cng_store_open(void *provctx, const char *uri) {
    cb_printf("xxx cng_store_open");    //xcs2  xcs16
    if (!are_store_open_args_ok(provctx, uri)) { return NULL; }
    T_CNG_STORE_CTX *store_ctx = (T_CNG_STORE_CTX *) malloc(sizeof(T_CNG_STORE_CTX));
    if (store_ctx == NULL) {
        cb_printf("xxx Could not allocate memory for store context");
        return NULL;
    }

    init_store_ctx(store_ctx);

    store_ctx->key = cng_keymgmt_new(provctx);  //管理密钥对象的创建
    if (store_ctx->key == NULL) {
        free(store_ctx);
        return NULL;
    }

    if (!parse_uri_from_store_open(store_ctx, uri)) {
        cb_printf("xxx STORE> Could not parse received URI");
        cng_keymgmt_free(store_ctx->key);
        free(store_ctx);

        return NULL;
    }

    if (!initialize_windows_cert_store(store_ctx)) {
        cng_keymgmt_free(store_ctx->key);
        free(store_ctx);
        return NULL;
    }


    /* Prepare for loading certificates */
    /* We do not check return value, if there are no keys we'll simply set eof */
    load_another_cert_from_store_into_context(store_ctx);
    if (store_ctx->cert_store_eof){
        cb_printf("STORE> No certificates were found in the store when opening it.");
    }
    /* Same story as with certificates */
    load_another_privkey_from_store_into_context(store_ctx);
    if (store_ctx->priv_key_store_eof){
        cb_printf("STORE> No private keys were found in the store when opening it.");
    }
    return store_ctx;
}

/**
 *  Return a constant array of descriptor OSSL_PARAM,
 *  for parameters that OSSL_FUNC_store_set_ctx_params() can handle
 *
 * @param provctx Provider context
 * @return Array of descriptors OSSL_FUNC_store_set_ctx_params() can handle
 */
const OSSL_PARAM *cng_store_settable_ctx_params(void *provctx) {
    cb_printf("cng_store_settable_ctx_params");
    return NULL;
}

/**
 *  Set additional parameters, such as what kind of data to expect,
 *  search criteria, and so on.
 *
 *  Passing params as NULL should return true
 *
 * @param loaderctx
 * @param params
 * @return True on success
 */
int cng_store_set_ctx_params(void *loaderctx, const OSSL_PARAM params[]) {
    cb_printf("cng_store_set_ctx_params");
    const OSSL_PARAM *p;

    T_CNG_STORE_CTX *store_ctx = loaderctx;

    if (params == NULL) {
        return 1;
    }

    if ((p = OSSL_PARAM_locate_const(params, OSSL_STORE_PARAM_EXPECT))) {
        int param_type = *(int *) p->data;
        //cb_printf("CORE EXPECTS A PARAMETER OF TYPE %d\n", param_type);
        store_ctx->expected_parameter_type = param_type;
    }

    // Other possible values: OSSL_STORE_PARAM_SUBJECT, OSSL_STORE_PARAM_ISSUER, OSSL_STORE_PARAM_SERIAL
    // OSSL_STORE_PARAM_DIGEST, OSSL_STORE_PARAM_FINGERPRINT, OSSL_STORE_PARAM_ALIAS, OSSL_STORE_PARAM_PROPERTIES
    // OSSL_STORE_PARAM_INPUT_TYPE
    return 1;
}

/**
 * Pass the certificate data from our context to an OpenSSL format
 *
 * @param store_ctx Our providers store management context
 * @param object_cb Function to callback with the stores data
 * @param object_cbarg Additional arguments for the callback
 */
void load_another_cert(T_CNG_STORE_CTX *store_ctx, OSSL_CALLBACK *object_cb, void *object_cbarg) {
    static const int object_type_cert = OSSL_OBJECT_CERT;
    OSSL_PARAM cert_params[] = {
            OSSL_PARAM_int(OSSL_OBJECT_PARAM_TYPE, (int *) &object_type_cert),
            OSSL_PARAM_octet_string(OSSL_OBJECT_PARAM_DATA,
                                    store_ctx->prev_cert_ctx->pbCertEncoded,
                                    store_ctx->prev_cert_ctx->cbCertEncoded),
            OSSL_PARAM_END
    };
    object_cb(cert_params, object_cbarg);
    store_ctx->prev_cert_ctx = CertEnumCertificatesInStore(store_ctx->windows_certificate_store, store_ctx->prev_cert_ctx);
    if (!store_ctx->prev_cert_ctx) {
        store_ctx->cert_store_eof = 1;
    }
}

/**
 * 将私钥数据从我们的上下文传递为OpenSSL格式
 *
 * 如果我们有一个对象引用，我们必须有一个数据类型 - store_result.c:196
 * @param store_ctx Our providers store management context
 * @param object_cb 使用存储数据进行回调的函数
 * @param object_cbarg 回调的其他参数
 */
int load_another_private_key(T_CNG_STORE_CTX *store_ctx, OSSL_CALLBACK *object_cb, void *object_cbarg) {
    /* 健全性检查 */
    if (store_ctx->priv_key_store_eof) { return 0; }

    static const int object_type_pkey = OSSL_OBJECT_PKEY;
    OSSL_PARAM privkey_params[] = {
            /* 这可以有一个 OSSL_OBJECT_PARAM_REFERENCE 而不是 OSSL_OBJECT_PARAM_DATA */
            OSSL_PARAM_int(OSSL_OBJECT_PARAM_TYPE, (int *) &object_type_pkey),
            OSSL_PARAM_utf8_string(OSSL_OBJECT_PARAM_DATA_TYPE, "rsaEncryption", 14),
            OSSL_PARAM_octet_string(OSSL_OBJECT_PARAM_REFERENCE, store_ctx->key, sizeof(T_CNG_KEYMGMT_KEYDATA)),
            /* 使用此功能直接将私钥作为DER数据发送，以后不需要签名算法
             * OSSL_PARAM_octet_string(OSSL_OBJECT_PARAM_DATA, cng2_client_private_key_der,
             * cng2_client_private_key_der_len),
             * OSSL_PARAM_utf8_string(OSSL_OBJECT_PARAM_DATA_STRUCTURE, "PrivateKeyInfo", 15) */
            OSSL_PARAM_END
    };
    if (!object_cb(privkey_params, object_cbarg)) {
        return 0;
    }

    /* We do not check return value, in case of error we set eof flag */
    cb_printf("STORE> Preloading private key for future use");              //xcs29
    load_another_privkey_from_store_into_context(store_ctx);

    return 1;
}

/**
 * 从 OSSL_FUNC_store_open() 打开的URI加载下一个对象,
 * 为其创建对象抽象,
 * 并用它调用 object_cb 以及 object_cbarg.
 * object_cb 将解释对象抽象，并尽其所能将其包装或解码为OpenSSL结构.
 * 如果需要提示密码来解锁对象,
 * 应该调用 pw_cb.
 *
 * @param loaderctx Loader context
 * @param object_cb Object callback
 * @param object_cbarg Object callback arguemnts
 * @param pw_cb Password callback
 * @param pw_cbarg Password callback arguments
 * @return If operation was a success
 */
int cng_store_load(void *loaderctx,
                   OSSL_CALLBACK *object_cb, /* store_result.c ossl_store_handle_load_result */
                   void *object_cbarg, /* store_lib.c load_data OSSL_STORE_INFO * */
                   OSSL_PASSPHRASE_CALLBACK *pw_cb, /* store_lib.c ossl_pw_passphrase_callback_dec */
                   void *pw_cbarg /* store_lib.c &ctx->pwdata */
) {
    cb_printf("cng_store_load");  //xcs6 xcs20
    T_CNG_STORE_CTX *store_ctx = loaderctx;

    /* 这仅建议用于优化搜索 */
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
    if (store_ctx->expected_parameter_type != OSSL_STORE_INFO_CERT && !store_ctx->expected_parameter_type) {
        cb_printf("STORE> Core asked for something else than a certificate while loading.");
    }
    if (!store_ctx->cert_store_eof) {
        load_another_cert(store_ctx, object_cb, object_cbarg);
        /* 当有证书时, 我们不能让这个查询失败 */
        return 1;
    }
    if (!store_ctx->priv_key_store_eof) {
        return load_another_private_key(store_ctx, object_cb, object_cbarg);
    }
    return 0;
}

/**
 * Indicates if the end of the set of objects from the URI has been
 * reached. When that happens, there's no point trying to do any
 * further loading
 *
 * @param loaderctx Loader context
 * @return True if operation was success
 */
int cng_store_eof(void *loaderctx) {
    cb_printf("cng_store_eof"); //xcs5 xcs19 xcs22
    T_CNG_STORE_CTX *store_ctx = loaderctx;
    return (store_ctx->cert_store_eof && store_ctx->priv_key_store_eof); //Or any other eof flag
}

/**
 * Free loader context
 * @param loaderctx Loader context
 * @return True if operation was success
 */
int cng_store_close(void *loaderctx) {
    T_CNG_STORE_CTX *store_ctx = (T_CNG_STORE_CTX *) loaderctx;
    cb_printf("cng_store_close");   //xcs13 xcs38
    BOOL cs = CertCloseStore(store_ctx->windows_certificate_store, 0);
    if (cs != TRUE) { return 0; }
    cng_keymgmt_free(store_ctx->key);
    free(store_ctx);
    return 1;
}


const OSSL_DISPATCH cng_store_functions[] = {
        {OSSL_FUNC_STORE_OPEN,                (void (*)(void)) cng_store_open},                 //调用OSSL_STORE_open_ex会执行 第一个参数uri会传递给cng_store_open的第二个参数
        {OSSL_FUNC_STORE_SETTABLE_CTX_PARAMS, (void (*)(void)) cng_store_settable_ctx_params},
        {OSSL_FUNC_STORE_SET_CTX_PARAMS,      (void (*)(void)) cng_store_set_ctx_params},
        {OSSL_FUNC_STORE_LOAD,                (void (*)(void)) cng_store_load},                 //OSSL_STORE_load
        {OSSL_FUNC_STORE_EOF,                 (void (*)(void)) cng_store_eof},
        {OSSL_FUNC_STORE_CLOSE,               (void (*)) cng_store_close},
        {0, NULL}
};
