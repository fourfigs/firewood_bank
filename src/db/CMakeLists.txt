add_library(db STATIC
    database.cpp
    database.h
)

target_include_directories(db 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(db
    PUBLIC
        firewood::core
        Qt6::Sql
        Qt6::Core
)

add_library(firewood::db ALIAS db)


