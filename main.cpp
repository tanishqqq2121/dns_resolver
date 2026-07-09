#include <iostream>
#include <winsock2.h>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>

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
    std::string label;
    std::string full_domain = domain + ".";

    for (char c : full_domain) {
        if (c == '.') {
            buffer.push_back(static_cast<uint8_t>(label.length()));
            for (char ch : label)
                buffer.push_back(static_cast<uint8_t>(ch));
            label.clear();
        } else {
            label += c;
        }
    }

    buffer.push_back(0);
}

int main(int argc, char* argv[]) {
    std::cout << "===== DNS Resolver =====\n\n";

    if (argc != 2) {
        std::cout << "Usage: dns-resolver.exe <domain>\n";
        return 1;
    }

    std::string targetDomain = argv[1];

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock.\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket.\n";
        WSACleanup();
        return 1;
    }

    // 5-second timeout
    DWORD timeout = 5000;
    setsockopt(
        clientSocket,
        SOL_SOCKET,
        SO_RCVTIMEO,
        (const char*)&timeout,
        sizeof(timeout)
    );

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(53);
    serverAddr.sin_addr.s_addr = inet_addr("8.8.8.8");

    std::vector<uint8_t> packet;

    DNSHeader header{};
    header.id = htons(0x1234);
    header.flags = htons(0x0100);
    header.q_count = htons(1);
    header.ans_count = 0;
    header.auth_count = 0;
    header.add_count = 0;

    uint8_t headerBytes[12];
    std::memcpy(headerBytes, &header, 12);
    packet.insert(packet.end(), headerBytes, headerBytes + 12);

    encode_dns_name(targetDomain, packet);

    // QTYPE = A
    packet.push_back(0);
    packet.push_back(1);

    // QCLASS = IN
    packet.push_back(0);
    packet.push_back(1);

    int sent = sendto(
        clientSocket,
        reinterpret_cast<const char*>(packet.data()),
        static_cast<int>(packet.size()),
        0,
        (sockaddr*)&serverAddr,
        sizeof(serverAddr)
    );

    if (sent == SOCKET_ERROR) {
        std::cerr << "Failed to send DNS query.\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    uint8_t response[512];

    int serverLen = sizeof(serverAddr);

    int received = recvfrom(
        clientSocket,
        reinterpret_cast<char*>(response),
        sizeof(response),
        0,
        (sockaddr*)&serverAddr,
        &serverLen
    );

    if (received == SOCKET_ERROR) {
        std::cerr << "Failed to receive response (timeout or invalid domain).\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    DNSHeader* respHeader = reinterpret_cast<DNSHeader*>(response);

    int answerCount = ntohs(respHeader->ans_count);

    std::cout << "Domain: " << targetDomain << "\n\n";

    if (answerCount == 0) {
        std::cout << "No A records found.\n";
        closesocket(clientSocket);
        WSACleanup();
        return 0;
    }

    std::cout << "IP Addresses:\n";

    int cursor = static_cast<int>(packet.size());

    int ipIndex = 1;

    for (int i = 0; i < answerCount; i++) {

        // Skip NAME
        if ((response[cursor] & 0xC0) == 0xC0) {
            cursor += 2;
        } else {
            while (response[cursor] != 0)
                cursor++;
            cursor++;
        }

        uint16_t type;
        std::memcpy(&type, response + cursor, 2);
        type = ntohs(type);
        cursor += 2;

        uint16_t dnsClass;
        std::memcpy(&dnsClass, response + cursor, 2);
        dnsClass = ntohs(dnsClass);
        cursor += 2;

        cursor += 4; // TTL

        uint16_t rdLength;
        std::memcpy(&rdLength, response + cursor, 2);
        rdLength = ntohs(rdLength);
        cursor += 2;

        if (type == 1 && dnsClass == 1 && rdLength == 4) {

            std::cout
                << ipIndex++ << ". "
                << (int)response[cursor] << "."
                << (int)response[cursor + 1] << "."
                << (int)response[cursor + 2] << "."
                << (int)response[cursor + 3]
                << "\n";
        }

        cursor += rdLength;
    }

    std::cout << "\nDNS query completed successfully!\n";

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}