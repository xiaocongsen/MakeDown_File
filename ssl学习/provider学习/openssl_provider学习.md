# provider
OpenSSL 3.x 引入了一个模块化的架构，允许不同的算法和功能通过不同的模块来实现。这些模块可以是内置的，也可以是外部提供的。根据 OpenSSL 3.x 的设计，有几个主要的模块类别，包括但不限于：

    keymgmt 模块 - 用于管理密钥生命周期的各种操作，如创建、加载、导出、验证等。
    cipher 模块 - 用于实现加密和解密操作。
    digest 模块 - 用于实现消息摘要算法。
    signature 模块 - 用于实现数字签名功能。
    dh 模块 - 用于实现 Diffie-Hellman 密钥交换协议。
    kdf 模块 - 用于实现密钥派生功能。
    pkey 模块 - 用于实现公钥操作。
    store 模块 - 用于管理存储机制，如证书存储和密钥存储。
    rand 模块 - 用于实现随机数生成。
    paramgen 模块 - 用于生成算法参数。
    pkeyutl 模块 - 提供公钥实用工具。
除了上述主要模块之外，还有其他的辅助模块和支持模块。每个模块都有自己的功能和接口，例如 OSSL_FUNC_* 函数，这些函数定义了模块之间的交互方式。

## 入口函数

int OSSL_provider_init(const OSSL_CORE_HANDLE *handle, const OSSL_DISPATCH *in, const OSSL_DISPATCH **out, void **provctx)      
提供程序的入口点，用于动态和静态加载
handle: 指向核心 OpenSSL 的句柄，用于调用核心 OpenSSL 提供的功能。
in: 是一个包含从核心传入的函数指针数组，provider 可以通过这些函数与 OpenSSL 核心交互。
out: 是一个输出参数，provider 必须填充一个函数指针数组，包含 provider 提供的实现。
provctx: 这个参数用于传递 provider 的上下文数据，它可以被用来在 provider 的生命周期内保持一些状态信息。


## Key Management
这些函数是 OpenSSL 中 keymgmt 模块的一部分，用于管理密钥生命周期的各种操作，如创建、加载、导出、验证等。下面是对这些函数的详细解释：

