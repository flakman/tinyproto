cmake_minimum_required (VERSION 3.5)

option(EXAMPLES "Build examples and tiny_loopback" OFF)
option(UNITTEST "Build unit tests" OFF)
option(CUSTOM "Do not use built-in HAL, but use Custom instead" OFF)
option(ENABLE_FD_LOGS "Enable full duplex protocol logs" OFF)
option(QT_HAL "Enable Qt hal" ON)
# set(LOG_LEVEL "0" CACHE STRING "Logging level option" FORCE)

# file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)
# file(GLOB_RECURSE HEADER_FILES src/*.h)

set(SOURCE_FILES
    src/TinyLightProtocol.h
    src/TinyLightProtocol.cpp
    src/TinyPacket.h

    src/proto/crc/tiny_crc.h
    src/proto/crc/tiny_crc.c

    src/proto/hdlc/low_level/hdlc_int.h
    src/proto/hdlc/low_level/hdlc.h
    src/proto/hdlc/low_level/hdlc.c

    src/proto/hdlc/high_level/hdlc.h
    src/proto/hdlc/high_level/hdlc.c

    src/proto/light/tiny_light.h
    src/proto/light/tiny_light.c

    src/hal/tiny_debug.h
    src/hal/tiny_list.h
    src/hal/tiny_list.c
    # src/hal/tiny_serial.h
    # src/hal/tiny_serial.c
    src/hal/tiny_types.h
    src/hal/tiny_types.c

    src/hal/tiny_types_cpp.cpp

    # src/hal/qt/qt_hal.h
    # src/hal/qt/qt_hal.inl
)

if (NOT DEFINED COMPONENT_DIR)

    project (tinyproto)

    include_directories(src)

    add_library(tinyproto STATIC ${SOURCE_FILES})

    if (EXAMPLES)
        add_subdirectory(examples/linux/loopback)
        add_subdirectory(examples/linux/hdlc_demo)
        add_subdirectory(examples/linux/hdlc_demo_multithread)
    endif()

    if (UNITTEST)
        add_subdirectory(unittest)
    endif()

else()

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS "src")

endif()

if (CUSTOM)
    add_definitions("-DTINY_CUSTOM_PLATFORM=1")
endif()
if (ENABLE_FD_LOGS)
    add_definitions("-DTINY_DEBUG=1")
    add_definitions("-DTINY_FD_DEBUG=1")
    if (LOG_LEVEL)
       add_definitions("-DTINY_LOG_LEVEL_DEFAULT=${LOG_LEVEL}")
    endif()
endif()

if (QT_HAL)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core;)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core;)

    target_link_libraries(tinyproto PRIVATE Qt${QT_VERSION_MAJOR}::Core)
endif()

add_definitions(-DTINY_STATIC_READ_BUF_SZ=4096)
add_definitions(-DTINY_STATIC_WRITE_BUF_SZ=4096)
