# default num of processes
n = 3
input-dir = input

CC = g++
CC_OPT = -std=c++14
CC_OPT += -g
CC_OPT += -O0

BIN_FILE = run-system
BIN_DIR = bin

all: build run

build: $(BIN_DIR) $(input-dir)
	$(CC) $(CC_OPT) src/*.cpp -o $(BIN_DIR)/$(BIN_FILE)

run:
	./$(BIN_DIR)/$(BIN_FILE) $n $(input-dir)

gen: $(input-dir)
	python3 gen-input.py $n $(input-dir)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(input-dir):
	mkdir -p $(input-dir)

clean:
	rm -rf $(BIN_DIR)
