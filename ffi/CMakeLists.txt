
message(STATUS "FFI is enabled")

add_library(mbgl-ffi SHARED
    src/main.cpp
    src/context.cpp
    )

target_include_directories(mbgl-ffi
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src/ffi
        PUBLIC
        ${PROJECT_SOURCE_DIR}/ffi/include
        )

target_link_options(mbgl-ffi PRIVATE
        -Wl,--build-id=sha1
        -Wl,--no-undefined
        -Wl,--gc-sections
        -Wl,--as-needed
        -Wl,-z,noexecstack
        -Wl,-z,relro -Wl,-z,now
        -v
        )

target_link_libraries(mbgl-ffi PUBLIC mbgl-core)

set_target_properties(mbgl-ffi PROPERTIES OUTPUT_NAME mbgl_ffi)

#
# LTO
#
cmake_policy(SET CMP0069 NEW)
include(CheckIPOSupported)
check_ipo_supported(
        RESULT IPO_SUPPORT_RESULT
        OUTPUT IPO_SUPPORT_OUTPUT
        LANGUAGES CXX
)
if (IPO_SUPPORT_RESULT)
    message(STATUS "IPO is supported")
    set_property(TARGET mbgl-ffi PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else ()
    message(STATUS "IPO is not supported: ${IPO_SUPPORT_OUTPUT}")
    set_property(TARGET mbgl-ffi PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
endif ()
