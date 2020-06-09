cmake_minimum_required(VERSION 3.7.2)
project(ayab_test)

set(CMAKE_CXX_STANDARD 11 CACHE STRING "Set the C++ standard to be used for compiling")

option(CLANG_TIDY_ENABLED "Check with Clang-Tidy" OFF)
option(CLANG_TIDY_FIX "Perform fixes for Clang-Tidy" OFF)
find_program(
    CLANG_TIDY_EXE
    NAMES "clang-tidy"
    DOC "Path to clang-tidy executable"
)

if(CLANG_TIDY_EXE AND CLANG_TIDY_ENABLED)
    if(CLANG_TIDY_FIX)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}" "-fix")
    else()
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

find_package(Threads REQUIRED)

add_subdirectory(arduino_mock)

set(SOURCE_DIRECTORY
    ${PROJECT_SOURCE_DIR}/../src/ayab
    )
set(LIBRARY_DIRECTORY
    ${PROJECT_SOURCE_DIR}/../libraries
    )

set(SYSTEM_INCLUDES
    ${ARDUINO_MOCK_INCLUDE_DIRS}
    ${ARDUINO_MOCK_LIBS_DIR}/lib/gtest/gtest/src/gtest/googletest/include
    ${ARDUINO_MOCK_LIBS_DIR}/lib/gtest/gtest/src/gtest/googlemock/include
    ${LIBRARY_DIRECTORY}/PacketSerial/src
    ${LIBRARY_DIRECTORY}/Adafruit_MCP23008
    ${LIBRARY_DIRECTORY}/SerialCommand
)
# Includes arduino-mock files directly inplace of original Arduino headers.
set(COMMON_INCLUDES
    ${PROJECT_SOURCE_DIR}/mocks
    ${PROJECT_SOURCE_DIR}/mocks/arduino
    ${PROJECT_SOURCE_DIR}/mocks/hw_test
    ${SOURCE_DIRECTORY}
    )
set(COMMON_SOURCES
    ${PROJECT_SOURCE_DIR}/test_all.cpp

    ${SOURCE_DIRECTORY}/encoders.cpp
    ${PROJECT_SOURCE_DIR}/test_encoders.cpp

    ${SOURCE_DIRECTORY}/beeper.cpp
    ${PROJECT_SOURCE_DIR}/test_beeper.cpp

    ${SOURCE_DIRECTORY}/solenoids.cpp
    ${PROJECT_SOURCE_DIR}/test_solenoids.cpp

    ${SOURCE_DIRECTORY}/serial_encoding.cpp
    ${PROJECT_SOURCE_DIR}/test_serial_encoding.cpp
    ${PROJECT_SOURCE_DIR}/mocks/serial_encoding/knitter_mock.cpp

    ${SOURCE_DIRECTORY}/hw_test.cpp
    ${PROJECT_SOURCE_DIR}/test_hw_test.cpp
    ${PROJECT_SOURCE_DIR}/mocks/hw_test/serial_command_mock.cpp
    ${PROJECT_SOURCE_DIR}/mocks/hw_test/soft_i2c_mock.cpp
    )
set(COMMON_DEFINES
    ARDUINO=105
    AYAB_QUIET
    AYAB_TESTS
    AYAB_ENABLE_CRC
    )
set(COMMON_FLAGS
    -Wall -Wextra -Wpedantic
    -Wno-vla
    -Werror
    -fprofile-arcs -ftest-coverage
    -g -Og
    )
set(COMMON_LINKER_FLAGS
    ${ARDUINO_MOCK_LIBS_DIR}/lib/gtest/gtest/src/gtest-build/googlemock/gtest/libgtest.a
    # For some reason the order of libarduinomock and libgmock matter, keep them this way.
    ${ARDUINO_MOCK_LIBS_DIR}/dist/lib/libarduino_mock.a
    ${ARDUINO_MOCK_LIBS_DIR}/lib/gtest/gtest/src/gtest-build/googlemock/libgmock.a
    ${CMAKE_THREAD_LIBS_INIT}
    -lgcov
    )
set(HARD_I2C_LIB
    ${LIBRARY_DIRECTORY}/Adafruit_MCP23008/Adafruit_MCP23008.cpp
    )
set(SOFT_I2C_LIB
    ${LIBRARY_DIRECTORY}/SoftI2CMaster/SoftI2CMaster.cpp
    )



# TODO(sl): Use gtest_discover_tests() ?
# https://cmake.org/cmake/help/git-master/module/GoogleTest.html
enable_testing()

function(add_machine machine board)
  if(${board} STREQUAL "Uno")
        set(processor
            __AVR_ATmega168__
            )
        set(I2C_LIB
            ${HARD_I2C_LIB}
            )
    else()
        set(processor
            __AVR_ATmega2560__
            )
        set(I2C_LIB
            )
    endif()
    add_executable(${PROJECT_NAME}_${machine}_${board}
        ${COMMON_SOURCES}
        # External libraries
        ${I2C_LIB}
        )
    target_include_directories(${PROJECT_NAME}_${machine}_${board}
        SYSTEM PRIVATE
        ${SYSTEM_INCLUDES}
        )
    target_include_directories(${PROJECT_NAME}_${machine}_${board}
        PRIVATE
        ${COMMON_INCLUDES}
        )
    target_compile_definitions(${PROJECT_NAME}_${machine}_${board}
        PRIVATE
        ${COMMON_DEFINES}
        AYAB_HW_TEST
        ${machine}
        ${processor}
        )
    target_compile_options(${PROJECT_NAME}_${machine}_${board} PRIVATE
        ${COMMON_FLAGS}
        )
    target_link_libraries(${PROJECT_NAME}_${machine}_${board}
        ${COMMON_LINKER_FLAGS}
        )
    add_dependencies(${PROJECT_NAME}_${machine}_${board} arduino_mock)
    add_test(test_${machine}_${board} ${PROJECT_NAME}_${machine}_${board})
