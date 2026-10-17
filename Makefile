
CC = gcc
CFLAGS = -Wall -Wextra
PTHREAD_FLAGS = -pthread
BIN = bin

.PHONY: all clean

all: $(BIN)/readers_writers \
     $(BIN)/co1_system_info \
     $(BIN)/co2_process \
     $(BIN)/co3_ipc \
     $(BIN)/co4_memory \
     $(BIN)/co5_file_io \
     $(BIN)/co6_concurrency

$(BIN):
	mkdir -p $(BIN)

$(BIN)/readers_writers: src/main.c | $(BIN)
	$(CC) $(CFLAGS) $(PTHREAD_FLAGS) $< -o $@

$(BIN)/co1_system_info: src/co1_system_info.c | $(BIN)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/co2_process: src/co2_process.c | $(BIN)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/co3_ipc: src/co3_ipc.c | $(BIN)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/co4_memory: src/co4_memory.c | $(BIN)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/co5_file_io: src/co5_file_io.c | $(BIN)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/co6_concurrency: src/co6_concurrency.c | $(BIN)
	$(CC) $(CFLAGS) $(PTHREAD_FLAGS) $< -o $@

clean:
	rm -rf $(BIN)


.PHONY: test

test: all
	@bash tests/run_tests.sh
