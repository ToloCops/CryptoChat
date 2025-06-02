#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>

#define MAX_FILE_SIZE 65536  // 64 KB (modificabile)

int run_client(const char* filename, uint64_t key, int num_threads, const char* ip_addr, int port);

#endif
