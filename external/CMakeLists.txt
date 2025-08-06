add_library(quirc STATIC
    quirc/lib/decode.c
    quirc/lib/identify.c
    quirc/lib/quirc.c
    quirc/lib/version_db.c
)
set_target_properties(quirc PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(quirc PUBLIC quirc/lib)

set(CMAKE_CXX_STANDARD 17)
set(WITH_BINARY OFF)
set(WITH_STATIC OFF)
add_subdirectory("i2pd/build")
