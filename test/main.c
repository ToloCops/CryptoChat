#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "crypto.h"

// Funzione di utilità per stampare blocchi
void print_blocks(const char* label, const uint64_t* blocks, size_t num_blocks) {
    printf("%s:\n", label);
    for (size_t i = 0; i < num_blocks; ++i) {
        printf("  [%zu]: 0x%016llx\n", i, (unsigned long long)blocks[i]);
    }
    printf("\n");
}

int main() {
    // Stringa di test (come contenuto di un file)
    const char* original = "il mio nome è legenda";
    size_t original_len = strlen(original);

    // Chiave XOR di 64 bit
    uint64_t key = 0x656D696C69616E6F;  // ASCII "emiliano"

    // Conversione a blocchi
    size_t num_blocks;
    uint64_t* plain_blocks = pad_and_convert_to_blocks((const uint8_t*)original, original_len, &num_blocks);
    if (!plain_blocks) {
        perror("Errore nella conversione a blocchi");
        return EXIT_FAILURE;
    }

    print_blocks("Blocchi originali", plain_blocks, num_blocks);

    // Cifratura
    uint64_t* encrypted_blocks = malloc(num_blocks * sizeof(uint64_t));
    for (size_t i = 0; i < num_blocks; ++i) {
        encrypt_block(&encrypted_blocks[i], plain_blocks[i], key);
    }

    print_blocks("Blocchi cifrati", encrypted_blocks, num_blocks);

    // Decifratura
    uint64_t* decrypted_blocks = malloc(num_blocks * sizeof(uint64_t));
    for (size_t i = 0; i < num_blocks; ++i) {
        decrypt_block(&decrypted_blocks[i], encrypted_blocks[i], key);
    }

    print_blocks("Blocchi decifrati", decrypted_blocks, num_blocks);

    // Ricostruzione stringa
    uint8_t* decrypted_data = convert_blocks_to_data(decrypted_blocks, num_blocks, original_len);
    if (!decrypted_data) {
        perror("Errore nella ricostruzione dati");
        return EXIT_FAILURE;
    }

    printf("Testo decifrato: \"%.*s\"\n", (int)original_len, decrypted_data);

    // Pulizia
    free(plain_blocks);
    free(encrypted_blocks);
    free(decrypted_blocks);
    free(decrypted_data);

    return EXIT_SUCCESS;
}
