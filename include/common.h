#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>

// Struttura per il messaggio cifrato (non serializzata direttamente)
typedef struct {
    uint64_t length;      // Lunghezza originale del file (in byte)
    uint64_t key;         // Chiave usata (64 bit)
    size_t num_blocks;    // Numero di blocchi cifrati
    uint64_t* blocks;     // Puntatore ai blocchi
} EncryptedMessage;

#endif // COMMON_H
