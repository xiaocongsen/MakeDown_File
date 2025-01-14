/*
 *  Copyright 2022 The OpenSSL Project Authors. All Rights Reserved.
 *
 *  Licensed under the Apache License 2.0 (the "License").  You may not use
 *  this file except in compliance with the License.  You can obtain a copy
 *  in the file LICENSE in the source distribution or at
 *  https://www.openssl.org/source/license.html
 *
 *  This source code has been modified to allow usage on Windows operating system
 *  by Ladislav Marko
 */

/**
 * This program connect via TLS to a server by given ip address and hostname and
 * sends its own client certificate
 */
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <stdio.h>
#include <string.h>
#include <openssl/provider.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SERVER_PORT 443
#define SERVER_IP "185.8.165.85"
#define SERVER_COMMON_NAME "cng.ladislavmarko.cz"
#define CNG_URI "cng://MY"
#define SEARCH_FACTOR NID_commonName
#define SEARCH_VALUE "xcs"


#include <openssl/trace.h>
#include <openssl/store.h>
#include <openssl/provider.h>
#include <openssl/core_names.h>
#include <openssl/core_dispatch.h>
#include <openssl/core.h>
#include <openssl/evp.h>


/**
 * 创建一个TCP套接字
 * 
 * 本函数尝试创建一个TCP套接字，以便后续进行网络通信
 * 使用AF_INET和SOCK_STREAM参数来指定使用IPv4协议和流式套接字
 * 
 * @return 返回创建的套接字的描述符
 *         如果创建失败，则在打印错误信息后退出程序
 */
SOCKET create_socket() {
    SOCKET s;
    // 创建一个TCP套接字
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        // 如果创建套接字失败，则打印错误信息并退出程序
        printf("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    return s;
}

/**
 * 创建并初始化一个SSL上下文。
 * SSL上下文定义了 SSL/TLS 协议的配置和环境，用于在客户端和服务器之间建立安全连接。
 *
 * @param libctx OpenSSL 库上下文，用于指定加密操作的提供程序。
 * @return 返回创建的SSL_CTX对象指针。如果创建失败，将返回NULL并打印错误信息后退出程序。
 *
 * 注意：这个函数使用了实验性的API（如SSL_CTX_new_ex），请确保在发布的产品中使用稳定版本的API。
 */
SSL_CTX *create_context(OSSL_LIB_CTX *libctx) {
    SSL_CTX *ctx;

    // 创建SSL上下文，指定使用"cng"提供程序进行加密操作。
    // "?provider=cng" 表示将这些配置选项应用于整个 SSL_CTX，而不是单个 SSL 对象。
    // 使用 TLS_client_method() 指定这是一个TLS客户端上下文。
    // 有关 "?provider=cng" 的详细讨论，请参考：
    // https://mta.openssl.org/pipermail/openssl-users/2023-February/015831.html
    ctx = SSL_CTX_new_ex(libctx, "?provider=cng", TLS_client_method());
    if (ctx == NULL) {
        // 如果创建SSL上下文失败，则打印错误信息并退出程序。
        printf("Unable to create SSL context");
        exit(EXIT_FAILURE);
    }
    return ctx;
}

/*
 * Callback function to log SSL/TLS secrets for later analysis
 */
void my_ssl_keylog_callback(const SSL *ssl, const char *line) {

    FILE *keylog_file = fopen("C:/sshkey/ssl_keys.log", "a");
    if (!keylog_file) {
        return;
    }

    fprintf(keylog_file, "%s\n", line);
    fflush(keylog_file);

    fclose(keylog_file);
}

/**
 * Compare common name of certificate with a string
 * @param cert Certificate to have its common name compared
 * @param common_name String to which the common name is compared to
 * @return 1 on match, 0 otherwise
 */
/**
 * 检查X.509证书的主体名称中是否包含指定的属性值。
 * 
 * @param cert X.509证书结构体指针。
 * @param nid 需要检查的属性的NID（名称标识符）。
 * @param common_name 需要检查的属性值字符串。
 * @return 如果证书中包含指定的属性值，返回1；否则返回0。
 */
int X509_has_attribute_value(X509 *cert, int nid, const char *common_name) {
    // 获取证书的主体名称
    X509_NAME *name = X509_get_subject_name(cert);
    // 如果主体名称为空，直接返回0
    if (name == NULL) { return 0; }

    // 根据NID查找属性的位置
    int loc = X509_NAME_get_index_by_NID(name, nid, -1);
    // 如果未找到指定的NID，或者检测到多个常用名称，直接返回0
    if (loc == -1) { return 0; }
    // 获取位置对应的ASN.1字符串数据
    ASN1_STRING *str = X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, loc));

    // 比较证书中的属性值与传入的值是否相等
    return (!strcmp(common_name, (char *) str->data));
}

