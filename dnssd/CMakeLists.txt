add_library(dnssd
    dnssd_clientlib.c
    dnssd_clientstub.c
    dnssd_ipc.c
    $<$<C_COMPILER_ID:MSVC>:DebugServices.c>
    $<$<C_COMPILER_ID:MSVC>:GenLinkedList.c>
    $<$<C_COMPILER_ID:MSVC>:dllmain.c>
)

if(MSVC)
    target_compile_definitions(dnssd PUBLIC
        MDNS_DEBUGMSGS=0
        WIN32_LEAN_AND_MEAN
        USE_TCP_LOOPBACK
        _CRT_SECURE_NO_DEPRECATE
        _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1
        NOT_HAVE_SA_LEN
    )
    target_link_libraries(dnssd PUBLIC ws2_32 iphlpapi)
    set_property(TARGET dnssd PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(UNIX)
    target_compile_definitions(dnssd PUBLIC
        _GNU_SOURCE
        HAVE_IPV6
        NOT_HAVE_SA_LEN
        USES_NETLINK
        HAVE_LINUX
        TARGET_OS_LINUX
        PID_FILE="/var/run/mdnsd.pid"
        MDNS_UDS_SERVERPATH="/var/run/mdnsd"
        MDNS_DEBUGMSGS=0
    )
endif()
