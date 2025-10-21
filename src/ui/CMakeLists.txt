add_library(ui STATIC
    MainWindow.cpp
    MainWindow.h
    LoginDialog.cpp
    LoginDialog.h
    DashboardWidget.cpp
    DashboardWidget.h
    VolunteerProfileWidget.cpp
    VolunteerProfileWidget.h
    BookkeepingWidget.cpp
    BookkeepingWidget.h
    ExpenseDialog.cpp
    ExpenseDialog.h
    IncomeDialog.cpp
    IncomeDialog.h
    ClientDialog.cpp
    ClientDialog.h
    WorkOrderDialog.cpp
    WorkOrderDialog.h
    InventoryDialog.cpp
    InventoryDialog.h
    EquipmentMaintenanceDialog.cpp
    EquipmentMaintenanceDialog.h
    UserManagementDialog.cpp
    UserManagementDialog.h
    MyProfileDialog.cpp
    MyProfileDialog.h
    EmployeeDirectoryDialog.cpp
    EmployeeDirectoryDialog.h
    ProfileChangeRequestDialog.cpp
    ProfileChangeRequestDialog.h
    DeliveryLogDialog.cpp
    DeliveryLogDialog.h
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

