add_executable(${PROJECT_NAME}
        main.c
        )

# pull in common dependencies
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    #RadioLib
    #hardware_adc
    #hardware_spi
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})
