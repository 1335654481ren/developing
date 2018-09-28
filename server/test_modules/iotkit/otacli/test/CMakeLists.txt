include_directories(
        ${PROJECT_SOURCE_DIR}/include
)


set(SOURCE_FILES
        test.cpp 
        )

add_executable(unit_test ${SOURCE_FILES})

if (${PLATFORM_WIN})
     set(CMAKE_CXX_FLAGS_RELEASE "/MT")
     set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
     target_link_libraries(unit_test debug gtestd)
     target_link_libraries(unit_test optimized gtest)
endif()
if (${PLATFORM_ANDROID})
	target_link_libraries(unit_test  -Bstatic otacli hobotlog curl ssl crypto z log)
else()
     target_link_libraries(unit_test -Bstatic otacli hobotlog curl ssl crypto z pthread)
endif()
