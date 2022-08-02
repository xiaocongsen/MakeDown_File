
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
#include "icode.h"

//#define SERVER_PORT 443
//#define SERVER_IP "10.19.13.109"
#define SERVER_PORT 14433
#define SERVER_IP "10.19.11.129"


#define CA_CERT_FILE        "../gm/ca/rca.crt"
#define CA_CERT_FILE2       "../gm/ca/oca.crt"
#define SIGN_CERT_FILE 		"../gm/c/sm2.sig.pem"
#define SIGN_KEY_FILE 		"../gm/c/sm2.sig.pem"
#define ENCODE_CERT_FILE 	"../gm/c/sm2.enc.pem"
#define ENCODE_KEY_FILE 	"../gm/c/sm2.enc.pem"
int init()
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

}

int create_tcp_connect()
{
    // 创建连接
    int nFd = socket(AF_INET, SOCK_STREAM, 0);
    char *address = SERVER_IP;
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
    return nFd;
}

uint32_t Cert_SSL_AddCACert(X509_STORE *pOldCertStore, char *pwd)
{

    BIO *b = BIO_new_file(pwd,"r");
    X509 * pX509Cert = GMold_PEM_read_bio_X509(b, NULL, NULL, NULL);
    BIO_free(b);
    if(pX509Cert == NULL)
    {
        return NULL;
    }
    CRYPTO_RWLOCK *lock = CRYPTO_THREAD_lock_new();
    X509_OBJECT *pstObj = CRYPTO_malloc(sizeof(X509_OBJECT), __FILE__, __LINE__);
    memset(pstObj, 0,sizeof(X509_OBJECT));
    pstObj->type      = X509_LU_X509;
    pstObj->data.x509 = pX509Cert;
    CRYPTO_THREAD_write_lock(lock);
    if(-1 != GMold_OPENSSL_sk_find(pOldCertStore->objs,pstObj))
    {
        CRYPTO_free(pstObj, __FILE__, __LINE__);
        return -1;
    }
    else
    {
        OPENSSL_sk_push(pOldCertStore->objs, pstObj);
        /* 将证书添加到证书栈之后，需要重新排序 */
        OPENSSL_sk_sort((_STACK*)pOldCertStore->objs);
    }

    CRYPTO_THREAD_unlock(lock);
}

SSL *create_ssl_conect(SSL_CTX *ctx, int nFd,int flag)
{
    SSL *ssl;
    // 使用SSL V3,V2
    ctx = GMold_SSL_CTX_new (GMold_client_method());
    if( ctx == NULL)
    {
        printf("SSL_CTX_new error!\n");
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    SSL_CTX_ctrl(ctx, SSL_CTRL_SET_CLIENT_SIGALGS_LIST, 0, "ECDSA+SM3");
    SSL_CTX_set_cipher_list(ctx, "SM2-WITH-SMS4-SM3");

    if(flag)
    {
        // 要求校验对方证书，表示需要验证服务器端，若不需要验证则使用  SSL_VERIFY_NONE
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER , NULL);
        SSL_CTX_set_verify_depth(ctx,9);
        X509_STORE *pOldCertStore = SSL_CTX_get_cert_store(ctx);
        Cert_SSL_AddCACert(pOldCertStore,CA_CERT_FILE2);
        Cert_SSL_AddCACert(pOldCertStore,CA_CERT_FILE);
    }
    // 将连接付给SSL
    ssl = SSL_new (ctx);
    if( ssl == NULL)
    {
        printf("SSL_new error!\n");
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    GMold_SSL_set_fd (ssl, nFd);
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
        return NULL;
    }
    else
    {
        printf("[SSL Create][SSLConnect connectSSL:%d]\n",connectSSL);
        return  ssl;
    }
}

