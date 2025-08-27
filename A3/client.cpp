#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <cerrno>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define TIMEOUT_SEC 5  // Timeout in seconds

void print_tcp_flags(struct tcphdr *tcph) {
    std::cout << "TCP Flags: "
              << "SYN:" << (tcph->syn ? "1" : "0")
              << " ACK:" << (tcph->ack ? "1" : "0")
              << " FIN:" << (tcph->fin ? "1" : "0")
              << " RST:" << (tcph->rst ? "1" : "0")
              << " PSH:" << (tcph->psh ? "1" : "0")
              << " URG:" << (tcph->urg ? "1" : "0")
              << " SEQ:" << ntohl(tcph->seq)
              << " ACK:" << ntohl(tcph->ack_seq)
              << std::endl;
}

int main() {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Tell kernel we're including our own IP header
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("Error setting IP_HDRINCL");
        exit(1);
    }

    // Set receive timeout
    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error setting socket timeout");
        exit(1);
    }

    char datagram[4096];
    memset(datagram, 0, 4096);

    // IP and TCP headers
    struct iphdr *iph = (struct iphdr *)datagram;
    struct tcphdr *tcph = (struct tcphdr *)(datagram + sizeof(struct iphdr));
    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &sin.sin_addr);

    // Fill IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
    iph->id = htons(54321);
    iph->frag_off = 0;
    iph->ttl = 64;
    iph->protocol = IPPROTO_TCP;
    iph->saddr = inet_addr("127.0.0.1");
    iph->daddr = sin.sin_addr.s_addr;
    iph->check = 0;  // Let kernel compute

    // Fill TCP Header (SYN)
    tcph->source = htons(54321);
    tcph->dest = htons(SERVER_PORT);
    tcph->seq = htonl(200);  // Initial sequence number
    tcph->ack_seq = 0;
    tcph->doff = 5;
    tcph->syn = 1;
    tcph->window = htons(8192);
    tcph->check = 0;  // Let kernel compute

    // Send SYN
    std::cout << "[+] Sending SYN..." << std::endl;
    print_tcp_flags(tcph);
    if (sendto(sock, datagram, sizeof(struct iphdr) + sizeof(struct tcphdr), 0,
               (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("Send failed");
        exit(1);
    }

    // Receive SYN-ACK with timeout
    char buffer[65536];
    bool received_syn_ack = false;
    time_t start_time = time(nullptr);

    while (true) {
        if (time(nullptr) - start_time > TIMEOUT_SEC) {
            std::cerr << "[-] Timeout: No response from server after " << TIMEOUT_SEC << " seconds" << std::endl;
            break;
        }

        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Timeout occurred
                std::cerr << "[-] Timeout: No response from server" << std::endl;
                break;
            }
            continue;  // Other errors, continue waiting
        }

        struct iphdr *recv_iph = (struct iphdr *)buffer;
        struct tcphdr *recv_tcph = (struct tcphdr *)(buffer + recv_iph->ihl * 4);

        if (recv_tcph->syn && recv_tcph->ack) {
            std::cout << "[+] Received SYN-ACK from server" << std::endl;
            print_tcp_flags(recv_tcph);
            received_syn_ack = true;

            // Verify the ACK number is our SEQ+1
            if (ntohl(recv_tcph->ack_seq) != 201) {
                std::cerr << "[-] Error: Invalid ACK number in SYN-ACK (expected 201, got " 
                          << ntohl(recv_tcph->ack_seq) << ")" << std::endl;
                close(sock);
                return 1;
            }

            // Send ACK
            memset(datagram, 0, 4096);
            iph = (struct iphdr *)datagram;
            tcph = (struct tcphdr *)(datagram + sizeof(struct iphdr));

            iph->ihl = 5;
            iph->version = 4;
            iph->tos = 0;
            iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
            iph->id = htons(54322);
            iph->frag_off = 0;
            iph->ttl = 64;
            iph->protocol = IPPROTO_TCP;
            iph->saddr = inet_addr("127.0.0.1");
            iph->daddr = sin.sin_addr.s_addr;
            iph->check = 0;  // Kernel fills in

            tcph->source = htons(54321);
            tcph->dest = htons(SERVER_PORT);
            tcph->seq = htonl(201);  // Our next sequence number (200 + 1)
            tcph->ack_seq = htonl(ntohl(recv_tcph->seq) + 1);  // ACK server's sequence number + 1
            tcph->doff = 5;
            tcph->ack = 1;
            tcph->window = htons(8192);
            tcph->check = 0;  // Kernel fills in

            std::cout << "[+] Sending ACK..." << std::endl;
            print_tcp_flags(tcph);
            if (sendto(sock, datagram, sizeof(struct iphdr) + sizeof(struct tcphdr), 0,
                       (struct sockaddr *)&sin, sizeof(sin)) < 0) {
                perror("Send failed");
                close(sock);
                return 1;
            }
            std::cout << "[+] Handshake complete!" << std::endl;
            break;
        }
    }

    if (!received_syn_ack) {
        std::cerr << "[-] TCP handshake failed: server may not be running" << std::endl;
    }

    close(sock);
    return received_syn_ack ? 0 : 1;
}