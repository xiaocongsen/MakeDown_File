# openssl相关问题

## SSL出错源码位置获取

``` c
    SSL_load_error_strings();           //加载错误码对应信息
    unsigned long ret = ERR_get_error(); //获取错误码
    char *szErrMsg[1024] = {0};
    ERR_error_string(ret,szErrMsg); //获取错误信息 格式： error:errld:库:函数:原因
```

## 调试SSL数据解密

``` c
void my_ssl_keylog_callback(const SSL *ssl, const char *line) {
    /* 将密钥日志写入指定的文件中 */
    char* str = getenv("MYSSLKEYLOGFILE");
    if(str != NULL)
    {
        FILE *fp = fopen("./sslkeylogfile.txt", "a");
        fprintf(fp, "%s\n", line);
        fclose(fp);
    }
}

SSL_CTX_set_keylog_callback(ctx, my_ssl_keylog_callback);   //设置回调函数 用于记录SSL密钥 给wireshark解密
```

```c
#include<openssl/err.h>
int log_print_cb(const char *str, size_t len, void *u)
{
    DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[xxxxxxxxxxxxxxxxxxx][%s]",str);
    return 1;
}
ERR_print_errors_cb(log_print_cb, NULL);
```

## PKCS12_parse解析弱加密算法证书报错问题
    
1. 错误信息：
    4037D21AC17F0000:error:0308010C:digital envelope routines:inner_evp_generic_fetch:unsupported:crypto/evp/evp_fetch.c:342:Global default library context, Algorithm (RC2-40-CBC : 0), Properties ()
2. 方案：修改代码，main函数一开始加载legacy provider
（1）调用OSSL_PROVIDER_set_default_search_path接口设置legacy.so所在目录，第一个参数为NULL 也可以setenv("OPENSSL_MODULES", "/path/to/your/openssl/lib/ossl-modules", 1);
（2）调用OSSL_PROVIDER_load接口加载default provider，第一个参数为NULL，第二个参数为"default"，并保存返回值
（3）调用OSSL_PROVIDER_load接口加载legacy provider，第一个参数为NULL，第二个参数为"legacy"，并保存返回值
（4）进程退出时调用OSSL_PROVIDER_unload卸载legacy provider和default provider

## SSL_Read出现SSL_ERROR_SSL

- 详细错误信息:00570000:error:0A000126:SSL routines:ssl3_read_n:unexpected eof while reading:ssl\record\rec_layer_s3.c:304:
- 表明在SSL/TLS通信过程中遇到了一个未预期的EOF（End Of File），即文件结束符。在SSL/TLS上下文中，这通常意味着对端在数据传输过程中突然关闭了连接，而接收方在尝试读取更多数据时发现连接已经关闭，没有更多的数据可读。

## tls1.3 关键代码记录

