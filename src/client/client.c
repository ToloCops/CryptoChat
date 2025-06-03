#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include "crypto.h"
#include "thread_pool.h"
#include "socket_utils.h"
#include "signal_utils.h"

int run_client(const char* filename, uint64_t key, int num_threads, const char* ip_addr, int port) {

    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    uint8_t* buffer = malloc(MAX_FILE_SIZE);
    if (!buffer) {
        perror("Error allocating memory");
        fclose(f);
        return EXIT_FAILURE;
    }

    size_t read_bytes = fread(buffer, 1, MAX_FILE_SIZE, f);
    fclose(f);

    if (read_bytes == 0) {
        fprintf(stderr, "Empty file or error while reading it.\n");
        free(buffer);
        return EXIT_FAILURE;
    }

    // Convert the data to blocks of 64 bits (8 bytes)
    size_t num_blocks;
    uint64_t* blocks = pad_and_convert_to_blocks(buffer, read_bytes, &num_blocks);
    if (!blocks) {
        perror("Errore conversione a blocchi");
        free(buffer);
        return EXIT_FAILURE;
    }

    sigset_t set = get_blocking_signal_set();
    sigset_t old;
    pthread_sigmask(SIG_BLOCK, &set, &old);  // Blocks critical signals

    run_parallel_encryption(blocks, num_blocks, key, num_threads);

    pthread_sigmask(SIG_SETMASK, &old, NULL);  // Unblocks critical signals

    printf("Cyphred blocks (hex):\n");
    for (size_t i = 0; i < num_blocks; ++i) {
        printf("  [%zu]: 0x%016llx\n", i, (unsigned long long)blocks[i]);
    }

    int sockfd = connect_to_server(ip_addr, port);
    if (sockfd < 0) {
        perror("Connection to server failed");
        free(buffer);
        free(blocks);
        return EXIT_FAILURE;
    }

    pthread_sigmask(SIG_BLOCK, &set, &old);

    // Sending the header: [L, K, n]
    uint64_t header[3] = { (uint64_t)read_bytes, key, (uint64_t)num_blocks };
    if (send_all(sockfd, header, sizeof(header)) < 0 ||
        send_all(sockfd, blocks, num_blocks * sizeof(uint64_t)) < 0) {
        perror("Errore invio dati");
        close(sockfd);
        free(buffer);
        free(blocks);
        return EXIT_FAILURE;
    }

    pthread_sigmask(SIG_SETMASK, &old, NULL);

    // Wait for ACK from server
    char ack;
    if (recv_all(sockfd, &ack, 1) < 0) {
        perror("Errore ricezione ACK");
    } else {
        printf("ACK ricevuto: %c\n", ack);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}
