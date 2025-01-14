# tassl的skf库engine示例

代码示例
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "openssl/ec.h"
#include "openssl/err.h"
#include "openssl/sm2.h"
#include "openssl/evp.h"
#include "openssl/ssl.h"
#include "openssl/x509.h"

#include "SKFAPI.h"

static int ukey_sslcli( void );
static void *ukey_export( int crt, int sign );

#define USAGE "Usage : \n\
    --Ukeysslcli        ukey ssl client\n\
    -AuthKey key        use auth key\n\
    -AuthKeyNew key     set new auth key\n\
    -UserPin pin        User pin\n\
    -ca crt             CA certificate\n\
    -sc cert            sign cert\n\
    -ec cert            enc cert\n\
    -s host:port        server address\n\
    --verify            verify peer\n"

// --Ukeysslcli: 这个选项可能用于将Ukey用作SSL客户端，可能涉及使用Ukey上的证书和密钥来进行SSL/TLS握手。
// -AuthKey key: 指定一个认证密钥，通常用于验证身份或签名操作。
// -AuthKeyNew key: 设置一个新的认证密钥，可能用于更新或更换现有的认证密钥。
// -UserPin pin: 提供用户PIN码，用于执行需要用户权限的操作。
// -ca crt: 指定CA证书，通常用于签发新证书时引用的证书颁发机构的证书。
// -sc cert: 指定签名证书，可能用于在SSL/TLS连接中或在签名操作中使用。
// -ec cert: 指定加密证书，可能用于在SSL/TLS连接中或在加密操作中使用。
// -s host:port: 指定服务器的地址和端口，可能用于连接到远程服务器进行操作。
// --verify: 启用对等方的验证，通常用于SSL/TLS连接，确保对方的证书是可信的。

static int Ukeysslcli = 0;
static char *UserPin = NULL;
static char *AuthKey = NULL, *AuthKeyNew = NULL;
static long AuthKeyNewLen = 0;
static char *ca = NULL;
static char *sign_cert = NULL, *enc_cert = NULL;
static int verify_peer = 0;
static char *host = NULL;

#define MAX_BUF_LEN 4096

int main( int argc, char *argv[] )
{
    int ret = 0, err;

    /* for openssl memory debug */
    //CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
    /**************************/

    /* options */
    for (err = 1; err < argc; err++)
    {
        if (!strcasecmp(argv[err], "--help") || !strcasecmp(argv[err], "-h") )
        {
            printf("%s", USAGE);
            goto error;
        }
        else if (!strcasecmp(argv[err], "--Ukeysslcli"))
        {
            Ukeysslcli = 1;
        }
        else if (!strcasecmp(argv[err], "-AuthKey"))
        {
            AuthKey = OPENSSL_hexstr2buf(argv[++err], NULL);
        }
        else if (!strcasecmp(argv[err], "-AuthKeyNew"))
        {
            AuthKeyNew = OPENSSL_hexstr2buf(argv[++err], &AuthKeyNewLen);
        }
        else if (!strcasecmp(argv[err], "-UserPin"))
        {
            UserPin = argv[++err];
        }
        else if (!strcasecmp(argv[err], "-ca"))
        {
            ca = argv[++err];
        }
        else if (!strcasecmp(argv[err], "-sc"))
        {
            sign_cert = argv[++err];
        }
        else if (!strcasecmp(argv[err], "-ec"))
        {
            enc_cert = argv[++err];
        }
        else if (!strcasecmp(argv[err], "-s"))
        {
            host = argv[++err];
        }
        else if (!strcasecmp(argv[err], "--verify"))
        {
            verify_peer = 1;
        }
        else
        {
            printf("unknown options, use --help\n");
            goto error;
        }
    }
    Ukeysslcli = 1;
    if( Ukeysslcli )
    {
        if( 1 != ukey_sslcli() )
        {
            printf("ukey_sslcli error\n");
            goto error;
        }
        printf("ukey_sslcli success\n");
    }

    ret = 1;

error:
    if( AuthKey ) OPENSSL_free(AuthKey);
    if( AuthKeyNew ) OPENSSL_free(AuthKeyNew);

    /*for openssl memory debug*/
    //CRYPTO_mem_leaks_fp(stderr);
    /**************************/

    return ret;
}

