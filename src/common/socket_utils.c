#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int connect_to_server(const char* ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) return -1;

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) return -1;

    return sockfd;
}

int send_all(int sockfd, const void* buf, size_t len) {
    const uint8_t* p = buf;
    size_t total = 0;

    while (total < len) {
        ssize_t sent = send(sockfd, p + total, len - total, 0);
        if (sent <= 0) return -1;
        total += sent;
    }
    return 0;
}

int recv_all(int sockfd, void* buf, size_t len) {
    uint8_t* p = buf;
    size_t total = 0;

    while (total < len) {
        ssize_t recvd = recv(sockfd, p + total, len - total, 0);
        if (recvd <= 0) return -1;
        total += recvd;
    }
    return 0;
}
