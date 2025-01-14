# 



```c
#include <SKFAPI.h>
#include <windows.h>

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

```