#define RETURN_NULL(x) if ((x)==NULL) { ERR_print_errors_fp(stderr); goto error; }
#define RETURN_ERR(err,s) if ((err)==-1) { perror(s); goto error; }
#define RETURN_SSL(err) if ((err)!=1) { ERR_print_errors_fp(stderr); goto error; }

#define APP_NAME "JITAPPLICATION_SM2"
#define CONTAINER_NAME "1ea7c63c-93b3-4bee-92c7-c21ff0e14e86"

#define DEFAULT_USER_PIN "123456"

static int ukey_sign(EVP_PKEY_CTX *ctx, unsigned char *sig, size_t *siglen,
            const unsigned char *tbs, size_t tbslen)
{
    ULONG retval;
    LPSTR szNameList = NULL;
    ULONG pulSize = 0;
    HAPPLICATION hApp = NULL;
    HCONTAINER hCon = NULL;
    DEVHANDLE hDev = NULL;
    ULONG ulRetryCount = 0;
    ECCSIGNATUREBLOB Signature = {0};

    int ret = 0, err;
    BIGNUM *R = NULL, *S = NULL;
    ECDSA_SIG *ecdsa_sig = NULL;
    char *tbs_tmp = NULL;
    const int sig_sz = ECDSA_size(EVP_PKEY_get0_EC_KEY(EVP_PKEY_CTX_get0_pkey(ctx)));

    if (sig_sz <= 0) {
        return 0;
    }

    if (sig == NULL) {
        *siglen = (size_t)sig_sz;
        return 1;
    }

    if (*siglen < (size_t)sig_sz) {
        return 0;
    }

    /* private key operation, MUST use Ukey */

    /* enum all dev */
    if( SAR_OK != (retval = SKF_EnumDev(1, szNameList, &pulSize))
                || (0 != pulSize && NULL == (szNameList = OPENSSL_malloc(pulSize)))
                || (0 != pulSize && NULL == memset(szNameList, 0, pulSize))
                || SAR_OK != (retval = SKF_EnumDev(1, szNameList, &pulSize)) )
    {
        printf("SKF_EnumDev error 0x%08lX\n", retval);
        goto error;
    }

    /* open first dev */
    if( SAR_OK != (retval = SKF_ConnectDev(szNameList, &hDev)) )
    {
        printf("SKF_ConnectDev %s error 0x%08lX\n", szNameList, retval);
        goto error;
    }

    /* open application */
    if( SAR_OK != (retval = SKF_OpenApplication(hDev, APP_NAME, &hApp)) )
    {
        printf("SKF_OpenApplication %s error 0x%08lX\n", APP_NAME, retval);
        goto error;
    }

    /* open container */
    if( SAR_OK != (retval = SKF_OpenContainer(hApp, CONTAINER_NAME, &hCon)) )
    {
        printf("SKF_OpenContainer %s error 0x%08lX\n", CONTAINER_NAME, retval);
        goto error;
    }

    /* verify user pin */
    if( SAR_OK != (retval = SKF_VerifyPIN(hApp, USER_TYPE, UserPin ? UserPin : DEFAULT_USER_PIN, &ulRetryCount)) )
    {
        printf("SKF_VerifyPIN error 0x%08lX, ulRetryCount = %lu\n", retval, ulRetryCount);
        goto error;
    }

    tbs_tmp = OPENSSL_malloc(tbslen);
    RETURN_NULL(tbs_tmp);

    memcpy(tbs_tmp, tbs, tbslen);

    /* sign */
    if(SAR_OK != (retval = SKF_ECCSignData(hCon, tbs_tmp, tbslen, &Signature)) )
    {
        printf("SKF_ECCSignData error 0x%08lX\n", retval);
        goto error;
    }

    if( SAR_OK != (retval = SKF_CloseContainer(hCon)) )
    {
        printf("SKF_CloseContainer error 0x%08lX\n", retval);
        goto error;
    }

    if( SAR_OK != (retval = SKF_CloseApplication(hApp)) )
    {
        printf("SKF_CloseApplication error 0x%08lX\n", retval);
        goto error;
    }

    if( SAR_OK != (retval = SKF_DisConnectDev(hDev)) )
    {
        printf("SKF_DisConnectDev error 0x%08lX\n", retval);
        goto error;
    }

    /* format transfer */
    ecdsa_sig = ECDSA_SIG_new();
    RETURN_NULL(ecdsa_sig);

    R = BN_bin2bn(Signature.r+32, 32, NULL);
    RETURN_NULL(R);

    S = BN_bin2bn(Signature.s+32, 32, NULL);
    RETURN_NULL(S);

    err = ECDSA_SIG_set0(ecdsa_sig, R, S);
    RETURN_SSL(err);

    R = S = 0;

    *siglen = i2d_ECDSA_SIG(ecdsa_sig, &sig);
    if( *siglen <= 0 )
      goto error;

    ret = 1;

error:
    if( R ) BN_free(R);
    if( S ) BN_free(S);
    if( szNameList ) OPENSSL_free(szNameList);
    if( tbs_tmp ) OPENSSL_free(tbs_tmp);
    if( ecdsa_sig ) ECDSA_SIG_free(ecdsa_sig);
    return ret;
}

