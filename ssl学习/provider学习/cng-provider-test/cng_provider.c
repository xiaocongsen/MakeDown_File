#include "cng_provider.h"
#include "store/cng_store_functions.h"
#include "keymgmt/cng_keymgmt_functions.h"
#include "signature/cng_signature_functions.h"
#include "debug.h"
#define DEBUG_LEVEL 1

/**
 * cng_gettable_params 函数
 * 
 * @param prov Provider 的指针，未使用，保留以满足函数接口要求
 * @return 返回一个常量指针，指向静态的 OSSL_PARAM 数组 cng_param_types
 * 
 * 该函数用于获取 CNG (Cryptographic Next Generation) Provider 可以公开的参数列表
 * 这些参数包含了 Provider 支持的各种配置和信息查询的名称和数据类型
 * OpenSSL 库使用这些参数来查询 Provider 的信息，例如版本号、支持的算法等
 */
static const OSSL_PARAM *cng_gettable_params(void *prov) {
    cb_printf("PROV> cng_gettable_params");
    return cng_param_types;     //返回包含了 provider 可以公开的参数的名称和数据类型。OpenSSL 使用这些参数来查询 provider 提供的各种信息，如版本号、支持的算法等。
}

/**
 * 检查配置过程是否正在运行。
 *
 * 此函数模拟了一个配置过程正在运行的状态检查，实际上总是返回1（表示正在运行）。
 * 使用此函数可以了解到配置过程的状态，以便决定是否需要等待或进行其他处理。
 *
 * 返回:
 *   总是返回1，表示配置过程正在运行。
 */
int cng_prov_is_running() {
    cb_printf("PROV> cng_prov_is_running");
    return 1;
}

/**
 *  调用它来获取前面 cng_gettable_params 返回的参数的实际值。这个函数通常会根据提供的参数名称返回相应的值。
 *
 * [1] https://www.openssl.org/docs/manmaster/man7/provider-base.html#Provider-parameters
 * @param provctx Providers context
 * @param params Parameters that the caller wants to know as in [1]
 * @return One on success, zero on error
 */
static int cng_get_params(void *provctx, OSSL_PARAM params[]) {
    cb_printf("PROV> cng_get_params");

    OSSL_PARAM *p;

    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_NAME);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, CNG_PROVIDER_NAME_STR))
        return 0;
    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_VERSION);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, CNG_PROVIDER_VERSION_STR))
        return 0;
    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_BUILDINFO);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, CNG_PROVIDER_BUILDINFO_STR))
        return 0;
    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_STATUS);
    if (p != NULL && !OSSL_PARAM_set_int(p, cng_prov_is_running()))
        return 0;
    return 1;
}

/**
 * 返回operation_id指示的特定操作的函数指针数组
 * 用于告诉 OpenSSL 核心，provider 支持哪些操作（operation），以及如何调用它们。这个函数通常会返回与指定操作相关的函数表。
 * [1] https://www.openssl.org/docs/manmaster/man7/provider.html#Operations
 * @param provctx Providers context
 * @param operation_id Requested operation as in [1]
 * @param no_cache Whether the OpenSSL core can store the references returned
 * @return OSSL_ALGORITHM array with function pointers of given operation or NULL on error
 */
