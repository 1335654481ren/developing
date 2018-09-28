# Install script for directory: /home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/lib/libotacli.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/lib/libotacli.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/lib/libotacli.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/lib/libotacli.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/lib" TYPE SHARED_LIBRARY FILES "/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/built/lib/libotacli.so")
  if(EXISTS "$ENV{DESTDIR}/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/lib/libotacli.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/lib/libotacli.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/lib/libotacli.so"
         OLD_RPATH "/home/renxl/.gradle/caches/modules-2/files-2.1/third_party/openssl/1.0.3_linux_centos_gcc-4.8.5/files/openssl/lib:/home/renxl/.gradle/caches/modules-2/files-2.1/third_party/curl/7.53.1_linux_centos_gcc-4.8.5/files/curl/lib:/home/renxl/.gradle/caches/modules-2/files-2.1/com.hobot.native.hobot/hobotlog/1.0.4_linux_centos_gcc-4.8.5/files/hobotlog/lib:/home/renxl/.gradle/caches/modules-2/files-2.1/third_party.google.googletest/gtest/1.0.0_linux_centos_gcc-4.8.5/files/gtest/lib:/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/built/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/lib/libotacli.so")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/include/otacli/ota_client.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/include/otacli" TYPE FILE FILES "/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/include/otacli/ota_client.h")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/include/otacli/ota_typedef.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/output/otacli/include/otacli" TYPE FILE FILES "/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/include/otacli/ota_typedef.h")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/built/test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/built/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
