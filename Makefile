CC := gcc
SRC_DIR := src
OBJ_DIR := build/objs
BIN_DIR := build
IMG_DIR := images
IMC_DIR := build/images
EXECUTABLE := $(BIN_DIR)/nikopetsim

SRCS := $(shell find $(SRC_DIR) -type f -name "*.c")
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
IMGS := $(shell find $(IMG_DIR) -type f -name "*.png")
IMCS := $(patsubst $(IMG_DIR)/%.png,$(IMC_DIR)/%.c,$(IMGS))
CFLAGS = -Wall -g -I src -I build/images
LDFLAGS :=
LIBS :=

ifeq ($(OS),Windows_NT)
	CFLAGS += -DWINDOWS
	LIBS += -static $(shell pkg-config --libs --static sdl2) -lm
else
	LIBS += -lSDL2 -lSDL2main -lm
endif

.PHONY: all clean mkimg compile

all:
	@make mkimg --silent
	@make compile --silent

mkimg: $(IMCS)

$(IMC_DIR)/%.c: $(IMG_DIR)/%.png
	@printf "\033[1m\033[32mConverting \033[36m$< \033[32m-> \033[36m$@\033[0m\n"
	@mkdir -p $(dir $@)
	@hexdump -v -e '1/1 "0x%X,"' $< > $@

compile: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	@printf "\033[1m\033[32mLinking \033[36m$(EXECUTABLE)\033[0m\n"
	@mkdir -p $(BIN_DIR)
	@$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@printf "\033[1m\033[32mCompiling \033[36m$< \033[32m-> \033[36m$@\033[0m\n"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)

-include $(OBJS:.o=.d)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< -o $@

-include $(OBJS:.o=.d)