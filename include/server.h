#ifndef SERVER_H
#define SERVER_H
#include <semaphore.h>

#define MAX_BLOCKS 8192  // max 64KB

extern sem_t sem_conn; // Semaphore for managing concurrent connections

typedef struct {
    int client_sock;
    char file_prefix[128];
    int num_threads;
} ClientThreadArgs;

int run_server(int num_threads, const char* file_prefix, int port, int max_conn);

void* handle_client(void* arg);

void handle_sigint(int signo); // Signal handler for graceful shutdown

#endif
