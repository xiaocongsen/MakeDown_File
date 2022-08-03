服务端启动，等待客户端连接时处理初始状态为 TLS_ST_BEFORE (0)
客户端启动，处理初始状态为 TLS_ST_BEFORE (0)
1.客户端通过tcp与服务端建立连接

服务端：
2. ossl_statem_server_write_transition 感知到连接成功后函数返回 WRITE_TRAN_FINISHED 将消息流状态设置为读取数据中 MSG_FLOW_READING 后调用read_state_machine 等待客户端发送数据



7. tls_get_message_header 服务端收到客户端TLS_ST_CW_CLNT_HELLO的数据获取消息头为SSL3_MT_CLIENT_HELLO(1)
8. ossl_statem_server_read_transition 将处理状态由 TLS_ST_BEFORE 设置为 TLS_ST_SR_CLNT_HELLO
9. ossl_statem_server_max_message_size  返回TLS_ST_SR_CLNT_HELLO消息允许的最大长度。排除消息头。CLIENT_HELLO_MAX_LENGTH(131396)
10. ossl_statem_server_process_message->tls_process_client_hello     解析服务器从客户端收到的数据进行验证。成功返回MSG_PROCESS_CONTINUE_PROCESSING 并将读状态设置为READ_STATE_POST_PROCESS, 将读工作阶段设置为WORK_MORE_A
11.  由于读状态为READ_STATE_POST_PROCESS,所有调用ossl_statem_server_post_process_message->tls_post_process_client_hello 进行进一步处理业务逻辑
{
    1.如果需要，让证书回调更新服务器证书
    2.从客户提供的密码中选择一个密码
    3.检查我们是否应该禁用会话恢复
    4.ssl3 摘要缓存记录
    5.处理我们之前 无法检查的TLS扩展
    6.读工作阶段设置为WORK_MORE_B
    7.ssl检查srp ext的ClientHello
    8.读工作阶段设置为WORK_FINISHED_STOP 证明已经完成读处理
}
12 读取处理完成,将消息流状态切换为MSG_FLOW_WRITING 并调用init_write_state_machine将写状态设置为 WRITE_STATE_TRANSITION
13 通过写状态WRITE_STATE_TRANSITION调用ossl_statem_server_write_transition 将状态 TLS_ST_SR_CLNT_HELLO 设置为 TLS_ST_SW_SRVR_HELLO 并返回WRITE_TRAN_CONTINUE 将写状态设置为WRITE_STATE_PRE_WORK 并将写工作阶段设置为WORK_MORE_A
14. 通过写状态WRITE_STATE_PRE_WORK调用ossl_statem_server_pre_work TLS_ST_SW_SRVR_HELLO无发消息前处理 并返回WORK_FINISHED_CONTINUE 并将写状态设置为WRITE_STATE_SEND
15. 继续调用ossl_statem_server_construct_message->tls_construct_server_hello 构造一条从服务端发送到客户端的TLS_ST_SW_SRVR_HELLO消息
{
        {02} {00 00 46} {01} {01} {55 12 50 3c 44 98 5e 8c b9 ad 4c af 5a 89 aa 0c f7 f0 75 85 dc c6 39 ab 67 bd 7b 75 b8 15 51 0d} 
        {20} {10 40 59 6d 11 20 da eb 43 91 2a 4f bc db e1 33 ae e0 6e f4 f1 e6 c3 c0 0b f0 c7 71 fa a1 34 d9} {e0} {13}
        头 4个字节 前一个字节存放消息类型SSL3_MT_SERVER_HELLO(02) 后3个字节存放消息长度
        版本号 1个字节 存放协议版本号
        处理后的版本号 1个字节    存放&0xff后的版本号
        服务端随机数   32个字节     存放服务端随机数
        会话id长度      1个字节     
        会话id数据      有前面长度决定大小  长度为0不存在该字段
        选中的加密套件   1字节   通过ssl_cipher_list_to_bytes得到支持的加密套件
        支持压缩算法的数量  1个字节         0不支持
        支持压缩算法的id    一个字节一个id  数量为0不存在该字段

}
16. 在write_state_machine调用statem_do_write 将构造好的TLS_ST_SW_SRVR_HELLO消息写入发送缓存中,将写状态设置为WORK_FINISHED_CONTINUE 将写工作阶段设置为WORK_MORE_A
17  并调用ossl_statem_server_post_work TLS_ST_SW_SRVR_HELLO发送数据前无其他操作 将写工作阶段设置为WORK_FINISHED_CONTINUE 并将写状态设置为WRITE_STATE_TRANSITION
18 
11 通过写状态WRITE_STATE_TRANSITION调用ossl_statem_server_write_transition  将处理状态由TLS_ST_SW_SRVR_HELLO 设置为 TLS_ST_SW_CERT 并返回WRITE_TRAN_CONTINUE 将写状态设置为WRITE_STATE_PRE_WORK 并将写工作阶段设置为WORK_MORE_A
12 通过写状态WRITE_STATE_PRE_WORK调用ossl_statem_server_pre_work 发消息到客户端之前处理函数 无处理 返回并将写工作阶段设置为WORK_FINISHED_CONTINUE 设置为写数据状态 WRITE_STATE_SEND
13 ossl_statem_server_construct_message调用gmtls_construct_server_certificate构造一条TLS_ST_SW_CERT消息
{ 
    [{0b} {00 04 4b} {00 04 48} {00 02 21} {30 82 02 1d ...} {00 02 21} {30 82 02 1d ...}]
    头 4个字节 前一个字节存放消息类型SSL3_MT_CERTIFICATE(11) 后3个字节存放消息长度
    所有证书数据长度 3个字节   多本证书的数据总长度
    签名证书数据长度  3个字节 通过i2d_X509返回值获取  一本证书的数据长度
    签名证书数据        通过i2d_X509传入参数获取
    加密证书数据长度     3个字节 通过i2d_X509返回值获取  一本证书的数据长度
    加密证书数据        通过i2d_X509传入参数获取
    ...证书链
}
14. 在write_state_machine调用statem_do_write 将构造好的消息写入发送缓存中,将写状态设置为WRITE_STATE_POST_WORK 将写工作阶段设置为WORK_MORE_A
15 ossl_statem_server_post_work TLS_ST_SW_CERT发送数据前无其他操作 将写工作阶段设置为WORK_FINISHED_CONTINUE 并将写状态设置为WRITE_STATE_TRANSITION
16 通过写状态WRITE_STATE_TRANSITION调用ossl_statem_server_write_transition 将处理状态由 TLS_ST_SW_CERT 设置为 TLS_ST_SW_KEY_EXCH 并返回WRITE_TRAN_CONTINUE 将写状态设置为WRITE_STATE_PRE_WORK 并将写工作阶段设置为WORK_MORE_A
17 通过写状态WRITE_STATE_PRE_WORK调用ossl_statem_server_pre_work 发消息到客户端之前处理函数 无处理 返回并将写工作阶段设置为WORK_FINISHED_CONTINUE 将写状态设置为 WRITE_STATE_SEND
18 ossl_statem_server_construct_message调用gmtls_construct_server_key_exchange 构造一条 TLS_ST_SW_KEY_EXCH 消息
{
        1.准备签名密钥
        2.准备加密证书buff
        3.签名摘要{客户端随机、服务器随机、加密证书}
        4.生成签名
        
        [{0c} {00 00 4a} {00 48} {30 46 02 21 00 b5 ...}]
        头 4个字节 前一个字节存放消息类型SSL3_MT_SERVER_KEY_EXCHANGE(12) 后3个字节存放消息长度
        签名数据长度    2个字节
        签名数据        
}
19. 在write_state_machine调用statem_do_write 将构造好的消息写入发送缓存中,将写状态设置为WRITE_STATE_POST_WORK 将写工作阶段设置为WORK_MORE_A

