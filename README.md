# DNS Resolver (C++)

A mini DNS resolver built completely from scratch in C++ without using any built-in DNS lookup libraries.

The program sends raw DNS queries to Google’s public DNS server (8.8.8.8), receives the binary response over UDP, and manually parses the DNS packet to extract IP addresses.





## Features

* Constructs DNS packets manually
* Uses UDP sockets to communicate with DNS servers
* Encodes domain names into DNS wire format
* Parses DNS response headers
* Parses answer records
* Supports multiple A records
* Handles CNAME records
* Handles DNS compression pointers (0xC0)
* Supports command-line input
* Handles invalid domains
* Uses socket timeout to avoid hanging forever





## Technologies Used

* C++
* UDP sockets
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
* CNAME records
* Multiple A records
* Binary packet parsing





## Project Structure

```text
dns-resolver-cpp/
├── .gitignore
├── README.md
└── main.cpp
```





## Future Improvements

* AAAA (IPv6) record support
* MX record support
* Recursive resolution
* Local DNS caching
* Configurable DNS servers





## Author

**Tanishq Gautam**  
24/CS/456  
B.Tech CSE (2024–2028)  

Delhi Technological University (DTU)  
Computer Science and Engineering