static const OSSL_ALGORITHM *cng_query_operation(void *provctx,
                                                 int operation_id,
                                                 int *no_cache) {
    cb_printf("PROV> cng_query_operation");

    T_CNG_PROVIDER_CTX *tprovctx = provctx;
    *no_cache = 0; /* 不要存储我们将返回的数据的引用 */

    switch (operation_id) {
    case OSSL_OP_DIGEST:    //xcs12
        cb_printf("PROV> Digest operations are not supported");     //不支持摘要操作，用于计算数据的哈希值。
        break;  //OSSL_FUNC_DIGEST_NEWCTX OSSL_FUNC_DIGEST_INIT OSSL_FUNC_DIGEST_UPDATE OSSL_FUNC_DIGEST_FINAL OSSL_FUNC_DIGEST_FREECTX
    case OSSL_OP_STORE: //xcs1
        cb_printf("PROV> Returning cng_store to core");             //存储操作，用于从外部存储加载和保存密钥、证书或其他对象。
        return cng_store;//OSSL_FUNC_STORE_OPEN OSSL_FUNC_STORE_LOAD OSSL_FUNC_STORE_CLOSE
    case OSSL_OP_CIPHER:
        cb_printf("PROV> Cipher operations are not supported");     //不支持对称加密操作，用于数据的加密和解密。
        break;//OSSL_FUNC_CIPHER_NEWCTX OSSL_FUNC_CIPHER_ENCRYPT_INIT OSSL_FUNC_CIPHER_UPDATE OSSL_FUNC_CIPHER_FINAL OSSL_FUNC_CIPHER_FREECTX
    case OSSL_OP_MAC:
        cb_printf("PROV> Message authentication code operations are not supported");    //不支持消息认证码操作 用于生成和验证数据的消息认证码。
        break;//OSSL_FUNC_MAC_NEWCTX OSSL_FUNC_MAC_INIT OSSL_FUNC_MAC_UPDATE OSSL_FUNC_MAC_FINAL OSSL_FUNC_MAC_FREECTX
    case OSSL_OP_KDF:
        cb_printf("PROV> Key derivation functions operations are not supported");       //不支持密钥派生函数(KDF)，用于从一个或多个密钥材料生成新密钥。
        break;//OSSL_FUNC_KDF_NEWCTX OSSL_FUNC_KDF_DERIVE OSSL_FUNC_KDF_FREECTX
    case OSSL_OP_RAND:
        cb_printf("PROV> Random operations are not supported");                 //不支持随机数生成操作，用于生成加密安全的随机数。
        break;//OSSL_FUNC_RAND_NEWCTX OSSL_FUNC_RAND_GENERATE OSSL_FUNC_RAND_FREECTX
    case OSSL_OP_KEYMGMT: //xcs7 xcs18 xcs23 xcs42
        cb_printf("PROV> Returning cng_keymgmt to the core");               //密钥管理操作 用于管理密钥的生命周期（生成、加载、导出、释放等）。
        return cng_keymgmt;//OSSL_FUNC_KEYMGMT_NEWCTX OSSL_FUNC_KEYMGMT_LOAD OSSL_FUNC_KEYMGMT_IMPORT OSSL_FUNC_KEYMGMT_EXPORT OSSL_FUNC_KEYMGMT_FREECTX
    case OSSL_OP_KEYEXCH:
        cb_printf("PROV> Key exchange operations are not supported");       //不支持密钥交换操作，用于协商和交换会话密钥。
        break;//OSSL_FUNC_KEYEXCH_NEWCTX OSSL_FUNC_KEYEXCH_EXCHANGE OSSL_FUNC_KEYEXCH_FREECTX
    case OSSL_OP_SIGNATURE:
        cb_printf("PROV> Returning cng_signature to core");         //签名操作，用于生成和验证数字签名。
        return cng_signature;//OSSL_FUNC_SIGNATURE_NEWCTX OSSL_FUNC_SIGNATURE_SIGN OSSL_FUNC_SIGNATURE_VERIFY OSSL_FUNC_SIGNATURE_FREECTX
    case OSSL_OP_ASYM_CIPHER:
        cb_printf("PROV> Asymmetric cipher operations are not supported");  //不支持非对称加密操作，用于使用非对称密钥对数据进行加密或解密。
        break;//OSSL_FUNC_ASYM_CIPHER_NEWCTX OSSL_FUNC_ASYM_CIPHER_ENCRYPT OSSL_FUNC_ASYM_CIPHER_DECRYPT OSSL_FUNC_ASYM_CIPHER_FREECTX
    case OSSL_OP_KEM:
        cb_printf("PROV> Key encapsulation operations are not supported");  //不支持密钥封装机制（KEM）操作，用于安全地分发对称密钥。
        break;//OSSL_FUNC_KEM_NEWCTX OSSL_FUNC_KEM_ENCAPSULATE OSSL_FUNC_KEM_DECAPSULATE OSSL_FUNC_KEM_FREECTX
    case OSSL_OP_ENCODER:   //xcs11  xcs32
        cb_printf("PROV> Encoder operations are not supported");        //不支持编码操作，用于将密钥、证书或其他对象编码为特定格式（如 PEM、DER）。
        break;//OSSL_FUNC_ENCODER_NEWCTX OSSL_FUNC_ENCODER_ENCODE OSSL_FUNC_ENCODER_FREECTX
    case OSSL_OP_DECODER: //xcs8 xcs21  xcs43
        cb_printf("PROV> Decoding is not supported");           //不支持解码操作，用于将特定格式的编码数据解码为密钥、证书或其他对象。
        break;//OSSL_FUNC_DECODER_NEWCTX OSSL_FUNC_DECODER_DECODE OSSL_FUNC_DECODER_FREECTX
    default:
        cb_printf("PROV> Returning nothing, no algorithm matches");     //不返回任何内容，没有算法匹配
        return NULL;
    }
    return NULL; /* When unsupported return NULL */
}