密钥管理（Key Management）
创建和销毁
OSSL_FUNC_keymgmt_new (OSSL_FUNC_KEYMGMT_NEW):
用途：创建一个新的密钥管理对象。
描述：这个函数用于初始化一个密钥管理对象，并设置其初始状态。
参数：通常接受一些配置选项，比如密钥类型、长度等。
返回值：返回一个密钥管理对象的指针。
OSSL_FUNC_keymgmt_free (OSSL_FUNC_KEYMGMT_FREE):
用途：释放一个密钥管理对象。
描述：这个函数用于清理并释放与密钥管理对象相关的资源。
参数：接受一个密钥管理对象的指针作为输入。
返回值：无。
密钥生成
OSSL_FUNC_keymgmt_gen_init (OSSL_FUNC_KEYMGMT_GEN_INIT):
用途：初始化密钥生成过程。
描述：这个函数用于准备密钥生成所需的资源。
参数：通常接受一些配置选项，比如密钥类型、长度等。
返回值：返回一个密钥生成上下文的指针。
OSSL_FUNC_keymgmt_gen_set_template (OSSL_FUNC_KEYMGMT_GEN_SET_TEMPLATE):
用途：设置密钥生成模板。
描述：这个函数用于指定密钥生成的模板参数，比如密钥类型、长度等。
参数：接受一个密钥生成上下文的指针和一个模板参数结构体。
返回值：无。
OSSL_FUNC_keymgmt_gen_get_params (OSSL_FUNC_KEYMGMT_GEN_GET_PARAMS):
用途：获取密钥生成的当前参数。
描述：这个函数用于获取密钥生成过程中的当前参数设置。
参数：接受一个密钥生成上下文的指针和一个参数结构体。
返回值：返回获取结果的状态。
OSSL_FUNC_keymgmt_gen_gettable_params (OSSL_FUNC_KEYMGMT_GEN_GETTABLE_PARAMS):
用途：获取可获取的密钥生成参数列表。
描述：这个函数用于获取密钥生成过程中可以获取的参数列表。
参数：接受一个密钥生成上下文的指针。
返回值：返回一个参数列表。
OSSL_FUNC_keymgmt_gen_set_params (OSSL_FUNC_KEYMGMT_GEN_SET_PARAMS):
用途：设置密钥生成参数。
描述：这个函数用于设置密钥生成过程中的参数。
参数：接受一个密钥生成上下文的指针和一个参数结构体。
返回值：返回设置结果的状态。
OSSL_FUNC_keymgmt_gen_settable_params (OSSL_FUNC_KEYMGMT_GEN_SETTABLE_PARAMS):
用途：获取可设置的密钥生成参数列表。
描述：这个函数用于获取密钥生成过程中可以设置的参数列表。
参数：接受一个密钥生成上下文的指针。
返回值：返回一个参数列表。
OSSL_FUNC_keymgmt_gen (OSSL_FUNC_KEYMGMT_GEN):
用途：执行密钥生成。
描述：这个函数用于执行密钥生成过程。
参数：接受一个密钥生成上下文的指针。
返回值：返回生成结果的状态。
OSSL_FUNC_keymgmt_gen_cleanup (OSSL_FUNC_KEYMGMT_GEN_CLEANUP):
用途：清理密钥生成过程。
描述：这个函数用于清理密钥生成过程中分配的资源。
参数：接受一个密钥生成上下文的指针。
返回值：无。
密钥加载
OSSL_FUNC_keymgmt_load (OSSL_FUNC_KEYMGMT_LOAD):
用途：从外部来源加载密钥。
描述：这个函数用于从文件、存储或其他外部来源加载密钥到密钥管理对象中。
参数：接受一个密钥管理对象的指针和加载选项。
返回值：返回加载结果的状态。
密钥操作
OSSL_FUNC_keymgmt_get_params (OSSL_FUNC_KEYMGMT_GET_PARAMS):
用途：获取密钥管理对象的当前参数。
描述：这个函数用于获取密钥管理对象中的当前参数设置。
参数：接受一个密钥管理对象的指针和一个参数结构体。
返回值：返回获取结果的状态。
OSSL_FUNC_keymgmt_gettable_params (OSSL_FUNC_KEYMGMT_GETTABLE_PARAMS):
用途：获取可获取的密钥管理参数列表。
描述：这个函数用于获取密钥管理对象中可以获取的参数列表。
参数：接受一个密钥管理对象的指针。
返回值：返回一个参数列表。
OSSL_FUNC_keymgmt_set_params (OSSL_FUNC_KEYMGMT_SET_PARAMS):
用途：设置密钥管理对象的参数。
描述：这个函数用于设置密钥管理对象中的参数。
参数：接受一个密钥管理对象的指针和一个参数结构体。
返回值：返回设置结果的状态。
OSSL_FUNC_keymgmt_settable_params (OSSL_FUNC_KEYMGMT_SETTABLE_PARAMS):
用途：获取可设置的密钥管理参数列表。
描述：这个函数用于获取密钥管理对象中可以设置的参数列表。
参数：接受一个密钥管理对象的指针。
返回值：返回一个参数列表。
OSSL_FUNC_keymgmt_query_operation_name (OSSL_FUNC_KEYMGMT_QUERY_OPERATION_NAME):
用途：查询支持的操作名称。
描述：这个函数用于查询密钥管理对象支持的操作名称。
参数：接受一个密钥管理对象的指针和操作名称。
返回值：返回操作名称的状态。
OSSL_FUNC_keymgmt_has (OSSL_FUNC_KEYMGMT_HAS):
用途：检查密钥管理对象是否支持某个操作。
描述：这个函数用于检查密钥管理对象是否支持某个特定的操作。
参数：接受一个密钥管理对象的指针和操作名称。
返回值：返回支持状态。
OSSL_FUNC_keymgmt_validate (OSSL_FUNC_KEYMGMT_VALIDATE):
用途：验证密钥管理对象的状态。
描述：这个函数用于验证密钥管理对象的当前状态是否有效。
参数：接受一个密钥管理对象的指针。
返回值：返回验证结果的状态。
OSSL_FUNC_keymgmt_match (OSSL_FUNC_KEYMGMT_MATCH):
用途：检查密钥管理对象是否匹配特定条件。
描述：这个函数用于检查密钥管理对象是否满足某些条件。
参数：接受一个密钥管理对象的指针和条件参数。
返回值：返回匹配状态。
密钥导入和导出
OSSL_FUNC_keymgmt_import (OSSL_FUNC_KEYMGMT_IMPORT):
用途：从外部格式导入密钥。
描述：这个函数用于从外部格式（如 PEM、DER 等）导入密钥到密钥管理对象中。
参数：接受一个密钥管理对象的指针和导入数据。
返回值：返回导入结果的状态。
OSSL_FUNC_keymgmt_import_types (OSSL_FUNC_KEYMGMT_IMPORT_TYPES):
用途：获取支持的密钥导入类型。
描述：这个函数用于获取密钥管理对象支持的密钥导入类型。
参数：接受一个密钥管理对象的指针。
返回值：返回导入类型列表。
OSSL_FUNC_keymgmt_import_types_ex (OSSL_FUNC_KEYMGMT_IMPORT_TYPES_EX):
用途：获取支持的密钥导入类型（扩展版本）。
描述：这个函数用于获取密钥管理对象支持的密钥导入类型，可能包括更详细的选项。
参数：接受一个密钥管理对象的指针。
返回值：返回导入类型列表。
OSSL_FUNC_keymgmt_export (OSSL_FUNC_KEYMGMT_EXPORT):
用途：将密钥导出为外部格式。
描述：这个函数用于将密钥管理对象中的密钥导出为外部格式（如 PEM、DER 等）。
参数：接受一个密钥管理对象的指针和导出选项。
返回值：返回导出结果的状态。
OSSL_FUNC_keymgmt_export_types (OSSL_FUNC_KEYMGMT_EXPORT_TYPES):
用途：获取支持的密钥导出类型。
描述：这个函数用于获取密钥管理对象支持的密钥导出类型。
参数：接受一个密钥管理对象的指针。
返回值：返回导出类型列表。
OSSL_FUNC_keymgmt_export_types_ex (OSSL_FUNC_KEYMGMT_EXPORT_TYPES_EX):
用途：获取支持的密钥导出类型（扩展版本）。
描述：这个函数用于获取密钥管理对象支持的密钥导出类型，可能包括更详细的选项。
参数：接受一个密钥管理对象的指针。
返回值：返回导出类型列表。
复制
OSSL_FUNC_keymgmt_dup (OSSL_FUNC_KEYMGMT_DUP):
用途：复制密钥管理对象。
描述：这个函数用于复制一个密钥管理对象。
参数：接受一个密钥管理对象的指针。
返回值：返回一个新的密钥管理对象的指针。
总结
这些函数提供了一套全面的工具，用于管理和操作密钥。它们涵盖了从创建密钥、设置和获取参数，到验证密钥的有效性、导入和导出密钥，再到清理资源等各个方面。通过这些函数，你可以轻松地集成密钥管理功能到你的应用中。




