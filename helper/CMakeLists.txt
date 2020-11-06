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
)

find_package(wxWidgets REQUIRED COMPONENTS base core adv)
include(${wxWidgets_USE_FILE})
add_executable(AnchorHelper WIN32 ${SRC})
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
elseif(UNIX)
    target_include_directories(AnchorHelper PRIVATE ../dnssd)
    target_compile_options(AnchorHelper PUBLIC -pthread)
endif()
