/*  Write a C program to
      a) Capture TCP packets and dump the result to a log file
      b) Capture UDP packets and dump the result to a log file
      c) Capture ICMP packets and dump the result to a log file 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>    // struct ethhdr
#include <netinet/ip.h>      // struct iphdr
#include <netinet/tcp.h>     // struct tcphdr
#include <netinet/udp.h>     // struct udphdr
#include <netinet/ip_icmp.h> // struct icmphdr

#define BUFFER_SIZE 65536

// Global file pointer for logging
FILE *logfile = NULL;
int raw_sock = -1;

// Packet counters
int tcp_count = 0;
int udp_count = 0;
int icmp_count = 0;

// Graceful exit on Ctrl+C (SIGINT)
void handle_sigint(int sig) {
    printf("\n[!] Stopping packet capture...\n");
    printf("[+] Total TCP Packets : %d\n", tcp_count);
    printf("[+] Total UDP Packets : %d\n", udp_count);
    printf("[+] Total ICMP Packets: %d\n", icmp_count);
    printf("[+] Results saved to packet_log.txt\n");

    if (logfile) fclose(logfile);
    if (raw_sock >= 0) close(raw_sock);
    exit(0);
}

// Helper to dump raw hex/ASCII payload data to the log file
void log_payload(const unsigned char *data, int size) {
    for (int i = 0; i < size; i++) {
        if (i != 0 && i % 16 == 0) {
            fprintf(logfile, "         ");
            for (int j = i - 16; j < i; j++) {
                if (data[j] >= 32 && data[j] <= 128)
                    fprintf(logfile, "%c", (unsigned char)data[j]);
                else
                    fprintf(logfile, ".");
            }
            fprintf(logfile, "\n");
        }
        if (i % 16 == 0) fprintf(logfile, "   ");
        fprintf(logfile, " %02X", (unsigned int)data[i]);

        if (i == size - 1) {
            for (int j = 0; j < 15 - (i % 16); j++) {
                fprintf(logfile, "   ");
            }
            fprintf(logfile, "         ");
            for (int j = i - (i % 16); j <= i; j++) {
                if (data[j] >= 32 && data[j] <= 128)
                    fprintf(logfile, "%c", (unsigned char)data[j]);
                else
                    fprintf(logfile, ".");
            }
            fprintf(logfile, "\n");
        }
    }
}

// Helper to log common IPv4 header fields
void log_ip_header(const struct iphdr *iph) {
    struct sockaddr_in src, dst;
    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));
    src.sin_addr.s_addr = iph->saddr;
    dst.sin_addr.s_addr = iph->daddr;

    fprintf(logfile, "IP Header:\n");
    fprintf(logfile, "   |-Source IP        : %s\n", inet_ntoa(src.sin_addr));
    fprintf(logfile, "   |-Destination IP   : %s\n", inet_ntoa(dst.sin_addr));
    fprintf(logfile, "   |-IP Header Length : %d Bytes\n", ((unsigned int)(iph->ihl)) * 4);
    fprintf(logfile, "   |-TTL              : %d\n", (unsigned int)iph->ttl);
    fprintf(logfile, "   |-Total Length     : %d Bytes\n", ntohs(iph->tot_len));
}

/* 
 * ============================================================================
 * Question (a): Capture TCP packets and dump the result to a log file
 * ============================================================================
 */
void process_tcp_packet(const unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;

    // TCP header immediately follows the IP header
    struct tcphdr *tcph = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + iphdrlen);
    int tcphdrlen = tcph->doff * 4;
    int header_size = sizeof(struct ethhdr) + iphdrlen + tcphdrlen;

    tcp_count++;

    fprintf(logfile, "\n==================== TCP PACKET #%d ====================\n", tcp_count);
    log_ip_header(iph);

    fprintf(logfile, "TCP Header:\n");
    fprintf(logfile, "   |-Source Port      : %u\n", ntohs(tcph->source));
    fprintf(logfile, "   |-Destination Port : %u\n", ntohs(tcph->dest));
    fprintf(logfile, "   |-Sequence Number  : %u\n", ntohl(tcph->seq));
    fprintf(logfile, "   |-Acknowledge Number: %u\n", ntohl(tcph->ack_seq));
    fprintf(logfile, "   |-Header Length    : %d Bytes\n", tcphdrlen);
    fprintf(logfile, "   |-Flags            : [ %s%s%s%s%s%s]\n",
            tcph->urg ? "URG " : "",
            tcph->ack ? "ACK " : "",
            tcph->psh ? "PSH " : "",
            tcph->rst ? "RST " : "",
            tcph->syn ? "SYN " : "",
            tcph->fin ? "FIN " : "");
    fprintf(logfile, "   |-Window Size      : %d\n", ntohs(tcph->window));
    fprintf(logfile, "   |-Checksum         : %d\n", ntohs(tcph->check));

    // Log TCP payload
    int payload_size = size - header_size;
    if (payload_size > 0) {
        fprintf(logfile, "Payload (%d Bytes):\n", payload_size);
        log_payload(buffer + header_size, payload_size);
    }
    fflush(logfile);
}

