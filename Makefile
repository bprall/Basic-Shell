# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Source files
COMPONENTS_SRC = $(SRC_DIR)/executor.c $(SRC_DIR)/builtins.c $(SRC_DIR)/io.c $(SRC_DIR)/utils.c
MAIN_SRC = $(SRC_DIR)/main.c
SOURCES = $(COMPONENTS_SRC) $(MAIN_SRC)

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