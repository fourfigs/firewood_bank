add_library(core STATIC
    core.cpp
    core.h
)

target_include_directories(core 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(core
    PUBLIC
        Qt6::Core
)

add_library(firewood::core ALIAS core)


