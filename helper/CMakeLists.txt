set(SRC
    HelperApp.cpp
    HelperFrame.cpp
    Discovery.cpp
    Socket.cpp
    SocketCompat.c
    Shutdown.cpp
    Connection.cpp
    crc16.cpp
    DB.cpp
    tools.cpp
)

find_package(wxWidgets REQUIRED COMPONENTS base core adv)
include(${wxWidgets_USE_FILE})
add_executable(AnchorHelper WIN32 MACOSX_BUNDLE ${CMAKE_SOURCE_DIR}/anchorhelper-mac.icns ${SRC})
# set_source_files_properties must be called in same file as add_executable
set_source_files_properties(${CMAKE_SOURCE_DIR}/anchorhelper-mac.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
target_link_libraries(AnchorHelper PRIVATE pthread dnssd ${wxWidgets_LIBRARIES})

if(MSVC)
    target_include_directories(AnchorHelper PRIVATE ../dnssd ../pthread)
    target_compile_definitions(AnchorHelper PRIVATE
    UNICODE
    _LIB
    WIN32_LEAN_AND_MEAN
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
    __MSVS2015__
    _WINSOCK_DEPRECATED_NO_WARNINGS
    )
    set_property(TARGET AnchorHelper PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(UNIX)
    target_include_directories(AnchorHelper PRIVATE ../dnssd)
    target_compile_options(AnchorHelper PUBLIC -pthread)
endif()
