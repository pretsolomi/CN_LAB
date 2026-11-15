/*  Write a C program to
      a) Capture TCP packets and dump the result to a log file
      b) Capture UDP packets and dump the result to a log file
      c) Capture ICMP packets and dump the result to a log file 
*/
//to run use : sudo gcc lab7.c -o lab7 && ./lab7
//or
/*
//to run use : 
1st: sudo gcc lab7.c -o lab7 
2nd : ./lab7
*/
// Compile: gcc lab7.c -o lab7
// Run    : sudo ./lab7

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

int main() {
    unsigned char buffer[65536];
    int tcp = 0, udp = 0, icmp = 0;

    FILE *log = fopen("packet_log.txt", "w");
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));

    if (!log || sock < 0) {
        perror("Error (Run with sudo!)");
        return 1;
    }

    printf("Capturing packets... Press Ctrl+C to stop.\n");

    while (1) {
        int size = recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL);
        if (size < 0) break;

        // 1. Jump past Ethernet Header (14 bytes) to get IP Header
        struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
        int ip_len = iph->ihl * 4;

        // Convert binary IPs to readable strings
        char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &iph->saddr, src_ip, sizeof(src_ip));
        inet_ntop(AF_INET, &iph->daddr, dst_ip, sizeof(dst_ip));

        // 2. Jump past Ethernet + IP Headers to reach Transport Header
        unsigned char *transport_hdr = buffer + sizeof(struct ethhdr) + ip_len;

        // Question (a): Capture TCP
        if (iph->protocol == IPPROTO_TCP) {
            struct tcphdr *tcph = (struct tcphdr *)transport_hdr;
            fprintf(log, "[TCP #%d] %s:%u -> %s:%u | Length: %d\n",
                    ++tcp, src_ip, ntohs(tcph->source), dst_ip, ntohs(tcph->dest), size);
        }
        // Question (b): Capture UDP
        else if (iph->protocol == IPPROTO_UDP) {
            struct udphdr *udph = (struct udphdr *)transport_hdr;
            fprintf(log, "[UDP #%d] %s:%u -> %s:%u | Length: %d\n",
                    ++udp, src_ip, ntohs(udph->source), dst_ip, ntohs(udph->dest), size);
        }
        // Question (c): Capture ICMP
        else if (iph->protocol == IPPROTO_ICMP) {
            struct icmphdr *icmph = (struct icmphdr *)transport_hdr;
            fprintf(log, "[ICMP #%d] %s -> %s | Type: %d, Code: %d\n",
                    ++icmp, src_ip, dst_ip, icmph->type, icmph->code);
        }

        fflush(log); // Immediately save to file before Ctrl+C kills the loop
    }

    close(sock);
    fclose(log);
    return 0;
}
