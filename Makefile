# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Source files
ZIP_SRC = $(SRC_DIR)/zip/zip.c $(SRC_DIR)/zip/unzip.c
UTILS_SRC = $(SRC_DIR)/utils/zip_utils.c
SIMPLE_SHELL_SRC = $(SRC_DIR)/simple_shell.c
SOURCES = $(ZIP_SRC) $(UTILS_SRC) $(SIMPLE_SHELL_SRC)

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SOURCES:.c=.o))

# Targets
all: $(BUILD_DIR)/simple_shell

$(BUILD_DIR)/simple_shell: $(OBJECTS)
	gcc -o $@ $(OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)  # Create the directory if it doesn't exist
	gcc -c $< -o $@ -I$(INCLUDE_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