20 ossl_statem_server_post_work TLS_ST_SW_KEY_EXCH发送数据前无其他操作 将写工作阶段设置为WORK_FINISHED_CONTINUE 并将写状态设置为WRITE_STATE_TRANSITION
21 通过写状态WRITE_STATE_TRANSITION调用ossl_statem_server_write_transition 将处理状态由 TLS_ST_SW_KEY_EXCH 设置为 TLS_ST_SW_SRVR_DONE 并返回WRITE_TRAN_CONTINUE 将写状态设置为WRITE_STATE_PRE_WORK 并将写工作阶段设置为WORK_MORE_A
22 通过写状态WRITE_STATE_PRE_WORK调用ossl_statem_server_pre_work TLS_ST_SW_SRVR_DONE发消息到客户端之前处理函数 无处理 返回并将写工作阶段设置为WORK_FINISHED_CONTINUE 将写状态设置为 WRITE_STATE_SEND
23 ossl_statem_server_construct_message->tls_construct_server_done 构造一条 TLS_ST_SW_SRVR_DONE 消息
{
    {0e} {00 00 00}
    头 4个字节 前一个字节存放消息类型SSL3_MT_SERVER_DONE(14) 后3个字节存放消息长度长度为0
}
24. 在write_state_machine调用statem_do_write 将构造好的消息写入发送缓存中,将写状态设置为WRITE_STATE_POST_WORK 将写工作阶段设置为WORK_MORE_A


