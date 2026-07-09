# DNS Resolver (C++)

A mini DNS resolver built completely from scratch in C++ without using any built-in DNS lookup libraries.

The program sends raw DNS queries to Google's public DNS server (`8.8.8.8`), receives the binary response over UDP, and manually parses the DNS packet to extract IP addresses.

## Features

* **Constructs DNS packets manually:** Builds the raw binary format required by the DNS protocol.
* **Uses UDP sockets:** Communicates directly with DNS servers over connectionless transport.
* **Encodes domain names:** Converts standard strings into DNS wire format (length-prefixed labels).
* **Parses DNS response headers:** Decodes the 12-byte header to read status flags and record counts.
* **Parses answer records:** Extracts resource data fields from incoming payloads.
* **Supports multiple A records:** Successfully iterates through and lists multiple mapped IP addresses.
* **Handles CNAME records:** Accounts for canonical name aliases during the pointer parsing phase.
* **Handles DNS compression pointers (`0xC0`):** Correctly reads and jumps to compressed name offsets.
* **Supports command-line input:** Dynamically processes domain targets passed via terminal arguments.
* **Handles invalid domains:** Gracefully detects and reports when a domain lookup fails.
* **Uses socket timeout:** Prevents the application from hanging forever if a packet drops.

## Technologies Used

* **C++** (Standard Standard Template Library)
* **Winsock2 / POSIX Sockets** (Low-level networking API)

## Project Structure

```text
dns-resolver-cpp/
├── main.cpp
├── README.md
└── .gitignore