# SSL出错源码位置获取
``` c
    SSL_load_error_strings();           //加载错误码对应信息
    unsigned long ret = ERR_get_error(); //获取错误码
    char *szErrMsg[1024] = {0};
    ERR_error_string(ret,szErrMsg); //获取错误信息 格式： error:errld:库:函数:原因
```

# tls1.3 关键代码记录
```c
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
    tls13_enc,
    tls1_mac,
    tls13_setup_key_block,
    tls13_generate_master_secret,
    tls13_change_cipher_state,
    tls13_final_finish_mac,
    TLS_MD_CLIENT_FINISH_CONST, TLS_MD_CLIENT_FINISH_CONST_SIZE,
    TLS_MD_SERVER_FINISH_CONST, TLS_MD_SERVER_FINISH_CONST_SIZE,
    tls13_alert_code,
    tls13_export_keying_material,
    SSL_ENC_FLAG_SIGALGS | SSL_ENC_FLAG_SHA256_PRF,
    ssl3_set_handshake_header,
    tls_close_construct_packet,
    ssl3_handshake_write
};

```



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