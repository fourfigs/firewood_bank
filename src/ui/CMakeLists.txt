add_library(ui STATIC
    MainWindow.cpp
    MainWindow.h
    LoginDialog.cpp
    LoginDialog.h
    DashboardWidget.cpp
    DashboardWidget.h
    ClientDialog.cpp
    ClientDialog.h
)

target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ui
    PUBLIC
        firewood::core
        firewood::db
        Qt6::Widgets
        Qt6::Sql
)

add_library(firewood::ui ALIAS ui)