```c
const SSL_METHOD *tlsv1_3_method(void) 
{ 
    static const SSL_METHOD tlsv1_3_method_data= 
    { 
        TLS1_3_VERSION, 
        0, 
        SSL_OP_NO_TLSv1_3, 
        tls1_new, 
        tls1_clear, 
        tls1_free, 
        ossl_statem_accept, 
        ossl_statem_connect, 
        ssl3_read, 
        ssl3_peek,
        ssl3_write, 
        ssl3_shutdown, 
        ssl3_renegotiate, 
        ssl3_renegotiate_check, 
        ssl3_read_bytes, 
        ssl3_write_bytes, 
        ssl3_dispatch_alert, 
        ssl3_ctrl, 
        ssl3_ctx_ctrl, 
        ssl3_get_cipher_by_char, 
        ssl3_put_cipher_by_char, 
        ssl3_pending, 
        ssl3_num_ciphers,
        ssl3_get_cipher, 
        tls1_default_timeout, 
        &TLSv1_3_enc_data,
        ssl_undefined_void_function, 
        ssl3_callback_ctrl, 
        ssl3_ctx_callback_ctrl,             
        }; 
        return &tlsv1_3_method_data; 
}
const SSL_METHOD *tlsv1_3_server_method(void) 
{ 
    static const SSL_METHOD tlsv1_3_server_method_data= 
    { 
        TLS1_3_VERSION, 
        0, 
        SSL_OP_NO_TLSv1_3, 
        tls1_new, 
        tls1_clear, 
        tls1_free, 
        ossl_statem_accept, 
        ssl_undefined_function,
        ssl3_read,
        ssl3_peek, 
        ssl3_write, 
        ssl3_shutdown, 
        ssl3_renegotiate, 
        ssl3_renegotiate_check, 
        ssl3_read_bytes, 
        ssl3_write_bytes, 
        ssl3_dispatch_alert, 
        ssl3_ctrl, 
        ssl3_ctx_ctrl, 
        ssl3_get_cipher_by_char, 
        ssl3_put_cipher_by_char, 
        ssl3_pending, 
        ssl3_num_ciphers,
        ssl3_get_cipher, 
        tls1_default_timeout, 
        &TLSv1_3_enc_data, 
        ssl_undefined_void_function, 
        ssl3_callback_ctrl, 
        ssl3_ctx_callback_ctrl, 
        }; 
        return &tlsv1_3_server_method_data; 
}
const SSL_METHOD *tlsv1_3_client_method(void) 
{ 
    static const SSL_METHOD tlsv1_3_client_method_data= { 
        TLS1_3_VERSION,             tls1.3的version
        0, 
        SSL_OP_NO_TLSv1_3,          tls1.3的mask
        tls1_new, 
        tls1_clear, 
        tls1_free, 
        ssl_undefined_function,         
        ossl_statem_connect,            //连接函数
        ssl3_read, 
        ssl3_peek, 
        ssl3_write, 
        ssl3_shutdown, 
        ssl3_renegotiate,               //重协商 
        ssl3_renegotiate_check, 
        ssl3_read_bytes, 
        ssl3_write_bytes, 
        ssl3_dispatch_alert, 
        ssl3_ctrl, 
        ssl3_ctx_ctrl, 
        ssl3_get_cipher_by_char, 
        ssl3_put_cipher_by_char, 
        ssl3_pending, 
        ssl3_num_ciphers, 
        ssl3_get_cipher,
        tls1_default_timeout, 
        &TLSv1_3_enc_data,                  //tls1.3的独有结构
        ssl_undefined_void_function,        
        ssl3_callback_ctrl,                 
        ssl3_ctx_callback_ctrl, 
        }; 
        return &tlsv1_3_client_method_data; 
}

SSL3_ENC_METHOD const TLSv1_3_enc_data = {
    tls13_enc,                                                      //用于加密 SSL/TLS 协议中的记录数据
    tls1_mac,                                                       //用于计算 SSL/TLS 协议中的 MAC 值
    tls13_setup_key_block,                                          //用于设置 SSL/TLS 协议中的密钥块
    tls13_generate_master_secret,                                   //用于生成 SSL/TLS 协议中的主密钥
    tls13_change_cipher_state,                                      //用于切换 SSL/TLS 协议中的密码状态
    tls13_final_finish_mac,                                         //用于计算 SSL/TLS 协议中的最终消息认证码
    TLS_MD_CLIENT_FINISH_CONST, TLS_MD_CLIENT_FINISH_CONST_SIZE,    //一个指向客户端 Finished 消息标签的指针和标签长度；
    TLS_MD_SERVER_FINISH_CONST, TLS_MD_SERVER_FINISH_CONST_SIZE,    //一个指向服务器端 Finished 消息标签的指针和标签长度；
    tls13_alert_code,                                               //用于将 TLS 协议中的警告代码转换为 SSL/TLS 协议中的警告代码
    tls13_export_keying_material,                                   //用于导出 SSL/TLS 协议中的密钥材料
    SSL_ENC_FLAG_SIGALGS | SSL_ENC_FLAG_SHA256_PRF,                 //表示协议版本的要求
    ssl3_set_handshake_header,                                    //用于设置 SSL/TLS 协议中的握手消息头
    tls_close_construct_packet,                                    //用于关闭 SSL/TLS 协议中的构造数据包
    ssl3_handshake_write                                        //用于设置 SSL/TLS 协议中握手消息的头部
};


//Diffie-Hellman生成密钥对的结构
const EVP_PKEY_ASN1_METHOD ecx25519_asn1_meth = {
    EVP_PKEY_X25519,
    EVP_PKEY_X25519,
    0,
    "X25519",
    "OpenSSL X25519 algorithm",

    ecx_pub_decode,
    ecx_pub_encode,
    ecx_pub_cmp,
    ecx_pub_print,

    ecx_priv_decode,
    ecx_priv_encode,
    ecx_priv_print,

    ecx_size,
    ecx_bits,
    ecx_security_bits,

    0, 0, 0, 0,
    ecx_cmp_parameters,
    0, 0,

    ecx_free,
    ecx_ctrl,
    NULL,
    NULL,

    NULL,
    NULL,
    NULL,

    NULL,
    NULL,
    NULL,

    ecx_set_priv_key,
    ecx_set_pub_key,
    ecx_get_priv_key,
    ecx_get_pub_key,
};


EVP_PKEY_get_raw_private_key


add_key_share 添加tls1.3握手密钥扩展
```

