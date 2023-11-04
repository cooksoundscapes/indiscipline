cmake_minimum_required(VERSION 3.0)
project(hw-control-sdl
        VERSION 1.0
        DESCRIPTION "minimalistic front end & hardware interface"
        LANGUAGES CXX
)

# Configuração das variáveis
set(CMAKE_CXX_STANDARD 17)  # Versão do C++ utilizada
set(CMAKE_CXX_STANDARD_REQUIRED True)


#debugging
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

set(SOURCES
        src/main.cpp
        src/window.cpp
        src/lua-runner.cpp
        src/cairo-wrapper.cpp
        src/audio-sink.cpp
        src/osc-server.cpp
        src/hardware/pcf-8574.cpp
        src/hardware/gpio.cpp
)

include_directories(hw-control-sdl /usr/include/pango-1.0)
include_directories(hw-control-sdl /usr/include/glib-2.0)
include_directories(hw-control-sdl /usr/lib/glib-2.0/include)
include_directories(hw-control-sdl /usr/include/harfbuzz)
include_directories(hw-control-sdl /usr/include/cairo)

# Configuração do executável
add_executable(hw-control-sdl ${SOURCES})

# Configuração de bibliotecas adicionais (opcional)
target_link_libraries(hw-control-sdl
        jack
        i2c
        pigpio
        rt
        lo
        lua
        SDL2
        SDL2_image
        SDL2_ttf
        cairo
        pango-1.0
        glib-2.0
        gobject-2.0
        pangocairo-1.0
)