static int ukey_verify(EVP_PKEY_CTX *ctx, const unsigned char *sig, size_t siglen,
            const unsigned char *tbs, size_t tbslen)
{
    EC_KEY *ec = EVP_PKEY_get0_EC_KEY(EVP_PKEY_CTX_get0_pkey(ctx));

    /* public key operation, use Ukey or NOT is OK */
    return sm2_verify(tbs, tbslen, sig, siglen, ec);
}

static int ukey_encrypt(EVP_PKEY_CTX *ctx,
            unsigned char *out, size_t *outlen,
            const unsigned char *in, size_t inlen)
{
    EC_KEY *ec = EVP_PKEY_get0_EC_KEY(EVP_PKEY_CTX_get0_pkey(ctx));
    const EVP_MD *md = EVP_sm3();

    if (out == NULL) {
        if (!sm2_ciphertext_size(ec, md, inlen, outlen))
          return -1;
        else
          return 1;
    }

    /* public key operation, use Ukey or NOT is OK */
    return sm2_encrypt(ec, md, in, inlen, out, outlen);
}

static int ukey_digest_custom(EVP_PKEY_CTX *ctx, EVP_MD_CTX *mctx)
{
    uint8_t z[EVP_MAX_MD_SIZE];
    EC_KEY *ec = EVP_PKEY_get0_EC_KEY(EVP_PKEY_CTX_get0_pkey(ctx));
    const EVP_MD *md = EVP_MD_CTX_md(mctx);
    int mdlen = EVP_MD_size(md);

    if (mdlen < 0) {
        return 0;
    }

    /* get hashed prefix 'z' of tbs message */
    if (!sm2_compute_z_digest(z, md, (const uint8_t *)SM2_DEFAULT_USERID, sizeof(SM2_DEFAULT_USERID)-1, ec))
      return 0;

    return EVP_DigestUpdate(mctx, z, (size_t)mdlen);
}

static int ukey_ctrl(EVP_PKEY_CTX *ctx, int type, int p1, void *p2)
{
    switch (type) {
    case EVP_PKEY_CTRL_MD:          //设置与密钥操作相关联的消息摘要算法（通常是哈希算法）。这对于签名或验证操作很重要，因为它决定了将使用哪种哈希算法。
    case EVP_PKEY_CTRL_DIGESTINIT:  //初始化摘要操作。这个命令通常用于在密钥操作中开始一个新的消息摘要计算
        return 1;       //返回1 表示支持
    case EVP_PKEY_CTRL_PEER_KEY:        //设置对端公钥，通常用于密钥交换协议中，以便进行密钥协商。
    case EVP_PKEY_CTRL_PKCS7_ENCRYPT:   //用于配置PKCS#7加密操作。PKCS#7是一种常用的加密/签名消息格式。
    case EVP_PKEY_CTRL_PKCS7_DECRYPT:   //用于配置PKCS#7解密操作。
    case EVP_PKEY_CTRL_PKCS7_SIGN:      //用于配置PKCS#7签名操作。
    case EVP_PKEY_CTRL_SET_MAC_KEY:     //用于设置用于消息认证码（MAC）的密钥。MAC通常用于确保数据完整性和真实性
    case EVP_PKEY_CTRL_SET_IV:          //设置初始化向量（IV），用于某些对称加密算法。这在加密协议中非常重要，例如在TLS中使用GOST加密时
    case EVP_PKEY_CTRL_CMS_ENCRYPT:     //用于配置CMS（Cryptographic Message Syntax）加密操作。CMS是一种用于安全消息的标准
    case EVP_PKEY_CTRL_CMS_DECRYPT:     //用于配置CMS解密操作。
    case EVP_PKEY_CTRL_CMS_SIGN:        //用于配置CMS签名操作。
    case EVP_PKEY_CTRL_CIPHER:          //设置用于加密操作的密码算法。
    case EVP_PKEY_CTRL_GET_MD:          //获取当前密钥操作中使用的消息摘要算法。
    case EVP_PKEY_CTRL_SET_DIGEST_SIZE: //设置摘要的大小，这在某些情况下可能需要，例如自定义摘要长度的情况下
    default:
        return -2;      //表示不支持该操作
    }
}