## 这些函数是 OpenSSL 中 signature 模块的一部分，用于实现数字签名的功能。下面是对这些函数的详细解释：

数字签名上下文管理
创建和销毁
OSSL_FUNC_signature_newctx (OSSL_FUNC_SIGNATURE_NEWCTX):
用途：创建一个新的签名上下文。
描述：这个函数用于初始化一个签名上下文，通常用于签名或验证操作的准备。
参数：通常接受一些配置选项，比如密钥、算法等。
返回值：返回一个签名上下文的指针。
OSSL_FUNC_signature_freectx (OSSL_FUNC_SIGNATURE_FREECTX):
用途：释放一个签名上下文。
描述：这个函数用于清理并释放与签名上下文相关的资源。
参数：接受一个签名上下文的指针作为输入。
返回值：无。
OSSL_FUNC_signature_dupctx (OSSL_FUNC_SIGNATURE_DUPCTX):
用途：复制一个签名上下文。
描述：这个函数用于创建一个签名上下文的副本。
参数：接受一个签名上下文的指针作为输入。
返回值：返回一个新的签名上下文的指针。
签名操作
签名初始化
OSSL_FUNC_signature_sign_init (OSSL_FUNC_SIGNATURE_SIGN_INIT):
用途：初始化签名操作。
描述：这个函数用于准备签名操作的上下文，设置好密钥和其他必要参数。
参数：接受一个签名上下文的指针作为输入。
返回值：返回初始化结果的状态。
执行签名
OSSL_FUNC_signature_sign (OSSL_FUNC_SIGNATURE_SIGN):
用途：执行签名操作。
描述：这个函数用于对数据进行签名。
参数：接受一个签名上下文的指针和数据作为输入。
返回值：返回签名结果的状态。
验证操作
验证初始化
OSSL_FUNC_signature_verify_init (OSSL_FUNC_SIGNATURE_VERIFY_INIT):
用途：初始化验证操作。
描述：这个函数用于准备验证操作的上下文，设置好公钥和其他必要参数。
参数：接受一个签名上下文的指针作为输入。
返回值：返回初始化结果的状态。
执行验证
OSSL_FUNC_signature_verify (OSSL_FUNC_SIGNATURE_VERIFY):
用途：执行验证操作。
描述：这个函数用于验证签名数据的有效性。
参数：接受一个签名上下文的指针和签名数据作为输入。
返回值：返回验证结果的状态。
验证恢复操作
验证恢复初始化
OSSL_FUNC_signature_verify_recover_init (OSSL_FUNC_SIGNATURE_VERIFY_RECOVER_INIT):
用途：初始化验证恢复操作。
描述：这个函数用于准备验证恢复操作的上下文，设置好公钥和其他必要参数。
参数：接受一个签名上下文的指针作为输入。
返回值：返回初始化结果的状态。
执行验证恢复
OSSL_FUNC_signature_verify_recover (OSSL_FUNC_SIGNATURE_VERIFY_RECOVER):
用途：执行验证恢复操作。
描述：这个函数用于从签名数据中恢复原始数据，并验证签名的有效性。
参数：接受一个签名上下文的指针和签名数据作为输入。
返回值：返回验证恢复结果的状态。
消息摘要签名操作
初始化
OSSL_FUNC_signature_digest_sign_init (OSSL_FUNC_SIGNATURE_DIGEST_SIGN_INIT):
用途：初始化消息摘要签名操作。
描述：这个函数用于准备消息摘要签名操作的上下文。
参数：接受一个签名上下文的指针作为输入。
返回值：返回初始化结果的状态。
更新
OSSL_FUNC_signature_digest_sign_update (OSSL_FUNC_SIGNATURE_DIGEST_SIGN_UPDATE):
用途：更新消息摘要签名操作。
描述：这个函数用于更新消息摘要签名操作中的消息摘要。
参数：接受一个签名上下文的指针和要更新的数据作为输入。
返回值：返回更新结果的状态。
最终化
OSSL_FUNC_signature_digest_sign_final (OSSL_FUNC_SIGNATURE_DIGEST_SIGN_FINAL):
用途：最终化消息摘要签名操作。
描述：这个函数用于完成消息摘要签名操作，并产生签名。
参数：接受一个签名上下文的指针作为输入。
返回值：返回最终化结果的状态。
OSSL_FUNC_signature_digest_sign (OSSL_FUNC_SIGNATURE_DIGEST_SIGN):
用途：执行消息摘要签名操作。
描述：这个函数用于一次性执行消息摘要签名操作。
参数：接受一个签名上下文的指针和要签名的数据作为输入。
返回值：返回签名结果的状态。
消息摘要验证操作
初始化
OSSL_FUNC_signature_digest_verify_init (OSSL_FUNC_SIGNATURE_DIGEST_VERIFY_INIT):
用途：初始化消息摘要验证操作。
描述：这个函数用于准备消息摘要验证操作的上下文。
参数：接受一个签名上下文的指针作为输入。
返回值：返回初始化结果的状态。
更新
OSSL_FUNC_signature_digest_verify_update (OSSL_FUNC_SIGNATURE_DIGEST_VERIFY_UPDATE):
用途：更新消息摘要验证操作。
描述：这个函数用于更新消息摘要验证操作中的消息摘要。
参数：接受一个签名上下文的指针和要更新的数据作为输入。
返回值：返回更新结果的状态。
最终化
OSSL_FUNC_signature_digest_verify_final (OSSL_FUNC_SIGNATURE_DIGEST_VERIFY_FINAL):
用途：最终化消息摘要验证操作。
描述：这个函数用于完成消息摘要验证操作，并验证签名。
参数：接受一个签名上下文的指针作为输入。
返回值：返回最终化结果的状态。
OSSL_FUNC_signature_digest_verify (OSSL_FUNC_SIGNATURE_DIGEST_VERIFY):
用途：执行消息摘要验证操作。
描述：这个函数用于一次性执行消息摘要验证操作。
参数：接受一个签名上下文的指针和要验证的数据作为输入。
返回值：返回验证结果的状态。
参数管理
获取和设置上下文参数
OSSL_FUNC_signature_get_ctx_params (OSSL_FUNC_SIGNATURE_GET_CTX_PARAMS):
用途：获取签名上下文的当前参数。
描述：这个函数用于获取签名上下文中的当前参数设置。
参数：接受一个签名上下文的指针和一个参数结构体。
返回值：返回获取结果的状态。
OSSL_FUNC_signature_gettable_ctx_params (OSSL_FUNC_SIGNATURE_GETTABLE_CTX_PARAMS):
用途：获取可获取的签名上下文参数列表。
描述：这个函数用于获取签名上下文中可以获取的参数列表。
参数：接受一个签名上下文的指针。
返回值：返回一个参数列表。
OSSL_FUNC_signature_set_ctx_params (OSSL_FUNC_SIGNATURE_SET_CTX_PARAMS):
用途：设置签名上下文的参数。
描述：这个函数用于设置签名上下文中的参数。
参数：接受一个签名上下文的指针和一个参数结构体。
返回值：返回设置结果的状态。
OSSL_FUNC_signature_settable_ctx_params (OSSL_FUNC_SIGNATURE_SETTABLE_CTX_PARAMS):
用途：获取可设置的签名上下文参数列表。
描述：这个函数用于获取签名上下文中可以设置的参数列表。
参数：接受一个签名上下文的指针。
返回值：返回一个参数列表。
获取和设置上下文消息摘要参数
OSSL_FUNC_signature_get_ctx_md_params (OSSL_FUNC_SIGNATURE_GET_CTX_MD_PARAMS):
用途：获取签名上下文的消息摘要参数。
描述：这个函数用于获取签名上下文中用于消息摘要的参数。
参数：接受一个签名上下文的指针和一个消息摘要参数结构体。
返回值：返回获取结果的状态。
OSSL_FUNC_signature_gettable_ctx_md_params (OSSL_FUNC_SIGNATURE_GETTABLE_CTX_MD_PARAMS):
用途：获取可获取的签名上下文消息摘要参数列表。
描述：这个函数用于获取签名上下文中可以获取的消息摘要参数列表。
参数：接受一个签名上下文的指针。
返回值：返回一个消息摘要参数列表。
OSSL_FUNC_signature_set_ctx_md_params (OSSL_FUNC_SIGNATURE_SET_CTX_MD_PARAMS):
用途：设置签名上下文的消息摘要参数。
描述：这个函数用于设置签名上下文中用于消息摘要的参数。
参数：接受一个签名上下文的指针和一个消息摘要参数结构体。
返回值：返回设置结果的状态。
OSSL_FUNC_signature_settable_ctx_md_params (OSSL_FUNC_SIGNATURE_SETTABLE_CTX_MD_PARAMS):
用途：获取可设置的签名上下文消息摘要参数列表。
描述：这个函数用于获取签名上下文中可以设置的消息摘要参数列表。
参数：接受一个签名上下文的指针。
返回值：返回一个消息摘要参数列表。
总结
这些函数提供了一套全面的工具，用于管理和操作数字签名。它们涵盖了从创建签名上下文、设置和获取参数，到执行签名和验证操作等各个方面。通过这些函数，你可以轻松地集成数字签名功能到你的应用中。