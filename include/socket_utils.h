#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <stddef.h>

int connect_to_server(const char* ip, int port);
int send_all(int sockfd, const void* buf, size_t len);
int recv_all(int sockfd, void* buf, size_t len);

#endif
