# main/CMakeLists.txt

add_executable(pico_emb
    main.c
    tft_lcd_ili9341/ili9341/ili9341.c
    tft_lcd_ili9341/gfx/gfx.c
)

target_include_directories(pico_emb PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}                                   # <raiz>/main
    ${CMAKE_CURRENT_LIST_DIR}/tft_lcd_ili9341/ili9341           # headers ili9341
    ${CMAKE_CURRENT_LIST_DIR}/tft_lcd_ili9341/gfx               # headers gfx
)

target_link_libraries(pico_emb
    pico_stdlib
    hardware_spi
    hardware_adc
    hardware_dma
    freertos
)

pico_enable_stdio_uart(pico_emb 1)
pico_enable_stdio_usb(pico_emb 0)
pico_add_extra_outputs(pico_emb)
