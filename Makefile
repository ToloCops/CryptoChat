# Compilatore e flag
CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -Iinclude

# Directory
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

CLIENT_SRC = $(wildcard $(SRC_DIR)/client/*.c) $(wildcard $(SRC_DIR)/common/*.c)
SERVER_SRC = $(wildcard $(SRC_DIR)/server/*.c) $(wildcard $(SRC_DIR)/common/*.c)

CLIENT_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CLIENT_SRC))
SERVER_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SERVER_SRC))

# Output
CLIENT_BIN = $(BIN_DIR)/client
SERVER_BIN = $(BIN_DIR)/server

# Target di default
all: $(CLIENT_BIN) $(SERVER_BIN)

# Compilazione client
$(CLIENT_BIN): $(CLIENT_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Compilazione server
$(SERVER_BIN): $(SERVER_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Regole di compilazione per gli oggetti
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Pulizia
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Ricompilazione totale
rebuild: clean all
