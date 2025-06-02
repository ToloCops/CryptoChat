#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "common.h"
#include "socket_utils.h"
#include "crypto.h"
#include "thread_pool.h"

#define MAX_BLOCKS 8192  // max 64KB

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

    // Inizializza socket di ascolto
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        perror("Errore creazione socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
    };

    if (bind(listen_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Errore bind");
        close(listen_sock);
        return EXIT_FAILURE;
    }

    if (listen(listen_sock, 5) < 0) {
        perror("Errore listen");
        close(listen_sock);
        return EXIT_FAILURE;
    }

    printf("Server in ascolto su porta %d...\n", port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int client_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &addrlen);
        if (client_sock < 0) {
            perror("Errore accept");
            continue;
        }

        // Ricevi [L, K, n]
        uint64_t header[3];
        if (recv_all(client_sock, header, sizeof(header)) < 0) {
            perror("Errore ricezione header");
            close(client_sock);
            continue;
        }

        uint64_t original_len = header[0];
        uint64_t key = header[1];
        size_t num_blocks = (size_t)header[2];

        if (num_blocks > MAX_BLOCKS) {
            fprintf(stderr, "Troppi blocchi, rifiutato\n");
            close(client_sock);
            continue;
        }

        // Ricevi blocchi cifrati
        uint64_t* encrypted_blocks = malloc(num_blocks * sizeof(uint64_t));
        if (!encrypted_blocks) {
            perror("Errore malloc");
            close(client_sock);
            continue;
        }

        if (recv_all(client_sock, encrypted_blocks, num_blocks * sizeof(uint64_t)) < 0) {
            perror("Errore ricezione blocchi");
            free(encrypted_blocks);
            close(client_sock);
            continue;
        }

        // Decifra
        run_parallel_encryption(encrypted_blocks, num_blocks, key, num_threads);

        // Ricostruisci testo originale
        uint8_t* data = convert_blocks_to_data(encrypted_blocks, num_blocks, original_len);

        // Costruisci nome file
        char filename[128];
        snprintf(filename, sizeof(filename), "%s_output.txt", file_prefix);

        FILE* f = fopen(filename, "wb");
        if (f) {
            fwrite(data, 1, original_len, f);
            fclose(f);
            printf("File scritto: %s\n", filename);
        } else {
            perror("Errore scrittura file");
        }

        // Invia ACK
        char ack = 'A';
        send_all(client_sock, &ack, 1);

        close(client_sock);
        free(encrypted_blocks);
        free(data);
    }

    close(listen_sock);
    return EXIT_SUCCESS;
}
