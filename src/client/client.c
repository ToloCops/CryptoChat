#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "crypto.h"
#include "common.h"
#include "thread_pool.h"
#include "socket_utils.h"

#define MAX_FILE_SIZE 65536  // 64 KB (modificabile)

void print_usage(const char* progname) {
    fprintf(stderr, "Uso: %s <file> <chiave esadecimale> <num_thread>\n", progname);
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Uso: %s <file> <chiave_hex> <thread> <ip> <porta>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* filename = argv[1];
    uint64_t key = strtoull(argv[2], NULL, 16);
    int num_threads = atoi(argv[3]);
    const char* ip_addr = argv[4];
    int port = atoi(argv[5]);

    if (num_threads <= 0) {
        fprintf(stderr, "Errore: il numero di thread deve essere maggiore di 0\n");
        return EXIT_FAILURE;
    }

    // Apri il file
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Errore apertura file");
        return EXIT_FAILURE;
    }

    // Leggi tutto il contenuto
    uint8_t* buffer = malloc(MAX_FILE_SIZE);
    if (!buffer) {
        perror("Errore allocazione buffer");
        fclose(f);
        return EXIT_FAILURE;
    }

    size_t read_bytes = fread(buffer, 1, MAX_FILE_SIZE, f);
    fclose(f);

    if (read_bytes == 0) {
        fprintf(stderr, "File vuoto o errore nella lettura\n");
        free(buffer);
        return EXIT_FAILURE;
    }

    // Converti in blocchi da 64 bit
    size_t num_blocks;
    uint64_t* blocks = pad_and_convert_to_blocks(buffer, read_bytes, &num_blocks);
    if (!blocks) {
        perror("Errore conversione a blocchi");
        free(buffer);
        return EXIT_FAILURE;
    }

    run_parallel_encryption(blocks, num_blocks, key, num_threads);

    // Stampa i blocchi cifrati
    printf("Blocchi cifrati (hex):\n");
    for (size_t i = 0; i < num_blocks; ++i) {
        printf("  [%zu]: 0x%016llx\n", i, (unsigned long long)blocks[i]);
    }

    // Qui costruirai il messaggio da inviare via socket nella fase successiva
    // EncryptedMessage msg = { read_bytes, key, num_blocks, blocks };

    int sockfd = connect_to_server(ip_addr, port);
    if (sockfd < 0) {
        perror("Connessione al server fallita");
        free(buffer);
        free(blocks);
        return EXIT_FAILURE;
    }

    // Invio: [L, K, n, blocchi] (tutti uint64_t)
    uint64_t header[3] = { (uint64_t)read_bytes, key, (uint64_t)num_blocks };
    if (send_all(sockfd, header, sizeof(header)) < 0 ||
        send_all(sockfd, blocks, num_blocks * sizeof(uint64_t)) < 0) {
        perror("Errore invio dati");
        close(sockfd);
        free(buffer);
        free(blocks);
        return EXIT_FAILURE;
    }

    // Attendi ACK (es: 1 byte di risposta)
    char ack;
    if (recv_all(sockfd, &ack, 1) < 0) {
        perror("Errore ricezione ACK");
    } else {
        printf("ACK ricevuto: %c\n", ack);
    }

    close(sockfd);
}
