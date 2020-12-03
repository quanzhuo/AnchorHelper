cmake_minimum_required(VERSION 3.18)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10" CACHE STRING "Minimum OS X deployment version" FORCE)
cmake_policy(SET CMP0091 NEW)
project(AnchorHelper
        VERSION 0.4
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD	11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD	99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# include GNUInstallDirs to set misc CACHE variables
include(GNUInstallDirs)

if(APPLE)
    set(CMAKE_INSTALL_RPATH @loader_path @loader_path/../Frameworks)
endif()

add_subdirectory(dnssd)
if(MSVC)
    add_subdirectory(pthread)
endif()
add_subdirectory(helper)

# set install prefix
if(NOT WIN32 AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CMAKE_INSTALL_PREFIX "/opt/zzdc/${PROJECT_NAME}")
endif()

if(NOT MSVC)
    install(TARGETS AnchorHelper
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION /usr/share/applications)

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        install(FILES anchorhelper.desktop
            DESTINATION share/applications/
        )
        install(FILES anchorhelper-linux.png
            TYPE DATA
        )
        install(FILES anchorhelper.desktop
            DESTINATION /usr/share/applications
        )
    elseif(APPLE)

    endif()

    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Set static ip or use DHCP for anchor")
    set(CPACK_PACKAGE_VENDOR "ZZDC")
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

    set(CPACK_RPM_PACKAGE_LICENSE "ZZDC Copyright")
    set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
    set(CPACK_RPM_PACKAGE_REQUIRES_PRE "wxGTK3, wxBase3")

    set_target_properties(AnchorHelper PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME ${CMAKE_PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
        MACOSX_BUNDLE_COPYRIGHT "ZZDC Copyright"
        MACOSX_BUNDLE_GUI_IDENTIFIER com.zzdc.anchorhelper
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION}
        MACOSX_BUNDLE_ICON_FILE anchorhelper-mac.icns
    )

    if(APPLE)
        set(CPACK_GENERATOR DragNDrop)

        # 本来下面注释掉的语句应该是通用的打包依赖库的方法，但是木前有bug，导致打包失败。因此注释掉，
        # 配合19至 21行硬编码的设置来打包。本来应该没有19-21行，只使用下面的语句来打包的。
        #
        # Note Mac specific extension .app
        #set(APPS "\${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}.app")
        # Directories to look for dependencies
        #set(DIRS ${CMAKE_BINARY_DIR})
        #install(CODE "include(BundleUtilities)
        #    fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")")

    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(CPACK_GENERATOR RPM)
        set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/zzdc/${PROJECT_NAME}")
    endif()

    include(CPack)
endif()
