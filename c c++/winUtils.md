# 获取本地网卡mac
``` c++
#include <iphlpapi.h>
#pragma comment(lib,"iphlpapi.lib")
bool Tools::GetSocketMac(std::string& macOUT)
{
	setlocale(LC_ALL, "chs");
	bool ret = false;
	ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
	PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	if (pAddresses == NULL)
		return false;
	// Make an initial call to GetAdaptersAddresses to get the necessary size into the ulOutBufLen variable
	if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
		if (pAddresses == NULL)
			return false;
	}

	if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == NO_ERROR)
	{
		// If successful, output some information from the data we received
		for (PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
		{
			// 确保MAC地址的长度为 00-00-00-00-00-00
			if (pCurrAddresses->PhysicalAddressLength != 6)
				continue;
			if (wcscmp(pCurrAddresses->FriendlyName, L"WLAN") == 0)
			{
				char acMAC[32];
				sprintf(acMAC, "%02X-%02X-%02X-%02X-%02X-%02X",
					int(pCurrAddresses->PhysicalAddress[0]),
					int(pCurrAddresses->PhysicalAddress[1]),
					int(pCurrAddresses->PhysicalAddress[2]),
					int(pCurrAddresses->PhysicalAddress[3]),
					int(pCurrAddresses->PhysicalAddress[4]),
					int(pCurrAddresses->PhysicalAddress[5]));
				macOUT = acMAC;
				ret = true;
				
			}	
			else
			{
				wprintf(L"name:%s\n", pCurrAddresses->FriendlyName);
			}
		}
	}
	free(pAddresses);
	return ret;
}
```