int SSL_ERR_Log_Print_Cb(const char *str, size_t len, void *u)
{
    char* userlog = (char*)u;
    printf("PROGRAM>[");
    printf(" %s", userlog);
    printf(",%s]\n", str);
    // 清除之前的错误
    ERR_clear_error();
    return 1;
}

int find_and_use_client_certificate(const char *uri, OSSL_LIB_CTX *libctx, SSL_CTX *ssl_ctx,
                                    EVP_PKEY **public_key_of_certificate) {
    OSSL_STORE_CTX *ossl_store_ctx = OSSL_STORE_open_ex(uri, libctx, NULL, NULL,
                                                        NULL, NULL, NULL, NULL);
    if (ossl_store_ctx == NULL) { return 0; }
    BIO *stdoutbio = BIO_new_fd(_fileno(stdout), BIO_NOCLOSE);
    /* 枚举我们刚刚打开的存储中的证书 */
    while (!OSSL_STORE_eof(ossl_store_ctx)) {
        OSSL_STORE_INFO *info = OSSL_STORE_load(ossl_store_ctx);
        if (info == NULL) {
            BIO_free(stdoutbio);
            OSSL_STORE_close(ossl_store_ctx);
            return 0;
        }

        if (OSSL_STORE_INFO_get_type(info) == OSSL_STORE_INFO_CERT) {
            printf("PROGRAM> Found certificate in store\n");
            X509 *loaded_certificate = OSSL_STORE_INFO_get0_CERT(info);

            /* 打印证书的主题名称 */
            X509_NAME *a = X509_get_subject_name(loaded_certificate);
            X509_NAME_print(stdoutbio, a, 80);
            printf("\n");

            /* 检查它是否是我们想要的证书 */
            if (!X509_has_attribute_value(loaded_certificate, SEARCH_FACTOR, SEARCH_VALUE)) {
                printf("PROGRAM> The certificate does not match search factor. Skipping.\n");
                continue;
            }
            printf("PROGRAM> The certificate matches search factor. Trying to load it into SSL context.\n");
            /* 保存公钥，以便稍后将其与私钥进行比较 */
            *public_key_of_certificate = X509_get0_pubkey(loaded_certificate);
            printf("PROGRAM> Public key of the certificate is: ");
            EVP_PKEY_print_public(stdoutbio, *public_key_of_certificate, 1, NULL);
            /* Use this certificate for SSL/TLS */
//            if (!SSL_CTX_use_gm_certificate(ssl_ctx, loaded_certificate,SSL_USAGE_SIG)) {
            if (!SSL_CTX_use_certificate(ssl_ctx, loaded_certificate)) {
                ERR_print_errors_cb(SSL_ERR_Log_Print_Cb, "PROGRAM> The certificate cannot be loaded into SSL context"); //无法将证书加载到SSL上下文中
                break;
            }
            BIO_free(stdoutbio);
            OSSL_STORE_close(ossl_store_ctx);
            return 1;
        } else {
            printf("PROGRAM> Found something that is not a certificate, skipping\n");
        }
    }
    BIO_free(stdoutbio);
    OSSL_STORE_close(ossl_store_ctx);
    return 0;
}

int find_and_use_client_private_key(const char *uri, OSSL_LIB_CTX *libctx, SSL_CTX *ssl_ctx,
                                    EVP_PKEY *certificate_public_key) {
    OSSL_STORE_CTX *ossl_store_ctx = OSSL_STORE_open_ex(uri, libctx, NULL, NULL,
                                                        NULL, NULL, NULL, NULL);
    if (ossl_store_ctx == NULL) { return 0; }
    BIO *stdoutbio = BIO_new_fd(_fileno(stdout), BIO_NOCLOSE);

    /* 枚举我们刚刚打开的商店中的钥匙 */
    while (!OSSL_STORE_eof(ossl_store_ctx)) {
        OSSL_STORE_INFO *info = OSSL_STORE_load(ossl_store_ctx);
        if (info == NULL) {
            BIO_free(stdoutbio);
            OSSL_STORE_close(ossl_store_ctx);
            return 0;
        }

        if (OSSL_STORE_INFO_get_type(info) == OSSL_STORE_INFO_PKEY) {
            printf("PROGRAM> Found private key in store\n");
            EVP_PKEY *pkey = OSSL_STORE_INFO_get0_PKEY(info);
            printf("PROGRAM> Public key of the private key is: ");
            EVP_PKEY_print_public(stdoutbio, pkey, 1, NULL);
            /* Check that this is the private key of our certificate */
            if (!EVP_PKEY_eq(pkey, certificate_public_key)) {
                printf("PROGRAM> The public key does not match the public key of the certificate. Skipping.\n");
                continue;
            }
            printf("PROGRAM> The public keys of the loaded certificate and this private key match.\n");
            printf("PROGRAM> Trying to load this private key into SSL context.\n");


            /* Use this private key for SSL/TLS */
            if (!SSL_CTX_use_PrivateKey(ssl_ctx, pkey)) {
                printf("PROGRAM> Private key cannot be loaded into SSL context\n");
                BIO_free(stdoutbio);
                return 0;
            }
            BIO_free(stdoutbio);

            OSSL_STORE_close(ossl_store_ctx);
            return 1;
        }
    }
    BIO_free(stdoutbio);
    OSSL_STORE_close(ossl_store_ctx);
    return 0; // Could not find matching private key
}

