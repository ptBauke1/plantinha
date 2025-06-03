# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(proj_pico C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Adiciona o executável (main.cpp na raiz do projeto)
add_executable(${PROJECT_NAME}
    main.cpp
    src/ssd1306/SSD1306_OLED.cpp
    src/ssd1306/SSD1306_OLED_graphics.cpp
    src/ssd1306/SSD1306_OLED_font.cpp
    src/ssd1306/SSD1306_OLED_Print.cpp
)

# Adiciona o diretório 'include' ao caminho de busca dos headers
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    # Adicione outros diretórios de include aqui, se houver
)

# Link com as bibliotecas do Pico SDK
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_i2c
    hardware_adc
    # A biblioteca matemática (m) geralmente é vinculada automaticamente com o C++ no Pico SDK.
)

# Outras configurações do Pico SDK (gerar .uf2, etc.)
pico_add_extra_outputs(${PROJECT_NAME})

# Habilitar saida USB e/ou UART para o printf
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
