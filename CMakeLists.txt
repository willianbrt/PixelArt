cmake_minimum_required(VERSION 3.15)
project(PixelEditor LANGUAGES C CXX)

# Diretórios
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src/view/painting")
set(BUILD_DIR "${SRC_DIR}/build")

# Arquivo final
set(TARGET PixelEditor)

# Buscar todos os .cpp
file(GLOB_RECURSE SOURCES
    "${SRC_DIR}/lib/helpers/*/*.cpp"
    "${SRC_DIR}/lib/interfaces/*/*.cpp"
    "${SRC_DIR}/lib/graphics/*/*.cpp"
    "${SRC_DIR}/lib/observers/*/*.cpp"
    "${SRC_DIR}/lib/strategy/*/*.cpp"
    "${SRC_DIR}/lib/rasterize/*/*.cpp"
    "${SRC_DIR}/lib/objects/*/*.cpp"
    "${SRC_DIR}/lib/commands/*/*.cpp"
    "${SRC_DIR}/lib/context/*/*.cpp"
    "${SRC_DIR}/lib/app/dto/*/*.cpp"
    "${SRC_DIR}/lib/app/ViewModel/*/*.cpp"
    "${SRC_DIR}/lib/app/*/*.cpp"
    "${SRC_DIR}/lib/gpu/*/*.cpp"
    "${SRC_DIR}/lib/app/main.cpp"
)

# Criar executável (Emscripten gera .js + .wasm)
add_executable(${TARGET} ${SOURCES})

# Diretório de saída
set_target_properties(${TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}"
    SUFFIX ".js"
)

# Flags de compilação
target_compile_options(${TARGET} PRIVATE
    -O3
    -gsource-map
)

# Flags de link (Emscripten)
target_link_options(${TARGET} PRIVATE
    -O3
    -gsource-map

    -sENVIRONMENT=web
    -sMODULARIZE=1
    -sEXPORT_ES6=1
    -sALLOW_MEMORY_GROWTH=1
    -sSTANDALONE_WASM=0

    -sASSERTIONS
    -sNO_DISABLE_EXCEPTION_CATCHING

    -sFULL_ES3=1
    -sUSE_WEBGL2=1
    -sMIN_WEBGL_VERSION=2
    -sMAX_WEBGL_VERSION=2
    -sUSE_GLFW=3

    --use-port=emdawnwebgpu
    "SHELL:--js-library=${SRC_DIR}/js/library.js"

    -lembind
)

# (Opcional) incluir diretórios se necessário
target_include_directories(${TARGET} PRIVATE
    "${SRC_DIR}/lib"
)