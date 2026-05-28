cmake_minimum_required(VERSION 3.15)
project(PixelEditor LANGUAGES C CXX)

# Diretórios
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src/view/painting")
set(BUILD_DIR "${SRC_DIR}/build")
set(OBJ_DIR "${BUILD_DIR}/obj")


file(MAKE_DIRECTORY ${OBJ_DIR})

# Arquivo final
set(TARGET PixelEditor)

# Buscar todos os .cpp
file(GLOB_RECURSE SOURCES
    ${SRC_DIR}/lib/helpers/*/*.cpp
    ${SRC_DIR}/lib/interfaces/*/*.cpp
    ${SRC_DIR}/lib/graphics/*/*.cpp
    ${SRC_DIR}/lib/observers/*/*.cpp
    ${SRC_DIR}/lib/strategy/*/*.cpp
    ${SRC_DIR}/lib/objects/*/*.cpp
    ${SRC_DIR}/lib/commands/*/*.cpp
    ${SRC_DIR}/lib/context/*/*.cpp
    ${SRC_DIR}/lib/app/dto/*/*.cpp
    ${SRC_DIR}/lib/app/ViewModel/*/*.cpp
    ${SRC_DIR}/lib/app/*/*.cpp
    ${SRC_DIR}/lib/gpu/*/*.cpp
    ${SRC_DIR}/lib/app/main.cpp
)

# Compilar cada fonte em objeto
set(OBJECTS "")
foreach(src ${SOURCES})
    get_filename_component(fname ${src} NAME_WE)
    set(obj ${OBJ_DIR}/${fname}.o)
    add_custom_command(
        OUTPUT ${obj}
        COMMAND ${CMAKE_CXX_COMPILER} -c ${src} -o ${obj}
            -O3 -gsource-map -s USE_SDL=2
        DEPENDS ${src}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    list(APPEND OBJECTS ${obj})
endforeach()

# Agora, linkar todos os objetos no bundle WASM + JS
add_custom_command(
    OUTPUT ${BUILD_DIR}/${TARGET}.js ${BUILD_DIR}/${TARGET}.wasm
    COMMAND ${CMAKE_CXX_COMPILER} ${OBJECTS} -o ${BUILD_DIR}/${TARGET}.js
        -O3 -gsource-map
        -s ENVIRONMENT=web
        -s MODULARIZE=1
        -s EXPORT_ES6=1
        -s ALLOW_MEMORY_GROWTH=1
        -s STANDALONE_WASM=0
        -lembind
        -s ASSERTIONS
        -s NO_DISABLE_EXCEPTION_CATCHING
         
        -s FULL_ES3=1
        -s USE_WEBGL2=1
        -s MIN_WEBGL_VERSION=2
        -s MAX_WEBGL_VERSION=2 
        -s USE_GLFW=3
        --use-port=emdawnwebgpu
        --js-library=${SRC_DIR}/js/library.js
    DEPENDS ${OBJECTS}
)

# Alvo final
add_custom_target(${TARGET}_wasm ALL
    DEPENDS ${BUILD_DIR}/${TARGET}.js ${BUILD_DIR}/${TARGET}.wasm
)

# Alias para permitir --target ALL no VSCode
add_custom_target(ALL DEPENDS ${TARGET}_wasm)
