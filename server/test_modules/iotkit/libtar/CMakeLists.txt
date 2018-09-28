cmake_minimum_required(VERSION 2.8)


include(cmake/hobot_util.cmake)
project(libtar)



message("config types: ${CMAKE_CONFIGURATION_TYPES}")
add_compile_options(-fPIC)
set(CMAKE_BUILD_TYPE Release)


include_directories(
        include
        lib
	libtar
	listhash
	.
	compat
)

set(SOURCE_FILES
        lib/append.c
	lib/block.c
	lib/decode.c
	lib/encode.c
	lib/extract.c
	lib/handle.c
	lib/output.c
	lib/util.c
	lib/wrapper.c
	listhash/libtar_hash.c
	listhash/libtar_list.c
	compat/basename.c
	compat/dirname.c
	compat/strlcpy.c
	compat/strmode.c
        )

#add_library(tar SHARED ${SOURCE_FILES})
add_library(tar STATIC ${SOURCE_FILES})
add_executable(libtar libtar/libtar.c)
target_link_libraries(libtar tar z)
# add_subdirectory(test)

set(OUTPUT_ROOT ${PROJECT_SOURCE_DIR}/output)

set(MY_OUTPUT_ROOT ${OUTPUT_ROOT}/)

install(TARGETS  tar
	DESTINATION ${MY_OUTPUT_ROOT}/lib)
install(FILES ${PROJECT_SOURCE_DIR}/lib/libtar.h
	DESTINATION ${MY_OUTPUT_ROOT}/include/)
install(FILES ${PROJECT_SOURCE_DIR}/listhash/libtar_listhash.h
	DESTINATION ${MY_OUTPUT_ROOT}/include/)

