#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include "server.h"
#include "socket_utils.h"
#include "crypto.h"
#include "thread_pool.h"
#include "signal_utils.h"

volatile sig_atomic_t stop_server = 0;
int listen_sock = -1;
sem_t sem_conn;

void handle_sigint(int signo) {
    (void)signo;
    stop_server = 1;
    if (listen_sock >= 0) {
        close(listen_sock);  // Force close the listening socket
        listen_sock = -1;
    }
}

void* handle_client(void* arg) {
    ClientThreadArgs* args = (ClientThreadArgs*)arg;
    int client_sock = args->client_sock;
    int num_threads = args->num_threads;
    const char* file_prefix = args->file_prefix;

    // Receives [L, K, n]
    uint64_t header[3];
    if (recv_all(client_sock, header, sizeof(header)) < 0) {
        perror("Errore ricezione header");
        close(client_sock);
        return NULL;
    }

    uint64_t original_len = header[0];
    uint64_t key = header[1];
    size_t num_blocks = (size_t)header[2];

    if (num_blocks > MAX_BLOCKS) {
        fprintf(stderr, "Troppi blocchi, rifiutato\n");
        close(client_sock);
        return NULL;
    }

    uint64_t* encrypted_blocks = malloc(num_blocks * sizeof(uint64_t));
    if (!encrypted_blocks) {
        perror("Errore malloc");
        close(client_sock);
        return NULL;
    }

    if (recv_all(client_sock, encrypted_blocks, num_blocks * sizeof(uint64_t)) < 0) {
        perror("Errore ricezione blocchi");
        free(encrypted_blocks);
        close(client_sock);
        return NULL;
    }

    // Blocks critical signals before decrypting
    sigset_t block = get_blocking_signal_set();
    sigset_t old;
    pthread_sigmask(SIG_BLOCK, &block, &old);

    run_parallel_encryption(encrypted_blocks, num_blocks, key, num_threads);

    // Unblocks critical signals after decryption
    pthread_sigmask(SIG_SETMASK, &old, NULL);

    // Debug: simulate heavy work
    printf("Thread client %d: sleeping...\n", client_sock);
    sleep(5); 

    uint8_t* data = convert_blocks_to_data(encrypted_blocks, num_blocks, original_len);

    char ack = 'A';
    send_all(client_sock, &ack, 1);
    close(client_sock);
    sem_post(&sem_conn);  // Frees the semaphore for a new connection

    char filename[256];
    snprintf(filename, sizeof(filename), "%s_output.txt", file_prefix);

    // Blocks critical signals before writing to file
    pthread_sigmask(SIG_BLOCK, &block, &old);

    FILE* f = fopen(filename, "wb");
    if (f) {
        fwrite(data, 1, original_len, f);
        fclose(f);
        printf("File scritto: %s\n", filename);
    } else {
        perror("Errore scrittura file");
    }

    // Unblocks critical signals after writing to file
    pthread_sigmask(SIG_SETMASK, &old, NULL);

    free(encrypted_blocks);
    free(data);
    free(args);

    return NULL;
}

int run_server(int num_threads, const char* file_prefix, int port, int max_conn) {
    // Prints file prefix
    printf("File prefix: %s\n", file_prefix);
    sem_init(&sem_conn, 0, max_conn);
    signal(SIGINT, handle_sigint);

    if (num_threads <= 0 || port <= 0) {
        fprintf(stderr, "Argomenti non validi\n");
        return EXIT_FAILURE;
    }

    // Initialize the listening socket
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        perror("Errore creazione socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
    };

    int opt = 1;
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Errore setsockopt");
    }


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

    printf("Server listening at port %d...\n", port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int client_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &addrlen);
        if (client_sock < 0) {
            if (stop_server) break;
            perror("Error accepting connection");
            continue;
        }

        // Wait for a semaphore slot
        sem_wait(&sem_conn);

        pthread_t tid;
        ClientThreadArgs* args = malloc(sizeof(ClientThreadArgs));
        if (!args) {
            perror("malloc");
            close(client_sock);
            continue;
        }

        args->client_sock = client_sock;
        args->num_threads = num_threads;
        strncpy(args->file_prefix, file_prefix, sizeof(args->file_prefix) - 1);
        // Prints file prefix
        printf("File prefix: %s\n", file_prefix);
        args->file_prefix[sizeof(args->file_prefix) - 1] = '\0';

        if (pthread_create(&tid, NULL, handle_client, args) != 0) {
            perror("Errore creazione thread");
            close(client_sock);
            free(args);
            continue;
        }

        pthread_detach(tid);
    }

    sem_destroy(&sem_conn);  // Destroy the semaphore

    printf("Terminazione server...\n");
    close(listen_sock);
    listen_sock = -1;
    return EXIT_SUCCESS;
}