25 ossl_statem_server_post_work 调用statem_flush发送缓存中的消息 并返回WORK_FINISHED_CONTINUE
26 ossl_statem_server_write_transition 消息流状态修改为MSG_FLOW_READING 读状态修改为READ_STATE_HEADER 调用tls_get_message_header等待客户端数据头

53 收到客户端数据头 调用ossl_statem_server_read_transition 将处理状态由 TLS_ST_SW_SRVR_DONE 修改为 TLS_ST_SR_KEY_EXCH
54 ossl_statem_server_max_message_size 返回我们正在读取的当前消息允许的最大长度。排除消息头 CLIENT_KEY_EXCH_MAX_LENGTH 2048。
55 ossl_statem_server_process_message调用gmtls_process_client_key_exchange 处理服务器从客户端收到的数据。
{
    1. 准备解密密钥
    2. 解析加密的预主密钥
    3. 解密加密的预主密钥
}
ossl_statem_server_post_process_message->tls_post_process_client_key_exchange 解析完成客户端数据后处理逻辑，执行所需的任何进一步处理 
ossl_statem_server_read_transition 将处理状态由 TLS_ST_SR_KEY_EXCH 修改为 TLS_ST_SR_CHANGE
ossl_statem_server_max_message_size 返回我们正在读取的当前消息允许的最大长度。排除消息头。 CCS_MAX_LENGTH 1
ossl_statem_server_process_message->tls_process_change_cipher_spec  处理服务器从客户端收到的数据。
ossl_statem_server_read_transition 将处理状态由 TLS_ST_SR_CHANGE 修改为 TLS_ST_SR_FINISHED
ossl_statem_server_max_message_size 返回我们正在读取的当前消息允许的最大长度。排除消息头。 FINISHED_MAX_LENGTH 64
ossl_statem_server_process_message->tls_process_finished  处理服务器从客户端收到的数据。
ossl_statem_server_write_transition 将处理状态由 TLS_ST_SR_FINISHED 改变为 TLS_ST_SW_CHANGE 数据状态修改为MSG_FLOW_READING 等待客户端发送数据
ossl_statem_server_pre_work 发消息到客户端之前处理函数 tls1_setup_key_block 
ossl_statem_server_construct_message->tls_construct_change_cipher_spec






