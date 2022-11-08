# socket优化
```c
        //关闭 NODELAY算法
        int uiErr = setsockopt(pstConInf->uiFd, IPPROTO_TCP, TCP_NODELAY, (CHAR *)&iOpt, sizeof(iOpt));
        if (VOS_OK != uiErr)
        {
            Log(DDM_LOG_NETC,DDM_LOG_LEVEL_WARNING,"[SSL Connect][set TCP NODELAY flag error, error is %d]",uiErr);
        }
        else
        {
           Log(DDM_LOG_NETC,DDM_LOG_LEVEL_INFO,"[SSL Connect][set TCP NODELAY flag ok]");
        }


        //设置端口复用
        LinuxRet = VOS_Setsockopt(uiFd, SOL_SOCKET, SO_REUSEADDR, (char*)&uiReuseAddr, sizeof(uiReuseAddr));

        if (VOS_OK != LinuxRet)
        {
            DDM_Log(DDM_LOG_UI, DDM_LOG_LEVEL_ERROR, "[Process plugin listen port create][Fail to set plugin listen socket reuseaddr!]");
            (VOS_VOID)VOS_CloseSocket(uiFd);
            return VOS_ERR;
        }
```

# 域套接字
```c
UINT32 CSDK_ListenPort_Create(UINT32 *puiPort)
{
    UINT32 LinuxuiBindRet;
    UINT32 LinuxuiListenRet;
    UINT32 uiFd;
    UINT32 uiCurPort = 11035;
    CHAR tmpBuff[32] = {0};
    struct  sockaddr_un LinuxserverAddr = {0};

    FLAG_CREATE_SOCKET:
    
    uiFd = socket(AF_UNIX, SOCK_STREAM, IPPROTO_TCP);
    if(-1 == uiFd)
    {
        return -1;
    }

    // 设定服务端地址及端口
    memset(&LinuxserverAddr,0,sizeof(LinuxserverAddr));
    LinuxserverAddr.sun_family = AF_UNIX;
    sprintf(tmpBuff, 32, "/tmp/%d", uiCurPort);
    memcpy(LinuxserverAddr.sun_path,tmpBuff,strlen(tmpBuff));
    unlink(tmpBuff);
    // 绑定
    LinuxuiBindRet = bind(uiFd, (struct sockaddr*)&LinuxserverAddr, sizeof(LinuxserverAddr));
    if(0 != LinuxuiBindRet)
    {
        //递增端口号
        ++uiCurPort;
        (VOID)close(uiFd);
        goto FLAG_CREATE_SOCKET;
    }

    // 监听，最大接受10个连接
    LinuxuiListenRet = listen(uiFd, 10);
    if(0 != LinuxuiListenRet)
    {
        (VOID)close(uiFd);
        return -1;
    }
    fd_set Readfds;
    FD_ZERO(&Readfds);
    FD_SET(uiFd, &Readfds);

     // timeout setting
        tv.tv_sec = 30;
        tv.tv_usec = 0;

        LinuxRet = VOS_Select((VOS_INT32)(LinuxMaxFd + 1), &Readfds, NULL, NULL, &tv);
        if(VOS_NULL_LONG == LinuxRet)
        {
            DDM_Log(DDM_LOG_CADM,DDM_LOG_LEVEL_ERROR, "[CSDK recv thread select failed]");
            break;
        }
}
```