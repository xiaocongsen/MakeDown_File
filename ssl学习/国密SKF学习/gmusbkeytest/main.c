#include <stdio.h>
#include <SKFAPI.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/err.h>

void PrintError(char *szFunName,ULONG dwErrorCode)
{
    printf("the Fun %s failed! the ErrorCode is 0x%0lx\n",szFunName,dwErrorCode);
}

// 读取二进制证书数据
X509 *readBinaryCertificate(const unsigned char *binaryData, size_t length)
{
    BIO *bio = BIO_new_mem_buf(binaryData, length);
    X509 *cert = NULL;

    cert = d2i_X509_bio(bio, &cert);

    BIO_free(bio);

    return cert;
}

// 将证书写入 PEM 格式文件
int writeCertificateToPEMFile(X509 *cert, const char *filename)
{
    BIO *bio = BIO_new_file(filename, "w");
    int result = 0;

    if (bio != NULL) {
        result = PEM_write_bio_X509(bio, cert);
        BIO_free(bio);
    }

    return result;
}

// 函数定义
void print_strings(const char *memory, size_t memory_size) {
    const char *start = memory;
    const char *end;
    size_t pos = 0;

    while (pos < memory_size) {
        end = strchr(start, '\0'); // 查找下一个\0的位置
        if (end == NULL ) {
            break;
        } else {
            // 打印从start到end-1的内容
            printf("%s\n", start);
        }
        pos += end - start + 1; // 更新已遍历的字节数
        start = end + 1; // 更新起始位置为下一个字符串的开始
        if(*start == '\0')
        {
            start++;
            pos++;
        }
    }
}