``` c
//打印EVP_PKEY的密钥内容
void print_private_key(EVP_PKEY *pkey) {
    BIO *bio = BIO_new(BIO_s_mem());
    if (!bio) {
        // Handle error
        return;
    }

    if (PEM_write_bio_PrivateKey(bio, pkey, NULL, NULL, 0, NULL, NULL) != 1) {
        // Handle error
        BIO_free(bio);
        return;
    }

    char *buffer;
    long length = BIO_get_mem_data(bio, &buffer);
    if (length > 0) {
        printf("Private key:\n%s\n", buffer);
    }

    BIO_free(bio);
}
```

SSL_DEBUG 用于SSL打印调试信息

tls1.3 流程
    客户端通过调用注册函数ossl_statem_connect->tls_construct_client_hello函数创建client hello包给服务端
    ssl_set_client_hello_version->ssl_get_min_max_version   通过获取客户端支持的ssl协议的最大最小版本号





SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION           作为服务器，不允许在重新协商时恢复会话

SSL_OP_NO_COMPRESSION                               即使支持，也不要使用压缩

SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION        允许不安全的遗留重新协商

SSL_OP_NO_ENCRYPT_THEN_MAC                  禁用扩展encrypt-then-mac

SSL_OP_ENABLE_MIDDLEBOX_COMPAT      启用TLSv1.3兼容模式. 默认处于启用状态

SSL_OP_PRIORITIZE_CHACHA            当客户端修改SSL_OP_CIPHER_SERVER_PREFERENCE时，对Chacha20Poly1305进行优先级排序

SSL_OP_CIPHER_SERVER_PREFERENCE      在服务器上设置，以根据服务器的首选项选择密码

SSL_OP_TLS_ROLLBACK_BUG     如果设置，则服务器将允许客户端发布SSLv3.0版本号作为预主密钥中支持的最新版本，即使在客户端hello中宣布TLSv1.0（版本3.1）。通常，这是禁止的，以防止版本回滚攻击。

SSL_OP_NO_ANTI_REPLAY       关闭早期数据的自动TLSv1.3防重放保护。这只是一个服务器端选项（对客户端没有影响）

SSL_OP_NO_RENEGOTIATION      不允许所有重协商




## tls1.2协议流程

### 普通认证流程
1. 客户端发送"ClientHello"消息：客户端发送一个"ClientHello"消息到服务器，包含其支持的协议版本、密码套件、随机数和会话ID。
2. 服务器回应"ServerHello"消息：服务器从客户端发送的"ClientHello"消息中选择一组最高版本和客户端匹配的协议版本和密码套件，以及自己生成的随机数和会话ID。服务器通过"ServerHelloDone"消息通知客户端，使其进入下一步骤。
3. 客户端发送"ClientKeyExchange"消息：客户端随机生成一个"pre-master secret"，并将其使用服务器公钥加密后，通过"ClientKeyExchange"消息发送给服务器。
4. 服务器接收"ClientKeyExchange"消息：服务器使用其私钥对"pre-master secret"解密，得到"pre-master secret"。
5. 服务器和客户端计算密钥：客户端和服务器使用"pre-master secret"和两个随机数计算出主密钥。
6. 服务器和客户端发送"ChangeCipherSpec"消息：服务器和客户端分别发送一个"ChangeCipherSpec"消息，以通知对方之后发送的所有消息将使用新协议规定的密钥和加密算法进行加密和认证。
6. 客户端发送"Finished"消息：客户端使用主密钥和一个"Finished"消息验证协商完成，向服务器发送一个已加密的消息。
6. 服务器发送"Finished"消息：服务器使用主密钥和一个"Finished"消息验证协商完成，向客户端发送一个已加密的消息。
6. 客户端和服务器之间进行数据传输：协商完成后，客户端和服务器之间开始进行加密的数据传输。

