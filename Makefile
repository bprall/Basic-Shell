# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
BUILTINS_DIR = $(SRC_DIR)/builtins
EXECUTOR_DIR = $(SRC_DIR)/executor

# Source files
SRC_FILES = $(SRC_DIR)/io.c $(SRC_DIR)/utils.c

# Include all .c files from src/builtins
BUILTINS_SRC = $(wildcard $(BUILTINS_DIR)/*.c)

# Include all .c files from src/executor
EXECUTOR_SRC = $(wildcard $(EXECUTOR_DIR)/*.c)

# All source files
SOURCES = $(SRC_FILES) $(BUILTINS_SRC) $(EXECUTOR_SRC) $(SRC_DIR)/main.c

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SOURCES:.c=.o))

# Targets
all: $(BUILD_DIR)/main

$(BUILD_DIR)/main: $(OBJECTS)
	gcc -o $@ $(OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)  # Create the directory if it doesn't exist
	gcc -c $< -o $@ -I$(INCLUDE_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean