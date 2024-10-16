SRC_DIR := src
INCLUDE_DIR := include
BIN_DIR := bin
LIB_DIR := lib

CC := gcc
CFLAGS := -Wall $(shell find $(INCLUDE_DIR) -type d -exec echo -I{} \;)
LDFLAGS := -L$(LIB_DIR) -lm

SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC_FILES))

# Detect operating system (Windows vs Linux)
ifeq ($(OS), Windows_NT)
    TARGET := $(BIN_DIR)/my_program.exe
else
    TARGET := $(BIN_DIR)/my_program
endif

# Default target
all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) $(LDFLAGS) -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN_DIR)/*.o $(TARGET)
	find $(BIN_DIR) -type d -empty -delete