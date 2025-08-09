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

EMCC_COMPILE_FLAGS = -gsource-map \
                     -O3 \

EMCC_FLAGS = -s ENVIRONMENT=web \
             -s MODULARIZE=1 \
             -s EXPORT_ES6=1 \
             -s ALLOW_MEMORY_GROWTH=1 \
             -s STANDALONE_WASM=0 \
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
	@$(RMDIR) "$(OBJ_DIR)"
	@echo Build successful.

clean:
	@echo Cleaning objects and binaries...
	@if exist "$(OBJ_DIR)" rmdir /S /Q "$(OBJ_DIR)" || echo "Directory $(OBJ_DIR) not found"
	@echo Clean complete.