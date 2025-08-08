add_library(quirc STATIC
    quirc/lib/decode.c
    quirc/lib/identify.c
    quirc/lib/quirc.c
    quirc/lib/version_db.c
)
set_target_properties(quirc PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(quirc PUBLIC quirc/lib)

set(CMAKE_CXX_STANDARD 17)
set(WITH_BINARY ON)

if (STATIC)
    set(WITH_STATIC ON)
else()
    set(WITH_STATIC OFF)
endif()

add_subdirectory("i2pd/build")

set_property(TARGET i2pd PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