### 包含双向认证的流程
1. 客户端发送Client Hello消息给服务端，该消息包含了客户端支持的加密算法列表以及一个随机数。
1. 服务端收到Client Hello消息后，返回Server Hello消息给客户端，该消息包含了服务端选择的加密算法以及另一个随机数。
1. 服务端发送Certificate消息给客户端，该消息包含了服务端的证书。
1. 服务端发送Server Hello Done消息给客户端，表示服务端已经完成了Hello消息的发送。
1. 客户端收到Server Hello Done消息后，对服务端发送的证书进行验证，包括证书是否过期、证书是否被吊销、证书是否由可信的CA签发等。
1. 客户端发送Client Key Exchange消息给服务端，该消息包含了客户端的公钥，用于在接下来的协商过程中生成共享密钥。
1. 客户端发送Certificate Verify消息给服务端，该消息包含了客户端对证书的签名，用于证明客户端拥有相应的私钥。
1. 客户端发送Change Cipher Spec消息给服务端，表示客户端即将使用新的加密算法。
1. 客户端发送Finished消息给服务端，该消息包含了用协商出的密钥对之前所有数据的哈希值，用于验证协商的密钥是否正确。
1. 服务端收到Client Key Exchange消息后，使用客户端发送的公钥生成共享密钥。
1. 服务端发送Certificate Request消息给客户端，请求客户端提供证书。
1. 服务端发送Server Key Exchange消息给客户端，该消息包含了用于生成共享密钥的服务器公钥，仅在需要使用临时密钥时发送。
1. 服务端发送Certificate Verify消息给客户端，用于验证客户端的证书。
1. 服务端发送Change Cipher Spec消息给客户端，表示服务端即将使用新的加密算法。
1. 服务端发送Finished消息给客户端，该消息包含了用协商出的密钥对之前所有数据的哈希值，用于验证协商的密钥是否正确。

> 在整个TLS 1.2协议流程中，服务端认证和客户端验证的过程主要发生在第5、7、11和13步。客户端需要验证服务端的证书，以确保连接的安全性，而服务端也需要验证客户端的证书，以确保客户端身份的合法性。