int32_t NETC_SSL_X509_ASN1_STRING_print(BIO *pbio, ASN1_STRING *pASN1string)
{
    int inumdex,IPos;
    char acbuf[80] = "";
    char*pctemp;

    if (pASN1string == NULL)
    {
        return -1;
    }

    IPos=0;
    pctemp=(char *)pASN1string->data;
    for (inumdex=0; inumdex < pASN1string->length; inumdex++)
    {
        acbuf[IPos]=pctemp[inumdex];
        IPos++;
        if (IPos >= 80)
        {
            if (BIO_write(pbio,acbuf,IPos) <= 0)
            {
                return -1;
            }
            IPos=0;
        }
    }

    if (IPos > 0)
    {
        if (BIO_write(pbio,acbuf,IPos) <= 0)
        {
            return -1;
        }
    }

    return 0;
}

int32_t NETC_ChangeEndianOrder(unsigned char *BMPString, unsigned int BMPString_len)
{
    int32_t i;
    uint8_t *p, temp;
    p = BMPString;
    for (i=0; i < (int)( BMPString_len/2); i++)
    {
        temp=*p;
        *p=*(p+1);
        *(p+1)=temp;
        p+=2;
    }
    return 0;
}
extern int32_t VOS_Unicode2GBK(const uint8_t* pucInput, uint32_t ulInLen, uint8_t* pucOut, uint32_t iMaxOutLen, uint32_t *ulOutLen);
extern int32_t VOS_Utf8toGBK(uint8_t *pucInBuf, uint32_t ulInLen,uint8_t **ppucOutBuf, uint32_t *pulOutLen);

uint32_t NETC_SSL_X509ExtratTextByNID(X509_NAME *pX509Name,int32_t iNID, uint8_t *pucOutputBuff,const uint32_t ulOutputBuffLen)
{
    int32_t iRet = -1;
    int32_t iCurrentIndex = -1;
    int32_t iNextIndex;
    BIO *pTmpIO = NULL;
    uint32_t ulOutLen = 0;
    uint32_t NidInLen = 0;
    uint8_t *pucNidOutBuff = NULL;
    ASN1_STRING *pASN1Str = NULL;
    uint8_t *pucOutBuffTmp = NULL;

    /*入参检测*/
    if((NULL == pX509Name)
            || (NULL == pucOutputBuff)
            || (0 == ulOutputBuffLen))
    {
        return -1;
    }


    /*目前依据X509证书标准, 同一类型字段的最后一个为主要标识内容*/
    while(0 <= (iNextIndex = X509_NAME_get_index_by_NID(pX509Name, iNID, iCurrentIndex)))
    {
        iCurrentIndex = iNextIndex;
    }

    if(0 <= iCurrentIndex)
    {
        if(NULL == (pTmpIO = BIO_new(BIO_s_mem())))
        {
            return -1;
        }
        pASN1Str = X509_NAME_ENTRY_get_data(X509_NAME_get_entry(pX509Name, iCurrentIndex));

        if (NULL == pASN1Str)
        {
            goto label_return;
        }

        if(0 == NETC_SSL_X509_ASN1_STRING_print(pTmpIO, pASN1Str))
        {
            pucOutBuffTmp = (uint8_t *)malloc((uint32_t)(pASN1Str->length + 1));
            if(NULL == pucOutBuffTmp)
            {
                goto label_return;
            }

            /*内存初始化*/
            memset(pucOutBuffTmp, 0, (uint32_t)(pASN1Str->length + 1));

            BIO_read(pTmpIO, pucOutBuffTmp, pASN1Str->length);
        }
        else
        {
            goto label_return;
        }

        NidInLen = (uint32_t)(pASN1Str->length);

        switch (pASN1Str->type)
        {
            case V_ASN1_BMPSTRING: /*MBSTRING_BMP - unicode certificate */

                /*分配临时的输出缓存*/
                pucNidOutBuff = (uint8_t *)malloc(NidInLen + 1);
                if(NULL == pucNidOutBuff)
                {
                    goto label_return;
                }

                /*内存初始化*/
                memset(pucNidOutBuff, 0, NidInLen + 1);
                NETC_ChangeEndianOrder(pucOutBuffTmp,NidInLen);

                if ((NidInLen > 0) && (-1 == VOS_Unicode2GBK(pucOutBuffTmp, NidInLen, pucNidOutBuff, NidInLen, &ulOutLen)))
                {
                    goto label_return;
                }
                break;

            case V_ASN1_UTF8STRING:     /*MBSTRING_UTF8 utf8 certificate */
                    if ((NidInLen > 0) && (-1 == VOS_Utf8toGBK((unsigned char *)pucOutBuffTmp, NidInLen, &pucNidOutBuff, &ulOutLen)))
                    {
                        goto label_return;
                    }
                    break;

            case V_ASN1_PRINTABLESTRING:

                if (ulOutputBuffLen <= NidInLen)
                {
                    memset(pucOutputBuff, 0, ulOutputBuffLen - 1);
                }
                else
                {
                   memcpy(pucOutputBuff, pucOutBuffTmp, NidInLen);
                    pucOutputBuff[NidInLen] = '\0';
                }
                break;

            default:
                if (ulOutputBuffLen <= NidInLen)
                {
                    memset(pucOutputBuff, 0, ulOutputBuffLen - 1);
                }
                else
                {
                    memcpy(pucOutputBuff, pucOutBuffTmp, NidInLen);
                    pucOutputBuff[NidInLen] = '\0';
                }
                break;
        }

        if ((NULL != pucNidOutBuff) && (0 < ulOutLen))
        {
            if (ulOutputBuffLen <= ulOutLen)
            {
                memset(pucOutputBuff, 0, ulOutputBuffLen - 1);
            }
            else
            {
                memcpy(pucOutputBuff,pucNidOutBuff, ulOutLen);
                pucOutputBuff[ulOutLen] = '\0';
            }
        }
    }

    iRet = 0;

label_return:

    if(NULL != pTmpIO)
    {
        BIO_free(pTmpIO);
    }

    if(NULL != pucNidOutBuff)
    {
        free(pucNidOutBuff);
    }

    if(NULL != pucOutBuffTmp)
    {
        free(pucOutBuffTmp);
    }

    return (uint32_t)iRet;
}

