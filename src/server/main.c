// main.c

#include <stdio.h>
#include <stdlib.h>
#include "server.h"

void print_usage(const char* progname) {
    fprintf(stderr, "Usage: %s <thread> <file_prefix> <port>\n", progname);
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    int num_threads = atoi(argv[1]);
    const char* file_prefix = argv[2];
    int port = atoi(argv[3]);
    int max_conn = atoi(argv[4]);

    return run_server(num_threads, file_prefix, port, max_conn);
}
