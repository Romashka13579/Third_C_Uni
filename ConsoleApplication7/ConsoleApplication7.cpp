
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#include <iostream>


int IdDivision(int AIS_ID) {

    int sum = 0;
    int remain = AIS_ID % 10;
    AIS_ID = (AIS_ID - remain)/10;
    int fifthnum = AIS_ID % 10;
    for (int i = 0; i < 5; i++) {
        int remain = AIS_ID % 10;
        AIS_ID = (AIS_ID - remain) / 10;
        sum += remain;
    }
    int result = sum % fifthnum;

    return result;
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    //uvodne nastavovacky

    WSADATA wsaData;    //struktura WSADATA pre pracu s Winsock
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);     //zakladna inicializacia
    if (iResult != 0)     //kontrola, ci nestala chyba
    {
        printf("WSAStartup failed : % d\n", iResult);
        return 1;
    }

    struct addrinfo* result = NULL, * ptr = NULL;     //struktura pre pracu s adresami
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;     //pracujeme s protokolom TCP/IP

    // Resolve the server address and port
    iResult = getaddrinfo("147.175.115.34", "777", &hints, &result);
    if (iResult != 0)     //kontrola, ci nenastala chyba
    {
        printf("getaddrinfo failed : % d\n", iResult);
        WSACleanup();
        return 1;
    }
    else
        printf("getaddrinfo didn’t fail…\n");

    //vytvorenie socketu a pripojenie sa

    SOCKET ConnectSocket = INVALID_SOCKET;

    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr = result;

    // Create a SOCKET for connecting to server => pokus o vytvorenie socketu
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET)     //kontrola, ci nenastala chyba 
    {
        printf("Error at socket() : % ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    else
        printf("Error at socket DIDN’T occur…\n");

    // Connect to server. => pokus o pripojenie sa na server

    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR)     //kontrola, ci nenastala chyba
        printf("Not connected to server…\n");
    else
        printf("Connected to server!\n");

    if (iResult == SOCKET_ERROR)    //osetrenie chyboveho stavu
    {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        WSACleanup();
        return 1;
    }

    Sleep(250);





    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    int i = 0;

    while (true) {
        //posielanie

        if (i >= 4) {
            SetConsoleTextAttribute(hConsole, 4);
        }

        char sendbuf[4096]; //buffer (v zasade retazec), kam sa budu ukladat data, ktore chcete posielat

        if (i == 5) {
            sendbuf[0] = IdDivision(127858) + '0';
            printf("%c\n", sendbuf[0]);
            i++;
        }
        else {
            fgets(sendbuf, 4096, stdin);
        }

        iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed : %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
        if (i >= 4) {
            SetConsoleTextAttribute(hConsole, 5);
        }

        printf("Bytes Sent : %ld\n", iResult);     //vypisanie poctu odoslanych dat



        //prijimanie

        #define DEFAULT_BUFLEN 4096 //makro, kde definujeme velkost prijimacieho buffera

        int recvbuflen = DEFAULT_BUFLEN;
        char recvbuf[DEFAULT_BUFLEN];

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);     //funkcia na príjimanie

        if (iResult > 0)
        {
            printf("Bytes received : % d\n", iResult);     //prisli validne data, vypis poctu

            recvbuf[iResult] = '\0'; // Add null-terminator to make it a valid C-string

            if (!(strstr(recvbuf, "You didn't send the correct remainder, try again...") != NULL)) {
                i++;
            }

            if (i == 3) {
                printf("\nRudenko Roman\n\n");
            }

            if (i >= 4) {
                SetConsoleTextAttribute(hConsole, 10);
            }

            for (int j = 0; j < iResult; j++) {
                printf("%c", recvbuf[j]);
                if (i >= 3) {
                    Sleep(10);//100
                }
            }
            //printf("Received data: %s\n", recvbuf);
        }
        else if (iResult == 0)
            printf("Connection closed\n");     //v tomto pripade server ukoncil komunikaciu
        else
            printf("recv failed with error : % d\n", WSAGetLastError());     //ina chyba
     }


    closesocket(ConnectSocket);
    WSACleanup();
}