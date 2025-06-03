#!/bin/bash

set -e

SERVER_BIN=./bin/server
CLIENT_BIN=./bin/client
PORT=12345
KEY=0123456789abcdef
THREADS=2
MAX_CONN=5
PREFIX="test"
TEST_DIR=test_output
LOG_FILE="$TEST_DIR/test_log.txt"
GREEN="\033[0;32m"
RED="\033[0;31m"
NC="\033[0m"

mkdir -p "$TEST_DIR"
echo "Test start: $(date)" > "$LOG_FILE"

# Cleanup
cleanup() {
    echo "Interrupt, terminating server..."
    kill -SIGINT $SERVER_PID 2>/dev/null || true
    wait $SERVER_PID 2>/dev/null || true
    echo -e "${RED}Test canceled.${NC}"
    exit 1
}
trap cleanup INT TERM

run_test() {
    local test_name=$1
    local input_file=$2
    local expected_output=$3

    echo "Running test: $test_name" | tee -a "$LOG_FILE"
    cp "$input_file" "$TEST_DIR/original.txt"

    $CLIENT_BIN "$input_file" "$KEY" "$THREADS" 127.0.0.1 "$PORT" >> "$LOG_FILE" 2>&1

    sleep 1
    OUTPUT_FILE="${PREFIX}_output.txt"
    if cmp -s "$OUTPUT_FILE" "$expected_output"; then
        echo -e "${GREEN}✅ $test_name PASS${NC}" | tee -a "$LOG_FILE"
    else
        echo -e "${RED}❌ $test_name FAIL${NC}" | tee -a "$LOG_FILE"
        diff "$expected_output" "$OUTPUT_FILE" >> "$LOG_FILE"
    fi

    rm -f "$OUTPUT_FILE"
}

# Start the server
$SERVER_BIN "$THREADS" "$PREFIX" "$PORT" "$MAX_CONN" >> "$LOG_FILE" 2>&1 &
SERVER_PID=$!
sleep 1

### Test 1: Base case test
run_test "Simple text file" "test/test1.txt" "test/test1.txt"

### Test 2: Not aligned file with padding
run_test "File with padding" "test/test2_padding.txt" "test/test2_padding.txt"

### Test 3: Binary file test
run_test "Binary file" "test/test3.bin" "test/test3.bin"

# Stop the server
kill -SIGINT $SERVER_PID
wait $SERVER_PID 2>/dev/null || true
echo -e "\n${GREEN}Tutti i test completati.${NC}"
