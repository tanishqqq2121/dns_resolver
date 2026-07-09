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
    std::cout << "--- Final Project: DNS Resolver with Parsing ---" << std::endl;

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

    // Build Query
    std::vector<uint8_t> packet;
    DNSHeader header;
    header.id = htons(0x1234);
    header.flags = htons(0x0100);
    header.q_count = htons(1);
    header.ans_count = htons(0);
    header.auth_count = htons(0);
    header.add_count = htons(0);

    uint8_t header_bytes[12];
    std::memcpy(header_bytes, &header, 12);
    packet.insert(packet.end(), header_bytes, header_bytes + 12);

    std::string targetDomain = "example.com";
    encode_dns_name(targetDomain, packet);

    packet.push_back(0); packet.push_back(1); // Type A
    packet.push_back(0); packet.push_back(1); // Class IN

    // Send Query
    int bytesSent = sendto(clientSocket, reinterpret_cast<const char*>(packet.data()), packet.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Failed to send query." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Receive Response
    uint8_t responseBuffer[512];
    int serverAddrLen = sizeof(serverAddr);
    int bytesReceived = recvfrom(clientSocket, reinterpret_cast<char*>(responseBuffer), sizeof(responseBuffer), 0, (struct sockaddr*)&serverAddr, &serverAddrLen);

    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Failed to receive data." << std::endl;
    } else {
        std::cout << "[SUCCESS] Received response payload." << std::endl;
        
        // --- PARSING ENGINE ---
        // 1. Skip Header (12 bytes) + original encoded question size
        // The question starts at byte 12 and ends after the domain name length + 4 bytes (QTYPE/QCLASS)
        int answerOffset = packet.size(); 
        
        // Read how many answers the server sent back from the response header
        DNSHeader* respHeader = reinterpret_cast<DNSHeader*>(responseBuffer);
        int answerCount = ntohs(respHeader->ans_count);
        std::cout << "Found " << answerCount << " answer records in packet.\n" << std::endl;

        int cursor = answerOffset;
        for (int i = 0; i < answerCount; i++) {
            if (cursor >= bytesReceived) break;

            // Check if it's a pointer/compression (starts with 0xC0)
            if ((responseBuffer[cursor] & 0xC0) == 0xC0) {
                cursor += 2; // Skip the name pointer
            } else {
                while (responseBuffer[cursor] != 0 && cursor < bytesReceived) {
                    cursor++;
                }
                cursor++; // Skip null terminator
            }

            // Read Type, Class, and TTL metadata fields (skip 2 + 2 + 4 = 8 bytes)
            cursor += 8;

            // Read data length (RDLENGTH is 2 bytes)
            uint16_t rdLength;
            std::memcpy(&rdLength, &responseBuffer[cursor], 2);
            rdLength = ntohs(rdLength);
            cursor += 2;

            // If it's an IPv4 address (Type A), it must be exactly 4 bytes long
            if (rdLength == 4) {
                int ip1 = responseBuffer[cursor];
                int ip2 = responseBuffer[cursor+1];
                int ip3 = responseBuffer[cursor+2];
                int ip4 = responseBuffer[cursor+3];
                
                std::cout << "=> Resolved IP [" << i + 1 << "]: " 
                          << ip1 << "." << ip2 << "." << ip3 << "." << ip4 << std::endl;
            }
            cursor += rdLength; // Advance cursor past this record's data payload
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}