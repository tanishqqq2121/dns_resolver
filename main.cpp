#include <iostream>
#include <winsock2.h>
#include <vector>
#include <string>
#include <cstring>
#include <iomanip>

#pragma comment(lib, "ws2_32.lib")

struct DNSHeader {
    uint16_t id;
    uint16_t flags;
    uint16_t q_count;
    uint16_t ans_count;
    uint16_t auth_count;
    uint16_t add_count;
};

void encode_dns_name(const std::string& domain, std::vector<uint8_t>& buffer) {
    std::string label = "";
    std::string full_domain = domain + ".";
    for (char c : full_domain) {
        if (c == '.') {
            buffer.push_back(static_cast<uint8_t>(label.length()));
            for (char l : label) {
                buffer.push_back(static_cast<uint8_t>(l));
            }
            label = "";
        } else {
            label += c;
        }
    }
    buffer.push_back(0);
}

int main() {
    std::cout << "--- Milestone 5: Sending & Receiving DNS Response ---" << std::endl;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(53);
    serverAddr.sin_addr.s_addr = inet_addr("8.8.8.8");

    // Build Query Packet
    std::vector<uint8_t> packet;
    DNSHeader header;
    header.id = htons(0x1234);
    header.flags = htons(0x0100); // Recursion Desired
    header.q_count = htons(1);
    header.ans_count = htons(0);
    header.auth_count = htons(0);
    header.add_count = htons(0);

    uint8_t header_bytes[12];
    std::memcpy(header_bytes, &header, 12);
    packet.insert(packet.end(), header_bytes, header_bytes + 12);

    encode_dns_name("example.com", packet);

    uint16_t qtype = htons(1); // Type A (IPv4)
    packet.push_back(0); packet.push_back(1);
    
    uint16_t qclass = htons(1); // Class IN
    packet.push_back(0); packet.push_back(1);

    // Send Query
    int bytesSent = sendto(clientSocket, reinterpret_cast<const char*>(packet.data()), packet.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Failed to send query." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "[Step 1] Query sent successfully." << std::endl;

    // Receive Response
    std::cout << "[Step 2] Waiting for Google DNS response..." << std::endl;
    
    uint8_t responseBuffer[512]; // DNS over UDP has a max classic size of 512 bytes
    int serverAddrLen = sizeof(serverAddr);
    
    int bytesReceived = recvfrom(clientSocket, reinterpret_cast<char*>(responseBuffer), sizeof(responseBuffer), 0, (struct sockaddr*)&serverAddr, &serverAddrLen);

    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "[ERROR] Failed to receive data. Code: " << WSAGetLastError() << std::endl;
    } else {
        std::cout << "[SUCCESS] Received " << bytesReceived << " bytes back from Google!" << std::endl;
        
        // Print out the raw hexadecimal response layout
        std::cout << "\n--- Raw Hex Response ---" << std::endl;
        for (int i = 0; i < bytesReceived; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(responseBuffer[i]) << " ";
            if ((i + 1) % 16 == 0) std::cout << std::endl; // Newline every 16 bytes
        }
        std::cout << std::dec << "\n------------------------" << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}