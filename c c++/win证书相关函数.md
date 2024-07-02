``` c
#include <Windows.h>
#include <Wincrypt.h>
#include <stdio.h>

int main()
{
    DWORD dwIndex = 0;
    DWORD dwFlags = 0;
    DWORD dwProvType;
    WCHAR szProvName[1024];
    DWORD cbProvName = sizeof(szProvName);

    while (CryptEnumProvidersW(dwIndex++, NULL, dwFlags, &dwProvType, szProvName, &cbProvName))
    {
        printf("Provider Type: %lu, Provider Name: %ws\n", dwProvType, szProvName);
        cbProvName = sizeof(szProvName);
    }

    return 0;
}
```


``` c

//完整的例子
#include <Windows.h>
#include <bcrypt.h>
#include <ncrypt.h>
#include <stdio.h>

void printCryptKeyProvInfo(PCRYPT_KEY_PROV_INFO pInfo)
{
    printf("Provider Name: %S\n", pInfo->pwszProvName); //%S输出双字节文字
    printf("Provider Type: %lu\n", pInfo->dwProvType);
    printf("Flags: 0x%lx\n", pInfo->dwFlags);
    printf("Container Name: %S\n", pInfo->pwszContainerName);
    printf("Key Spec: %lu\n", pInfo->dwKeySpec);
}

int main(int argc,char *argv[])
{
    LPWSTR CertName;
    if(argc < 2)
    {
        printf("not cert name\n");
        return 1;
    }
    else
    {
        DWORD len = MultiByteToWideChar(CP_ACP, 0, argv[1], -1, NULL, 0);   //MultiByteToWideChar函数将ANSI字符串转换为Unicode字符串，WideCharToMultiByte函数将Unicode字符串转换为ANSI字符串
        CertName = (LPWSTR)LocalAlloc(LPTR, len);
        MultiByteToWideChar(CP_ACP, 0, argv[1], -1, CertName, len);
    }
    PCRYPT_KEY_PROV_INFO data;
    HCERTSTORE hCertStore;
    PCCERT_CONTEXT ctx1,ctx2 = NULL;
    NCRYPT_PROV_HANDLE hProv;
    DWORD datalen;
    HCRYPTHASH hHash;
    DWORD dwKeySpec;
    LPWSTR pszName = NULL;
    ALG_ID hashid = CALG_SHA1;
    // 打开个人证书存储
    hCertStore = CertOpenSystemStore(0, L"MY");
    if (hCertStore == NULL)
    {
        printf("Failed to open the personal certificate store.\n");
        return 1;
    }
BEGIN:
    ctx1 = CertEnumCertificatesInStore(hCertStore, ctx2);       // ctx2为NULL时是获取第一个证书，否则是获取下一本证书
    if (ctx1 == NULL)
    {
        printf("No certificate found.\n");
        CertCloseStore(hCertStore, 0);      
        return 1;
    }

    DWORD  cchName = CertGetNameString(ctx1, CERT_NAME_FRIENDLY_DISPLAY_TYPE, CERT_NAME_STR_COMMA_FLAG|CERT_NAME_STR_REVERSE_FLAG, NULL, NULL, 0);       //最后第二个参数为NULL,最后一个参数为0 为获取证书显示名称的长度
       if (cchName > 0)
       {
           //为显示名称分配内存
           pszName = (LPWSTR)LocalAlloc(LPTR, cchName);
           if (pszName != NULL)
           {
               //获取证书显示名称
               if (CertGetNameString(ctx1, CERT_NAME_FRIENDLY_DISPLAY_TYPE, CERT_NAME_STR_COMMA_FLAG|CERT_NAME_STR_REVERSE_FLAG, NULL, pszName, cchName) > 0)
               {
                   //打印证书显示名称
                   printf("Certificate name: %S\n", pszName);
                   if(wcscmp(CertName,pszName) != 0)            //宽字符对比
                   {
                       ctx2 = ctx1;
                       goto BEGIN;
                   }
               }
               //释放内存
               LocalFree(pszName);
           }
       }

    BOOL b = CertGetCertificateContextProperty(ctx1, CERT_KEY_PROV_INFO_PROP_ID, NULL, &datalen);//取证书的密钥容器信息 CERT_KEY_PROV_INFO_PROP_ID 属性包含与证书关联的密钥容器的名称和提供程序类型 倒数第二个参数为NULL时为获取结构大小
    if (b && (datalen != 0))
    {
        data = (PCRYPT_KEY_PROV_INFO)malloc(datalen);
        if (data == NULL)
        {
            return 0;
        }
        b = CertGetCertificateContextProperty(ctx1, CERT_KEY_PROV_INFO_PROP_ID, data, &datalen);
        if (b)
        {
            printCryptKeyProvInfo(data);
            if(data->dwProvType == 0)       //找不到证书关联的加密服务提供程序
            {
                b = CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV_W, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET);
                dwKeySpec = 1;
            }
            else
            {
                b = CryptAcquireContext(&hProv, data->pwszContainerName, data->pwszProvName, data->dwProvType, data->dwFlags);      //获取可用的加密服务提供程序的加密上下文句柄
                dwKeySpec = data->dwKeySpec;
            }

            free(data);
            if (0 == b)
            {
                printf("[capi rsa sign CryptAcquireContextW failed, err:%lu]\n",GetLastError());
                return 0;
            }

            b = CryptCreateHash(hProv, hashid, 0, 0, &hHash);       //创建hash对象
            if (b == 0)
            {
                (VOID)CryptReleaseContext(hProv, 0);
                return 0;
            }
            const unsigned char *m = "1111111111111111111111";
            b = CryptSetHashParam(hHash, HP_HASHVAL, m, 0);     //HP_HASHVAL设置哈希对象的哈希值，用于将数据附加到哈希对象中，并为哈希对象计算消息摘要。
            if (b == 0)
            {
                (VOID)CryptDestroyHash(hHash);
                (VOID)CryptReleaseContext(hProv, 0);
                return 0;
            }
            unsigned char sigret[1024];
            unsigned int siglen = 1024;
            b = CryptSignHash(hHash, dwKeySpec, 0, 0 , sigret, &siglen);        //哈希值进行数字签名
            (VOID)CryptDestroyHash(hHash);
            (VOID)CryptReleaseContext(hProv, 0);
            if (b == 0)
            {
                return 0;
            }
        }
        else
        {
            free(data);
        }
    }
    return 0;
}

```
