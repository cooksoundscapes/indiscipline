cmake_minimum_required(VERSION 3.5)
project(indiscipline
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
        src/lua-functions.cpp
        src/lua-runner.cpp
        src/cairo-wrapper.cpp
        src/audio-sink.cpp
        src/osc-server.cpp
        src/spi-display.cpp
        src/fb-display.cpp
        src/hardware/panel.cpp
        src/hardware/pcf-8574.cpp
        #src/hardware/gpio --> this is for the non daemon library, probably never going to be used
        src/hardware/gpio-daemon.cpp
        src/hardware/ssd1306-spi.cpp
)

option(USE_GPIO "use gpio" ON)
option(USE_SSD1306 "use OLED display (spi)" ON)
option(USE_FB "use direct framebuffer" OFF)

if (USE_GPIO)
    add_definitions(-DUSE_GPIO)
endif()
if (USE_SSD1306)
    add_definitions(-DUSE_SSD1306)
endif()
if (USE_FB)
    add_definitions(-DUSE_FB)
endif()

include_directories(indiscipline /usr/include/pango-1.0)
include_directories(indiscipline /usr/include/glib-2.0)
include_directories(indiscipline /usr/lib/glib-2.0/include)
include_directories(indiscipline /usr/include/harfbuzz)
include_directories(indiscipline /usr/include/cairo)

# Configuração do executável
add_executable(indiscipline ${SOURCES})

install(TARGETS indiscipline DESTINATION /usr/bin)
 
target_link_libraries(indiscipline
        jack
        i2c
        pigpio
        pigpiod_if2
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
