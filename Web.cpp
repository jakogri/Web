
#include <stdio.h>
#include <Windows.h>
#include <winternl.h>
#include <atlbase.h>
#include <msxml6.h>
#include <comutil.h> 
#include <winhttp.h>
#include <list>
#include <string>
#include <variant>
#include <time.h>
#pragma comment(lib, "winhttp.lib")
typedef struct _SYSTEM_PROCESS_INFO
{
    ULONG                   NextEntryOffset;
    ULONG                   NumberOfThreads;
    LARGE_INTEGER           Reserved[3];
    LARGE_INTEGER           CreateTime;
    LARGE_INTEGER           UserTime;
    LARGE_INTEGER           KernelTime;
    UNICODE_STRING          ImageName;
    ULONG                   BasePriority;
    HANDLE                  ProcessId;
    HANDLE                  InheritedFromProcessId;
}SYSTEM_PROCESS_INFO, * PSYSTEM_PROCESS_INFO;
std::list<std::string>  myDynamicArray;

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

int get_services()
{
    PVOID buffer;
    char* buffer_str;
    PSYSTEM_PROCESS_INFO spi;

    buffer = VirtualAlloc(NULL, 1024 * 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!buffer) return -1;
   
    buffer_str = (char*)malloc(1024);
    if (!buffer_str)
    {
        if (buffer_str) free(buffer_str);
        return -1;
    }
    spi = (PSYSTEM_PROCESS_INFO)buffer;
    myDynamicArray.push_back(currentDateTime());
    while (spi->NextEntryOffset) 
     {
       sprintf(buffer_str, "\nProcess name: %ws | Process ID: %d\n", spi->ImageName.Buffer, (int)(spi->ProcessId));
       myDynamicArray.push_back(buffer_str);
       spi = (PSYSTEM_PROCESS_INFO)((LPBYTE)spi + spi->NextEntryOffset); 
      
     }
  if (buffer_str) free(buffer_str);
  if (buffer) free(buffer);
  return 0;
}



int main()
{
    myDynamicArray.clear();
    if (get_services() < 0) return -1;
    LPVOID ptr = &(myDynamicArray);
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    BOOL  requestResult = FALSE,
          resposeResult = false;
    HINTERNET  hSession = NULL,
        hConnect = NULL,
        hRequest = NULL;

    hSession = WinHttpOpen(L"WinHTTP Example/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

     if (hSession)
        hConnect = WinHttpConnect(hSession, L"www.microsoft.com",
            INTERNET_DEFAULT_HTTPS_PORT, 0);

     if (hConnect)
        hRequest = WinHttpOpenRequest(hConnect, L"GET", NULL,
            NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE);

   
    if (hRequest)
    { 
        while (!resposeResult)
        {
            requestResult = WinHttpSendRequest(hRequest,
                WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                ptr, 0,
                0, 0);
            if (requestResult)
                resposeResult = WinHttpReceiveResponse(hRequest, NULL);
        }
    }
}
    


 