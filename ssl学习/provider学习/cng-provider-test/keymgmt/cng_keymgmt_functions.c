#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ncrypt.h>
#include "cng_keymgmt_functions.h"


/**
 * 处理密钥复制问题
 * 通过密钥句柄获取密钥类型、提供者和名称，然后使用这些信息创建一个新的密钥句柄
 * 
 * @param old_key_handle 指向旧密钥句柄的指针，用于获取属性
 * @param new_key_handle 指向新密钥句柄的指针，用于存储创建的密钥句柄
 * @return 返回布尔值，非0表示成功，0表示失败
 */
int duplicate_key_handle(const NCRYPT_KEY_HANDLE *old_key_handle, NCRYPT_KEY_HANDLE *new_key_handle) {

    // 获取密钥类型标志
    DWORD key_type_flags;
    DWORD key_type_flags_expected_len = 0;
    SECURITY_STATUS ss2 = NCryptGetProperty(*old_key_handle, NCRYPT_KEY_TYPE_PROPERTY, (PBYTE) &key_type_flags,
                                            sizeof(key_type_flags), &key_type_flags_expected_len, 0);
    if (ss2 != ERROR_SUCCESS || sizeof(key_type_flags) != key_type_flags_expected_len) {
        return 0;
    }

    // 获取密钥提供者
    NCRYPT_PROV_HANDLE key_provider_handle;
    DWORD key_provider_handle_expected_len = 0;
    ss2 = NCryptGetProperty(*old_key_handle, NCRYPT_PROVIDER_HANDLE_PROPERTY, (PBYTE) &key_provider_handle,
                            sizeof(key_provider_handle), &key_provider_handle_expected_len, 0);
    if (ss2 != ERROR_SUCCESS || sizeof(key_provider_handle) != key_provider_handle_expected_len) {
        return 0;
    }

    // 获取密钥名称
    LPWSTR key_name = NULL;
    DWORD key_name_len = 0;
    DWORD key_name_expected_len = 0;
    ss2 = NCryptGetProperty(*old_key_handle, NCRYPT_NAME_PROPERTY, (PBYTE) key_name, key_name_len,
                            &key_name_expected_len, 0);
    if (ss2 != ERROR_SUCCESS) {
        return 0;
    }
    key_name = malloc(key_name_expected_len);
    key_name_len = key_name_expected_len;
    ss2 = NCryptGetProperty(*old_key_handle, NCRYPT_NAME_PROPERTY, (PBYTE) key_name, key_name_len,
                            &key_name_expected_len, 0);
    if (ss2 != ERROR_SUCCESS) {
        free(key_name);
        return 0;
    }

    // 再次打开密钥
    ss2 = NCryptOpenKey(key_provider_handle, new_key_handle, key_name, 0, key_type_flags);
    free(key_name);
    return (ss2 == ERROR_SUCCESS);
}

/**
 * 根据RSA模数的位长度计算推荐的安全强度位数
 * 
 * @param rsa_modulus_bits RSA公钥模数的位长度
 * @return 根据给定的RSA模数位长度对应的安全强度位数
 * 
 * 该函数通过判断RSA模数的位长度，返回一个对应的安全强度位数
 * 这在密码学应用中很有用，因为不同的模数长度提供了不同级别的安全性
 * 请注意，这些对应关系基于当前的安全性评估，并可能随着时间的推移而变化
 */
int rsaModulusSizeToStrengthBits(DWORD rsa_modulus_bits) {
    int security_bits = 0;
    // 如果RSA模数位长度超过1024位，则推荐的安全强度位数为80位
    if (rsa_modulus_bits > 1024) { security_bits = 80; }
    // 如果RSA模数位长度超过2048位，则推荐的安全强度位数为112位
    if (rsa_modulus_bits > 2048) { security_bits = 112; }
    // 如果RSA模数位长度超过3072位，则推荐的安全强度位数为128位
    if (rsa_modulus_bits > 3072) { security_bits = 128; }
    // 如果RSA模数位长度超过7680位，则推荐的安全强度位数为192位
    if (rsa_modulus_bits > 7680) { security_bits = 192; }
    // 如果RSA模数位长度超过15360位，则推荐的安全强度位数为256位
    if (rsa_modulus_bits > 15360) { security_bits = 256; }
    // 返回根据RSA模数位长度对应的安全强度位数
    return security_bits;
}

