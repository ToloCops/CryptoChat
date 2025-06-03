#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint64_t* blocks;
    uint64_t key;
    size_t start;
    size_t end;
} EncryptTask;

void run_parallel_encryption(uint64_t* blocks, size_t num_blocks, uint64_t key, int num_threads);  // Runs parallel encryption on the blocks using the specified key and number of threads

#endif