客户端：
2. ossl_statem_client_write_transition 客户端tcp连接后将处理状态从 TLS_ST_BEFORE 设置为 TLS_ST_CW_CLNT_HELLO 并返回WRITE_TRAN_CONTINUE
3.ossl_statem_client_pre_work  处理发 TLS_ST_CW_CLNT_HELLO 消息到服务端之前处理函数 设置shutdown值为0 返回WORK_FINISHED_CONTINUE (继续下一步操作) 将写状态设置为WRITE_STATE_SEND
4.ossl_statem_client_construct_message->tls_construct_client_hello 构造一条从客户端发送到服务器的消息。构建成功后处理写状态WRITE_STATE_SEND的逻辑
{
    确定使用协议版本
    创建新的会话对象或者使用预加载的会话
    填充客户端随机数字段
    构建消息类型和长度
    [
        {01} {00 00 2b} {01} {01} {c3 e7 f1 1b 1f df a2 63 04 23 cd 40 ce c4 50 18 c8 8f a0 68 dc 36 0c 91 fc e0 bf a0 ef d5 b0 0f} {00} {00 04} {e0 13 00 ff} {01}
        头 4个字节 前一个字节存放消息类型SSL3_MT_CLIENT_HELLO(01) 后3个字节存放消息长度
        版本号 1个字节 存放协议版本号
        处理后的版本号 1个字节    存放&0xff后的版本号
        客户端随机数   32个字节     存放客户端随机数
        会话id长度      1个字节     
        会话id数据      有前面长度决定大小  长度为0不存在该字段
        支持的加密套件数量  占用2个字节 通过ssl_cipher_list_to_bytes得到支持的加密套件数量
        支持的加密套件   一个占用一个字节   通过ssl_cipher_list_to_bytes得到支持的加密套件
        支持压缩算法的数量  1个字节         0不支持
        支持压缩算法的id    一个字节一个id  数量为0不存在该字段
    ]
}
5. 在write_state_machine调用statem_do_write 将构造好的消息写入发送缓存中,将写状态设置为WRITE_STATE_POST_WORK 将写工作阶段设置为WORK_MORE_A
6. ossl_statem_client_post_work 调用statem_flush发送缓存中的消息 并返回WORK_FINISHED_CONTINUE
7. ossl_statem_client_write_transition  不知道收到什么数据直接返回WRITE_TRAN_FINISHED 并将消息流状态设置为 MSG_FLOW_READING 调用read_state_machine 等待接收服务端数据

26 收到服务端数据 且读状态为READ_STATE_HEADER
27 ossl_statem_client_read_transition 将处理状态由 TLS_ST_CW_CLNT_HELLO变成 TLS_ST_CR_SRVR_HELLO
28 ossl_statem_client_max_message_size  返回我们正在读取的当前消息允许的最大长度。排除消息头。 SERVER_HELLO_MAX_LENGTH 20000 
29 读状态切换为READ_STATE_BODY 通过tls_get_message_body读取消息数据
30 ossl_statem_client_process_message->tls_process_server_hello TLS_ST_CR_SRVR_HELLO处理从服务器接收到客户端的消息。
{
    1. 获取版本号
    2. 加载服务端随机数
    3. 获取会话id
    4. 加载协商的加密套件
    5. 加载压缩算法
}
31 读状态切换为READ_STATE_HEADER 通过tls_get_message_header读取消息头
32 ossl_statem_client_read_transition 将处理状态由 TLS_ST_CR_SRVR_HELLO 变成 TLS_ST_CR_CERT
33 ossl_statem_client_max_message_size  返回我们正在读取的当前消息允许的最大长度。排除消息头。 max_cert_list:102400
34 ossl_statem_client_process_message->tls_process_server_certificate TLS_ST_CR_CERT处理从服务器接收到的数据处理
35 ossl_statem_client_read_transition 将处理状态由 TLS_ST_CR_CERT 变成 TLS_ST_CR_KEY_EXCH
36 ossl_statem_client_max_message_size  返回我们正在读取的当前消息允许的最大长度。排除消息头。 SERVER_KEY_EXCH_MAX_LENGTH 102400
37 ossl_statem_client_process_message->gmtls_process_server_key_exchange TLS_ST_CR_KEY_EXCH 处理从服务器接收到客户端的消息.
38 ossl_statem_client_read_transition 将处理状态由 TLS_ST_CR_KEY_EXCH 变成 TLS_ST_CR_SRVR_DONE
39 ossl_statem_client_max_message_size  返回我们正在读取的当前消息允许的最大长度。排除消息头。 SERVER_HELLO_DONE_MAX_LENGTH 0
40 ossl_statem_client_process_message->tls_process_server_done TLS_ST_CR_SRVR_DONE 处理接收服务器的消息完成.
{
    1. 检查服务器是否提供了所需的内容
    2. 返回MSG_PROCESS_FINISHED_READING
    3. 设置消息流状态为MSG_FLOW_WRITING
}
41 ossl_statem_client_write_transition将处理状态由 TLS_ST_CR_SRVR_DONE 设置为 TLS_ST_CW_KEY_EXCH 并返回WRITE_TRAN_CONTINUE  将写状态设置为WRITE_STATE_PRE_WORK 并将写工作阶段设置为WORK_MORE_A
42 ossl_statem_client_pre_work  TLS_ST_CW_KEY_EXCH发消息到客户端之前处理函数 无处理 返回并将写工作阶段设置为WORK_FINISHED_CONTINUE 将写状态设置为 WRITE_STATE_SEND
43 ossl_statem_client_construct_message->gmtls_construct_client_key_exchange 构造一条 TLS_ST_CW_KEY_EXCH 消息。
{
    1. 从enc证书获取sm2加密密钥
    2.生成预主密钥

    {10} {00 00 9f} {00 9d} {30 81 9a 02 21 ...}
    头 4个字节 前一个字节存放消息类型SSL3_MT_CLIENT_KEY_EXCHANGE(16) 后3个字节存放消息长度
    预主密钥长度 2个字节
    预主密钥数据    
}

