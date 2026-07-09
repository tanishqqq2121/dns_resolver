# DNS Resolver (C++)

A lightweight DNS resolver built completely from scratch in C++ without relying on any built-in DNS lookup libraries.

The program sends raw DNS queries to Google's public DNS server (8.8.8.8), receives the binary response over UDP, and manually parses the DNS packet to extract IPv4 (A) record IP addresses.

## Features

* Constructs DNS packets manually
* Uses UDP sockets (Winsock2) to communicate with DNS servers
* Encodes domain names into DNS wire format
* Parses DNS response headers
* Parses answer records
* Supports multiple A records
* Handles DNS compression pointers (0xC0)
* Supports command-line input
* Uses socket timeout to avoid hanging forever
* Displays resolved IPv4 addresses

## Technologies Used

* C++17
* Winsock2 (UDP sockets)
* DNS protocol (RFC 1035)
* Binary packet parsing

## Build

### Windows 11 (MinGW-w64)

```bash
g++ -std=c++17 -o dns-resolver.exe main.cpp -lws2_32
```

## Usage

```bash
dns-resolver.exe google.com

dns-resolver.exe brave.com

dns-resolver.exe www.youtube.com
```

## Example Output

```text
dns-resolver.exe google.com

===== DNS Resolver =====

Domain: google.com

IP Addresses:
1. 142.250.67.46

DNS query completed successfully!
```

## Concepts Learned

* UDP vs TCP
* DNS protocol
* DNS packet structure
* DNS header fields
* Question section
* Answer section
* Network byte order
* htons(), ntohs(), ntohl()
* DNS compression pointers
* Multiple A records
* Binary packet parsing
* Winsock2 programming

## Project Structure

```text
dns-resolver-cpp/
├── .gitignore
├── README.md
└── main.cpp
```

## Future Improvements

* AAAA (IPv6) record support
* CNAME record parsing
* MX record support
* TXT record support
* Recursive resolution
* Local DNS caching
* Configurable DNS servers
* Improved DNS response validation
* Better error handling


## Author

**Tanishq Gautam**  
24/CS/456  
B.Tech CSE (2024–2028)  

Delhi Technological University (DTU)  
Computer Science and Engineering