static int ukey_copy(EVP_PKEY_CTX *dst, EVP_PKEY_CTX *src)
{
    return 1;
}

static void *ukey_export( int crt, int sign )
{
    int err;
    void *ret = NULL;
    EVP_PKEY *pkey = NULL;
    EC_KEY *ec = NULL;
    EC_POINT *point = NULL;

    ULONG retval;
    LPSTR szNameList = NULL;
    ULONG pulSize = 0;
    HAPPLICATION hApp = NULL;
    HCONTAINER hCon = NULL;
    ECCPUBLICKEYBLOB stEccPub;
    DEVHANDLE hDev = NULL;
    ULONG pulBlobLen = sizeof(ECCPUBLICKEYBLOB);
    BYTE *pbCert = NULL;
    const unsigned char *p = NULL;
    ULONG pulCertLen = 0;
    X509 *x = NULL;
    unsigned char pubkey[65] = {0};

    /* enum all dev */
    if( SAR_OK != (retval = SKF_EnumDev(1, szNameList, &pulSize))
                || (0 != pulSize && NULL == (szNameList = OPENSSL_malloc(pulSize)))
                || (0 != pulSize && NULL == memset(szNameList, 0, pulSize))
                || SAR_OK != (retval = SKF_EnumDev(1, szNameList, &pulSize)) )
    {
        printf("SKF_EnumDev error 0x%08lX\n", retval);
        goto error;
    }

    /* open first dev */
    if( SAR_OK != (retval = SKF_ConnectDev(szNameList, &hDev)) )
    {
        printf("SKF_ConnectDev %s error 0x%08lX\n", szNameList, retval);
        goto error;
    }

    /* open application */
    if( SAR_OK != (retval = SKF_OpenApplication(hDev, APP_NAME, &hApp)) )
    {
        printf("SKF_OpenApplication %s error 0x%08lX\n", APP_NAME, retval);
        goto error;
    }

    /* open container */
    if( SAR_OK != (retval = SKF_OpenContainer(hApp, CONTAINER_NAME, &hCon)) )
    {
        printf("SKF_OpenContainer %s error 0x%08lX\n", CONTAINER_NAME, retval);
        goto error;
    }

    if( crt )
    {
        /* export certificate */
        if( SAR_OK != (retval = SKF_ExportCertificate(hCon, sign, NULL, &pulCertLen))
                    || (0 != pulCertLen && NULL == (pbCert = OPENSSL_malloc(pulCertLen)))
                    || SAR_OK != (retval = SKF_ExportCertificate(hCon, sign, pbCert, &pulCertLen)) )
        {
            printf("SKF_ExportCertificate error 0x%08lX\n", retval);
            goto error;
        }

        p = pbCert;
        x = d2i_X509(NULL, &p, pulCertLen);
        RETURN_NULL(x);

        ret = x; x = NULL;
    }
    else
    {
        /* export public key */
        if( SAR_OK != (retval = SKF_ExportPublicKey(hCon, sign, (BYTE *)&stEccPub, &pulBlobLen)) )
        {
            printf("SKF_ExportPublicKey error 0x%08lX\n", retval);
            goto error;
        }

        pubkey[0] = 0x04;
        memcpy(pubkey+1, stEccPub.XCoordinate+32, 32);
        memcpy(pubkey+1+32, stEccPub.YCoordinate+32, 32);

        pkey = EVP_PKEY_new();
        RETURN_NULL(pkey);

        ec = EC_KEY_new_by_curve_name(NID_sm2);
        RETURN_NULL(ec);

        point = EC_POINT_new(EC_KEY_get0_group(ec));
        RETURN_NULL(point);

        err = EC_POINT_oct2point(EC_KEY_get0_group(ec), point, pubkey, 65, NULL);
        RETURN_SSL(err);

        err = EC_KEY_set_public_key(ec, point);
        RETURN_SSL(err);

        EC_POINT_free(point); point = NULL;

        err = EVP_PKEY_set1_EC_KEY(pkey, ec);
        RETURN_SSL(err);

        EC_KEY_free(ec); ec = NULL;

        err = EVP_PKEY_set_alias_type(pkey, NID_sm2);
        RETURN_SSL(err);

        ret = pkey; pkey = NULL;
    }

    if( SAR_OK != (retval = SKF_CloseContainer(hCon)) )
    {
        printf("SKF_CloseContainer error 0x%08lX\n", retval);
        goto error;
    }

    if( SAR_OK != (retval = SKF_CloseApplication(hApp)) )
    {
        printf("SKF_CloseApplication error 0x%08lX\n", retval);
        goto error;
    }

    if( SAR_OK != (retval = SKF_DisConnectDev(hDev)) )
    {
        printf("SKF_DisConnectDev error 0x%08lX\n", retval);
        goto error;
    }

error:
    if( x ) X509_free(x);
    if( ec ) EC_KEY_free(ec);
    if( pbCert ) OPENSSL_free(pbCert);
    if( point ) EC_POINT_free(point);
    if( pkey ) EVP_PKEY_free(pkey);
    if( szNameList ) OPENSSL_free(szNameList);
    return ret;
}

