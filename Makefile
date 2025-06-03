# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -Iinclude

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Common source files
COMMON_SRC = $(wildcard $(SRC_DIR)/common/*.c)

CLIENT_SRC = $(wildcard $(SRC_DIR)/client/*.c) $(COMMON_SRC)
SERVER_SRC = $(wildcard $(SRC_DIR)/server/*.c) $(COMMON_SRC)

CLIENT_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CLIENT_SRC))
SERVER_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SERVER_SRC))

CLIENT_BIN = $(BIN_DIR)/client
SERVER_BIN = $(BIN_DIR)/server

# Target to build both client and server
all: $(CLIENT_BIN) $(SERVER_BIN)

# Client
$(CLIENT_BIN): $(CLIENT_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Server
$(SERVER_BIN): $(SERVER_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Object files compilation rule
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

rebuild: clean all