void NETC_Get_SSL_ServiceCertIssuerName(SSL *pstSSL, char *pcIssuerName, char *pcSubName, int uiIssUerNameLen)
{
    X509_NAME *pstX509Name = NULL;
    X509_NAME *pSubName = NULL;
    STACK_OF(X509) *sk = NULL;
    int certNum = 0;

    if (NULL == pstSSL || NULL == pcIssuerName || NULL == pcSubName || 0 == uiIssUerNameLen)
    {
        return;
    }

    /* 网关侧会在握手过程中发送用于设备证书校验的CA证书,此时需要将证书链获取到--by swx305442*/
    sk = SSL_get_peer_cert_chain(pstSSL);/*lint !e838 */
    if (NULL == sk)
    {
        return;
    }

    certNum = OPENSSL_sk_num((OPENSSL_STACK *)sk);/*lint !e838 */
    if(0 >= certNum)
    {
        return;
    }


    /* 在发送过来的证书链中证书是按照标准顺序排序，只需要取出最后一本证书的颁发者即可*/
    pstX509Name = X509_get_issuer_name((const X509*)OPENSSL_sk_value((OPENSSL_STACK *)sk, (certNum - 1)));/*lint !e838 */

    NETC_SSL_X509ExtratTextByNID(pstX509Name,NID_commonName,(uint8_t *)pcIssuerName,uiIssUerNameLen);

    /* Begin, Modifed by LH LWX756088, 2019-11-12 DTS2019111105577*/
    //判断颁发者是否为空，为空则获取OU字段
    if(0 != strlen(pcIssuerName))
    {
    }
    else
    {
        NETC_SSL_X509ExtratTextByNID(pstX509Name,NID_organizationalUnitName,pcIssuerName,uiIssUerNameLen);
    }
    /* End, Modifed by LH LWX756088,2019-11-12 DTS2019111105577*/

    //获取网关发送过来证书链中 最后一本证书的使用者
    pSubName = X509_get_subject_name(OPENSSL_sk_value((OPENSSL_STACK*)sk, 0));/*lint !e838 */

    X509_NAME_get_text_by_NID(pSubName, NID_commonName, pcSubName, 256);
    return;
}


