if [[ ! -e "./build" ]]; then
    # No build folder? Then create it
    # and configure the build
    cmake -S . -B build/ -D "CMAKE_C_COMPILER:FILEPATH=$(which arm-none-eabi-gcc)" -D CMAKE_BUILD_TYPE:STRING=Release
    err=$?
fi

# Build the app
cmake --build build
err=$?