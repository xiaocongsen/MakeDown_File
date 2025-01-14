#pragma once

#include <string.h>
#include <stdio.h>
#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include "prov/provider_ctx.h"
#include "prov/implementations.h"
#include "prov/names.h"
#include "prov/providercommon.h"

// 声明获取可配置参数的函数接口
static OSSL_FUNC_provider_gettable_params_fn legacy_gettable_params;
// 声明设置参数的函数接口
static OSSL_FUNC_provider_get_params_fn legacy_get_params;
// 声明操作查询的函数接口
static OSSL_FUNC_provider_query_operation_fn legacy_query;

// 定义算法映射宏，用于将算法名称和提供者信息关联到指定功能函数
#define ALG(NAMES, FUNC) { NAMES, "provider=legacy", FUNC }

// 条件编译判断，用于静态链接场景
#ifdef STATIC_LEGACY
// 声明一个初始化函数指针，以便在静态链接时使用
OSSL_provider_init_fn ossl_legacy_provider_init;
// 定义一个宏，使得静态链接时可以使用该初始化函数
# define OSSL_provider_init ossl_legacy_provider_init
#endif

/* 我们提供给核心的参数定义
 *
 * 此数组定义了加密提供者核心期望接收的一组参数。这些参数提供了关于提供者本身的信息，
 * 如其名称、版本、构建信息以及状态。参数定义使用OSSL_PARAM宏族来设定每个参数的类型、
 * 名称和默认值。这些参数对于调试和信息收集非常有用。
 */
static const OSSL_PARAM legacy_param_types[] = {
    OSSL_PARAM_DEFN(OSSL_PROV_PARAM_NAME, OSSL_PARAM_UTF8_PTR, NULL, 0), /* 提供者的名称 */
    OSSL_PARAM_DEFN(OSSL_PROV_PARAM_VERSION, OSSL_PARAM_UTF8_PTR, NULL, 0), /* 提供者的版本 */
    OSSL_PARAM_DEFN(OSSL_PROV_PARAM_BUILDINFO, OSSL_PARAM_UTF8_PTR, NULL, 0), /* 提供者的构建信息 */
    OSSL_PARAM_DEFN(OSSL_PROV_PARAM_STATUS, OSSL_PARAM_INTEGER, NULL, 0), /* 提供者的状态 */
    OSSL_PARAM_END /* 参数列表的结束标志 */
};


int OSSL_mylegacy_provider_init(const OSSL_CORE_HANDLE *handle,
                       const OSSL_DISPATCH *in,
                       const OSSL_DISPATCH **out,
                       void **provctx);