/**
 * 该函数用于创建并返回一个反转内容的字节缓冲区副本。
 * 
 * @param buffer 原始字节缓冲区的指针。
 * @param buffer_size 原始字节缓冲区的大小。
 * @return 返回一个指向反转后字节缓冲区的指针。如果内存分配失败，返回NULL。
 *
 * 说明：
 * 1. 函数首先使用malloc为反转后的缓冲区分配内存。
 * 2. 如果内存分配失败，函数将返回NULL。
 * 3. 使用一个循环将原始缓冲区的内容反转复制到新分配的缓冲区中。
 * 4. 最后返回指向反转后缓冲区的指针。
 */
unsigned char *duplicate_buffer_in_reverse(const unsigned char *buffer, const ULONG buffer_size) {
    // 分配内存以存储反转后的缓冲区内容
    unsigned char *reverse_buffer = malloc(buffer_size);
    // 如果内存分配失败，返回NULL
    if (reverse_buffer == NULL) { return NULL; }
    
    // 通过循环将原始缓冲区的内容反转并复制到新缓冲区
    for (int i = 0; i < buffer_size; i++) {
        reverse_buffer[i] = *(buffer + buffer_size - i - 1);
    }
    
    // 返回指向反转后缓冲区的指针
    return reverse_buffer;
}

/**
 * 根据密钥句柄获取密钥名称
 * 
 * @param key 指向T_CNG_KEYMGMT_KEYDATA结构的指针，其中包含密钥信息
 * @return 返回密钥名称的宽字符指针，如果获取失败或分配内存失败，则返回NULL
 * 
 * 该函数通过NCryptGetProperty函数从Windows Crypto API (CNG)获取密钥的名称属性
 * 首先，调用NCryptGetProperty确定密钥名称的大小，然后根据返回的大小分配内存
 * 再次调用NCryptGetProperty填充密钥名称，最后将得到的名称转换为宽字符指针返回
 */
wchar_t *get_key_name(T_CNG_KEYMGMT_KEYDATA *key) {
    DWORD size;
    SECURITY_STATUS ss = NCryptGetProperty(key->windows_key_handle, NCRYPT_NAME_PROPERTY, NULL, 0, &size, 0);
    if (ss == ERROR_SUCCESS) {
        unsigned char *name = malloc(size);
        if (name == NULL) { return NULL; }
        NCryptGetProperty(key->windows_key_handle, NCRYPT_NAME_PROPERTY, name, size, &size, 0);
        return (wchar_t *) name;
    }
    return NULL;
}

/**
 * 创建一个新的CNG密钥管理对象
 * 
 * 该函数负责分配内存并初始化一个CNG密钥管理对象，该对象用于后续的密钥操作
 * 
 * @param provctx void* 类型的参数，表示提供者上下文在此函数中未使用
 * @return 返回一个指向新创建的CNG密钥管理对象的指针如果内存分配失败，返回NULL
 */
void *cng_keymgmt_new(void *provctx) {
    // 打印函数入口信息，用于调试和日志记录
    cb_printf("KEYMGMT> cng_keymgmt_new"); //xcs3  xcs17 xcs26 xcs44 xcs58

    // 分配内存用于存储CNG密钥管理对象的数据
    T_CNG_KEYMGMT_KEYDATA *keydata = malloc(sizeof(T_CNG_KEYMGMT_KEYDATA));
    // 检查内存分配是否成功，如果失败则返回NULL
    if (keydata == NULL) { return NULL; }

    // 初始化新分配的CNG密钥管理对象的窗口键句柄为0
    keydata->windows_key_handle = 0;

    // 返回指向新创建的CNG密钥管理对象的指针
    return keydata;
}

/**
 * cng_keymgmt_dup - 复制给定的密钥管理数据。
 * @keydata_from: 指向源密钥管理数据的指针。
 * @selection: 选择参数，目前未使用。
 * 
 * 该函数用于通过指定的密钥数据创建一个新的密钥管理数据实例。
 * 它主要负责复制Windows密钥句柄，并返回包含新密钥管理数据的指针。
 * 如果复制密钥句柄失败，函数返回NULL。
 */