44 ossl_statem_client_post_work调用tls_client_key_exchange_post_work发送TLS_ST_CW_KEY_EXCH消息后需要完成的
{
    1.检查 SRP
    2.生成主密钥
}
45 ossl_statem_client_write_transition 处理状态由 TLS_ST_CW_KEY_EXCH 改为 TLS_ST_CW_CHANGE 返回WRITE_TRAN_CONTINUE  将写状态设置为WRITE_STATE_PRE_WORK 并将写工作阶段设置为WORK_MORE_A
46 ossl_statem_client_pre_work  TLS_ST_CW_CHANGE发消息到服务端之前处理函数 无处理 返回并将写工作阶段设置为WORK_FINISHED_CONTINUE 将写状态设置为 WRITE_STATE_SEND
47 ossl_statem_client_construct_message->tls_construct_change_cipher_spec 构造一条 TLS_ST_CW_CHANGE 消息从客户端发送到服务器的消息。
{
        SSL3_MT_CCS(1)
}
48 ossl_statem_client_post_work调用tls_client_key_exchange_post_work 执行从客户端向服务器发送TLS_ST_CW_CHANGE消息后需要完成 设置密钥块。
49 ossl_statem_client_write_transition 状态 TLS_ST_CW_CHANGE 修改为 TLS_ST_CW_FINISHED 返回WRITE_TRAN_CONTINUE 将写状态设置为WRITE_STATE_PRE_WORK 并将写工作阶段设置为WORK_MORE_A
50 ossl_statem_client_pre_work  TLS_ST_CW_FINISHED发消息到服务端之前处理函数 无处理  返回WORK_FINISHED_CONTINUE
51 ossl_statem_client_construct_message调用tls_construct_finished 构造一条 TLS_ST_CW_FINISHED 消息
{
    SSL3_MT_FINISHED(20)
    {14} {00 00 0c} {a1 8a 9d 41 d6 62 47 ce cb cf d2 9c}
}
52 ossl_statem_client_post_work 调用statem_flush发送缓存中的消息 并返回WORK_FINISHED_CONTINUE
53 ossl_statem_client_write_transition 返回WRITE_TRAN_FINISHED 将消息流状态设置为读取数据中 MSG_FLOW_READING 后调用read_state_machine 等待客户端发送数据