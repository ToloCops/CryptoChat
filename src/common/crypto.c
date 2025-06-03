#include "crypto.h"
#include <stdlib.h>
#include <string.h>

// XOR block encryption
void encrypt_block(uint64_t* dest, uint64_t src, uint64_t key) {
    *dest = src ^ key;
}

// Converts a byte buffer to blocks of 64 bits (8 bytes), padding if necessary
uint64_t* pad_and_convert_to_blocks(const uint8_t* data, size_t length, size_t* num_blocks_out) {
    size_t num_blocks = (length + BLOCK_SIZE - 1) / BLOCK_SIZE;
    uint64_t* blocks = calloc(num_blocks, sizeof(uint64_t));
    if (!blocks) return NULL;

    for (size_t i = 0; i < num_blocks; ++i) {
        uint64_t block = 0;
        size_t offset = i * BLOCK_SIZE;
        size_t copy_len = (offset + BLOCK_SIZE <= length) ? BLOCK_SIZE : (length - offset);

        memcpy(&block, data + offset, copy_len);
        blocks[i] = block;
    }

    *num_blocks_out = num_blocks;
    return blocks;
}

// Converts an array of 64-bit blocks back to a byte buffer, handling padding
uint8_t* convert_blocks_to_data(const uint64_t* blocks, size_t num_blocks, size_t original_length) {
    uint8_t* data = malloc(original_length);
    if (!data) return NULL;

    for (size_t i = 0; i < num_blocks; ++i) {
        size_t offset = i * BLOCK_SIZE;
        size_t copy_len = (offset + BLOCK_SIZE <= original_length) ? BLOCK_SIZE : (original_length - offset);

        memcpy(data + offset, &blocks[i], copy_len);
    }

    return data;
}