void *cng_keymgmt_dup(const void *keydata_from, int selection) {
    cb_printf("KEYMGMT> cng_keymgmt_dup"); //xcs25 xcs57

    // 将传入的void指针转换为特定的类型，以便访问密钥管理数据结构。
    const T_CNG_KEYMGMT_KEYDATA *old_keydata = keydata_from;

    // 创建一个新的密钥管理数据结构，这里不使用提供者上下文。
    T_CNG_KEYMGMT_KEYDATA *new_keydata = cng_keymgmt_new(NULL);

    // 尝试复制旧密钥管理数据中的Windows密钥句柄到新创建的密钥管理数据中。
    // 如果复制失败，返回NULL。
    if (!duplicate_key_handle(&old_keydata->windows_key_handle, &new_keydata->windows_key_handle)) {
        return NULL;
    }

    // 返回包含新密钥管理数据的指针。
    return new_keydata;
}

/**
 * 释放密钥管理模块分配的内存资源
 * 
 * 该函数负责释放由密钥管理模块分配的内存资源，包括Windows加密密钥句柄和相关数据结构
 * 
 * @param vkeydata 由cng_keymgmt_alloc分配的内存块指针，包含必要的密钥信息
 * 
 * @note 该函数不返回任何值
 * 
 * 使用该函数时，需要确保所有对vkeydata的使用已经完成，以避免释放资源时的数据竞争或数据损坏问题
 */
void cng_keymgmt_free(void *vkeydata) {
    // 函数开始时打印信息，用于调试和日志记录
    cb_printf("KEYMGMT> cng_keymgmt_free"); //xcs14 xcs39 xcs45
    
    // 将void指针转换为特定的结构体指针，以便访问其成员
    T_CNG_KEYMGMT_KEYDATA *keydata = vkeydata;
    
    // 释放Windows加密服务提供的密钥句柄资源
    NCryptFreeObject(keydata->windows_key_handle);
    
    // 释放密钥管理模块之前分配的内存块
    free(keydata);
}

/**
 * 基于从与此提供程序兼容的函数接收到的对象（引用）构造提供程序端密钥对象。通常从提供程序STOREMGMT模块中store_load（）函数传递的OSSL_MAM数组接收。
 * 加载密钥管理功能的密钥。
 * 
 * 该函数根据给定的引用及其大小从存储中加载秘密密钥。这是纯提供者端的操作，
 * 使用提供的引用信息来检索和加载密钥。适用于需要根据引用信息从
 * 某种存储或缓存中获取密钥的情况。
 * 
 * @param reference 指向密钥引用的指针，具体类型由提供者定义和解释。
 * @param reference_sz 引用的大小，用于定位和加载密钥。
 * 
 * @return 返回一个指向加载的密钥的指针，或者在失败时返回 NULL。
 */
void *cng_keymgmt_load(const void *reference, size_t reference_sz) {
    cb_printf("KEYMGMT> cng_keymgmt_load"); //xcs24

    // 将传入的通用引用转换为特定的密钥数据引用类型。
    const T_CNG_KEYMGMT_KEYDATA *key = reference;

    // 调用密钥管理功能以复制并返回密钥。
    // 这里使用 OSSL_KEYMGMT_SELECT_ALL 标志表示选择密钥的所有可用部分。
    // 这是密钥加载和复制的实际操作，由提供者具体实现。
    return cng_keymgmt_dup(key, OSSL_KEYMGMT_SELECT_ALL);
}

/**
 * cng_keymgmt_get_params - 获取密钥管理参数
 * @keydata: 密钥数据指针
 * @params: 参数数组
 * 
 * 该函数负责填充关于密钥的一些基本参数，这些参数对于密钥的使用和管理是必要的。
 * 它主要与Windows的Cryptographic Next Generation (CNG)接口协同工作。
 * 
 * 返回: 成功则返回1，失败返回0。
 */
