
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/x509_int.h"
#include "openssl/x509_lcl.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>



//#define SERVER_PORT 443
//#define SERVER_IP "10.19.11.129"
#define SERVER_PORT 14433
#define SERVER_IP "10.19.11.129"

#define CA_CERT_FILE        "../gm/ca/rca.crt"
#define CA_CERT_FILE2        "../gm/ca/oca.crt"
#define SIGN_CERT_FILE 		"../gm/c/sm2.sig.pem"
#define SIGN_KEY_FILE 		"../gm/c/sm2.sig.pem"
#define ENCODE_CERT_FILE 	"../gm/c/sm2.enc.pem"
#define ENCODE_KEY_FILE 	"../gm/c/sm2.enc.pem"

int main(int argc, char **argv)
{
    char *address = SERVER_IP;
    if(argc > 1)
    {
        address = argv[1];
    }

    SSL_CTX     *ctx;
    SSL         *ssl;

    int nFd;

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    // 使用SSL V3,V2
    ctx = SSL_CTX_new (GMTLS_client_method());
    if( ctx == NULL)
    {
        printf("SSL_CTX_new error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    SSL_CTX_ctrl(ctx, SSL_CTRL_SET_CLIENT_SIGALGS_LIST, 0, "ECDSA+SM3");
    // 要求校验对方证书，表示需要验证服务器端，若不需要验证则使用  SSL_VERIFY_NONE
    //    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
//    SSL_CTX_set_verify_depth(ctx,9);
    SSL_CTX_set_cipher_list(ctx, "SM2-WITH-SMS4-SM3");

#if 1
    // 加载CA的证书
    if(!SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE, NULL))
    {
        printf("SSL_CTX_load_verify_locations error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    // 加载CA的证书
    if(!SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE2, NULL))
    {
        printf("SSL_CTX_load_verify_locations error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 加载自己的证书
    if(SSL_CTX_use_certificate_file(ctx, SIGN_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_certificate_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 加载自己的私钥  私钥的作用是，ssl握手过程中，对客户端发送过来的随机
    //消息进行加密，然后客户端再使用服务器的公钥进行解密，若解密后的原始消息跟
    //客户端发送的消息一直，则认为此服务器是客户端想要链接的服务器
    if(SSL_CTX_use_PrivateKey_file(ctx, SIGN_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_PrivateKey_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    if(SSL_CTX_use_certificate_file(ctx, ENCODE_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_certificate_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    if(SSL_CTX_use_PrivateKey_file(ctx, ENCODE_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_PrivateKey_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 判定私钥是否正确
    if(!SSL_CTX_check_private_key(ctx))
    {
        printf("SSL_CTX_check_private_key error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
#endif

    // 创建连接
    nFd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr(address);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);

    //链接服务器
    if(connect(nFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("connect\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 将连接付给SSL
    ssl = SSL_new (ctx);
    if( ssl == NULL)
    {
        printf("SSL_new error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    SSL_set_fd (ssl, nFd);
    SSL_set_options(ssl, SSL_OP_NO_TICKET | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);

    int connectSSL =0;
    for ( ; ; )
    {
        connectSSL = SSL_connect(ssl);
        if (connectSSL <= 0)
        {
            int err = SSL_get_error(ssl, connectSSL);
            int retry = 200;
            while (((err == SSL_ERROR_WANT_READ)||(err == SSL_ERROR_WANT_WRITE)) && retry--)
            {
                usleep(50);
                connectSSL = SSL_connect(ssl);
                err = SSL_get_error(ssl, connectSSL);

            }
            if(connectSSL <= 0)
            {
                const char* pState = SSL_state_string_long(ssl);
                printf("[SSL Create][SSLConnect errno: %d, state: %s,connectSSL:%d]\n", err, pState,connectSSL);
            }
        }

        if (BIO_sock_should_retry(connectSSL))
        {
            connectSSL = SSL_get_fd(ssl);

            int width = connectSSL + 1;
            fd_set      readfds;
            FD_ZERO(&readfds);
            FD_SET(connectSSL, &readfds);

            select(width, &readfds, NULL, NULL, NULL);
            continue;
        }

        break;
    }

    if ( 0 >= connectSSL )
    {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(nFd);
        return -1;
    }
    else
    {
        printf("[SSL Create][SSLConnect connectSSL:%d]\n",connectSSL);
    }

    char szBuffer[1024];
    strcat(szBuffer, "\n this is from client========client resend to server 1");
    if(SSL_write(ssl, szBuffer, strlen(szBuffer)) <= 0)
    {
        printf("111111111111111 SSL_write error\n");
    }

    sleep(2);
    memset(szBuffer, 0, sizeof(szBuffer));
    int nLen = SSL_read(ssl,szBuffer, sizeof(szBuffer));
    if(nLen <= 0)
    {
        nLen = SSL_read(ssl,szBuffer, sizeof(szBuffer));
        if(nLen <= 0)
        {
            printf("2222222222222222 SSL_read error\n");
        }else
            fprintf(stderr, "1111 Get Len %d %s ok\n", nLen, szBuffer);
    }else
        fprintf(stderr, "222 Get Len %d %s ok\n", nLen, szBuffer);

    strcat(szBuffer, "\n this is from client========client resend to server 2");
    if(SSL_write(ssl, szBuffer, strlen(szBuffer)) <= 0)
    {
        printf("3333333333333333333333333 SSL_write error\n");
    }

    sleep(2);
    memset(szBuffer, 0, sizeof(szBuffer));
    nLen = SSL_read(ssl,szBuffer, sizeof(szBuffer));
    if(nLen <= 0)
    {
        printf("444444444444444444 SSL_read error\n");
    }else
    fprintf(stderr, "Get Len %d %s ok\n", nLen, szBuffer);
    SSL_free(ssl);
    SSL_CTX_free (ctx);
    close(nFd);
}



//#include <openssl/sm2.h>
//#include <openssl/evp.h>

//X509 * GetFileX509(const char *file, int type)
//{
//    int j;
//    BIO *in;
//    int ret = 0;
//    X509 *x = NULL;

//    in = BIO_new(BIO_s_file());
//    if (in == NULL) {
//        goto end;
//    }

//    if (BIO_read_filename(in, file) <= 0) {
//        goto end;
//    }
//    if (type == SSL_FILETYPE_ASN1) {
//        j = ERR_R_ASN1_LIB;
//        x = d2i_X509_bio(in, NULL);
//    } else if (type == SSL_FILETYPE_PEM) {
//        j = ERR_R_PEM_LIB;
//        x = PEM_read_bio_X509(in, NULL, NULL, NULL);
//    } else {
//        goto end;
//    }

//    if (x == NULL) {
//        goto end;
//    }

// end:
//    BIO_free(in);
//    return (x);
//}

//int main(int argc, char **argv)
//{
//    SSLeay_add_ssl_algorithms();
//    OpenSSL_add_all_algorithms();
//    SSL_load_error_strings();
//    ERR_load_BIO_strings();
////    SSL_CTX *ctx = SSL_CTX_new (GMTLS_server_method());
////    // 加载CA的证书
////    if(!SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE, NULL))
////    {
////        printf("SSL_CTX_load_verify_locations error!\n");
////        ERR_print_errors_fp(stderr);
////        return -1;
////    }

////    // 加载自己的证书
////    if(SSL_CTX_use_certificate_file(ctx, SIGN_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
////    {
////        printf("SSL_CTX_use_certificate_file error!\n");
////        ERR_print_errors_fp(stderr);
////        return -1;
////    }

////    // 加载自己的私钥  私钥的作用是，ssl握手过程中，对客户端发送过来的随机
////    //消息进行加密，然后客户端再使用服务器的公钥进行解密，若解密后的原始消息跟
////    //客户端发送的消息一直，则认为此服务器是客户端想要链接的服务器
////    if(SSL_CTX_use_PrivateKey_file(ctx, SIGN_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
////    {
////        printf("SSL_CTX_use_PrivateKey_file error!\n");
////        ERR_print_errors_fp(stderr);
////        return -1;
////    }

////    if(SSL_CTX_use_certificate_file(ctx, ENCODE_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
////    {
////        printf("SSL_CTX_use_certificate_file error!\n");
////        ERR_print_errors_fp(stderr);
////        return -1;
////    }
////    if(SSL_CTX_use_PrivateKey_file(ctx, ENCODE_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
////    {
////        printf("SSL_CTX_use_PrivateKey_file error!\n");
////        ERR_print_errors_fp(stderr);
////        return -1;
////    }

////    // 判定私钥是否正确
////    if(!SSL_CTX_check_private_key(ctx))
////    {
////        printf("SSL_CTX_check_private_key error!\n");
////        ERR_print_errors_fp(stderr);
////        return -1;
////    }
//    char out[48];
//   X509 *x = GetFileX509(ENCODE_CERT_FILE, SSL_FILETYPE_PEM);
//    EC_KEY *ec_key = X509_get0_pubkey(x);
//    const unsigned char * in = {
//        0x01, 0x01, 0x48, 0x03, 0x78, 0xba, 0xf1, 0x35, 0xb0, 0xa5, 0x28, 0x12, 0x14, 0xe4, 0xe5, 0x36, 0x81, 0xcb, 0xbe, 0xbc, 0x65, 0x2a, 0xea, 0x5e, 0x22, 0xa7, 0x68, 0xde, 0x43, 0xb5, 0xcc, 0x0f, 0x58, 0x2f, 0xff, 0xcb, 0xb4, 0x53, 0x70, 0x36, 0x7f, 0x82, 0x79, 0x38, 0x5a, 0x63, 0x01, 0x4f
//    };
//    SM2CiphertextValue *cv = SM2_do_encrypt(EVP_sm3(),in,48,ec_key);
//    int outlen = i2d_SM2CiphertextValue(cv, &out);
//    SM2CiphertextValue_free(cv);


//}
