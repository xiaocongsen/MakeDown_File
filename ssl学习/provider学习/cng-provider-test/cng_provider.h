#pragma once

#include <openssl/core_dispatch.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <string.h>

// 定义CNG提供程序的名称
#define CNG_PROVIDER_NAME_STR "OpenSSL Cryptographic API: New Generation Provider"
// 定义CNG提供程序的版本
#define CNG_PROVIDER_VERSION_STR "0.1"
// 定义CNG提供程序的构建信息
#define CNG_PROVIDER_BUILDINFO_STR "Made for OpenSSL 3.2"

// 定义默认算法属性字符串，指定提供程序为cng_provider且不使用FIPS模式
static const char CNG_DEFAULT_ALG_PROPERTIES[] = "provider=cng_provider,fips=no";

/* OpenSSL用于获取提供程序信息（如版本或名称）的函数 */
static OSSL_FUNC_provider_get_params_fn cng_get_params;
/* 文档要求的存在，作为get_params()存在的指示，虽然在测试期间未调用 */
static OSSL_FUNC_provider_gettable_params_fn cng_gettable_params;
/* 向OpenSSL库返回此提供程序模块函数指针的函数 */
static OSSL_FUNC_provider_query_operation_fn cng_query_operation;
/* 关闭提供程序并销毁其上下文的函数 */
static OSSL_FUNC_provider_teardown_fn cng_teardown;

/* 其他核心提供程序函数 */
/* 通知提供程序不再需要query_operation()结果的函数 */
//static OSSL_FUNC_provider_unquery_operation_fn cng_unquery_operation;
/* 为core_put_error()提供原因字符串的函数 */
//static OSSL_FUNC_provider_get_reason_strings cng_get_reason_strings;
/* 不必枚举所有提供程序功能即可提供能力的函数，例如支持TLS1.3密码套件 */
//static OSSL_FUNC_provider_get_capabilities cng_get_capabilities;
/* 用于提供程序自身执行已知答案测试的函数 */
//static OSSL_FUNC_provider_self_test cng_self_test;

/** 我们提供给核心的参数 **/
// 定义提供程序参数类型数组，包括名称、版本、构建信息和状态等参数定义
static const OSSL_PARAM cng_param_types[] = {
        OSSL_PARAM_DEFN(OSSL_PROV_PARAM_NAME, OSSL_PARAM_UTF8_PTR, NULL, 0),
        OSSL_PARAM_DEFN(OSSL_PROV_PARAM_VERSION, OSSL_PARAM_UTF8_PTR, NULL, 0),
        OSSL_PARAM_DEFN(OSSL_PROV_PARAM_BUILDINFO, OSSL_PARAM_UTF8_PTR, NULL, 0),
        OSSL_PARAM_DEFN(OSSL_PROV_PARAM_STATUS, OSSL_PARAM_INTEGER, NULL, 0),
        OSSL_PARAM_END
};

/* 我们自定义提供者的上下文结构体 */
typedef struct s_cng_provider_ctx {
    const OSSL_DISPATCH *core_functions; /* 指向核心功能的函数指针数组 */
} T_CNG_PROVIDER_CTX;

/*
 * 检查CNG提供者是否正在运行
 * 返回: 1表示正在运行, 0表示未运行
 */
int cng_prov_is_running();

/*
 * 初始化CNG提供者
 * 参数:
 *   handle: 提供者的唯一标识符
 *   in: 提供者需要实现的函数列表
 *   out: 提供者实际实现的函数列表的指针，由提供者设置
 *   provctx: 提供者上下文的指针，由提供者使用
 * 返回:
 *   1: 初始化成功
 *   0: 初始化失败
 */
int OSSL_cng_provider_init(const OSSL_CORE_HANDLE *handle,
                       const OSSL_DISPATCH *in,
                       const OSSL_DISPATCH **out,
                       void **provctx);
