MAKEFLAGS += --always-make --debug=v
ifeq ($(OS),Windows_NT)
    CC := mingw32-g++
    RM := del /q
    RMDIR := rmdir /s /q
    MKDIR := if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
else
    CC := emcc
    RM := rm -f
    RMDIR := rm -rf
    MKDIR := mkdir -p $(dir $@)
endif
SRC_DIR = ./
BUILD_DIR = ../build/graphics
OBJ_DIR = $(BUILD_DIR)/obj

TARGET = PixelEditor.js
TARGET_DIR = $(BUILD_DIR)/$(TARGET)

SOURCES = $(wildcard $(SRC_DIR)/helpers/*/*.cpp) \
			$(wildcard $(SRC_DIR)/interfaces/*/*.cpp) \
			$(wildcard $(SRC_DIR)/command/*/*.cpp) \
			$(wildcard $(SRC_DIR)/observers/*/*.cpp) \
			$(wildcard $(SRC_DIR)/strategy/*/*.cpp) \
			$(wildcard $(SRC_DIR)/objects/*/*.cpp) \
			$(wildcard $(SRC_DIR)/main/main.cpp) \
			$(wildcard $(SRC_DIR)/*.cpp)

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

EMCC_COMPILE_FLAGS = -gsource-map \
                     -O3 \

EMCC_FLAGS = -s ENVIRONMENT=web \
             -s MODULARIZE=1 \
             -s EXPORT_ES6=1 \
             -s IMPORTED_MEMORY \
             -s ALLOW_MEMORY_GROWTH=1 \
             -s INITIAL_MEMORY=256MB -s MAXIMUM_MEMORY=2564MB \
             -s STANDALONE_WASM=1 \
             -s ERROR_ON_UNDEFINED_SYMBOLS=1 \
			-s NO_DISABLE_EXCEPTION_CATCHING=1 \
			-s USE_WEBGL2=1 \
			-s FULL_ES2 \
			-lembind \
			--js-library='./../js/library.js'
			
all: $(TARGET_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo Compiling $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	@emcc $(EMCC_COMPILE_FLAGS) -c $< -o $@

$(TARGET_DIR): $(OBJECTS)
	@echo Linking $(TARGET)...
	@emcc $(EMCC_FLAGS) $(OBJECTS) -o $@
	@echo Removing object files...
	@$(RMDIR) "$(subst /,\,$(OBJ_DIR))"
	@echo Build successful.

clean:
	@echo Cleaning objects and binaries...
	@if exist "$(OBJ_DIR)" rmdir /S /Q "$(OBJ_DIR)" || echo "Directory $(OBJ_DIR) not found"
	@echo Clean complete.