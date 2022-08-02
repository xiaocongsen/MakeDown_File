#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 14433
#define CA_CERT_FILE 		"../gm/ca/rca.crt"
#define SIGN_CERT_FILE 		"../gm/loca/sm2.sig.crt.pem"
#define SIGN_KEY_FILE 		"../gm/loca/sm2.sig.key.pem"
#define ENCODE_CERT_FILE 	"../gm/loca/sm2.enc.crt.pem"
#define ENCODE_KEY_FILE 	"../gm/loca/sm2.enc.key.pem"


static int SetSocketReuseAddr(int socket, int enable)
{
    int reuse = enable ? 1 : 0;

    if (0 != setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)))
    {
        return errno;
    }
    return 0;
}

typedef struct sockaddr SA;
int TcpInit()
{
    int listener;
    do
    {
        listener = socket(AF_INET, SOCK_STREAM, 0);
        if( listener == -1 )
            return 0;

        struct sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = 0;
        sin.sin_port = htons(SERVER_PORT);

        if( bind(listener, (SA*)&sin, sizeof(sin)) < 0 )
            break;

        if( listen(listener, 5) < 0)
            break;
        SetSocketReuseAddr(listener, 1);
        return listener;
    }while(0);

    return -1;
}

int ssl_connetc()
{

    SSL_CTX     *ctx;
    SSL         *ssl;
    X509        *client_cert;
    char szBuffer[1024];
    int nLen;
    struct sockaddr_in addr;
    int nListenFd, nAcceptFd;

    nListenFd = TcpInit();
    SSLeay_add_ssl_algorithms();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();

    //int iMode = 1;
    //int iret = ioctlsocket(nAcceptFd, FIONBIO, (u_long FAR*)&iMode);
    ctx = SSL_CTX_new (GMTLS_server_method());
    if( ctx == NULL)
    {
        printf("SSL_CTX_new error!\n");
        return -1;
    }

    // 是否要求校验对方证书 此处不验证客户端身份所以为： SSL_VERIFY_NONE
    //    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

#if 1
    // 加载CA的证书
    if(!SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE, NULL))
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
    // 将连接付给SSL
    ssl = SSL_new (ctx);
    if(!ssl)
    {
        printf("SSL_new error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    socklen_t len  = sizeof(addr);
    nAcceptFd = accept(nListenFd, (struct sockaddr *)&addr, &len);
    SSL_set_fd (ssl, nAcceptFd);
    SSL_set_options(ssl, SSL_OP_NO_TICKET | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
    while(1)
    {
        if(SSL_accept (ssl) != 1)
        {
            int icode = -1;
            ERR_print_errors_fp(stderr);
            int iret = SSL_get_error(ssl, icode);
            sleep(1);
            printf("SSL_accept error! code = %d, iret = %d\n", icode, iret);
            // 释放资源
            SSL_free (ssl);
            SSL_CTX_free (ctx);
            close(nAcceptFd);
            return 0;
        }
        else
        {
            printf("11111111111111111111111111111111 SSL_accept seccess");
            break;
        }
       }
//    int i = 0;
//    while (1) {
//        const char *p = SSL_get_cipher_list(ssl,i);
//        if(p == NULL)
//            break;
//        printf("cipcher[%d]:[%s]\n",i++,p);

//    }
    //send to client
    memset(szBuffer, 0, sizeof(szBuffer));
    nLen = SSL_read(ssl,szBuffer, sizeof(szBuffer));
    if(nLen <= 0)
    {
        int icode = -1;
        ERR_print_errors_fp(stderr);
        int iret = SSL_get_error(ssl, icode);
        sleep(1);
        printf("SSL_read error! code = %d, iret = %d\n", icode, iret);
        // 释放资源
        SSL_free (ssl);
        SSL_CTX_free (ctx);
        close(nAcceptFd);
        return 0;
    }
    fprintf(stderr, "11 Get Len %d %s ok\n", nLen, szBuffer);

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_set_verify_result(ssl,1);
    SSL_renegotiate(ssl);
    int ret = SSL_do_handshake(ssl);
    if(ret == 1)
    {
        memset(szBuffer, 0, sizeof(szBuffer));
        nLen = SSL_read(ssl, szBuffer, strlen(szBuffer));
        if(nLen <= 0)
        {
            ret = SSL_do_handshake(ssl);
        }
    }


    // 进行操作
    strcat(szBuffer, "\n this is from server========server resend to client 1");
    if(SSL_write(ssl, szBuffer, strlen(szBuffer)) <= 0)
    {
        printf("2222222222222222222222 SSL_write error\n");
    }
    memset(szBuffer, 0, sizeof(szBuffer));
    nLen = SSL_read(ssl,szBuffer, sizeof(szBuffer));
    if(nLen <= 0)
    {
        printf("333333333333333333333 SSL_read error\n");
    }else
    fprintf(stderr, "33 Get Len %d %s ok\n", nLen, szBuffer);
    strcat(szBuffer, "\n this is from server========server resend to client 2");
    if(SSL_write(ssl, szBuffer, strlen(szBuffer)) <= 0)
    {
        printf("4444444444444444444444 SSL_write error\n");
    }
    sleep(2);
    // 释放资源
//    if(SSL_shutdown(ssl) == 0)
//    {
//        int index = 0;
//        while(SSL_shutdown(ssl) != 1)
//        {
//            if(index++ > 100)
//            {
//                SSL_free(ssl);
//                break;
//            }
//        }
//    }
//    else
//    {
        SSL_free(ssl);
//    }
    SSL_CTX_free (ctx);
    close(nAcceptFd);
}

int main(int argc, char **argv)
{
    ssl_connetc();
}
