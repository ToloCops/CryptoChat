// main.c

#include <stdio.h>
#include <stdlib.h>
#include "server.h"

void print_usage(const char* progname) {
    fprintf(stderr, "Uso: %s <thread> <prefisso_file> <porta>\n", progname);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    int num_threads = atoi(argv[1]);
    const char* file_prefix = argv[2];
    int port = atoi(argv[3]);

    if (num_threads <= 0 || port <= 0) {
        fprintf(stderr, "Argomenti non validi\n");
        return EXIT_FAILURE;
    }

    return run_server(num_threads, file_prefix, port);
}
