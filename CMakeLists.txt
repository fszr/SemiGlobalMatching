cmake_minimum_required(VERSION 3.16)

project(SGM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

# 设置输出目录
set(OUTPUT_DIR ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(
        -Wall
        -Wextra
        -Wshadow
        -Werror=return-type
        -Wstrict-aliasing
        -Wuninitialized
    )
elseif(MSVC)
    add_compile_options(
        /W4
    )
endif()

# 寻找依赖库opencv
option(OpenCV_DIR "Path to OpenCV CMake directory" "")
if(${OpenCV_DIR} STREQUAL "" OR NOT EXISTS ${OpenCV_DIR})
    message(FATAL_ERROR "Please specify OpenCV CMake directory")
endif()

find_package(OpenCV REQUIRED)

# 是否使用openmp
option(OPENMP_ENABLED "Enable OpenMP library" ON)
set(OpenMP_LIBS "")
if(OPENMP_ENABLED)
    find_package(OpenMP)
    if(OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        add_compile_definitions(OPENMP_ENABLED)
        set(OpenMP_LIBS ${OpenMP_libomp_LIBRARY})
        message("-- OpenMP found. Enabling OpenMP support.")
    else()
        message("-- Can't find OpenMP. Continuing without it.")
    endif()
endif()


add_library(libtsgm SHARED
    SemiGlobalMatching/SemiGlobalMatching.cpp
    SemiGlobalMatching/SemiGlobalMatching.h
    SemiGlobalMatching/sgm_types.h
    SemiGlobalMatching/sgm_util.cpp
    SemiGlobalMatching/sgm_util.h
    SemiGlobalMatching/export.h
)

target_include_directories(libtsgm PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(libtsgm PUBLIC 
    ${OpenCV_LIBS}
    ${OpenMP_LIBS}
)
target_compile_definitions(libtsgm PRIVATE LIBTSGM_EXPORTS)

add_executable(SGM 
    SemiGlobalMatching/main.cpp
)
target_include_directories(SGM PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(SGM PUBLIC libtsgm)
add_dependencies(SGM libtsgm)

# 添加自定义 target 复制 opencv dll

add_custom_command(TARGET SGM POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_PROPERTY:opencv_world,IMPORTED_LOCATION_DEBUG>
    $<TARGET_PROPERTY:opencv_world,IMPORTED_LOCATION_RELEASE>
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