static void ShowCerts(SSL * ssl)
{
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl);
    if (cert != NULL) {
        printf("对端证书信息:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("证书: %s\n", line);
        OPENSSL_free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("颁发者: %s\n", line);
        OPENSSL_free(line);
        X509_free(cert);
    } else
        printf("无对端证书信息！\n");
}

static int ukey_sslcli( void )
{
    int err, ret = 0;
    int len;
    char buf[MAX_BUF_LEN];
    SSL_CTX *ctx = NULL;
    SSL *ssl = NULL;
    BIO *bio = NULL, *in = NULL;
    X509 *x = NULL;
    EVP_PKEY_METHOD *ukey_meth = NULL;
    EVP_PKEY *pkey = NULL;
    EC_KEY *ec = NULL;
    EC_POINT *point = NULL;

    fprintf(stdout, "Start With\n\
                Server Address %s\n\
                CA Cert %s\n\
                Verify Peer %s\n",
                host,
                ca ? ca : "null",
                verify_peer ? "True" : "False");

    /* Load encryption & hashing algorithms for the SSL program */
    SSL_library_init();

    /* Load the error strings for SSL & CRYPTO APIs */
    SSL_load_error_strings();

    ukey_meth = EVP_PKEY_meth_new(EVP_PKEY_SM2, 0);
    RETURN_NULL(ukey_meth);

    EVP_PKEY_meth_set_sign(ukey_meth, NULL, ukey_sign);
    EVP_PKEY_meth_set_verify(ukey_meth, NULL, ukey_verify);
    EVP_PKEY_meth_set_encrypt(ukey_meth, NULL, ukey_encrypt);
    //EVP_PKEY_meth_set_decrypt(ukey_meth, NULL, ukey_decrypt);
    EVP_PKEY_meth_set_digest_custom(ukey_meth, ukey_digest_custom);
    EVP_PKEY_meth_set_ctrl(ukey_meth, ukey_ctrl, NULL);
    EVP_PKEY_meth_set_copy(ukey_meth, ukey_copy);

    err = EVP_PKEY_meth_add0(ukey_meth);
    RETURN_SSL(err);

    ukey_meth = NULL;

    /* Create a SSL_CTX structure */
    //we use GMTLSv1.1
    ctx = SSL_CTX_new(CNTLS_client_method());
    RETURN_NULL(ctx);

    if( sign_cert )
    {
        /* Load sign cert form file */
        err = SSL_CTX_use_certificate_file(ctx, sign_cert, SSL_FILETYPE_PEM);
        RETURN_SSL(err);
    }
    else
    {
        /* Load sign cert from ukey */
        x = ukey_export(1, 1);          //导出签名证书
        if( NULL == x )
          goto error;

        err = SSL_CTX_use_certificate(ctx, x);
        RETURN_SSL(err);

        X509_free(x); x = NULL;
    }

    /* Load sign key from ukey */
    pkey = ukey_export(0, 1);           //导出签名
    if( NULL == pkey )
      goto error;

    err = SSL_CTX_use_PrivateKey(ctx, pkey);
    RETURN_SSL(err);

    EVP_PKEY_free(pkey); pkey = NULL;

    /* Check if the certificate and private-key matches */
    err = SSL_CTX_check_private_key(ctx);
    RETURN_SSL(err);

    if( enc_cert )
    {
        /* Load enc cert from file */
        err = SSL_CTX_use_certificate_file(ctx, enc_cert, SSL_FILETYPE_PEM);
        RETURN_SSL(err);
    }
    else
    {
        /* Load enc cert from ukey */
        x = ukey_export(1, 0);
        if( NULL == x )
          goto error;

        err = SSL_CTX_use_certificate(ctx, x);
        RETURN_SSL(err);

        X509_free(x); x = NULL;
    }

    /* Load enc key from ukey */
    pkey = ukey_export(0, 0);
    if( NULL == pkey )
      goto error;

    err = SSL_CTX_use_PrivateKey(ctx, pkey);
    RETURN_SSL(err);

    EVP_PKEY_free(pkey); pkey = NULL;

    /* Check if the certificate and private-key matches */
    err = SSL_CTX_check_enc_private_key(ctx);
    RETURN_SSL(err);

    if ( NULL != ca )
    {
        /* Load the CA certificate into the SSL_CTX structure */
        err = SSL_CTX_load_verify_locations(ctx, ca, NULL);
        RETURN_SSL(err);
    }

    if ( verify_peer )
    {
        /* Set to verify peer certificate */
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

        /* Set the verification depth to 1 */
        SSL_CTX_set_verify_depth(ctx, 1);
    }

    /* TLS use TCP, Connect to address:port */
    bio = BIO_new_connect("10.19.12.121:9443");
    RETURN_NULL(bio);

    err = BIO_do_connect(bio);
    RETURN_SSL(err);

    ssl = SSL_new(ctx);
    RETURN_NULL(ssl);

    SSL_set_bio(ssl, bio, bio);
    bio = NULL;

    err = SSL_connect(ssl);
    RETURN_SSL(err);

    /* Informational output (optional) */
    fprintf(stdout, "SSL connection using %s, %s\n", SSL_get_version(ssl), SSL_get_cipher(ssl));
    ShowCerts(ssl);

    while(1) {
        /*------- DATA EXCHANGE - Receive message and send reply. -------*/
        memset(buf, 0x00, sizeof(buf));
        fgets(buf, sizeof(buf), stdin);
        if (SSL_write(ssl, buf, strlen(buf)) != strlen(buf))
        {
            ERR_print_errors_fp(stderr);
            exit(1);
        }

        /* Receive data from the SSL client */
        memset(buf, 0x00, sizeof(buf));
        len = SSL_read(ssl, buf, sizeof(buf) - 1);
        if( len <= 0 )
        {
            ERR_print_errors_fp(stderr);
            exit(1);
        }
        fprintf(stdout, "recv %d bytes : %s\n", len, buf);
    }

    /*--------------- SSL closure ---------------*/
    /* Shutdown this side (server) of the connection. */
    SSL_shutdown(ssl);

    ret = 1;

error:
    /* Free the SSL structure */
    if (ssl) SSL_free(ssl);

    if (bio) BIO_free(bio);

    if (in) BIO_free(in);

    if (x) X509_free(x);

    if (ec) EC_KEY_free(ec);

    if (point) EC_POINT_free(point);

    if (pkey) EVP_PKEY_free(pkey);

    /* Free the SSL_CTX structure */
    if (ctx) SSL_CTX_free(ctx);

    if (ukey_meth) EVP_PKEY_meth_free(ukey_meth);

    return ret;
}
```