1. 客户端发送Client Hello消息：客户端随机生成一个32字节的随机数，称为Client Random，然后发送包含以下信息的Client Hello消息给服务端：TLS版本号，一个32字节的Client Random，一个16字节的会话ID，一个由客户端支持的密码套件列表，包括加密算法和哈希算法等信息，一个由客户端支持的压缩算法列表，可选的扩展字段，如SNI等信息
1. 服务端发送Server Hello消息：服务端从客户端支持的密码套件列表中选择一个密码套件，然后随机生成一个32字节的随机数，称为Server Random，最后发送以下信息的Server Hello消息给客户端：TLS版本号，一个32字节的Server Random，一个16字节的会话ID（如果使用了会话重用），选择的密码套件，包括加密算法和哈希算法等信息，选择的压缩算法
1. 服务端发送Certificate消息：如果服务端需要进行身份认证，服务端会发送一个包含数字证书的Certificate消息给客户端。数字证书中包含了服务端的公钥和身份信息，客户端可以用数字证书中的公钥来验证服务端的身份。
1. 服务端发送Server Key Exchange消息：如果服务器选择的密码套件需要进行密钥交换，那么服务端会发送一个Server Key Exchange消息给客户端，该消息包含了用于生成共享密钥的参数。
1. 服务端发送Certificate Request消息：如果服务端需要客户端进行身份认证，那么服务端会发送一个Certificate Request消息给客户端，该消息告诉客户端需要提供什么类型的数字证书。客户端可以选择是否发送数字证书进行身份认证。
1. 服务端发送Server Hello Done消息：服务端发送一个Server Hello Done消息告诉客户端，服务端的Hello消息已经结束。
1. 客户端发送Certificate消息：如果服务端请求了客户端的数字证书，客户端会发送一个包含数字证书的Certificate消息给服务端。
1. 客户端发送Client Key Exchange消息：客户端生成一个随机数，称为Pre-Master Secret，然后用服务端的公钥加密该随机数，并发送一个Client Key Exchange消息给服务端。
1. 客户端发送Certificate Verify消息：如果客户端使用数字证书进行身份认证，客户端会发送一个Certificate Verify消息给服务端，该消息包含了客户端使用私钥对握手过程中某些数据的签名。服务端可以通过客户端的数字证书中的公钥来验证该签名。
1. 客户端发送Change Cipher Spec消息：客户端发送Change Cipher Spec消息表示客户端即将开始使用新的加密套件进行通信。该消息仅包含一个字节，即值为1的字节。客户端发送该消息后，就使用新的加密套件进行加密和解密。
1. 客户端发送Finished消息：客户端发送Finished消息，该消息包含客户端发送的所有信息的哈希值，以验证服务端是否正确地解密了和处理了所有信息。客户端将使用在协商过程中生成的对称密钥计算所有先前发送的消息的哈希值，并将该哈希值放入Finished消息中。客户端也将使用与服务端协商的加密套件来加密Finished消息。服务端收到该消息后，也会使用协商的对称密钥计算所有先前接收的消息的哈希值，并将其与接收到的Finished消息中的哈希值进行比较，以验证客户端是否正确地解密了和处理了所有信息。
1. 服务端发送Change Cipher Spec消息：服务端发送Change Cipher Spec消息表示服务端即将开始使用新的加密套件进行通信。该消息仅包含一个字节，即值为1的字节。服务端发送该消息后，就使用新的加密套件进行加密和解密。
1. 服务端发送Finished消息：服务端发送Finished消息，该消息包含服务端发送的所有信息的哈希值，以验证客户端是否正确地解密了和处理了所有信息。服务端将使用在协商过程中生成的对称密钥计算所有先前发送的消息的哈希值，并将该哈希值放入Finished消息中。服务端也将使用与客户端协商的加密套件来加密Finished消息。客户端收到该消息后，也会使用协商的对称密钥计算所有先前接收的消息的哈希值，并将其与接收到的Finished消息中的哈希值进行比较，以验证服务端是否正确地解密了和处理了所有信息。
1. 客户端和服务端完成握手：一旦服务端成功验证了客户端的身份，并且客户端成功验证了服务端的身份，客户端和服务端就可以开始使用TLS协议进行通信了。客户端和服务端都会使用协商出的对称密钥来加密和解密通信数据。


## tls1.3协议流程
### Client Hello
> TLS 1.3的Client Hello主要包括以下内容：
1. Protocol Version：协议版本，指示客户端使用的TLS版本。
1. Random：32字节的随机数，用于生成后续的密钥材料。
1. Cipher Suites：加密套件列表，客户端支持的加密套件。
1. Compression Methods：压缩方法列表，客户端支持的压缩方法。
1. Extensions：扩展列表，包含一些额外的功能或配置信息。

> 其中Extensions中可能包括以下扩展：
1. Supported Versions：支持的协议版本列表，允许客户端通知服务器它支持的其他TLS版本，以便允许协议降级。
1. Key Share：客户端提供的Diffie-Hellman公钥，用于计算共享密钥。
1. Signature Algorithms：支持的签名算法列表，用于证书验证和消息完整性检查。
1. Application-Layer Protocol Negotiation (ALPN)：应用层协议协商，允许客户端指定它支持的应用层协议。
1. Pre-Shared Key (PSK) Extension：提供一种预共享密钥的方式，可以加速协议握手的过程。


