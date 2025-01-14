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


SOCKET create_socket() {
    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        printf("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    return s;
}

SSL_CTX *create_context(OSSL_LIB_CTX *libctx) {
    SSL_CTX *ctx;

    ctx = SSL_CTX_new_ex(libctx, "?provider=cng", TLS_client_method()); //?用于指示这些配置选项应该应用于整个 SSL_CTX 而不是单个 SSL 对象, provider=cng 指定了提供程序的名字 在进行 SSL/TLS 操作时，将使用 "cng" 提供程序来执行加密操作。
    // The question mark means that our provider is preferred
    // https://mta.openssl.org/pipermail/openssl-users/2023-February/015831.html
    if (ctx == NULL) {
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
int X509_has_attribute_value(X509 *cert, int nid, const char *common_name) {
    X509_NAME *name = X509_get_subject_name(cert);
    if (name == NULL) { return 0; }

    int loc = X509_NAME_get_index_by_NID(name, nid, -1);
    if (loc == -1) { return 0; } //检测到多个常用名称
    ASN1_STRING *str = X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, loc));

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

void configure_client_context(SSL_CTX *ssl_ctx) {
    SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, NULL);
    SSL_CTX_set_min_proto_version(ssl_ctx, TLS1_3_VERSION);
    SSL_CTX_set_keylog_callback(ssl_ctx, my_ssl_keylog_callback);
    SSL_CTX_set_security_level(ssl_ctx,0);
    SSL_CTX_set_cipher_list(ssl_ctx, TLS1_TXT_CIPHER_LIST_MATCH_GATEWAY);
}
void provider_print_cb (const char *str){
    printf("[provider print cb][%s]\n",str);
}

int main() //加载dll
{
    // Remove old file with SSL/TLS secrets, we do not care much if this fails
    remove("C:/sshkey/ssl_keys.log");

    OSSL_LIB_CTX *libctx;
//    start_tracing();

    // LOAD PROVIDERS  ----------------------------------------------------------------------------------
    OSSL_PROVIDER *prov_cng;
    OSSL_PROVIDER *prov_default;
    libctx = OSSL_LIB_CTX_new(); //NULL is default context

    /* Load Multiple providers into the library context */
    prov_cng = OSSL_PROVIDER_load(libctx, "F:/Documents/build-cng_provider-Desktop_Qt_5_15_2_MinGW_64_bit-Debug/debug/cng_provider.dll");    //加载自定义的provider
    if (prov_cng == NULL) {
        printf("PROGRAM> Failed to load CNG provider\n");
        OSSL_PROVIDER_unload(prov_cng);
        exit(EXIT_FAILURE);
    }
    HMODULE hModule = LoadLibraryEx("F:/Documents/build-cng_provider-Desktop_Qt_5_15_2_MinGW_64_bit-Debug/debug/cng_provider.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (hModule == NULL) {
           fprintf(stderr, "Failed to get module handle\n");
       }
    typedef void (*RegisterLogFuncPtr)(void (*)(const char*));
    RegisterLogFuncPtr initlog = (RegisterLogFuncPtr)GetProcAddress(hModule, "RegisterLogFunction");
    if (initlog == NULL) {
        fprintf(stderr, "Failed to get initlog function address\n");
        return 1;
    }
    initlog(provider_print_cb);

    prov_default = OSSL_PROVIDER_load(libctx, "default");       //获取默认的provider
    if (prov_default == NULL) {
        printf("PROGRAM> Failed to load default provider\n");
        OSSL_PROVIDER_unload(prov_cng);
        OSSL_PROVIDER_unload(prov_default);
        exit(EXIT_FAILURE);
    }


    //SSL 连接开始 ----------------------------------------------------------------------------------------
    SSL_CTX *ssl_ctx = NULL;
    SSL *ssl = NULL;

    SOCKET client_skt = INVALID_SOCKET;

    char rxbuf[512];
    size_t rxcap = sizeof(rxbuf);
    int rxlen;

    struct sockaddr_in addr;
    printf("PROGRAM> We will connect to a remote server and check the SSL certificate\n");

    /* Microsoft Docs */
    // The WSAStartup function must be the first Windows Sockets function called by an application or DLL.
    // - https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
    WSADATA wsaData;
    int err;

    err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0) {
        printf("PROGRAM> WSAStartup failed with error: %d\n", err);
        exit(1);
    }
    /* End of Microsoft Docs */

    /* Create ssl context used by client */
    ssl_ctx = create_context(libctx);

    /* Configure client context so we verify with the server correctly */
    configure_client_context(ssl_ctx);

    EVP_PKEY *pubkey = NULL;
    if (!find_and_use_client_certificate(CNG_URI, libctx, ssl_ctx, &pubkey)) {
        printf("PROGRAM> Could not find certificate with this search criteria in store\n");
        goto exit;
    }
    printf("PROGRAM> Certificate successfully loaded into SSL context\n");

    if (!find_and_use_client_private_key(CNG_URI, libctx, ssl_ctx, pubkey)) {
        printf("PROGRAM> Could not find matching private key in store\n");
        goto exit;
    }
    printf("PROGRAM> Private key successfully loaded into SSL context\n");
    printf("PROGRAM> Setting ssl contex is finished, now creating socket\n");
    /* Create "bare" socket */
    client_skt = create_socket();
    /* Set up connect address */
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &addr.sin_addr.s_addr);
    addr.sin_port = htons(SERVER_PORT);
    /* Do TCP connect with server */
    if (connect(client_skt, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        printf("PROGRAM> Unable to TCP connect to server");
        goto exit;
    } else {
        printf("PROGRAM> TCP connection to server successful\n");
    }

    /* Create client SSL structure using dedicated client socket */
    ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, client_skt);
    /* Set host name for SNI */
    SSL_set_tlsext_host_name(ssl, SERVER_COMMON_NAME);
    /* Configure server hostname check */
    SSL_set1_host(ssl, SERVER_COMMON_NAME);

    /* Now do SSL connect with server */
    if (SSL_connect(ssl) == 1) {
        printf("PROGRAM> SSL connection to server successful\n\n");

        char *header = "GET / HTTP/1.1\nHost: ";
        char *end = "\n\n";
        size_t msg_len = strlen(header) + strlen(SERVER_COMMON_NAME) + strlen(end) + 1;
        char *msg = malloc(msg_len);
        if (msg == NULL) { exit(2); }
        snprintf(msg, msg_len, "%s%s%s", header, SERVER_COMMON_NAME, end);

        /* Send it to the server */
        if (SSL_write(ssl, msg, msg_len) <= 0) {
            printf("PROGRAM> Server closed connection on write\n");
            free(msg);
            goto exit;
        }

        /* Wait for the response */
        rxlen = SSL_read(ssl, rxbuf, rxcap);
        if (rxlen <= 0) {
            printf("PROGRAM> Server closed connection on read\n");
            free(msg);
            goto exit;
        } else {
            /* Show it */
            rxbuf[rxlen < rxcap ? rxlen : rxcap - 1] = 0;
            printf("Received: %s\n", rxbuf);
        }
        free(msg);
    } else {
        printf("PROGRAM> SSL connection to server failed\n\n");
        ERR_print_errors_fp(stderr);
    }

    exit:

    /* Close up */
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