/* 当 OpenSSL 不再需要这个 provider 时，会调用 cng_teardown 进行清理。此函数负责释放 provider 在初始化过程中分配的所有资源，确保没有内存泄漏。*/
static void cng_teardown(void *provctx) {
    cb_printf("PROV> cng_teardown");
    free(provctx);
}

/* Functions we provide to the core */
static const OSSL_DISPATCH cng_dispatch_table[] = {
    {OSSL_FUNC_PROVIDER_GETTABLE_PARAMS, (void (*)(void)) cng_gettable_params},         //用于返回 provider 可获取的参数列表。
    {OSSL_FUNC_PROVIDER_GET_PARAMS,      (void (*)(void)) cng_get_params},              //用于获取 provider 的具体参数值。
    {OSSL_FUNC_PROVIDER_QUERY_OPERATION, (void (*)(void)) cng_query_operation},         //提供对特定操作（如加密、解密、签名等）的查询。
    {OSSL_FUNC_PROVIDER_TEARDOWN,        (void (*)(void)) cng_teardown},                //清理和释放 provider 相关的资源。
    {0, NULL}
    //OSSL_FUNC_PROVIDER_UNQUERY_OPERATION  //撤销对特定操作的查询。
    //当 OpenSSL 核心不再需要某种操作时，它可能调用这个函数来通知 provider 释放与该操作相关的资源。这对于动态资源管理很有用。
    //OSSL_FUNC_PROVIDER_GET_REASON_STRINGS  //获取 provider 特定的错误原因字符串。
//    OpenSSL 核心使用这个函数来获取 provider 的错误信息映射表。每个错误代码对应一个错误字符串，帮助用户更好地理解错误原因。
    //OSSL_FUNC_PROVIDER_GET_CAPABILITIES   //提供 provider 的能力描述。
    //个函数用于告诉 OpenSSL 核心，provider 支持哪些特定的功能或硬件加速选项等。例如，某个 provider 可能支持特定的硬件加速算法，或者特定的加密模式。OpenSSL 可以调用这个函数获取这些信息并进行相应优化。
    //OSSL_FUNC_PROVIDER_SELF_TEST  //执行 provider 的自检操作。
//     Provider 可能需要在初始化时或特定事件发生时进行自检，以确保其算法实现是正确的。OpenSSL 核心会调用这个函数执行自检操作。
};

/**
 * 提供程序的入口点，用于动态和静态加载
 *
 * Definded in openssl/core.h
 * @param handle OpenSSLs object for this provider, useful only for calling functions given in the in array
 * @param in OpenSSL核心向提供者提供的功能
 * @param out 它为OpenSSL核心提供的基本提供者函数的功能
 * @param provctx 特定于提供商的上下文，因此可以同时使用多个
 * @return One on success, zero on error
 */
int OSSL_cng_provider_init(const OSSL_CORE_HANDLE *handle,
                       const OSSL_DISPATCH *in,
                       const OSSL_DISPATCH **out,
                       void **provctx) {
    cb_printf("PROV> OSSL_provider_init");
    *out = cng_dispatch_table;
    *provctx = malloc(sizeof(T_CNG_PROVIDER_CTX));
    if (*provctx == NULL) { return 0; }
    T_CNG_PROVIDER_CTX *tprovctx = *provctx;
    /* 保存核心功能，以便我们以后使用 */
    tprovctx->core_functions = in;
    return 1;
}
