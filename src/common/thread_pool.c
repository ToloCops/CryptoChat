#include "thread_pool.h"
#include "crypto.h"
#include <pthread.h>
#include <stdlib.h>

void* encrypt_worker(void* arg) {
    EncryptTask* task = (EncryptTask*)arg;

    for (size_t i = task->start; i < task->end; ++i) {
        encrypt_block(&task->blocks[i], task->blocks[i], task->key);
    }

    return NULL;
}

void run_parallel_encryption(uint64_t* blocks, size_t num_blocks, uint64_t key, int num_threads) {
    if (num_threads <= 0) num_threads = 1;
    if ((size_t)num_threads > num_blocks) num_threads = (int)num_blocks;

    pthread_t threads[num_threads];
    EncryptTask tasks[num_threads];

    size_t chunk = num_blocks / num_threads;
    size_t remainder = num_blocks % num_threads;

    size_t index = 0;
    for (int i = 0; i < num_threads; ++i) {
        size_t count = chunk + (i < (int)remainder ? 1 : 0);
        tasks[i].blocks = blocks;
        tasks[i].key = key;
        tasks[i].start = index;
        tasks[i].end = index + count;
        index += count;

        pthread_create(&threads[i], NULL, encrypt_worker, &tasks[i]);
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
}
