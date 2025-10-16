add_executable(firewood_bank
    main.cpp
)

target_link_libraries(firewood_bank
    PRIVATE
        firewood::ui
        firewood::db
        firewood::core
        Qt6::Widgets
        Qt6::Core
)

# Platform-specific executable settings
if(WIN32)
    set_target_properties(firewood_bank PROPERTIES
        WIN32_EXECUTABLE ON
    )
elseif(APPLE)
    set_target_properties(firewood_bank PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.firewoodbank.app"
        MACOSX_BUNDLE_BUNDLE_NAME "Firewood Bank"
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    )
endif()