int cng_keymgmt_get_params(void *keydata, OSSL_PARAM params[]) {
    // 将keydata指针转换为相应的结构体指针
    T_CNG_KEYMGMT_KEYDATA *key = keydata;
    cb_printf("KEYMGMT> cng_keymgmt_get_params"); // 日志输出，表示进入函数

    // OSSL_PARAM的指针，用于定位和设置参数
    OSSL_PARAM *p;

    // 定位OSSL_PKEY_PARAM_BITS参数，设置其值为512
    p = OSSL_PARAM_locate(params, OSSL_PKEY_PARAM_BITS);
    /* 该值应该是密钥所属密码系统对应的密码长度，以位为单位。
     * 密码长度的定义特定于密钥的密码系统。 */
    if (p != NULL && !OSSL_PARAM_set_int(p, 512))
        return 0;

    // 定位OSSL_PKEY_PARAM_SECURITY_BITS参数，设置密钥的安全位数
    p = OSSL_PARAM_locate(params, OSSL_PKEY_PARAM_SECURITY_BITS);
    /* 该值应该是给定密钥的安全位数。
     * 安全位数的定义参见SP800-57。 */
    if (p != NULL) {
        DWORD key_length;
        DWORD received_bytes;
        // 从Windows密钥句柄中获取密钥长度属性
        SECURITY_STATUS ss = NCryptGetProperty(key->windows_key_handle, NCRYPT_LENGTH_PROPERTY, (PBYTE) &key_length,
                                               sizeof(key_length), &received_bytes, 0);
        if (ss != ERROR_SUCCESS || received_bytes != sizeof(key_length)) { return 0; }

        // 根据密钥长度计算并设置安全位数
        if (!OSSL_PARAM_set_int(p, rsaModulusSizeToStrengthBits(key_length))) { return 0; }
    }

    // 定位OSSL_PKEY_PARAM_MAX_SIZE参数，设置最大大小为4096
    p = OSSL_PARAM_locate(params, OSSL_PKEY_PARAM_MAX_SIZE);
    if (p != NULL && !OSSL_PARAM_set_int(p, 4096))
        return 0;

    // 成功完成参数设置
    return 1;
}

/**
 * cng_keymgmt_gettable_params函数返回一个参数类型表。
 * 这个函数是用于获取与密钥管理操作相关的参数类型信息。
 * 它主要用于内部处理，与特定的密码学操作或密钥管理任务相关联。
 * 
 * 参数:
 * provctx(void *): 提供的上下文指针，通常用于识别或配置特定的提供者或环境状态。
 * 
 * 返回值:
 * const OSSL_PARAM *: 返回一个指向OSSL_PARAM类型的指针数组，该数组定义了可获取的参数类型。
 */
const OSSL_PARAM *cng_keymgmt_gettable_params(void *provctx) {
    // 打印函数调用的日志信息
    cb_printf("KEYMGMT> cng_keymgmt_gettable_params");
    // 返回预定义的参数类型表
    return cng_keymgmt_param_types;
}

/**
 * 检查提供者端密钥对象是否具有关联的特定密钥部分
 * 
 * 此函数通过检查提供的密钥数据，确定其是否包含特定类型的密钥组件，
 * 如私钥、公钥或域参数等。它主要用于密钥管理场景，帮助决策者了解
 * 密钥数据集中的具体内容。
 * 
 * @param keydata 指向密钥数据的指针，该数据将被检查。
 * @param selection 一个指示需要检查的密钥组件类型的掩码。可以包括：
 *        - OSSL_KEYMGMT_SELECT_PRIVATE_KEY: 检查私钥。
 *        - OSSL_KEYMGMT_SELECT_PUBLIC_KEY: 检查公钥。
 *        - OSSL_KEYMGMT_SELECT_DOMAIN_PARAMETERS: 检查域参数。
 *        - OSSL_KEYMGMT_SELECT_OTHER_PARAMETERS: 检查其他参数。
 * 
 * @return 总是返回1，表示我们的密钥总是包含有效的公钥和私钥部分。
 *         注意：这种实现假设密钥数据总是有效的，并且不需要进一步检查。
 */
