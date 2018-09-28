cmake_minimum_required(VERSION 2.8)

include(cmake/hobot_util.cmake)
project(otacli)
message(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0) 
add_definitions(-DJSONCPP_NO_LOCALE_SUPPORT)
add_definitions(-DLIBTARGZ)
add_compile_options(-Wall -Werror -v -fPIC -DSTATIC_LINKING=1 -DCUSTOM_MEMORY_MANAGEMENT=1)

include_directories(
	../libtar/output/include
        include
	include/otacli
        src
)

set(SOURCE_FILES
        #src/myapi.cpp
        src/jsoncpp.cpp
        src/ota_client.cpp
        src/http_client.cpp
        src/hobot_utils.cpp
        )

add_library(otacli SHARED ${SOURCE_FILES})
#add_library(otacli STATIC ${SOURCE_FILES})

find_library(LIBTAR libtar.a ${CMAKE_CURRENT_SOURCE_DIR}/../libtar/output/lib/)
IF(NOT LIBTAR)
	MESSAGE(FATAL_ERROR "build libtar first")
ENDIF(NOT LIBTAR) 

if (PLATFORM_ANDROID)
	target_link_libraries(otacli -Wl,-Bstatic libtar.a hobotlog curl ssl crypto z -Wl,-Bdynamic log dl)
	target_link_libraries(otacli ${LIBTAR})
else() 
	target_link_libraries(otacli -Wl,-Bstatic hobotlog curl ssl crypto -Wl,-Bdynamic z pthread dl)
	target_link_libraries(otacli ${LIBTAR})
endif()
add_subdirectory(test)
#add_subdirectory(jni)
set(OUTPUT_ROOT ${PROJECT_SOURCE_DIR}/output)

set(MY_OUTPUT_ROOT ${OUTPUT_ROOT}/otacli)
install(TARGETS otacli
    DESTINATION ${MY_OUTPUT_ROOT}/lib)
install(FILES ${PROJECT_SOURCE_DIR}/include/otacli/ota_client.h
    DESTINATION ${MY_OUTPUT_ROOT}/include/otacli/)
install(FILES ${PROJECT_SOURCE_DIR}/include/otacli/ota_typedef.h
    DESTINATION ${MY_OUTPUT_ROOT}/include/otacli/)
