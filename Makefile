# Compilatore e flag
CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -Iinclude

# Directory
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Sorgenti comuni
COMMON_SRC = $(wildcard $(SRC_DIR)/common/*.c)

# Sorgenti client/server
CLIENT_SRC = $(wildcard $(SRC_DIR)/client/*.c) $(COMMON_SRC)
SERVER_SRC = $(wildcard $(SRC_DIR)/server/*.c) $(COMMON_SRC)

# Oggetti compilati
CLIENT_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CLIENT_SRC))
SERVER_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SERVER_SRC))

# Binari finali
CLIENT_BIN = $(BIN_DIR)/client
SERVER_BIN = $(BIN_DIR)/server

# Target di default
all: $(CLIENT_BIN) $(SERVER_BIN)

# Client
$(CLIENT_BIN): $(CLIENT_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Server
$(SERVER_BIN): $(SERVER_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Compilazione oggetti
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Pulizia
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Rebuild completo
rebuild: clean all