SKF_API *GetSKFAPI(CHAR **errorMessage)
{
    static SKF_API *api = NULL; // 初始化结构体为0
    if (api != NULL)
    {
        return api;
    }
#ifdef _WIN64
#if defined(_UNICODE) || defined(UNICODE)
    LPCTSTR acSKFLibPath = L"C:/Windows/System32/JITGMKEY_SJK1424.dll";
#else
    LPCTSTR acSKFLibPath = "C:/Windows/System32/JITGMKEY_SJK1424.dll";
#endif
#else
#if defined(_UNICODE) || defined(UNICODE)
    LPCTSTR acSKFLibPath = L"C:/Windows/SysWOW64/JITGMKEY_SJK1424.dll";
#else
    LPCTSTR acSKFLibPath = "C:/Windows/SysWOW64/JITGMKEY_SJK1424.dll";
#endif
#endif
    HMODULE hModule = NULL;
    // 加载DLL
    hModule = LoadLibraryEx(acSKFLibPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (hModule == NULL)
    {
        *errorMessage = strdup("Failed to load library.");
        return NULL;
    }

    api = (SKF_API *)VOS_Malloc(0, sizeof(SKF_API));
    if (api == NULL)
    {
        *errorMessage = strdup("Failed to allocate memory for SKF_API");
        FreeLibrary(hModule);
        return NULL;
    }

    // 获取函数地址
#define GETProcAddress(name)                                                    \
    api->pf##name = (PFN_##name)GetProcAddress(hModule, #name);                 \
    if (api->pf##name == NULL)                                                  \
    {                                                                           \
        *errorMessage = strdup(("Failed to get procedure address for " #name)); \
        VOS_Free(api);                                                          \
        api = NULL;                                                             \
        FreeLibrary(hModule);                                                   \
        return NULL;                                                            \
    }
    GETProcAddress(SKF_VerifyPIN);
    GETProcAddress(SKF_GetPINInfo);
    GETProcAddress(SKF_UnblockPIN);
    GETProcAddress(SKF_ClearSecureState);
    GETProcAddress(SKF_CreateApplication);
    GETProcAddress(SKF_EnumApplication);
    GETProcAddress(SKF_DeleteApplication);
    GETProcAddress(SKF_OpenApplication);
    GETProcAddress(SKF_CloseApplication);
    GETProcAddress(SKF_CreateFile);
    GETProcAddress(SKF_DeleteFile);
    GETProcAddress(SKF_EnumFiles);
    GETProcAddress(SKF_GetFileInfo);
    GETProcAddress(SKF_ReadFile);
    GETProcAddress(SKF_WriteFile);
    GETProcAddress(SKF_GenRandom);
    GETProcAddress(SKF_GenExtRSAKey);
    GETProcAddress(SKF_GenRSAKeyPair);
    GETProcAddress(SKF_ImportRSAKeyPair);
    GETProcAddress(SKF_RSASignData);
    GETProcAddress(SKF_RSAVerify);
    GETProcAddress(SKF_RSAExportSessionKey);
    GETProcAddress(SKF_ExtRSAPubKeyOperation);
    GETProcAddress(SKF_ExtRSAPriKeyOperation);
    GETProcAddress(SKF_GenECCKeyPair);
    GETProcAddress(SKF_ImportECCKeyPair);
    GETProcAddress(SKF_ECCSignData);
    GETProcAddress(SKF_ECCVerify);
    GETProcAddress(SKF_ECCExportSessionKey);
    GETProcAddress(SKF_ExtECCEncrypt);
    GETProcAddress(SKF_ExtECCDecrypt);
    GETProcAddress(SKF_ExtECCSign);
    GETProcAddress(SKF_ExtECCVerify);
    GETProcAddress(SKF_ExportPublicKey);
    GETProcAddress(SKF_ImportSessionKey);
    GETProcAddress(SKF_SetSymmKey);
    GETProcAddress(SKF_EncryptInit);
    GETProcAddress(SKF_Encrypt);
    GETProcAddress(SKF_EncryptUpdate);
    GETProcAddress(SKF_EncryptFinal);
    GETProcAddress(SKF_DecryptInit);
    GETProcAddress(SKF_Decrypt);
    GETProcAddress(SKF_DecryptUpdate);
    GETProcAddress(SKF_DecryptFinal);
    GETProcAddress(SKF_DigestInit);
    GETProcAddress(SKF_Digest);
    GETProcAddress(SKF_DigestUpdate);
    GETProcAddress(SKF_DigestFinal);
    GETProcAddress(SKF_MacInit);
    GETProcAddress(SKF_Mac);
    GETProcAddress(SKF_MacUpdate);
    GETProcAddress(SKF_MacFinal);
    GETProcAddress(SKF_CloseHandle);
    GETProcAddress(SKF_CreateContainer);
    GETProcAddress(SKF_DeleteContainer);
    GETProcAddress(SKF_OpenContainer);
    GETProcAddress(SKF_CloseContainer);
    GETProcAddress(SKF_EnumContainer);
    GETProcAddress(SKF_GetContainerType);
    GETProcAddress(SKF_ImportCertificate);
    GETProcAddress(SKF_ExportCertificate);
    GETProcAddress(SKF_EnumDev);
    GETProcAddress(SKF_ConnectDev);
    GETProcAddress(SKF_DisConnectDev);
    GETProcAddress(SKF_WaitForDevEvent);
    GETProcAddress(SKF_CancelWaitForDevEvent);

#undef GETProcAddress

    return api;
}

// 获取所有应用程序句柄的函数
int GetAppHandles(DEVHANDLE hDev, HAPPLICATION **phApps, ULONG *pNumApps) {
    ULONG rv = 0;
    // 获取应用程序列表的长度
    char *szAppList = NULL;
    ULONG ulAppListLen = 0;
    CHAR *errmsg = NULL;
    SKF_API *pf_skf = GetSKFAPI(&errmsg);
    if (errmsg != NULL)
    {
        DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[wait For Device Event][Get SKFAPI error:%s]", errmsg);
        free(errmsg);
        return NULL;
    }

    rv = pf_skf->pfSKF_EnumApplication(hDev, szAppList, &ulAppListLen);
    if (rv != SAR_OK) {
        PrintError("SKF_EnumApplication", rv);
        return rv;
    }

    if (ulAppListLen < 2) {
        printf("No Application!\n");
        return -1;
    }

    // 分配内存来存储应用程序列表
    szAppList = (char *)malloc(ulAppListLen);
    if (szAppList == NULL) {
        printf("Memory Error!\n");
        return -1;
    }

    // 再次调用 SKF_EnumApplication 来获取完整的应用程序列表
    rv = pf_skf->pfSKF_EnumApplication(hDev, szAppList, &ulAppListLen);
    if (rv != SAR_OK) {
        PrintError("SKF_EnumApplication", rv);
        free(szAppList);
        return rv;
    }

    // 计算应用程序数量
    char *pp = szAppList;
    ULONG numApps = 0;
    while (szAppList + ulAppListLen - pp) {
        if (strlen(pp)) {
            numApps++;
            pp += strlen(pp) + 1; // 跳过字符串和终止符
        } else {
            pp++;
        }
    }
    if(numApps == 0)
        return 0;
    // 分配内存来存储应用程序句柄数组
    *phApps = (HAPPLICATION *)malloc(numApps * sizeof(HAPPLICATION));
    if (*phApps == NULL) {
        printf("Memory Error!\n");
        free(szAppList);
        return -1;
    }

    // 连接到每个应用程序并获取句柄
    HAPPLICATION *appHandles = *phApps;
    pp = szAppList;
    ULONG i = 0;
    while (szAppList + ulAppListLen - pp) {
        if (strlen(pp)) {
            HAPPLICATION hApp;
            rv = pf_skf->pfSKF_OpenApplication(hDev, pp, &hApp);
            if (rv != SAR_OK) {
                PrintError("SKF_OpenApplication", rv);
                free(*phApps);
                *phApps = NULL;
                free(szAppList);
                return rv;
            }
            appHandles[i++] = hApp;
            printf("Opened Application %s succeed\n", pp);
            pp += strlen(pp) + 1; // 跳过字符串和终止符
        } else {
            pp++;
        }
    }

    *pNumApps = numApps;
    free(szAppList);
    return SAR_OK;
}
// 枚举容器并返回句柄数组
int GetContainerHandles(HAPPLICATION hApp, HCONTAINER **ppHandles, ULONG *pNumContainers)
{
    ULONG rv = 0;

    // 获取容器名称列表的长度
    char *szContainerName = NULL;
    ULONG ulContainerListLen = 0;
    CHAR *errmsg = NULL;
    SKF_API *pf_skf = GetSKFAPI(&errmsg);
    if (errmsg != NULL)
    {
        DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[wait For Device Event][Get SKFAPI error:%s]", errmsg);
        free(errmsg);
        return NULL;
    }
    rv = pf_skf->pfSKF_EnumContainer(hApp, szContainerName, &ulContainerListLen);
    if (rv != SAR_OK) {
        PrintError("SKF_EnumContainer", rv);
        return rv;
    }

    if (ulContainerListLen < 2) {
        printf("No Containers!\n");
        return -1;
    }

    // 分配内存来存储容器名称列表
    szContainerName = (char *)malloc(ulContainerListLen);
    if (szContainerName == NULL) {
        printf("Memory Error!\n");
        return -1;
    }

    // 再次调用 SKF_EnumContainer 来获取完整的容器名称列表
    rv = pf_skf->pfSKF_EnumContainer(hApp, szContainerName, &ulContainerListLen);
    if (rv != SAR_OK) {
        PrintError("SKF_EnumContainer", rv);
        free(szContainerName);
        return rv;
    }

    // 计算容器数量
    char *pp = szContainerName;
    ULONG numContainers = 0;
    while (ulContainerListLen - (pp - szContainerName)) {
        if (strlen(pp)) {
            numContainers++;
            pp += strlen(pp) + 1; // 跳过字符串和终止符
        } else {
            pp++;
        }
    }
    if(numContainers == 0)
        return 0;
    // 分配内存来存储句柄数组
    *ppHandles = (HCONTAINER *)malloc(numContainers * sizeof(HCONTAINER));
    if (*ppHandles == NULL) {
        printf("Memory Error!\n");
        free(szContainerName);
        return -1;
    }
    memset(*ppHandles,0,sizeof(numContainers * sizeof(HCONTAINER)));
    // 获取每个容器的句柄
    pp = szContainerName;
    ULONG i = 0;
    while (ulContainerListLen - (pp - szContainerName)) {
        if (strlen(pp)) {
            HCONTAINER hContainer;
            rv = pf_skf->pfSKF_OpenContainer(hApp, pp, &hContainer);
            if (rv != SAR_OK) {
                PrintError("SKF_OpenContainer", rv);
                free(szContainerName);
                free(*ppHandles);
                *ppHandles = NULL;
                return rv;
            }
            (*ppHandles)[i] = hContainer;
            i++;
            pp += strlen(pp) + 1; // 跳过字符串和终止符
        } else {
            pp++;
        }
    }

    *pNumContainers = numContainers;
    free(szContainerName);
    return SAR_OK;
}
// 枚举容器并返回容器名称列表
int GetContainerNames(HAPPLICATION hApp, char ***ppContainerNames, ULONG *pNumContainers) {
    ULONG rv = 0;
    // 获取容器名称列表的长度
    char *szContainerName = NULL;
    ULONG ulContainerListLen = 0;
    CHAR *errmsg = NULL;
    SKF_API *pf_skf = GetSKFAPI(&errmsg);
    if (errmsg != NULL)
    {
        DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[wait For Device Event][Get SKFAPI error:%s]", errmsg);
        free(errmsg);
        return NULL;
    }

    rv = pf_skf->pfSKF_EnumContainer(hApp, szContainerName, &ulContainerListLen);
    if (rv != SAR_OK) {
        PrintError("SKF_EnumContainer", rv);
        return rv;
    }

    if (ulContainerListLen < 2) {
        printf("No Containers!\n");
        return -1;
    }

    // 分配内存来存储容器名称列表
    szContainerName = (char *)malloc(ulContainerListLen);
    if (szContainerName == NULL) {
        printf("Memory Error!\n");
        return -1;
    }

    // 再次调用 SKF_EnumContainer 来获取完整的容器名称列表
    rv = pf_skf->pfSKF_EnumContainer(hApp, szContainerName, &ulContainerListLen);
    if (rv != SAR_OK) {
        PrintError("SKF_EnumContainer", rv);
        free(szContainerName);
        return rv;
    }

    // 计算容器数量
    char *pp = szContainerName;
    ULONG numContainers = 0;
    while (ulContainerListLen - (pp - szContainerName)) {
        if (strlen(pp)) {
            numContainers++;
            pp += strlen(pp) + 1; // 跳过字符串和终止符
        } else {
            pp++;
        }
    }
    if(numContainers == 0)
        return 0;
    // 分配内存来存储容器名称数组
    *ppContainerNames = (char **)malloc(numContainers * sizeof(char *));
    if (*ppContainerNames == NULL) {
        printf("Memory Error!\n");
        free(szContainerName);
        return -1;
    }

    // 分配内存来存储每个容器名称
    pp = szContainerName;
    ULONG i = 0;
    while (ulContainerListLen - (pp - szContainerName)) {
        if (strlen(pp)) {
            (*ppContainerNames)[i] = (char *)malloc(strlen(pp) + 1);
            if ((*ppContainerNames)[i] == NULL) {
                printf("Memory Error!\n");
                free(szContainerName);
                for (ULONG j = 0; j < i; j++) {
                    free((*ppContainerNames)[j]);
                }
                free(*ppContainerNames);
                *ppContainerNames = NULL;
                return -1;
            }
            strcpy((*ppContainerNames)[i], pp);
            i++;
            pp += strlen(pp) + 1; // 跳过字符串和终止符
        } else {
            pp++;
        }
    }

    *pNumContainers = numContainers;
    free(szContainerName);
    return SAR_OK;
}

ULONG RasKeyPairTest(DEVHANDLE hDev,HAPPLICATION hApp)
{

    ULONG rv = SAR_OK,ulRetryCount =0;
    HCONTAINER hCon;
    char szContainer[64]={0};
    CHAR *errmsg = NULL;
    SKF_API *pf_skf = GetSKFAPI(&errmsg);
    if (errmsg != NULL)
    {
        DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[wait For Device Event][Get SKFAPI error:%s]", errmsg);
        free(errmsg);
        return NULL;
    }

    rv = pf_skf->pfSKF_VerifyPIN(hApp,USER_TYPE,"123456",&ulRetryCount);
    if(rv)
    {
        PrintError("SKF_VerifyPIN",rv);
        return rv;
    }

    memcpy(szContainer,"RSA_Container",13);
    rv = pf_skf->pfSKF_OpenContainer(hApp,szContainer,&hCon);
    if(rv)
    {
        if(SAR_FAIL == rv)
        {
            rv = pf_skf->pfSKF_CreateContainer(hApp,szContainer,&hCon);
            if(rv)
            {
                PrintError("SKF_CreateContainer",rv);
                return rv;
            }
        }else{
            PrintError("SKF_OpenContainer",rv);
        }
    }


    RSAPUBLICKEYBLOB RsaPubKey;

    rv = pf_skf->pfSKF_GenRSAKeyPair(hCon,1024,&RsaPubKey);
    if(rv)
    {
        PrintError("SKF_GenRSAKeyPair",rv);
        return rv;
    }

    unsigned char pbData[1024]={0},pbDigest[32]={0},pbSignData[128]={0};
    ULONG ulDataLen = 1024,ulDigestLen = 32,ulSignDataLen = 128;

    ULONG i = 0;
    for(i =0;i<1024;i++)
    {
        pbData[i] = (unsigned char)((i*4+3)%256);
    }

    HANDLE hHash;

    rv = pf_skf->pfSKF_DigestInit(hDev,SGD_SHA1,NULL,NULL,0,&hHash);
    if(rv)
    {
        PrintError("SKF_DigestInit",rv);
        return rv;
    }

    rv = pf_skf->pfSKF_Digest(hHash,pbData,ulDataLen,pbDigest,&ulDigestLen);
    if(rv)
    {
        PrintError("SKF_Digest",rv);
        return rv;
    }

    printf("the Digest of the Data is :\n");
    for(i=0;i<ulDigestLen;i++)
    {
        printf("0x%02x ",pbDigest[i]);
    }

    printf("\n");

    rv = pf_skf->pfSKF_RSASignData(hCon,pbDigest,ulDigestLen,pbSignData,&ulSignDataLen);
    if(rv)
    {
        PrintError("SKF_RSASignData",rv);
        return rv;
    }

    printf("the signValue of the Data is :\n");
    for(i=0;i<ulSignDataLen;i++)
    {
        printf("0x%02x ",pbSignData[i]);
    }

    printf("\n");

    rv = pf_skf->pfSKF_RSAVerify(hDev,&RsaPubKey,pbDigest,ulDigestLen,pbSignData,ulSignDataLen);
    if(rv)
    {
        PrintError("SKF_RSAVerify",rv);
        return rv;
    }
    printf("RSA verify SignValue is succeed!\n");

    return SAR_OK;

}

void saveBinaryDataToFile(const char *filename, const unsigned char *data, size_t length)
{
    FILE *file = fopen(filename, "wb"); // 使用 "wb" 模式打开文件
    if (file == NULL)
    {
        printf("无法打开文件 %s\n", filename);
        return;
    }

    size_t bytesWritten = fwrite(data, 1, length, file); // 写入数据
    if (bytesWritten != length)
    {
        printf("写入错误\n");
    }

    fclose(file); // 关闭文件
}

ULONG KeyPairTest(DEVHANDLE hDev,HAPPLICATION hApp)
{

    ULONG rv = SAR_OK,ulRetryCount =0;
    HCONTAINER hCon;
    char szContainer[64]={0};
    ECCPUBLICKEYBLOB pEccSignKey;
    ECCPUBLICKEYBLOB pEccEncKey;
    CHAR *errmsg = NULL;
    SKF_API *pf_skf = GetSKFAPI(&errmsg);
    if (errmsg != NULL)
    {
        DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[wait For Device Event][Get SKFAPI error:%s]", errmsg);
        free(errmsg);
        return NULL;
    }
    rv = pf_skf->pfSKF_VerifyPIN(hApp,USER_TYPE,"123456",&ulRetryCount);
    if(rv)
    {
        PrintError("SKF_VerifyPIN",rv);
        return rv;
    }

    //    memcpy(szContainer,"1ea7c63c-93b3-4bee-92c7-c21ff0e14e86",37);
    memcpy(szContainer,"SM2_Container",13);
    //        rv = SKF_DeleteContainer(hApp,szContainer);
    //        if(rv)
    //        {
    //            PrintError("SKF_DeleteContainer",rv);
    //        }
    rv = pf_skf->pfSKF_OpenContainer(hApp,szContainer,&hCon);
    if(rv)
    {
        if(SAR_FAIL == rv)
        {
            rv = pf_skf->pfSKF_CreateContainer(hApp,szContainer,&hCon);
            if(rv)
            {
                PrintError("SKF_CreateContainer",rv);
                return rv;
            }
        }else{
            PrintError("SKF_OpenContainer",rv);
            return rv;
        }

        rv = pf_skf->pfSKF_GenECCKeyPair(hCon,SGD_SM2_1,&pEccSignKey);  //生成签名密钥并输出公钥
        if(rv)
        {
            PrintError("SKF_GenECCKeyPair",rv);
            return rv;
        }

        rv = pf_skf->pfSKF_GenECCKeyPair(hCon,SGD_SM2_3,&pEccEncKey);  //生成加密密钥并输出公钥
        if(rv)
        {
            PrintError("SKF_GenECCKeyPair",rv);
            return rv;
        }
    }
    else
    {
        ULONG type; //为1表示为RSA容器，为2表示为ECC容器
        pf_skf->pfSKF_GetContainerType(hCon,&type);   //获取类型

        //数字签名
        ULONG ulEccPubKeyLen = sizeof(ECCPUBLICKEYBLOB);
        rv = pf_skf->pfSKF_ExportPublicKey(hCon,TRUE,(unsigned char *)&pEccSignKey,&ulEccPubKeyLen); // TRUE表示导出签名公钥，FALSE表示导出加密公钥
        if(rv)
        {
            PrintError("SKF_ExportPublicKey",rv);
            return rv;
        }

        rv = pf_skf->pfSKF_ExportPublicKey(hCon,FALSE,(unsigned char *)&pEccEncKey,&ulEccPubKeyLen);
        if(rv)
        {
            PrintError("SKF_ExportPublicKey",rv);
            return rv;
        }
    }
    {
        unsigned char pbData[1024]={0},pbDigest[32]={0},pbSignData[128]={0};
        ULONG ulDataLen = 1024,ulDigestLen = 32;
        ULONG i =0;
        for(i =0;i<1024;i++)
        {
            pbData[i] = (unsigned char )((i*4+3)%256);
        }
        printf("begin the Digest of the Data is :\n");
        for(i=0;i<ulDigestLen;i++)
        {
            printf("0x%02x ",pbData[i]);
        }
        HANDLE hHash;
        unsigned char userId[32]={0};
        ULONG ulUserIdLen = 0;
        memcpy(userId,"xcs",3);

        ulUserIdLen = 3;

        rv = pf_skf->pfSKF_DigestInit(hDev,SGD_SM3,&pEccSignKey,userId,ulUserIdLen,&hHash);
        if(rv)
        {
            PrintError("SKF_DigestInit",rv);
            return rv;
        }

        rv = pf_skf->pfSKF_Digest(hHash,pbData,ulDataLen,pbDigest,&ulDigestLen);
        if(rv)
        {
            PrintError("SKF_Digest",rv);
            return rv;
        }
        printf("the Digest of the Data is :\n");
        for(i=0;i<ulDigestLen;i++)
        {
            printf("0x%02x ",pbDigest[i]);
        }

        printf("\n");

        ECCSIGNATUREBLOB EccSignBlob;

        rv = pf_skf->pfSKF_ECCSignData(hCon,pbDigest,ulDigestLen,&EccSignBlob);
        if(rv)
        {
            PrintError("SKF_ECCSignData",rv);
            return rv;
        }
        memcpy(pbSignData,EccSignBlob.r+32,32);
        memcpy(pbSignData+32,EccSignBlob.s+32,32);
        printf("the signValue of the Data is :\n");
        for(i=0;i<64;i++)
        {
            printf("0x%02x ",pbSignData[i]);
        }

        printf("\n");

        rv = pf_skf->pfSKF_ECCVerify(hDev,&pEccSignKey,pbDigest,ulDigestLen,&EccSignBlob);
        if(rv)
        {
            PrintError("SKF_RSAVerify",rv);
            return rv;
        }
        printf("SM2 verify SignValue is succeed!\n");
    }


    //导出证书
    ULONG certlen;
    rv = pf_skf->pfSKF_ExportCertificate(hCon,TRUE,NULL,&certlen);
    if(rv)
    {
        PrintError("SKF_ExportCertificate",rv);
        return rv;
    }
    unsigned char *pbsinCert = malloc(certlen);
    rv = pf_skf->pfSKF_ExportCertificate(hCon,TRUE,pbsinCert,&certlen);
    if(rv)
    {
        PrintError("SKF_ExportCertificate",rv);
        return rv;
    }

    X509 *cert = readBinaryCertificate(pbsinCert,certlen);
    if (cert == NULL) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // 将证书写入 PEM 格式文件
    const char *outputFilename = "my_sinCertificate.pem";
    if (!writeCertificateToPEMFile(cert, outputFilename)) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    X509_free(cert);
    rv = pf_skf->pfSKF_ExportCertificate(hCon,FALSE,NULL,&certlen);
    if(rv)
    {
        PrintError("SKF_ExportCertificate",rv);
        return rv;
    }
    unsigned char *pbencCert = malloc(certlen);
    rv = pf_skf->pfSKF_ExportCertificate(hCon,FALSE,pbencCert,&certlen);
    if(rv)
    {
        PrintError("SKF_ExportCertificate",rv);
        return rv;
    }
    cert = readBinaryCertificate(pbencCert,certlen);
    if (cert == NULL) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // 将证书写入 PEM 格式文件
    outputFilename = "my_encCertificate.pem";
    if (!writeCertificateToPEMFile(cert, outputFilename)) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    X509_free(cert);

    //    rv = pf_skf->pfSKF_DeleteContainer(hApp,szContainer);
    //    if(rv)
    //    {
    //        PrintError("SKF_DeleteContainer",rv);
    //    }




    return SAR_OK;
}

ULONG SM2KeyPairTest(DEVHANDLE hDev,HAPPLICATION hApp)
{

    ULONG rv = SAR_OK,ulRetryCount =0;
    HCONTAINER hCon;
    char szContainer[64]={0};
    CHAR *errmsg = NULL;
    SKF_API *pf_skf = GetSKFAPI(&errmsg);
    if (errmsg != NULL)
    {
        DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[wait For Device Event][Get SKFAPI error:%s]", errmsg);
        free(errmsg);
        return NULL;
    }
    rv = pf_skf->pfSKF_VerifyPIN(hApp,USER_TYPE,"123456",&ulRetryCount);
    if(rv)
    {
        PrintError("SKF_VerifyPIN",rv);
        return rv;
    }

    memcpy(szContainer,"SM2_Container",13);

    rv = pf_skf->pfSKF_CreateContainer(hApp,szContainer,&hCon);
    if(rv)
    {
        PrintError("SKF_CreateContainer",rv);
        return rv;

    }

    ECCPUBLICKEYBLOB EccPubKey;

    rv = pf_skf->pfSKF_GenECCKeyPair(hCon,SGD_SM2_1,&EccPubKey);
    if(rv)
    {
        PrintError("SKF_GenECCKeyPair",rv);
        return rv;
    }

    unsigned char pbData[1024]={0},pbDigest[32]={0},pbSignData[128]={0};
    ULONG ulDataLen = 1024,ulDigestLen = 32,ulSignDataLen = 128;
    ULONG i =0;
    for(i =0;i<1024;i++)
    {
        pbData[i] = (unsigned char )((i*4+3)%256);
    }

    HANDLE hHash;
    unsigned char userId[32]={0};
    ULONG ulUserIdLen = 0;
    memcpy(userId,"feitian",7);

    ulUserIdLen = 7;

    rv = pf_skf->pfSKF_DigestInit(hDev,SGD_SM3,&EccPubKey,userId,ulUserIdLen,&hHash);
    if(rv)
    {
        PrintError("SKF_DigestInit",rv);
        return rv;
    }

    rv = pf_skf->pfSKF_Digest(hHash,pbData,ulDataLen,pbDigest,&ulDigestLen);
    if(rv)
    {
        PrintError("SKF_Digest",rv);
        return rv;
    }

    printf("the Digest of the Data is :\n");
    for(i=0;i<ulDigestLen;i++)
    {
        printf("0x%02x ",pbDigest[i]);
    }

    printf("\n");

    ECCSIGNATUREBLOB EccSignBlob;

    rv = pf_skf->pfSKF_ECCSignData(hCon,pbDigest,ulDigestLen,&EccSignBlob);
    if(rv)
    {
        PrintError("SKF_ECCSignData",rv);
        return rv;
    }
    memcpy(pbSignData,EccSignBlob.r+32,32);
    memcpy(pbSignData+32,EccSignBlob.s+32,32);
    printf("the signValue of the Data is :\n");
    for(i=0;i<64;i++)
    {
        printf("0x%02x ",pbSignData[i]);
    }

    printf("\n");

    rv = pf_skf->pfSKF_ECCVerify(hDev,&EccPubKey,pbDigest,ulDigestLen,&EccSignBlob);
    if(rv)
    {
        PrintError("SKF_RSAVerify",rv);
        return rv;
    }
    printf("SM2 verify SignValue is succeed!\n");

    return SAR_OK;

}

ULONG ImportSM2KeyPair_Test(DEVHANDLE hDev,HAPPLICATION hApp)
{
    ULONG rv,rLen;
    ECCPUBLICKEYBLOB pEccSignKey;
    ULONG ulEccPubKeyLen = sizeof(ECCPUBLICKEYBLOB);
    ECCCIPHERBLOB  *pEccCipherBlob=NULL;
    HANDLE hSessionKey;

    PENVELOPEDKEYBLOB pEnvelopedKeyBlob;
    unsigned char pbWrappedKey[32]={0},pbTmpData[1024]={0},pbEncryptedData[1024]={0},pbData[1024]={0};
    ULONG ulWrappedKeyLen=32,ulTmpDataLen=1024,ulEncryptedDataLen=1024;
    BLOCKCIPHERPARAM EncryptParam;
    int offset=0;


    ECCPRIVATEKEYBLOB pEccPriBlb = { 256,{ \
                                         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \
                                         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \
                                         0x40,0x00,0x14,0x24,0x83,0x02,0x14,0x20,0x42,0x88,0x02,0x4A,0x10,0x14,0x80,0x00,\
                                         0x02,0x1C,0x00,0x09,0x83,0x58,0x21,0xAC,0x80,0x00,0xA0,0x13,0x11,0x00,0xA8,0x59
                                     }};

    ECCPUBLICKEYBLOB pEccPubBlob = {256,{ \
                                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \
                                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \
                                        0x26,0xEA,0x8A,0x39,0x30,0x20,0x8E,0xFD,0x91,0x32,0xF7,0x1C,0x51,0x0A,0xAB,0x57, \
                                        0x43,0x8B,0x3D,0xBC,0x27,0xD3,0x04,0xE7,0x98,0xEC,0xCA,0xF2,0xA0,0xEA,0x74,0xEB \
                                    }, \
                                    { \
                                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \
                                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \
                                        0x75,0x00,0xD9,0xCF,0xF3,0x0E,0x63,0x10,0x15,0xC7,0x73,0x72,0x8E,0x8C,0x25,0x09, \
                                        0x38,0x0A,0x22,0xE1,0xE7,0x42,0xB6,0xAB,0xA0,0x9D,0xCF,0x85,0x7C,0x42,0xCC,0xEA \
                                    }};

    char szContainer[64]={0};
    CHAR *errmsg = NULL;
    SKF_API *pf_skf = GetSKFAPI(&errmsg);
    if (errmsg != NULL)
    {
        DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[wait For Device Event][Get SKFAPI error:%s]", errmsg);
        free(errmsg);
        return NULL;
    }

    HCONTAINER hCon;

    memcpy(szContainer,"SM2_Container",13);

    rv = pf_skf->pfSKF_OpenContainer(hApp,szContainer,&hCon);
    if(rv)
    {
        PrintError("SKF_OpenContainer",rv);
        return rv;
    }

    rv = pf_skf->pfSKF_ExportPublicKey(hCon,TRUE,(unsigned char *)&pEccSignKey,&ulEccPubKeyLen);
    if(rv)
    {
        PrintError("SKF_ExportPublicKey",rv);
        return rv;
    }


    pEccCipherBlob = (ECCCIPHERBLOB *)malloc(sizeof(ECCCIPHERBLOB)+16-1);
    pEccCipherBlob->CipherLen =16;
    rv = pf_skf->pfSKF_ECCExportSessionKey(hCon,SGD_SM1_ECB,&pEccSignKey,pEccCipherBlob,&hSessionKey);
    if(rv != SAR_OK)
    {
        PrintError("SKF_ExportPublicKey",rv);
        return rv;
    }

    memcpy(pbTmpData,(char *)&pEccPriBlb.PrivateKey,pEccPriBlb.BitLen/4);
    ulTmpDataLen = pEccPriBlb.BitLen/4;

    EncryptParam.IVLen = 0;
    EncryptParam.PaddingType = 0;
    rv = pf_skf->pfSKF_EncryptInit(hSessionKey,EncryptParam);
    if(rv != SAR_OK)
    {
        PrintError("SKF_EncryptInit",rv);
        return rv;
    }

    rv = pf_skf->pfSKF_EncryptUpdate(hSessionKey,pbTmpData,ulTmpDataLen,pbEncryptedData,&ulEncryptedDataLen);
    if(rv != SAR_OK)
    {
        PrintError("SKF_EncryptUpdate",rv);
        return rv;
    }
    rv = pf_skf->pfSKF_EncryptFinal(hSessionKey,pbEncryptedData+ulEncryptedDataLen,&rLen);
    if(rv != SAR_OK)
    {
        PrintError("SKF_EncryptFinal",rv);
        return rv;
    }

    ulEncryptedDataLen += rLen;

    pEnvelopedKeyBlob = (PENVELOPEDKEYBLOB)malloc(sizeof(ENVELOPEDKEYBLOB)+16-1);
    if(pEccCipherBlob == NULL)
    {
        printf("申请内存失败!");
        return -1;

    }

    pEnvelopedKeyBlob->Version = 1;
    pEnvelopedKeyBlob->ulSymmAlgID = SGD_SM1_ECB;
    pEnvelopedKeyBlob->ulBits = 256;
    pEnvelopedKeyBlob->PubKey = pEccPubBlob;

    memset(pbEncryptedData,0x00,32);
    memcpy((char *)&(pEnvelopedKeyBlob->ECCCipherBlob),pEccCipherBlob,sizeof(ECCCIPHERBLOB)+16-1);
    memcpy((char *)&(pEnvelopedKeyBlob->cbEncryptedPriKey),&pbEncryptedData,ulEncryptedDataLen);


    rv = pf_skf->pfSKF_ImportECCKeyPair(hCon,pEnvelopedKeyBlob);
    if(rv != SAR_OK)
    {
        PrintError("SKF_ImportECCKeyPair",rv);
        return rv;
    }
    if(pEnvelopedKeyBlob)
    {
        free(pEnvelopedKeyBlob);
    }
    if(pEccCipherBlob)
    {
        free(pEccCipherBlob);
    }

    printf("Import SM2 KeyPiar succeed!\n");

    return SAR_OK;
}

ULONG ImportSessionKey_Test(DEVHANDLE hDev,HAPPLICATION hApp)
{
    ULONG rv =0;
    unsigned char *pbBlob = NULL,*pCipherData = NULL;
    ULONG ulBlobLen = 0;
    HANDLE hSessionKey;

    char szContainer[64]={0};
    HCONTAINER hCon;
    CHAR *errmsg = NULL;
    SKF_API *pf_skf = GetSKFAPI(&errmsg);
    if (errmsg != NULL)
    {
        DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[wait For Device Event][Get SKFAPI error:%s]", errmsg);
        free(errmsg);
        return NULL;
    }

    ///使用固定的SM2容器  可以枚举枚举容器，获取容器类型
    memcpy(szContainer,"SM2_Container",13);
    rv = pf_skf->pfSKF_OpenContainer(hApp,szContainer,&hCon);
    if(rv)
    {
        PrintError("SKF_OpenContainer",rv);
        return rv;
    }

    ///因为是SM2，所以预先知道公钥大小，，可以调用两次SKF_ExportPublicKey，第一次获取长度
    pbBlob = (unsigned char *)malloc(sizeof(ECCPUBLICKEYBLOB));
    ulBlobLen = sizeof(ECCPUBLICKEYBLOB);


    /////注意，必须导出加密密钥对的公钥
    rv = pf_skf->pfSKF_ExportPublicKey(hCon,FALSE,pbBlob,&ulBlobLen);
    if(rv)
    {
        printf("SKF_ExportPublicKey failed,rv = 0x%0lx",rv);
        if(pbBlob)
        {
            free(pbBlob);
            pbBlob = NULL;
        }
        return rv;
    }

    ECCCIPHERBLOB *pEccCipher= NULL;
    pEccCipher = (ECCCIPHERBLOB *)malloc(sizeof(ECCCIPHERBLOB)+16-1);
    memset((char *)pEccCipher,0x00,sizeof(ECCCIPHERBLOB)+16-1);

    pEccCipher->CipherLen = 16;   ///必须设定

    rv = pf_skf->pfSKF_ECCExportSessionKey(hCon,SGD_SM1_ECB,(ECCPUBLICKEYBLOB *)pbBlob,pEccCipher,&hSessionKey);
    if(rv)
    {
        printf("SKF_ECCExportSessionKey failed,rv = 0x%0lx",rv);
        if(pbBlob)
        {
            free(pbBlob);
            pbBlob = NULL;
        }
        if(pEccCipher)
        {
            free(pEccCipher);
            pEccCipher = NULL;
        }
        return rv;
    }

    rv = pf_skf->pfSKF_ImportSessionKey(hCon,SGD_SM1_ECB,(unsigned char *)pEccCipher,sizeof(ECCCIPHERBLOB)+16-1,&hSessionKey);
    if(rv)
    {
        printf("SKF_ImportSessionKey failed,rv = 0x%0lx",rv);
        if(pbBlob)
        {
            free(pbBlob);
            pbBlob = NULL;
        }
        if(pEccCipher)
        {
            free(pEccCipher);
            pEccCipher = NULL;
        }
        return rv;
    }

    if(pbBlob)
        free(pbBlob);
    if(pEccCipher)
        free(pCipherData);

    printf("ImportSessionKey succeed!\n");
    return SAR_OK;


}

// 获取所有设备句柄的函数
ULONG GetDevHandles(DEVHANDLE **phDev,ULONG *oSize)
{
    ULONG rv=0;
    ULONG size = 0;
    char *pbDevList= 0;
    ULONG ulDevListLen = 0;
    CHAR *errmsg = NULL;
    SKF_API *pf_skf = GetSKFAPI(&errmsg);
    if (errmsg != NULL)
    {
        DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[wait For Device Event][Get SKFAPI error:%s]", errmsg);
        free(errmsg);
        return NULL;
    }

    rv = pf_skf->pfSKF_EnumDev(1,pbDevList,&ulDevListLen);
    if(rv != SAR_OK)
    {
        PrintError("SKF_EnumDev",rv);
        return rv;
    }

    if(ulDevListLen <2)
    {
        printf("No Device!\n");
        return -1;
    }

    pbDevList = (char *)malloc(ulDevListLen);
    if(pbDevList == NULL)
    {
        printf("Memory Error!");
        return -1;
    }
    rv = pf_skf->pfSKF_EnumDev(1,pbDevList,&ulDevListLen);
    if(rv != SAR_OK)
    {
        PrintError("SKF_EnumDev",rv);
        if(pbDevList)
        {
            free(pbDevList);
            pbDevList = NULL;
        }
        return rv;
    }

    char *pp = pbDevList;
    while(pbDevList+ulDevListLen - pp)
    {
        if(strlen(pp))
        {
            size++;
            printf("find Device %s\n",pp);
            pp+=strlen(pp);
        }
        else
        {
            pp++;
        }
    }

    pp = 0;
    *phDev = (DEVHANDLE *)malloc(size * sizeof(DEVHANDLE));
    if (*phDev == NULL)
    {
        printf("Memory Error!");
        free(pbDevList);
        return -1;
    }

    pp = pbDevList;
    ULONG i = 0;
    while(pbDevList+ulDevListLen - pp)
    {
        if(strlen(pp))
        {
            DEVHANDLE hDev=NULL;
            rv = pf_skf->pfSKF_ConnectDev(pp,&hDev);
            if(rv)
            {
                PrintError("SKF_ConnectDev",rv);
                free(pbDevList);
                pbDevList = NULL;
                return rv;
            }
            printf("Connect Device %s\n",pp);
            (*phDev)[i++] = hDev;
            pp+=strlen(pp);
        }
        else
        {
            pp++;
        }
    }
    *oSize = i;
    if(pbDevList)
        free(pbDevList);

    return SAR_OK;

}

char* get_field_from_name(X509_NAME* name, int nid) {
    if (!name) {
        return NULL;
    }

    int idx = X509_NAME_get_index_by_NID(name, nid, -1);
    if (idx < 0) {
        return NULL;
    }

    X509_NAME_ENTRY* entry = X509_NAME_get_entry(name, idx);
    if (!entry) {
        return NULL;
    }

    ASN1_STRING* asn1_str = X509_NAME_ENTRY_get_data(entry);
    if (!asn1_str) {
        return NULL;
    }

    return (char*)ASN1_STRING_get0_data(asn1_str);
}

// General function to get a field from either the subject or issuer of a certificate
char* get_field_from_cert(X509* cert, int nid, int is_issuer) {
    X509_NAME* name = is_issuer ? X509_get_issuer_name(cert) : X509_get_subject_name(cert);
    return get_field_from_name(name, nid);
}

// 导出证书并返回X509对象
X509* GetCertificateToX509(HCONTAINER hContainer, BOOL bSignFlag) {
    ULONG retval;
    ULONG pulCertLen = 0;
    BYTE *pbCert = NULL;
    X509 *x509_cert = NULL;
    CHAR *errmsg = NULL;
    SKF_API *pf_skf = GetSKFAPI(&errmsg);
    if (errmsg != NULL)
    {
        DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[wait For Device Event][Get SKFAPI error:%s]", errmsg);
        free(errmsg);
        return NULL;
    }
    // 获取证书长度
    if (SAR_OK != (retval = pf_skf->pfSKF_ExportCertificate(hContainer, bSignFlag, NULL, &pulCertLen))) {
        printf("SKF_ExportCertificate error 0x%08lX\n", retval);
        return NULL;
    }

    // 分配内存并导出证书
    if (0 != pulCertLen && NULL == (pbCert = OPENSSL_malloc(pulCertLen))) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    if (SAR_OK != (retval = pf_skf->pfSKF_ExportCertificate(hContainer, bSignFlag, pbCert, &pulCertLen))) {
        printf("SKF_ExportCertificate error 0x%08lX\n", retval);
        OPENSSL_free(pbCert);
        return NULL;
    }

    // 将导出的证书转换为X509对象
    const unsigned char *p = pbCert;
    x509_cert = d2i_X509(NULL, &p, pulCertLen);

    if (x509_cert == NULL) {
        printf("Failed to convert certificate to X509 object.\n");
        OPENSSL_free(pbCert);
        return NULL;
    }

    // 清理
    OPENSSL_free(pbCert);

    return x509_cert;
}

int main()
{
#if defined(_UNICODE) || defined(UNICODE)
    printf("This project is built as Unicode.\n");
#else
    printf("This project is built as non-Unicode.\n");
#endif
    DEVHANDLE hDev = NULL;
    HAPPLICATION hApp = NULL;
    ULONG rv;
    DEVHANDLE *hDevs = NULL;
    ULONG devsize;
    rv = GetDevHandles(&hDevs,&devsize);
    if(rv)
    {
        return 0;
    }
    for(ULONG i= 0; i < devsize; i++)
    {
        HAPPLICATION *phApps = NULL;
        ULONG appsize;
        rv = GetAppHandles(hDevs[i],&phApps,&appsize);
        if(rv)
        {
            continue;
        }
        for(ULONG j= 0; j < appsize; j++)
        {
            HCONTAINER *hCons = NULL;
            ULONG numContainers = 0;
            GetContainerHandles(phApps[j],&hCons,&numContainers);
            for (ULONG k = 0; k < numContainers; ++k)
            {
                X509 *x = GetCertificateToX509(hCons[k],TRUE);
                if (x==NULL)
                {
                    ERR_print_errors_fp(stderr);
                    continue;
                }
                char* cn = get_field_from_cert(x, NID_commonName, 0);
                printf("cert cn name=%s\n", cn);
            }
        }
    }
    rv = KeyPairTest(hDev,hApp);
    if(rv)
    {
        return 0;
    }
    rv = ImportSM2KeyPair_Test(hDev,hApp);
    if(rv)
    {
        return 0;
    }

    rv = ImportSessionKey_Test(hDev,hApp);
    if(rv)
    {
        return 0;
    }
    CHAR *errmsg = NULL;
    SKF_API *pf_skf = GetSKFAPI(&errmsg);
    if (errmsg != NULL)
    {
        DDM_Log(DDM_LOG_CERT, DDM_LOG_LEVEL_ERROR, "[wait For Device Event][Get SKFAPI error:%s]", errmsg);
        free(errmsg);
        return NULL;
    }
    rv = pf_skf->pfSKF_CloseApplication(hApp);
    if(rv)
    {
        return 0;
    }

    rv = pf_skf->pfSKF_DisConnectDev(hDev);
    if(rv)
    {
        return 0;
    }

    printf("Hello World!\n");
    return 0;
}