1. Extension: server_name (len=28)             //此扩展是为了支持服务器名称指示（Server Name Indication, SNI），客户端通过在Client Hello中包含SNI，告诉服务器它要连接的是哪个主机名的服务器。这样服务器就可以根据主机名选择对应的证书和配置。
1. Extension: ec_point_formats (len=4)         //此扩展是指定客户端支持的椭圆曲线点的格式。TLS 1.3中只支持uncompressed格式，所以该扩展的值为0x00。
1. Extension: supported_groups (len=14)        //此扩展是指定客户端支持的椭圆曲线组。TLS 1.3中支持多种椭圆曲线组，包括secp256r1、secp384r1、secp521r1等。
1. Extension: encrypt_then_mac (len=0)         //此扩展指示客户端是否支持“先加密再认证”（Encrypt-then-MAC）模式。如果支持，则表示对于所有加密套件都将使用这种模式。
1. Extension: extended_master_secret (len=0)   //此扩展指示客户端是否支持扩展的主密钥（Extended Master Secret）协议。如果支持，则表示客户端和服务器将使用扩展的主密钥协议，用于提供更好的前向安全性。
1. Extension: post_handshake_auth (len=0)      //此扩展指示客户端是否支持Post-Handshake身份验证，如果支持，则表示客户端在连接建立后仍然可以通过身份验证来重新验证服务器。
1. Extension: signature_algorithms (len=50)    //此扩展指定客户端支持的数字签名算法，包括RSA、DSA、ECDSA等。这些算法将用于证书验证和签名。
1. Extension: supported_versions (len=9)       //此扩展指定客户端支持的TLS版本，包括TLS 1.3、TLS 1.2、TLS 1.1、TLS 1.0等。
1. Extension: psk_key_exchange_modes (len=2)   //此扩展指定客户端支持的预共享密钥（PSK）密钥交换模式，包括psk_ke（PSK密钥交换）和psk_dhe_ke（基于PSK的DHE密钥交换）。
1. Extension: key_share (len=38)               //此扩展是TLS 1.3中最重要的扩展之一，用于在客户端和服务器之间协商出共享密钥。客户端使用Diffie-Hellman算法生成一对公钥和私钥，并将公钥包含在Client Hello中的key_share扩展中发送给服务器。服务器也使用Diffie-Hellman算法生成一对公钥和私钥，并从客户端的key_share扩展中获取客户端的公钥，使用自己的私钥和客户端的公钥计算出共享密钥，然后将自己的公钥包含在Server Hello中的key_share扩展中发送给客户端。客户端收到Server Hello后，也会使用自己的私钥和服务器的公钥计算出共享密钥。

### Server Hello
> TLS 1.3的Server Hello主要包括以下内容：
1. Protocol Version：协议版本，表明服务器所选用的TLS协议版本。
1. Random：32字节的随机数，用于后续计算生成对称密钥。
1. Cipher Suite：服务器选择的加密套件，表示服务器和客户端将使用哪种加密算法、哈希算法等进行通信。

> Extension：服务端可以在Server Hello中发送一些扩展信息，以便进一步协商密钥材料和其他参数。常见的扩展包括：
1. key_share：服务器向客户端发送它选择的ECDHE（或DHE）参数。
1. supported_versions：服务器告知客户端支持哪些TLS版本。
1. signature_algorithms：服务器告知客户端支持哪些签名算法。
1. pre_shared_key：如果采用PSK模式，服务器会向客户端发送预共享密钥的ID。
1. certificate_request：如果启用了客户端认证，服务器会向客户端发送证书请求，告知客户端需要提供哪些证书信息。

### Change Cipher Spec
> Change Cipher Spec是TLS协议中的一个特殊消息，用于通知对方切换加密算法。TLS 1.3中，Change Cipher Spec消息只有一个字节，值为0x01。在TLS 1.3中，Change Cipher Spec消息的发送时机如下：
1. 客户端收到Server Hello后，发送Change Cipher Spec消息。
1. 服务器收到Client Hello后，发送Change Cipher Spec消息。
1. 服务器收到Client Finished后，发送Change Cipher Spec消息。