endfunction()

add_machine(KH930 Uno)
add_machine(KH910 Uno)
add_machine(KH930 Mega)
add_machine(KH910 Mega)

# Knitter (device independent)
add_executable(${PROJECT_NAME}_knitter
    ${PROJECT_SOURCE_DIR}/test_all.cpp
    ${SOURCE_DIRECTORY}/knitter.cpp
    ${PROJECT_SOURCE_DIR}/test_knitter.cpp
    ${PROJECT_SOURCE_DIR}/mocks/knitter/solenoids_mock.cpp
    ${PROJECT_SOURCE_DIR}/mocks/knitter/encoders_mock.cpp
    ${PROJECT_SOURCE_DIR}/mocks/knitter/beeper_mock.cpp
    ${PROJECT_SOURCE_DIR}/mocks/knitter/serial_encoding_mock.cpp
    ${SOFT_I2C_LIB}
    )
target_include_directories(${PROJECT_NAME}_knitter
    SYSTEM PRIVATE
    ${SYSTEM_INCLUDES}
    )
target_include_directories(${PROJECT_NAME}_knitter
    PRIVATE
    ${COMMON_INCLUDES}
    )
target_compile_definitions(${PROJECT_NAME}_knitter
    PRIVATE
    ${COMMON_DEFINES}
    KH930 # should not matter which one is chosen for knitter
    __AVR_ATmega2560__ # should not matter which one is chosen for knitter
    )
target_compile_options(${PROJECT_NAME}_knitter PRIVATE
    ${COMMON_FLAGS}
    )
target_link_libraries(${PROJECT_NAME}_knitter
    ${COMMON_LINKER_FLAGS}
    )
add_dependencies(${PROJECT_NAME}_knitter arduino_mock)
add_test(test_knitter ${PROJECT_NAME}_knitter)

# Integration tests (only mock the arduino itself)
add_executable(${PROJECT_NAME}_integration
    ${PROJECT_SOURCE_DIR}/test_all.cpp
    ${PROJECT_SOURCE_DIR}/test_integration.cpp
    ${SOURCE_DIRECTORY}/main.cpp
    ${SOURCE_DIRECTORY}/knitter.cpp
    ${SOURCE_DIRECTORY}/beeper.cpp
    ${SOURCE_DIRECTORY}/solenoids.cpp
    ${SOURCE_DIRECTORY}/encoders.cpp
    ${SOURCE_DIRECTORY}/serial_encoding.cpp
    ${HARD_I2C_LIB}
    )
target_include_directories(${PROJECT_NAME}_integration
    SYSTEM PRIVATE
    ${SYSTEM_INCLUDES}
    )
target_include_directories(${PROJECT_NAME}_integration
    PRIVATE
    ${SOURCE_DIRECTORY}
    ${PROJECT_SOURCE_DIR}/mocks/arduino
)
target_compile_definitions(${PROJECT_NAME}_integration
    PRIVATE
    ${COMMON_DEFINES}
    KH910
    __AVR_ATmega328P__
)
target_compile_options(${PROJECT_NAME}_integration PRIVATE
    ${COMMON_FLAGS}
)
target_link_libraries(${PROJECT_NAME}_integration
    ${COMMON_LINKER_FLAGS}
)
add_dependencies(${PROJECT_NAME}_integration arduino_mock)
add_test(test_integration ${PROJECT_NAME}_integration)

# Integration of hw tests (only mock the arduino itself)
add_executable(${PROJECT_NAME}_hw_test_integration
    ${PROJECT_SOURCE_DIR}/test_all.cpp
    ${PROJECT_SOURCE_DIR}/test_hw_test_integration.cpp
    ${SOURCE_DIRECTORY}/main.cpp
    ${SOURCE_DIRECTORY}/hw_test.cpp
    ${SOURCE_DIRECTORY}/knitter.cpp
    ${SOURCE_DIRECTORY}/beeper.cpp
    ${SOURCE_DIRECTORY}/solenoids.cpp
    ${SOURCE_DIRECTORY}/encoders.cpp
    ${SOURCE_DIRECTORY}/serial_encoding.cpp
    ${HARD_I2C_LIB}
    ${LIBRARY_DIRECTORY}/SerialCommand/SerialCommand.cpp
    )
target_include_directories(${PROJECT_NAME}_hw_test_integration
    SYSTEM PRIVATE
    ${SYSTEM_INCLUDES}
    )
target_include_directories(${PROJECT_NAME}_hw_test_integration
    PRIVATE
    ${SOURCE_DIRECTORY}
    ${PROJECT_SOURCE_DIR}/mocks/arduino
)
target_compile_definitions(${PROJECT_NAME}_hw_test_integration
    PRIVATE
    ${COMMON_DEFINES}
    KH910
    __AVR_ATmega328P__
    AYAB_HW_TEST
)
target_compile_options(${PROJECT_NAME}_hw_test_integration PRIVATE
    ${COMMON_FLAGS}
)
target_link_libraries(${PROJECT_NAME}_hw_test_integration
    ${COMMON_LINKER_FLAGS}
)
add_dependencies(${PROJECT_NAME}_hw_test_integration arduino_mock)
add_test(test_hw_test_integration ${PROJECT_NAME}_hw_test_integration)
