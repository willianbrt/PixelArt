MAKEFLAGS += --always-make --debug=v

CC := emcc
RM := del /q
# Remove diretórios usando cmd, ignora erro com 'exit 0'
RMDIR = if exist "$(subst /,\\,$(1))" rmdir /s /q "$(subst /,\\,$(1))" || exit 0
# Cria diretórios, remove barra final para cmd aceitar
MKDIR = if not exist "$(subst /,\\,$(patsubst %\,%,$(subst /,\\,$(dir $@))))" mkdir "$(subst /,\\,$(patsubst %\,%,$(subst /,\\,$(dir $@))))"
IGNORE_ERR = rem

SRC_DIR = ./
BUILD_DIR = ../build/graphics
OBJ_DIR = $(BUILD_DIR)/obj

TARGET = PixelEditor.js
TARGET_DIR = $(BUILD_DIR)/$(TARGET)

SOURCES = 	$(wildcard $(SRC_DIR)/helpers/*/*.cpp) \
			$(wildcard $(SRC_DIR)/interfaces/*/*.cpp) \
		 	$(wildcard $(SRC_DIR)/graphics/*/*.cpp) \
			$(wildcard $(SRC_DIR)/tools/*.cpp) \
			$(wildcard $(SRC_DIR)/command/*/*.cpp) \
			$(wildcard $(SRC_DIR)/observers/*/*.cpp) \
			$(wildcard $(SRC_DIR)/strategy/*/*.cpp) \
			$(wildcard $(SRC_DIR)/objects/*/*.cpp) \
			$(wildcard $(SRC_DIR)/main/main.cpp) \
			$(wildcard $(SRC_DIR)/*.cpp)

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

EMCC_COMPILE_FLAGS = -gsource-map -O3

EMCC_FLAGS = -s ENVIRONMENT=web \
             -s MODULARIZE=1 \
             -s EXPORT_ES6=1 \
             -s ALLOW_MEMORY_GROWTH=1 \
             -s STANDALONE_WASM=0 \
			 -lembind \
			 --js-library='./../js/library.js'

all: clean $(TARGET_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo Compiling $<...
	@$(MKDIR)
	@$(RM) "$(subst /,\\,$@)" 2>nul || $(IGNORE_ERR)
	@$(CC) $(EMCC_COMPILE_FLAGS) -c $< -o $@

$(TARGET_DIR): $(OBJECTS)
	@echo Linking $(TARGET)...
	@$(CC) $(EMCC_FLAGS) $(OBJECTS) -o $@
	@echo Build successful.

clean:
	@echo Cleaning objects and binaries...
	$(call RMDIR,$(OBJ_DIR))