/* 
 * ============================================================================
 * Question (b): Capture UDP packets and dump the result to a log file
 * ============================================================================
 */
void process_udp_packet(const unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;

    // UDP header immediately follows the IP header (fixed 8 bytes)
    struct udphdr *udph = (struct udphdr *)(buffer + sizeof(struct ethhdr) + iphdrlen);
    int header_size = sizeof(struct ethhdr) + iphdrlen + sizeof(struct udphdr);

    udp_count++;

    fprintf(logfile, "\n==================== UDP PACKET #%d ====================\n", udp_count);
    log_ip_header(iph);

    fprintf(logfile, "UDP Header:\n");
    fprintf(logfile, "   |-Source Port      : %u\n", ntohs(udph->source));
    fprintf(logfile, "   |-Destination Port : %u\n", ntohs(udph->dest));
    fprintf(logfile, "   |-UDP Length       : %d Bytes\n", ntohs(udph->len));
    fprintf(logfile, "   |-UDP Checksum     : %d\n", ntohs(udph->check));

    // Log UDP payload
    int payload_size = size - header_size;
    if (payload_size > 0) {
        fprintf(logfile, "Payload (%d Bytes):\n", payload_size);
        log_payload(buffer + header_size, payload_size);
    }
    fflush(logfile);
}

/* 
 * ============================================================================
 * Question (c): Capture ICMP packets and dump the result to a log file
 * ============================================================================
 */
void process_icmp_packet(const unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;

    // ICMP header immediately follows the IP header
    struct icmphdr *icmph = (struct icmphdr *)(buffer + sizeof(struct ethhdr) + iphdrlen);
    int header_size = sizeof(struct ethhdr) + iphdrlen + sizeof(struct icmphdr);

    icmp_count++;

    fprintf(logfile, "\n==================== ICMP PACKET #%d ====================\n", icmp_count);
    log_ip_header(iph);

    fprintf(logfile, "ICMP Header:\n");
    fprintf(logfile, "   |-Type             : %d ", (unsigned int)(icmph->type));
    if ((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
        fprintf(logfile, "(Echo Reply)\n");
    else if ((unsigned int)(icmph->type) == ICMP_ECHO)
        fprintf(logfile, "(Echo Request)\n");
    else
        fprintf(logfile, "(Other)\n");

    fprintf(logfile, "   |-Code             : %d\n", (unsigned int)(icmph->code));
    fprintf(logfile, "   |-Checksum         : %d\n", ntohs(icmph->checksum));

    // Log ICMP payload (e.g., ping timestamp & data)
    int payload_size = size - header_size;
    if (payload_size > 0) {
        fprintf(logfile, "Payload (%d Bytes):\n", payload_size);
        log_payload(buffer + header_size, payload_size);
    }
    fflush(logfile);
}

int main() {
    unsigned char buffer[BUFFER_SIZE];

    // Catch SIGINT (Ctrl + C) to flush file and clean up
    signal(SIGINT, handle_sigint);

    // Open file for logging
    logfile = fopen("packet_log.txt", "w");
    if (!logfile) {
        perror("[-] Failed to create log file");
        return 1;
    }

    // Create raw socket to capture all IPv4 packets at layer 2
    raw_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (raw_sock < 0) {
        perror("[-] Socket creation failed (Are you running as root/sudo?)");
        fclose(logfile);
        return 1;
    }

    printf("[+] Packet sniffer started. Logging packets to 'packet_log.txt'...\n");
    printf("[+] Press Ctrl+C to stop.\n");

    while (1) {
        int data_size = recvfrom(raw_sock, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (data_size < 0) {
            perror("[-] Failed to receive packets");
            break;
        }

        // Parse IPv4 header (skip Ethernet header)
        struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));

        // Dispatch based on protocol number
        switch (iph->protocol) {
            case IPPROTO_TCP:   // 6
                process_tcp_packet(buffer, data_size);
                break;
            case IPPROTO_UDP:   // 17
                process_udp_packet(buffer, data_size);
                break;
            case IPPROTO_ICMP:  // 1
                process_icmp_packet(buffer, data_size);
                break;
            default:
                // Ignore other protocols (e.g., IGMP, OSPF)
                break;
        }
    }

    close(raw_sock);
    fclose(logfile);
    return 0;
}
