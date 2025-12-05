cmake_minimum_required(VERSION 3.13)

# 🧩 Import Pico SDK
include(pico_sdk_import.cmake)

project(qwerty_midi_pico C CXX ASM)
pico_sdk_init()

add_compile_definitions(CFG_TUD_HID=1 CFG_TUD_MIDI=1)

add_executable(qwerty_midi_pico
    main.c
    drivers/current/gpio_driver.c
    drivers/current/msqt32_shift_register.c
    drivers/current/velocity_matrix.c
    usb_descriptors.c
)

target_include_directories(qwerty_midi_pico PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/drivers/current
)

target_link_libraries(qwerty_midi_pico
    pico_stdlib
    tinyusb_device
    tinyusb_board
)

pico_add_extra_outputs(qwerty_midi_pico)