int cng_keymgmt_has(const void *keydata, int selection) {
    cb_printf("KEYMGMT> cng_keymgmt_has key structure");
    // 根据selection参数检查并打印相应的密钥组件信息
    if (selection & OSSL_KEYMGMT_SELECT_PRIVATE_KEY) { cb_printf(" private key?"); }
    if (selection & OSSL_KEYMGMT_SELECT_PUBLIC_KEY) { cb_printf(" public key?"); }
    if (selection & OSSL_KEYMGMT_SELECT_DOMAIN_PARAMETERS) { cb_printf(" domain parameters?"); }
    if (selection & OSSL_KEYMGMT_SELECT_OTHER_PARAMETERS) { cb_printf(" other parameters?"); }
    // 我们的密钥总是有有效的公共和私有部分，额外的信息没有意义，所以我们总是返回true
    return 1;
}

/**
 * 导出密钥管理功能的密钥数据。
 * 
 * @param keydata 指向密钥数据的指针。
 * @param selection 一个整数，指示要导出的密钥类型（私钥、公钥等）。
 * @param param_cb 回调函数，用于处理导出的参数。
 * @param cbarg 回调函数的额外参数。
 * 
 * @return 返回一个整数，0 表示导出失败，非0表示导出成功。
 */
int cng_keymgmt_export(void *keydata, int selection,
                       OSSL_CALLBACK *param_cb, void *cbarg) {
    cb_printf("KEYMGMT> cng_keymgmt_export"); //xcs33  xcs35

    T_CNG_KEYMGMT_KEYDATA *cng_keydata = (T_CNG_KEYMGMT_KEYDATA *) keydata;
    OSSL_PARAM *p = NULL;
    OSSL_PARAM params[3];

    // 检查是否请求导出私钥
    if (selection & OSSL_KEYMGMT_SELECT_PRIVATE_KEY) {
        cb_printf("KEYMGMT> Somebody wants private key exported");//有人想导出私钥
        /* 不支持导出私钥 */
        return 0;
    }

    // 检查是否请求导出公钥
    if (selection & OSSL_KEYMGMT_SELECT_PUBLIC_KEY) {
        cb_printf("KEYMGMT> Somebody wants public key exported");
        DWORD public_blob_size; // 公钥blob的大小
        SECURITY_STATUS ss = NCryptExportKey(cng_keydata->windows_key_handle, 0, BCRYPT_RSAPUBLIC_BLOB, NULL, NULL, 0,
                                             &public_blob_size, 0);
        if (ss != ERROR_SUCCESS) { return 0; }

        DWORD publicBlobExpectedSize = public_blob_size;
        PBYTE public_blob = malloc(public_blob_size);
        if (public_blob == NULL) { return 0; }

        ss = NCryptExportKey(cng_keydata->windows_key_handle, 0, BCRYPT_RSAPUBLIC_BLOB, NULL, public_blob,
                             publicBlobExpectedSize, &public_blob_size, 0);
        if (ss != ERROR_SUCCESS) {
            free(public_blob);
            return 0;
        }

        BCRYPT_RSAKEY_BLOB *pb = (BCRYPT_RSAKEY_BLOB *) public_blob;

        // 反转公钥指数和模量的字节序
        unsigned char *rsa_public_exp_little_endian = duplicate_buffer_in_reverse(
                public_blob + sizeof(BCRYPT_RSAKEY_BLOB), pb->cbPublicExp);
        if (rsa_public_exp_little_endian == NULL) {
            free(public_blob);
            return 0;
        }

        unsigned char *rsa_modulus_little_endian = duplicate_buffer_in_reverse(
                public_blob + sizeof(BCRYPT_RSAKEY_BLOB) + pb->cbPublicExp, pb->cbModulus);
        if (rsa_modulus_little_endian == NULL) {
            free(public_blob);
            free(rsa_public_exp_little_endian);
            return 0;
        }

        // 构造导出的公钥参数
        params[0] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_RSA_E, rsa_public_exp_little_endian,
                                            pb->cbPublicExp);
        params[1] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_RSA_N, rsa_modulus_little_endian,
                                            pb->cbModulus);
        params[2] = OSSL_PARAM_construct_end();
        p = params;
    }

    // 检查是否请求导出域参数
    if (selection & OSSL_KEYMGMT_SELECT_DOMAIN_PARAMETERS) {
        cb_printf("KEYMGMT>  Somebody wants domain parameters exported");//有人想导出域参数
        if (p == NULL) { return 0; }
    }

    // 检查是否请求导出其他参数
    if (selection & OSSL_KEYMGMT_SELECT_OTHER_PARAMETERS) {
        cb_printf("KEYMGMT> Somebody wants other parameters exported"); //有人想导出其他参数
        if (p == NULL) { return 0; }
    }

    // 调用回调函数处理导出的参数
    return param_cb(p, cbarg);
}

