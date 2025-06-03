#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "client.h"

void print_usage(const char* progname) {
    fprintf(stderr, "Usage: %s <file> <hex_key> <thread> <ip> <port>\n", progname);
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* filename = argv[1];
    uint64_t key = strtoull(argv[2], NULL, 16);
    int num_threads = atoi(argv[3]);
    const char* ip_addr = argv[4];
    int port = atoi(argv[5]);

    if (num_threads <= 0) {
        fprintf(stderr, "Error: # of threads must be > 0.\n");
        return EXIT_FAILURE;
    }

    return run_client(filename, key, num_threads, ip_addr, port);
}
