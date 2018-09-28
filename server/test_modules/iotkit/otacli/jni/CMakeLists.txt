cmake_minimum_required(VERSION 2.8)


project(otaclijni)


message("config types: ${CMAKE_CONFIGURATION_TYPES}")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0) 
add_definitions(-DJSONCPP_NO_LOCALE_SUPPORT)
add_compile_options(-Wall -Werror -fPIC -DSTATIC_LINKING=1 -DCUSTOM_MEMORY_MANAGEMENT=1)

include_directories(
	otacli/include/otacli/
	jni/
	/home/guoxing/tools/jdk1.8.0_131/include/
	/home/guoxing/tools/jdk1.8.0_131/include/linux/
)

set(SOURCE_FILES
        jni/com_hobot_otacli_OTACli.cpp
        jni/wrapper_otacli.cpp
   )
#link_directories("/home/guoxing/dev/c++/otacli/java/otacli/lib/")
add_library(otaclijni SHARED ${SOURCE_FILES})
if (PLATFORM_ANDROID)
	target_link_libraries(otaclijni -BStatic otaclid hobotlog curl ssl crypto z log)
else()
	target_link_libraries(otaclijni -Bstatic otaclid hobotlog curl ssl crypto z)
endif()