int main(int argc, char **argv)
{
//    SSL_CTX *ctx = GMold_SSL_CTX_new (GMold_client_method());
//    if( ctx == NULL)
//    {
//        printf("SSL_CTX_new error!\n");
//        ERR_print_errors_fp(stderr);
//        return -1;
//    }
//    int fd = create_tcp_connect();
//    if(fd == -1)
//        return -1;
//    SSL * ssl = create_ssl_conect(ctx,fd,1);
//    if(ssl == NULL)
//        return -1;
////    char pcIssuerName[256];
////    char acSubName[256];
////    NETC_Get_SSL_ServiceCertIssuerName(ssl, pcIssuerName, acSubName, 256);
////    if(SSL_shutdown(ssl) == 0)
////    {
////        int index = 0;
////        while(SSL_shutdown(ssl) != 1)
////        {
////            if(index++ > 10)
////            {
////                SSL_free(ssl);
////                break;
////            }
////        }
////    }
////    else
////    {
//        SSL_free(ssl);
////    }
//    SSL_CTX_free(ctx);
//    close(fd);
//    return 1;



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
    ctx = GMold_SSL_CTX_new(GMold_client_method());
    if( ctx == NULL)
    {
        printf("SSL_CTX_new error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    SSL_CTX_ctrl(ctx, SSL_CTRL_SET_CLIENT_SIGALGS_LIST, 0, "ECDSA+SM3");
    // 要求校验对方证书，表示需要验证服务器端，若不需要验证则使用  SSL_VERIFY_NONE
//    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
//    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
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


    if(!GMold_SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE, NULL))
    {
        printf("SSL_CTX_load_verify_locations error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    // 加载CA的证书
    if(!GMold_SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE2, NULL))
    {
        printf("SSL_CTX_load_verify_locations error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 加载自己的证书
    if(GMold_SSL_CTX_use_certificate_file(ctx, SIGN_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_certificate_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 加载自己的私钥  私钥的作用是，ssl握手过程中，对客户端发送过来的随机
    //消息进行加密，然后客户端再使用服务器的公钥进行解密，若解密后的原始消息跟
    //客户端发送的消息一直，则认为此服务器是客户端想要链接的服务器
    if(GMold_SSL_CTX_use_PrivateKey_file(ctx, SIGN_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_PrivateKey_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 判定私钥是否正确
    if(!GMold_SSL_CTX_check_private_key(ctx))
    {
        printf("SSL_CTX_check_private_key error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    if(GMold_SSL_CTX_use_certificate_file(ctx, ENCODE_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_certificate_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    if(GMold_SSL_CTX_use_PrivateKey_file(ctx, ENCODE_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_PrivateKey_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 判定私钥是否正确
    if(!GMold_SSL_CTX_check_private_key(ctx))
    {
        printf("SSL_CTX_check_private_key error 2\n");
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
    GMold_SSL_set_fd (ssl, nFd);
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
    if(GMold_SSL_write(ssl, szBuffer, strlen(szBuffer)) <= 0)
    {
        printf("111111111111111 SSL_write error\n");
    }

    sleep(2);
    memset(szBuffer, 0, sizeof(szBuffer));
    int nLen = GMold_SSL_read(ssl,szBuffer, sizeof(szBuffer));
    if(nLen <= 0)
    {
        nLen = GMold_SSL_read(ssl,szBuffer, sizeof(szBuffer));
        if(nLen <= 0)
        {
            printf("2222222222222222 SSL_read error\n");
        }else
            fprintf(stderr, "1111 Get Len %d %s ok\n", nLen, szBuffer);
    }else
        fprintf(stderr, "222 Get Len %d %s ok\n", nLen, szBuffer);

    strcat(szBuffer, "\n this is from client========client resend to server 2");
    if(GMold_SSL_write(ssl, szBuffer, strlen(szBuffer)) <= 0)
    {
        printf("3333333333333333333333333 SSL_write error\n");
    }

    sleep(2);
    memset(szBuffer, 0, sizeof(szBuffer));
    nLen = GMold_SSL_read(ssl,szBuffer, sizeof(szBuffer));
    if(nLen <= 0)
    {
        printf("444444444444444444 SSL_read error\n");
    }else
    fprintf(stderr, "Get Len %d %s ok\n", nLen, szBuffer);
    SSL_free(ssl);
    SSL_CTX_free (ctx);
    close(nFd);
}

