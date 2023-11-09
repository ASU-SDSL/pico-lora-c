add_executable(${PROJECT_NAME}
        main.cpp
        )

# pull in common dependencies
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    RadioLib
    hardware_adc
    hardware_spi
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})