/**
 * 根据选型参数返回密钥管理对象的导出参数表
 * 
 * @param selection 指定的选型参数，用于确定哪种类型的密钥参数表需要被返回
 * 
 * @return 返回对应的OSSL_PARAM参数表，如果selection中包含OSSL_KEYMGMT_SELECT_PUBLIC_KEY标志，则返回RSA公钥参数表；否则返回NULL
 * 
 * 此函数主要用于响应不同类型的密钥管理请求，根据selection参数判断是否需要准备公钥参数表
 * 如果需要，将返回一个静态的、包含RSA公钥参数（e和n）的OSSL_PARAM表，并以OSSL_PARAM_END标志结束
 * 如果selection不包含公钥选型标志，则返回NULL，表示不需要导出公钥参数
 */
const OSSL_PARAM *cng_keymgmt_export_types(int selection) {
    cb_printf("KEYMGMT> cng_keymgmt_export_types");
    if (selection & OSSL_KEYMGMT_SELECT_PUBLIC_KEY) {
        static const OSSL_PARAM export_param_table[] = {
                OSSL_PARAM_BN(OSSL_PKEY_PARAM_RSA_E, NULL, 0),
                OSSL_PARAM_BN(OSSL_PKEY_PARAM_RSA_N, NULL, 0),
                OSSL_PARAM_END
        };
        return export_param_table;
    } else {
        return NULL;
    }
}

// /**
//  * 返回 _keymgmt_export() 可以导出的数据
//  * 
//  * [1] https://www.openssl.org/docs/manmaster/man7/provider-keymgmt.html
//  * @param provctx 提供程序上下文
//  * @param selection 选择位，如 [1] 所述
//  * @return _keymgmt_export() 可以返回的 OSSL_PARAM 数组
//  */
// const OSSL_PARAM *cng_keymgmt_export_types_ex(void *provctx, int selection) {
//     cb_printf("KEYMGMT> cng_keymgmt_export_types_ex");
//     return cng_keymgmt_export_types(selection);
// }

/* OSSL_DISPATCH 数组，包含从 OpenSSL 库可调用的提供程序函数 */
const OSSL_DISPATCH cng_keymgmt_functions[] = {
    {OSSL_FUNC_KEYMGMT_NEW,             (void (*)(void)) cng_keymgmt_new},   // 创建一个新的密钥管理对象
    {OSSL_FUNC_KEYMGMT_DUP,             (void (*)(void)) cng_keymgmt_dup},   // 复制一个现有的密钥管理对象
    {OSSL_FUNC_KEYMGMT_FREE,            (void (*)(void)) cng_keymgmt_free},  // 释放一个密钥管理对象
    {OSSL_FUNC_KEYMGMT_LOAD,            (void (*)(void)) cng_keymgmt_load},  // 加载一个已存在的密钥管理对象
    {OSSL_FUNC_KEYMGMT_GET_PARAMS,      (void (*)(void)) cng_keymgmt_get_params},  // 获取密钥管理对象的参数
    {OSSL_FUNC_KEYMGMT_GETTABLE_PARAMS, (void (*)(void)) cng_keymgmt_gettable_params},  // 获取密钥管理对象的表格参数
    {OSSL_FUNC_KEYMGMT_HAS,             (void (*)(void)) cng_keymgmt_has},   // 检查密钥管理对象是否支持某个功能
    {OSSL_FUNC_KEYMGMT_EXPORT,          (void (*)(void)) cng_keymgmt_export},  // 导出密钥管理对象的数据
    {OSSL_FUNC_KEYMGMT_EXPORT_TYPES,    (void (*)(void)) cng_keymgmt_export_types},  // 获取支持的导出类型
//    {OSSL_FUNC_KEYMGMT_EXPORT_TYPES_EX,    (void (*)(void)) cng_keymgmt_export_types_ex},  // 获取扩展的导出类型（未实现）
    {0, NULL}  // 结束标志
};