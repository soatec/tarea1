TARGET := tarea1

SOURCE_DIR  := ./src
INCLUDE_DIR := ./include
BUILD_DIR   := ./build

CC        := gcc
CFLAGS    := -Wall
LDFLAGS   := -lm -lpthread
INC_FLAGS := -I$(INCLUDE_DIR)

SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
OBJECTS = $(SOURCES:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)


.PHONY: all
all: $(BUILD_DIR)/$(TARGET)


$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@


$(BUILD_DIR):
	mkdir -p $@


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
