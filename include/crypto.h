#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include <stddef.h>

#define BLOCK_SIZE 8  // 64 bit = 8 byte

void encrypt_block(uint64_t* dest, uint64_t src, uint64_t key);

uint64_t* pad_and_convert_to_blocks(const uint8_t* data, size_t length, size_t* num_blocks_out);
uint8_t* convert_blocks_to_data(const uint64_t* blocks, size_t num_blocks, size_t original_length);

#endif // CRYPTO_H