#define TLS1_TXT_CIPHER_LIST_MATCH_GATEWAY "AES256-SHA:DES-CBC3-SHA:RC4-SHA:RC4-MD5:AES128-SHA:DES-CBC-SHA:ECDHE-RSA-AES256-SHA:ECDHE-RSA-AES128-SHA:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384"

/**
 * 配置客户端SSL上下文
 * 
 * 本函数用于在TLS（传输层安全性协议）连接中配置客户端SSL上下文以满足特定的安全和兼容性需求
 * 通过设置不同的参数，如验证模式、协议版本、安全级别和密码套件，来调整SSL上下文的行为
 * 
 * @param ssl_ctx 指向SSL_CTX结构的指针SSL_CTX是用于SSL/TLS通信的上下文它包含了用于SSL/TLS连接的所有必要信息和配置
 */
void configure_client_context(SSL_CTX *ssl_ctx) {
    // 禁用客户端验证服务器证书关闭SSL/TLS中的双向认证功能这在某些情况下可能会降低安全性，但可以简化连接过程
    SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, NULL);
    
    // 设置SSL上下文支持的最低TLS协议版本为TLS 1.3TLS 1.3是一个更安全、更快速的TLS版本，提供了改进的安全特性和性能
    SSL_CTX_set_min_proto_version(ssl_ctx, TLS1_3_VERSION);
    
    // 设置密钥日志回调函数为my_ssl_keylog_callback，用于记录SSL/TLS握手过程中交换的秘密信息这对于调试和安全审计很有用
    SSL_CTX_set_keylog_callback(ssl_ctx, my_ssl_keylog_callback);
    
    // 重置SSL上下文的安全等级此操作相当于取消了默认的安全强度检查，允许更灵活地配置加密套件
    SSL_CTX_set_security_level(ssl_ctx,0);
    
    // 设置SSL上下文使用推荐的密码套件列表，这个列表是根据特定的匹配网关要求设计的，以确保兼容性和安全性
    SSL_CTX_set_cipher_list(ssl_ctx, TLS1_TXT_CIPHER_LIST_MATCH_GATEWAY);
}
void provider_print_cb (const char *str){
    printf("[provider print cb][%s]\n",str);
}
#include "legacy_provider.h"
#include "cng_provider.h"
#include "debug.h"
int main()
{
    // 移除旧的SSL/TLS密钥文件，如果移除失败我们不是很关心
    remove("C:/sshkey/ssl_keys.log");

    OSSL_LIB_CTX *libctx;
    // 开始跟踪（调试功能，可能被注释掉）

    // 加载提供者 ----------------------------------------------------------------------------------
    OSSL_PROVIDER *prov_cng;
    OSSL_PROVIDER *prov_default;
    libctx = OSSL_LIB_CTX_new(); // NULL表示使用默认上下文

    // 注册内置提供者
    if (!OSSL_PROVIDER_add_builtin(libctx, "legacy", OSSL_mylegacy_provider_init)) {
        fprintf(stderr, "Failed to add built-in provider\n");
        return 1;
    }
    RegisterLogFunction(provider_print_cb);
    // 加载提供者
    prov_cng = OSSL_PROVIDER_load(libctx, "legacy");
    if (!prov_cng) {
        fprintf(stderr, "Failed to load provider\n");
        return 1;
    }

    // 注册内置提供者
    if (!OSSL_PROVIDER_add_builtin(libctx, "cng_provider", OSSL_cng_provider_init)) {
        fprintf(stderr, "Failed to add built-in provider\n");
        return 1;
    }
    RegisterLogFunction(provider_print_cb);
    // 加载提供者
    prov_cng = OSSL_PROVIDER_load(libctx, "cng_provider");
    if (!prov_cng) {
        fprintf(stderr, "Failed to load provider\n");
        return 1;
    }

    prov_default = OSSL_PROVIDER_load(libctx, "default"); // 获取默认提供者
    if (prov_default == NULL) {
        printf("PROGRAM> Failed to load default provider\n");
        OSSL_PROVIDER_unload(prov_cng);
        OSSL_PROVIDER_unload(prov_default);
        exit(EXIT_FAILURE);
    }

    // SSL连接开始 ----------------------------------------------------------------------------------------
    SSL_CTX *ssl_ctx = NULL;
    SSL *ssl = NULL;

    SOCKET client_skt = INVALID_SOCKET;

    char rxbuf[512];
    size_t rxcap = sizeof(rxbuf);
    int rxlen;

    struct sockaddr_in addr;
    printf("PROGRAM> We will connect to a remote server and check the SSL certificate\n");

    /* Microsoft Docs */
    // WSAStartup函数必须是应用程序或DLL调用的第一个Windows套接字函数。
    // - https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
    WSADATA wsaData;
    int err;

    err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0) {
        printf("PROGRAM> WSAStartup failed with error: %d\n", err);
        exit(1);
    }
    /* End of Microsoft Docs */

    // 创建客户端使用的SSL上下文
    ssl_ctx = create_context(libctx);

    // 配置客户端上下文以正确验证服务器
    configure_client_context(ssl_ctx);

    EVP_PKEY *pubkey = NULL;
    // 查找并使用客户端证书
    if (!find_and_use_client_certificate(CNG_URI, libctx, ssl_ctx, &pubkey)) {
        printf("PROGRAM> Could not find certificate with this search criteria in store\n");
        goto exit;
    }
    printf("PROGRAM> Certificate successfully loaded into SSL context\n");

    // 查找并使用客户端私钥
    if (!find_and_use_client_private_key(CNG_URI, libctx, ssl_ctx, pubkey)) {
        printf("PROGRAM> Could not find matching private key in store\n");
        goto exit;
    }
    printf("PROGRAM> Private key successfully loaded into SSL context\n");
    printf("PROGRAM> Setting ssl context is finished, now creating socket\n");

    // 创建“裸”套接字
    client_skt = create_socket();
    // 设置连接地址
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &addr.sin_addr.s_addr);
    addr.sin_port = htons(SERVER_PORT);
    // 与服务器建立TCP连接
    if (connect(client_skt, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        printf("PROGRAM> Unable to TCP connect to server");
        goto exit;
    } else {
        printf("PROGRAM> TCP connection to server successful\n");
    }

    // 使用专用客户端套接字创建客户端SSL结构
    ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, client_skt);
    // 设置SNI主机名
    SSL_set_tlsext_host_name(ssl, SERVER_COMMON_NAME);
    // 配置服务器主机名检查
    SSL_set1_host(ssl, SERVER_COMMON_NAME);

    // 与服务器建立SSL连接
    if (SSL_connect(ssl) == 1) {
        printf("PROGRAM> SSL connection to server successful\n\n");

        char *header = "GET / HTTP/1.1\nHost: ";
        char *end = "\n\n";
        size_t msg_len = strlen(header) + strlen(SERVER_COMMON_NAME) + strlen(end) + 1;
        char *msg = malloc(msg_len);
        if (msg == NULL) { exit(2); }
        snprintf(msg, msg_len, "%s%s%s", header, SERVER_COMMON_NAME, end);

        // 向服务器发送请求
        if (SSL_write(ssl, msg, msg_len) <= 0) {
            printf("PROGRAM> Server closed connection on write\n");
            free(msg);
            goto exit;
        }

        // 等待响应
        rxlen = SSL_read(ssl, rxbuf, rxcap);
        if (rxlen <= 0) {
            printf("PROGRAM> Server closed connection on read\n");
            free(msg);
            goto exit;
        } else {
            // 显示响应
            rxbuf[rxlen < rxcap ? rxlen : rxcap - 1] = 0;
            printf("Received: %s\n", rxbuf);
        }
        free(msg);
    } else {
        printf("PROGRAM> SSL connection to server failed\n\n");
        ERR_print_errors_fp(stderr);
    }

    exit:

    // 清理
    printf("PROGRAM> Client exiting...\n");
    if (ssl != NULL) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    SSL_CTX_free(ssl_ctx);

    if (client_skt != -1)
        closesocket(client_skt);

    WSACleanup();
    OSSL_PROVIDER_unload(prov_cng);
    OSSL_PROVIDER_unload(prov_default);
    OSSL_LIB_CTX_free(libctx);
    return 0;
}
