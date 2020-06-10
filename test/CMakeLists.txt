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

add_subdirectory(arduino_mock)
get_property(ARDUINO_MOCK_INCLUDE_DIRS TARGET arduino_mock PROPERTY INCLUDE_DIRECTORIES)

# TODO(sl): Use gtest_discover_tests() ?
# https://cmake.org/cmake/help/git-master/module/GoogleTest.html
enable_testing()

set(SOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/../src/ayab)
set(LIBRARY_DIRECTORY ${PROJECT_SOURCE_DIR}/../libraries)
set(MOCKS_DIRECTORY ${PROJECT_SOURCE_DIR}/mocks)
set(STUBS_DIRECTORY ${PROJECT_SOURCE_DIR}/stubs)

set(SYSTEM_INCLUDES
    ${ARDUINO_MOCK_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    ${GMOCK_INCLUDE_DIRS}
)

set(COMMON_DEFINES
    ARDUINO=105
    AYAB_TESTS
    )

set(COMMON_FLAGS
    -Wall -Wextra -Wpedantic
    -Wno-vla
    -Werror
    -fprofile-arcs -ftest-coverage
    -g -Og
    )

set(COMMON_LINKER_FLAGS
    arduino_mock
    ${CMAKE_THREAD_LIBS_INIT}
    -lgcov
    )

function(add_component component)
    cmake_parse_arguments(
        ADD_COMPONENT
        ""
        ""
        "DEFINES;SOURCES;INCLUDES;SYSTEM_INCLUDES"
        ${ARGN}
    )
    set(EXE_NAME ${PROJECT_NAME}_${component}${ADD_COMPONENT_DEFINES})
    add_executable(${PROJECT_NAME}_${EXE_NAME}
        ${PROJECT_SOURCE_DIR}/test_all.cpp
        ${PROJECT_SOURCE_DIR}/test_${component}.cpp
        ${SOURCE_DIRECTORY}/${component}.cpp
        ${ADD_COMPONENT_SOURCES}
        )
    target_include_directories(${PROJECT_NAME}_${EXE_NAME}
        SYSTEM PRIVATE
        ${SYSTEM_INCLUDES}
        ${ADD_COMPONENT_SYSTEM_INCLUDES}
        ${MOCKS_DIRECTORY}/Arduino
        )
    target_include_directories(${PROJECT_NAME}_${EXE_NAME}
        PRIVATE
        ${SOURCE_DIRECTORY}
        ${ADD_COMPONENT_INCLUDES}
        )
    target_compile_definitions(${PROJECT_NAME}_${EXE_NAME}
        PRIVATE
        ${COMMON_DEFINES}
        ${ADD_COMPONENT_DEFINES}
        )
    target_compile_options(${PROJECT_NAME}_${EXE_NAME} PRIVATE
        ${COMMON_FLAGS}
        )
    target_link_libraries(${PROJECT_NAME}_${EXE_NAME}
        ${COMMON_LINKER_FLAGS}
        )
    add_dependencies(${PROJECT_NAME}_${EXE_NAME} arduino_mock)
    add_test(test_${EXE_NAME} ${PROJECT_NAME}_${EXE_NAME})
endfunction()

add_component(beeper)
add_component(encoders DEFINES KH910 SYSTEM_INCLUDES ${STUBS_DIRECTORY}/PacketSerial)
add_component(encoders DEFINES KH930 SYSTEM_INCLUDES ${STUBS_DIRECTORY}/PacketSerial)
add_component(solenoids
  DEFINES __AVR_ATmega168__
  SYSTEM_INCLUDES ${LIBRARY_DIRECTORY}/Adafruit_MCP23008
  SOURCES ${LIBRARY_DIRECTORY}/Adafruit_MCP23008/Adafruit_MCP23008.cpp
  )
add_component(solenoids
  DEFINES __AVR_ATmega2560__
  SYSTEM_INCLUDES ${MOCKS_DIRECTORY}/SoftI2CMaster
  SOURCES ${MOCKS_DIRECTORY}/SoftI2CMaster/soft_i2c_mock.cpp
  )
add_component(knitter
SYSTEM_INCLUDES
    ${MOCKS_DIRECTORY}/Solenoids
    ${MOCKS_DIRECTORY}/Beeper
    ${MOCKS_DIRECTORY}/Encoders
    ${MOCKS_DIRECTORY}/SerialEncoding
    ${STUBS_DIRECTORY}/PacketSerial
  SOURCES
    ${MOCKS_DIRECTORY}/Solenoids/solenoids_mock.cpp
    ${MOCKS_DIRECTORY}/Encoders/encoders_mock.cpp
    ${MOCKS_DIRECTORY}/Beeper/beeper_mock.cpp
    ${MOCKS_DIRECTORY}/SerialEncoding/serial_encoding_mock.cpp
  )
add_component(hw_test
  DEFINES AYAB_HW_TEST
  SYSTEM_INCLUDES
    ${MOCKS_DIRECTORY}/SoftI2CMaster
    ${MOCKS_DIRECTORY}/SerialCommand
    ${MOCKS_DIRECTORY}/Solenoids
    ${MOCKS_DIRECTORY}/Beeper
    ${LIBRARY_DIRECTORY}/SerialCommand
    ${STUBS_DIRECTORY}/PacketSerial
  SOURCES
    ${MOCKS_DIRECTORY}/Solenoids/solenoids_mock.cpp
    ${MOCKS_DIRECTORY}/Beeper/beeper_mock.cpp
    ${MOCKS_DIRECTORY}/SoftI2CMaster/soft_i2c_mock.cpp
    ${MOCKS_DIRECTORY}/SerialCommand/serial_command_mock.cpp
  )
add_component(serial_encoding
  INCLUDES ${MOCKS_DIRECTORY}/Knitter
  SOURCES ${MOCKS_DIRECTORY}/Knitter/knitter_mock.cpp
  SYSTEM_INCLUDES ${LIBRARY_DIRECTORY}/PacketSerial/src
  DEFINES AYAB_ENABLE_CRC
  )

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
    ${LIBRARY_DIRECTORY}/Adafruit_MCP23008/Adafruit_MCP23008.cpp
    )
target_include_directories(${PROJECT_NAME}_integration
    SYSTEM PRIVATE
    ${SYSTEM_INCLUDES}
    ${LIBRARY_DIRECTORY}/PacketSerial/src
    ${LIBRARY_DIRECTORY}/Adafruit_MCP23008
    )
target_include_directories(${PROJECT_NAME}_integration
    PRIVATE
    ${SOURCE_DIRECTORY}
    ${PROJECT_SOURCE_DIR}/mocks/Arduino
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
    ${LIBRARY_DIRECTORY}/Adafruit_MCP23008/Adafruit_MCP23008.cpp
    ${LIBRARY_DIRECTORY}/SerialCommand/SerialCommand.cpp
    )
target_include_directories(${PROJECT_NAME}_hw_test_integration
    SYSTEM PRIVATE
    ${SYSTEM_INCLUDES}
    ${LIBRARY_DIRECTORY}/Adafruit_MCP23008
    ${LIBRARY_DIRECTORY}/PacketSerial/src
    ${LIBRARY_DIRECTORY}/SerialCommand
    )
target_include_directories(${PROJECT_NAME}_hw_test_integration
    PRIVATE
    ${SOURCE_DIRECTORY}
    ${PROJECT_SOURCE_DIR}/mocks/Arduino
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
