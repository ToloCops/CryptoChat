// main.c

#include <stdio.h>
#include <stdlib.h>
#include "server.h"

void print_usage(const char* progname) {
    fprintf(stderr, "Uso: %s <thread> <prefisso_file> <porta>\n", progname);
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <thread> <prefisso_file> <porta> <max_conn>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_threads = atoi(argv[1]);
    const char* file_prefix = argv[2];
    int port = atoi(argv[3]);
    int max_conn = atoi(argv[4]);

    return run_server(num_threads, file_prefix, port, max_conn);
}
