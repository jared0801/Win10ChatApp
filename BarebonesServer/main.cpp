#define _WIN32_WINNT _WIN32_WINNT_WIN10
#include <iostream>
#include <string>
#include <sstream>
#include <WS2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

using std::cout;
using std::cerr;
using std::endl;
using std::string;

int main() {
    // Initialize winsock
    WSAData wsData;
    WORD ver = MAKEWORD(2, 2); // Version 2.2
    const int port = 54000;

    int wsOk = WSAStartup(ver, &wsData);

    if(wsOk != 0) {
        cerr << "Can't initialize WinSock! Quitting..." << endl;
        return 1;
    }

    // Create a socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == INVALID_SOCKET) {
        cerr << "Can't create a socket! Quitting " << endl;
        return 1;
    }

    // Bind socket to IP and port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could use inet_pton...

    bind(listening, (sockaddr*)&hint, sizeof(hint));


    // Tell winsock the socket is for listening
    listen(listening, SOMAXCONN);

    fd_set master;
    FD_ZERO(&master);

    FD_SET(listening, &master);

    cout << "Server is listening on port " << port << endl;

    while(true) {
        fd_set working = master;

        // Place sockets with readability in working fd_set
        int socketCount = select(0, &working, nullptr, nullptr, nullptr);

        for(int i = 0; i < socketCount; i++) {
            SOCKET sock = working.fd_array[i];
            if(sock == listening) {
                // Accept a new connection
                SOCKET client = accept(listening, nullptr, nullptr);

                // Add the new connection to list of connected clients
                FD_SET(client, &master);

                // Send welcome message to connected client
                string welcomeMsg = "Welcome to the Awesome Chat Server!\r\n";
                send(client, welcomeMsg.c_str(), welcomeMsg.size()+1, 0);

                // todo: Broadcast we have a new connection

            } else {
                char buf[4096];
                ZeroMemory(buf, 4096);

                // Accept a new message
                int bytesIn = recv(sock, buf, 4096, 0);
                if(bytesIn <= 0) {
                    // Drop the client
                    closesocket(sock);
                    FD_CLR(sock, &master);
                } else {
                    // Send message to other clients, not listening socket
                    for(int i = 0; i < master.fd_count; i++) {
                        SOCKET outSock = master.fd_array[i];

                        if(outSock == listening) continue;

                        std::ostringstream ss;

                        if(outSock != sock) {
                            ss << "SOCKET #" << sock << ": " << buf << "\r\n";
                        } else {
                            ss << "ME: " << buf << "\r\n";
                        }

                        string strOut = ss.str();
                        send(outSock, strOut.c_str(), strOut.size()+1, 0);
                        
                    }
                }
            }

        }

    }
    

    // Close the socket
    //closesocket(clientSocket);

    // Cleanup winsock
    WSACleanup();

    return 0;
}



// Single client code
/*sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if(clientSocket == INVALID_SOCKET) {
        cerr << "Can't accept client socket! Quitting " << endl;
        return 1;
    }

    char host[NI_MAXHOST]; // Client's remote name
    char service[NI_MAXSERV]; // Service / port the client is connected on

    ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST)
    ZeroMemory(service, NI_MAXSERV);

    // Attempt to get hostname, otherwise display ip address
    if(getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
        cout << host << " connected on port " << service << endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port " << ntohs(client.sin_port) << endl;
    }

    // Close listening socket (one client only)
    closesocket(listening);

    // While loop: accept and echo message back to client
    char buf[4096];

    while(true) {
        ZeroMemory(buf, 4096);

        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if(bytesReceived == SOCKET_ERROR) {
            cerr << "Error in recv(). Quitting..." << endl;
            return 1;
        }

        if(bytesReceived == 0) {
            cout << "Client disconnected";
            break;
        }

        cout << string(buf, 0, bytesReceived);

        // Echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0); // Add 1 to account for null terminator

    }
    
    // Close the socket
    closesocket(clientSocket);
    */