#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    std::cout << "--- Milestone 3: Initializing Socket Target ---" << std::endl;

    // 1. Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }
    std::cout << "[Step 1] Winsock initialized successfully." << std::endl;

    // 2. Create the UDP Socket
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "[Step 2] UDP Socket created successfully." << std::endl;

    // 3. Configure the Target Server Address (Google DNS: 8.8.8.8 on Port 53)
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;                 // IPv4 Protocol family
    serverAddr.sin_port = htons(53);                 // DNS standard Port 53
    serverAddr.sin_addr.s_addr = inet_addr("8.8.8.8"); // Target IP Address conversion

    std::cout << "[Step 3] Target configured: Google DNS (8.8.8.8) on Port 53." << std::endl;

    // 4. Clean up and close socket
    closesocket(clientSocket);
    WSACleanup();
    std::cout << "Socket closed cleanly. Milestone 3 complete!" << std::endl;

    return 0;
}